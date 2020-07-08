/*******************************************************************************
***  File Name    : main.c
***  Project name : STM8S_Matrix_Clock_V2.0
***  Author       : Pham Van Lap KMT
***  Company      : FPT Software
***  Create Date  : 20/01/2017
***  Device Use   : STM8S105K4T6C
*******************************************************************************/
#include "nRF24L01.h"
/******************************************************************************/
uint8_t TxpipeAddrs[5] = {0x11, 0x22, 0x33, 0x44, 0xAA};
char myTxData[32] = "Hello World!";
char AckPayload[32];
#define txMsg "Transmitted Successfully\r\n"
/******************************************************************************/
static void CLK_Config(void);
static void GPIO_Config(void);
/******************************************************************************/
void main(void)
{
    CLK_Config();
    GPIO_Config();
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER,\
				SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, \
				SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
    SPI_Cmd(ENABLE);
    NRF24_begin(GPIOB, GPIO_PIN_1, GPIO_PIN_0);
    NRF24_stopListening();
    NRF24_setAutoAck(true);
    NRF24_enableDynamicPayloads();
    NRF24_enableAckPayload();
    NRF24_openWritingPipe(TxpipeAddrs);
    NRF24_setPALevel(RF24_PA_MIN);
    NRF24_stopListening();
    while(1)
    {
        if(NRF24_write((uint8_t*)myTxData, 32))
        {
            //NRF24_read((uint8_t*)AckPayload, 32);
            ////HAL_UART_Transmit(&huart1, (uint8_t *)txMsg, strlen(txMsg), 10);
            //UART_SendString((uint8_t *)txMsg);
            //char myDataack[80];
            //sprintf(myDataack, "AckPayload:  %s \r\n", AckPayload);
            //UART_SendString((uint8_t *)myDataack);
          delay_ms(10);
        }
        delay_ms(100);
    }
}
/******************************************************************************/
static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);
}
/******************************************************************************/
static void GPIO_Config(void)
{
    //GPIO_DeInit(GPIOE);        
    //GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    //GPIO_DeInit(GPIOC);        
    //GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix      
    //GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix        
    //GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix
    //GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_IN_FL_IT); 
    //EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE, EXTI_SENSITIVITY_FALL_ONLY);
    GPIO_DeInit(GPIOB);
    GPIO_Init(GPIOB, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
}
/******************************************************************************/
//static void TIM4_Config()
//{
//    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 200);
//    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
//    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
//    TIM4_Cmd(ENABLE);
//}
/******************************************************************************/