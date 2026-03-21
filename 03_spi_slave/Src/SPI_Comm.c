/*
 * SPI_Comm.c
 *
 *  Created on: 20-Mar-2026
 *      Author: prateek
 */


/*
 * 03_SPI_comm.c
 *
 *  Created on: Mar 20, 2026
 *      Author: prate
 */


/*
 * 006spi_tx_testing.c
 *
 *  Created on: Feb 10, 2019
 *      Author: admin
 */

#include<stdio.h>
#include<string.h>
#include <stdint.h>
#include <math.h>

#include "stm32c031xx.h"
#include "stm32c031xx_gpio_driver.h"
#include "stm32c031xx_systick_timer.h"
#include "stm32c031xx_spi_driver.h"

extern void initialise_monitor_handles();

//command codes
#define COMMAND_LED_CTRL      		0x50
#define COMMAND_SENSOR_READ      	0x51
#define COMMAND_PRINT      			0x53
#define COMMAND_ID_READ      		0x54

#define LED_ON     1
#define LED_OFF    0

#define NACK 0xA5
#define ACK 0xF5

//arduino analog pins
#define ANALOG_PIN0 	0
#define ANALOG_PIN1 	1
#define ANALOG_PIN2 	2
#define ANALOG_PIN3 	3
#define ANALOG_PIN4 	4

//arduino led

#define LED_PIN  9

uint8_t dataBuff[255];

uint8_t board_id[13] = "NUCLEO-C031C6";

static uint32_t rng_state;

void rng_seed(uint32_t seed) {
    rng_state = seed;
}

uint32_t rng_next() {
    rng_state = (1664525 * rng_state + 1013904223);
    return rng_state;
}

// Uniform [0,1)
float rng_uniform() {
    return (rng_next() >> 8) * (1.0f / 16777216.0f);
}

// ---------------- Normal Distribution ----------------
float rng_normal(float mean, float stddev) {
    float u, v, s;

    do {
        u = 2.0f * rng_uniform() - 1.0f;
        v = 2.0f * rng_uniform() - 1.0f;
        s = u*u + v*v;
    } while (s >= 1.0f || s == 0.0f);

    float mul = sqrtf(-2.0f * logf(s) / s);
    return mean + stddev * u * mul;
}

uint8_t rng_normal_u8(uint32_t seed, float mean, float stddev) {
    rng_seed(seed);

    float val = rng_normal(mean, stddev);

    return (uint8_t)val;
}



void SPI1_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOA;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 0; // AF0 for SPI1
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_5;
	GPIO_Init(&SPIPins);

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_7;
	GPIO_Init(&SPIPins);

	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_6;
	GPIO_Init(&SPIPins);

	// NSS (software)
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_4;
	GPIO_Init(&SPIPins);
}


void SPI1_Inits(void)
{
	SPI_Handle_t SPI1handle;
	SPI1handle.pSPIx = SPI1;
	SPI1handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_SLAVE_MODE;
	SPI1handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2; // 8 MHz
	SPI1handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI1handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI1handle.SPIConfig.SPI_DS = SPI_DS_8BITS;
	SPI1handle.SPIConfig.SPI_SSM = SPI_SSM_EN; // software slave management for NSS pin

	(void)SPI_Init(&SPI1handle);
}


uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{

	if(ackbyte == (uint8_t)0xF5)
	{
		//ack
		return 1;
	}

	return 0;
}

//void LEDInit(void)
//{
//	GPIO_Handle_t GpioLed;
//	memset(&GpioLed, 0, sizeof(GpioLed));
//
//	GpioLed.pGPIOx = GPIOA;
//	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_5;
//	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_OUTPUT;
//	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
//	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
//	(void)GPIO_Init(&GpioLed);
//}

uint8_t checkData(uint8_t command)
{
	// Todo
	return ACK;
}

int main(void)
{
	init_systick(1000, 0);

	initialise_monitor_handles();

	printf("Application is running\n");

	//this function is used to initialize the GPIO pins to behave as SPI2 pins
	SPI1_GPIOInits();

	//This function is used to initialize the SPI2 peripheral parameters
	SPI1_Inits();

	SPI_SSIConfig(SPI1, PERIPH_DISABLE);

	printf("SPI Init. done\n");

	/*
	* making SSOE 1 does NSS output enable.
	* The NSS pin is automatically managed by the hardware.
	* i.e when SPE=1 , NSS will be pulled to low
	* and NSS pin will be high when SPE=0
	*/
//	SPI_SSOEConfig(SPI1, PERIPH_DISABLE);

	while(1)
	{
		uint8_t data;
		uint8_t command;
		uint8_t len;
		uint8_t ackornack=NACK;
		uint8_t dummy;

		//enable the SPI2 peripheral
		SPI_PCtrl(SPI1, PERIPH_ENABLE);

		//2. now lets wait until rx buffer has a byte
		SPI_ReceiveData(SPI1, &command, 1);

		ackornack = checkData(command);

//		delay_ms(100);

		SPI_SendData(SPI1, &ackornack, 1);

		SPI_ReceiveData(SPI1, &len, 1); // dummy byte

		if (command == COMMAND_LED_CTRL)
		{
			uint8_t pin, value;
			SPI_ReceiveData(SPI1, &pin, 1);
			SPI_ReceiveData(SPI1, &value, 1);

			printf("RCVD:Command LED Control\n");

			if(value == LED_ON)
			{
				printf("LED ON\n");
//				GPIO_WriteToOutputPin(GPIOA, GPIO_NO_5, GPIO_PIN_SET);
			}
			else
			{
				printf("LED OFF\n");
//				GPIO_WriteToOutputPin(GPIOA, GPIO_NO_5, GPIO_PIN_RESET);
			}
		}

		else if (command == COMMAND_SENSOR_READ)
		{
			uint8_t aread;
			uint8_t seed, val;

			SPI_ReceiveData(SPI1, &seed, 1);

			rng_seed(seed);
			aread = rng_normal(30000.0f, 5000.0f);

			SPI_SendData(SPI1, &aread, 1);
			SPI_ReceiveData(SPI1, &val, 1);
			printf("RCVD:Command Random Control\n");
		}

		else if (command == COMMAND_PRINT)
		{
			uint8_t len;
			SPI_ReceiveData(SPI1, &len, 1);
			for (int i=0; i < len; i++)
			{
				SPI_ReceiveData(SPI1, &dataBuff[i], 1);
			}

			dataBuff[len] = '\0';

			printf("%s\n", (char*)dataBuff);
			printf("RCV:Command Print\n");
		}

		else if (command == COMMAND_ID_READ)
		{
			for(int i=0; i < strlen(board_id); i++)
			{
				SPI_SendData(SPI1, &board_id[i], 1);
			}
			SPI_ReceiveData(SPI1, &dummy, 1);
			printf("RCV:Command ID Read\n");

		}

		//Disable the SPI2 peripheral
		SPI_PCtrl(SPI1, PERIPH_DISABLE);

		printf("SPI Communication Closed\n");
	}

	return 0;

}
