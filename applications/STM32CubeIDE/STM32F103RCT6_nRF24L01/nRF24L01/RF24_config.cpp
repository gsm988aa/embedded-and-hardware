/*
 * RF24_config.cpp
 *
 *  Created on: Aug 8, 2020
 *      Author: quoclaptk
 */
#include "RF24_config.h"

SPI_t	_SPI;

void RF24_Conf::reg_nordic_spi_begin(void (*_begin)(void))
{
	_SPI.begin = _begin;
}

void RF24_Conf::reg_nordic_spi_transfer(uint8_t (*cbfunc)(uint8_t data))
{
	_SPI.transfer = cbfunc;
}

void RF24_Conf::reg_nordic_ce_func(void (*ce_func)(bool level))
{
	_SPI.ce_pin = ce_func;
}

void RF24_Conf::reg_nordic_csn_fnc(void (*csn_fnc)(bool level))
{
	_SPI.csn_pin = csn_fnc;
}

void RF24_Conf::reg_nordic_irq_fnc(bool (*irq_fnc)(void))
{
	_SPI.irq_pin = irq_fnc;
}
