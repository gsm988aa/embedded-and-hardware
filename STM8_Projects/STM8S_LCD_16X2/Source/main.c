/*******************************************************************************
** File Name   : main.c
** Description : Source file for Project
** Author      : Pham Van Lap
** Create Date : 23/03/2018
*******************************************************************************/
#include "lcd_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
/******************************************************************************/
void EXT_Clock_Init()
{
    CLK_CKDIVR = 0x00;
    CLK_ECKR = 0x01;              // Enable HSE
    while(!(CLK_ECKR & 0x01));    // Wait HSE Ready
    CLK_SWCR_SWEN = 1;
    CLK_SWR = 0xB4;
    while(CLK_CMSR != 0xB4);
    while(!CLK_SWCR_SWIF);
    CLK_SWCR_SWIF = 0;
}
/******************************************************************************/
void delay_us(unsigned long time)
{
    while(time--);
}
/******************************************************************************/
void Port_Init()
{
    PB_DDR = 0xff;
    PB_CR1 = 0xff;
    PB_CR2 = 0xff;
    PF_DDR = 0xff;
    PF_CR1 = 0xff;
    PF_CR2 = 0xff;
}
void ADC_Init()
{
  ADC_CSR = 2;        // Channel AIN2
  ADC_CR2 = 0x08;     // Canh ch?nh ph?i
  ADC_CR1 = 0x01;     // Cho phép ADC
}
unsigned int Get_ADC()
{
    unsigned int result;
    ADC_CR1 = 0x01;  // B?t d?u Convert
    while(!(ADC_CSR & 0x80));   // ch? c? EOC
    result = ADC_DRL;      // L?y 8 bit th?p
    result |= ADC_DRH<<8;  //l?y 8 bit cao
    ADC_CSR &= 0x7F;  // Xóa c?
    return result;
}
/******************************************************************************/
void main()
{
    CLK_CKDIVR = 0x00; // CPU run max speed (16MHz IRC)
    //EXT_Clock_Init();
    Port_Init();
    LCD_Init();
    ADC_Init();
    char buff[6] = "";
    while(1)
    {
        LCD_Gotoxy(0,0);
        delay_us(50000);
        sprintf(buff, "%d", Get_ADC());
        LCD_PutChars(buff);
        LCD_Gotoxy(0,1);
        LCD_PutChars("FPT Software");
    }
}
/******************************************************************************
********************************* END OF FILE *********************************
******************************************************************************/