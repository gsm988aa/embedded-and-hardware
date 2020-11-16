/*
 * wizchip_conf.h
 *
 *  Created on: Oct 31, 2020
 *      Author: quoclaptk
 */

#ifndef _WIZCHIP_CONF_H_
#define _WIZCHIP_CONF_H_

#include "main.h"

/******************************************************************************/
typedef struct _SPI
{
	void (*ce_pin)(bool level);
	void (*cs_pin)(bool level);
	bool (*irq_pin)(void);
	uint8_t (*transfer)(uint8_t data);
	void (*begin)(void);
}SPI_t;
/******************************************************************************/
class RegFunc
{
public:
	void reg_spi_transfer(uint8_t (*cbfunc)(uint8_t data));
	void reg_ce_func(void (*ce_func)(bool level));
	void reg_cs_fnc(void (*cs_fnc)(bool level));
	void reg_irq_fnc(bool (*irq_fnc)(void));
	void reg_spi_begin(void (*_begin)(void));
};
/******************************************************************************/

class wizchip_conf {
public:
	wizchip_conf();
	virtual ~wizchip_conf();
};

#endif /* _WIZCHIP_CONF_H_ */
