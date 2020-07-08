
#include <W5500_driver.h>
extern wiz_NetInfo gWIZNETINFO;
SPI_HandleTypeDef W5500_SPIx;

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
    /*Configure GPIO pins : PE7 PE8 PE9 */
    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

static void MX_SPI1_Init(void)
{
	W5500_SPIx.Instance = SPI2;
	W5500_SPIx.Init.Mode = SPI_MODE_MASTER;
	W5500_SPIx.Init.Direction = SPI_DIRECTION_2LINES;
	W5500_SPIx.Init.DataSize = SPI_DATASIZE_8BIT;
	W5500_SPIx.Init.CLKPolarity = SPI_POLARITY_LOW;
	W5500_SPIx.Init.CLKPhase = SPI_PHASE_1EDGE;
	W5500_SPIx.Init.NSS = SPI_NSS_SOFT;
	W5500_SPIx.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	W5500_SPIx.Init.FirstBit = SPI_FIRSTBIT_MSB;
	W5500_SPIx.Init.TIMode = SPI_TIMODE_DISABLE;
	W5500_SPIx.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	W5500_SPIx.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&W5500_SPIx) != HAL_OK)while(1);
}

void W5500_Reset(void)
{
	HAL_GPIO_WritePin(W5500_RESET_PORT, W5500_RESET_PIN, GPIO_PIN_RESET);
	//delay_us(500);
	HAL_GPIO_WritePin(W5500_RESET_PORT, W5500_RESET_PIN, GPIO_PIN_SET);
	//delay_ms(1);
}

void W5500_Init()
{
	MX_GPIO_Init();
	MX_SPI1_Init();
	W5500_Reset();
}

void W5500_chipInit(void)
{
	uint8_t temp;
	uint8_t W5500FifoSize[2][8] = {{2, 2, 2, 2, 2, 2, 2, 2, }, {2, 2, 2, 2, 2, 2, 2, 2}};
	W5500DeSelect();
	/* spi function register */
	reg_wizchip_spi_cbfunc(W5500ReadByte, W5500WriteByte);
	/* CS function register */
	reg_wizchip_cs_cbfunc(W5500Select, W5500DeSelect);
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*)W5500FifoSize) == -1)
	{
		printf("W5500 initialized fail.\r\n");
	}
	//check phy status
	do
	{
		if (ctlwizchip(CW_GET_PHYLINK, (void*)&temp) == -1)
		{
			printf("Unknown PHY link status.\r\n");
		}
	} while (temp == PHY_LINK_OFF);
}

void W5500WriteByte(uint8_t byte)
{
	HAL_SPI_Transmit(&W5500_SPIx, &byte, 1, 10);
}

uint8_t W5500ReadByte(void)
{
	uint8_t txData = 0xFF;
	uint8_t rxData;
	HAL_SPI_TransmitReceive(&W5500_SPIx, &txData, &rxData, 1, 10);
	return rxData;
}

void W5500Select(void)
{
	HAL_GPIO_WritePin(W5500_CS_PORT, W5500_CS_PIN, GPIO_PIN_RESET);
}

void W5500DeSelect(void)
{
	HAL_GPIO_WritePin(W5500_CS_PORT, W5500_CS_PIN, GPIO_PIN_SET);
}
