/*******************************************************************************
** File Name:  i2c_driver.h
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_
#include "stm8s.h"
/******************************************************************************/
#define SDA_OUT         PC_ODR_ODR6
#define SDA_IN          PC_IDR_IDR6
#define SCL             PC_ODR_ODR4

#define SDA_DDR         PC_DDR_DDR6
#define SDA_CR1         PC_CR1_C16
#define SDA_CR2         PC_CR2_C26
/******************************************************************************/
void I2C_Delay(u16 Count);
void I2C_SDA_DDR(uint8_t ddr);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_WriteByte(uint8_t DAT);
uint8_t I2C_ReadByteNACK(void);
uint8_t I2C_ReadByteACK(void);
#endif /* I2C_DRIVER_H_ */
/*******************************************************************************
**                            END OF FILE                                     **
*******************************************************************************/