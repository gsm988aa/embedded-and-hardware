
#include "sysclock.h"
#define DEBUG
void HSE_ClockStartUpConfiguration(HSE_Clock_TypeDef HSE_Clock,HSE_ClockStartUpTime_TypeDef HSE_ClockStartUpTime)
{
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  while(!(FLASH->IAPSR & FLASH_IAPSR_DUL));
  if(HSE_Clock==HSE_24MHz)   
  FLASH_ProgramOptionByte(FLASH_WAIT_STATES_ADDRESS,HSE_Clock);
  FLASH_ProgramOptionByte(HSE_CLOCK_STARTUP_ADDRESS,HSE_ClockStartUpTime);
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void DefaultSystemClockForHSI(void)
{
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  while(!(FLASH->IAPSR & FLASH_IAPSR_DUL));
  FLASH_EraseOptionByte(FLASH_WAIT_STATES_ADDRESS);
  FLASH_EraseOptionByte(HSE_CLOCK_STARTUP_ADDRESS);
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void SystemClock_Init(SystemClock_TypeDef sysclk)
{

   if(sysclk==HSE_Clock)
   {
     #ifdef DEBUG
     HSE_ClockStartUpConfiguration(HSE_24MHz,HSECNT_8CLK);
     #endif
     while (!CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE,\
            CLK_CURRENTCLOCKSTATE_DISABLE));
   }
   else 
   {
     #ifdef DEBUG
     DefaultSystemClockForHSI();
     #endif
     CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
   }

}