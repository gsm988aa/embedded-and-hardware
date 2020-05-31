/******************************************************************************/
/* Change this according to your STM32 series */
#include "stm8s.h"
#include "DELAY.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
/******************************************************************************/
#define _BV(x) (1<<(x))
/******************************************************************************/
/* Memory Map */
#define REG_CONFIG              0x00
#define REG_EN_AA               0x01
#define REG_EN_RXADDR           0x02
#define REG_SETUP_AW            0x03
#define REG_SETUP_RETR          0x04
#define REG_RF_CH               0x05
#define REG_RF_SETUP            0x06
#define REG_STATUS              0x07
#define REG_OBSERVE_TX          0x08
#define REG_CD                  0x09
#define REG_RX_ADDR_P0          0x0A
#define REG_RX_ADDR_P1          0x0B
#define REG_RX_ADDR_P2          0x0C
#define REG_RX_ADDR_P3          0x0D
#define REG_RX_ADDR_P4          0x0E
#define REG_RX_ADDR_P5          0x0F
#define REG_TX_ADDR             0x10
#define REG_RX_PW_P0            0x11
#define REG_RX_PW_P1            0x12
#define REG_RX_PW_P2            0x13
#define REG_RX_PW_P3            0x14
#define REG_RX_PW_P4            0x15
#define REG_RX_PW_P5            0x16
#define REG_FIFO_STATUS         0x17
#define REG_DYNPD               0x1C
#define REG_FEATURE             0x1D
/* Bit Mnemonics */
#define MASK_RX_DR              0x06
#define MASK_TX_DS              0x05
#define MASK_MAX_RT             0x04
#define BIT_EN_CRC              0x03
#define BIT_CRCO                0x02
#define BIT_PWR_UP              0x01
#define BIT_PRIM_RX             0x00
#define BIT_ENAA_P5             0x05
#define BIT_ENAA_P4             0x04
#define BIT_ENAA_P3             0x03
#define BIT_ENAA_P2             0x02
#define BIT_ENAA_P1             0x01
#define BIT_ENAA_P0             0x00
#define BIT_ERX_P5              0x05
#define BIT_ERX_P4              0x04
#define BIT_ERX_P3              0x03
#define BIT_ERX_P2              0x02
#define BIT_ERX_P1              0x01
#define BIT_ERX_P0              0x00
#define BIT_AW                  0x00
#define BIT_ARD                 0x04
#define BIT_ARC                 0x00
#define BIT_PLL_LOCK            0x04
#define BIT_RF_DR               0x03
#define BIT_RF_PWR              0x06
#define BIT_RX_DR               0x06
#define BIT_TX_DS               0x05
#define BIT_MAX_RT              0x04
#define BIT_RX_P_NO             0x01
#define BIT_TX_FULL             0x00
#define BIT_PLOS_CNT            0x04
#define BIT_ARC_CNT             0x00
#define BIT_TX_REUSE            0x06
#define BIT_FIFO_FULL           0x05
#define BIT_TX_EMPTY            0x04
#define BIT_RX_FULL             0x01
#define BIT_RX_EMPTY            0x00
#define BIT_DPL_P5              0x05
#define BIT_DPL_P4              0x04
#define BIT_DPL_P3              0x03
#define BIT_DPL_P2              0x02
#define BIT_DPL_P1              0x01
#define BIT_DPL_P0              0x00
#define BIT_EN_DPL              0x02
#define BIT_EN_ACK_PAY          0x01
#define BIT_EN_DYN_ACK          0x00
/* Instruction Mnemonics */
#define CMD_R_REGISTER          0x00
#define CMD_W_REGISTER          0x20
#define CMD_REGISTER_MASK       0x1F
#define CMD_ACTIVATE            0x50
#define CMD_R_RX_PL_WID         0x60
#define CMD_R_RX_PAYLOAD        0x61
#define CMD_W_TX_PAYLOAD        0xA0
#define CMD_W_ACK_PAYLOAD       0xA8
#define CMD_FLUSH_TX            0xE1
#define CMD_FLUSH_RX            0xE2
#define CMD_REUSE_TX_PL         0xE3
#define CMD_NOP                 0xFF
/* Non-P omissions */
#define LNA_HCURR               0x00
/* P model memory Map */
#define REG_RPD                 0x09
/* P model bit Mnemonics */
#define RF_DR_LOW               0x05
#define RF_DR_HIGH              0x03
#define RF_PWR_LOW              0x01
#define RF_PWR_HIGH             0x02
/* ---------------------------- */
#define MAX_PAYLOAD_SIZE        32
#define MAX_CHANNEL             127
/******************************************************************************/
/* Power Amplifier function, NRF24_setPALevel() */
typedef enum { 
    RF24_PA_MIN = 0,/* RF24_PA_m18dB */
    RF24_PA_LOW, /* RF24_PA_m12dB */
    RF24_PA_HIGH, /* RF24_PA_m6dB */
    RF24_PA_MAX, /* RF24_PA_0dB */
    RF24_PA_ERROR /* RF24_PA_ERROR */
}rf24_pa_dbm_e ;
/* NRF24_setDataRate() input */
typedef enum { 
    RF24_1MBPS = 0,
    RF24_2MBPS,
    RF24_250KBPS
}rf24_datarate_e;
/* NRF24_setCRCLength() input */
typedef enum { 
    RF24_CRC_DISABLED = 0,
    RF24_CRC_8,
    RF24_CRC_16
}rf24_crclength_e;
/* Pipe address registers */
static const uint8_t NRF24_ADDR_REGS[7] = {
    REG_RX_ADDR_P0,
    REG_RX_ADDR_P1,
    REG_RX_ADDR_P2,
    REG_RX_ADDR_P3,
    REG_RX_ADDR_P4,
    REG_RX_ADDR_P5,
    REG_TX_ADDR
};
//5. RX_PW_Px registers addresses
static const uint8_t RF24_RX_PW_PIPE[6] = {
    REG_RX_PW_P0, 
    REG_RX_PW_P1,
    REG_RX_PW_P2,
    REG_RX_PW_P3,
    REG_RX_PW_P4,
    REG_RX_PW_P5
};
/******************************************************************************/
//**** Functions prototypes ****//
void NRF24_csn(uint8_t mode);
void NRF24_ce(uint8_t level);
bool NRF24L01_Check(void);
uint8_t NRF24_read_register(uint8_t reg);
void NRF24_read_registerN(uint8_t reg, uint8_t* buf, uint8_t len);
void NRF24_write_register(uint8_t reg, uint8_t value);
void NRF24_write_registerN(uint8_t reg, const uint8_t* buf, uint8_t len);
void NRF24_write_payload(const uint8_t* buf, uint8_t len);
void NRF24_read_payload(uint8_t* buf, uint8_t len);
void NRF24_flush_tx(void);
void NRF24_flush_rx(void);
uint8_t NRF24_get_status(void);
void NRF24_toggle_features(void);
bool NRF24_begin(GPIO_TypeDef* nrf24PORT, GPIO_Pin_TypeDef nrfCSN_Pin, GPIO_Pin_TypeDef nrfCE_Pin);
void NRF24_startListening(void);
void NRF24_stopListening(void);
bool NRF24_write(const void* buf, uint8_t len);
bool NRF24_available(void);
bool NRF24_read(void* buf, uint8_t len);
void NRF24_openWritingPipe(uint8_t* address);
void NRF24_openReadingPipe(uint8_t number, uint64_t address);
void NRF24_setRetries(uint8_t delay, uint8_t count);
void NRF24_setChannel(uint8_t channel);
void NRF24_setPayloadSize(uint8_t size);
uint8_t NRF24_getPayloadSize(void);
uint8_t NRF24_getDynamicPayloadSize(void);
void NRF24_enableAckPayload(void);
void NRF24_enableDynamicPayloads(void);
void NRF24_disableDynamicPayloads(void);
bool NRF24_isNRF_Plus(void);
void NRF24_setAutoAck(bool enable);
void NRF24_setAutoAckPipe(uint8_t pipe, bool enable);
void NRF24_setPALevel(rf24_pa_dbm_e level);
rf24_pa_dbm_e NRF24_getPALevel(void);
bool NRF24_setDataRate(rf24_datarate_e speed);
rf24_datarate_e NRF24_getDataRate(void);
void NRF24_setCRCLength(rf24_crclength_e length);
rf24_crclength_e NRF24_getCRCLength(void);
void NRF24_disableCRC(void) ;
void NRF24_powerUp(void) ;
void NRF24_powerDown(void);
bool NRF24_availablePipe(uint8_t* pipe_num);
void NRF24_startWrite(const void* buf, uint8_t len);
void NRF24_writeAckPayload(uint8_t pipe, const uint8_t* buf, uint8_t len);
bool NRF24_isAckPayloadAvailable(void);
void NRF24_whatHappened(bool* tx_ok, bool* tx_fail, bool* rx_ready);
bool NRF24_testCarrier(void);
bool NRF24_testRPD(void);
void NRF24_resetStatus(void);
uint8_t NRF24_GetAckPayloadSize(void);
/******************************************************************************/
//**********  DEBUG Functions **********//
/*//1. Print radio settings
void printRadioSettings(void);
//2. Print Status 
void printStatusReg(void);
//3. Print Config 
void printConfigReg(void);
//4. Init Variables
void nrf24_DebugUART_Init(UART_HandleTypeDef nrf24Uart);
//5. FIFO Status
void printFIFOstatus(void);*/
/******************************************************************************/

