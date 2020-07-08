/***********************************************************************************
* Project Name           :
*
* @Revision Number       : 1.0
*
* @File Name             : nRF24L01.h
*
* Target Compiler        : Atollic TrueStudio
*
* Created By             : Pham Van Lap
*
* Created on Date        : 04/01/2019
*
* @Brief Description     : Header file for nRF24L01 library
************************************************************************************
*
*                             Revision History
*-----------------------------------------------------------------------------------
*     Date                  Author                     Description
*   ----------           --------------           -----------------------
*   04/01/2019            Pham Van Lap                 Create new
***********************************************************************************/
#ifndef _TM_NRF24L01_H_
#define _TM_NRF24L01_H_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f4xx.h"
/* SPI chip enable pin */
#define NRF24L01_CSN_PORT           GPIOE
#define NRF24L01_CSN_PIN            GPIO_PIN_8
/* Chip enable for transmitting */
#define NRF24L01_CE_PORT            GPIOE
#define NRF24L01_CE_PIN             GPIO_PIN_7
/* Pins configuration */
#define NRF24L01_CE_LOW             HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_RESET)
#define NRF24L01_CE_HIGH            HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_SET)
#define NRF24L01_CSN_LOW            HAL_GPIO_WritePin(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN, GPIO_PIN_RESET)
#define NRF24L01_CSN_HIGH           HAL_GPIO_WritePin(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN, GPIO_PIN_SET)
/***********************************************************************************
***************************** NRF24L01+ Macro **************************************
***********************************************************************************/
#define NRF24L01_REG_CONFIG                         0x00    //Configuration Register
#define NRF24L01_REG_EN_AA                          0x01    //Enable ‘Auto Acknowledgment’ Function
#define NRF24L01_REG_EN_RXADDR                      0x02    //Enabled RX Addresses
#define NRF24L01_REG_SETUP_AW                       0x03    //Setup of Address Widths (common for all data pipes)
#define NRF24L01_REG_SETUP_RETR                     0x04    //Setup of Automatic Retransmission
#define NRF24L01_REG_RF_CH                          0x05    //RF Channel
#define NRF24L01_REG_RF_SETUP                       0x06    //RF Setup Register
#define NRF24L01_REG_STATUS                         0x07    //Status Register
#define NRF24L01_REG_OBSERVE_TX                     0x08    //Transmit observe register
#define NRF24L01_REG_RPD                            0x09
#define NRF24L01_REG_RX_ADDR_P0                     0x0A    //Receive address data pipe 0. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P1                     0x0B    //Receive address data pipe 1. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P2                     0x0C    //Receive address data pipe 2. Only LSB
#define NRF24L01_REG_RX_ADDR_P3                     0x0D    //Receive address data pipe 3. Only LSB
#define NRF24L01_REG_RX_ADDR_P4                     0x0E    //Receive address data pipe 4. Only LSB
#define NRF24L01_REG_RX_ADDR_P5                     0x0F    //Receive address data pipe 5. Only LSB
#define NRF24L01_REG_TX_ADDR                        0x10    //Transmit address. Used for a PTX device only
#define NRF24L01_REG_RX_PW_P0                       0x11
#define NRF24L01_REG_RX_PW_P1                       0x12
#define NRF24L01_REG_RX_PW_P2                       0x13
#define NRF24L01_REG_RX_PW_P3                       0x14
#define NRF24L01_REG_RX_PW_P4                       0x15
#define NRF24L01_REG_RX_PW_P5                       0x16
#define NRF24L01_REG_FIFO_STATUS                    0x17    //FIFO Status Register
#define NRF24L01_REG_DYNPD                          0x1C    //Enable dynamic payload length
#define NRF24L01_REG_FEATURE                        0x1D
/* Registers default values */
#define NRF24L01_REG_DEFAULT_VAL_CONFIG             0x08
#define NRF24L01_REG_DEFAULT_VAL_EN_AA              0x3F
#define NRF24L01_REG_DEFAULT_VAL_EN_RXADDR          0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_AW           0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_RETR         0x03
#define NRF24L01_REG_DEFAULT_VAL_RF_CH              0x02
#define NRF24L01_REG_DEFAULT_VAL_RF_SETUP           0x0E
#define NRF24L01_REG_DEFAULT_VAL_STATUS             0x0E
#define NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX         0x00
#define NRF24L01_REG_DEFAULT_VAL_RPD                0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0       0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1       0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2       0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3       0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4       0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0       0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1       0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2       0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3       0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4       0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2         0xC3
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3         0xC4
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4         0xC5
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5         0xC6
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0          0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1          0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2          0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3          0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4          0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P0           0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P1           0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P2           0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P3           0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P4           0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P5           0x00
#define NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS        0x11
#define NRF24L01_REG_DEFAULT_VAL_DYNPD              0x00
#define NRF24L01_REG_DEFAULT_VAL_FEATURE            0x00
/* Configuration register*/
#define NRF24L01_MASK_RX_DR             	        6
#define NRF24L01_MASK_TX_DS             	        5
#define NRF24L01_MASK_MAX_RT            	        4
#define NRF24L01_EN_CRC                 	        3
#define NRF24L01_CRCO                   	        2
#define NRF24L01_PWR_UP                 	        1
#define NRF24L01_PRIM_RX                	        0
/* Enable auto acknowledgment*/
#define NRF24L01_ENAA_P5                	        5
#define NRF24L01_ENAA_P4                	        4
#define NRF24L01_ENAA_P3                	        3
#define NRF24L01_ENAA_P2                	        2
#define NRF24L01_ENAA_P1                	        1
#define NRF24L01_ENAA_P0                	        0
/* Enable rx addresses */
#define NRF24L01_ERX_P5                 	        5
#define NRF24L01_ERX_P4                 	        4
#define NRF24L01_ERX_P3                 	        3
#define NRF24L01_ERX_P2                 	        2
#define NRF24L01_ERX_P1                 	        1
#define NRF24L01_ERX_P0                 	        0
/* Setup of address width */
#define NRF24L01_AW                     	        0 //2 bits
/* Setup of auto re-transmission*/
#define NRF24L01_ARD                    	        4 //4 bits
#define NRF24L01_ARC                    	        0 //4 bits
/* RF setup register*/
#define NRF24L01_PLL_LOCK               	        4
#define NRF24L01_RF_DR_LOW              	        5
#define NRF24L01_RF_DR_HIGH             	        3
#define NRF24L01_RF_DR                  	        3
#define NRF24L01_RF_PWR                 	        1 //2 bits
/* General status register */
#define NRF24L01_RX_DR                  	        6
#define NRF24L01_TX_DS                  	        5
#define NRF24L01_MAX_RT                 	        4
#define NRF24L01_RX_P_NO                	        1 //3 bits
#define NRF24L01_TX_FULL                	        0
/* Transmit observe register */
#define NRF24L01_PLOS_CNT               	        4 //4 bits
#define NRF24L01_ARC_CNT                	        0 //4 bits
/* FIFO status*/
#define NRF24L01_TX_REUSE               	        6
#define NRF24L01_FIFO_FULL              	        5
#define NRF24L01_TX_EMPTY               	        4
#define NRF24L01_RX_FULL                	        1
#define NRF24L01_RX_EMPTY               	        0
//Dynamic length
#define NRF24L01_DPL_P0                 	        0
#define NRF24L01_DPL_P1                 	        1
#define NRF24L01_DPL_P2                 	        2
#define NRF24L01_DPL_P3                 	        3
#define NRF24L01_DPL_P4                 	        4
#define NRF24L01_DPL_P5                 	        5
/* Configuration */
#define NRF24L01_CONFIG                 	        ((1 << NRF24L01_EN_CRC)|(0 << NRF24L01_CRCO))
/* Instruction Mnemonics */
#define NRF24L01_REGISTER_MASK                      0x1F
#define NRF24L01_READ_REGISTER_MASK(reg)            (0x00|(NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_WRITE_REGISTER_MASK(reg)           (0x20|(NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_R_RX_PAYLOAD_MASK                  0x61
#define NRF24L01_W_TX_PAYLOAD_MASK                  0xA0
#define NRF24L01_FLUSH_TX_MASK                      0xE1
#define NRF24L01_FLUSH_RX_MASK                      0xE2
#define NRF24L01_REUSE_TX_PL_MASK                   0xE3
#define NRF24L01_ACTIVATE_MASK                      0x50
#define NRF24L01_R_RX_PL_WID_MASK                   0x60
#define NRF24L01_NOP_MASK                           0xFF
#define NRF24L01_TRANSMISSON_OK                     0
#define NRF24L01_MESSAGE_LOST                       1
#define NRF24L01_CHECK_BIT(reg, bit)                (reg & (1 << bit))
/***********************************************************************************
******************************** NRF24L01+ Structure *******************************
***********************************************************************************/
/**
 * @brief  Transmission status enumeration
 */
typedef enum {
    lost = 0x00,   		/*!< Message is lost, reached maximum number of retransmissions */
    success = 0x01,     /*!< Message sent successfully */
    sending = 0xFF 		/*!< Message is still sending */
} transmit_status_t;
/**
 * @brief  Data rate enumeration
 */
typedef enum {
    DR_2Mbps,  		/*!< Data rate set to 2Mbps */
    DR_1Mbps,  		/*!< Data rate set to 1Mbps */
    DR_250kbps 		/*!< Data rate set to 250kbps */
} air_data_rate_t;
/**
 * @brief  Output power enumeration
 */
typedef enum {
    PA_M18dBm,    	/*!< Output power set to -18dBm */
    PA_M12dBm, 		/*!< Output power set to -12dBm */
    PA_M6dBm,  		/*!< Output power set to -6dBm */
    PA_0dBm    		/*!< Output power set to 0dBm */
} power_amplifier_t;
/**
 * @brief  Output power enumeration
 */
typedef struct {
    uint8_t PayloadSize;        //Payload size
    uint8_t Channel;            //Channel selected
    power_amplifier_t OutPwr;    //Output power
    air_data_rate_t DataRate;    //Data rate
} TM_NRF24L01_t;
/* Clear interrupt flags */
#define NRF24L01_CLEAR_INTERRUPTS   do{TM_NRF24L01_WriteRegister(0x07, 0x70);}while(0)
/* Gets interrupt status from device */
#define NRF24L01_GET_INTERRUPTS     TM_NRF24L01_GetStatus()
/* Interrupt masks */
#define NRF24L01_IRQ_DATA_READY     0x40 /*!< Data ready for receive */
#define NRF24L01_IRQ_TRAN_OK        0x20 /*!< Transmission went OK */
#define NRF24L01_IRQ_MAX_RT         0x10 /*!< Max retransmissions reached, last transmission failed */

/**
 * @brief  Initializes NRF24L01+ module
 * @param  channel: channel you will use for communication, from 0 to 125 eg. working frequency from 2.4 to 2.525 GHz
 * @param  payload_size: maximum data to be sent in one packet from one NRF to another.
 * @note   Maximal payload size is 32bytes
 * @retval 1
 */
uint8_t TM_NRF24L01_Init(uint8_t channel, uint8_t payload_size);

/**
 * @brief  Sets own address. This is used for settings own id when communication with other modules
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetMyAddress(uint8_t* addr);

/**
 * @brief  Sets address you will communicate with
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void TM_NRF24L01_SetTxAddress(uint8_t* addr);

/**
 * @brief  Gets number of retransmissions needed in last transmission
 * @param  None
 * @retval Number of retransmissions, between 0 and 15.
 */
uint8_t TM_NRF24L01_GetRetransmissionsCount(void);

/**
 * @brief  Sets NRF24L01+ to TX mode
 * @note   In this mode is NRF able to send data to another NRF module
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpTx(void);

/**
 * @brief  Sets NRF24L01+ to RX mode
 * @note   In this mode is NRF able to receive data from another NRF module.
 *         This is default mode and should be used all the time, except when sending data
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerUpRx(void);

/**
 * @brief  Sets NRF24L01+ to power down mode
 * @note   In power down mode, you are not able to transmit/receive data.
 *         You can wake up device using @ref TM_NRF24L01_PowerUpTx() or @ref TM_NRF24L01_PowerUpRx() functions
 * @param  None
 * @retval None
 */
void TM_NRF24L01_PowerDown(void);

/**
 * @brief  Gets transmissions status
 * @param  None
 * @retval Transmission status. Return is based on @ref TM_NRF24L01_Transmit_Status_t enumeration
 */
transmit_status_t TM_NRF24L01_GetTransmissionStatus(void);

/**
 * @brief  Transmits data with NRF24L01+ to another NRF module
 * @param  *data: Pointer to 8-bit array with data.
 *         Maximum length of array can be the same as "payload_size" parameter on initialization
 * @retval None
 */
void TM_NRF24L01_Transmit(uint8_t *data);

/**
 * @brief  Checks if data is ready to be read from NRF24L01+
 * @param  None
 * @retval Data ready status:
 *            - 0: No data available for receive in bufferReturns
 *            - > 0: Data is ready to be collected
 */
uint8_t TM_NRF24L01_DataReady(void);

/**
 * @brief  Gets data from NRF24L01+
 * @param  *data: Pointer to 8-bits array where data from NRF will be saved
 * @retval None
 */
void TM_NRF24L01_GetData(uint8_t *data);

/**
 * @brief  Sets working channel
 * @note   Channel value is just an offset in units MHz from 2.4GHz
 *         For example, if you select channel 65, then operation frequency will be set to 2.465GHz.
 * @param  channel: RF channel where device will operate
 * @retval None 
 */
void TM_NRF24L01_SetChannel(uint8_t channel);

/**
 * @brief  Sets RF parameters for NRF24L01+
 * @param  DataRate: Data rate selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_DataRate_t enumeration
 * @param  OutPwr: Output power selection for NRF module. This parameter can be a value of @ref TM_NRF24L01_OutputPower_t enumeration
 * @retval None
 */
void TM_NRF24L01_SetRF(air_data_rate_t DataRate, power_amplifier_t OutPwr);

/**
 * @brief  Gets NRLF+ status register value
 * @param  None
 * @retval Status register from NRF
 */
uint8_t TM_NRF24L01_GetStatus(void);

/* Private */
void TM_NRF24L01_WriteRegister(uint8_t reg, uint8_t value);
/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

