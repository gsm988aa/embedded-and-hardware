/*******************************************************************************
** File Name:  i2c_driver.c
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#include "i2c_driver.h"
/******************************************************************************/
void I2C_Delay(uint16_t Count)
{
    uint8_t i,j;
    while (Count--)
    {
        for(i=0;i<20;i++)
          for(j=0;j<10;j++);
    }
}
/******************************************************************************/
void I2C_SDA_DDR(uint8_t ddr)
{
    if(ddr==1)
    {
        SDA_DDR=1;
        SDA_CR1=1;
        SDA_CR2=0;
    }
    else
    {
        SDA_DDR=0;
        SDA_CR1=1;
        SDA_CR2=0;
    }
}
/******************************************************************************/
void I2C_Start(void)
{
    I2C_SDA_DDR(1);
    SDA_OUT=1;
    SCL=1;
    I2C_Delay(1);
    SDA_OUT=0;
    I2C_Delay(1);
    SCL=0;
    I2C_Delay(1);
}
/******************************************************************************/
void I2C_Stop(void)
{
    I2C_SDA_DDR(1);
    SDA_OUT=0;
    SCL=0;
    I2C_Delay(1);
    SCL=1;
    I2C_Delay(1);
    SDA_OUT=1;
    I2C_Delay(1);
}
/******************************************************************************/
uint8_t I2C_WriteByte(uint8_t DAT)
{
    uint8_t num,I2C_Write_ACK=0;
    I2C_SDA_DDR(1);
    I2C_Delay(1);
    for(num=0x80; num!=0; num>>=1)
    {
        if((DAT&num)==0)SDA_OUT=0;
        else SDA_OUT=1;
        I2C_Delay(1);
        SCL=1;
        I2C_Delay(1);
        SCL=0;
        I2C_Delay(1);
    }
    SDA_OUT=1;
    I2C_Delay(1);
    SCL=1;
    I2C_Delay(1);
    I2C_SDA_DDR(0);
    I2C_Delay(1);
    I2C_Write_ACK=SDA_IN;
    I2C_Delay(1);
    SCL=0;
    return I2C_Write_ACK;
}
/******************************************************************************/
uint8_t I2C_ReadByteNACK(void)
{
    uint8_t x, I2CDATA;
    I2C_SDA_DDR(1);
    SDA_OUT=1;
    I2C_Delay(1);
    I2C_SDA_DDR(0);
    I2C_Delay(1);
    for(x=0x80;x!=0;x>>=1)
    {
        I2C_Delay(1);
        SCL=1;
        if(SDA_IN==0)I2CDATA&=~x;
        else I2CDATA|=x;
        I2C_Delay(1);
        SCL=0;
    }
    I2C_SDA_DDR(1);
    I2C_Delay(1);
    SDA_OUT=1;
    I2C_Delay(1);
    SCL=1;
    I2C_Delay(1);
    SCL=0;
    return I2CDATA;
}
/******************************************************************************/
uint8_t I2C_ReadByteACK(void)
{
    uint8_t x,I2CDATA;
    I2C_SDA_DDR(1);
    I2C_Delay(1);
    SDA_OUT=1;
    I2C_Delay(1);
    I2C_SDA_DDR(0);
    I2C_Delay(1);
    for(x=0x80;x!=0;x>>=1)
    {
        I2C_Delay(1);
        SCL=1;
        if(SDA_IN==0)I2CDATA&=~x;
        else I2CDATA|=x;
        I2C_Delay(1);
        SCL=0;
    }
    I2C_SDA_DDR(1);
    I2C_Delay(1);
    SDA_OUT=0;
    I2C_Delay(1);
    SCL=1;
    I2C_Delay(1);
    SCL=0;
    return I2CDATA;
}
/*******************************************************************************
**                            END OF FILE                                     **
*******************************************************************************/