#include <string.h>
#include <stdio.h>

#include "stm32c031xx.h"
#include "stm32c031xx_gpio_driver.h"
#include "stm32c031xx_spi_driver.h"

extern void initialise_monitor_handles(void);

/*
 * SPI2 GPIO peripheral initialization as master mode
 *
 * PB14 -> SPI2_MISO
 * PB15 -> SPI2_MOSI - Not required
 * PB13 -> SPI2_SCK
 * PB12 -> SPI2_NSS - Not required
 * AF5 -> Alternate functionality mapping 5
 */
void SPI_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOB;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5; // SPI Alt fn
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_13;
	(void)GPIO_Init(&SPIPins);

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_15;
	(void)GPIO_Init(&SPIPins);

	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_14;
	(void)GPIO_Init(&SPIPins);

	// NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_12;
	(void)GPIO_Init(&SPIPins);
}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;
	SPI2handle.pSPIx = SPI2;
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MASTER_MODE;
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; // 8 MHz
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN; // software slave management for NSS pin

	SPI_Init(&SPI2handle);
}


int main(void)
{
	char user_data[] = "..... .....";

	initialise_monitor_handles();

	SPI_GPIOInits();

	SPI2_Inits();

	SPI_SSIConfig(SPI2, PERIPH_ENABLE);

	SPI_PCtrl(SPI2, PERIPH_ENABLE);

	SPI_ReceiveData(SPI2, (uint8_t*)user_data, strlen(user_data));

	printf(user_data);

	SPI_PCtrl(SPI2, PERIPH_DISABLE);

	for(;;);
}
