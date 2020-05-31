/*******************************************************************************                                     
**                 STM8S                     74HC595		 LCD
**                 Master         /|\         Slave	        16x2
**             -----------------   |       -----------------  |---------|  
**           -|XIN          RST |--|	  |            7/Q7 |-|D7       |
**            |                 |         |            6/Q6 |-|D6       |
**           -|XOUT        P1.0 |<------->|11/SH_CP    5/Q5 |-|D5       |
**            |     	   P1.1 |<------->|14/DS       4/Q4 |-|D4       |
**            |     	   P1.2 |<------->|12/ST_CP         | |         |
**            |                 |         |            3/Q3 |-|E        |
**            |                 |         |            2/Q2 |-|RS       |
**            |                 |         |                 | |         |
**             -----------------           -----------------  |---------|
*******************************************************************************/
#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_
/******************************************************************************/
#include "stm8s.h"
/******************************************************************************/
#define USE_74HC595_DRIVER
/******************************************************************************/
#ifdef USE_74HC595_DRIVER
#define HC595_PORT              GPIOD
#define HC595_DATA_PIN          GPIO_PIN_3
#define HC595_CLOCK_PIN         GPIO_PIN_1
#define HC595_LATCH_PIN         GPIO_PIN_2

#define HC595_DATA_LOW          GPIO_WriteLow(HC595_PORT, HC595_DATA_PIN)
#define HC595_DATA_HIGH         GPIO_WriteHigh(HC595_PORT, HC595_DATA_PIN)
#define HC595_CLOCK_LOW         GPIO_WriteLow(HC595_PORT, HC595_CLOCK_PIN)
#define HC595_CLOCK_HIGH        GPIO_WriteHigh(HC595_PORT, HC595_CLOCK_PIN)
#define HC595_LATCH_LOW         GPIO_WriteLow(HC595_PORT, HC595_LATCH_PIN)
#define HC595_LATCH_HIGH        GPIO_WriteHigh(HC595_PORT, HC595_LATCH_PIN)

#define LCD_EN_MASK             0x08
#define LCD_RS_MASK             0x0C

#define HC595_INIT_PINS         GPIO_Init(HC595_PORT, (GPIO_Pin_TypeDef)\
                                (HC595_DATA_PIN|HC595_CLOCK_PIN|HC595_LATCH_PIN),\
                                GPIO_MODE_OUT_PP_LOW_FAST)
#endif
/*******************************************************************************
*******************************************************************************/
void LCD_Cmd(uint8_t cmd);
void LCD_Putc(uint8_t data);
void LCD_PutString(uint8_t *s);
void LCD_GotoXy(uint8_t x, uint8_t y);
void LCD_Clear(void);
void LCD_Init(void);
void LCD_Number(uint32_t val, uint8_t dec, uint8_t neg);
void LCD_2Line(uint8_t *s1 , uint8_t *s2);
void LCD_2LineShiftToRight(uint8_t *s1, uint8_t *s2);
void LCD_IntToStr(uint8_t i, uint8_t *s);
#endif
/*******************************************************************************
*************************** END OF FILE ****************************************
*******************************************************************************/