/*
 * stm32c031xx_i2c_driver.c
 *
 *  Created on: 21-Jun-2026
 *      Author: prateek
 */


#include "stm32c031xx_i2c_driver.h"
#include <stdio.h>


uint32_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if (pI2Cx->ISR & FlagName)
	{
		return I2C_FLAG_SET;
	}

    return I2C_FLAG_RESET;
}

PeriphStatus_t I2C_PClkCtrl(I2C_RegDef_t *pI2Cx, PeriphState_t pState)
{
    if (pI2Cx == NULL) return PERIPH_ERROR;

    if (pState == PERIPH_ENABLE) {
        if      (pI2Cx == I2C1) I2C1_PCLK_EN();
        else if (pI2Cx == I2C2) I2C2_PCLK_EN();
        else return PERIPH_ERROR;
    } else {
        if      (pI2Cx == I2C1) I2C1_PCLK_CLR();
        else if (pI2Cx == I2C2) I2C2_PCLK_CLR();
        else return PERIPH_ERROR;
    }
    return PERIPH_OK;
}


/*
 * Call order per RM0490 Figure 225:
 *   1. I2C_PClkCtrl(pI2CHandle->pI2Cx, PERIPH_ENABLE) - Enable APB clock
 *   2. I2C_Init(pI2CHandle)                           - Programs filters, timings, OAR1, enables PE
 * Peripheral is immediately ready for communication.
 */
PeriphStatus_t I2C_Init(I2C_Handle_t *pI2CHandle)
{
    uint32_t tempreg = 0;

    I2C_PClkCtrl(pI2CHandle->pI2Cx, PERIPH_ENABLE);

    if (pI2CHandle == NULL || pI2CHandle->pI2Cx == NULL)
    {
        return PERIPH_ERROR;
    }

    /* 1. Ensure peripheral is disabled before modifying TIMINGR, OAR1, or CR1 filters
     * (RM0490 Section 25.4.5, 25.9.5, Figure 225) */
    pI2CHandle->pI2Cx->CR1 &= ~(1U << I2C_CR1_PE);

    /* RM0490 page 10 Note: Wait/read back to ensure PE=0 has propagated through APB */
    while (pI2CHandle->pI2Cx->CR1 & (1U << I2C_CR1_PE));

    /* 2. Configure Noise Filters (RM0490 Figure 225 & Section 25.4.5)
     * ANFOFF = 0: Analog noise filter enabled (suppresses spikes <= 50ns)
     * DNF[3:0] = 0: Digital filter disabled */
    pI2CHandle->pI2Cx->CR1 &= ~((1U << I2C_CR1_ANFOFF) | (0xFU << I2C_CR1_DNF));

    /* 3. Configure Timing Register (RM0490 25.9.5, Page 58) */
    tempreg = 0;
    tempreg |= ((uint32_t)(pI2CHandle->I2C_Config.I2C_SCL.PRESC_VALUE  & 0x0FU) << 28);
    tempreg |= ((uint32_t)(pI2CHandle->I2C_Config.I2C_SCL.SCLDEL_VALUE & 0x0FU) << 20);
    tempreg |= ((uint32_t)(pI2CHandle->I2C_Config.I2C_SCL.SDADEL_VALUE & 0x0FU) << 16);
    tempreg |= ((uint32_t)(pI2CHandle->I2C_Config.I2C_SCL.SCLH_VALUE   & 0xFFU) << 8);
    tempreg |= ((uint32_t)(pI2CHandle->I2C_Config.I2C_SCL.SCLL_VALUE   & 0xFFU));

    pI2CHandle->pI2Cx->TIMINGR = tempreg;

    /* 4. Configure Own Address 1 (RM0490 25.9.3, Page 56 & Figure 228, Page 14) */
    /* Step 4a: Disable OA1EN first - OA1[9:0] & OA1MODE are write-protected if OA1EN=1 */
    pI2CHandle->pI2Cx->OAR1 &= ~(1U << 15);

    if (pI2CHandle->I2C_Config.I2C_DeviceAddress != 0)
    {
        tempreg = 0;
        if (pI2CHandle->I2C_Config.I2C_DeviceAddress <= 0x7F)
        {
            /* 7-bit mode: address is placed in OA1[7:1] */
            tempreg |= ((uint32_t)pI2CHandle->I2C_Config.I2C_DeviceAddress << 1);
        }
        else
        {
            /* 10-bit mode: OA1MODE = 1, address in OA1[9:0] */
            tempreg |= (1U << 10);
            tempreg |= (pI2CHandle->I2C_Config.I2C_DeviceAddress & 0x3FFU);
        }

        /* Step 4b: Write address and assert OA1EN in OAR1 */
        tempreg |= (1U << 15); /* OA1EN = 1 */
        pI2CHandle->pI2Cx->OAR1 = tempreg;
    }
    else
    {
        pI2CHandle->pI2Cx->OAR1 = 0; /* Keep target address disabled for master-only */
    }

//    pI2CHandle->pI2Cx->CR1 |= (1U << I2C_CR1_SBC);

    /* 5. Enable Peripheral (RM0490 Figure 225 Step 6) */
    pI2CHandle->pI2Cx->CR1 |= (1U << I2C_CR1_PE);

    return PERIPH_OK;
}


PeriphStatus_t I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
    if      (pI2Cx == I2C1) I2C1_REG_RESET();
    else if (pI2Cx == I2C2) I2C2_REG_RESET();
    else return PERIPH_ERROR;
    return PERIPH_OK;
}


void I2C_PCtrl(I2C_RegDef_t *pI2Cx, PeriphState_t pState)
{
	if (pState == PERIPH_ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else
	{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}


PeriphStatus_t I2C_Master_SendData(I2C_Handle_t *pI2CHandle, uint8_t address, uint8_t *transmitBuffer, uint8_t size)
{
	//Check for NULL Pointer
	if(transmitBuffer == 0)
		return PERIPH_ERROR;

    // 1. Wait for Bus to be Idle
    while(pI2CHandle->pI2Cx->ISR & (0x1UL << I2C_ISR_BUSY));

    // 2. Clear the CR2 register
    pI2CHandle->pI2Cx->CR2 = 0;

    // 3. Configure CR2 for the packet (Write Direction is 0)
    uint32_t tmpreg = 0;
    // CRITICAL: Shift Address Left by 1
    tmpreg |= ((address << 1) & (0x3FFUL << I2C_CR2_SADD));
    tmpreg |= ((size << I2C_CR2_NBYTES) & (0xFFUL << I2C_CR2_NBYTES));
    tmpreg |= (0x1UL << I2C_CR2_AUTOEND);
    tmpreg |= (0x1UL << I2C_CR2_START);

    // 4. Write to CR2 and Initiate Transfer
    pI2CHandle->pI2Cx->CR2 = tmpreg;

    // 5. Transmit Loop
    for(int i = 0; i < size; i++)
    {
        // Wait for TXIS (Transmit Interrupt Status) and check for errors
        while( !(pI2CHandle->pI2Cx->ISR & (0x1UL << I2C_ISR_TXIS)) )
        {
            // Check for NACK (Address or Data NACK'd)
            if(pI2CHandle->pI2Cx->ISR & (0x1UL << I2C_ISR_NACKF))
            {
            	// NACK Received (Peripheral failed to respond)
            	pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_NACKCF);
                return PERIPH_ERROR;
            }
        }

        // Write the current byte from the buffer and advance the pointer
        pI2CHandle->pI2Cx->TXDR = *transmitBuffer++;
    }

    // 6. Wait for STOP flag (generated by AUTOEND)
    while(!(pI2CHandle->pI2Cx->ISR & (0x1UL << I2C_ISR_STOPF)));

    // 7. Clear STOP flag
    pI2CHandle->pI2Cx->ICR |= (1 << I2C_ICR_STOPCF);

    return PERIPH_OK;
}


PeriphStatus_t I2C_Master_ReceiveData(I2C_Handle_t *pI2CHandle,
                                      uint8_t address,
                                      uint8_t *readBuffer,
                                      uint8_t size)
{
    // Check for NULL Pointer
    if(readBuffer == 0)
        return PERIPH_ERROR;

    // 1. Ensure the bus is ready (not busy)
    while(pI2CHandle->pI2Cx->ISR & (0x1UL << I2C_ISR_BUSY));

    // 2. Clear the CR2 configuration fields
    pI2CHandle->pI2Cx->CR2 &= ~(
          (0x3FFUL << I2C_CR2_SADD)
        | (0xFFUL  << I2C_CR2_NBYTES)
        | (0x1UL   << I2C_CR2_RELOAD)
        | (0x1UL   << I2C_CR2_AUTOEND)
        | (0x1UL   << I2C_CR2_RD_WRN)
        | (0x1UL   << I2C_CR2_START)
        | (0x1UL   << I2C_CR2_STOP)
    );

    // 3. Configure CR2 for this packet
    // 7-bit slave address -> shifted left by 1
    // Read direction -> RD_WRN = 1
    uint32_t tmpreg = 0;

    tmpreg |= ((address << 1) &
               (0x3FFUL << I2C_CR2_SADD));

    tmpreg |= ((size << I2C_CR2_NBYTES) &
               (0xFFUL << I2C_CR2_NBYTES));

    tmpreg |= (0x1UL << I2C_CR2_RD_WRN);

    // Automatically generate STOP after NBYTES
    tmpreg |= (0x1UL << I2C_CR2_AUTOEND);

    // Generate START
    tmpreg |= (0x1UL << I2C_CR2_START);

    // 4. Start the transfer
    pI2CHandle->pI2Cx->CR2 = tmpreg;

    // 5. Receive loop
    for(int i = 0; i < size; i++)
    {
        // Wait for RXNE
        while(!(pI2CHandle->pI2Cx->ISR &
                (0x1UL << I2C_ISR_RXNE)))
        {
            // Check for NACK
            if(pI2CHandle->pI2Cx->ISR &
               (0x1UL << I2C_ISR_NACKF))
            {
                // Clear NACK flag
                pI2CHandle->pI2Cx->ICR |=
                    (0x1UL << I2C_ICR_NACKCF);

                return PERIPH_ERROR;
            }
        }

        // Read received byte from RXDR
        *readBuffer++ =
            (uint8_t)pI2CHandle->pI2Cx->RXDR;
    }

    // 6. Wait for STOP generated by AUTOEND
    while(!(pI2CHandle->pI2Cx->ISR &
            (0x1UL << I2C_ISR_STOPF)));

    // 7. Clear STOP flag
    pI2CHandle->pI2Cx->ICR |=
        (0x1UL << I2C_ICR_STOPCF);

    return PERIPH_OK;
}


PeriphStatus_t I2C_Slave_SendData(I2C_RegDef_t *pI2Cx, uint8_t *pTxBuffer, uint32_t len)
{
    uint32_t count = 0;
    uint16_t address;

    if (pI2Cx == NULL || pTxBuffer == NULL)
        return PERIPH_ERROR;

    /* 1. Wait for address match (RM0490 Figure 229) */
    while (!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR));

    address = ((pI2Cx->ISR & I2C_FLAG_ADDCODE) >> I2C_ISR_ADDCODE);

    /* 2. DIR=1 means controller wants to read from us. If writing (DIR=0), abort. */
    if (!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_DIR))
    {
        pI2Cx->ICR |= (1U << I2C_ICR_ADDRCF);
        return PERIPH_ERROR;
    }

    /* 3. Flush any stale byte left in TXDR before releasing SCL (RM0490 25.4.8) */
    pI2Cx->ISR |= (1U << I2C_ISR_TXE);

    /* 4. Clear ADDR to release the SCL clock stretch */
    pI2Cx->ICR |= (1U << I2C_ICR_ADDRCF);

    /* 5. Transfer loop */
    while (1)
    {
        /* Check for unrecoverable bus faults during transmission */
//        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ARLO) || I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BERR))
//        {
//            pI2Cx->ICR |= (1U << I2C_ICR_ARLOCF) | (1U << I2C_ICR_BERRCF);
//            pI2Cx->ISR |= (1U << I2C_ISR_TXE);
//            return PERIPH_ERROR;
//        }

        /* Controller sent NACK: Normal end of read request */
        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_NACKF))
        {
            pI2Cx->ICR |= (1U << I2C_ICR_NACKCF);

            /* Wait for the bus cycle to terminate with STOP or Repeated START */
//            while (!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_STOPF) &&
//                   !I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR)  &&
//                   !I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ARLO)  &&
//                   !I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BERR));

            if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_STOPF))
            {
                pI2Cx->ICR |= (1U << I2C_ICR_STOPCF);
            }

            pI2Cx->ISR |= (1U << I2C_ISR_TXE); /* Flush TXDR */
            break;
        }

        /* Direct STOP condition */
        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_STOPF))
        {
            pI2Cx->ICR |= (1U << I2C_ICR_STOPCF);
            pI2Cx->ISR |= (1U << I2C_ISR_TXE);
            break;
        }

        /* Repeated START: Hand off to subsequent call without clearing ADDR */
        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR))
        {
            pI2Cx->ISR |= (1U << I2C_ISR_TXE);
            break;
        }

        /* Transmit register empty and ready for data */
        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_TXIS))
        {
            pI2Cx->TXDR = (count < len) ? pTxBuffer[count++] : 0xFFU;
            printf("Got byte...\n");
        }
    }

    return PERIPH_OK;
}


PeriphStatus_t I2C_Slave_ReceiveData(I2C_RegDef_t *pI2Cx, uint8_t *pRxBuffer, uint32_t len)
{
    uint32_t count = 0;

    if (pI2Cx == NULL || pRxBuffer == NULL)
        return PERIPH_ERROR;

    /* 1. Wait for address match (RM0490 Figure 232) */
    while (!I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR));

    /* 2. DIR=0 means controller wants to write. If reading (DIR=1), abort. */
    if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_DIR))
    {
        pI2Cx->ICR |= (1U << I2C_ICR_ADDRCF);
        return PERIPH_ERROR;
    }

    /* 3. Clear ADDR to release the SCL clock stretch */
    pI2Cx->ICR |= (1U << I2C_ICR_ADDRCF);

    /* 4. Receive loop */
    while (1)
    {
        /* Bus error check during active reception */
//        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ARLO) || I2C_GetFlagStatus(pI2Cx, I2C_FLAG_BERR))
//        {
//            pI2Cx->ICR |= (1U << I2C_ICR_ARLOCF) | (1U << I2C_ICR_BERRCF);
//            return PERIPH_ERROR;
//        }

        /* Priority 1: Drain RXDR whenever new data is available */
        if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_RXNE))
        {
            uint8_t received = (uint8_t)(pI2Cx->RXDR);
            if (count < len)
            {
                pRxBuffer[count++] = received;
            }
        }
        /* Priority 2: Controller completed transmission via STOP */
        else if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_STOPF))
        {
            pI2Cx->ICR |= (1U << I2C_ICR_STOPCF);
            break;
        }
        /* Priority 3: Repeated START */
        else if (I2C_GetFlagStatus(pI2Cx, I2C_FLAG_ADDR))
        {
            break;
        }
    }

    return PERIPH_OK;
}

