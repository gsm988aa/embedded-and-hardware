/*
 * delay.cpp
 *
 *  Created on: Jul 9, 2019
 *      Author: LapPV
 */
#include <stm32f4xx.h>
#include "delay.h"
/**
 * Initialization routine.
 * You might need to enable access to DWT registers on Cortex-M7
 *   DWT->LAR = 0xC5ACCE55
 */
void delay::Init(void)
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

#if DWT_DELAY_NEWBIE
/**
 * If you are a newbie and see magic in DWT_Delay, consider this more
 * illustrative function, where you explicitly determine a counter
 * value when delay should stop while keeping things in bounds of uint32.
*/
void delay::delay_us(uint32_t us)
{
	uint32_t startTick  = DWT->CYCCNT;
	uint32_t targetTick = DWT->CYCCNT + us * (SystemCoreClock/1000000);
	// Must check if target tick is out of bounds and overflowed
	if (targetTick > startTick) {
		// Not overflowed
		while (DWT->CYCCNT < targetTick);
	} else {
		// Overflowed
		while (DWT->CYCCNT > startTick || DWT->CYCCNT < targetTick);
	}
}
#else
/**
 * Delay routine itself.
 * Time is in microseconds (1/1000000th of a second), not to be
 * confused with millisecond (1/1000th).
 *
 * No need to check an overflow. Let it just tick :)
 *
 * @param uint32_t us  Number of microseconds to delay for
 */
void delay::delay_us(uint32_t us) // microseconds
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock/1000000);
    while (DWT->CYCCNT - startTick < delayTicks);
}
#endif
void delay::delay_ms(uint32_t ms)
{
	while(ms--)
	{
		delay_us(1000);
	}
}

