#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "stm32c031xx.h"
#include "stm32c031xx_gpio_driver.h"
#include "stm32c031xx_systick_timer.h"
#include "stm32c031xx_spi_driver.h"

extern void initialise_monitor_handles(void);

#define COMMAND_LED_CTRL    0x50
#define COMMAND_SENSOR_READ 0x51
#define COMMAND_PRINT       0x53
#define COMMAND_ID_READ     0x54

#define LED_ON  1
#define LED_OFF 0
#define ACK     0xF5
#define NACK    0xA5

#define ANALOG_PIN0 0
#define LED_PIN     9
#define BOARD_ID_LEN 10   // must match master's read loop

uint8_t dataBuff[255];
uint8_t board_id[BOARD_ID_LEN + 1] = "NUCLEO-C03"; // exactly 10 chars

/* ---------- RNG ---------- */
static uint32_t rng_state;

void rng_seed(uint32_t seed)   { rng_state = seed; }

uint32_t rng_next(void)
{
    rng_state = (1664525u * rng_state + 1013904223u);
    return rng_state;
}

float rng_uniform(void)
{
    return (rng_next() >> 8) * (1.0f / 16777216.0f);
}

float rng_normal(float mean, float stddev)
{
    float u, v, s;
    do {
        u = 2.0f * rng_uniform() - 1.0f;
        v = 2.0f * rng_uniform() - 1.0f;
        s = u*u + v*v;
    } while (s >= 1.0f || s == 0.0f);

    float mul = sqrtf(-2.0f * logf(s) / s);
    return mean + stddev * u * mul;
}

/* ---------- SPI GPIO ---------- */
void SPI1_GPIOInits(void)
{
    GPIO_Handle_t SPIPins;
    SPIPins.pGPIOx = GPIOA;
    SPIPins.GPIO_PinConfig.GPIO_PinMode        = GPIO_ALTFN;
    SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode  = 0;
    SPIPins.GPIO_PinConfig.GPIO_PinOPType      = GPIO_OP_TYPE_PP;
    SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    SPIPins.GPIO_PinConfig.GPIO_PinSpeed       = GPIO_SPEED_FAST;

    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_5;
    GPIO_Init(&SPIPins); // SCLK

    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_7;
    GPIO_Init(&SPIPins); // MOSI

    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_6;
    GPIO_Init(&SPIPins); // MISO

    SPIPins.pGPIOx = GPIOB;
    SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_0;
    GPIO_Init(&SPIPins); // NSS
}


void SPI1_Inits(void)
{
    SPI_Handle_t SPI1handle;
    SPI1handle.pSPIx                       = SPI1;
    SPI1handle.SPIConfig.SPI_BusConfig     = SPI_BUS_CONFIG_FD;
    SPI1handle.SPIConfig.SPI_DeviceMode    = SPI_DEVICE_SLAVE_MODE;
    SPI1handle.SPIConfig.SPI_SclkSpeed     = SPI_SCLK_SPEED_DIV2;
    SPI1handle.SPIConfig.SPI_CPHA          = SPI_CPHA_LOW;
    SPI1handle.SPIConfig.SPI_CPOL          = SPI_CPOL_LOW;
    SPI1handle.SPIConfig.SPI_DS            = SPI_DS_8BITS;
    SPI1handle.SPIConfig.SPI_SSM           = SPI_SSM_DI;
    (void)SPI_Init(&SPI1handle);
}

uint8_t checkData(uint8_t command)
{
    // Validate command — return ACK for known commands, NACK otherwise
    if (command == COMMAND_LED_CTRL    ||
        command == COMMAND_SENSOR_READ ||
        command == COMMAND_PRINT       ||
        command == COMMAND_ID_READ)
    {
        return ACK;
    }
    return NACK;
}

int main(void)
{
    init_systick(1000, 0);
    initialise_monitor_handles();

    printf("Application is running\n");

    SPI1_GPIOInits();
    SPI1_Inits();

    SPI_SSOEConfig(SPI1, PERIPH_DISABLE); // SSI low = slave asserts NSS low to itself

    printf("SPI Init. done\n");

    SPI_PCtrl(SPI1, PERIPH_ENABLE);     // enable once, outside the loop

    while (1)
    {
        uint8_t command;
        uint8_t ackornack;
        uint8_t dummy;

        // Step 1: receive command byte
        SPI_ReceiveData(SPI1, &command, 1);

        // Step 2: pre-load ACK/NACK so it's ready when master sends dummy
        ackornack = checkData(command);
        SPI_SendData(SPI1, &ackornack, 1);

        // Step 3: receive master's dummy byte that clocked out our ACK
        //         THIS keeps slave in sync — without it we drift by 1 byte
        SPI_ReceiveData(SPI1, &dummy, 1);

        if (ackornack == NACK)
        {
            printf("Unknown command 0x%02X — sent NACK\n", command);
            continue;
        }

        if (command == COMMAND_LED_CTRL)
        {
            uint8_t pin, value;
            SPI_ReceiveData(SPI1, &pin,   1);
            SPI_ReceiveData(SPI1, &value, 1);
            // master does a 2-byte dummy read after args — slave TX will output
            // garbage for those clocks, which master discards, so no action needed

            printf("RCVD: COMMAND_LED_CTRL  pin=%d  value=%d\n", pin, value);
            printf(value == LED_ON ? "LED ON\n" : "LED OFF\n");
        }

        else if (command == COMMAND_SENSOR_READ)
        {
            uint8_t pin;
            SPI_ReceiveData(SPI1, &pin, 1);    // receive analog pin number

            rng_seed(pin);
            uint8_t aread = (uint8_t)rng_normal(128.0f, 30.0f);

            // Pre-load reading into TX, then receive master's dummy that clocks it out
            SPI_SendData(SPI1, &aread, 1);
            SPI_ReceiveData(SPI1, &dummy, 1);

            printf("RCVD: COMMAND_SENSOR_READ  pin=%d  value=%d\n", pin, aread);
        }

        else if (command == COMMAND_PRINT)
        {
            uint8_t len;
            SPI_ReceiveData(SPI1, &len, 1);

            for (int i = 0; i < len; i++)
            {
                SPI_ReceiveData(SPI1, &dataBuff[i], 1);
            }
            dataBuff[len] = '\0';

            printf("RCVD: COMMAND_PRINT  msg=\"%s\"\n", (char *)dataBuff);
        }

        else if (command == COMMAND_ID_READ)
        {
            for (int i = 0; i < BOARD_ID_LEN; i++)
            {
                SPI_SendData(SPI1, &board_id[i], 1);
                SPI_ReceiveData(SPI1, &dummy, 1); // receive master's dummy that clocked out each byte
            }

            printf("RCVD: COMMAND_ID_READ  id=\"%s\"\n", board_id);
        }

        printf("SPI transaction complete\n");
    }

    SPI_PCtrl(SPI1, PERIPH_DISABLE);

    return 0;
}
