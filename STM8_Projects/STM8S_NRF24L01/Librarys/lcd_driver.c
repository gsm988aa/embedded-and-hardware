/*******************************************************************************
** File Name   : lcd_driver.c
** Description : Source file for LCD Display
** Author      : Pham Van Lap
** Create Date : 23/03/2018
*******************************************************************************/
#include "lcd_driver.h"
/******************************************************************************/
void LCD_Delay(unsigned short time)
{
    while(time--)
    {
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
        __no_operation();
    }
}
/******************************************************************************/
void LCD_Enable()
{
    LCD_EN = 1;
    LCD_Delay(36);
    LCD_EN = 0;
    LCD_Delay(400);
}
/******************************************************************************/
void LCD_Send4Bit(unsigned char data)
{
    LCD_D4 = data & 0x01;
    LCD_D5 = (data>>1)&1;
    LCD_D6 = (data>>2)&1;
    LCD_D7 = (data>>3)&1;
}
/******************************************************************************/
void LCD_PutCmd(unsigned char cmd)
{
    LCD_Send4Bit(cmd>>4);/* Gui 4 bit cao */
    LCD_Enable();
    LCD_Send4Bit(cmd); /* Gui 4 bit thap*/
    LCD_Enable();
}
/******************************************************************************/
void LCD_Init()
{
    LCD_Send4Bit(0x00);
    LCD_Delay(1000);
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_Send4Bit(0x03);
    LCD_Enable();
    LCD_Delay(2000);
    LCD_Enable();
    LCD_Delay(800);
    LCD_Enable();
    LCD_Send4Bit(0x02);
    LCD_Enable();
    LCD_PutCmd(0x28); // giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
    LCD_PutCmd(0x0c); // cho phep hien thi man hinh
    LCD_PutCmd(0x06); // tang ID, khong dich khung hinh
    LCD_PutCmd(0x01); // xoa toan bo khung hinh
}
/******************************************************************************/
void LCD_Gotoxy(unsigned char x, unsigned char y)
{
   unsigned char address;
   if(!y)address = (0x80+x);
   else address = (0xc0+x);
   LCD_Delay(8);
   LCD_PutCmd(address);
   LCD_Delay(400);
}
/******************************************************************************/
void LCD_PutChar(char data)
{
    LCD_RS = 1;
    LCD_PutCmd(data);
    LCD_RS = 0 ;
}
/******************************************************************************/
void LCD_PutChars(char *s)
{
    while(*s!='\0')
    {
        LCD_PutChar(*s);
        s++;
    }
}
/******************************************************************************
********************************* END OF FILE *********************************
******************************************************************************/