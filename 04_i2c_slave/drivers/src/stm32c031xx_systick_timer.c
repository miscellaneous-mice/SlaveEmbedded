/*
 * stm32c031xx_systick_timer.c
 *
 *  Created on: 04-Mar-2026
 *      Author: prateek
 */


#include "stm32c031xx_systick_timer.h"

void init_systick(uint32_t s, uint8_t en)
{
	// Main clock source is running with HSI by default which is at 8 Mhz.
	// SysTick clock source can be set with CTRL register (Bit 2) (pm0056 - page 151)
	// 0: AHB/8 -> (1 MHz)
	// 1: Processor clock (AHB) -> (8 MHz)
	SYSTICK->CSR |= 0x00000; // Currently set to run at 1 Mhz
	// Enable callback
	SYSTICK->CSR |= (en << 1);
	// Load the reload value
	SYSTICK->RVR = s;
	// Set the current value to 0
	SYSTICK->CVR = 0;
	// Enable SysTick
	SYSTICK->CSR |= (1 << 0);
}

void delay_ms(volatile uint32_t s)
{
	for(s; s>0; s--){
		while(!(SYSTICK->CSR & (1 << 16))); // Wait until COUNTFLAG is 1
	}
}
