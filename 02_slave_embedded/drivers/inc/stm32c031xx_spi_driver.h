/*
 * stm32c031xx_spi_driver.h
 *
 *  Created on: 01-Mar-2026
 *      Author: prateek
 */

#ifndef INC_STM32C031XX_SPI_DRIVER_H_
#define INC_STM32C031XX_SPI_DRIVER_H_


#include <stddef.h>
#include "stm32c031xx.h"


#define SPI_TXE_FLAG	(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG	(1 << SPI_SR_BSY)


/************ Important configuration structures *************/

typedef struct {
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DS;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
} SPI_Config_t;

typedef struct {
	SPI_RegDef_t *pSPIx;
	SPI_Config_t SPIConfig;
} SPI_Handle_t;


/******************************************************************************************
 *Bit position definitions of SPI peripheral
 ******************************************************************************************/
/*
 * Bit position definitions SPI_CR1
 */
typedef enum
{
    SPI_CR1_CPHA       = 0,
    SPI_CR1_CPOL       = 1,
    SPI_CR1_MSTR       = 2,
    SPI_CR1_BR         = 3,   /* BR[2:0] = bits 5:3 */
    SPI_CR1_SPE        = 6,
    SPI_CR1_LSBFIRST   = 7,
    SPI_CR1_SSI        = 8,
    SPI_CR1_SSM        = 9,
    SPI_CR1_RXONLY     = 10,
    SPI_CR1_CRCL       = 11,
    SPI_CR1_CRCNEXT    = 12,
    SPI_CR1_CRCEN      = 13,
    SPI_CR1_BIDIOE     = 14,
    SPI_CR1_BIDIMODE   = 15
} SPI_CR1_BitPos_t;


typedef enum
{
    SPI_CR2_RXDMAEN   = 0,
    SPI_CR2_TXDMAEN   = 1,
    SPI_CR2_SSOE      = 2,
    SPI_CR2_NSSP      = 3,
    SPI_CR2_FRF       = 4,
    SPI_CR2_ERRIE     = 5,
    SPI_CR2_RXNEIE    = 6,
    SPI_CR2_TXEIE     = 7,
    SPI_CR2_DS        = 8,   /* DS[3:0] = bits 11:8 */
    SPI_CR2_FRXTH     = 12,
    SPI_CR2_LDMARX    = 13,
    SPI_CR2_LDMATX    = 14
} SPI_CR2_BitPos_t;


typedef enum
{
    SPI_SR_RXNE     = 0,
    SPI_SR_TXE      = 1,
    SPI_SR_CHSIDE   = 2,
    SPI_SR_UDR      = 3,
    SPI_SR_CRCERR   = 4,
    SPI_SR_MODF     = 5,
    SPI_SR_OVR      = 6,
    SPI_SR_BSY      = 7,
    SPI_SR_FRE      = 8,
    SPI_SR_FRLVL    = 9,   /* FRLVL[1:0] = bits 10:9 */
    SPI_SR_FTLVL    = 11   /* FTLVL[1:0] = bits 12:11 */
} SPI_SR_BitPos_t;


/*
 * @SPI_DeviceMode
 */
typedef enum {
	SPI_DEVICE_SLAVE_MODE = 0,
	SPI_DEVICE_MASTER_MODE,
} SPI_DeviceMode_t;

/*
 * @SPI_BusConfig
 */
typedef enum {
	SPI_BUS_CONFIG_FD = 1,		// Full duplex mode
	SPI_BUS_CONFIG_HD,			// Half duplex mode
//	SPI_BUS_CONFIG_S_TXONLY,	// Simplex Tx mode. commented out because TXONLY is just full duplex mode where MISO line is not connected
	SPI_BUS_CONFIG_S_RXONLY,	// Simplex Rx mode
} SPI_BusConfig_t;

/*
 * @SPI_SclkSpeed
 */
typedef enum {
	SPI_SCLK_SPEED_DIV2 = 0,
	SPI_SCLK_SPEED_DIV4,
	SPI_SCLK_SPEED_DIV8,
	SPI_SCLK_SPEED_DIV16,
	SPI_SCLK_SPEED_DIV32,
	SPI_SCLK_SPEED_DIV64,
	SPI_SCLK_SPEED_DIV128,
	SPI_SCLK_SPEED_DIV256,
} SPI_SclkSpeed_t;

/*
 * @SPI_DFF
 */
typedef enum
{
    SPI_DS_4BITS  = 0x3,
    SPI_DS_5BITS  = 0x4,
    SPI_DS_6BITS  = 0x5,
    SPI_DS_7BITS  = 0x6,
    SPI_DS_8BITS  = 0x7,
    SPI_DS_9BITS  = 0x8,
    SPI_DS_10BITS = 0x9,
    SPI_DS_11BITS = 0xA,
    SPI_DS_12BITS = 0xB,
    SPI_DS_13BITS = 0xC,
    SPI_DS_14BITS = 0xD,
    SPI_DS_15BITS = 0xE,
    SPI_DS_16BITS = 0xF
} SPI_DS_t;

/*
 * @SPI_CPHA
 */
typedef enum {
	SPI_CPHA_LOW = 0,
	SPI_CPHA_HIGH,
} SPI_CPHAL_t;


/*
 * @SPI_CPOL
 */
typedef enum {
	SPI_CPOL_LOW = 0,
	SPI_CPOL_HIGH,
} SPI_CPOL_t;



/*
 * @SPI_SSM
 */
typedef enum {
	SPI_SSM_DI = 0,  // default
	SPI_SSM_EN
} SPI_CPHA_t;


/************ SPI functionality *****************/

/*
 * Peripheral Clock configuration
 */
PeriphStatus_t SPI_PClkCtrl(SPI_RegDef_t *pSPIx, PeriphState_t pState);

/*
 * Initialization and de-initialization
 */
PeriphStatus_t SPI_Init(SPI_Handle_t *pSPIHandle);
PeriphStatus_t SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Data read and write
 */
PeriphStatus_t SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t len);
PeriphStatus_t SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t len);

/*
 * IRQ configuration and ISR handling
 */
void SPI_IRQConfig(uint8_t IRQNumber, PeriphState_t pState);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandler(SPI_Handle_t *pSPIHandle);


/*
 * Other peripheral control API's
 */
void SPI_PCtrl(SPI_RegDef_t *pSPIx, PeriphState_t pState);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, PeriphState_t pState);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, PeriphState_t pState);


#endif /* INC_STM32C031XX_SPI_DRIVER_H_ */
