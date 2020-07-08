/*******************************************************************************
** File Name:  matrix_driver.h
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#ifndef _MATRIX_DRIVER_H_
#define _MATRIX_DRIVER_H_
#include "stm8s.h"
/******************************************************************************/
#define Matrix_8x48
//#define MatrixP10
#define HC595_DATA                  PC_ODR_ODR7
#define HC595_CLOCK                 PC_ODR_ODR5
#define HC595_LATCH                 PC_ODR_ODR3

#define SHIFT_UP     1
#define SHIFT_DOWN   0
/******************************************************************************/
extern void delay_us(uint32_t nCount);
void HC595_Write(uint8_t value);
void out_data();
void write_data(uint8_t colum_addr);
void Coppy_Data_To_Ram(uint8_t Line, uint8_t fill);
void ColumScroll(uint8_t colum, uint8_t value);
void ScrollTextFont (uint8_t Line, uint32_t speed, uint8_t fill);
void Clear_Display_Ram(uint8_t fill);
void number_time_effect(uint8_t position, uint8_t value, uint8_t step, uint8_t pull, uint8_t Line);
#endif
/*******************************************************************************
****************************** END OF FILE *************************************
*******************************************************************************/