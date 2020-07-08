/*******************************************************************************
***  File Name    : main.h
***  Project name : STM8S_Matrix_Clock_V2.0
***  Author       : Pham Van Lap KMT
***  Company      : FPT Software
***  Create Date  : 20/01/2017
***  Device Use   : STM8S105K4T6C
*******************************************************************************/
#ifndef __MAIN_H__
#define __MAIN_H__
#include "stm8s.h"
#include "stdio.h"
/******************************************************************************/
#define INFR_DATA    GPIO_ReadInputPin(GPIOB, GPIO_PIN_1)
/******************************************************************************/

/******************************************************************************/
static void CLK_Config(void);
static void GPIO_Config(void);
static void TIM4_Config();
static void INFR_Config();
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
