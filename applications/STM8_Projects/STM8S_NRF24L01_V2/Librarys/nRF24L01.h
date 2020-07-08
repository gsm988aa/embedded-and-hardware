#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include "stm8s.h"

#define MISO	        GPIO_PIN_7
#define MOSI	        GPIO_PIN_6
#define	SCK	        GPIO_PIN_5
#define	CE	        GPIO_PIN_0
#define	CSN		GPIO_PIN_1
#define IRQ		GPIO_PIN_2

#define SetCE	GPIO_WriteHigh(GPIOB, CE)
#define ClrCE	GPIO_WriteLow(GPIOB, CE)

#define SetCSN	GPIO_WriteHigh(GPIOB, CSN)
#define ClrCSN	GPIO_WriteLow(GPIOB, CSN)

#define TX_ADDR_WITDH 	 5
#define RX_ADDR_WITDH 	 5
#define TX_DATA_WITDH 	 32
#define RX_DATA_WITDH 	 32


#define  R_REGISTER      0x00
#define  W_REGISTER      0x20
#define  R_RX_PAYLOAD 	 0x61
#define  W_TX_PAYLOAD	 0xa0
#define  FLUSH_TX	 0xe1
#define  FLUSH_RX	 0xe2
#define  REUSE_TX_PL     0xe3
#define  NOP             0xff

#define  CONFIG          0x00
#define  EN_AA	         0x01
#define  EN_RXADDR       0x02
#define  SETUP_AW        0x03
#define  SETUP_RETR      0x04
#define  RF_CH           0x05
#define  RF_SETUP        0x06
#define  STATUS          0x07
#define  OBSERVE_TX      0x08
#define  CD              0x09
#define  RX_ADDR_P0      0x0a
#define  RX_ADDR_P1      0x0b
#define  RX_ADDR_P2      0x0c
#define  RX_ADDR_P3      0x0d
#define  RX_ADDR_P4      0x0e
#define  RX_ADDR_P5      0x0f
#define  TX_ADDR         0x10
#define  RX_PW_P0        0x11
#define  RX_PW_P1        0x12
#define  RX_PW_P2        0x13
#define  RX_PW_P3        0x14
#define  RX_PW_P4        0x15
#define  RX_PW_P5        0x16
#define  FIFO_STATUS     0x17

void nRF24L01_Pin_Conf();
static u8 nRF24L01_SPI_RW(u8 date);
static u8 nRF24L01_Read_Reg(u8 RegAddr);
static u8 nRF24L01_Write_Reg(u8 RegAddr,u8 data);
static u8 nRF24L01_Read_RxData(u8 RegAddr, u8 *RxData, u8 DataLen);
static u8 nRF24L01_Write_TxData(u8 RegAddr,u8 *TxData,u8 DataLen);
void nRF24L01_Set_TxMode();
void nRF24L01_SendData(u8 *data);
u8 nRRF24L01_CheckACK();
u8 nRF24L01_RevData(u8 *RevData);
void nRF24L01_Set_RxMode();
#endif