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
static bool dynamic_payloads_enabled; /* Whether dynamic payloads are enabled. */
static bool p_variant; /* False for RF24L01 and true for RF24L01P */
static bool wide_band; /* 2Mbs data rate in use? */
/******************************************************************************/
/*** NRF24L01 pins and handles ***/
static GPIO_TypeDef*        nrf24_PORT;
static uint16_t             nrf24_CSN_PIN;
static uint16_t             nrf24_CE_PIN;
/******************************************************************************/
static SPI_HandleTypeDef    nrf24_hspi;
extern delay* dl;
/******************************************************************************/
/******************************************************************************/
void NRF24::csn(int state)
{
    if (state) HAL_GPIO_WritePin(nrf24_PORT, nrf24_CSN_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(nrf24_PORT, nrf24_CSN_PIN, GPIO_PIN_RESET);
}
/******************************************************************************/
void NRF24::ce(int state)
{
    if (state) HAL_GPIO_WritePin(nrf24_PORT, nrf24_CE_PIN, GPIO_PIN_SET);
    else HAL_GPIO_WritePin(nrf24_PORT, nrf24_CE_PIN, GPIO_PIN_RESET);
}
/******************************************************************************/
uint8_t NRF24::read_register(uint8_t reg)
{
    uint8_t spiBuf[3];
    uint8_t retData;
    csn(0);
    spiBuf[0] = reg & 0x1F;
    HAL_SPI_Transmit(&nrf24_hspi, spiBuf, 1, 100);
    HAL_SPI_Receive(&nrf24_hspi, &spiBuf[1], 1, 100);
    retData = spiBuf[1];
    csn(1);
    return retData;
}
/******************************************************************************/
void NRF24::read_register(uint8_t reg, uint8_t * buf, uint8_t len)
{
    uint8_t spiBuf[3];
    csn(0);
    spiBuf[0] = reg & 0x1F;
    HAL_SPI_Transmit(&nrf24_hspi, spiBuf, 1, 100);
    HAL_SPI_Receive(&nrf24_hspi, buf, len, 100);
    csn(1);
}
/******************************************************************************/
void NRF24::write_register(uint8_t reg, uint8_t value)
{
    uint8_t spiBuf[3];
    csn(0);
    spiBuf[0] = reg | 0x20;
    spiBuf[1] = value;
    HAL_SPI_Transmit(&nrf24_hspi, spiBuf, 2, 100);
    csn(1);
}
/******************************************************************************/
void NRF24::write_register(uint8_t reg, const uint8_t* buf, uint8_t len)
{
    uint8_t spiBuf[3];
    csn(0);
    spiBuf[0] = reg | 0x20;
    HAL_SPI_Transmit(&nrf24_hspi, spiBuf, 1, 100);
    HAL_SPI_Transmit(&nrf24_hspi, (uint8_t*) buf, len, 100);
    csn(1);
}
/******************************************************************************/
void NRF24::write_payload(const uint8_t* buf, uint8_t len)
{
    uint8_t wrPayloadCmd;
    csn(0);
    wrPayloadCmd = CMD_W_TX_PAYLOAD;
    HAL_SPI_Transmit(&nrf24_hspi, &wrPayloadCmd, 1, 100);
    HAL_SPI_Transmit(&nrf24_hspi, (uint8_t*) buf, len, 100);
    csn(1);
}
/******************************************************************************/
void NRF24::read_payload(uint8_t* buf, uint8_t len)
{
    uint8_t cmdRxBuf;
    uint8_t data_len = MIN(len, getPayloadSize());
    csn(0);
    cmdRxBuf = CMD_R_RX_PAYLOAD;
    HAL_SPI_Transmit(&nrf24_hspi, &cmdRxBuf, 1, 100);
    HAL_SPI_Receive(&nrf24_hspi, buf, data_len, 100);
    csn(1);
}
/******************************************************************************/
void NRF24::flush_tx(void)
{
	this->write_register(CMD_FLUSH_TX, 0xFF);
}
/******************************************************************************/
void NRF24::flush_rx(void)
{
    this->write_register(CMD_FLUSH_RX, 0xFF);
}
/******************************************************************************/
/* Get status register value */
uint8_t NRF24::get_status(void)
{
    uint8_t statReg;
    statReg = this->read_register(REG_STATUS);
    return statReg;
}
/******************************************************************************/
void NRF24::toggle_features(void)
{
    csn(0);
    HAL_SPI_Transmit(&nrf24_hspi, (uint8_t*)CMD_ACTIVATE, 1, 100);
    HAL_SPI_Transmit(&nrf24_hspi, (uint8_t*)0x73, 1, 100);
    csn(1);
}
/******************************************************************************/
/* Begin function */
bool NRF24::begin(GPIO_TypeDef* nrf24PORT, uint16_t nrfCSN_Pin, uint16_t nrfCE_Pin, SPI_HandleTypeDef nrfSPI)
{
	uint8_t setup = 0;
	uint8_t pipeAddrVar[6];
    memcpy(&nrf24_hspi, &nrfSPI, sizeof(nrfSPI));
    nrf24_PORT = nrf24PORT;
    nrf24_CSN_PIN = nrfCSN_Pin;
    nrf24_CE_PIN = nrfCE_Pin;
    csn(1);
    ce(0);
    dl->delay_ms(5);
    this->write_register(REG_CONFIG, 0x08);
    this->write_register(REG_EN_AA, 0x3f);
    this->write_register(REG_EN_RXADDR, 0x03);
    this->write_register(REG_SETUP_AW, 0x03);
    this->write_register(REG_SETUP_RETR, 0x03);
    this->write_register(REG_RF_CH, 0x02);
    this->write_register(REG_RF_SETUP, 0x0f);
    this->write_register(REG_STATUS, 0x0e);
    this->write_register(REG_OBSERVE_TX, 0x00);
    this->write_register(REG_CD, 0x00);
    pipeAddrVar[4]=0xE7; pipeAddrVar[3]=0xE7; pipeAddrVar[2]=0xE7; pipeAddrVar[1]=0xE7; pipeAddrVar[0]=0xE7;
    this->write_register(REG_RX_ADDR_P0, pipeAddrVar, 5);
    pipeAddrVar[4]=0xC2; pipeAddrVar[3]=0xC2; pipeAddrVar[2]=0xC2; pipeAddrVar[1]=0xC2; pipeAddrVar[0]=0xC2;
    this->write_register(REG_RX_ADDR_P1, pipeAddrVar, 5);
    this->write_register(REG_RX_ADDR_P2, 0xC3);
    this->write_register(REG_RX_ADDR_P3, 0xC4);
    this->write_register(REG_RX_ADDR_P4, 0xC5);
    this->write_register(REG_RX_ADDR_P5, 0xC6);
    pipeAddrVar[4]=0xE7; pipeAddrVar[3]=0xE7; pipeAddrVar[2]=0xE7; pipeAddrVar[1]=0xE7; pipeAddrVar[0]=0xE7;
    this->write_register(REG_TX_ADDR, pipeAddrVar, 5);
    this->write_register(REG_RX_PW_P0, 0);
    this->write_register(REG_RX_PW_P1, 0);
    this->write_register(REG_RX_PW_P2, 0);
    this->write_register(REG_RX_PW_P3, 0);
    this->write_register(REG_RX_PW_P4, 0);
    this->write_register(REG_RX_PW_P5, 0);
    this->toggle_features();
    this->write_register(REG_FEATURE, 0);
    this->write_register(REG_DYNPD, 0);
    this->setCRCLength(RF24_CRC_16);
    this->setRetries(5, 15);
    if(this->setDataRate(RF24_250KBPS))p_variant = true;
    setup = this->read_register(REG_RF_SETUP);
    this->setDataRate(RF24_1MBPS);
    dynamic_payloads_enabled = false;
    this->write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    this->setPayloadSize(32);
    this->setChannel(76);
    this->flush_tx();
    this->flush_rx();
    this->powerUp();
    this->write_register(REG_CONFIG, (this->read_register(REG_CONFIG))&(~_BV(BIT_PRIM_RX)));
    return ((setup != 0)&&(setup != 0xff));
}
/******************************************************************************/
/* Listen on open pipes for reading (Must call openReadingPipe() first) */
void NRF24::startListening(void)
{
    write_register(REG_CONFIG, read_register(REG_CONFIG)|_BV(BIT_PRIM_RX));
    write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    if(pipe0_reading_address)write_register(REG_RX_ADDR_P0, (uint8_t*)(&pipe0_reading_address), 5);
    flush_tx();
    flush_rx();
    ce(1);
    dl->delay_us(150);
}
/******************************************************************************/
/* Stop listening (essential before any write operation) */
void NRF24::stopListening(void)
{
    ce(0);
    flush_tx();
    flush_rx();
}
/******************************************************************************/
/* Write(Transmit data), returns true if successfully sent */
bool NRF24::write(const uint8_t* buf, uint8_t len)
{
    bool retStatus;
    uint8_t observe_tx;
    uint8_t status;
    bool tx_ok, tx_fail;
    uint32_t sent_at = HAL_GetTick();
    resetStatus();
    startWrite(buf, len);
    do
    {
        read_register(REG_OBSERVE_TX, &observe_tx, 1);
        status = get_status();
    }while(!(status&(_BV(BIT_TX_DS)|_BV(BIT_MAX_RT)))&&((HAL_GetTick() - sent_at) < 10));
    whatHappened(&tx_ok, &tx_fail, &ack_payload_available);
    retStatus = tx_ok;
    if(ack_payload_available)
    {
        ack_payload_length = getDynamicPayloadSize();
    }
    available();
    flush_tx();
    return retStatus;
}
/******************************************************************************/
/* Check for available data to read */
bool NRF24::available(void)
{
    return availablePipe(NULL);
}
/******************************************************************************/
/* Read received data */
bool NRF24::read(uint8_t* buf, uint8_t len)
{
    read_payload(buf, len);
    uint8_t rxStatus = read_register(REG_FIFO_STATUS)&_BV(BIT_RX_EMPTY);
    flush_rx();
    getDynamicPayloadSize();
    return rxStatus;
}
/******************************************************************************/
/* Open Tx pipe for writing (Cannot perform this while Listenning, has to call stopListening) */
void NRF24::openWritingPipe(uint64_t address)
{
    write_register(REG_RX_ADDR_P0, (uint8_t*)(&address), 5);
    write_register(REG_TX_ADDR, (uint8_t*)(&address), 5);
    write_register(REG_RX_PW_P0, MIN(payload_size, MAX_PAYLOAD_SIZE));
}
/******************************************************************************/
/* Open reading pipe */
void NRF24::openReadingPipe(uint8_t number, uint64_t address)
{
    if(number == 0)pipe0_reading_address = address;
    if(number <= 6)
    {
        if(number < 2)write_register(NRF24_ADDR_REGS[number], (uint8_t*)(&address), 5);
        else write_register(NRF24_ADDR_REGS[number], (uint8_t*)(&address), 1);
        write_register(RF24_RX_PW_PIPE[number], payload_size);
        write_register(REG_EN_RXADDR, read_register(REG_EN_RXADDR)|_BV(number));
    }
}
/******************************************************************************/
/* set transmit retries (rf24_Retries_e) and delay */
void NRF24::setRetries(uint8_t delay, uint8_t count)
{
    write_register(REG_SETUP_RETR, ((delay&0xf)<<BIT_ARD)|((count&0xf)<<BIT_ARC));
}
/******************************************************************************/
/* Set RF channel frequency */
void NRF24::setChannel(uint8_t channel)
{
    write_register(REG_RF_CH, MIN(channel, MAX_CHANNEL));
}
/******************************************************************************/
/* Set payload size */
void NRF24::setPayloadSize(uint8_t size)
{
    payload_size = MIN(size, MAX_PAYLOAD_SIZE);
}
/******************************************************************************/
/* Get payload size */
uint8_t NRF24::getPayloadSize(void)
{
    return payload_size;
}
/******************************************************************************/
/* Get dynamic payload size, of latest packet received */
uint8_t NRF24::getDynamicPayloadSize(void)
{
    return read_register(CMD_R_RX_PL_WID);
}
/******************************************************************************/
/* Enable payload on Ackknowledge packet */
void NRF24::enableAckPayload(void)
{
    write_register(REG_FEATURE, read_register(REG_FEATURE)|_BV(BIT_EN_ACK_PAY)|_BV(BIT_EN_DPL));
    if (!read_register(REG_FEATURE))
    {
        toggle_features();
        write_register(REG_FEATURE, read_register(REG_FEATURE)|_BV(BIT_EN_ACK_PAY)|_BV(BIT_EN_DPL));
    }
    write_register(REG_DYNPD, read_register(REG_DYNPD)|_BV(BIT_DPL_P1)|_BV(BIT_DPL_P0));
}
/******************************************************************************/
/* Enable dynamic payloads */
void NRF24::enableDynamicPayloads(void)
{
    write_register(REG_FEATURE, read_register(REG_FEATURE)|_BV(BIT_EN_DPL));
    if(!read_register(REG_FEATURE))
    {
        toggle_features();
        write_register(REG_FEATURE, read_register(REG_FEATURE)|_BV(BIT_EN_DPL));
    }
    write_register(REG_DYNPD, read_register(REG_DYNPD)|_BV(BIT_DPL_P5)|_BV(BIT_DPL_P4)|_BV(BIT_DPL_P3)|_BV(BIT_DPL_P2)|_BV(BIT_DPL_P1)|_BV(BIT_DPL_P0));
    dynamic_payloads_enabled = true;
}
/******************************************************************************/
/* Disable dynamic payloads */
void NRF24::disableDynamicPayloads(void)
{
    write_register(REG_FEATURE, read_register(REG_FEATURE)&(~_BV(BIT_EN_DPL)));
    write_register(REG_DYNPD, 0);
    dynamic_payloads_enabled = false;
}
/******************************************************************************/
/* Check if module is NRF24L01+ or normal module */
bool NRF24::isNRF_Plus(void)
{
    return p_variant;
}
/******************************************************************************/
/* Set Auto Ack for all */
void NRF24::setAutoAck(bool enable)
{
    if(enable)write_register(REG_EN_AA, 0x3F);
    else write_register(REG_EN_AA, 0x00);
}
/******************************************************************************/
/* Set Auto Ack for certain pipe */
void NRF24::setAutoAckPipe(uint8_t pipe, bool enable)
{
    uint8_t en_aa;
    if(pipe <= 6)
    {
        en_aa = read_register(REG_EN_AA);
        if(enable)en_aa |= _BV(pipe);
        else en_aa &= ~_BV(pipe);
        write_register(REG_EN_AA, en_aa);
    }
}
/******************************************************************************/
/* Set transmit power level */
void NRF24::setPALevel(rf24_pa_dbm_e level)
{
    uint8_t setup = read_register(REG_RF_SETUP);
    setup &= (~(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH)));
    if (level == RF24_PA_MAX)setup|=(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    else if (level == RF24_PA_HIGH)setup|=_BV(RF_PWR_HIGH);
    else if (level == RF24_PA_LOW)setup|=_BV(RF_PWR_LOW);
    else if (level == RF24_PA_MIN) {/* nothing */}
    else if (level == RF24_PA_ERROR)setup|=(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    write_register(REG_RF_SETUP, setup);
}
/******************************************************************************/
/* Get transmit power level */
rf24_pa_dbm_e NRF24::getPALevel(void)
{
    uint8_t power = read_register(REG_RF_SETUP)&(_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH));
    rf24_pa_dbm_e result = RF24_PA_ERROR;
    if (power == (_BV(RF_PWR_LOW)|_BV(RF_PWR_HIGH)))result = RF24_PA_MAX;
    else if(power == _BV(RF_PWR_HIGH))result = RF24_PA_HIGH;
    else if(power == _BV(RF_PWR_LOW))result = RF24_PA_LOW;
    else result = RF24_PA_MIN;
    return result;
}
/******************************************************************************/
/* Set data rate (250 Kbps, 1Mbps, 2Mbps) */
bool NRF24::setDataRate(rf24_datarate_e speed)
{
    bool result = false;
    uint8_t setup = read_register(REG_RF_SETUP);
    wide_band = false;
    setup &= (~(_BV(RF_DR_LOW)|_BV(RF_DR_HIGH)));
    if(speed == RF24_250KBPS)
    {
        wide_band = false;
        setup |= _BV(RF_DR_LOW);
    }
    else
    {
        if (speed == RF24_2MBPS)
        {
            wide_band = true;
            setup |= _BV(RF_DR_HIGH);
        }
        else
        {
            wide_band = false;
        }
    }
    write_register(REG_RF_SETUP, setup);
    if(read_register(REG_RF_SETUP) == setup)result = true;
    else wide_band = false;
    return result;
}
/******************************************************************************/
/* Get data rate */
rf24_datarate_e NRF24::getDataRate(void)
{
    rf24_datarate_e result;
    uint8_t dr = read_register(REG_RF_SETUP)&(_BV(RF_DR_LOW)|_BV(RF_DR_HIGH));
    if(dr == _BV(RF_DR_LOW))result = RF24_250KBPS;
    else if(dr == _BV(RF_DR_HIGH))result = RF24_2MBPS;
    else result = RF24_1MBPS;
    return result;
}
/******************************************************************************/
/* Set crc length (disable, 8-bits or 16-bits) */
void NRF24::setCRCLength(rf24_crclength_e length)
{
    uint8_t config = read_register(REG_CONFIG)&(~(_BV(BIT_CRCO)|_BV(BIT_EN_CRC)));
    if(length == RF24_CRC_8)config |= _BV(BIT_EN_CRC);
    else
    {
        config |= _BV(BIT_EN_CRC);
        config |= _BV(BIT_CRCO);
    }
    write_register(REG_CONFIG, config);
}
/******************************************************************************/
/* Get CRC length */
rf24_crclength_e NRF24::getCRCLength(void)
{
    rf24_crclength_e result = RF24_CRC_DISABLED;
    uint8_t config = read_register(REG_CONFIG)&(_BV(BIT_CRCO)|_BV(BIT_EN_CRC));
    if(config&_BV(BIT_EN_CRC))
    {
        if(config & _BV(BIT_CRCO))result = RF24_CRC_16;
        else result = RF24_CRC_8;
    }
    return result;
}
/******************************************************************************/
/* Disable CRC */
void NRF24::disableCRC(void)
{
    uint8_t disable = read_register(REG_CONFIG)&(~_BV(BIT_EN_CRC));
    write_register(REG_CONFIG, disable);
}
/******************************************************************************/
/* power up */
void NRF24::powerUp(void)
{
    write_register(REG_CONFIG, read_register(REG_CONFIG)|_BV(BIT_PWR_UP));
}
/******************************************************************************/
/* power down */
void NRF24::powerDown(void)
{
    write_register(REG_CONFIG, read_register(REG_CONFIG)&(~_BV(BIT_PWR_UP)));
}
/******************************************************************************/
/* Check if data are available and on which pipe (Use this for multiple rx pipes) */
bool NRF24::availablePipe(uint8_t* pipe_num)
{
    uint8_t status = get_status();
    bool result = (status&_BV(BIT_RX_DR));
    if(result)
    {
        if (pipe_num)
        {
            *pipe_num = (status >> BIT_RX_P_NO)&0x07;
        }
        write_register(REG_STATUS, _BV(BIT_RX_DR));
        if (status & _BV(BIT_TX_DS))
        {
            write_register(REG_STATUS, _BV(BIT_TX_DS));
        }
    }
    return result;
}
/******************************************************************************/
/* Start write (for IRQ mode) */
void NRF24::startWrite(const uint8_t* buf, uint8_t len)
{
    write_register(REG_CONFIG, (read_register(REG_CONFIG)|_BV(BIT_PWR_UP))&(~_BV(BIT_PRIM_RX)));
    dl->delay_us(150);
    write_payload(buf, len);
    ce(1);
    dl->delay_us(15);
    ce(0);
}
/******************************************************************************/
/* Write acknowledge payload */
void NRF24::writeAckPayload(uint8_t pipe, const void* buf, uint8_t len)
{
    const uint8_t *current = (uint8_t*)buf;
    uint8_t data_len = MIN(len, MAX_PAYLOAD_SIZE);
    csn(0);
    write_register(CMD_W_ACK_PAYLOAD|(pipe & 0x7), current, data_len);
    csn(1);
}
/******************************************************************************/
/* Check if an Ack payload is available */
bool NRF24::isAckPayloadAvailable(void)
{
    bool result = ack_payload_available;
    ack_payload_available = false;
    return result;
}
/******************************************************************************/
/* Check interrupt flags */
void NRF24::whatHappened(bool* tx_ok, bool* tx_fail, bool* rx_ready)
{
    uint8_t status = get_status();
    *tx_ok = 0;
    write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
    *tx_ok = status & _BV(BIT_TX_DS);
    *tx_fail = status & _BV(BIT_MAX_RT);
    *rx_ready = status & _BV(BIT_RX_DR);
}
/******************************************************************************/
/* Test if there is a carrier on the previous listenning period (useful to check for intereference) */
bool NRF24::testCarrier(void)
{
    return read_register(REG_CD)&0x01;
}
/******************************************************************************/
/* Test if a signal carrier exists (=> -64dB), only for NRF24L01+ */
bool NRF24::testRPD(void)
{
    return read_register(REG_RPD)&0x01;
}
/******************************************************************************/
/* Reset Status */
void NRF24::resetStatus(void)
{
    write_register(REG_STATUS, _BV(BIT_RX_DR)|_BV(BIT_TX_DS)|_BV(BIT_MAX_RT));
}
/******************************************************************************/
/* Get AckPayload Size */
uint8_t NRF24::GetAckPayloadSize(void)
{
    return ack_payload_length;
}
/******************************************************************************/
