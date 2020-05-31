
#include "stm8s.h"
#include "uart2.h"
#include "nRF24L01.h"


uint8_t  TxAddr[]={0x34, 0x43, 0x10, 0x10, 0x01};
/******************************************************************************/
void Delay(u32 cnt)
{
    u32 i;
    for(; cnt > 0; cnt--)
    {
        for (i = 0; i < 3000; i++);
    }
}
/******************************************************************************/
void nRF24L01_Pin_Conf()
{
    SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,\
                SPI_CLOCKPOLARITY_LOW, SPI_CLOCKPHASE_1EDGE, \
                SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT, 0x07);
    SPI_Cmd(ENABLE);
    GPIO_Init(GPIOB, CSN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB, CE, GPIO_MODE_OUT_PP_HIGH_FAST);
}
/******************************************************************************/
static uint8_t nRF24L01_SPI_RW(uint8_t date)
{
    while(SPI_GetFlagStatus( SPI_FLAG_TXE) == RESET);
    SPI_SendData(date);
    while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    return SPI_ReceiveData();  
}
/******************************************************************************/
static uint8_t nRF24L01_Read_Reg(uint8_t RegAddr)
{
    uint8_t BackData;
    ClrCSN;
    nRF24L01_SPI_RW(RegAddr);
    BackData = nRF24L01_SPI_RW(0x00);
    SetCSN;
    return(BackData);
}
/******************************************************************************/
static uint8_t nRF24L01_Write_Reg(uint8_t RegAddr, uint8_t data)
{
    uint8_t BackData;
    ClrCSN;
    BackData = nRF24L01_SPI_RW(RegAddr);
    nRF24L01_SPI_RW(data);
    SetCSN;
    return(BackData);
}
/******************************************************************************/
static uint8_t nRF24L01_Read_RxData(uint8_t RegAddr, uint8_t *RxData, uint8_t DataLen)
{ 
    uint8_t BackData, i;
    ClrCSN;
    BackData = nRF24L01_SPI_RW(RegAddr);
    for(i = 0; i < DataLen; i++)
    {
        RxData[i] = nRF24L01_SPI_RW(0);
    } 
    SetCSN;
    return(BackData); 
}
/******************************************************************************/
static uint8_t nRF24L01_Write_TxData(uint8_t RegAddr, uint8_t *TxData, uint8_t DataLen)
{ 
    uint8_t BackData,i;
    ClrCSN;
    BackData = nRF24L01_SPI_RW(RegAddr);
    for(i = 0; i < DataLen; i++)
    {
        nRF24L01_SPI_RW(*TxData++);
    }   
    SetCSN;
    return(BackData);
}
/******************************************************************************/
void nRF24L01_Set_TxMode()
{
    ClrCE; 
    nRF24L01_Write_TxData(W_REGISTER+TX_ADDR,TxAddr,TX_ADDR_WITDH);    
    nRF24L01_Write_TxData(W_REGISTER+RX_ADDR_P0,TxAddr,TX_ADDR_WITDH);    
    nRF24L01_Write_Reg(W_REGISTER+EN_AA,0x01);
    nRF24L01_Write_Reg(W_REGISTER+EN_RXADDR,0x01);
    nRF24L01_Write_Reg(W_REGISTER+SETUP_RETR,0x0a);
    nRF24L01_Write_Reg(W_REGISTER+RF_CH,0x40);
    nRF24L01_Write_Reg(W_REGISTER+RF_SETUP,0x07);
    nRF24L01_Write_Reg(W_REGISTER+CONFIG,0x0e);
    SetCE;
    Delay(5);
}
/******************************************************************************/
void nRF24L01_SendData(uint8_t *data)
{
    ClrCE;
    nRF24L01_Write_TxData(W_TX_PAYLOAD, data, TX_DATA_WITDH);
    SetCE;
    Delay(5);
}
/******************************************************************************/
uint8_t nRRF24L01_CheckACK()
{  
    uint8_t sta;
    sta=nRF24L01_Read_Reg(R_REGISTER+STATUS);
    if((sta&0x20)||(sta&0x10))
    {
       nRF24L01_Write_Reg(W_REGISTER+STATUS,0xff);
       ClrCSN;
       nRF24L01_SPI_RW(FLUSH_TX);
       SetCSN; 
       return 0;
    }
    else return 1;
}
/******************************************************************************/
uint8_t nRF24L01_RevData(uint8_t *RevData)
{
    uint8_t RevFlags = 1;
    uint8_t sta;
    sta = nRF24L01_Read_Reg(R_REGISTER+STATUS);
    if(sta & 0x40)
    {
        ClrCE;             
        nRF24L01_Read_RxData(R_RX_PAYLOAD,RevData,RX_DATA_WITDH);
        nRF24L01_Write_Reg(W_REGISTER+STATUS,0xff); 
        ClrCSN;
        nRF24L01_SPI_RW(FLUSH_RX);
        SetCSN;    
        RevFlags = 0;       
    }
    return(RevFlags);
}
/******************************************************************************/
void nRF24L01_Set_RxMode()
{
    ClrCE; 
    nRF24L01_Write_TxData(W_REGISTER+RX_ADDR_P0, TxAddr, TX_ADDR_WITDH);  
    nRF24L01_Write_Reg(W_REGISTER+EN_AA, 0x01);
    nRF24L01_Write_Reg(W_REGISTER+EN_RXADDR, 0x01);
    nRF24L01_Write_Reg(W_REGISTER+RF_CH, 0x40);
    nRF24L01_Write_Reg(W_REGISTER+RX_PW_P0, TX_DATA_WITDH);
    nRF24L01_Write_Reg(W_REGISTER+RF_SETUP, 0x07);
    nRF24L01_Write_Reg(W_REGISTER+CONFIG, 0x0f);
    SetCE;
    Delay(5);
}
/******************************************************************************/