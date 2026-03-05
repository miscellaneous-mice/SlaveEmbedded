/*
 * stm32c031xx.h
 *
 *  Created on: 01-Mar-2026
 *      Author: prateek
 */

#ifndef INC_STM32C031XX_H_
#define INC_STM32C031XX_H_


#include <stdint.h>

/*
 * start : start address from where memory size has to be calculated
 * end   : end address to which memory size has to be calculated
 */
#define CALC_SIZE(start, end)	(uint32_t)(end - start + 1U)

/*************** Processor NVIC register *************/

/*************** NVIC Registers (Cortex-M0+) ***************/

#define NVIC_ISER        ( (volatile uint32_t*)0xE000E100U )
#define NVIC_ICER        ( (volatile uint32_t*)0xE000E180U )

#define NVIC_ISPR        ( (volatile uint32_t*)0xE000E200U )
#define NVIC_ICPR        ( (volatile uint32_t*)0xE000E280U )

#define NVIC_IPR_BASE_ADDR   ( (volatile uint32_t*)0xE000E400U )

/*
 * Cortex-M0+ implements only upper 2 bits of priority field.
 * STM32C0 implements 2 priority bits.
 */
#define NO_PR_BITS_IMPLEMENTED    2


/*************** BASE ADDRESSES ***************/

#define FLASH_BASE_ADDR			0x08000000U /* Flash region base address */
#define FLASH_OTP_BASE_ADDR		0x1FFF7000U /* Flash One time programmable region base address */
#define SRAM_BASE_ADDR			0x20000000U /* SRAM1 region base address */

#define FLASH_END_ADDR			0x08007FFFU /* Flash region end address */
#define FLASH_OTP_END_ADDR		0x1FFF73FFU /* Flash One time programmable region end address */
#define SRAM_END_ADDR			0x20002FFFU /* SRAM1 region end address */


#define FLASH_SIZE				(CALC_SIZE(FLASH_BASE_ADDR, FLASH_END_ADDR))
#define FLASH_OTP_SIZE			(CALC_SIZE(FLASH_OTP_BASE_ADDR, FLASH_OTP_END_ADDR))
#define SRAM_SIZE				(CALC_SIZE(SRAM_BASE_ADDR, SRAM_END_ADDR))

#define FLASH_ROM_BASE_ADDR		0x1FFF0000U
#define FLASH_ROM_END_ADDR		0x1FFF17FFU
#define FLASH_ROM_SIZE			(CALC_SIZE(FLASH_ROM_BASE_ADDR, FLASH_ROM_END_ADDR))

#define ROM						FLASH_ROM_BASE_ADDR
#define SRAM 					SRAM1_BASE_ADDR


#define PERIPH_BASE_ADDR	   0x40000000U
#define APB_BASE_ADDR          PERIPH_BASE_ADDR
#define AHB_BASE_ADDR          0x40020000U
#define IOPORT_BASE_ADDR       0x50000000U

#define SYSTICK_BASE_ADDR      0xE000E010U


/******************* OFFSET ********************/

#define GPIOA_PERIPH_OFST		(0x0000)
#define GPIOB_PERIPH_OFST		(0x0400)
#define GPIOC_PERIPH_OFST		(0x0800)
#define GPIOD_PERIPH_OFST		(0x0C00)
#define GPIOF_PERIPH_OFST		(0x1400)

#define SPI1_PERIPH_OFST		(0x13000)
#define SPI2_PERIPH_OFST		(0x3800)

#define USART1_PERIPH_OFST		(0x13800)
#define USART2_PERIPH_OFST		(0x4400)
#define USART3_PERIPH_OFST		(0x4800)
#define USART4_PERIPH_OFST		(0x4C00)

#define I2C1_PERIPH_OFST		(0x5400)
#define I2C2_PERIPH_OFST		(0x5800)

#define SYSCFG_PERIPH_OFST		(0x10000)
#define EXTI_PERIPH_OFST		(0x1800)

#define RCC_PERIPH_OFST			(0x1000)



/****************** PERIPHERAL ADDRESS ********************/

#define GPIOA_BASE_ADDR        (IOPORT_BASE_ADDR + GPIOA_PERIPH_OFST)
#define GPIOB_BASE_ADDR        (IOPORT_BASE_ADDR + GPIOB_PERIPH_OFST)
#define GPIOC_BASE_ADDR        (IOPORT_BASE_ADDR + GPIOC_PERIPH_OFST)
#define GPIOD_BASE_ADDR        (IOPORT_BASE_ADDR + GPIOD_PERIPH_OFST)
#define GPIOF_BASE_ADDR        (IOPORT_BASE_ADDR + GPIOF_PERIPH_OFST)

#define SPI1_BASE_ADDR         (APB_BASE_ADDR + SPI1_PERIPH_OFST)
#define SPI2_BASE_ADDR         (APB_BASE_ADDR + SPI2_PERIPH_OFST)

#define USART1_BASE_ADDR       (APB_BASE_ADDR + USART1_PERIPH_OFST)
#define USART2_BASE_ADDR       (APB_BASE_ADDR + USART2_PERIPH_OFST)
#define USART3_BASE_ADDR       (APB_BASE_ADDR + USART3_PERIPH_OFST)
#define USART4_BASE_ADDR       (APB_BASE_ADDR + USART4_PERIPH_OFST)

#define I2C1_BASE_ADDR         (APB_BASE_ADDR + I2C1_PERIPH_OFST)
#define I2C2_BASE_ADDR         (APB_BASE_ADDR + I2C2_PERIPH_OFST)

#define SYSCFG_BASE_ADDR       (APB_BASE_ADDR + SYSCFG_PERIPH_OFST)
#define EXTI_BASE_ADDR         (AHB_BASE_ADDR + EXTI_PERIPH_OFST)

#define RCC_BASE_ADDR		   (AHB_BASE_ADDR + RCC_PERIPH_OFST)

/***************** Configuration Structures *******************/
typedef struct
{
    volatile uint32_t MODER;     /* 0x00 */
    volatile uint32_t OTYPER;    /* 0x04 */
    volatile uint32_t OSPEEDR;   /* 0x08 */
    volatile uint32_t PUPDR;     /* 0x0C */
    volatile uint32_t IDR;       /* 0x10 */
    volatile uint32_t ODR;       /* 0x14 */
    volatile uint32_t BSRR;      /* 0x18 */
    volatile uint32_t LCKR;      /* 0x1C */
    volatile uint32_t AFR[2];    /* 0x20 (AFRL), 0x24 (AFRH) */
    volatile uint32_t BRR;       /* 0x28 */
} GPIO_RegDef_t;


typedef struct
{
    volatile uint32_t CR1;        /* 0x00 */
    volatile uint32_t CR2;        /* 0x04 */
    volatile uint32_t OAR1;       /* 0x08 */
    volatile uint32_t OAR2;       /* 0x0C */
    volatile uint32_t TIMINGR;    /* 0x10 */
    volatile uint32_t TIMEOUTR;   /* 0x14 */
    volatile uint32_t ISR;        /* 0x18 */
    volatile uint32_t ICR;        /* 0x1C */
    volatile uint32_t PECR;       /* 0x20 */
    volatile uint32_t RXDR;       /* 0x24 */
    volatile uint32_t TXDR;       /* 0x28 */
} I2C_RegDef_t;


typedef struct
{
    volatile uint32_t CR1;       /* 0x00 */
    volatile uint32_t CR2;       /* 0x04 */
    volatile uint32_t SR;        /* 0x08 */
    volatile uint32_t DR;        /* 0x0C */
    volatile uint32_t CRCPR;     /* 0x10 */
    volatile uint32_t RXCRCR;    /* 0x14 */
    volatile uint32_t TXCRCR;    /* 0x18 */
    volatile uint32_t I2SCFGR;   /* 0x1C */
    volatile uint32_t I2SPR;     /* 0x20 */
} SPI_RegDef_t;


typedef struct
{
    volatile uint32_t CR1;     /* 0x00 */
    volatile uint32_t CR2;     /* 0x04 */
    volatile uint32_t CR3;     /* 0x08 */
    volatile uint32_t BRR;     /* 0x0C */
    volatile uint32_t GTPR;    /* 0x10 */
    volatile uint32_t RTOR;    /* 0x14 */
    volatile uint32_t RQR;     /* 0x18 */
    volatile uint32_t ISR;     /* 0x1C */
    volatile uint32_t ICR;     /* 0x20 */
    volatile uint32_t RDR;     /* 0x24 */
    volatile uint32_t TDR;     /* 0x28 */
    volatile uint32_t PRESC;   /* 0x2C */
} USART_RegDef_t;


typedef struct
{
    volatile uint32_t CR;          /* 0x00 */
    volatile uint32_t ICSCR;       /* 0x04 */
    volatile uint32_t CFGR;        /* 0x08 */
    uint32_t RESERVED0[2];         /* 0x0C–0x10 */

    volatile uint32_t CRRCR;       /* 0x14 */
    volatile uint32_t CIER;        /* 0x18 */
    volatile uint32_t CIFR;        /* 0x1C */
    volatile uint32_t CICR;        /* 0x20 */

    volatile uint32_t IOPRSTR;     /* 0x24 */
    volatile uint32_t AHBRSTR;     /* 0x28 */
    volatile uint32_t APBRSTR1;    /* 0x2C */
    volatile uint32_t APBRSTR2;    /* 0x30 */

    volatile uint32_t IOPENR;      /* 0x34 */
    volatile uint32_t AHBENR;      /* 0x38 */
    volatile uint32_t APBENR1;     /* 0x3C */
    volatile uint32_t APBENR2;     /* 0x40 */

    volatile uint32_t IOPSMENR;    /* 0x44 */
    volatile uint32_t AHBSMENR;    /* 0x48 */
    volatile uint32_t APBSMENR1;   /* 0x4C */
    volatile uint32_t APBSMENR2;   /* 0x50 */

    volatile uint32_t CCIPR;       /* 0x54 */
    volatile uint32_t CCIPR2;      /* 0x58 */
    volatile uint32_t CSR1;        /* 0x5C */
    volatile uint32_t CSR2;        /* 0x60 */

} RCC_RegDef_t;

/*
 * peripheral register definition structure for EXTI
 */
typedef struct
{
    volatile uint32_t RTSR1;      /* 0x000 */
    volatile uint32_t FTSR1;      /* 0x004 */
    volatile uint32_t SWIER1;     /* 0x008 */
    volatile uint32_t RPR1;       /* 0x00C */
    volatile uint32_t FPR1;       /* 0x010 */
    uint32_t RESERVED0[3];        /* 0x014–0x01C */

    volatile uint32_t RTSR2;      /* 0x020 */
    volatile uint32_t FTSR2;      /* 0x024 */
    volatile uint32_t SWIER2;     /* 0x028 */
    volatile uint32_t RPR2;       /* 0x02C */
    volatile uint32_t FPR2;       /* 0x030 */
    uint32_t RESERVED1[11];       /* 0x034–0x05C */

    volatile uint32_t EXTICR[4];    /* 0x060-0x06C */
    uint32_t RESERVED2[4];        /* 0x070–0x07C */

    volatile uint32_t IMR1;       /* 0x080 */
    volatile uint32_t EMR1;       /* 0x084 */
    uint32_t RESERVED3[2];        /* 0x088–0x08C */

    volatile uint32_t IMR2;       /* 0x090 */
    volatile uint32_t EMR2;       /* 0x094 */

} EXTI_RegDef_t;


/*
 * peripheral register definition structure for SYSCFG
 */
typedef struct
{
    volatile uint32_t CFGR1;          /* 0x00 */
    uint32_t RESERVED0[5];            /* 0x04–0x14 */
    volatile uint32_t CFGR2;          /* 0x18 */
    uint32_t RESERVED1[8];            /* 0x1C–0x38 */
    volatile uint32_t CFGR3;          /* 0x3C */
    uint32_t RESERVED2[16];           /* 0x40–0x7C */

    volatile uint32_t ITLINE0;        /* 0x80 */
    volatile uint32_t ITLINE1;        /* 0x84 */
    volatile uint32_t ITLINE2;        /* 0x88 */
    volatile uint32_t ITLINE3;        /* 0x8C */
    volatile uint32_t ITLINE4;        /* 0x90 */
    volatile uint32_t ITLINE5;        /* 0x94 */
    volatile uint32_t ITLINE6;        /* 0x98 */
    volatile uint32_t ITLINE7;        /* 0x9C */
    volatile uint32_t ITLINE8;        /* 0xA0 */
    volatile uint32_t ITLINE9;        /* 0xA4 */
    volatile uint32_t ITLINE10;       /* 0xA8 */
    volatile uint32_t ITLINE11;       /* 0xAC */
    volatile uint32_t ITLINE12;       /* 0xB0 */
    volatile uint32_t ITLINE13;       /* 0xB4 */
    volatile uint32_t ITLINE14;       /* 0xB8 */
    volatile uint32_t ITLINE15;       /* 0xBC */
    volatile uint32_t ITLINE16;       /* 0xC0 */
    uint32_t RESERVED3[3];            /* 0xC4–0xCC */
    volatile uint32_t ITLINE19;       /* 0xCC */
    volatile uint32_t ITLINE20;       /* 0xD0 */
    volatile uint32_t ITLINE21;       /* 0xD4 */
    volatile uint32_t ITLINE22;       /* 0xD8 */
    volatile uint32_t ITLINE23;       /* 0xDC */
    volatile uint32_t ITLINE24;       /* 0xE0 */
    volatile uint32_t ITLINE25;       /* 0xE4 */
    volatile uint32_t ITLINE26;       /* 0xE8 */
    volatile uint32_t ITLINE27;       /* 0xEC */
    volatile uint32_t ITLINE28;       /* 0xF0 */
    volatile uint32_t ITLINE29;       /* 0xF4 */
    volatile uint32_t ITLINE30;       /* 0xF8 */
    volatile uint32_t ITLINE31;       /* 0xFC */

} SYSCFG_RegDef_t;


typedef struct
{
	uint32_t CSR;      /* SYSTICK control and status register,       Address offset: 0x00 */
	uint32_t RVR;      /* SYSTICK reload value register,             Address offset: 0x04 */
	uint32_t CVR;      /* SYSTICK current value register,            Address offset: 0x08 */
	uint32_t CALIB;    /* SYSTICK calibration value register,        Address offset: 0x0C */
} STK_type;


/***************** Peripheral interfaces ******************/
#define PERIPH_CAST(T, ADDR) ((volatile T*)(ADDR))

#define GPIO_PERIPH_CAST(GPIO_ADDR)		(PERIPH_CAST(GPIO_RegDef_t, GPIO_ADDR))
#define SPI_PERIPH_CAST(SPI_ADDR)		(PERIPH_CAST(SPI_RegDef_t, SPI_ADDR))
#define I2C_PERIPH_CAST(I2C_ADDR)		(PERIPH_CAST(I2C_RegDef_t, I2C_ADDR))
#define UART_PERIPH_CAST(UART_ADDR)		(PERIPH_CAST(UART_USART_RegDef_t, UART_ADDR))
#define RCC_PERIPH_CAST(RCC_ADDR)		(PERIPH_CAST(RCC_RegDef_t, RCC_ADDR))
#define EXTI_PERIPH_CAST(EXTI_ADDR)		(PERIPH_CAST(EXTI_RegDef_t, EXTI_ADDR))
#define SYSCFG_PERIPH_CAST(SYSCFG_ADDR)	(PERIPH_CAST(SYSCFG_RegDef_t, SYSCFG_ADDR))
#define SYSTICK_PERIPH_CAST(SYSTICK_ADDR)  (PERIPH_CAST(STK_type, SYSTICK_ADDR))


#define GPIOA		GPIO_PERIPH_CAST(GPIOA_BASE_ADDR)
#define GPIOB		GPIO_PERIPH_CAST(GPIOB_BASE_ADDR)
#define GPIOC		GPIO_PERIPH_CAST(GPIOC_BASE_ADDR)
#define GPIOD		GPIO_PERIPH_CAST(GPIOD_BASE_ADDR)
#define GPIOF		GPIO_PERIPH_CAST(GPIOF_BASE_ADDR)

#define I2C1		I2C_PERIPH_CAST(I2C1_BASE_ADDR)
#define I2C2 		I2C_PERIPH_CAST(I2C2_BASE_ADDR)

#define SPI1		SPI_PERIPH_CAST(SPI1_BASE_ADDR)
#define SPI2 		SPI_PERIPH_CAST(SPI2_BASE_ADDR)

#define USART1		UART_PERIPH_CAST(USART1_BASE_ADDR)
#define USART2 		UART_PERIPH_CAST(USART2_BASE_ADDR)
#define USART3 		UART_PERIPH_CAST(USART3_BASE_ADDR)
#define USART4		UART_PERIPH_CAST(USART4_BASE_ADDR)

#define RCC			RCC_PERIPH_CAST(RCC_BASE_ADDR)

#define EXTI		EXTI_PERIPH_CAST(EXTI_BASE_ADDR)
#define SYSCFG		SYSCFG_PERIPH_CAST(SYSCFG_BASE_ADDR)

#define SYSTICK		SYSTICK_PERIPH_CAST(SYSTICK_BASE_ADDR)



/*************** Clock configuration for peripherals *****************/

/*** Clock Enable Macros ***/
#define GPIOA_PCLK_EN()			(RCC->IOPENR |= (1 << 0))
#define GPIOB_PCLK_EN()			(RCC->IOPENR |= (1 << 1))
#define GPIOC_PCLK_EN()			(RCC->IOPENR |= (1 << 2))
#define GPIOD_PCLK_EN()			(RCC->IOPENR |= (1 << 3))
#define GPIOF_PCLK_EN()			(RCC->IOPENR |= (1 << 5))


#define I2C1_PCLK_EN()			(RCC->APBENR1 |= (1 << 21))
#define I2C2_PCLK_EN()			(RCC->APBENR1 |= (1 << 22))

#define SPI1_PCLK_EN()			(RCC->APBENR2 |= (1 << 12))
#define SPI2_PCLK_EN()			(RCC->APBENR1 |= (1 << 14))

#define USART1_PCLK_EN()		(RCC->APBENR2 |= (1 << 14))
#define USART2_PCLK_EN()		(RCC->APBENR1 |= (1 << 17))
#define USART3_PCLK_EN()		(RCC->APBENR1 |= (1 << 18))
#define USART4_PCLK_EN()		(RCC->APBENR1 |= (1 << 19))

#define SYSCFG_PCLK_EN()		(RCC->APBENR2 |= (1 << 0 ))

/*** Clock Disable Macros ***/
/* GPIO (IOPENR) */
#define GPIOA_PCLK_CLR()        (RCC->IOPENR &= ~(1U << 0))
#define GPIOB_PCLK_CLR()        (RCC->IOPENR &= ~(1U << 1))
#define GPIOC_PCLK_CLR()        (RCC->IOPENR &= ~(1U << 2))
#define GPIOD_PCLK_CLR()        (RCC->IOPENR &= ~(1U << 3))
#define GPIOF_PCLK_CLR()        (RCC->IOPENR &= ~(1U << 5))


/* I2C (APBENR1) */
#define I2C1_PCLK_CLR()         (RCC->APBENR1 &= ~(1U << 21))
#define I2C2_PCLK_CLR()         (RCC->APBENR1 &= ~(1U << 22))


/* SPI */
#define SPI1_PCLK_CLR()         (RCC->APBENR2 &= ~(1U << 12))
#define SPI2_PCLK_CLR()         (RCC->APBENR1 &= ~(1U << 14))


/* USART */
#define USART1_PCLK_CLR()       (RCC->APBENR2 &= ~(1U << 14))
#define USART2_PCLK_CLR()       (RCC->APBENR1 &= ~(1U << 17))
#define USART3_PCLK_CLR()       (RCC->APBENR1 &= ~(1U << 18))
#define USART4_PCLK_CLR()       (RCC->APBENR1 &= ~(1U << 19))


/* SYSCFG */
#define SYSCFG_PCLK_CLR()       (RCC->APBENR2 &= ~(1U << 0))



/*** Clock Reset Macros ***/
/*** Clock Reset Macros (STM32C031) ***/

/* GPIO (IOPRSTR) */
#define GPIOA_REG_RESET()  do{ (RCC->IOPRSTR |=  (1U << 0)); \
                               (RCC->IOPRSTR &= ~(1U << 0)); } while(0)

#define GPIOB_REG_RESET()  do{ (RCC->IOPRSTR |=  (1U << 1)); \
                               (RCC->IOPRSTR &= ~(1U << 1)); } while(0)

#define GPIOC_REG_RESET()  do{ (RCC->IOPRSTR |=  (1U << 2)); \
                               (RCC->IOPRSTR &= ~(1U << 2)); } while(0)

#define GPIOD_REG_RESET()  do{ (RCC->IOPRSTR |=  (1U << 3)); \
                               (RCC->IOPRSTR &= ~(1U << 3)); } while(0)

#define GPIOF_REG_RESET()  do{ (RCC->IOPRSTR |=  (1U << 5)); \
                               (RCC->IOPRSTR &= ~(1U << 5)); } while(0)


/* SPI */
#define SPI1_REG_RESET()   do{ (RCC->APBRSTR2 |=  (1U << 12)); \
                               (RCC->APBRSTR2 &= ~(1U << 12)); } while(0)

#define SPI2_REG_RESET()   do{ (RCC->APBRSTR1 |=  (1U << 14)); \
                               (RCC->APBRSTR1 &= ~(1U << 14)); } while(0)


/* I2C */
#define I2C1_REG_RESET()   do{ (RCC->APBRSTR1 |=  (1U << 21)); \
                               (RCC->APBRSTR1 &= ~(1U << 21)); } while(0)

#define I2C2_REG_RESET()   do{ (RCC->APBRSTR1 |=  (1U << 22)); \
                               (RCC->APBRSTR1 &= ~(1U << 22)); } while(0)


/* USART */
#define USART1_REG_RESET() do{ (RCC->APBRSTR2 |=  (1U << 14)); \
                               (RCC->APBRSTR2 &= ~(1U << 14)); } while(0)

#define USART2_REG_RESET() do{ (RCC->APBRSTR1 |=  (1U << 17)); \
                               (RCC->APBRSTR1 &= ~(1U << 17)); } while(0)

#define USART3_REG_RESET() do{ (RCC->APBRSTR1 |=  (1U << 18)); \
                               (RCC->APBRSTR1 &= ~(1U << 18)); } while(0)

#define USART4_REG_RESET() do{ (RCC->APBRSTR1 |=  (1U << 19)); \
                               (RCC->APBRSTR1 &= ~(1U << 19)); } while(0)


/* SYSCFG */
#define SYSCFG_REG_RESET() do{ (RCC->APBRSTR2 |=  (1U << 0)); \
                               (RCC->APBRSTR2 &= ~(1U << 0)); } while(0)


#define GPIO_BASEADDR_TO_CODE(x)   ( \
    ((x) == GPIOA) ? 0U :          \
    ((x) == GPIOB) ? 1U :          \
    ((x) == GPIOC) ? 2U :          \
    ((x) == GPIOD) ? 3U :          \
    ((x) == GPIOF) ? 5U : 0U       \
)


/************ Generic macros ************/
/*
 * Used to set/show the status of the peripheral
 */
typedef enum {
    PERIPH_ENABLE,
    PERIPH_DISABLE,
    PERIPH_SET,
    PERIPH_RESET
} PeriphState_t;

#define GPIO_PIN_SET		PERIPH_SET
#define GPIO_PIN_RESET		PERIPH_RESET

#define SPI_FLAG_SET		PERIPH_SET
#define SPI_FLAG_RESET		PERIPH_RESET

/*
 * Return types
 */
typedef enum
{
    PERIPH_OK = 0,
    PERIPH_ERROR,
    PERIPH_BUSY,
    PERIPH_TIMEOUT,
    PERIPH_INVALID_CONFIG
} PeriphStatus_t;


typedef enum
{
    NVIC_IRQ_PRI0 = 0,  // Highest
    NVIC_IRQ_PRI1,
    NVIC_IRQ_PRI2,
    NVIC_IRQ_PRI3   // Lowest
} NVIC_IRQ_Priority_t;


typedef enum
{
    IRQ_NO_EXTI0_1      = 5,
    IRQ_NO_EXTI2_3      = 6,
    IRQ_NO_EXTI4_15     = 7,

    IRQ_NO_I2C1         = 23,
    IRQ_NO_I2C2         = 24,

    IRQ_NO_SPI1         = 25,
    IRQ_NO_SPI2         = 26,

    IRQ_NO_USART1       = 27,
    IRQ_NO_USART2       = 28,
    IRQ_NO_USART3_4     = 29,

} IRQn_Number_t;


#endif /* INC_STM32C031XX_H_ */
