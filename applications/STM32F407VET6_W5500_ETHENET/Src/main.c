/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>

#include "w5500.h"
#include "dhcp.h"
#include "dns.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "net_config.h"
#include "W5500_driver.h"
TIM_HandleTypeDef htim2;
void SystemClock_Config(void);
// SOCKET NUMBER DEFINION for Examples
#define SOCK_TCPS       0
#define SOCK_UDPS       1
#define SOCK_DHCP		7
// Receive Buffer
#define BUFFER_SIZE	2048
uint8_t tempBuffer[BUFFER_SIZE];
// Global variables
uint32_t dhcp_counter;
uint8_t mqtt_push_counter;
uint8_t mqtt_flag;
uint16_t mes_id;

wiz_NetInfo gWIZNETINFO =
{
	.mac = {ETHADDR0, ETHADDR1, ETHADDR2, ETHADDR3, ETHADDR4, ETHADDR5},
	.ip = {IPADDR0, IPADDR1, IPADDR2, IPADDR3},
	.sn = {NETMASK0, NETMASK1, NETMASK2, NETMASK3},
	.gw = {DRIPADDR0, DRIPADDR1, DRIPADDR2, DRIPADDR3},
	.dns = {DRIPADDR0, DRIPADDR1, DRIPADDR2, DRIPADDR3},
	.dhcp = NETINFO_DHCP  // NETINFO_STATIC
};

void msTick_Handler(void)
{
	MilliTimer_Handler();
	if (++dhcp_counter >= 1000)
	{
		dhcp_counter = 0;
		DHCP_time_handler();
		if (++mqtt_push_counter >= 10)
		{
			mqtt_push_counter = 0;
			mqtt_flag = 1;
		}
	}
}

void print_network_information(void)
{
	wizchip_getnetinfo(&gWIZNETINFO);
	printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
	if(gWIZNETINFO.dhcp == NETINFO_DHCP)printf("DHCP\n\r");
	else printf("Static IP\n\r");
	printf("IP address : %d.%d.%d.%d\n\r", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	printf("SM Mask    : %d.%d.%d.%d\n\r", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	printf("Gate way   : %d.%d.%d.%d\n\r", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
	printf("DNS Server : %d.%d.%d.%d\n\r", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
}

void messageArrived(MessageData* md)
{
	MQTTMessage* message = md->message;
	for (uint8_t i = 0; i < md->topicName->lenstring.len; i++)
	{
		putchar(*(md->topicName->lenstring.data + i));
	}
	//printf(" (%.*s)\r\n", (int32_t)message->payloadlen, (char*)message->payload);
}

uint8_t DHCP_proc(void)
{
	uint8_t dhcp_res = DHCP_run();
	switch (dhcp_res)
	{
		case DHCP_IP_ASSIGN:
		case DHCP_IP_CHANGED:
		case DHCP_IP_LEASED:
			getIPfromDHCP(gWIZNETINFO.ip);
			getGWfromDHCP(gWIZNETINFO.gw);
			getSNfromDHCP(gWIZNETINFO.sn);
			getDNSfromDHCP(gWIZNETINFO.dns);
			gWIZNETINFO.dhcp = NETINFO_DHCP;
			ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
		#ifdef _DHCP_DEBUG_
			printf("\r\n>> DHCP IP Leased Time : %ld Sec\r\n", getDHCPLeasetime());
		#endif
		break;
		case DHCP_FAILED:
		#ifdef _DHCP_DEBUG_
			printf(">> DHCP Failed\r\n");
		#endif
		gWIZNETINFO.dhcp = NETINFO_STATIC;
		break;
	}
	return dhcp_res;
}

int8_t str_printf(char *StrBuff, uint8_t BuffLen, const char *args, ...)
{
	va_list ap;
	va_start(ap, args);
	int8_t len = vsnprintf(StrBuff, BuffLen, args, ap);
	va_end(ap);
	return len;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	msTick_Handler();
}

int main(void)
{
	int32_t rc = 0;
	uint8_t dhcp_ret = DHCP_STOPPED;
	uint8_t buf[100];
	Network n;
	Client c;

	HAL_Init();
	SystemClock_Config();
	W5500_Init(SPI1);
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	W5500_chipInit();
	/*Set network informations*/
	wizchip_setnetinfo(&gWIZNETINFO);
	setSHAR(gWIZNETINFO.mac);
	if (gWIZNETINFO.dhcp == NETINFO_DHCP)
	{
		DHCP_init(SOCK_DHCP, tempBuffer);
		while (!((dhcp_ret == DHCP_IP_ASSIGN) || (dhcp_ret == DHCP_IP_CHANGED) || (dhcp_ret == DHCP_FAILED) || (dhcp_ret == DHCP_IP_LEASED)))
		{
			dhcp_ret = DHCP_proc();
		}
	}
	print_network_information();
	n.my_socket = 0;
	uint8_t targetIP[4] = {192, 168, 1, 35};
	NewNetwork(&n);
	ConnectNetwork(&n, targetIP, 1883);
	MQTTClient(&c, &n, 1000, buf, 100, tempBuffer, BUFFER_SIZE);
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 0;
	data.MQTTVersion = 4;//3;
	data.clientID.cstring = (char*)"w5500-client";
	data.username.cstring = "username";
	data.password.cstring = "";
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	rc = MQTTConnect(&c, &data);
	printf("Connected %ld\r\n", rc);
	char SubString[] = "/#";
	rc = MQTTSubscribe(&c, SubString, QOS0, messageArrived);
	printf("Subscribed (%s) %ld\r\n", SubString, rc);
	while (1)
	{
		if (gWIZNETINFO.dhcp == NETINFO_DHCP)dhcp_ret = DHCP_proc();
		if (mqtt_flag)
		{
			mqtt_flag = 0;
			char message[16];
			int8_t len = str_printf(message, sizeof(message), "%d.%d.%d.%d", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
			if (len > 0)
			{
				MQTTMessage pubMessage;
				pubMessage.qos = QOS0;
				pubMessage.id = mes_id++;
				pubMessage.payloadlen = len;
				pubMessage.payload = message;
				MQTTPublish(&c, "/w5500_stm32_client", &pubMessage);
			}
		}
		MQTTYield(&c, 1000);
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
	  while(1);
  }
}
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 40000 -1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
