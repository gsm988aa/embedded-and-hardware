
/*#include "stm8s.h"
#include "sysclock.h"
#include "uart2.h"
#include "nRF24L01.h"

void Delay(u32 nCount);

u8 TxBuf[32]="Hello World!";

int main(void)
{
	//SystemClock_Init(HSE_Clock);
	
	//Uart2_Init();
	
	nRF24L01_Pin_Conf();
	nRF24L01_Set_TxMode();
	
	enableInterrupts();
	
	while(1)
	{ 
		nRF24L01_SendData(TxBuf);
		
		while(nRRF24L01_CheckACK());
	}
}





#ifdef USE_FULL_ASSERT

void assert_failed(u8* file, u32 line)
{ 
  while (1)
  {
  }
}
#endif*/



#include "stm8s.h"
#include "sysclock.h"
#include "uart2.h"
#include "nRF24L01.h"
void delay_us(uint32_t time)
{
    while(time--);
}
int main(void)
{
	u8 RxBuf[32];
	SystemClock_Init(HSE_Clock);
	//Uart2_Init();
	nRF24L01_Pin_Conf();
	
	__enable_interrupt();
	
	while(1)
	{ 
		
		nRF24L01_Set_RxMode();
		if (!(nRF24L01_RevData(RxBuf))) {
			//UART2_SendString(RxBuf, 32);
                  delay_us(15);
		}
	}
}
#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{ 
  while (1);
}
#endif