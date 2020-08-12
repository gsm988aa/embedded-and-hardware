/*
 * virtualwire.h
 *
 *  Created on: Aug 12, 2020
 *      Author: quoclaptk
 */

#ifndef VIRTUALWIRE_H_
#define VIRTUALWIRE_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include "VirtualWire_Config.h"
typedef struct VWConf
{
	bool (*vw_rx_pin)(void);
	void (*vw_tx_pin)(bool level);
	void (*vw_ptt_pin)(bool level);
}VWConf_t;
#define lo8(x) ((x)&0xff)
#define hi8(x) ((x)>>8)
#define vw_digitalRead_rx() _VWConf.vw_rx_pin
#define vw_digitalWrite_tx(value) _VWConf.vw_tx_pin(value)
#define vw_digitalWrite_ptt(value) _VWConf.vw_ptt_pin(value)
#define vw_event_tx_done()
#define vw_event_rx_done(message, length)
#define vw_event_rx_byte_internal(byte)
#define VW_HEADER_LEN 8
#define VW_MAX_MESSAGE_LEN 80
#define VW_RX_SAMPLES_PER_BIT 8
/// The maximum payload length
#define VW_MAX_PAYLOAD VW_MAX_MESSAGE_LEN-3
/// The size of the receiver ramp. Ramp wraps modulo this number
#define VW_RX_RAMP_LEN 160
/// Internal ramp adjustment parameter
#define VW_RAMP_INC (VW_RX_RAMP_LEN/VW_RX_SAMPLES_PER_BIT)
/// Internal ramp adjustment parameter
#define VW_RAMP_TRANSITION VW_RX_RAMP_LEN/2
/// Internal ramp adjustment parameter
#define VW_RAMP_ADJUST 9
/// Internal ramp adjustment parameter
#define VW_RAMP_INC_RETARD (VW_RAMP_INC-VW_RAMP_ADJUST)
/// Internal ramp adjustment parameter
#define VW_RAMP_INC_ADVANCE (VW_RAMP_INC+VW_RAMP_ADJUST)

class virtual_wire {
private:
	uint8_t vw_tx_buf[(VW_MAX_MESSAGE_LEN * 2) + VW_HEADER_LEN] = {0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x38, 0x2c};
	// Number of symbols in vw_tx_buf to be sent;
	uint8_t vw_tx_len = 0;
	// Index of the next symbol to send. Ranges from 0 to vw_tx_len
	uint8_t vw_tx_index = 0;
	// Bit number of next bit to send
	uint8_t vw_tx_bit = 0;
	// Sample number for the transmitter. Runs 0 to 7 during one bit interval
	uint8_t vw_tx_sample = 0;
	// Flag to indicated the transmitter is active
	volatile uint8_t vw_tx_enabled = 0;
	// Total number of messages sent
	uint16_t vw_tx_msg_count = 0;
	uint8_t vw_ptt_inverted = 0;
	uint8_t vw_rx_inverted = 0;
	// Current receiver sample
	uint8_t vw_rx_sample = 0;
	// Last receiver sample
	uint8_t vw_rx_last_sample = 0;
	// PLL ramp, varies between 0 and VW_RX_RAMP_LEN-1 (159) over
	// VW_RX_SAMPLES_PER_BIT (8) samples per nominal bit time.
	// When the PLL is synchronised, bit transitions happen at about the
	// 0 mark.
	uint8_t vw_rx_pll_ramp = 0;
	// This is the integrate and dump integral. If there are <5 0 samples in the PLL cycle
	// the bit is declared a 0, else a 1
	uint8_t vw_rx_integrator = 0;
	// Flag indicates if we have seen the start symbol of a new message and are
	// in the processes of reading and decoding it
	uint8_t vw_rx_active = 0;
	// Flag to indicate that a new message is available
	volatile uint8_t vw_rx_done = 0;
	// Flag to indicate the receiver PLL is to run
	uint8_t vw_rx_enabled = 0;
	// Last 12 bits received, so we can look for the start symbol
	uint16_t vw_rx_bits = 0;
	// How many bits of message we have received. Ranges from 0 to 12
	uint8_t vw_rx_bit_count = 0;
	// The incoming message buffer
	uint8_t vw_rx_buf[VW_MAX_MESSAGE_LEN];
	// The incoming message expected length
	uint8_t vw_rx_count = 0;
	// The incoming message buffer length received so far
	volatile uint8_t vw_rx_len = 0;
	// Number of bad messages received and dropped due to bad lengths
	uint8_t vw_rx_bad = 0;
	// Number of good messages received
	uint8_t vw_rx_good = 0;
	// 4 bit to 6 bit symbol converter table
	// Used to convert the high and low nybbles of the transmitted data
	// into 6 bit symbols for transmission. Each 6-bit symbol has 3 1s and 3 0s
	// with at most 3 consecutive identical bits
	uint8_t symbols[16] =
	{
	    0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c,
	    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
	};
public:
	virtual_wire();
	virtual ~virtual_wire();
	VWConf_t _VWConf;
	void reg_vw_rx_pin(bool (*rx_pin)(void));
	void reg_vw_tx_pin(void (*tx_pin)(bool value));
	void reg_vw_ptt_pin(void (*ptt_pin)(bool value));
	uint16_t vw_crc(uint8_t *ptr, uint8_t count);
	void vw_set_rx_inverted(uint8_t inverted);
	uint8_t vw_symbol_6to4(uint8_t symbol);
	void vw_set_ptt_inverted(uint8_t inverted);
	void vw_pll();
	void vw_tx_start();
	void vw_tx_stop();
	void vw_rx_start();
	void vw_rx_stop();
	void vw_setup(uint16_t speed);
	uint8_t vw_tx_active();
	void vw_wait_tx();
	void vw_wait_rx();
	uint8_t vw_wait_rx_max(unsigned long milliseconds);
	int8_t vw_send(uint8_t* buf, uint8_t len);
	uint8_t vw_have_message();
	uint8_t vw_get_message(uint8_t* buf, uint8_t* len);
	uint8_t vw_get_rx_good();
	uint8_t vw_get_rx_bad();
	void vw_Int_Handler();
	uint16_t crc16_update(uint16_t crc, uint8_t a);
	uint16_t crc_xmodem_update (uint16_t crc, uint8_t data);
	uint16_t _crc_ccitt_update (uint16_t crc, uint8_t data);
	uint8_t _crc_ibutton_update(uint8_t crc, uint8_t data);
};

#endif /* VIRTUALWIRE_H_ */
