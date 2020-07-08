#include "spi_driver.h"
/**********************************************************************/
/* Set SPI Initial state, ready to communicate */
void SPIInit()
{
    SPI_SS = 1;//Set SS high, ready to pull down to transmiss signals
    SPI_SCLK = 0; //Set SCLK low, SPOL = 0
    SPI_MISO = 1;//Set master input high, ready to read signals from slave
}
/**********************************************************************/
/* Send a byte to peripheral device*/ 
void SPI_Write_Byte(unsigned char _byte)
{
    unsigned char i; 
    for(i=0; i<8; i++)
    {
        SPI_SCLK = 0;
        if(_byte & 0x80)SPI_MOSI = 1;
        else SPI_MOSI = 0;
        SPI_SCLK = 1;
        _byte <<= 1;
    }
}
/**********************************************************************/
/* Receive a byte from peripheral device*/
uint8_t SPI_Read_Byte()
{
    uint8_t i, _byte;
    for(i=0; i<8; i++)
    { 
        _byte <<= 1;
        SPI_SCLK = 1;
        if(SPI_MISO)_byte |= 0x01;
        SPI_SCLK = 0;
    }
    return _byte;
}
/**********************************************************************/
/**********************************************************************/