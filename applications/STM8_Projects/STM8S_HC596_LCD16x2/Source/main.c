#include "DELAY.h"
#include "LCD16X2.h"
#include "DS1307.h"
#include "DS18B20.h"
#include <stdio.h>
#include <stdlib.h>
char buff[16] = "";
/******************************************************************************/
#define LAMP_CONTROL_PORT    (GPIOE)
#define BUTTON_GPIO_PORT     (GPIOC)
#define BUTTON_EXTI_PORT     (EXTI_PORT_GPIOC)

#define LAMP_CONTROL_PIN     (GPIO_PIN_5)

#define BUTTON1_PIN          (GPIO_PIN_1)
#define BUTTON2_PIN          (GPIO_PIN_2)
#define BUTTON3_PIN          (GPIO_PIN_3)
#define BUTTON4_PIN          (GPIO_PIN_4)
#define BUTTON_PINS          (BUTTON1_PIN|BUTTON2_PIN|BUTTON3_PIN|BUTTON4_PIN)

#define BUTTON1_INPUT   GPIO_ReadInputPin(BUTTON_GPIO_PORT, BUTTON1_PIN)
#define BUTTON2_INPUT   GPIO_ReadInputPin(BUTTON_GPIO_PORT, BUTTON2_PIN)
#define BUTTON3_INPUT   GPIO_ReadInputPin(BUTTON_GPIO_PORT, BUTTON3_PIN)
#define BUTTON4_INPUT   GPIO_ReadInputPin(BUTTON_GPIO_PORT, BUTTON4_PIN)

#define LAMP_CONTROL_ENABLE  GPIO_WriteHigh(LAMP_CONTROL_PORT, LAMP_CONTROL_PIN)
#define LAMP_CONTROL_DISABLE GPIO_WriteLow(LAMP_CONTROL_PORT, LAMP_CONTROL_PIN)
/******************************************************************************/
uint8_t rtcMode = 0, setupting = 0;
uint8_t minHour=0, maxHour=0;
uint8_t minMin=0, maxMin=0;
RTC_TypeDef rtc;
/******************************************************************************/
void System_Init(void);
void ButtonProcess(void);
/******************************************************************************/
void System_Init(void)
{
    EXTI_DeInit();
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(LAMP_CONTROL_PORT, LAMP_CONTROL_PIN, GPIO_MODE_OUT_OD_LOW_SLOW);
    GPIO_Init(BUTTON_GPIO_PORT, (GPIO_Pin_TypeDef)(BUTTON_PINS), GPIO_MODE_IN_PU_IT);
    EXTI_SetExtIntSensitivity(BUTTON_EXTI_PORT, EXTI_SENSITIVITY_FALL_ONLY);
    enableInterrupts();
}
/******************************************************************************/
void ButtonProcess(void)
{
    if(BUTTON1_INPUT==0)/* Menu button */
    {
        setupting = 1;
        rtcMode++;
        if(rtcMode>7)rtcMode = 0;
        delay_ms(50);
        while(!BUTTON1_INPUT);
    }
    if(BUTTON2_INPUT==0) /* Button up */
    {
        if(rtcMode==1)/* Setup hour */
        {
            rtc.hour++;
            if(rtc.hour>23)rtc.hour = 0;
        }
        if(rtcMode==2)/* Setup min */
        {
            rtc.min++;
            if(rtc.min>59)rtc.min = 0;
        }
        if(rtcMode==3)/* Setup sec */
        {
            rtc.sec++;
            if(rtc.sec>59)rtc.sec = 0;
        }
        if(rtcMode==4)/* Setup weekDay */
        {
            rtc.weekDay++;
            if(rtc.weekDay>7)rtc.weekDay = 0;
        }
        if(rtcMode==5)/* Setup Day */
        {
            rtc.date++;
            if(rtc.date>31)rtc.date = 0;
        }
        if(rtcMode==6)/* Setup month */
        {
            rtc.month++;
            if(rtc.month>12)rtc.month = 0;
        }
        if(rtcMode==7)/* Setup year */
        {
            rtc.year++;
            if(rtc.year>99)rtc.year = 0;
        }
    }
    if(BUTTON3_INPUT==0) /* Button down */
    {
        if(rtcMode==1)/* Setup hour */
        {
            if(rtc.hour>0)rtc.hour--;
            else rtc.hour = 23;
        }
        if(rtcMode==2)/* Setup min */
        {
            if(rtc.min>0)rtc.min--;
            else rtc.min = 59;
        }
        if(rtcMode==3)/* Setup sec */
        {
            if(rtc.sec>0)rtc.sec--;
            else rtc.sec = 59;
        }
        if(rtcMode==4)/* Setup weekDay */
        {
            if(rtc.weekDay>0)rtc.weekDay--;
            else rtc.weekDay = 7;
        }
        if(rtcMode==5)/* Setup Day */
        {
            if(rtc.date>0)rtc.date--;
            else rtc.date = 31;
        }
        if(rtcMode==6)/* Setup month */
        {
            if(rtc.month>0)rtc.month--;
            else rtc.month = 12;
        }
        if(rtcMode==7)/* Setup year */
        {
            if(rtc.year>0)rtc.year--;
            else rtc.year = 99;
        }
    }
    if(BUTTON4_INPUT==0) /* Button ok */
    {
        RTC_SetDateTime(&rtc);
        rtcMode=0;
        setupting = 0;
    }
}
/******************************************************************************/
void ControlLamp(void)
{
    /*if((rtc.hour>minHour)&&(rtc.hour<maxHour))
    {
        if((rtc.min>minMin)&&(rtc.min<maxMin))
        {       
            LAMP_CONTROL_ENABLE;
        }
        else LAMP_CONTROL_DISABLE;
    }*/
    if(rtc.hour<7)LAMP_CONTROL_ENABLE;
    else LAMP_CONTROL_DISABLE;
}
/******************************************************************************/
void main(void)
{
    System_Init();
    LCD_Init();
    RTC_Init();
    //DS18B20_ReadTemperature();
    while(1)
    {
        if(setupting==1)
        {
            LCD_GotoXy(0, 0);
            LCD_PutString("SETUP TIME      ");
            if(rtcMode==1)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Hour: %d%d       ", rtc.hour/10, rtc.hour%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==2)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Min:  %d%d       ", rtc.min/10, rtc.min%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==3)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Sec:  %d%d       ", rtc.sec/10, rtc.sec%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==4)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "WeekDay:  %d%d   ", rtc.weekDay/10, rtc.weekDay%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==5)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Date:  %d%d      ", rtc.date/10, rtc.date%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==6)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Month:  %d%d     ", rtc.month/10, rtc.month%10);
                LCD_PutString((uint8_t*)buff);
            }
            if(rtcMode==7)
            {
                LCD_GotoXy(0, 1);
                sprintf(buff, "Year:  %d%d      ", rtc.year/10, rtc.year%10);
                LCD_PutString((uint8_t*)buff);
            }
        }
        else
        {
            RTC_GetDateTime(&rtc);
            LCD_GotoXy(0, 0);
            sprintf(buff, "    %d%d:%d%d:%d%d   ", rtc.hour/10, rtc.hour%10, rtc.min/10, rtc.min%10, rtc.sec/10, rtc.sec%10);
            LCD_PutString((uint8_t*)buff);
            LCD_GotoXy(0, 1);
            LCD_PutString("Mode: AUTO");
            //LAMP_CONTROL_DISABLE;
            ControlLamp();
        }
    }
}
/******************************************************************************/
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
    while(1);
}
#endif
/*******************************************************************************
*************************** END OF FILE ****************************************
*******************************************************************************/
