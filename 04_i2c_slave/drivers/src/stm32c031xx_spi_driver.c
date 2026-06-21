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
	uint32_t tempreg_cr1 = 0;
	uint32_t tempreg_cr2 = 0;
	PeriphStatus_t status = PERIPH_OK;

	// enable peripheral clock
	SPI_PClkCtrl(pSPIHandle->pSPIx, PERIPH_ENABLE);

	// 1. Configure the device mode
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;

	// 2. Configure the bus config
	if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// bidi mode should be cleared
		tempreg_cr1 &= ~(1 << SPI_CR1_RXONLY);
		tempreg_cr1 &= ~(1 << SPI_CR1_BIDIMODE);
	}
	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// bidi mode should be cleared
		tempreg_cr1 &= ~(1 << SPI_CR1_RXONLY);
		tempreg_cr1 |= (1 << SPI_CR1_BIDIMODE);
	}
	else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_S_RXONLY)
	{
		// bidi mode should be cleared
		tempreg_cr1 &= ~(1 << SPI_CR1_BIDIMODE);

		// RXOnly bit should be set
		tempreg_cr1 |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the spi serial clock speed (baud rate)
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	//4.  Configure the FIFO Rx threshold
	tempreg_cr2 |= (pSPIHandle->SPIConfig.SPI_FRXTH << SPI_CR2_FRXTH);

	// 5. Configure the DFF
	tempreg_cr2 |= (pSPIHandle->SPIConfig.SPI_DS << SPI_CR2_DS);

	// 6. configure the CPOL
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	// 7. configure the CPHA
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	// 8. Configure the software slave management
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM;

	// 9. Configure data endianess
	tempreg_cr1 |= pSPIHandle->SPIConfig.SPI_Endian << SPI_CR1_LSBFIRST;

	if(pSPIHandle->SPIConfig.SPI_CRCCalculation == SPI_CRC_EN)
	{
		pSPIHandle->pSPIx->CRCPR = pSPIHandle->SPIConfig.SPI_CRCPolynomial;

		tempreg_cr1 |= (1 << SPI_CR1_CRCEN);

		if (pSPIHandle->SPIConfig.SPI_DS <= SPI_DS_8BITS)
		{
			tempreg_cr1 &= ~(1 << SPI_CR1_CRCL);
		}
		else
		{
			tempreg_cr1 |= (1 << SPI_CR1_CRCL);
		}
	}

	pSPIHandle->pSPIx->CR1 = tempreg_cr1;
	pSPIHandle->pSPIx->CR2 = tempreg_cr2;
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

    while(SPI_GetFlagStatus(pSPIx, SPI_BUSY_FLAG));

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

    while(SPI_GetFlagStatus(pSPIx, SPI_BUSY_FLAG));

    return status;
}


PeriphStatus_t SPI_TransmitReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t len)
{
    uint32_t ds = (pSPIx->CR2 >> SPI_CR2_DS) & 0xF;
    uint32_t dataSize = ds + 1;
    uint8_t crc_en = (pSPIx->CR1 & (1 << SPI_CR1_CRCEN)) ? 1 : 0;
    uint32_t tx_len = len;
    uint32_t rx_len = len;

    while (tx_len > 0 || rx_len > 0)
    {
        if (tx_len > 0 && SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == SPI_FLAG_SET) {
            if (dataSize > 8) {
                pSPIx->DR = *((uint16_t*)pTxBuffer);
                pTxBuffer += 2; tx_len -= 2;
            } else {
                // Must cast to 8-bit pointer to prevent 16-bit FIFO packing!
                *((volatile uint8_t*)&pSPIx->DR) = *pTxBuffer;
                pTxBuffer++; tx_len--;
            }

            if (crc_en && tx_len == 0) {
                pSPIx->CR1 |= (1 << SPI_CR1_CRCNEXT);
            }
        }

        if (rx_len > 0 && SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == SPI_FLAG_SET) {
            if (dataSize > 8) {
                *((uint16_t*)pRxBuffer) = pSPIx->DR;
                pRxBuffer += 2; rx_len -= 2;
            } else {
                *pRxBuffer = *((volatile uint8_t*)&pSPIx->DR);
                pRxBuffer++; rx_len--;
            }
        }
    }

    if(crc_en) {
        while (SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == SPI_FLAG_RESET);
        if (SPI_GetFlagStatus(pSPIx, SPI_CRCERR_FLAG)) {
            pSPIx->SR &= ~(1 << SPI_SR_CRCERR);
            return PERIPH_ERROR;
        }
        volatile uint32_t dummy = pSPIx->DR;
        (void)dummy;
    }

    while(SPI_GetFlagStatus(pSPIx, SPI_BUSY_FLAG) == SPI_FLAG_SET);
    return PERIPH_OK;
}

// Add this to your C031 Driver too if it's missing!
void SPI_ClearCRC(SPI_RegDef_t *pSPIx) {
    pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);
    pSPIx->CR1 &= ~(1 << SPI_CR1_CRCEN);
    pSPIx->CR1 |= (1 << SPI_CR1_CRCEN);
    pSPIx->CR1 |= (1 << SPI_CR1_SPE);
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


