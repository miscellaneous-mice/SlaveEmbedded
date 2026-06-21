/*
 * stm32c031xx_i2c_driver.h
 *
 *  Created on: 21-Jun-2026
 *      Author: prateek
 */

#ifndef STM32C031XX_I2C_DRIVER_H_
#define STM32C031XX_I2C_DRIVER_H_

#include <stddef.h>
#include "stm32c031xx.h"

typedef struct {
	uint8_t PRESC_VALUE;
	uint8_t SCLDEL_VALUE;
	uint8_t SDADEL_VALUE;
	uint8_t SCLH_VALUE;
	uint8_t SCLL_VALUE;
} I2C_Timing_t;

typedef struct {
	I2C_Timing_t 	I2C_SCL;
	uint8_t  		I2C_DeviceAddress;
	uint8_t  		I2C_ACKControl;
} I2C_Config_t;

typedef struct {
	I2C_RegDef_t *pI2Cx;
	I2C_Config_t I2C_Config;
} I2C_Handle_t;

/* Bit position definitions I2C_CR1 */
typedef enum
{
    I2C_CR1_PE            = 0,
    I2C_CR1_TXIE          = 1,
    I2C_CR1_RXIE          = 2,
    I2C_CR1_ADDRIE        = 3,
    I2C_CR1_NACKIE        = 4,
    I2C_CR1_STOPIE        = 5,
    I2C_CR1_TCIE          = 6,
    I2C_CR1_ERRIE         = 7,
    I2C_CR1_DNF           = 8,   // DNF[3:0] occupies bits 11:8
    I2C_CR1_ANFOFF        = 12,
    I2C_CR1_TXDMAEN       = 14,
    I2C_CR1_RXDMAEN       = 15,
    I2C_CR1_SBC           = 16,
    I2C_CR1_NOSTRETCH     = 17,
    I2C_CR1_WUPEN         = 18,
    I2C_CR1_GCEN          = 19,
    I2C_CR1_SMBHEN        = 20,
    I2C_CR1_SMBDEN        = 21,
    I2C_CR1_ALERTEN       = 22,
    I2C_CR1_PECEN         = 23

} I2C_CR1_BitPos_t;


/* Bit position definitions I2C_CR2 */
typedef enum
{
    I2C_CR2_SADD      = 0,   // SADD[9:0]  bits 0..9
    I2C_CR2_RD_WRN    = 10,  // Transfer direction
    I2C_CR2_ADD10     = 11,  // 10-bit addressing mode
    I2C_CR2_HEAD10R   = 12,  // 10-bit address header only read
    I2C_CR2_START     = 13,  // Start generation
    I2C_CR2_STOP      = 14,  // Stop generation
    I2C_CR2_NACK      = 15,  // NACK generation

    I2C_CR2_NBYTES    = 16,  // NBYTES[7:0] bits 16..23
    I2C_CR2_RELOAD    = 24,  // Reload mode
    I2C_CR2_AUTOEND   = 25,  // Automatic end mode
    I2C_CR2_PECBYTE   = 26   // Packet error checking byte
} I2C_CR2_BitPos_t;


/* Bit position definitions I2C_ISR */
typedef enum
{
    I2C_ISR_TXE        = 0,   // Transmit data register empty
    I2C_ISR_TXIS       = 1,   // Transmit interrupt status
    I2C_ISR_RXNE       = 2,   // Receive data register not empty
    I2C_ISR_ADDR       = 3,   // Address matched
    I2C_ISR_NACKF      = 4,   // NACK received flag
    I2C_ISR_STOPF      = 5,   // STOP detection flag
    I2C_ISR_TC         = 6,   // Transfer complete
    I2C_ISR_TCR        = 7,   // Transfer complete reload
    I2C_ISR_BERR       = 8,   // Bus error
    I2C_ISR_ARLO       = 9,   // Arbitration lost
    I2C_ISR_OVR        = 10,  // Overrun/Underrun
    I2C_ISR_PECERR     = 11,  // PEC error in reception
    I2C_ISR_TIMEOUT    = 12,  // Timeout or tLOW detection
    I2C_ISR_ALERT      = 13,  // SMBus alert
    I2C_ISR_BUSY       = 15,  // Bus busy

    I2C_ISR_DIR        = 16,  // Transfer direction

    I2C_ISR_ADDCODE    = 17   // ADDCODE[6:0] occupies bits 23:17

} I2C_ISR_BitPos_t;


typedef enum
{
    I2C_ICR_ADDRCF      = 3,   /* Address matched clear flag */
    I2C_ICR_NACKCF      = 4,   /* NACK received clear flag */
    I2C_ICR_STOPCF      = 5,   /* STOP detection clear flag */

    I2C_ICR_BERRCF      = 8,   /* Bus error clear flag */
    I2C_ICR_ARLOCF      = 9,   /* Arbitration lost clear flag */
    I2C_ICR_OVRCF       = 10,  /* Overrun/Underrun clear flag */
    I2C_ICR_PECCF       = 11,  /* PEC error clear flag */
    I2C_ICR_TIMOUTCF    = 12,  /* Timeout clear flag */
    I2C_ICR_ALERTCF     = 13   /* SMBus alert clear flag */

} I2C_ICR_BitPos_t;


/* Bit position definitions I2C_CCR */
typedef enum
{
    I2C_TIMINGR_SCLL     = 0,   // SCL low period   (bits 7:0)
    I2C_TIMINGR_SCLH     = 8,   // SCL high period  (bits 15:8)
    I2C_TIMINGR_SDADEL   = 16,  // Data hold time   (bits 19:16)
    I2C_TIMINGR_SCLDEL   = 20,  // Data setup time  (bits 23:20)
    I2C_TIMINGR_PRESC    = 28   // Prescaler        (bits 31:28)
} I2C_TIMINGR_BitPos_t;


/*
 * @I2C_SCLSpeed
 */
//#define PRESC_VALUE		0xBU
//#define SCLDEL_VALUE	0x4U
//#define SDADEL_VALUE	0x2U
//#define SCLH_VALUE		0xFU
//#define SCLL_VALUE		0x13U


/*
 * @I2C_ACKControl (CR1)
 */
#define I2C_ACK_ENABLE  1
#define I2C_ACK_DISABLE 0


/*
 * @I2C_flag (CR1)
 */
#define I2C_FLAG_SET   1
#define I2C_FLAG_RESET 0


/*
 * I2C Status flags
 */
#define I2C_FLAG_TXE        (1U << I2C_ISR_TXE)
#define I2C_FLAG_TXIS       (1U << I2C_ISR_TXIS)
#define I2C_FLAG_RXNE       (1U << I2C_ISR_RXNE)
#define I2C_FLAG_ADDR       (1U << I2C_ISR_ADDR)
#define I2C_FLAG_NACKF      (1U << I2C_ISR_NACKF)
#define I2C_FLAG_STOPF      (1U << I2C_ISR_STOPF)
#define I2C_FLAG_TC         (1U << I2C_ISR_TC)
#define I2C_FLAG_TCR        (1U << I2C_ISR_TCR)
#define I2C_FLAG_BERR       (1U << I2C_ISR_BERR)
#define I2C_FLAG_ARLO       (1U << I2C_ISR_ARLO)
#define I2C_FLAG_OVR        (1U << I2C_ISR_OVR)
#define I2C_FLAG_PECERR     (1U << I2C_ISR_PECERR)
#define I2C_FLAG_TIMEOUT    (1U << I2C_ISR_TIMEOUT)
#define I2C_FLAG_ALERT      (1U << I2C_ISR_ALERT)
#define I2C_FLAG_BUSY       (1U << I2C_ISR_BUSY)
#define I2C_FLAG_DIR        (1U << I2C_ISR_DIR)
#define I2C_FLAG_ADDCODE    (0x7FU << I2C_ISR_ADDCODE)


/*
 * @I2C_FMDutyCycle (CCR)
 */
#define I2C_FM_DUTY_2    0 // t(low) / t(high) = 2
#define I2C_FM_DUTY_16_9 1 // t(low) / t(high) = 16 / 9


PeriphStatus_t I2C_PClkCtrl(I2C_RegDef_t *pI2Cx, PeriphState_t pState);
PeriphStatus_t I2C_Init(I2C_Handle_t *pI2CHandle);
PeriphStatus_t I2C_DeInit(I2C_RegDef_t *pI2Cx);

PeriphStatus_t I2C_Master_SendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint16_t SlaveAddr);
PeriphStatus_t I2C_Master_ReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t len);

PeriphStatus_t I2C_Slave_SendData(I2C_RegDef_t *pI2Cx, uint8_t *pTxBuffer, uint32_t len);
PeriphStatus_t I2C_Slave_ReceiveData(I2C_RegDef_t *pI2Cx, uint8_t *pRxBuffer, uint32_t len);

void I2C_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority);
void I2C_IRQInterruptConfig(uint8_t IRQNumber, PeriphState_t EnorDi);

uint32_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);

/*
 * Other peripheral control API's
 */
void I2C_PCtrl(I2C_RegDef_t *pI2Cx, PeriphState_t pState);
void I2C_SSIConfig(I2C_RegDef_t *pI2Cx, PeriphState_t pState);
void I2C_SSOEConfig(I2C_RegDef_t *pI2Cx, PeriphState_t pState);


/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pSPIHandle, uint8_t event);


#endif /* STM32C031XX_I2C_DRIVER_H_ */
