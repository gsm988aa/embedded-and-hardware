/*******************************************************************************
*******************************************************************************/
#ifndef _SOFT_I2C_H_
#define _SOFT_I2C_H_
/******************************************************************************/
#include "stm8s.h"
/******************************************************************************/
#define PORT_SCL            GPIOB
#define PORT_SDA            GPIOB

#define SCL_PIN             GPIO_PIN_4
#define SDA_PIN             GPIO_PIN_5

#define SCL_INPUT_MODE      GPIO_Init(PORT_SCL, SCL_PIN, GPIO_MODE_IN_FL_NO_IT)
#define SCL_OUTPUT_MODE     GPIO_Init(PORT_SCL, SCL_PIN, GPIO_MODE_OUT_OD_LOW_SLOW)
#define SDA_INPUT_MODE      GPIO_Init(PORT_SDA, SDA_PIN, GPIO_MODE_IN_FL_NO_IT)
#define SDA_OUTPUT_MODE     GPIO_Init(PORT_SDA, SDA_PIN, GPIO_MODE_OUT_OD_LOW_SLOW)

#define SDA_PIN_INPUT       GPIO_ReadInputPin(PORT_SDA, SDA_PIN)

#define SCL_PIN_LOW         GPIO_WriteLow(PORT_SCL, SCL_PIN)
#define SCL_PIN_HIGH        GPIO_WriteHigh(PORT_SCL, SCL_PIN)
#define SDA_PIN_LOW         GPIO_WriteLow(PORT_SDA, SDA_PIN)
#define SDA_PIN_HIGH        GPIO_WriteHigh(PORT_SDA, SDA_PIN)

#define I2C_DELAY_TIMES     10
/******************************************************************************/
void I2C_Init();
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WriteByte(uint8_t dat);
uint8_t I2C_ReadByte(void);
/******************************************************************************/
#endif /* _SOFT_I2C_H_ */
/******************************************************************************/