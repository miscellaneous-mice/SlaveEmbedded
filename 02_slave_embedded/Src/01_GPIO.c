/*
 * 01_GPIO.c
 *
 *  Created on: 03-Mar-2026
 *      Author: prateek
 */


/*
 * main.c
 *
 *  Created on: Jan 16, 2026
 *      Author: prate
 */


#include <string.h>
#include "stm32c031xx.h"
#include "stm32c031xx_gpio_driver.h"
#include "stm32c031xx_systick_timer.h"

void delay(void)
{
	for (uint32_t i = 0; i < 500000/2; i++);
}

int main(void) {
	GPIO_Handle_t GpioLed, GpioBtn;
	memset(&GpioLed, 0, sizeof(GpioLed));
	memset(&GpioBtn, 0, sizeof(GpioBtn));

	init_systick(1000, 0);

	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OUTPUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	(void)GPIO_Init(&GpioLed);

//	(void)GPIO_PClkCtrl(GPIOA, PERIPH_ENABLE);

	GpioBtn.pGPIOx = GPIOC;
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_13;
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_IT_FT;
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	(void)GPIO_Init(&GpioBtn);

//	(void)GPIO_PClkCtrl(GPIOC, PERIPH_ENABLE);

	GPIO_WriteToOutputPin(GPIOA, GPIO_NO_5, GPIO_PIN_RESET);

	GPIO_EXTIConfig(GPIOC, GPIO_NO_13, GPIO_IT_FT);

	// IRQ Configurations
	GPIO_IRQPriorityConfig(IRQ_NO_EXTI4_15, NVIC_IRQ_PRI3);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI4_15, PERIPH_ENABLE);

//	GPIO_IRQPriorityConfig(IRQ_NO_EXTI0_1, NVIC_IRQ_PRI3);
//	GPIO_IRQInterruptConfig(IRQ_NO_EXTI0_1, PERIPH_ENABLE);

	for(;;);

	return 0;
}

void EXTI4_15_IRQHandler(void)
//void EXTI0_1_IRQHandler(void)
{
	GPIO_IRQHandling(GPIO_NO_13);
	delay_ms(1000);
	(void)GPIO_ToggleOutputPin(GPIOA, GPIO_NO_5);
}





/*
 * 002led_button.c
 *
 *  Created on: Feb 1, 2019
 *      Author: admin
 */


//#include "stm32c031xx.h"
//#include "stm32c031xx_gpio_driver.h"
//#include "stm32c031xx_systick_timer.h"
//
//#define LOW 0
//#define HIGH 1
////#define BTN_PRESSED HIGH
//#define BTN_PRESSED LOW
//
//int main(void)
//{
//	init_systick(1000, 0);
//
//	GPIO_Handle_t GpioLed, GPIOBtn;
//
//	//this is led gpio configuration
//	GpioLed.pGPIOx = GPIOA;
//	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_5;
//	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OUTPUT;
//	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
////	GPIO_PClkCtrl(GPIOA,PERIPH_ENABLE);
//
//	GPIO_Init(&GpioLed);
//
//
//	//this is btn gpio configuration
//	GPIOBtn.pGPIOx = GPIOC;
//	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_13;
//	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_INPUT;
//	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
////	GPIO_PClkCtrl(GPIOC,PERIPH_ENABLE);
//
//	GPIO_Init(&GPIOBtn);
//
//	while(1)
//	{
//		if(GPIO_ReadFromInputPin(GPIOC,GPIO_NO_13) == BTN_PRESSED)
//		{
//			delay_ms(1000);
//			GPIO_ToggleOutputPin(GPIOA,GPIO_NO_5);
//		}
//	}
//	return 0;
//}

