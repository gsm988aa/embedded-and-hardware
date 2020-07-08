/*******************************************************************************
** File Name:  rtc_driver.h
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_
#include "i2c_driver.h"
/******************************************************************************/
#define DS1307_READ_MODE  	    0xD1u   // DS1307 ID in read mode
#define DS1307_WRITE_MODE  	    0xD0u   // DS1307 ID in write mode
#define DS1307_SECOND_REG_ADDRESS   0x00u   // Address to access Ds1307 SEC register
#define DS1307_DATE_REG_ADDRESS     0x04u   // Address to access Ds1307 DATE register
#define DS1307_CONTROL_REG_ADDRESS  0x07u   // Address to access Ds1307 CONTROL register
/******************************************************************************/
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t weekDay;
    uint8_t date;
    uint8_t month;
    uint8_t year;  
    uint8_t adate; 
    uint8_t amonth; 
    uint8_t ayear; 
}rtc_t;
/******************************************************************************/
void RTC_Init(void);
void RTC_SetDateTime(rtc_t *rtc);
void RTC_GetDateTime(rtc_t *rtc);
uint32_t lunar_calculate(uint8_t x, uint8_t y, uint8_t t);
/******************************************************************************/
#endif /* RTC_DRIVER_H_ */
/*******************************************************************************
**                            END OF FILE                                     **
*******************************************************************************/