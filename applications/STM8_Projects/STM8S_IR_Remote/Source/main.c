#include "main.h"
extern uint16_t TimingDelay;
extern uint8_t start;
uint16_t irCode=0, irCode1=0, irCode2=0;
void delay_50us(uint16_t us);
void delay_ms(uint16_t ms);
uint16_t remote_get_code();
uint8_t remote_check_start();

static void InternalClockInit(void)
{
    CLK_DeInit();
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY)!=SET);
}
/******************************************************************************/
static void TIM4_Init()
{
    TIM4_TimeBaseInit(TIM4_PRESCALER_32, 24);//50us
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
}
/******************************************************************************/
void delay_50us(uint16_t us)
{
    TimingDelay = us;
    while(TimingDelay!=0);
}
/******************************************************************************/
void delay_ms(uint16_t ms)
{
   while(ms--)delay_50us(20);
}
/******************************************************************************/
/******************************************************************************/
static void GPIO_Config(void)
{
    GPIO_DeInit(GPIOI);  
    GPIO_DeInit(GPIOB);
    GPIO_Init(GPIOI, GPIO_PIN_0, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOI, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOI, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);
    EXTI_DeInit();
    GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_IN_FL_IT);
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
}
/******************************************************************************/
uint16_t remote_get_code()
{
    uint8_t k;
    uint16_t data_infr=0;
    while(INFR_DATA==0);
    while(INFR_DATA!=0);
    for(k=0;k<15;k++)
    {
        while(INFR_DATA==0);
        while(INFR_DATA!=0);
    }
    for(k=0;k<15;k++)
    {
        while(INFR_DATA==0);
        delay_50us(15);
        data_infr*=2;
        if(INFR_DATA!=0){data_infr|=1;}
        while(INFR_DATA!=0);
        while(INFR_DATA==0);
    }
    delay_ms(200);
    return data_infr;
}
/******************************************************************************/
/******************************************************************************/
int main(void)
{
    disableInterrupts();
    InternalClockInit();
    TIM4_Init();
    GPIO_Config();
    enableInterrupts();
    while(1)
    {
        if(start!=0)
        {
            EXTI_DeInit();
            irCode = remote_get_code();
            EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
            start=0;
        }
        if(irCode==0x3A05)GPIO_WriteReverse(GPIOI, GPIO_PIN_0); // Phim 1
        if(irCode==0x1A25)GPIO_WriteReverse(GPIOI, GPIO_PIN_1); // Phim 2
        if(irCode==0x2A15)GPIO_WriteReverse(GPIOI, GPIO_PIN_2); // Phim 3
        irCode=0;
    }
}
