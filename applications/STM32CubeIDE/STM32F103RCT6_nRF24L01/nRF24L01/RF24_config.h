
/*
 Copyright (C) 
    2011            J. Coliz <maniacbug@ymail.com>
    2015-2019 TMRh20
    2015            spaniakos <spaniakos@gmail.com>
    2015            nerdralph
    2015            zador-blood-stained
    2016            akatran
    2017-2019 Avamander <avamander@gmail.com>
    2019            IkpeohaGodson
                 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdbool.h>

/*** USER DEFINES:    ***/    
#define FAILURE_HANDLING
#define PROGMEM
#define _BV(x) (1<<(x))
#define pgm_read_word(p) (*(p))
#define pgm_read_byte(p) (*(p))
#define pgm_read_ptr(p) (*(p))
//#define SERIAL_DEBUG
//#define MINIMAL
//#define SPI_UART    // Requires library from https://github.com/TMRh20/Sketches/tree/master/SPI_UART
//#define SOFTSPI     // Requires library from https://github.com/greiman/DigitalIO
    
/**********************/
#define rf24_max(a,b)  (a>b?a:b)
#define rf24_min(a,b)  (a<b?a:b)
#define RF24_SPI_SPEED (10000000)

typedef struct SPI
{
	void (*ce_pin)(bool level);
	void (*csn_pin)(bool level);
	bool (*irq_pin)(void);
	uint8_t (*transfer)(uint8_t data);
	void (*begin)(void);
}SPI_t;

class RF24_Conf
{
public:
	void reg_nordic_spi_transfer(uint8_t (*cbfunc)(uint8_t data));
	void reg_nordic_ce_func(void (*ce_func)(bool level));
	void reg_nordic_csn_fnc(void (*csn_fnc)(bool level));
	void reg_nordic_irq_fnc(bool (*irq_fnc)(void));
	void reg_nordic_spi_begin(void (*_begin)(void));
};
#endif // __RF24_CONFIG_H__
