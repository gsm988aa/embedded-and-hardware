#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"

void SysTick_Init(void);
void TimeTick_Decrement(void);
void delay_10us(uint32_t n);
void delay_ms(uint32_t n) ;
#endif