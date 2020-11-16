/*
 * wizchip_conf.cpp
 *
 *  Created on: Oct 31, 2020
 *      Author: quoclaptk
 */

#include <wizchip_conf.h>

SPI_t SPI;

void RegFunc::reg_spi_begin(void (*_begin)(void))
{
	SPI.begin = _begin;
}

void RegFunc::reg_spi_transfer(uint8_t (*cbfunc)(uint8_t data))
{
	SPI.transfer = cbfunc;
}

void RegFunc::reg_ce_func(void (*ce_func)(bool level))
{
	SPI.ce_pin = ce_func;
}

void RegFunc::reg_cs_fnc(void (*cs_fnc)(bool level))
{
	SPI.cs_pin = cs_fnc;
}

void RegFunc::reg_irq_fnc(bool (*irq_fnc)(void))
{
	SPI.irq_pin = irq_fnc;
}
/******************************************************************************/

wizchip_conf::wizchip_conf() {
	// TODO Auto-generated constructor stub

}

wizchip_conf::~wizchip_conf() {
	// TODO Auto-generated destructor stub
}

