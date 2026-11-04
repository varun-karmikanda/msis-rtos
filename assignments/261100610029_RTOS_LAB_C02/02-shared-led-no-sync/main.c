/*
 * shared-led-no-sync/main.c
 *
 * Two tasks, same priority, both toggling the SAME GPIO pin (PC13, the
 * Bluepill's onboard LED) with no mutex, no critical section, no
 * coordination between them at all. Each task's loop is now explicit
 * about on/off rather than a single toggle per iteration:
 *
 *   task1_toggle_1hz - on, delay 1000 ms, off, delay 1000 ms
 *                      -> one complete blink every 2 s
 *   task2_toggle_2hz - on, delay  500 ms, off, delay  500 ms
 *                      -> one complete blink every 1 s
 *
 * The underlying toggle rate on the pin is unchanged from the simpler
 * version of this file - this only makes "on" and "off" explicit
 * instead of letting them fall out of alternating toggles.
 *
 * This is deliberately the "wrong" way to share a pin - see the note
 * at the bottom of this file for what that actually costs you.
 */
#include <stdint.h>
#include "stm32f103_regs.h"

#include "FreeRTOS.h"
#include "task.h"

static void gpio_output_init(GPIO_TypeDef *port, uint32_t pin)
{
    volatile uint32_t *cr = (pin < 8) ? &port->CRL : &port->CRH;
    uint32_t shift = (pin % 8U) * 4U;

    *cr &= ~(0xFU << shift);
    *cr |=  (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << shift);
}

static void task1_toggle_1hz(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        GPIOC->ODR ^= (1U << 13);           /* on */
        vTaskDelay(pdMS_TO_TICKS(1000));
        GPIOC->ODR ^= (1U << 13);           /* off */
        vTaskDelay(pdMS_TO_TICKS(1000));
        /* one full loop iteration = one complete blink, 2000 ms total */
    }
}

static void task2_toggle_2hz(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        GPIOC->ODR ^= (1U << 13);           /* on */
        vTaskDelay(pdMS_TO_TICKS(500));
        GPIOC->ODR ^= (1U << 13);           /* off */
        vTaskDelay(pdMS_TO_TICKS(500));
        /* one full loop iteration = one complete blink, 1000 ms total */
    }
}

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    gpio_output_init(GPIOC, 13);

    xTaskCreate(task1_toggle_1hz, "task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(task2_toggle_2hz, "task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

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
 * What "no synchronization" actually costs here:
 *
 * GPIOC->ODR ^= (1U << 13) is NOT a single atomic operation - it compiles
 * to a load of ODR, an XOR, and a store back to ODR. Both tasks run this
 * exact sequence on the exact same register.
 *
 * Since these two tasks share one priority, a SysTick-driven context
 * switch can land between the load and the store of either task's
 * toggle. If task1 is preempted right there, and task2 runs an entire
 * toggle to completion before task1 resumes, task1 finishes its own
 * store using the STALE value it loaded before being preempted -
 * silently undoing task2's toggle. One of the two intended edges on
 * PC13 is lost, with no error, no warning, and no way to tell from
 * the code alone that it happened.
 *
 * The intended behaviour visually - if nothing ever raced - would be
 * an LED that spends noticeably more time in one state than the other
 * (task2's extra mid-second toggle keeps flipping it). What you'll
 * actually observe, run for long enough, is that pattern occasionally
 * glitching - a "toggle" that doesn't visibly happen - as this race
 * resolves the wrong way. It's rare, not visible on every run, and
 * exactly the kind of bug that a mutex around this read-modify-write
 * (or simply using GPIOC->BSRR to set/reset the specific bit atomically
 * from a single write) makes impossible instead of merely unlikely.
 */
