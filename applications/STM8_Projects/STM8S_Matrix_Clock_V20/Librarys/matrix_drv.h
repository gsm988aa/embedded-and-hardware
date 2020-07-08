/**
  ******************************************************************************
  * @File     matrix_drv.h
  * @Author   Pham Van Lap KMT
  * @Version  V1.0
  * @Date     16/01/2017
  * @Brief    This file is used to configure the "matrix_drv.c".
  ******************************************************************************
**/
#ifndef _MATRIX_DRV_H_
#define _MATRIX_DRV_H_
/******************************************************************************/
#include "stm8s.h"
#include "DELAY.h"
/******************************************************************************/
#define Matrix_8x48
//#define MatrixP10
/******************************************************************************/
#define HC595_PORT                  GPIOC
#define HC595_DATA_PIN              GPIO_PIN_7
#define HC595_CLOCK_PIN             GPIO_PIN_5
#define HC595_LATCH_PIN             GPIO_PIN_3

#define HC595_DATA_LOW              GPIO_WriteLow(HC595_PORT, HC595_DATA_PIN)
#define HC595_DATA_HIGH             GPIO_WriteHigh(HC595_PORT, HC595_DATA_PIN)
#define HC595_CLOCK_LOW             GPIO_WriteLow(HC595_PORT, HC595_CLOCK_PIN)
#define HC595_CLOCK_HIGH            GPIO_WriteHigh(HC595_PORT, HC595_CLOCK_PIN)
#define HC595_LATCH_LOW             GPIO_WriteLow(HC595_PORT, HC595_LATCH_PIN)
#define HC595_LATCH_HIGH            GPIO_WriteHigh(HC595_PORT, HC595_LATCH_PIN)
/******************************************************************************/
void HC595_Write(uint8_t value);
void out_data();
void write_data(uint8_t colum_addr);
void Coppy_Data_To_Ram(uint8_t Line, uint8_t fill);
void ColumScroll(uint8_t colum, uint8_t value);
void ScrollTextFont(uint32_t Line, uint32_t speed, uint8_t fill);
void Clear_Display_Ram(uint8_t fill);
void number_time_effect(uint8_t position, uint8_t value, uint8_t step, uint8_t pull, uint8_t Line);
#endif
/*******************************************************************************
****************************** END OF FILE *************************************
*******************************************************************************/