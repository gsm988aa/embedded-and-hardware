/*******************************************************************************
                       Copyright by Pham Van Lap KMT
********************************************************************************
** File Name        : LCD_Driver.c
** Version          : 1.0
** Author           : Pham Van Lap
** Create Date      : 10/12/2018
** Company          : FPT Software
** Email            : quoclaptk@gmail.com
** Phone Numbers    : 0886560383 or 0963836272
*******************************************************************************/

/*******************************************************************************
                             Revision History
********************************************************************************
**    Date              Author                           Detail
** ----------       --------------      ----------------------------------------
** 10/12/2018        Pham Van Lap                      Create new
*******************************************************************************/
/*******************************************************************************                                     
**              STM8S                     74HC595		 LCD
**              Master         /|\         Slave	        16x2
**         |-----------------|  |      |-----------------| |---------|  
**        -|XIN          RST |--|      |            7/Q7 |-|D7       |
**         |                 |         |            6/Q6 |-|D6       |
**        -|XOUT        P1.0 |<------->|11/SH_CP    5/Q5 |-|D5       |
**         |     	P1.1 |<------->|14/DS       4/Q4 |-|D4       |
**         |     	P1.2 |<------->|12/ST_CP         | |         |
**         |                 |         |            3/Q3 |-|E        |
**         |                 |         |            2/Q2 |-|RS       |
**         |                 |         |                 | |         |
**         |-----------------|         |-----------------| |---------|
*******************************************************************************/

/*******************************************************************************
                          Include Files
*******************************************************************************/
#include "LCD16X2.h"
/*******************************************************************************
                      void HC595_Write(uint8_t data)
*******************************************************************************/
#ifdef USE_74HC595_DRIVER
void HC595_Write(uint8_t data)
{
    for(uint8_t i=0; i<8; i++)
    {
        if(((data<<i)&0x80)==0x80)HC595_DATA_HIGH;
        else HC595_DATA_LOW;
        HC595_CLOCK_LOW;
        HC595_CLOCK_HIGH;
    }
    HC595_LATCH_LOW;
    HC595_LATCH_HIGH;
}
#endif
/*******************************************************************************
                      void LCD_Cmd(uint8_t cmd)
*******************************************************************************/
void LCD_Cmd(uint8_t cmd)
{
    #ifdef USE_74HC595_DRIVER
    HC595_Write(LCD_EN_MASK);
    HC595_Write((cmd&0xF0)|LCD_EN_MASK);
    HC595_Write(((cmd&0xF0)|LCD_EN_MASK)&0xF0);
  
    HC595_Write(LCD_EN_MASK);
    HC595_Write((cmd<<4)|LCD_EN_MASK);
    HC595_Write(((cmd<<4)|LCD_EN_MASK)&0xF0);
    #endif
}
/*******************************************************************************
                      void LCD_Putc(uint8_t data)
********************************************************************************
** Put a writable character on LCD
** If the character is '\f', clear the LCD
** If the character is '\n', move cursor to the second line
*******************************************************************************/
void LCD_Putc(uint8_t data)
{
    #ifdef USE_74HC595_DRIVER
    //send 4 bit LOW
    HC595_Write(LCD_RS_MASK);
    HC595_Write((data&0xF0)|LCD_RS_MASK);
    HC595_Write(((data&0xF0)|LCD_RS_MASK)&0xF4);
    //Send 4 bit HIGH
    HC595_Write(LCD_RS_MASK);
    HC595_Write((data<<4)|LCD_RS_MASK);
    HC595_Write(((data<<4)|LCD_RS_MASK)&0xF4);
    #endif
}
/*******************************************************************************
                      void LCD_PutString(uint8_t *s)
********************************************************************************
** Put a string s[] on LCD.
** please note that the string is ended with '/0' (or 0x00)
*******************************************************************************/
void LCD_PutString(uint8_t *s)
{
    while(*s!='\0')
    {
        LCD_Putc(*s++);
    }
}
/*******************************************************************************
                      void LCD_GotoXy(uint8_t xPos, uint8_t yPos)
********************************************************************************
** Move the pointer of LCD to coordinate (column, row)
** Note that cursor is at (0,0) after reset of clear LCD
*******************************************************************************/ 
void LCD_GotoXy(uint8_t xPos, uint8_t yPos)
{
    if(yPos==0)LCD_Cmd(0x80 + (xPos%16));
    else LCD_Cmd(0xC0 + (xPos%16));
}
/*******************************************************************************
                      void LCD_Clear(void)
*******************************************************************************/
void LCD_Clear(void)
{
    LCD_Cmd(0x01);
    delay_us(200);
}
/*******************************************************************************
                      void LCD_Init(void)
********************************************************************************
** Initialization for LCD module
** If you like to re-initialization: Please refer to LCD's document at the
**            {repository}\Documents\LCD16x2Commands.pdf
*******************************************************************************/
void LCD_Init(void)
{
    #ifdef USE_74HC595_DRIVER
    HC595_INIT_PINS;
    HC595_Write(0x00);
    delay_us(250);
    HC595_Write(0x08);
    LCD_Cmd(0x02);
    delay_us(20);
    LCD_Cmd(0x28);
    LCD_Cmd(0x0C);
    LCD_Clear(); 
    LCD_Cmd(0x06);
    LCD_Cmd(0x80);
    #endif
}
/*******************************************************************************
                      void LCD_Number(uint32_t value, uint8_t dec, uint8_t neg)
********************************************************************************
** Put a constant or a number stored in 'value' on LCD
** The number must be multiplied and convert in type "unsigned long"
**                                                or "unsigned int"
**   dec: amount of digits after '.'
**   neg: sign of the number, neg = 0 if the number is positive
**
** Example 1: lcd_number(value, 3, 0); // value = 12345
**        will put "12.345" on the LCD
** Example 2: lcd_number(value, 6, 1); // value = 12345
**        will put "-0.012345" on the LCD
**
** Note: This function may contain some bugs. If you can detect and fix them
**       , please contact me :d
*******************************************************************************/
void LCD_Number(uint32_t value, uint8_t dec, uint8_t neg)
{
    uint8_t i, j, digit;
    uint32_t temp;
    
    if(neg != 0)LCD_Putc('-');
    if((value - dec) <= 0)LCD_Putc('0');// if value < dec put 0.xxx
    else
    {      
        for (i=0; i<(value-dec); i++)
        {
            temp = 1;
            for(j=1; j<(value-i); j++)
            temp *=10;
            digit = (value/temp)%10;
            LCD_Putc(digit + 0x30);
        }
    }
    if(dec > 0)
    {
        LCD_Putc('.');
        if((dec - value)> 0)	// ex: val = 55; dec = 3 put 0.055
        {
            for (i = 0; i<(dec-value); i++)
            LCD_Putc('0');
        }
        else
        {
            for(i=(value - dec); i< value; i++)
            {
                temp = 1;
                for(j=1; j<(value-i); j++)temp *= 10;
                digit = (value/temp)%10;
                LCD_Putc(digit + 0x30);
            }
        }
    }    
}
/*******************************************************************************
                      void LCD_2Line(uint8_t *s1 , uint8_t *s2)
********************************************************************************
** Clear LCD. Put string s1[] on line 1 and string s2[] on line 2
*******************************************************************************/ 
void LCD_2Line(uint8_t *s1 , uint8_t *s2)
{
    LCD_Clear();
    LCD_PutString(s1);
    LCD_GotoXy(0, 1);
    LCD_PutString(s2);
}
/*******************************************************************************
                      void LCD_2LineShiftToRight(uint8_t *s1, uint8_t *s2)
********************************************************************************
** Put 2 string s1 and s2 on LCD and shift all of them to right 
*******************************************************************************/
void LCD_2LineShiftToRight(uint8_t *s1, uint8_t *s2)
{
    LCD_2Line(s1, s2);
    delay_us(100);
    for(uint8_t i=0; i<15; i++)
    {
        LCD_Cmd(0x1C);
        delay_us(150);
    }
}
/*******************************************************************************
                      void LCD_IntToStr(uint8_t i, uint8_t *s)
********************************************************************************
* Convert a Interger number to String. Ex: 123 -> "123"
*******************************************************************************/
void LCD_IntToStr(uint8_t i, uint8_t *s)
{
    uint8_t *p;
    p = s;
    p[2] = i % 10;
    i -= p[2];
    i /= 10;
    p[1] = i % 10;
    i -= p[1];
    i /= 10;
    p[0] = i%10;
    p[3] = 0; // mark end of string
    p[2] += 0x30;
    p[1] += 0x30;
    p[0] += 0x30;
}
/*******************************************************************************
******************************** END OF FILE ***********************************
*******************************************************************************/