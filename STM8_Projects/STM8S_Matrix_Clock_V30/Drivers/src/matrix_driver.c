/*******************************************************************************
** File Name:  matrix_driver.c
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#include "matrix_driver.h"
#include "font5x7.h"
/******************************************************************************/
char display_ram[48];
char display_ram_effect[48];
char Text[400]="00:00:00";
extern void delay_us(uint32_t time);
/******************************************************************************/
void HC595_Write(uint8_t value)
{
    uint8_t i=0;
    for(i=0;i<8;i++)
    {
        if(((value<<i)&0x80)==0x80)HC595_DATA = 1;
        else HC595_DATA = 0;
        HC595_CLOCK = 1;
        HC595_CLOCK = 0;
    }
}
/******************************************************************************/
void out_data()
{
    HC595_LATCH = 1;
    HC595_LATCH = 0;
}
/******************************************************************************/
void write_data(uint8_t colum_addr)
{
    uint8_t i=0,j=0;
    while(i<6)
    {
        HC595_Write(display_ram[j+colum_addr]);
        j = j+8;
        i++;
    }
}
/******************************************************************************/
void Coppy_Data_To_Ram(uint8_t Line, uint8_t fill)
{
    uint8_t b=0,i=0;
    uint8_t cdata=0;
    uint8_t clm=0;
    while (Text[i]!='\0')
    {
        for(b=0;b<5;b++)
        {
            cdata=Font_5x7[Text[i]-32][b]<<(1-Line);
            if(fill)display_ram[clm+b]=~cdata;
            else display_ram[clm+b]=cdata;
        }
        i++;
        clm = clm+6;
    }
}
/******************************************************************************/
void ColumScroll(uint8_t colum, uint8_t value)
{
    int i=0;
    for(i=0;i<47;i++){display_ram[i]=display_ram[i+1];}
    display_ram[47]=~value;
}
/******************************************************************************/
void ScrollTextFont (uint8_t Line, uint32_t speed, uint8_t fill)
{
    uint8_t b=0,i=0;
    uint8_t cdata=0;
    while(Text[i]!='\0')
    {
        for(b=0;b<5;b++)
        {
            cdata=Font_5x7[Text[i]-32][b]<<(1-Line);
            if(fill)ColumScroll(1, cdata);
            else ColumScroll(1, ~cdata);
            delay_us(speed);
        }
        ColumScroll(1,0x00);
        delay_us(speed);
        i++;
    }
}
/******************************************************************************/
void Clear_Display_Ram(uint8_t fill)
{
    uint8_t i;
    for(i=0;i<48;i++)
    {
        if(fill==1)display_ram[i]=0xff;
        else display_ram[i]=0x00;
    }
}
/******************************************************************************/
void number_time_effect(uint8_t position, uint8_t value, uint8_t step, uint8_t pull, uint8_t Line)
{
    uint8_t i=0;
    uint8_t cdata=0xff;
    for(i=0;i<5;i++)
    {
        cdata=Font_5x7[16+value][i]<<(1-Line);
        if(pull==SHIFT_DOWN)
        {
            display_ram[position+i]=~((~display_ram[position+i]<<1)|(cdata>>step));
        }
        if(pull==SHIFT_UP)
        {
            display_ram[position+i]=~((~display_ram[position+i]>>1)|(cdata<<step));
        }
    }
}
/*******************************************************************************
****************************** END OF FILE *************************************
*******************************************************************************/