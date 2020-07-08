/*******************************************************************************
                       Copyright by Pham Van Lap KMT
********************************************************************************
** File Name        : I2C.H
** Version          : 1.0
** Author           : Pham Van Lap
** Create Date      : 10/12/2018
** Company          : FPT Software
** Email            : quoclaptk@gmail.com
** Phone Numbers    : 0886560383 or 0963836272
*******************************************************************************/

/*******************************************************************************
                             Revision History
********************************************************************************
**    Date              Author                           Detail
** ----------       --------------      ----------------------------------------
** 10/12/2018        Pham Van Lap                      Create new
*******************************************************************************/
#ifndef _SOFT_I2C_H_
#define _SOFT_I2C_H_
/*******************************************************************************
                          Include Files
*******************************************************************************/
#include "stm8s.h"
#include "DELAY.h"
/*******************************************************************************
                          Macros/Constants
*******************************************************************************/
#define PORT_SCL            GPIOB
#define PORT_SDA            GPIOB

#define SCL_PIN             GPIO_PIN_4
#define SDA_PIN             GPIO_PIN_3

#define SCL_INPUT_MODE      GPIO_Init(PORT_SCL, SCL_PIN, GPIO_MODE_IN_PU_NO_IT)
#define SCL_OUTPUT_MODE     GPIO_Init(PORT_SCL, SCL_PIN, GPIO_MODE_OUT_PP_LOW_FAST)
#define SDA_INPUT_MODE      GPIO_Init(PORT_SDA, SDA_PIN, GPIO_MODE_IN_PU_NO_IT)
#define SDA_OUTPUT_MODE     GPIO_Init(PORT_SDA, SDA_PIN, GPIO_MODE_OUT_PP_LOW_FAST)

#define SDA_PIN_INPUT       GPIO_ReadInputPin(PORT_SDA, SDA_PIN)

#define SCL_PIN_LOW         GPIO_WriteLow(PORT_SCL, SCL_PIN)
#define SCL_PIN_HIGH        GPIO_WriteHigh(PORT_SCL, SCL_PIN)
#define SDA_PIN_LOW         GPIO_WriteLow(PORT_SDA, SDA_PIN)
#define SDA_PIN_HIGH        GPIO_WriteHigh(PORT_SDA, SDA_PIN)

#define I2C_DELAY_TIMES     10
/*******************************************************************************
                          Struct/Enums
*******************************************************************************/

/*******************************************************************************
                          Function Prototypes
*******************************************************************************/
void I2C_Init();
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WriteByte(uint8_t dat);
uint8_t I2C_ReadByte(void);
/******************************************************************************/
#endif /* _SOFT_I2C_H_ */
/*******************************************************************************
******************************** END OF FILE ***********************************
*******************************************************************************/