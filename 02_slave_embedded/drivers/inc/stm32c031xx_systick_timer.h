/*
 * stm32c031xx_systick_timer.h
 *
 *  Created on: 04-Mar-2026
 *      Author: prateek
 */

#ifndef INC_STM32C031XX_SYSTICK_TIMER_H_
#define INC_STM32C031XX_SYSTICK_TIMER_H_

#include "stm32c031xx.h"


void init_systick(uint32_t s, uint8_t en);
void delay_ms(volatile uint32_t s);

#endif /* INC_STM32C031XX_SYSTICK_TIMER_H_ */
