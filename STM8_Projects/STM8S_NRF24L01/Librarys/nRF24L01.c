/******************************************************************************/
#include "nRF24L01.h"
/******************************************************************************/
//*** Variables declaration ***//
#define MAX(x, y)(((x) > (y)) ? (x) : (y))
#define MIN(x, y)(((x) < (y)) ? (x) : (y))
#define _BOOL(x)(((x) > 0) ? 1 : 0)
/******************************************************************************/
//*** Library variables ***//
static uint64_t pipe0_reading_address;
static bool ack_payload_available; /* Whether there is an ack payload waiting */
static uint8_t ack_payload_length; /* Dynamic size of pending ack payload. */
static uint8_t payload_size; /* Fixed size of payloads */
bool dynamic_payloads_enabled; /* Whether dynamic payloads are enabled. */
static bool p_variant; /* False for RF24L01 and true for RF24L01P */
bool wide_band; /* 2Mbs data rate in use? */
/******************************************************************************/
/*** NRF24L01 pins and handles ***/
static GPIO_TypeDef*        nrf24_PORT;
static GPIO_Pin_TypeDef     nrf24_CSN_PIN;
static GPIO_Pin_TypeDef     nrf24_CE_PIN;
/******************************************************************************/
/* Debugging UART handle */
//static UART_HandleTypeDef nrf24_huart;
/******************************************************************************/
void NRF24_csn(uint8_t state)
{
    if(state)GPIO_WriteHigh(nrf24_PORT, nrf24_CSN_PIN);
    else GPIO_WriteLow(nrf24_PORT, nrf24_CSN_PIN);
}
/******************************************************************************/
void NRF24_ce(uint8_t state)
{
    if(state)GPIO_WriteHigh(nrf24_PORT, nrf24_CE_PIN);
    else GPIO_WriteLow(nrf24_PORT, nrf24_CE_PIN);
}
/******************************************************************************/
bool NRF24L01_Check(void)
{
    uint8_t buf[5]={0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    uint8_t buf1[5], i=0;
    NRF24_write_registerN(0x20 + 0x10, buf, 5);
    NRF24_read_registerN(CMD_R_REGISTER + 0x10, buf1, 5);
    for(i=0; i<5; i++)if(buf1[i]!=0xA5)return false;
    return true;
}
/******************************************************************************/
static uint8_t nRF24L01_SPI_RW(uint8_t data)
{
    while(SPI_GetFlagStatus( SPI_FLAG_TXE) == RESET);
    SPI_SendData(data);
    while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    return SPI_ReceiveData();  
}
/******************************************************************************/
uint8_t NRF24_read_register(uint8_t reg)
{
    uint8_t retData;
    //Put CSN low
    NRF24_csn(0);
    //Transmit register address
    nRF24L01_SPI_RW(reg & 0x1f);
    //Receive data
    retData = nRF24L01_SPI_RW(0xff);
    //Bring CSN high
    NRF24_csn(1);
    return retData;
}
/******************************************************************************/
/* Read multiple bytes register */
void NRF24_read_registerN(uint8_t reg, uint8_t* buf, uint8_t len)
{
    //Put CSN low
    NRF24_csn(0);
    //Transmit register address
    nRF24L01_SPI_RW(reg & 0x1f);
    //Receive data
    for(uint8_t i=0; i<len; i++)
    {
        buf[i] = nRF24L01_SPI_RW(0xff);
    }
    //Bring CSN high
    NRF24_csn(1);
}
/******************************************************************************/
/* Write single byte register */
void NRF24_write_register(uint8_t reg, uint8_t value)
{
    //Put CSN low
    NRF24_csn(0);
    //Transmit register address and data
    nRF24L01_SPI_RW(reg|0x20);
    nRF24L01_SPI_RW(value);
    //Bring CSN high
    NRF24_csn(1);
}
/******************************************************************************/
/* Write multipl bytes register */
void NRF24_write_registerN(uint8_t reg, const uint8_t* buf, uint8_t len) 
{
    //Put CSN low
    NRF24_csn(0);
    //Transmit register address and data
    nRF24L01_SPI_RW(reg|0x20);
    for(uint8_t i=0; i<len; i++)
    {
        nRF24L01_SPI_RW(buf[i]);
    }
    //Bring CSN high
    NRF24_csn(1);
}
/******************************************************************************/
/* Write transmit payload */
void NRF24_write_payload(const uint8_t* buf, uint8_t len)
{
    //Bring CSN low
    NRF24_csn(0);
    //Send Write Tx payload command followed by pbuf data
    nRF24L01_SPI_RW(CMD_W_TX_PAYLOAD);
    for(uint8_t i=0; i<len; i++)
    {
        nRF24L01_SPI_RW(buf[i]);
    }
    //Bring CSN high
    NRF24_csn(1);
}
/******************************************************************************/
/* Read receive payload */
void NRF24_read_payload(uint8_t* buf, uint8_t len)
{
    //Get data length using payload size
    uint8_t data_len = MIN(len, NRF24_getPayloadSize());
    //Read data from Rx payload buffer
    NRF24_csn(0);
    nRF24L01_SPI_RW(CMD_R_RX_PAYLOAD);
    for(uint8_t i=0; i<data_len; i++)
    {
        buf[i] = nRF24L01_SPI_RW(0);
    }
    NRF24_csn(1);
}
/******************************************************************************/
/* Flush Rx buffer */
void NRF24_flush_tx(void)
{
    NRF24_write_register(CMD_FLUSH_TX, 0xFF);
}
/******************************************************************************/
/* Flush Rx buffer */
void NRF24_flush_rx(void)
{
    NRF24_write_register(CMD_FLUSH_RX, 0xFF);
}
/******************************************************************************/
/* Get status register value */
uint8_t NRF24_get_status(void)
{
    uint8_t statReg;
    statReg = NRF24_read_register(REG_STATUS);
    return statReg;
}
/******************************************************************************/
void NRF24_toggle_features(void)
{
    NRF24_csn(0);
    nRF24L01_SPI_RW(CMD_ACTIVATE);
    nRF24L01_SPI_RW(0x73);
    NRF24_csn(1);
}
/******************************************************************************/
/* Begin function */
bool NRF24_begin(GPIO_TypeDef* nrf24PORT, GPIO_Pin_TypeDef nrfCSN_Pin, GPIO_Pin_TypeDef nrfCE_Pin) 
{
    uint8_t setup = 0;
    uint8_t pipeAddrVar[6];
    /* Copy Pins and Port variables */
    nrf24_PORT = nrf24PORT;
    nrf24_CSN_PIN = nrfCSN_Pin;
    nrf24_CE_PIN = nrfCE_Pin;
    /* Put pins to idle state */
    NRF24_csn(1);
    NRF24_ce(0);
    /* 5 ms initial delay */
    delay_ms(5);
    /* Soft Reset Registers default values */
    while(!NRF24L01_Check());
    NRF24_write_register(REG_CONFIG, 0x08);
    NRF24_write_register(REG_EN_AA, 0x3f);
    NRF24_write_register(REG_EN_RXADDR, 0x03);
    NRF24_write_register(REG_SETUP_AW, 0x03);
    NRF24_write_register(REG_SETUP_RETR, 0x03);
    NRF24_write_register(REG_RF_CH, 0x02);
    NRF24_write_register(REG_RF_SETUP, 0x0f);
    NRF24_write_register(REG_STATUS, 0x0e);
    NRF24_write_register(REG_OBSERVE_TX, 0x00);
    NRF24_write_register(REG_CD, 0x00);
    pipeAddrVar[4]=0xE7; pipeAddrVar[3]=0xE7; pipeAddrVar[2]=0xE7; pipeAddrVar[1]=0xE7; pipeAddrVar[0]=0xE7;
    NRF24_write_registerN(REG_RX_ADDR_P0, pipeAddrVar, 5);
    pipeAddrVar[4]=0xC2; pipeAddrVar[3]=0xC2; pipeAddrVar[2]=0xC2; pipeAddrVar[1]=0xC2; pipeAddrVar[0]=0xC2;
    NRF24_write_registerN(REG_RX_ADDR_P1, pipeAddrVar, 5);
    NRF24_write_register(REG_RX_ADDR_P2, 0xC3);
    NRF24_write_register(REG_RX_ADDR_P3, 0xC4);
    NRF24_write_register(REG_RX_ADDR_P4, 0xC5);
    NRF24_write_register(REG_RX_ADDR_P5, 0xC6);
    pipeAddrVar[4]=0xE7; pipeAddrVar[3]=0xE7; pipeAddrVar[2]=0xE7; pipeAddrVar[1]=0xE7; pipeAddrVar[0]=0xE7;
    NRF24_write_registerN(REG_TX_ADDR, pipeAddrVar, 5);
    NRF24_write_register(REG_RX_PW_P0, 0);
    NRF24_write_register(REG_RX_PW_P1, 0);
    NRF24_write_register(REG_RX_PW_P2, 0);
    NRF24_write_register(REG_RX_PW_P3, 0);
    NRF24_write_register(REG_RX_PW_P4, 0);
    NRF24_write_register(REG_RX_PW_P5, 0);
    NRF24_toggle_features();
    NRF24_write_register(REG_FEATURE, 0);
    NRF24_write_register(REG_DYNPD, 0);
    NRF24_setCRCLength(RF24_CRC_16);
    NRF24_setRetries(5, 15);
    if(NRF24_setDataRate(RF24_250KBPS))p_variant = true;
    setup = NRF24_read_register(REG_RF_SETUP);
    NRF24_setDataRate(RF24_1MBPS);
    dynamic_payloads_enabled = false;
    NRF24_write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    NRF24_setPayloadSize(32);
    NRF24_setChannel(76);
    NRF24_flush_tx();
    NRF24_flush_rx();
    NRF24_powerUp();
    NRF24_write_register(REG_CONFIG, (NRF24_read_register(REG_CONFIG))&(~_BV(BIT_PRIM_RX)));
    return ((setup != 0)&&(setup != 0xff));
}
/******************************************************************************/
/* Listen on open pipes for reading (Must call NRF24_openReadingPipe() first) */
void NRF24_startListening(void)
{
    //Power up and set to RX mode
    NRF24_write_register(REG_CONFIG, NRF24_read_register(REG_CONFIG)|_BV(BIT_PRIM_RX));
    NRF24_write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    //Restore pipe 0 address if exists
    if(pipe0_reading_address)NRF24_write_registerN(REG_RX_ADDR_P0, (uint8_t*)(&pipe0_reading_address), 5);
    //Flush buffers
    NRF24_flush_tx();
    NRF24_flush_rx();
    //Set CE HIGH to start listenning
    NRF24_ce(1);
    //Wait for 130 uSec for the radio to come on
    delay_us(150);
}
/******************************************************************************/
/* Stop listening (essential before any write operation) */
void NRF24_stopListening(void)
{
    NRF24_ce(0);
    NRF24_flush_tx();
    NRF24_flush_rx();
}
/******************************************************************************/
/* Write(Transmit data), returns true if successfully sent */
bool NRF24_write(const void* buf, uint8_t len)
{
    bool retStatus;
    uint8_t observe_tx;
    uint8_t status;
    bool tx_ok, tx_fail;
    uint32_t sent_at = 10;
    //Start writing
    NRF24_resetStatus();
    NRF24_startWrite(buf, len);
    //Data monitor
    do 
    {
        NRF24_read_registerN(REG_OBSERVE_TX, &observe_tx, 1);
        //Get status register
        status = NRF24_get_status();
    }while(!(status&(_BV(BIT_TX_DS)|_BV(BIT_MAX_RT)))&&(sent_at--));
    NRF24_whatHappened(&tx_ok, &tx_fail, &ack_payload_available);
    retStatus = tx_ok;
    if(ack_payload_available)
    {
        ack_payload_length = NRF24_getDynamicPayloadSize();
    }
    //Power down
    NRF24_available();
    NRF24_flush_tx();
    return retStatus;
}
/******************************************************************************/
/* Check for available data to read */
bool NRF24_available(void)
{
    return NRF24_availablePipe(NULL);
}
/******************************************************************************/
/* Read received data */
bool NRF24_read(void* buf, uint8_t len)
{
    NRF24_read_payload(buf, len);
    uint8_t rxStatus = NRF24_read_register(REG_FIFO_STATUS)&_BV(BIT_RX_EMPTY);
    NRF24_flush_rx();
    NRF24_getDynamicPayloadSize();
    return rxStatus;
}
/******************************************************************************/
/* Open Tx pipe for writing (Cannot perform this while Listenning, has to call NRF24_stopListening) */
void NRF24_openWritingPipe(uint8_t* address)
{
    NRF24_write_registerN(REG_RX_ADDR_P0, address, 5);
    NRF24_write_registerN(REG_TX_ADDR, address, 5);
    NRF24_write_register(REG_RX_PW_P0, MIN(payload_size, MAX_PAYLOAD_SIZE));
}
/******************************************************************************/
/* Open reading pipe */
void NRF24_openReadingPipe(uint8_t number, uint64_t address)
{
    if(number == 0)pipe0_reading_address = address;
    if(number <= 6)
    {
        if(number < 2)
        {
            //Address width is 5 bytes
            NRF24_write_registerN(NRF24_ADDR_REGS[number], (uint8_t*)(&address), 5);
        }
        else
        {
            NRF24_write_registerN(NRF24_ADDR_REGS[number], (uint8_t*)(&address), 1);
        }
        //Write payload size
        NRF24_write_register(RF24_RX_PW_PIPE[number], payload_size);
        //Enable pipe
        NRF24_write_register(REG_EN_RXADDR, NRF24_read_register(REG_EN_RXADDR)|_BV(number));
    }
}
/******************************************************************************/
/* set transmit retries (rf24_Retries_e) and delay */
void NRF24_setRetries(uint8_t delay, uint8_t count)
{
    NRF24_write_register(REG_SETUP_RETR, ((delay&0xf)<<BIT_ARD)|((count&0xf)<<BIT_ARC));
}
/******************************************************************************/
/* Set RF channel frequency */
void NRF24_setChannel(uint8_t channel)
{
    NRF24_write_register(REG_RF_CH, MIN(channel, MAX_CHANNEL));
}
/******************************************************************************/
/* Set payload size */
void NRF24_setPayloadSize(uint8_t size)
{
    payload_size = MIN(size, MAX_PAYLOAD_SIZE);
}
/******************************************************************************/
/* Get payload size */
uint8_t NRF24_getPayloadSize(void)
{
    return payload_size;
}
/******************************************************************************/
/* Get dynamic payload size, of latest packet received */
uint8_t NRF24_getDynamicPayloadSize(void)
{
    return NRF24_read_register(CMD_R_RX_PL_WID);
}
/******************************************************************************/
/* Enable payload on Ackknowledge packet */
void NRF24_enableAckPayload(void)
{
    //Need to enable dynamic payload and Ack payload together
    NRF24_write_register(REG_FEATURE, NRF24_read_register(REG_FEATURE)|_BV(BIT_EN_ACK_PAY)|_BV(BIT_EN_DPL));
    if (!NRF24_read_register(REG_FEATURE))
    {
        NRF24_toggle_features();
        NRF24_write_register(REG_FEATURE, NRF24_read_register(REG_FEATURE)|_BV(BIT_EN_ACK_PAY)|_BV(BIT_EN_DPL));
    }
    // Enable dynamic payload on pipes 0 & 1
    NRF24_write_register(REG_DYNPD, NRF24_read_register(REG_DYNPD)|_BV(BIT_DPL_P1)|_BV(BIT_DPL_P0));
}
/******************************************************************************/
/* Enable dynamic payloads */
void NRF24_enableDynamicPayloads(void)
{
    //Enable dynamic payload through FEATURE register
    NRF24_write_register(REG_FEATURE, NRF24_read_register(REG_FEATURE)|_BV(BIT_EN_DPL));
    if(!NRF24_read_register(REG_FEATURE)) 
    {
        NRF24_toggle_features();
        NRF24_write_register(REG_FEATURE, NRF24_read_register(REG_FEATURE)|_BV(BIT_EN_DPL));
    }
    //Enable Dynamic payload on all pipes
    NRF24_write_register(REG_DYNPD, NRF24_read_register(REG_DYNPD)|_BV(BIT_DPL_P5)|_BV(BIT_DPL_P4)|_BV(BIT_DPL_P3)|_BV(BIT_DPL_P2)|_BV(BIT_DPL_P1)|_BV(BIT_DPL_P0));
    dynamic_payloads_enabled = true;
}
/******************************************************************************/
/* Disable dynamic payloads */
void NRF24_disableDynamicPayloads(void)
{
    NRF24_write_register(REG_FEATURE, NRF24_read_register(REG_FEATURE)&(~_BV(BIT_EN_DPL)));
    //Disable for all pipes
    NRF24_write_register(REG_DYNPD, 0);
    dynamic_payloads_enabled = false;
}
/******************************************************************************/
/* Check if module is NRF24L01+ or normal module */
bool NRF24_isNRF_Plus(void)
{
    return p_variant;
}
/******************************************************************************/
/* Set Auto Ack for all */
void NRF24_setAutoAck(bool enable)
{
    if(enable)NRF24_write_register(REG_EN_AA, 0x3F);
    else NRF24_write_register(REG_EN_AA, 0x00);
}
/******************************************************************************/
/* Set Auto Ack for certain pipe */
void NRF24_setAutoAckPipe(uint8_t pipe, bool enable)
{
    uint8_t en_aa;
    if(pipe <= 6)
    {
        en_aa = NRF24_read_register(REG_EN_AA);
        if(enable)en_aa |= _BV(pipe);
        else en_aa &= ~_BV(pipe);
        NRF24_write_register(REG_EN_AA, en_aa);
    }
}
/******************************************************************************/
/* Set transmit power level */
void NRF24_setPALevel(rf24_pa_dbm_e level) 
{
    uint8_t setup = NRF24_read_register(REG_RF_SETUP);
    setup &= (~(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH)));
    // switch uses RAM (evil!)
    if (level == RF24_PA_MAX)setup|=(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    else if (level == RF24_PA_HIGH)setup|=_BV(RF_PWR_HIGH);
    else if (level == RF24_PA_LOW)setup|=_BV(RF_PWR_LOW);
    else if (level == RF24_PA_MIN) {/* nothing */}
    else if (level == RF24_PA_ERROR)setup|=(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    NRF24_write_register(REG_RF_SETUP, setup);
}
/******************************************************************************/
/* Get transmit power level */
rf24_pa_dbm_e NRF24_getPALevel(void) 
{
    uint8_t power = NRF24_read_register(REG_RF_SETUP)&(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    rf24_pa_dbm_e result = RF24_PA_ERROR;
    // switch uses RAM (evil!)
    if (power == (_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH)))result = RF24_PA_MAX;
    else if(power == _BV(RF_PWR_HIGH))result = RF24_PA_HIGH;
    else if(power == _BV(RF_PWR_LOW))result = RF24_PA_LOW;
    else result = RF24_PA_MIN;
    return result;
}
/******************************************************************************/
/* Set data rate (250 Kbps, 1Mbps, 2Mbps) */
bool NRF24_setDataRate(rf24_datarate_e speed)
{
    bool result = false;
    uint8_t setup = NRF24_read_register(REG_RF_SETUP);
    // HIGH and LOW '00' is 1Mbs - our default
    wide_band = false;
    setup &= (~(_BV(RF_DR_LOW)|_BV(RF_DR_HIGH)));
    if(speed == RF24_250KBPS)
    {
        // Must set the RF_DR_LOW to 1; RF_DR_HIGH (used to be RF_DR) is already 0
        // Making it '10'.
        wide_band = false;
        setup |= _BV(RF_DR_LOW);
    }
    else
    {
        // Set 2Mbs, RF_DR (RF_DR_HIGH) is set 1
        // Making it '01'
        if (speed == RF24_2MBPS)
        {
            wide_band = true;
            setup |= _BV(RF_DR_HIGH);
        } 
        else 
        {
            // 1Mbs
            wide_band = false;
        }
    }
    NRF24_write_register(REG_RF_SETUP, setup);
    // Verify our result
    if (NRF24_read_register(REG_RF_SETUP) == setup)result = true;
    else wide_band = false;
    return result;
}
/******************************************************************************/
/* Get data rate */
rf24_datarate_e NRF24_getDataRate(void)
{
    rf24_datarate_e result;
    uint8_t dr = NRF24_read_register(REG_RF_SETUP)&(_BV(RF_DR_LOW)|_BV(RF_DR_HIGH));
    if(dr == _BV(RF_DR_LOW))result = RF24_250KBPS;
    else if(dr == _BV(RF_DR_HIGH))result = RF24_2MBPS;
    else result = RF24_1MBPS;
    return result;
}
/******************************************************************************/
/* Set crc length (disable, 8-bits or 16-bits) */
void NRF24_setCRCLength(rf24_crclength_e length)
{
    uint8_t config = NRF24_read_register(REG_CONFIG)&(~(_BV(BIT_CRCO)|_BV(BIT_EN_CRC)));
    // switch uses RAM
    if(length == RF24_CRC_8)config |= _BV(BIT_EN_CRC);
    else 
    {
        config |= _BV(BIT_EN_CRC);
        config |= _BV(BIT_CRCO);
    }
    NRF24_write_register(REG_CONFIG, config);
}
/******************************************************************************/
/* Get CRC length */
rf24_crclength_e NRF24_getCRCLength(void)
{
    rf24_crclength_e result = RF24_CRC_DISABLED;
    uint8_t config = NRF24_read_register(REG_CONFIG)&(_BV(BIT_CRCO)|_BV(BIT_EN_CRC));
    if(config&_BV(BIT_EN_CRC))
    {
        if(config & _BV(BIT_CRCO))result = RF24_CRC_16;
        else result = RF24_CRC_8;
    }
    return result;
}
/******************************************************************************/
/* Disable CRC */
void NRF24_disableCRC(void)
{
    uint8_t disable = NRF24_read_register(REG_CONFIG)&(~_BV(BIT_EN_CRC));
    NRF24_write_register(REG_CONFIG, disable);
}
/******************************************************************************/
/* power up */
void NRF24_powerUp(void)
{
    NRF24_write_register(REG_CONFIG, NRF24_read_register(REG_CONFIG)|_BV(BIT_PWR_UP));
}
/******************************************************************************/
/* power down */
void NRF24_powerDown(void)
{
    NRF24_write_register(REG_CONFIG, NRF24_read_register(REG_CONFIG)&(~_BV(BIT_PWR_UP)));
}
/******************************************************************************/
/* Check if data are available and on which pipe (Use this for multiple rx pipes) */
bool NRF24_availablePipe(uint8_t* pipe_num)
{
    uint8_t status = NRF24_get_status();
    bool result = (status&_BV(BIT_RX_DR));
    if(result)
    {
        // If the caller wants the pipe number, include that
        if (pipe_num)
        {
            *pipe_num = (status >> BIT_RX_P_NO)&0x07;
        }
        // Clear the status bit
        NRF24_write_register(REG_STATUS, _BV(BIT_RX_DR));
        // Handle ack payload receipt
        if (status & _BV(BIT_TX_DS))
        {
            NRF24_write_register(REG_STATUS, _BV(BIT_TX_DS));
        }
    }
    return result;
}
/******************************************************************************/
/* Start write (for IRQ mode) */
void NRF24_startWrite(const void* buf, uint8_t len)
{
    // Transmitter power-up
    NRF24_write_register(REG_CONFIG, (NRF24_read_register(REG_CONFIG)|_BV(BIT_PWR_UP))&(~_BV(BIT_PRIM_RX)));
    delay_us(150);
    // Send the payload
    NRF24_write_payload(buf, len);
    // Enable Tx for 15usec
    NRF24_ce(1);
    delay_us(15);
    NRF24_ce(0);
}
/******************************************************************************/
/* Write acknowledge payload */
void NRF24_writeAckPayload(uint8_t pipe, const uint8_t* buf, uint8_t len)
{
    const uint8_t *current = (uint8_t*)buf;
    uint8_t data_len = MIN(len, MAX_PAYLOAD_SIZE);
    NRF24_csn(0);
    NRF24_write_registerN(CMD_W_ACK_PAYLOAD|(pipe & 0x7), current, data_len);
    NRF24_csn(1);
}
/******************************************************************************/
/* Check if an Ack payload is available */
bool NRF24_isAckPayloadAvailable(void)
{
    bool result = ack_payload_available;
    ack_payload_available = false;
    return result;
}
/******************************************************************************/
/* Check interrupt flags */
void NRF24_whatHappened(bool* tx_ok, bool* tx_fail, bool* rx_ready)
{
    uint8_t status = NRF24_get_status();
    *tx_ok = 0;
    NRF24_write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    // Report to the user what happened
    *tx_ok = status & _BV(BIT_TX_DS);
    *tx_fail = status & _BV(BIT_MAX_RT);
    *rx_ready = status & _BV(BIT_RX_DR);
}
/******************************************************************************/
/* Test if there is a carrier on the previous listenning period (useful to check for intereference) */
bool NRF24_testCarrier(void)
{
    return NRF24_read_register(REG_CD)&0x01;
}
/******************************************************************************/
/* Test if a signal carrier exists (=> -64dB), only for NRF24L01+ */
bool NRF24_testRPD(void)
{
    return NRF24_read_register(REG_RPD)&0x01;
}
/******************************************************************************/
/* Reset Status */
void NRF24_resetStatus(void)
{
    NRF24_write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
}
/******************************************************************************/
/* Get AckPayload Size */
uint8_t NRF24_GetAckPayloadSize(void)
{
    return ack_payload_length;
}
/******************************************************************************/
/*void printRadioSettings(void)
{
    uint8_t reg8Val;
    char uartTxBuf[100];
    sprintf(uartTxBuf, "\r\n**********************************************\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //a) Get CRC settings - Config Register
    reg8Val = NRF24_read_register(0x00);
    if (reg8Val & (1 << 3)) {
        if (reg8Val & (1 << 2)) sprintf(uartTxBuf, "CRC:\r\n		Enabled, 2 Bytes \r\n");
        else sprintf(uartTxBuf, "CRC:\r\n		Enabled, 1 Byte \r\n");
    } else {
        sprintf(uartTxBuf, "CRC:\r\n		Disabled \r\n");
    }
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //b) AutoAck on pipes
    reg8Val = NRF24_read_register(0x01);
    sprintf(uartTxBuf, "ENAA:\r\n		P0:	%d\r\n		P1:	%d\r\n		P2:	%d\r\n		P3:	%d\r\n		P4:	%d\r\n		P5:	%d\r\n",
        _BOOL(reg8Val & (1 << 0)), _BOOL(reg8Val & (1 << 1)), _BOOL(reg8Val & (1 << 2)), _BOOL(reg8Val & (1 << 3)), _BOOL(reg8Val & (1 << 4)), _BOOL(reg8Val & (1 << 5)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //c) Enabled Rx addresses
    reg8Val = NRF24_read_register(0x02);
    sprintf(uartTxBuf, "EN_RXADDR:\r\n		P0:	%d\r\n		P1:	%d\r\n		P2:	%d\r\n		P3:	%d\r\n		P4:	%d\r\n		P5:	%d\r\n",
        _BOOL(reg8Val & (1 << 0)), _BOOL(reg8Val & (1 << 1)), _BOOL(reg8Val & (1 << 2)), _BOOL(reg8Val & (1 << 3)), _BOOL(reg8Val & (1 << 4)), _BOOL(reg8Val & (1 << 5)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //d) Address width
    reg8Val = NRF24_read_register(0x03) & 0x03;
    reg8Val += 2;
    sprintf(uartTxBuf, "SETUP_AW:\r\n		%d bytes \r\n", reg8Val);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //e) RF channel
    reg8Val = NRF24_read_register(0x05);
    sprintf(uartTxBuf, "RF_CH:\r\n		%d CH \r\n", reg8Val & 0x7F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //f) Data rate & RF_PWR
    reg8Val = NRF24_read_register(0x06);
    if (reg8Val & (1 << 3)) sprintf(uartTxBuf, "Data Rate:\r\n		2Mbps \r\n");
    else sprintf(uartTxBuf, "Data Rate:\r\n		1Mbps \r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    reg8Val &= (3 << 1);
    reg8Val = (reg8Val >> 1);
    if (reg8Val == 0) sprintf(uartTxBuf, "RF_PWR:\r\n		-18dB \r\n");
    else if (reg8Val == 1) sprintf(uartTxBuf, "RF_PWR:\r\n		-12dB \r\n");
    else if (reg8Val == 2) sprintf(uartTxBuf, "RF_PWR:\r\n		-6dB \r\n");
    else if (reg8Val == 3) sprintf(uartTxBuf, "RF_PWR:\r\n		0dB \r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
    //g) RX pipes addresses
    uint8_t pipeAddrs[6];
    NRF24_read_registerN(0x0A, pipeAddrs, 5);
    sprintf(uartTxBuf, "RX_Pipe0 Addrs:\r\n		%02X,%02X,%02X,%02X,%02X  \r\n", pipeAddrs[4], pipeAddrs[3], pipeAddrs[2], pipeAddrs[1], pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 1, pipeAddrs, 5);
    sprintf(uartTxBuf, "RX_Pipe1 Addrs:\r\n		%02X,%02X,%02X,%02X,%02X  \r\n", pipeAddrs[4], pipeAddrs[3], pipeAddrs[2], pipeAddrs[1], pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 2, pipeAddrs, 1);
    sprintf(uartTxBuf, "RX_Pipe2 Addrs:\r\n		xx,xx,xx,xx,%02X  \r\n", pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 3, pipeAddrs, 1);
    sprintf(uartTxBuf, "RX_Pipe3 Addrs:\r\n		xx,xx,xx,xx,%02X  \r\n", pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 4, pipeAddrs, 1);
    sprintf(uartTxBuf, "RX_Pipe4 Addrs:\r\n		xx,xx,xx,xx,%02X  \r\n", pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 5, pipeAddrs, 1);
    sprintf(uartTxBuf, "RX_Pipe5 Addrs:\r\n		xx,xx,xx,xx,%02X  \r\n", pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    NRF24_read_registerN(0x0A + 6, pipeAddrs, 5);
    sprintf(uartTxBuf, "TX Addrs:\r\n		%02X,%02X,%02X,%02X,%02X  \r\n", pipeAddrs[4], pipeAddrs[3], pipeAddrs[2], pipeAddrs[1], pipeAddrs[0]);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    //h) RX PW (Payload Width 0 - 32)
    reg8Val = NRF24_read_register(0x11);
    sprintf(uartTxBuf, "RX_PW_P0:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x11 + 1);
    sprintf(uartTxBuf, "RX_PW_P1:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x11 + 2);
    sprintf(uartTxBuf, "RX_PW_P2:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x11 + 3);
    sprintf(uartTxBuf, "RX_PW_P3:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x11 + 4);
    sprintf(uartTxBuf, "RX_PW_P4:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x11 + 5);
    sprintf(uartTxBuf, "RX_PW_P5:\r\n		%d bytes \r\n", reg8Val & 0x3F);
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    //i) Dynamic payload enable for each pipe
    reg8Val = NRF24_read_register(0x1c);
    sprintf(uartTxBuf, "DYNPD_pipe:\r\n		P0:	%d\r\n		P1:	%d\r\n		P2:	%d\r\n		P3:	%d\r\n		P4:	%d\r\n		P5:	%d\r\n",
        _BOOL(reg8Val & (1 << 0)), _BOOL(reg8Val & (1 << 1)), _BOOL(reg8Val & (1 << 2)), _BOOL(reg8Val & (1 << 3)), _BOOL(reg8Val & (1 << 4)), _BOOL(reg8Val & (1 << 5)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    //j) EN_DPL (is Dynamic payload feature enabled ?)
    reg8Val = NRF24_read_register(0x1d);
    if (reg8Val & (1 << 2)) sprintf(uartTxBuf, "EN_DPL:\r\n		Enabled \r\n");
    else sprintf(uartTxBuf, "EN_DPL:\r\n		Disabled \r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    //k) EN_ACK_PAY
    if (reg8Val & (1 << 1)) sprintf(uartTxBuf, "EN_ACK_PAY:\r\n		Enabled \r\n");
    else sprintf(uartTxBuf, "EN_ACK_PAY:\r\n		Disabled \r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    sprintf(uartTxBuf, "\r\n**********************************************\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
}

//2. Print Status 
void printStatusReg(void) {
    uint8_t reg8Val;
    char uartTxBuf[100];
    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x07);
    sprintf(uartTxBuf, "STATUS reg:\r\n		RX_DR:		%d\r\n		TX_DS:		%d\r\n		MAX_RT:		%d\r\n		RX_P_NO:	%d\r\n		TX_FULL:	%d\r\n",
        _BOOL(reg8Val & (1 << 6)), _BOOL(reg8Val & (1 << 5)), _BOOL(reg8Val & (1 << 4)), _BOOL(reg8Val & (3 << 1)), _BOOL(reg8Val & (1 << 0)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
}
//3. Print Config 
void printConfigReg(void) {
    uint8_t reg8Val;
    char uartTxBuf[100];

    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x00);
    sprintf(uartTxBuf, "CONFIG reg:\r\n		PWR_UP:		%d\r\n		PRIM_RX:	%d\r\n",
        _BOOL(reg8Val & (1 << 1)), _BOOL(reg8Val & (1 << 0)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);
}

//4. Init Variables
void nrf24_DebugUART_Init(UART_HandleTypeDef nrf24Uart) {
    memcpy( & nrf24_huart, & nrf24Uart, sizeof(nrf24Uart));
}
//5. FIFO Status
void printFIFOstatus(void) {
    uint8_t reg8Val;
    char uartTxBuf[100];
    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    reg8Val = NRF24_read_register(0x17);
    sprintf(uartTxBuf, "FIFO Status reg:\r\n		TX_FULL:		%d\r\n		TX_EMPTY:		%d\r\n		RX_FULL:		%d\r\n		RX_EMPTY:		%d\r\n",
        _BOOL(reg8Val & (1 << 5)), _BOOL(reg8Val & (1 << 4)), _BOOL(reg8Val & (1 << 1)), _BOOL(reg8Val & (1 << 0)));
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

    sprintf(uartTxBuf, "\r\n-------------------------\r\n");
    HAL_UART_Transmit( & nrf24_huart, (uint8_t * ) uartTxBuf, strlen(uartTxBuf), 10);

}
*/
