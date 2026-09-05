/*
 * led-blink/main.c
 *
 * Plain baremetal blink of the Bluepill's onboard LED (PC13, wired
 * active-low) - no RTOS, no vendor library, just direct register
 * writes plus a SysTick-driven millisecond delay.
 */
#include <stdint.h>
#include "stm32f103_regs.h"

static volatile uint32_t g_msticks;

void SysTick_Handler(void)
{
    g_msticks++;
}

static void delay_ms(uint32_t ms)
{
    uint32_t target = g_msticks + ms;
    while (g_msticks < target) {
    }
}

int main(void)
{
    /* enable GPIOC clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* PC13: output push-pull, 2 MHz (pin 13 -> CRH, nibble at bit 20) */
    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |=  (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << 20);

    /* SysTick at the default HSI 8 MHz core clock -> 1 kHz tick */
    SYSTICK->LOAD = (8000000U / 1000U) - 1U;
    SYSTICK->VAL  = 0;
    SYSTICK->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;

    for (;;) {
        GPIOC->ODR ^= (1U << 13);
        delay_ms(500);
    }
}
