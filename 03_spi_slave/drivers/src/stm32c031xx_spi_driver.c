/*
 * stm32c031xx_spi_driver.c
 *
 *  Created on: 01-Mar-2026
 *      Author: prateek
 */

#include "stm32c031xx_spi_driver.h"


/*
 * Peripheral Clock configuration
 */
PeriphStatus_t SPI_PClkCtrl(SPI_RegDef_t *pSPIx, PeriphState_t pState)
{
	PeriphStatus_t status = PERIPH_OK;
    if (pSPIx == NULL)
        status = PERIPH_ERROR;

    if ((status == PERIPH_OK) && (pState == PERIPH_ENABLE))
    {
        if      (pSPIx == SPI1) SPI1_PCLK_EN();
        else if (pSPIx == SPI2) SPI2_PCLK_EN();
        else status = PERIPH_ERROR;
    }
    else if ((status == PERIPH_OK) && (pState == PERIPH_DISABLE))
    {
        if      (pSPIx == SPI1) SPI1_PCLK_CLR();
        else if (pSPIx == SPI2) SPI2_PCLK_CLR();
        else status = PERIPH_ERROR;
    }
    else
    {
    	status = PERIPH_ERROR;
    }

    return status;
}

/*
 * Initialization and de-initialization
 */
PeriphStatus_t SPI_Init(SPI_Handle_t *pSPIHandle)
{
	uint32_t tempreg = 0;
	PeriphStatus_t status = PERIPH_OK;

	// enable peripheral clock
	SPI_PClkCtrl(pSPIHandle->pSPIx, PERIPH_ENABLE);

	// 1. Configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	// 2. Configure the bus config
	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// bidi mode should be cleared
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	}
	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RXONLY)
	{
		// bidi mode should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);

		// RXOnly bit should be set
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the spi serial clock speed (baud rate)
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4.  Configure the DFF
	pSPIHandle->pSPIx->CR2 &= ~(0xF << SPI_CR2_DS);
	pSPIHandle->pSPIx->CR2 |= (pSPIHandle->SPIConfig.SPI_DS << SPI_CR2_DS);

	//5. configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	//6 . configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	tempreg |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	pSPIHandle->pSPIx->CR1 = tempreg;
	return status;
}


PeriphStatus_t SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	PeriphStatus_t status = PERIPH_OK;
	if      (pSPIx == SPI1) SPI1_REG_RESET();
	else if (pSPIx == SPI2) SPI2_REG_RESET();
	else status = PERIPH_ERROR;
	return status;
}

uint32_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if (pSPIx->SR & FlagName)
	{
		return SPI_FLAG_SET;
	}

	return SPI_FLAG_RESET;
}

/*
 * Data read and write
 *
 * This is a blocking function call as this function will only return when data buffer
 * is completely sent
 *
 * This is a polling based function call as TXE flag is constantly checked
 * whether the Tx buffer is clear or not. Hence if SPI peripheral is not working
 * fine then it might loop forever, hence we would need watchdog timer setup.
 */
PeriphStatus_t SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len)
{
    PeriphStatus_t status = PERIPH_OK;

    uint32_t ds = (pSPIx->CR2 >> SPI_CR2_DS) & 0xF;
    uint32_t dataSize = ds + 1;   // actual bits

    while (len > 0)
    {
        // 1. Wait until TXE is set
        while (SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == SPI_FLAG_RESET);

        if (dataSize > 8)
        {
            // 9 to 16 bits → write 16-bit
            pSPIx->DR = *((uint16_t*)pTxBuffer);

            pTxBuffer += 2;
            len -= 2;
        }
        else
        {
            // 4 to 8 bits → write 8-bit
            pSPIx->DR = *pTxBuffer;
            pTxBuffer++;
            len--;
        }
    }

    return status;
}


PeriphStatus_t SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len)
{
    PeriphStatus_t status = PERIPH_OK;

    uint32_t ds = (pSPIx->CR2 >> SPI_CR2_DS) & 0xF;
    uint32_t dataSize = ds + 1;

    while (len > 0)
    {
        // 1. Wait until RXNE is set
        while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == SPI_FLAG_RESET);

        if (dataSize > 8)
        {
            // 9 to 16 bits
            *((uint16_t*)pRxBuffer) = pSPIx->DR;

            pRxBuffer += 2;
            len -= 2;
        }
        else
        {
            // 4 to 8 bits
            *pRxBuffer = *((volatile uint8_t*)&pSPIx->DR);

            pRxBuffer++;
            len--;
        }
    }

    return status;
}

/*
 * IRQ configuration and ISR handling
 */
void SPI_IRQConfig(uint8_t IRQNumber, PeriphState_t pState)
{

}

void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{

}

void SPI_IRQHandling(uint8_t IRQNumber, uint32_t IRQPriority)
{

}



void SPI_PCtrl(SPI_RegDef_t *pSPIx, PeriphState_t pState)
{
	if (pState == PERIPH_ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, PeriphState_t pState)
{
	if (pState == PERIPH_ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}
	else
	{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);
	}
}

void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, PeriphState_t pState)
{
	if (pState == PERIPH_ENABLE)
	{
		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);
	}
	else
	{
		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}


