/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include "rtc_driver.h"
#include "matrix_driver.h"
/******************************************************************************/
extern char Text[400];
extern char display_ram[48];
uint8_t select1=0,select2=0,select3=0,select4=0,select5=0,select6=0;
uint8_t temp1,temp2;
uint8_t mode=0;
uint8_t dem=0;
uint8_t ok=0;
uint8_t start=0;
uint8_t enable=0;
uint8_t int_syc = 0;
rtc_t rtc;
/******************************************************************************/
void display_time(uint32_t speed, uint8_t pull, uint8_t line, uint8_t space);
void enable_select_led(void);
void disable_select_led(void);
void select_led_time(void);
void DL_to_AL_change(void);
void delay_us(uint32_t nCount);
void TIM4_Init(void);
void GPIO_Init(void);
void InitialiseSystemClock();
/******************************************************************************/
int main(void)
{
    rtc.hour = 18; //  10:40:20 am
    rtc.min =  10;
    rtc.sec =  0;
    rtc.date = 6; //1st Jan 2016
    rtc.month = 3;
    rtc.year = 18;
    rtc.weekDay = 3; // Friday: 5th day of week considering monday as first day.
    InitialiseSystemClock();
    RTC_Init();
    RTC_SetDateTime(&rtc);
    RTC_GetDateTime(&rtc);
    sprintf(Text, "%d%d:%d%d:%d%d", rtc.hour/10, rtc.hour%10, rtc.min/10, rtc.min%10, rtc.sec/10, rtc.sec%10);
    Clear_Display_Ram(1);
    Coppy_Data_To_Ram(0, 1);
    GPIO_Init();
    TIM4_Init();
    asm("rim");/* Enable interrup global */
    while(1)
    {
        if(start==1)
        {
            select_led_time();
            dem++;
            if(dem<35)display_time(3000, SHIFT_DOWN, 0, 0);
            else
            {
                if(rtc.weekDay==2)sprintf(Text,"       Monday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==3)sprintf(Text,"       Tuesday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==4)sprintf(Text,"       Wednesday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==5)sprintf(Text,"       Thursday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==6)sprintf(Text,"       Friday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==7)sprintf(Text,"       Saturday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                if(rtc.weekDay==1)sprintf(Text,"       Sunday : %d%d/%d%d/20%d%d         ", rtc.date/10,rtc.date%10,rtc.month/10,rtc.month%10,rtc.year/10,rtc.year%10);
                ScrollTextFont(0, 3000, 1);
                DL_to_AL_change();
                sprintf(Text,"%d%d/%d%d AL         ", rtc.adate/10, rtc.adate%10, rtc.amonth/10, rtc.amonth%10);
                ScrollTextFont(0, 3000, 1);
                //RTC_GetDateTime(&rtc);
                RTC_GetDateTime(&rtc);
                enable_select_led();
                display_time(2000, SHIFT_DOWN, 0, 1);//40
                dem=0;
            }
            start=0;
        }
    }
}
/******************************************************************************/
void display_time(uint32_t speed, uint8_t pull, uint8_t line, uint8_t space)
{
    int8_t i;
    for(i=8; i>=0; i--)
    {
        if(select1==1)number_time_effect(42, rtc.sec %10, i, pull, line);
        if(select2==1)number_time_effect(36, rtc.sec/10, i, pull, line);
        if(select3==1)number_time_effect(24, rtc.min%10, i, pull, line);
        if(select4==1)number_time_effect(18, rtc.min/10, i, pull, line);
        if(select5==1)number_time_effect(6, rtc.hour%10, i, pull, line);
        if(select6==1)number_time_effect(0, rtc.hour/10, i, pull, line);
        if(space==1)
        {
            number_time_effect(12, 10, i, pull, line);
            number_time_effect(30, 10, i, pull, line);
        }
        delay_us(speed);
    }
    disable_select_led();
}
/******************************************************************************/
void enable_select_led(void)
{
    select1=1;select2=1;
    select3=1;select4=1;
    select5=1;select6=1;
}
/******************************************************************************/
void disable_select_led(void)
{
    select1=0;select2=0;
    select3=0;select4=0;
    select5=0;select6=0;
}
/******************************************************************************/
void select_led_time(void)
{
    temp1=rtc.sec%10;
    temp2=rtc.sec/10;
    rtc.sec++;
    if((rtc.sec%10)!=temp1)select1=1;
    if((rtc.sec/10)!=temp2)select2=1;
    temp1=rtc.min%10;
    temp2=rtc.min/10;
    if(rtc.sec>59){rtc.sec=0;rtc.min++;}
    if((rtc.min%10)!=temp1)select3=1;
    if((rtc.min/10)!=temp2)select4=1;
    temp1=rtc.hour%10;
    temp2=rtc.hour/10;
    if(rtc.min>59){rtc.min=0;rtc.hour++;if(rtc.hour>23)rtc.hour=0;}
    if((rtc.hour%10)!=temp1)select5=1;
    if((rtc.hour/10)!=temp2)select6=1;
}
/******************************************************************************/
void DL_to_AL_change(void)
{
    uint32_t amlich;
    amlich = lunar_calculate(rtc.date, rtc.month, rtc.year);
    rtc.adate  = amlich>>=8;
    rtc.amonth = amlich>>=16;
    rtc.ayear = amlich>>=24;
}
/******************************************************************************/
void TIM4_Init(void)
{
  TIM4_PSCR = 0x07;//TIM4_PRESCALER_128
  TIM4_ARR = 0xC8; // TIM4_Period 200
  TIM4_IER = 0x01;
  TIM4_CNTR = 0xFA;
  TIM4_CR1 |= 0x01; /* Enable interrup Time4 */
}
/******************************************************************************/
void GPIO_Init(void)
{
    PC_DDR_DDR7 = 1;
    PC_CR1_C17  = 1;
    PC_CR2_C27  = 0;
    
    PC_DDR_DDR5 = 1;
    PC_CR1_C15  = 1;
    PC_CR2_C25  = 0;
    
    PC_DDR_DDR3 = 1;
    PC_CR1_C13  = 1;
    PC_CR2_C23  = 0;
    
    PC_DDR_DDR4 = 1;
    PC_CR1_C14  = 1;
    PC_CR2_C24  = 0;
    
    PC_DDR_DDR6 = 1;
    PC_CR1_C16  = 1;// Push/pull
    PC_CR2_C26  = 0;
    
    PE_DDR_DDR5 = 0; // Input
    PE_CR1_C15 = 1;
    PE_CR2_C25 = 1; // cho phép ng?t t?i PE5
    EXTI_CR2 = 0x02;
}
/******************************************************************************/
void InitialiseSystemClock()
{
    CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
    CLK_ICKR_HSIEN = 1;                 //  Enable the HSI.
    CLK_ECKR = 0;                       //  Disable the external clock.
    while (CLK_ICKR_HSIRDY == 0);       //  Wait for the HSI to be ready for use.
    CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
    CLK_PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
    CLK_PCKENR2 = 0xff;                 //  Ditto.
    CLK_CCOR = 0;                       //  Turn off CCO.
    CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
    CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
    CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
    CLK_SWCR = 0;                       //  Reset the clock switch control register.
    CLK_SWCR_SWEN = 1;                  //  Enable switching.
    while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}
/******************************************************************************/
void delay_us(uint32_t nCount)
{
    while(nCount--);
}
/******************************************************************************/
#pragma vector=0x19
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
    switch(int_syc)
    {
        case 0:{HC595_Write(0x01);write_data(int_syc);out_data();}break;
        case 1:{HC595_Write(0x02);write_data(int_syc);out_data();}break;
        case 2:{HC595_Write(0x04);write_data(int_syc);out_data();}break;
        case 3:{HC595_Write(0x08);write_data(int_syc);out_data();}break;
        case 4:{HC595_Write(0x10);write_data(int_syc);out_data();}break;
        case 5:{HC595_Write(0x20);write_data(int_syc);out_data();}break;
        case 6:{HC595_Write(0x40);write_data(int_syc);out_data();}break;
        case 7:{HC595_Write(0x80);write_data(int_syc);out_data();}break;
    }
    int_syc++;if(int_syc>7)int_syc=0;
    TIM4_SR = 0; /* Clear timer4 interrup flag*/
}
/******************************************************************************/
#pragma vector =  EXTI4_vector // Vector ngat PORTE
__interrupt void EXTI_PORTE_IRQHandler(void)
{
    start=1;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
