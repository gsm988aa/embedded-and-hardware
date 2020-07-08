/*
 * onewire.h
 *
 *  Created on: Jul 10, 2019
 *      Author: quoclaptk
 */

#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "stm32f4xx.h"
#include "delay.h"

#define ONEWIRE_DELAY(x)				Delay(x)
/* Pin settings */
#define ONEWIRE_LOW(structure)			HAL_GPIO_WritePin((structure)->GPIOx, (structure)->GPIO_Pin, GPIO_PIN_RESET)
#define ONEWIRE_HIGH(structure)			HAL_GPIO_WritePin((structure)->GPIOx, (structure)->GPIO_Pin, GPIO_PIN_SET)
#define ONEWIRE_INPUT(structure)		this->SetPinAsInput(structure->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_OUTPUT(structure)		this->SetPinAsOutput(structure->GPIOx, (structure)->GPIO_Pin)

/* OneWire commands */
#define ONEWIRE_CMD_RSCRATCHPAD			0xBE
#define ONEWIRE_CMD_WSCRATCHPAD			0x4E
#define ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define ONEWIRE_CMD_RECEEPROM			0xB8
#define ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define ONEWIRE_CMD_SEARCHROM			0xF0
#define ONEWIRE_CMD_READROM				0x33
#define ONEWIRE_CMD_MATCHROM			0x55
#define ONEWIRE_CMD_SKIPROM				0xCC

typedef struct {
	GPIO_TypeDef* GPIOx;           /*!< GPIOx port to be used for I/O functions */
	uint16_t GPIO_Pin;             /*!< GPIO Pin to be used for I/O functions */
	uint8_t LastDiscrepancy;       /*!< Search private */
	uint8_t LastFamilyDiscrepancy; /*!< Search private */
	uint8_t LastDeviceFlag;        /*!< Search private */
	uint8_t ROM_NO[8];             /*!< 8-bytes address of last search device */
} OneWire_t;

class OneWire {
private:
	void FamilySkipSetup(OneWire_t* OneWireStruct);
	void TargetSetup(OneWire_t* OneWireStruct, uint8_t family_code);
	uint8_t Verify(OneWire_t* OneWireStruct);
	void SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	void SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	bool GetInputPinValue(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

public:
	void Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	uint8_t Reset(OneWire_t* OneWireStruct);
	uint8_t ReadByte(OneWire_t* OneWireStruct);
	void WriteByte(OneWire_t* OneWireStruct, uint8_t byte);
	void WriteBit(OneWire_t* OneWireStruct, uint8_t bit);
	uint8_t ReadBit(OneWire_t* OneWireStruct);
	uint8_t Search(OneWire_t* OneWireStruct, uint8_t command);
	void ResetSearch(OneWire_t* OneWireStruct);
	uint8_t First(OneWire_t* OneWireStruct);
	uint8_t Next(OneWire_t* OneWireStruct);
	uint8_t GetROM(OneWire_t* OneWireStruct, uint8_t index);
	void GetFullROM(OneWire_t* OneWireStruct, uint8_t *firstIndex);
	void Select(OneWire_t* OneWireStruct, uint8_t* addr);
	void SelectWithPointer(OneWire_t* OneWireStruct, uint8_t* ROM);
	uint8_t CRC8(uint8_t* addr, uint8_t len);
};
#endif /* _ONEWIRE_H_ */
