
/**
 ******************************************************************************
 * @file    main.c
 * @brief   STM32C031C6 I2C Slave
 *
 * I2C slave address: 0x68
 *
 * Commands:
 *   0x51 -> Return the length of the response string
 *   0x52 -> Return the response string
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "stm32c031xx.h"
#include "stm32c031xx_gpio_driver.h"
#include "stm32c031xx_systick_timer.h"
#include "stm32c031xx_i2c_driver.h"

#define SLAVE_ADDR       0x68U
#define LEN_REQ_CMD      0x51U
#define DATA_REQ_CMD     0x52U

#define RESPONSE_DATA    "We are testing I2C Slave Rx\n"
#define RESPONSE_LEN     (sizeof(RESPONSE_DATA) - 1U)

static I2C_Handle_t I2C1_Handle;

static uint8_t cmd_code;
static uint8_t response_len;
static uint8_t response_data[] = RESPONSE_DATA;

extern void initialise_monitor_handles(void);

/*
 * PB8 -> I2C1_SCL
 * PB9 -> I2C1_SDA
 *
 * AF6 for I2C1 on the pins used in the supplied example.
 */
void I2C1_GPIOInits(void)
{
    GPIO_Handle_t I2CPins = {0};

    I2CPins.pGPIOx = GPIOB;

    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 6;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_8;
    (void)GPIO_Init(&I2CPins);

    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_NO_9;
    (void)GPIO_Init(&I2CPins);
}

/*
 * Configure I2C1 in slave mode.
 */
void I2C1_Inits(void)
{
    I2C1_Handle.pI2Cx = I2C1;

    I2C1_Handle.I2C_Config.I2C_DeviceAddress = SLAVE_ADDR;

    /* Timing values from your supplied C031 example. */
    I2C1_Handle.I2C_Config.I2C_SCL.PRESC_VALUE  = 0xBU;
    I2C1_Handle.I2C_Config.I2C_SCL.SCLDEL_VALUE = 0x4U;
    I2C1_Handle.I2C_Config.I2C_SCL.SDADEL_VALUE = 0x2U;
    I2C1_Handle.I2C_Config.I2C_SCL.SCLH_VALUE   = 0xFU;
    I2C1_Handle.I2C_Config.I2C_SCL.SCLL_VALUE   = 0x13U;

    (void)I2C_Init(&I2C1_Handle);
}

int main(void)
{
    PeriphStatus_t status;

    initialise_monitor_handles();
    init_systick(1000, 0);

    I2C1_GPIOInits();
    I2C1_Inits();

    response_len = (uint8_t)RESPONSE_LEN;

    printf("STM32C031C6 I2C Slave Ready\n");
    printf("Slave address: 0x%02X\n", SLAVE_ADDR);
    printf("Response length: %u bytes\n", response_len);

    while (1)
    {
        /*
         * Step 1:
         * Receive the master's one-byte command.
         *
         * The master must write the command and generate STOP
         * before this blocking receive function completes.
         */
    	delay_ms(100);

        status = I2C_Slave_ReceiveData(
            I2C1,
            &cmd_code,
            1U
        );

        if (status != PERIPH_OK)
        {
            printf("Command reception failed\n");
            continue;
        }

        printf("Command received: 0x%02X\n", cmd_code);

        /*
         * Step 2:
         * Respond according to the received command.
         */
        switch (cmd_code)
        {
            case LEN_REQ_CMD:
            {
                printf("Length request received\n");

                /*
                 * The master now performs a read transaction
                 * to retrieve one byte containing the length.
                 */
                status = I2C_Slave_SendData(
                    I2C1,
                    &response_len,
                    1U
                );

                if (status != PERIPH_OK)
                {
                    printf("Length transmission failed\n");
                }

                break;
            }

            case DATA_REQ_CMD:
            {
                printf("Data request received\n");

                /*
                 * Send the complete response string.
                 * Do not transmit the terminating '\0'.
                 */
                status = I2C_Slave_SendData(
                    I2C1,
                    response_data,
                    response_len
                );

                if (status != PERIPH_OK)
                {
                    printf("Data transmission failed\n");
                }

                break;
            }

            default:
            {
                printf("Unknown command: 0x%02X\n", cmd_code);
                break;
            }
        }
    }
}
