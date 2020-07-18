/*
 * delay.h
 *
 *  Created on: Jul 9, 2019
 *      Author: LapPV
 */

#ifndef _DELAY_H_
#define _DELAY_H_
#define DWT_DELAY_NEWBIE 1
class delay {
private:


public:
	void Init(void);
	void delay_us(uint32_t us);
	void delay_ms(uint32_t ms);
};

#endif /* _DELAY_H_ */
