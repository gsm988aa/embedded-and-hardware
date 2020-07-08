#ifndef _P10_DRIVER_H_
#define _P10_DRIVER_H_
/********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
/*********************************************************************************************
**                           _____
**                     PWM__|     \
**                     OE___| AND  }---- Disp_Enable 
**                          |_____/      
**                    
**                      P10 Led Board
**                          1     2
**                     OE  --|oo|-- A
**                     GND --|oo|-- B
**                     GND --|oo|-- 
**                     GND --|oo|-- SCK
**                     GND --|oo|-- LATCH
**                     GND --|oo|-- DATA
**                     GND --|oo|--
**                     GND --|oo|--
**                         15    16 
*********************************************************************************************/
#define Font1H          8
#define Font2H          12
#define Font3H          14
#define Font4H          16
#define Panel           1
#define RIGHT           0
#define LEFT            1
#define WIDTH           31
#define HEIGHT          15
/********************************************************************************************/
#define GPIO_P10_PIN_A              GPIO_PIN_8
#define GPIO_P10_PIN_B              GPIO_PIN_7
#define GPIO_P10_PIN_OE             GPIO_PIN_9
#define GPIO_P10_PIN_LATCH          GPIO_PIN_2
#define GPIO_P10_PIN_DATA           GPIO_PIN_3
#define GPIO_P10_PIN_CLOCK          GPIO_PIN_10

#define GPIO_P10_PIN_A_LOW          HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_A, GPIO_PIN_RESET)
#define GPIO_P10_PIN_A_HIG          HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_A, GPIO_PIN_SET)

#define GPIO_P10_PIN_B_LOW          HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_B, GPIO_PIN_RESET)
#define GPIO_P10_PIN_B_HIG          HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_B, GPIO_PIN_SET)

#define GPIO_P10_PIN_OE_LOW         HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_OE, GPIO_PIN_RESET)
#define GPIO_P10_PIN_OE_HIG         HAL_GPIO_WritePin(GPIOE, GPIO_P10_PIN_OE, GPIO_PIN_SET)

#define GPIO_P10_PIN_LATCH_LOW      HAL_GPIO_WritePin(GPIOC, GPIO_P10_PIN_LATCH, GPIO_PIN_RESET)
#define GPIO_P10_PIN_LATCH_HIG      HAL_GPIO_WritePin(GPIOC, GPIO_P10_PIN_LATCH, GPIO_PIN_SET)

#define GPIO_P10_PIN_DATA_LOW       HAL_GPIO_WritePin(GPIOC, GPIO_P10_PIN_DATA, GPIO_PIN_RESET)
#define GPIO_P10_PIN_DATA_HIG       HAL_GPIO_WritePin(GPIOC, GPIO_P10_PIN_DATA, GPIO_PIN_SET)

#define GPIO_P10_PIN_CLOCK_LOW      HAL_GPIO_WritePin(GPIOB, GPIO_P10_PIN_CLOCK, GPIO_PIN_RESET)
#define GPIO_P10_PIN_CLOCK_HIG      HAL_GPIO_WritePin(GPIOB, GPIO_P10_PIN_CLOCK, GPIO_PIN_SET)
/********************************************************************************************/
void ScanMatrix(void);
uint8_t LineScroll(uint8_t Line, uint8_t Zerobit);
void clear_display(uint8_t fill);
void invert_screen(void);
void Set_Brightness(uint8_t value);
void PutPixel(uint8_t Xpos, uint8_t Ypos, uint8_t fill);
uint8_t GetPixel(uint8_t Xpos, uint8_t Ypos);
void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill);
void Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t fill);
void Circle(uint8_t x, uint8_t y, uint8_t size, uint8_t fill);
void Bargraph(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t hor, uint8_t value);
void ShowString(uint8_t x, uint8_t y, uint8_t size, uint8_t fill);
void TextToLeftScroll(uint8_t Line, uint8_t size, uint8_t speed, uint8_t fill);
void Writeimage(void);
#endif
