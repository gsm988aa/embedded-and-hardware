/*******************************************************************************
                       Copyright by Pham Van Lap KMT
********************************************************************************
** File Name        : DS18B20.C
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
                          Include Files
*******************************************************************************/
#include "DS18B20.h"
/*******************************************************************************
                         void DS18B20_Init(void)
*******************************************************************************/
void DS18B20_Init(void)
{
    DS18B20_DQ_OUTPUT_MODE;     
    delay_us(10);
    DS18B20_DQ_LOW;   
    delay_us(600);
    
    DS18B20_DQ_INPUT_MODE;     
    delay_us(100);     
    while(DS18B20_DQ_INPUT == 1);
    delay_us(400);
}
/*******************************************************************************
                         void DS18B20_WriteByte(uint8_t data)
*******************************************************************************/
void DS18B20_WriteByte(uint8_t data)
{
    DS18B20_DQ_OUTPUT_MODE;
    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_DQ_LOW;
        delay_us(2);
        if(data & 0x01)DS18B20_DQ_HIGH;
        data >>= 1;
        delay_us(60);
        DS18B20_DQ_HIGH;
    }
}
/*******************************************************************************
                         uint8_t DS18B20_ReadByte(void)
*******************************************************************************/
uint8_t DS18B20_ReadByte(void)
{
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_DQ_OUTPUT_MODE;
        DS18B20_DQ_LOW;
        delay_us(5);
        data >>= 1;
        DS18B20_DQ_HIGH;
        DS18B20_DQ_INPUT_MODE;
        if(DS18B20_DQ_INPUT)data |= 0x80;
        DS18B20_DQ_OUTPUT_MODE; 
        DS18B20_DQ_HIGH;
        delay_us(60);
    }
    return data;
}
/*******************************************************************************
                         float DS18B20_ReadTemperature(void)
*******************************************************************************/
float DS18B20_ReadTemperature(void)
{
    uint8_t temp = 0;
    float t = 0;
/* 
 *  0x44 is the command to calculate the temperature and save it in the scratchpad.  This takes up to 750 milliseconds.
 *  0xBE is the command to send the contents of of the scratchpad.
 *  What you are sending is "Everybody: Send your Scratchpad".  This only works if you only have one device.  
 */
    DS18B20_Init();
    DS18B20_WriteByte(0xcc);
    DS18B20_WriteByte(0x44);
    DS18B20_Init();
    DS18B20_WriteByte(0xcc);
    DS18B20_WriteByte(0xbe);
    temp = DS18B20_ReadByte();
    t = (((temp & 0xf0) >> 4) + (temp & 0x07) * 0.125); 
    temp = DS18B20_ReadByte();
    t += ((temp & 0x0f) << 4);
    return t;
}
/*******************************************************************************
******************************** END OF FILE ***********************************
*******************************************************************************/