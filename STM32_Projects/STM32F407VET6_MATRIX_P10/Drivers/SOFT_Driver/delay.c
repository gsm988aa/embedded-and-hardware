#include "delay.h"

static __IO uint32_t sysTickCounter;

void SysTick_Init(void)
{
    /****************************************
     *SystemFrequency/1000      1ms         *
     *SystemFrequency/100000    10us        *
     *SystemFrequency/1000000   1us         *
     *****************************************/
      while(HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/100000)!=0);
      HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
      HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * This method needs to be called in the SysTick_Handler
 */
void TimeTick_Decrement(void) 
{
    if(sysTickCounter != 0x00) 
    {
        sysTickCounter--;
    }
}

void delay_10us(uint32_t n) 
{
    sysTickCounter = n;
    while (sysTickCounter != 0);
}

void delay_ms(uint32_t n) 
{
    sysTickCounter = n*100;
    while (sysTickCounter != 0);
}