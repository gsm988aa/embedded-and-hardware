#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H
/**********************************************************************/
#include     "stm8s.h"
/**********************************************************************/
#define     SPI_SCLK    
#define     SPI_MOSI
#define     SPI_MISO
#define     SPI_SS
/**********************************************************************/
/*Function: Set SPI Initial state, ready to communicate*/
void SPI_Init();
/*Function: Send a byte to peripheral device*/
void SPI_Write_Byte(unsigned char _byte);
/*Function: Receive a byte from peripheral device*/
uint8_t SPI_Read_Byte();
/**********************************************************************/
#endif
/**********************************************************************/
/**********************************************************************/