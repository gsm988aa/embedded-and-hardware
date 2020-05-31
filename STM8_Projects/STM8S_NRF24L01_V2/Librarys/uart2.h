#ifndef __UART2_H
#define __UART2_H
#include <stdio.h>
#include "stm8s.h"
#define countof(a)   (sizeof(a) / sizeof(*(a)))
#define RxBufferSize 64
void Uart2_Init(void);
void UART2_SendByte(u8 data);
void UART2_SendString(u8* Data,u16 len);
u8 UART2_ReceiveByte(void);
static char *itoa(int value, char *string, int radix);
void UART2_printf( uint8_t *Data,...);
#endif