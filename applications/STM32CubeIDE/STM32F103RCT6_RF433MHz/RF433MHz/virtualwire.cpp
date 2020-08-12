/*
 * virtualwire.cpp
 *
 *  Created on: Aug 12, 2020
 *      Author: quoclaptk
 */

#include <virtualwire.h>

virtual_wire::virtual_wire() {
	// TODO Auto-generated constructor stub

}

virtual_wire::~virtual_wire() {
	// TODO Auto-generated destructor stub
}

uint16_t virtual_wire::vw_crc(uint8_t *ptr, uint8_t count)
{
    uint16_t crc = 0xffff;

    while (count-- > 0)
	crc = _crc_ccitt_update(crc, *ptr++);
    return crc;
}

// Convert a 6 bit encoded symbol into its 4 bit decoded equivalent
uint8_t virtual_wire::vw_symbol_6to4(uint8_t symbol)
{
    uint8_t i;
    uint8_t count;

    // Linear search :-( Could have a 64 byte reverse lookup table?
    // There is a little speedup here courtesy Ralph Doncaster:
    // The shortcut works because bit 5 of the symbol is 1 for the last 8
    // symbols, and it is 0 for the first 8.
    // So we only have to search half the table
    for (i = (symbol>>2) & 8, count=8; count-- ; i++)
	if (symbol == symbols[i]) return i;

    return 0; // Not found
}

// Set the rx pin inverted
void virtual_wire::vw_set_rx_inverted(uint8_t inverted)
{
    vw_rx_inverted = inverted;
}

// Set the ptt pin inverted (low to transmit)
void virtual_wire::vw_set_ptt_inverted(uint8_t inverted)
{
	vw_ptt_inverted = inverted;
}
// Called 8 times per bit period
// Phase locked loop tries to synchronise with the transmitter so that bit
// transitions occur at about the time vw_rx_pll_ramp is 0;
// Then the average is computed over each bit period to deduce the bit value
void virtual_wire::vw_pll()
{
    // Integrate each sample
    if (vw_rx_sample)
	vw_rx_integrator++;

    if (vw_rx_sample != vw_rx_last_sample)
    {
	// Transition, advance if ramp > 80, retard if < 80
	vw_rx_pll_ramp += ((vw_rx_pll_ramp < VW_RAMP_TRANSITION)
			   ? VW_RAMP_INC_RETARD
			   : VW_RAMP_INC_ADVANCE);
	vw_rx_last_sample = vw_rx_sample;
    }
    else
    {
	// No transition
	// Advance ramp by standard 20 (== 160/8 samples)
	vw_rx_pll_ramp += VW_RAMP_INC;
    }
    if (vw_rx_pll_ramp >= VW_RX_RAMP_LEN)
    {
	// Add this to the 12th bit of vw_rx_bits, LSB first
	// The last 12 bits are kept
	vw_rx_bits >>= 1;

	// Check the integrator to see how many samples in this cycle were high.
	// If < 5 out of 8, then its declared a 0 bit, else a 1;
	if (vw_rx_integrator >= 5)
	    vw_rx_bits |= 0x800;

	vw_rx_pll_ramp -= VW_RX_RAMP_LEN;
	vw_rx_integrator = 0; // Clear the integral for the next cycle

	if (vw_rx_active)
	{
	    // We have the start symbol and now we are collecting message bits,
	    // 6 per symbol, each which has to be decoded to 4 bits
	    if (++vw_rx_bit_count >= 12)
	    {
		// Have 12 bits of encoded message == 1 byte encoded
		// Decode as 2 lots of 6 bits into 2 lots of 4 bits
		// The 6 lsbits are the high nybble
		uint8_t this_byte =
		    (vw_symbol_6to4(vw_rx_bits & 0x3f)) << 4
		    | vw_symbol_6to4(vw_rx_bits >> 6);

		// The first decoded byte is the byte count of the following message
		// the count includes the byte count and the 2 trailing FCS bytes
		// REVISIT: may also include the ACK flag at 0x40
		if (vw_rx_len == 0)
		{
		    // The first byte is the byte count
		    // Check it for sensibility. It cant be less than 4, since it
		    // includes the bytes count itself and the 2 byte FCS
		    vw_rx_count = this_byte;
		    if (vw_rx_count < 4 || vw_rx_count > VW_MAX_MESSAGE_LEN)
		    {
			// Stupid message length, drop the whole thing
			vw_rx_active = false;
			vw_rx_bad++;
                        return;
		    }
		}
		vw_rx_buf[vw_rx_len++] = this_byte;

		if (vw_rx_len >= vw_rx_count)
		{
		    // Got all the bytes now
		    vw_rx_active = false;
		    vw_rx_good++;
		    vw_rx_done = true; // Better come get it before the next one starts
		}
		vw_rx_bit_count = 0;
	    }
	}
	// Not in a message, see if we have a start symbol
	else if (vw_rx_bits == 0xb38)
	{
	    // Have start symbol, start collecting message
	    vw_rx_active = true;
	    vw_rx_bit_count = 0;
	    vw_rx_len = 0;
	    vw_rx_done = false; // Too bad if you missed the last message
	}
    }
}

void virtual_wire::vw_setup(uint16_t speed)
{
    vw_digitalWrite_ptt(vw_ptt_inverted);
}
void virtual_wire::vw_tx_start()
{
    vw_tx_index = 0;
    vw_tx_bit = 0;
    vw_tx_sample = 0;

    // Enable the transmitter hardware
    this->vw_digitalWrite_ptt( true ^ vw_ptt_inverted);

    // Next tick interrupt will send the first bit
    vw_tx_enabled = true;
}

// Stop the transmitter, call when all bits are sent
void virtual_wire::vw_tx_stop()
{
    // Disable the transmitter hardware
    vw_digitalWrite_ptt(false ^ vw_ptt_inverted);
    vw_digitalWrite_tx(false);

    // No more ticks for the transmitter
    vw_tx_enabled = false;
}

// Enable the receiver. When a message becomes available, vw_rx_done flag
// is set, and vw_wait_rx() will return.
void virtual_wire::vw_rx_start()
{
    if (!vw_rx_enabled)
    {
		vw_rx_enabled = true;
		vw_rx_active = false; // Never restart a partial message
    }
}

// Disable the receiver
void virtual_wire::vw_rx_stop()
{
    vw_rx_enabled = false;
}

// Return true if the transmitter is active
uint8_t virtual_wire::vw_tx_active()
{
    return vw_tx_enabled;
}

// Wait for the transmitter to become available
// Busy-wait loop until the ISR says the message has been sent
void virtual_wire::vw_wait_tx()
{
    while (vw_tx_enabled)
	;
}

// Wait for the receiver to get a message
// Busy-wait loop until the ISR says a message is available
// can then call vw_get_message()
void virtual_wire::vw_wait_rx()
{
    while (!vw_rx_done)
	;
}
// Wait at most max milliseconds for the receiver to receive a message
// Return the truth of whether there is a message
uint8_t virtual_wire::vw_wait_rx_max(unsigned long milliseconds)
{
    unsigned long start = HAL_GetTick();

    while (!vw_rx_done && ((HAL_GetTick() - start) < milliseconds))
	;
    return vw_rx_done;
}

int8_t virtual_wire::vw_send(uint8_t* buf, uint8_t len)
{
    uint8_t i;
    uint8_t index = 0;
    uint16_t crc = 0xffff;
    uint8_t *p = vw_tx_buf + VW_HEADER_LEN; // start of the message area
    uint8_t count = len + 3; // Added byte count and FCS to get total number of bytes

    if (len > VW_MAX_PAYLOAD)
	return false;

    // Wait for transmitter to become available
    vw_wait_tx();

    // Encode the message length
    crc = _crc_ccitt_update(crc, count);
    p[index++] = symbols[count >> 4];
    p[index++] = symbols[count & 0xf];

    // Encode the message into 6 bit symbols. Each byte is converted into
    // 2 6-bit symbols, high nybble first, low nybble second
    for (i = 0; i < len; i++)
    {
	crc = _crc_ccitt_update(crc, buf[i]);
	p[index++] = symbols[buf[i] >> 4];
	p[index++] = symbols[buf[i] & 0xf];
    }

    // Append the fcs, 16 bits before encoding (4 6-bit symbols after encoding)
    // Caution: VW expects the _ones_complement_ of the CCITT CRC-16 as the FCS
    // VW sends FCS as low byte then hi byte
    crc = ~crc;
    p[index++] = symbols[(crc >> 4)  & 0xf];
    p[index++] = symbols[crc & 0xf];
    p[index++] = symbols[(crc >> 12) & 0xf];
    p[index++] = symbols[(crc >> 8)  & 0xf];

    // Total number of 6-bit symbols to send
    vw_tx_len = index + VW_HEADER_LEN;

    // Start the low level interrupt handler sending symbols
    vw_tx_start();

    return true;
}

// Return true if there is a message available
uint8_t virtual_wire::vw_have_message()
{
    return vw_rx_done;
}

// Get the last message received (without byte count or FCS)
// Copy at most *len bytes, set *len to the actual number copied
// Return true if there is a message and the FCS is OK
uint8_t virtual_wire::vw_get_message(uint8_t* buf, uint8_t* len)
{
    uint8_t rxlen;

    // Message available?
    if (!vw_rx_done)
	return false;

    // Wait until vw_rx_done is set before reading vw_rx_len
    // then remove bytecount and FCS
    rxlen = vw_rx_len - 3;

    // Copy message (good or bad)
    if (*len > rxlen)
	*len = rxlen;
    memcpy(buf, vw_rx_buf + 1, *len);

    vw_rx_done = false; // OK, got that message thanks

    // Check the FCS, return goodness
    return (vw_crc(vw_rx_buf, vw_rx_len) == 0xf0b8); // FCS OK?
}

uint8_t virtual_wire::vw_get_rx_good()
{
    return vw_rx_good;
}

uint8_t virtual_wire::vw_get_rx_bad()
{
    return vw_rx_bad;
}
void virtual_wire::vw_Int_Handler()
{
    if (vw_rx_enabled && !vw_tx_enabled)
	vw_rx_sample = (bool)vw_digitalRead_rx() ^ vw_rx_inverted;

    // Do transmitter stuff first to reduce transmitter bit jitter due
    // to variable receiver processing
    if (vw_tx_enabled && vw_tx_sample++ == 0)
    {
	// Send next bit
	// Symbols are sent LSB first
	// Finished sending the whole message? (after waiting one bit period
	// since the last bit)
	if (vw_tx_index >= vw_tx_len)
	{
	    vw_tx_stop();
	    vw_tx_msg_count++;
	}
	else
	{
	    vw_digitalWrite_tx(vw_tx_buf[vw_tx_index] & (1 << vw_tx_bit++));
	    if (vw_tx_bit >= 6)
	    {
		vw_tx_bit = 0;
		vw_tx_index++;
	    }
	}
    }
    if (vw_tx_sample > 7)
	vw_tx_sample = 0;

    if (vw_rx_enabled && !vw_tx_enabled)
	vw_pll();
}
uint16_t virtual_wire::crc16_update(uint16_t crc, uint8_t a)
{
int i;

crc ^= a;
for (i = 0; i < 8; ++i)
{
	if (crc & 1)
	crc = (crc >> 1) ^ 0xA001;
	else
	crc = (crc >> 1);
}

return crc;
}

uint16_t virtual_wire::crc_xmodem_update (uint16_t crc, uint8_t data)
{
	int i;

	crc = crc ^ ((uint16_t)data << 8);
	for (i=0; i<8; i++)
	{
		if (crc & 0x8000)
			crc = (crc << 1) ^ 0x1021;
		else
			crc <<= 1;
	}

	return crc;
}
uint16_t virtual_wire::_crc_ccitt_update (uint16_t crc, uint8_t data)
{
	data ^= lo8 (crc);
	data ^= data << 4;

	return ((((uint16_t)data << 8) | hi8 (crc)) ^ (uint8_t)(data >> 4) ^ ((uint16_t)data << 3));
}

uint8_t virtual_wire::_crc_ibutton_update(uint8_t crc, uint8_t data)
{
	uint8_t i;

	crc = crc ^ data;
	for (i = 0; i < 8; i++)
	{
		if (crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
		else
			crc >>= 1;
	}

	return crc;
}

void virtual_wire::reg_vw_rx_pin(bool (*rx_pin)(void))
{
	_VWConf.vw_rx_pin = rx_pin;
}

void virtual_wire::reg_vw_tx_pin(void (*tx_pin)(bool value))
{
	_VWConf.vw_tx_pin = tx_pin;
}

void virtual_wire::reg_vw_ptt_pin(void (*ptt_pin)(bool value))
{
	_VWConf.vw_ptt_pin = ptt_pin;
}
