/*******************************************************************************
***  File Name    : main.c
***  Project name : STM8S_Matrix_Clock_V2.0
***  Author       : Pham Van Lap KMT
***  Company      : FPT Software
***  Create Date  : 20/01/2017
***  Device Use   : STM8S105K4T6C
*******************************************************************************/
//#include "main.h"
#include "stdio.h"
#include "DELAY.h"
#include "DS1307.h"
#include "matrix_drv.h"
/******************************************************************************/
#define SHIFT_UP     1
#define SHIFT_DOWN   0
/******************************************************************************/
extern char Text[400];
extern uint8_t display_ram[48];
uint8_t select1=0,select2=0,select3=0,select4=0,select5=0,select6=0;
uint8_t temp1,temp2;
uint8_t mode=0;
uint8_t dem=0;
uint8_t ok=0;
uint8_t start=0;
uint8_t enable=0;
/******************************************************************************/
typedef struct
{
    uint8_t date;
    uint8_t month;
    uint8_t year;
}lunar_calendar_t;
/******************************************************************************/
rtc_t rtc;
lunar_calendar_t lunar;
/******************************************************************************/
static void CLK_Config(void);
static void GPIO_Config(void);
static void TIM4_Config();
void display_time(rtc_t *rtc, uint32_t speed, uint8_t pull, uint8_t Line, uint8_t space);
void enable_select_led();
void disable_select_led();
void select_led_time();
void DL_to_AL_change(lunar_calendar_t *lunar);
/******************************************************************************/
void main(void)
{
    RTC_Init();
    CLK_Config();
    GPIO_Config();
    TIM4_Config();
    Clear_Display_Ram(1);
    ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_0);//Timer4 priority is level 3
    sprintf(Text, "%d%d:%d%d:%d%d", rtc.hour/10, rtc.hour%10, rtc.min/10, rtc.min%10, rtc.sec/10, rtc.sec%10);
    Coppy_Data_To_Ram(0, 1);
    enableInterrupts();
    while(1)
    {
        if(start==1)
        {
            select_led_time();
            dem++;
            if(dem<35)display_time(&rtc, 3000, SHIFT_DOWN, 0, 0);
            else
            {
                if(rtc.weekDay==2)sprintf(Text,"       Monday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==3)sprintf(Text,"       Tuesday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==4)sprintf(Text,"       Wednesday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==5)sprintf(Text,"       Thursday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==6)sprintf(Text,"       Friday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==7)sprintf(Text,"       Saturday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                if(rtc.weekDay==1)sprintf(Text,"       Sunday : %d%d/%d%d/20%d%d         ", rtc.date/10, rtc.date%10, rtc.month/10, rtc.month%10, rtc.year/10, rtc.year%10);
                ScrollTextFont(0, 3000, 1);
                DL_to_AL_change(&lunar);
                sprintf(Text,"%d%d/%d%d AL         ", lunar.date/10, lunar.date%10, lunar.month/10, lunar.month%10);
                ScrollTextFont(0, 3000, 1);
                RTC_GetDateTime(&rtc);
                enable_select_led();
                display_time(&rtc, 2000, SHIFT_DOWN, 0, 1);//40
                dem=0;
            }
            start=0;
        }
    }
}
/******************************************************************************/
static void CLK_Config(void)
{
    CLK_DeInit();
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);
}
/******************************************************************************/
static void GPIO_Config(void)
{
    GPIO_DeInit(GPIOE);        
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_DeInit(GPIOC);        
    GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix      
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix        
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);// Pin for Matrix
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_IN_FL_IT); 
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE, EXTI_SENSITIVITY_FALL_ONLY);
}
/******************************************************************************/
static void TIM4_Config()
{
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 200);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
}
/******************************************************************************/
void display_time(rtc_t *rtc, uint32_t speed, uint8_t pull, uint8_t Line, uint8_t space)
{
    signed int i=8;
    for(i=8; i>=0; i--)
    {
        if(select1==1)number_time_effect(42, rtc->sec%10, i, pull, Line);
        if(select2==1)number_time_effect(36, rtc->sec/10, i, pull, Line);
        if(select3==1)number_time_effect(24, rtc->min%10, i, pull, Line);
        if(select4==1)number_time_effect(18, rtc->min/10, i, pull, Line);
        if(select5==1)number_time_effect(6, rtc->hour%10, i, pull, Line);
        if(select6==1)number_time_effect(0, rtc->hour/10, i, pull, Line);
        if(space==1)
        {
            number_time_effect(12, 10, i, pull, Line);
            number_time_effect(30, 10, i, pull, Line);
        }
        delay_us(speed);
    }
    disable_select_led();
}
/******************************************************************************/
void enable_select_led()
{
    select1=1; select2=1;
    select3=1; select4=1;
    select5=1; select6=1;
}
/******************************************************************************/
void disable_select_led()
{
    select1=0;select2=0;
    select3=0;select4=0;
    select5=0;select6=0;
}
/******************************************************************************/
void select_led_time()
{
    temp1=rtc.sec%10;
    temp2=rtc.sec/10;
    rtc.sec++;
    if((rtc.sec%10)!=temp1)select1=1;
    if((rtc.sec/10)!=temp2)select2=1;
    temp1=rtc.min%10;
    temp2=rtc.min/10;
    if(rtc.sec>59){rtc.sec=0; rtc.min++;}
    if((rtc.min%10)!=temp1)select3=1;
    if((rtc.min/10)!=temp2)select4=1;
    temp1=rtc.hour%10;
    temp2=rtc.hour/10;
    if(rtc.min>59){rtc.min=0;rtc.hour++;if(rtc.hour>23)rtc.hour=0;}
    if((rtc.hour%10)!=temp1)select5=1;
    if((rtc.hour/10)!=temp2)select6=1;
}
/******************************************************************************/
void DL_to_AL_change(lunar_calendar_t *lunar)
{
    uint32_t temp = 0;
    temp  = RTC_Lunar_Calculate(&rtc);
    temp>>=8;
    lunar->date = temp;
    temp>>=8;
    lunar->month = temp;
    temp>>=8;
    lunar->year = temp;
}
/******************************************************************************/