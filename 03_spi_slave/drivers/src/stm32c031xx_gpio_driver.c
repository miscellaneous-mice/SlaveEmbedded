/*
 * stm32c031xx_gpio_driver.c
 *
 *  Created on: 01-Mar-2026
 *      Author: prateek
 */


#include "stm32c031xx_gpio_driver.h"

/******************************
 * @fn				- GPIO_PClkCtrl
 *
 * @brief			- This function enables/disables the clock for given GPIO port
 *
 * @param[in]		- base address of GPIO peripheral
 * @param[in]		- Enable / Disable
 *
 * @return			- used to indicate the status of API
 *
 * @Note			- None
 *
 */
PeriphStatus_t GPIO_PClkCtrl(GPIO_RegDef_t *pGPIOx, PeriphState_t pState)
{
	PeriphStatus_t status = PERIPH_OK;
    if (pGPIOx == NULL)
        status = PERIPH_ERROR;

    if ((status == PERIPH_OK) && (pState == PERIPH_ENABLE))
    {
        if      (pGPIOx == GPIOA) GPIOA_PCLK_EN();
        else if (pGPIOx == GPIOB) GPIOB_PCLK_EN();
        else if (pGPIOx == GPIOC) GPIOC_PCLK_EN();
        else if (pGPIOx == GPIOD) GPIOD_PCLK_EN();
        else if (pGPIOx == GPIOF) GPIOF_PCLK_EN();
        else status = PERIPH_ERROR;
    }
    else if ((status == PERIPH_OK) && (pState == PERIPH_DISABLE))
    {
        if      (pGPIOx == GPIOA) GPIOA_PCLK_CLR();
        else if (pGPIOx == GPIOB) GPIOB_PCLK_CLR();
        else if (pGPIOx == GPIOC) GPIOC_PCLK_CLR();
        else if (pGPIOx == GPIOD) GPIOD_PCLK_CLR();
        else if (pGPIOx == GPIOF) GPIOF_PCLK_CLR();
        else status = PERIPH_ERROR;
    }
    else
    {
    	status = PERIPH_ERROR;
    }

    return status;
}


/******************************
 * @fn				- GPIO_Init
 *
 * @brief			- This function initializes the GPIO pin according to the specified parameters in the GPIO Handle
 *
 * @param[in]		- pGPIOHandle : Pointer to the GPIO Handle structure containing the base address and configuration settings
 *
 * @return			- used to indicate the status of API (SUCCESS / ERROR)
 *
 * @Note			- Ensure the Peripheral Clock for the specific Port is enabled before calling this function
 *
 */
PeriphStatus_t GPIO_Init(GPIO_Handle_t *pGPIOHandle) {
	uint32_t temp = 0;
	PeriphStatus_t status = PERIPH_OK;

	// Enable GPIO clock
	GPIO_PClkCtrl(pGPIOHandle->pGPIOx, PERIPH_ENABLE);

	// 1. Configure the mode of GPIO pin
	if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_ANALOG)
	{
		// Value is stored in temporary variable
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber ) );
		pGPIOHandle->pGPIOx->MODER &= ~( 0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
		pGPIOHandle->pGPIOx->MODER |= temp; //setting
	}
	else
	{
		// Interrupt mode code
		SYSCFG_PCLK_EN();

		// configure as input
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER |= temp;
	}

	temp = 0;

	// 2. Configure the speed
	// Value is stored in temporary variable
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->OSPEEDR &= ~( 0x3 << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	// 3. Configure the pull-up/pull-down configuration
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->pGPIOx->PUPDR &= ~( 0x3 << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber)); //clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	// 4. Configure the optype
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );
	pGPIOHandle->pGPIOx->OTYPER &= ~( 0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	// 5. Configure the alt functionality
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_ALTFN)
	{
		uint32_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF <<  (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= ((pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode) << (4 * temp2));
	}

	return status;
}

PeriphStatus_t GPIO_DeInit(GPIO_RegDef_t *pGPIOx) {
	PeriphStatus_t status = PERIPH_OK;
	if      (pGPIOx == GPIOA) GPIOA_REG_RESET();
	else if (pGPIOx == GPIOB) GPIOB_REG_RESET();
	else if (pGPIOx == GPIOC) GPIOC_REG_RESET();
	else if (pGPIOx == GPIOD) GPIOD_REG_RESET();
	else if (pGPIOx == GPIOF) GPIOF_REG_RESET();
	else status = PERIPH_ERROR;
	return status;
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber) {
	uint8_t value;
	value = (uint8_t)( ( pGPIOx->IDR >> PinNumber ) & 0x00000001 );
	return value;
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx) {
	uint16_t value;
	value = (uint16_t)pGPIOx->IDR;
	return value;
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
PeriphStatus_t GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value) {
	if (value == GPIO_PIN_SET)
	{
		pGPIOx->ODR |= ( 1 << PinNumber );
	} else
	{
		pGPIOx->ODR &= ~( 1 << PinNumber );
	}
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
PeriphStatus_t GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value) {
	PeriphStatus_t status = PERIPH_OK;
	pGPIOx->ODR = value;
	return status;
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
PeriphStatus_t GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber) {
	PeriphStatus_t status = PERIPH_OK;
	pGPIOx->ODR ^= (1 << PinNumber);
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, PeriphState_t EnorDi)
{
    if(EnorDi == PERIPH_ENABLE)
    {
        *NVIC_ISER |= (1 << IRQNumber);
    }
    else
    {
        *NVIC_ICER |= (1 << IRQNumber);
    }
}


void GPIO_EXTIConfig(GPIO_RegDef_t *GPIOx,
                     uint8_t pinNumber,
                     GPIO_ConfigMode_t triggerType)
{
    uint8_t portCode = 0;

    /* Determine port code */
    if(GPIOx == GPIOA) portCode = 0x00;
    else if(GPIOx == GPIOB) portCode = 0x01;
    else if(GPIOx == GPIOC) portCode = 0x02;
    else if(GPIOx == GPIOD) portCode = 0x03;
    else if(GPIOx == GPIOF) portCode = 0x05;

    /* EXTICR register index */
    uint8_t index = pinNumber / 4;

    /* Each line occupies 8 bits */
    uint8_t shift = (pinNumber % 4) * 8;

    /* Clear previous configuration */
    EXTI->EXTICR[index] &= ~(0xFF << shift);

    /* Write port selection */
    EXTI->EXTICR[index] |= (portCode << shift);

    /* Configure trigger */
    if(triggerType == GPIO_IT_RT)
    {
        EXTI->RTSR1 |=  (1 << pinNumber);
        EXTI->FTSR1 &= ~(1 << pinNumber);
    }
    else if(triggerType == GPIO_IT_FT)
    {
        EXTI->FTSR1 |=  (1 << pinNumber);
        EXTI->RTSR1 &= ~(1 << pinNumber);
    }
    else if(triggerType == GPIO_IT_RFT)
    {
        EXTI->RTSR1 |= (1 << pinNumber);
        EXTI->FTSR1 |= (1 << pinNumber);
    }

    /* Unmask interrupt */
    EXTI->IMR1 |= (1 << pinNumber);
}


void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;

    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    volatile uint32_t *ipr = NVIC_IPR_BASE_ADDR + iprx;

    *ipr &= ~(0xFF << (8 * iprx_section));     // clear priority field
    *ipr |=  (IRQPriority << shift_amount);    // set new priority
}

/******************************
 * @fn						GPIO_PClkCtrl
 *
 * @brief
 *
 * @param[in]
 * @param[in]
 * @param[in]
 *
 * @return
 *
 * @Note
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
    if(EXTI->RPR1 & (1 << PinNumber))
    {
        EXTI->RPR1 |= (1 << PinNumber);  // clear rising pending
    }

    if(EXTI->FPR1 & (1 << PinNumber))
    {
        EXTI->FPR1 |= (1 << PinNumber);  // clear falling pending
    }
}

