#include "DELAY.h"
/*******************************************************************************
                         void delay_us(uint16_t us_count)
********************************************************************************
 * I/P Arguments: uint16_t.
 * Return value    : none
 * description :
         This function is used generate delay in us.
         It generates a delay of approximate 10us for each count,
         if 5000 is passed as the argument then it generates a delay of 50ms.          
*******************************************************************************/
void delay_us(uint32_t us_count)
{    
    while(us_count--);
}
/*******************************************************************************
                         void delay_ms(uint16_t us_count)
********************************************************************************
 * I/P Arguments: uint16_t.
 * Return value    : none
 * description:
     This function is used generate delay in ms.
     It generates a delay of 1ms for each count,
     if 1000 is passed as the argument then it generates delay of 1000ms(1sec)
*******************************************************************************/
void delay_ms(uint16_t ms_count)
{
    while(ms_count!=0)
    {
        delay_us(C_CountForOneMsDelay_U16);
        ms_count--;
    }
}
/*******************************************************************************
                         void delay_sec(uint16_t us_count)
********************************************************************************
*** I/P Arguments: uint16_t.
*** Return value    : none
*** Description:
        This function is used generate delay in sec .
        It generates a approximate delay of 1sec for each count,
        if 10 is passed as the argument then it generates delay of 10sec
*******************************************************************************/
#if (ENABLE_DELAY_SEC == 1)
void delay_sec(uint16_t sec_count)
{
    while(sec_count!=0)
    {
        delay_ms(1000);
        sec_count--;
    }
}
#endif
/*******************************************************************************
                                END OF FILE
*******************************************************************************/