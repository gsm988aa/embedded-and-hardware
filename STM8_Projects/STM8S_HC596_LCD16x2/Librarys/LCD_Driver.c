/*******************************************************************************                                     
**                 STM8S                     74HC595		 LCD
**                 Master         /|\         Slave	        16x2
**             -----------------   |      |-----------------| |---------|  
**           -|XIN          RST |--|	  |            7/Q7 |-|D7       |
**            |                 |         |            6/Q6 |-|D6       |
**           -|XOUT        P1.0 |<------->|11/SH_CP    5/Q5 |-|D5       |
**            |     	   P1.1 |<------->|14/DS       4/Q4 |-|D4       |
**            |     	   P1.2 |<------->|12/ST_CP         | |         |
**            |                 |         |            3/Q3 |-|E        |
**            |                 |         |            2/Q2 |-|RS       |
**            |                 |         |                 | |         |
**             -----------------          |-----------------| |---------|
*******************************************************************************/
#include "LCD_Driver.h"
#include "DELAY.h"
/******************************************************************************/
#ifdef USE_74HC595_DRIVER
/*******************************************************************************
** Send a byte DATA to 74HC595
*******************************************************************************/
void HC595_Write(uint8_t data)
{
    uint8_t i=0;
    for(i=0; i<8; i++)
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
** Send a byte of command to LCD
*******************************************************************************/ 
void LCD_Cmd(uint8_t cmd)
{
    HC595_Write(LCD_EN_MASK);
    HC595_Write((cmd&0xF0)|LCD_EN_MASK);
    HC595_Write(((cmd&0xF0)|LCD_EN_MASK)&0xF0);
  
    HC595_Write(LCD_EN_MASK);
    HC595_Write((cmd<<4)|LCD_EN_MASK);
    HC595_Write(((cmd<<4)|LCD_EN_MASK)&0xF0);
}
/*****************************************************************************
** Put a writable character on LCD
** If the character is '\f', clear the LCD
** If the character is '\n', move cursor to the second line
*****************************************************************************/ 
void LCD_Putc(uint8_t data)
{
    //send 4 bit LOW
    HC595_Write(LCD_RS_MASK);
    HC595_Write((data&0xF0)|LCD_RS_MASK);
    HC595_Write(((data&0xF0)|LCD_RS_MASK)&0xF4);
    //Send 4 bit HIGH
    HC595_Write(LCD_RS_MASK);
    HC595_Write((data<<4)|LCD_RS_MASK);
    HC595_Write(((data<<4)|LCD_RS_MASK)&0xF4);
}
/*******************************************************************************
** Put a string s[] on LCD.
** please note that the string is ended with '/0' (or 0x00)
*******************************************************************************/
void LCD_PutString(uint8_t *s)
{
    while(*s)LCD_Putc(*s++);
}
/*******************************************************************************
** Move the pointer of LCD to coordinate (column, row)
** Note that cursor is at (0,0) after reset of clear LCD
*******************************************************************************/ 
void LCD_GotoXy(uint8_t x, uint8_t y)
{
    if(x==0)LCD_Cmd(0x80+((y-0)%16));
    else LCD_Cmd(0xC0+((y-0)%16));
}
/*******************************************************************************
** Clear the LCD
*******************************************************************************/ 
void LCD_Clear(void)
{
    LCD_Cmd(0x1);
    delay_us(20);
}
/*******************************************************************************
** Initialization for LCD module
*******************************************************************************/
void LCD_Init(void)
{
    HC595_INIT_PINS;
    HC595_Write(0x00);
    delay_us(150);
    HC595_Write(0x08);
    LCD_Cmd(0x02);
    delay_us(2);
    LCD_Cmd(0x28);
    LCD_Cmd(0x0C);
    LCD_Clear(); 
    LCD_Cmd(0x06);
    LCD_Cmd(0x80);
}
/*******************************************************************************
** Put a constant or a number stored in 'val' on LCD
** The number must be multiplied and convert in type "unsigned long"
**                                                or "unsigned int"
**   dec: amount of digits after '.'
**   neg: sign of the number, neg = 0 if the number is positive
**
** Example 1: lcd_number(val, 3, 0); // val = 12345
**        will put "12.345" on the LCD
** Example 2: lcd_number(val, 6, 1); // val = 12345
**        will put "-0.012345" on the LCD
**
** Note: This function may contain some bugs. If you can detect and fix them
**       , please contact me :d
*******************************************************************************/
void LCD_Number(uint32_t val, uint8_t dec, uint8_t neg)
{
    uint8_t i, j, digit, k;
    uint32_t total;
    uint32_t temp;
    for(i = 0, total = val; total > 0;i++) // count number
    {
        total /= 10;
        total = i;
    }
    if(neg !=0 )LCD_Putc('-');
    if((total - dec) <=0)LCD_Putc('0');// if total < dec put 0.xxx
    else
    {      
        for (i=0; i< (total-dec); i++)
        {
            temp = 1;
            for(j=1; j<(total-i); j++)
            temp *=10;
            digit = (val/temp)%10;
            LCD_Putc(digit + 0x30);
        }
    }
    if(dec > 0)
    {
        LCD_Putc('.');
        if((dec - total)> 0)	// ex: val = 55; dec = 3 put 0.055
        {
            for ( i = 0; i < (dec-total); i++)
            LCD_Putc('0');
            k = 0;
        }
        else k = total - dec;
        for(i=k; i< total; i++)
        {
            temp = 1;
            for(j=1; j<(total-i); j++)
            temp *=10;
            digit = (val/temp)%10;
            LCD_Putc(digit + 0x30);
        }
    }    
}
/*******************************************************************************
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
** Put 2 string s1 and s2 on LCD and shift all of them to right 
*******************************************************************************/
void LCD_2LineShiftToRight(uint8_t *s1, uint8_t *s2)
{
    LCD_2Line(s1, s2);
    delay_us(1000);
    for(int i=0; i<15; i++)
    {
        LCD_Cmd(0x1C);
        delay_us(250);
    }
}
/*******************************************************************************
* Convert a Interger number to String. Ex: 123 -> "123"
*******************************************************************************/
void LCD_IntToStr(uint8_t i, uint8_t *s)	// Convert Interger to String
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
*************************** END OF FILE ****************************************
*******************************************************************************/