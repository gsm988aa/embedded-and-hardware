/***********************************************************************************
* Project Name           :
*
* @Revision Number       : 1.0
*
* @File Name             : nRF24L01.c
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
/***********************************************************************************
******************************** Includes ******************************************
***********************************************************************************/
#include "nRF24L01.h"
/***********************************************************************************
**************************** Global variables **************************************
***********************************************************************************/
extern SPI_HandleTypeDef hspi2;
static TM_NRF24L01_t TM_NRF24L01;
/***********************************************************************************
***************************** NRF24L01+ Private Function ***************************
***********************************************************************************/
void TM_NRF24L01_FlushTx(void);
void TM_NRF24L01_FlushRx(void);
void TM_NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value);
uint8_t TM_NRF24L01_ReadBit(uint8_t reg, uint8_t bit);
uint8_t TM_NRF24L01_ReadRegister(uint8_t reg);
void TM_NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count);
void TM_NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count);
void TM_NRF24L01_SoftwareReset(void);
uint8_t TM_NRF24L01_RxFifoEmpty(void);
/***********************************************************************************
******************************* Function code **************************************
***********************************************************************************/
void TM_NRF24L01_FlushTx(void)
{
	do
	{
		NRF24L01_CSN_LOW;
		HAL_SPI_Transmit(&hspi2, (uint8_t *)NRF24L01_FLUSH_TX_MASK, 1, 10);
		NRF24L01_CSN_HIGH;
	}while(0);
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_FlushRx(void)
{
	do
	{
		NRF24L01_CSN_LOW;
		HAL_SPI_Transmit(&hspi2, (uint8_t *)NRF24L01_FLUSH_RX_MASK, 1, 10);
		NRF24L01_CSN_HIGH;
	}while(0);
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_Init(uint8_t channel, uint8_t payload_size)
{
    NRF24L01_CSN_HIGH;
    NRF24L01_CE_LOW;
    if(payload_size > 32)payload_size = 32; /* Max payload is 32bytes */
    /* Fill structure */
    TM_NRF24L01.Channel = !channel; /* Set channel to some different value for TM_NRF24L01_SetChannel() function */
    TM_NRF24L01.PayloadSize = payload_size;
    TM_NRF24L01.OutPwr = PA_0dBm;
    TM_NRF24L01.DataRate = DR_2Mbps;
    TM_NRF24L01_SoftwareReset(); /* Reset nRF24L01+ to power on registers values */
    TM_NRF24L01_SetChannel(channel); /* Channel select */
    /* Set pipeline to max possible 32 bytes */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P0, TM_NRF24L01.PayloadSize); // Auto-ACK pipe
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P1, TM_NRF24L01.PayloadSize); // Data payload pipe
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P2, TM_NRF24L01.PayloadSize);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P3, TM_NRF24L01.PayloadSize);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P4, TM_NRF24L01.PayloadSize);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P5, TM_NRF24L01.PayloadSize);
    TM_NRF24L01_SetRF(TM_NRF24L01.DataRate, TM_NRF24L01.OutPwr); /* Set RF settings (2mbps, output power) */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, NRF24L01_CONFIG); /* Config register */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_EN_AA, 0x3F); /* Enable auto-acknowledgment for all pipes */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_EN_RXADDR, 0x3F); /* Enable RX addresses */
    /* Auto retransmit delay: 1000 (4x250) us and Up to 15 retransmit trials */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_SETUP_RETR, 0x4F); /* Dynamic length configurations: No dynamic length */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_DYNPD, (0 << NRF24L01_DPL_P0) | (0 << NRF24L01_DPL_P1) | (0 << NRF24L01_DPL_P2) | (0 << NRF24L01_DPL_P3) | (0 << NRF24L01_DPL_P4) | (0 << NRF24L01_DPL_P5));
    TM_NRF24L01_FlushTx(); /* Clear Tx FIFOs */
    TM_NRF24L01_FlushRx(); /* Clear Rx FIFOs */
    NRF24L01_CLEAR_INTERRUPTS; /* Clear interrupts */
    TM_NRF24L01_PowerUpRx(); /* Go to RX mode */
    return 1; /* Return OK */
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_SetMyAddress(uint8_t *addr)
{
    NRF24L01_CE_LOW;
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P1, addr, 5);
    NRF24L01_CE_HIGH;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_SetTxAddress(uint8_t *addr)
{
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P0, addr, 5);
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_TX_ADDR, addr, 5);
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value)
{
    uint8_t tmp;
    /* Read register */
    tmp = TM_NRF24L01_ReadRegister(reg);
    /* Make operation */
    if(value)tmp |= 1 << bit;
    else tmp &= ~(1 << bit);
    /* Write back */
    TM_NRF24L01_WriteRegister(reg, tmp);
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_ReadBit(uint8_t reg, uint8_t bit)
{
    uint8_t tmp;
    tmp = TM_NRF24L01_ReadRegister(reg);
    if (!NRF24L01_CHECK_BIT(tmp, bit))
    {
        return 0;
    }
    return 1;
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_ReadRegister(uint8_t reg)
{
    uint8_t value;
    uint8_t regVal = NRF24L01_READ_REGISTER_MASK(reg);
    NRF24L01_CSN_LOW;
    HAL_SPI_Transmit(&hspi2, &regVal, 1, 10);
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)NRF24L01_NOP_MASK, &value, 1, 10);
    NRF24L01_CSN_HIGH;
    return value;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count)
{
    uint8_t regVal = NRF24L01_READ_REGISTER_MASK(reg);
    NRF24L01_CSN_LOW;
    HAL_SPI_Transmit(&hspi2, &regVal, 1, 10);
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)NRF24L01_NOP_MASK, data, count, 10);
    NRF24L01_CSN_HIGH;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_WriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t regVal = NRF24L01_READ_REGISTER_MASK(reg);
    NRF24L01_CSN_LOW;
    HAL_SPI_Transmit(&hspi2, &regVal, 1, 10);
    HAL_SPI_Transmit(&hspi2, &data, 1, 10);
    NRF24L01_CSN_HIGH;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count)
{
    uint8_t regVal = NRF24L01_READ_REGISTER_MASK(reg);
    NRF24L01_CSN_LOW;
    HAL_SPI_Transmit(&hspi2, &regVal, 1, 10);
    HAL_SPI_Transmit(&hspi2, (uint8_t *)data, count, 10);
    NRF24L01_CSN_HIGH;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_PowerUpTx(void)
{
    NRF24L01_CLEAR_INTERRUPTS;
    uint8_t config = NRF24L01_CONFIG|(0 << NRF24L01_PRIM_RX)|(1 << NRF24L01_PWR_UP);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, config);
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_PowerUpRx(void)
{
    uint8_t config = 0;
    /* Disable RX/TX mode */
    NRF24L01_CE_LOW;
    /* Clear RX buffer */
    TM_NRF24L01_FlushRx();
    /* Clear interrupts */
    NRF24L01_CLEAR_INTERRUPTS;
    /* Setup RX mode */
    config = NRF24L01_CONFIG|(1 << NRF24L01_PWR_UP)|(1 << NRF24L01_PRIM_RX);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, config);
    /* Start listening */
    NRF24L01_CE_HIGH;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_PowerDown(void)
{
    NRF24L01_CE_LOW;
    TM_NRF24L01_WriteBit(NRF24L01_REG_CONFIG, NRF24L01_PWR_UP, 0);
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_Transmit(uint8_t *data)
{
    NRF24L01_CE_LOW; /* Chip enable put to low, disable it */
    TM_NRF24L01_PowerUpTx(); /* Go to power up tx mode */
    TM_NRF24L01_FlushTx(); /* Clear TX FIFO from NRF24L01+ */
    NRF24L01_CSN_LOW; /* Send payload to nRF24L01+ */
    HAL_SPI_Transmit(&hspi2, (uint8_t *)NRF24L01_W_TX_PAYLOAD_MASK, 1, 10); /* Send write payload command */
    HAL_SPI_Transmit(&hspi2, data, TM_NRF24L01.PayloadSize, 10); /* Fill payload with data*/
    NRF24L01_CSN_HIGH; /* Disable SPI */
    NRF24L01_CE_HIGH; /* Send data! */
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_GetData(uint8_t* data)
{
    /* Pull down chip select */
    NRF24L01_CSN_LOW;
    /* Send read payload command*/
    HAL_SPI_Transmit(&hspi2, (uint8_t *)NRF24L01_R_RX_PAYLOAD_MASK, 1, 10);
    /* Read payload */
    HAL_SPI_TransmitReceive(&hspi2, data, data, TM_NRF24L01.PayloadSize, 10);
    /* Pull up chip select */
    NRF24L01_CSN_HIGH;
    /* Reset status register, clear RX_DR interrupt flag */
    TM_NRF24L01_WriteRegister(NRF24L01_REG_STATUS, (1 << NRF24L01_RX_DR));
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_DataReady(void)
{
    uint8_t status = TM_NRF24L01_GetStatus();
    if(NRF24L01_CHECK_BIT(status, NRF24L01_RX_DR))
    {
        return 1;
    }
    return !TM_NRF24L01_RxFifoEmpty();
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_RxFifoEmpty(void)
{
    uint8_t reg = TM_NRF24L01_ReadRegister(NRF24L01_REG_FIFO_STATUS);
    return NRF24L01_CHECK_BIT(reg, NRF24L01_RX_EMPTY);
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_GetStatus(void)
{
    uint8_t status;
    NRF24L01_CSN_LOW;
    /* First received byte is always status register */
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)NRF24L01_NOP_MASK, &status, 1, 10);
    /* Pull up chip select */
    NRF24L01_CSN_HIGH;
    return status;
}
/***********************************************************************************
***********************************************************************************/
transmit_status_t TM_NRF24L01_GetTransmissionStatus(void)
{
    uint8_t status = TM_NRF24L01_GetStatus();
    if(NRF24L01_CHECK_BIT(status, NRF24L01_TX_DS))
    {
        /* Successfully sent */
        return success;
    }
    else if(NRF24L01_CHECK_BIT(status, NRF24L01_MAX_RT))
    {
        /* Message lost */
        return lost;
    }
    /* Still sending */
    return sending;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_SoftwareReset(void)
{
    uint8_t data[5];
    TM_NRF24L01_WriteRegister(NRF24L01_REG_CONFIG, NRF24L01_REG_DEFAULT_VAL_CONFIG);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_EN_AA, NRF24L01_REG_DEFAULT_VAL_EN_AA);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_EN_RXADDR, NRF24L01_REG_DEFAULT_VAL_EN_RXADDR);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_SETUP_AW, NRF24L01_REG_DEFAULT_VAL_SETUP_AW);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_SETUP_RETR, NRF24L01_REG_DEFAULT_VAL_SETUP_RETR);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RF_CH, NRF24L01_REG_DEFAULT_VAL_RF_CH);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RF_SETUP, NRF24L01_REG_DEFAULT_VAL_RF_SETUP);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_STATUS, NRF24L01_REG_DEFAULT_VAL_STATUS);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_OBSERVE_TX, NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RPD, NRF24L01_REG_DEFAULT_VAL_RPD);
    //P0
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4;
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P0, data, 5);
    //P1
    data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4;
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_RX_ADDR_P1, data, 5);
    
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P2, NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P3, NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P4, NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_ADDR_P5, NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5);
    //TX
    data[0] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0;
    data[1] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1;
    data[2] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2;
    data[3] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3;
    data[4] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4;
    TM_NRF24L01_WriteRegisterMulti(NRF24L01_REG_TX_ADDR, data, 5);
    
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P0, NRF24L01_REG_DEFAULT_VAL_RX_PW_P0);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P1, NRF24L01_REG_DEFAULT_VAL_RX_PW_P1);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P2, NRF24L01_REG_DEFAULT_VAL_RX_PW_P2);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P3, NRF24L01_REG_DEFAULT_VAL_RX_PW_P3);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P4, NRF24L01_REG_DEFAULT_VAL_RX_PW_P4);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RX_PW_P5, NRF24L01_REG_DEFAULT_VAL_RX_PW_P5);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_FIFO_STATUS, NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_DYNPD, NRF24L01_REG_DEFAULT_VAL_DYNPD);
    TM_NRF24L01_WriteRegister(NRF24L01_REG_FEATURE, NRF24L01_REG_DEFAULT_VAL_FEATURE);
}
/***********************************************************************************
***********************************************************************************/
uint8_t TM_NRF24L01_GetRetransmissionsCount(void)
{
    /* Low 4 bits */
    return TM_NRF24L01_ReadRegister(NRF24L01_REG_OBSERVE_TX) & 0x0F;
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_SetChannel(uint8_t channel)
{
    if(channel <= 125 && channel != TM_NRF24L01.Channel)
    {
        /* Store new channel setting */
        TM_NRF24L01.Channel = channel;
        /* Write channel */
        TM_NRF24L01_WriteRegister(NRF24L01_REG_RF_CH, channel);
    }
}
/***********************************************************************************
***********************************************************************************/
void TM_NRF24L01_SetRF(air_data_rate_t DataRate, power_amplifier_t OutPwr)
{
    uint8_t tmp = 0;
    TM_NRF24L01.DataRate = DataRate;
    TM_NRF24L01.OutPwr = OutPwr;
    if (DataRate == DR_2Mbps)
    {
        tmp |= 1 << NRF24L01_RF_DR_HIGH;
    }
    else if(DataRate == DR_250kbps)
    {
        tmp |= 1 << NRF24L01_RF_DR_LOW;
    }
    /* If 1Mbps, all bits set to 0 */
    if(OutPwr == PA_0dBm)
    {
        tmp |= 3 << NRF24L01_RF_PWR;
    }
    else if(OutPwr == PA_M6dBm)
    {
        tmp |= 2 << NRF24L01_RF_PWR;
    }
    else if(OutPwr == PA_M12dBm)
    {
        tmp |= 1 << NRF24L01_RF_PWR;
    }
    TM_NRF24L01_WriteRegister(NRF24L01_REG_RF_SETUP, tmp);
}
/***********************************************************************************
***********************************************************************************/
