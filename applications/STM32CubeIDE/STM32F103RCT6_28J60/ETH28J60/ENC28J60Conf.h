/*
 * ENC28J60Conf.h
 *
 *  Created on: Aug 11, 2020
 *      Author: quoclaptk
 */

#ifndef ENC28J60CONF_H_
#define ENC28J60CONF_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdbool.h>

typedef struct SPI
{
	void (*wol_pin)(bool level);
	void (*cs_pin)(bool level);
	bool (*reset_pin)(void);
	uint8_t (*transfer)(uint8_t data);
	void (*begin)(void);
}SPI_t;

class ENC28J60_Conf {
public:
	SPI_t _SPI;
	ENC28J60_Conf();
	virtual ~ENC28J60_Conf();
	void reg_shield_spi_transfer(uint8_t (*cbfunc)(uint8_t data));
	void reg_shield_wol_func(void (*ce_func)(bool level));
	void reg_shield_cs_fnc(void (*csn_fnc)(bool level));
	void reg_shield_irq_fnc(bool (*irq_fnc)(void));
	void reg_shield_spi_begin(void (*_begin)(void));
};

#endif /* ENC28J60CONF_H_ */
