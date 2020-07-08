/*******************************************************************************
** File Name   : lcd_driver.h
** Description : Header file for LCD Display
** Author      : Pham Van Lap
** Create Date : 23/03/2018
*******************************************************************************/
#include "iostm8s208mb.h"
#include "intrinsics.h"
/******************************************************************************/
#define  LCD_RS         PF_ODR_ODR0
#define  LCD_RW         PF_ODR_ODR3
#define  LCD_EN         PF_ODR_ODR4

#define  LCD_D4         PB_ODR_ODR4
#define  LCD_D5         PB_ODR_ODR5
#define  LCD_D6         PB_ODR_ODR6
#define  LCD_D7         PB_ODR_ODR7
/******************************************************************************/
void LCD_PutCmd(unsigned char cmd);
void LCD_Init();
void LCD_Gotoxy(unsigned char x, unsigned char y);
void LCD_PutChar(char data);
void LCD_PutChars(char *s);
/******************************************************************************
****************************** END OF FILE ************************************
******************************************************************************/