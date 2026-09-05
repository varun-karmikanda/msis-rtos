/*
 * three-thread-blink_hse/main.c
 *
 * Same three LED tasks as three-thread-blink (PC13/1s, PA0/2s, PA1/3s),
 * with two differences:
 *
 *   1. The chip is switched from the default 8 MHz HSI onto its 8 MHz
 *      HSE crystal, run through the PLL up to 72 MHz - see
 *      clock_setup_hse_pll_72mhz() below.
 *
 *   2. vApplicationIdleHook() puts the CPU into real Sleep mode (the
 *      `wfi` instruction) instead of letting the idle task spin a
 *      software loop. Whenever all three LED tasks are Blocked in
 *      vTaskDelay(), the core is genuinely halted - not executing any
 *      instructions - until the next interrupt (SysTick, 1 ms later)
 *      wakes it back up. That's *not* the same as full tickless idle
 *      (SysTick keeps ticking every 1 ms either way, so the CPU still
 *      wakes 1000x/second) - it's the simple half of the idea: don't
 *      busy-loop when there's nothing to do, ever, at any level.
 */
#include <stdint.h>
#include "stm32f103_regs.h"

#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    GPIO_TypeDef *port;
    uint32_t      pin;
    uint32_t      period_ms;
} led_task_args_t;

static const led_task_args_t led_a = { GPIOC, 13, 1000 };
static const led_task_args_t led_b = { GPIOA, 0,  2000 };
static const led_task_args_t led_c = { GPIOA, 1,  3000 };

/*
 * HSE (8 MHz crystal) -> PLL x9 -> SYSCLK 72 MHz, AHB /1, APB2 /1,
 * APB1 /2 (its 36 MHz ceiling), 2 flash wait states. RM0008 §7.3.
 */
static void clock_setup_hse_pll_72mhz(void)
{
    /* 1. start the external oscillator and wait for it to settle.
     *    Bounded wait: if no crystal is fitted this gives up and the
     *    chip is simply left running on its default HSI clock instead
     *    of hanging forever. */
    RCC->CR |= RCC_CR_HSEON;
    for (volatile uint32_t timeout = 500000U; timeout > 0U; timeout--) {
        if ((RCC->CR & RCC_CR_HSERDY) != 0U) {
            break;
        }
        if (timeout == 1U) {
            return;   /* no HSE - stay on HSI, don't touch anything else */
        }
    }

    /* 2. flash must be able to keep up with 72 MHz before we switch to
     *    it - set this BEFORE raising the core clock, never after. */
    FLASHREGS->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS;

    /* 3. configure the prescalers and PLL while still running on HSE
     *    (SW hasn't been changed yet - this is safe to write now). */
    RCC->CFGR = RCC_CFGR_PPRE1_DIV2        /* APB1 = 72/2 = 36 MHz (its max) */
              | RCC_CFGR_PLLSRC_HSE        /* PLL input = HSE, not HSI/2     */
              | RCC_CFGR_PLLXTPRE_DIV1     /* HSE fed to PLL undivided       */
              | RCC_CFGR_PLLMUL_9;         /* 8 MHz x 9 = 72 MHz             */

    /* 4. start the PLL and wait for it to lock. */
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) {
    }

    /* 5. switch SYSCLK onto the PLL, then wait for the switch to take -
     *    SWS mirrors SW once the hardware has actually completed it. */
    RCC->CFGR = (RCC->CFGR & ~0x3U) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {
    }
}

static void gpio_output_init(GPIO_TypeDef *port, uint32_t pin)
{
    volatile uint32_t *cr = (pin < 8) ? &port->CRL : &port->CRH;
    uint32_t shift = (pin % 8U) * 4U;

    *cr &= ~(0xFU << shift);
    *cr |=  (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << shift);
}

static void led_task(void *pvParameters)
{
    const led_task_args_t *args = (const led_task_args_t *)pvParameters;

    for (;;) {
        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));
    }
}

int main(void)
{
    clock_setup_hse_pll_72mhz();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    gpio_output_init(led_a.port, led_a.pin);
    gpio_output_init(led_b.port, led_b.pin);
    gpio_output_init(led_c.port, led_c.pin);

    xTaskCreate(led_task, "led_1s", configMINIMAL_STACK_SIZE, (void *)&led_a, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(led_task, "led_2s", configMINIMAL_STACK_SIZE, (void *)&led_b, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(led_task, "led_3s", configMINIMAL_STACK_SIZE, (void *)&led_c, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    for (;;) {
        /* only reached if vTaskStartScheduler() fails, e.g. out of heap */
    }
}

/* Required because FreeRTOSConfig.h sets configCHECK_FOR_STACK_OVERFLOW=2 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    for (;;) {
    }
}

/*
 * Required because FreeRTOSConfig.h sets configUSE_IDLE_HOOK=1. Runs every
 * time the idle task is scheduled - i.e. whenever all three LED tasks are
 * Blocked. `wfi` (Wait For Interrupt) genuinely halts the core - clock
 * gated, no instructions executing - until the next exception, which in
 * steady state is SysTick, 1 ms later. Safe to call unconditionally: if an
 * interrupt is already pending the moment this runs, wfi is architecturally
 * defined to complete immediately rather than sleep through it.
 */
void vApplicationIdleHook(void)
{
    __asm volatile ("wfi");
}
