/*
 * ENC28J60Conf.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: quoclaptk
 */

#include <ENC28J60Conf.h>

ENC28J60_Conf::ENC28J60_Conf() {
	// TODO Auto-generated constructor stub

}

ENC28J60_Conf::~ENC28J60_Conf() {
	// TODO Auto-generated destructor stub
}

void ENC28J60_Conf::reg_shield_spi_transfer(uint8_t (*cbfunc)(uint8_t data))
{
	_SPI.transfer = cbfunc;
}
void ENC28J60_Conf::reg_shield_wol_func(void (*wol_func)(bool level))
{
	_SPI.wol_pin = wol_func;
}
void ENC28J60_Conf::reg_shield_cs_fnc(void (*cs_func)(bool level))
{
	_SPI.cs_pin = cs_func;
}
void ENC28J60_Conf::reg_shield_irq_fnc(bool (*reset_func)(void))
{
	_SPI.reset_pin = reset_func;
}
void ENC28J60_Conf::reg_shield_spi_begin(void (*_begin)(void))
{

}
