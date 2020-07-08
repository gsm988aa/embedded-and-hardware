#ifndef __W5500HARDWAREDRIVER_H_
#define __W5500HARDWAREDRIVER_H_
#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <wizchip_conf.h>


//// Пины для подключения к W5500 (кроме ног SCLK, MOSI, MISO)
// CS
#define W5500_CS_PORT	        GPIOE
#define W5500_CS_PIN	        GPIO_PIN_8
// RESET
#define W5500_RESET_PORT		GPIOE
#define W5500_RESET_PIN	    	GPIO_PIN_7
// INT
//#define W5500_INT_PORT	GPIOA
//#define W5500_INT_PIN	        (1 << 2)



void W5500_Init();
void W5500_chipInit(void);
void W5500WriteByte(unsigned char byte);
uint8_t W5500ReadByte(void);
void W5500Select(void);
void W5500DeSelect(void);
uint8_t wizchip_read(void);
void  wizchip_write(uint8_t wb);

#endif
