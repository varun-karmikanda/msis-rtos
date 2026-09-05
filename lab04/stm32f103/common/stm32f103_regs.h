/*
 * stm32f103_regs.h
 *
 * Minimal, self-written peripheral register definitions for the
 * STM32F103C8T6 (Bluepill) — just enough to drive RCC clock-enable bits,
 * GPIO, and the Cortex-M3 core's SysTick timer. No CMSIS, no HAL, no
 * libopencm3 — every address and bit here is taken directly from:
 *   - RM0008 (STM32F101/102/103/105/107 Reference Manual)
 *   - The ARMv7-M Architecture Reference Manual (for SysTick, a core
 *     peripheral defined by ARM, not ST — its address is the same on
 *     every Cortex-M3 part).
 *
 * Only what these two demo projects actually use is defined here. Add
 * registers/bits as you need them for other peripherals.
 */
#ifndef STM32F103_REGS_H
#define STM32F103_REGS_H

#include <stdint.h>

/* ---------------------------------------------------------------- */
/* RCC — Reset and Clock Control (RM0008 §7)                         */
/* ---------------------------------------------------------------- */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_TypeDef;

#define RCC_BASE        0x40021000UL
#define RCC             ((RCC_TypeDef *)RCC_BASE)

#define RCC_APB2ENR_IOPAEN   (1U << 2)
#define RCC_APB2ENR_IOPBEN   (1U << 3)
#define RCC_APB2ENR_IOPCEN   (1U << 4)

/* RCC_CR — clock sources (RM0008 §7.3.1) */
#define RCC_CR_HSEON     (1U << 16)  /* HSE oscillator on              */
#define RCC_CR_HSERDY    (1U << 17)  /* HSE ready                       */
#define RCC_CR_PLLON     (1U << 24)  /* PLL on                          */
#define RCC_CR_PLLRDY    (1U << 25)  /* PLL locked and ready            */

/* RCC_CFGR — system clock switch and PLL config (RM0008 §7.3.2) */
#define RCC_CFGR_SW_HSE         (0x1U)          /* SW[1:0]  = HSE       */
#define RCC_CFGR_SW_PLL         (0x2U)          /* SW[1:0]  = PLL       */
#define RCC_CFGR_SWS_MASK       (0x3U << 2)     /* SWS[1:0], read-only  */
#define RCC_CFGR_SWS_PLL        (0x2U << 2)
#define RCC_CFGR_PPRE1_DIV2     (0x4U << 8)     /* APB1 = SYSCLK/2 (<=36MHz) */
#define RCC_CFGR_PLLSRC_HSE     (1U << 16)      /* PLL input = HSE, not HSI/2 */
#define RCC_CFGR_PLLXTPRE_DIV1  (0U << 17)      /* HSE fed to PLL undivided */
#define RCC_CFGR_PLLMUL_9       (0x7U << 18)    /* PLLMUL = x9 (0111 -> x9) */

/* ---------------------------------------------------------------- */
/* GPIO (RM0008 §9) — F1 "legacy" CRL/CRH config style                */
/* ---------------------------------------------------------------- */
typedef struct {
    volatile uint32_t CRL;   /* config for pins 0-7  (4 bits/pin)   */
    volatile uint32_t CRH;   /* config for pins 8-15 (4 bits/pin)   */
    volatile uint32_t IDR;   /* input data register                */
    volatile uint32_t ODR;   /* output data register                */
    volatile uint32_t BSRR;  /* atomic set/reset register            */
    volatile uint32_t BRR;   /* atomic reset register                */
    volatile uint32_t LCKR;  /* config lock register                 */
} GPIO_TypeDef;

#define GPIOA_BASE      0x40010800UL
#define GPIOB_BASE      0x40010C00UL
#define GPIOC_BASE      0x40011000UL

#define GPIOA           ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB           ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC           ((GPIO_TypeDef *)GPIOC_BASE)

/* CRL/CRH nibble for "output push-pull, max 2 MHz": MODE=10, CNF=00 */
#define GPIO_CNF_MODE_OUTPUT_PP_2MHZ   0x2U

/* ---------------------------------------------------------------- */
/* SysTick — ARMv7-M core peripheral, same address on every Cortex-M3 */
/* ---------------------------------------------------------------- */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define SYSTICK_BASE            0xE000E010UL
#define SYSTICK                 ((SysTick_TypeDef *)SYSTICK_BASE)

#define SYSTICK_CTRL_ENABLE     (1U << 0)
#define SYSTICK_CTRL_TICKINT    (1U << 1)
#define SYSTICK_CTRL_CLKSOURCE  (1U << 2)  /* 1 = processor clock (AHB) */

/* ---------------------------------------------------------------- */
/* FLASH — only the access-control register (RM0008 §3.3.3);         */
/* flash *programming* registers are out of scope for this workspace */
/* ---------------------------------------------------------------- */
typedef struct {
    volatile uint32_t ACR;
} FLASH_TypeDef;

#define FLASH_BASE              0x40022000UL
#define FLASHREGS               ((FLASH_TypeDef *)FLASH_BASE)

#define FLASH_ACR_LATENCY_2WS   (0x2U)      /* 2 wait states: 48 < SYSCLK <= 72 MHz */
#define FLASH_ACR_PRFTBE        (1U << 4)   /* prefetch buffer enable */

#endif /* STM32F103_REGS_H */
