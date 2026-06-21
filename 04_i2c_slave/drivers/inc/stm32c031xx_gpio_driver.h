/*
 * stm32c031xx_gpio_driver.h
 *
 *  Created on: 01-Mar-2026
 *      Author: prateek
 */

#ifndef INC_STM32C031XX_GPIO_DRIVER_H_
#define INC_STM32C031XX_GPIO_DRIVER_H_


#include <stddef.h>
#include "stm32c031xx.h"


/************ GPIO peripheral state ***************/
typedef enum {
	GPIO_NO_0 = 0,
	GPIO_NO_1,
	GPIO_NO_2,
	GPIO_NO_3,
	GPIO_NO_4,
	GPIO_NO_5,
	GPIO_NO_6,
	GPIO_NO_7,
	GPIO_NO_8,
	GPIO_NO_9,
	GPIO_NO_10,
	GPIO_NO_11,
	GPIO_NO_12,
	GPIO_NO_13,
	GPIO_NO_14,
	GPIO_NO_15,
} GPIO_PinNo_t;

typedef enum {
/*
 * GPIO Input possible modes
 */
	GPIO_INPUT = 0,
	GPIO_OUTPUT,
	GPIO_ALTFN,
	GPIO_ANALOG,
	GPIO_IT_FT, // GPIO Input falling edge (Used in Interrupt mode)
	GPIO_IT_RT, // GPIO Input rising edge (Used in Interrupt mode)
	GPIO_IT_RFT, // GPIO Input rising edge, falling edge (Used in Interrupt mode)
} GPIO_ConfigMode_t;

typedef enum {
/*
 * GPIO Input possible output types
 */
	GPIO_OP_TYPE_PP = 0, // GPIO Output Push Pull
	GPIO_OP_TYPE_OD, // GPIO Output Open drain
} GPIO_ConfigOutput_t;

typedef enum {
/*
 * GPIO pin possible output speeds
 */
	GPIO_SPEED_LOW = 0,
	GPIO_SPEED_MEDIUM,
	GPIO_SPEED_FAST,
	GPIO_SPEED_HIGH,
} GPIO_ConfigSpeed_t;

typedef enum {
/*
 * Pull-up / Pull-down configuration
 */
	GPIO_NO_PUPD = 0, // GPIO no Pull-up / Pull-down
	GPIO_PU, // GPIO Pull up
	GPIO_PD // GPIO Pull down
} GPIO_ConfigPuPd_t;



/************ Important configuration structures *************/

typedef struct {
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode; /* Check GPIO Input possible modes in GPIO_PinConfig_t */
	uint8_t GPIO_PinSpeed; /* Check GPIO pin possible output speeds in GPIO_PinConfig_t */
	uint8_t GPIO_PinPuPdControl; /* Check Pull-up / Pull-down configuration in GPIO_PinConfig_t */
	uint8_t GPIO_PinOPType; /* Check GPIO Input possible output types in GPIO_PinConfig_t */
	uint8_t GPIO_PinAltFunMode;
} GPIO_PinConfig_t;

typedef struct {
	GPIO_RegDef_t *pGPIOx;
	GPIO_PinConfig_t GPIO_PinConfig;
} GPIO_Handle_t;


/************ GPIO functionality *****************/

/*
 * Peripheral Clock configuration
 */
PeriphStatus_t GPIO_PClkCtrl(GPIO_RegDef_t *pGPIOx, PeriphState_t pState);

/*
 * Initialization and de-initialization
 */
PeriphStatus_t GPIO_Init(GPIO_Handle_t *pGPIOHandle);
PeriphStatus_t GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*
 * Data read and write
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
PeriphStatus_t GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value);
PeriphStatus_t GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value);
PeriphStatus_t GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*
 * IRQ configuration and ISR handling
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, PeriphState_t EnorDi);
void GPIO_EXTIConfig(GPIO_RegDef_t *GPIOx, uint8_t pinNumber, GPIO_ConfigMode_t triggerType);




#endif /* INC_STM32C031XX_GPIO_DRIVER_H_ */
