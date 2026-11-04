/*
 * shared-led-mutex/main.c
 *
 * Same two tasks as shared-led-no-sync, but this time a mutex is held
 * across the ENTIRE blink - not just each individual toggle:
 *
 *   task1_toggle_1hz - take mutex; on, delay 1000, off, delay 1000; give mutex
 *   task2_toggle_2hz - take mutex; on, delay  500, off, delay  500; give mutex
 *
 * This does fully remove the lost-toggle race from Q V1/V2 - while one
 * task holds the mutex, the other cannot touch PC13 at all, not even
 * once. But see the note at the bottom of this file for what that
 * actually costs: it does not merely fix the race, it changes the
 * program's behaviour altogether.
 */
#include <stdint.h>
#include "stm32f103_regs.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t xLedMutex;

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
        xSemaphoreTake(xLedMutex, portMAX_DELAY);
        GPIOC->ODR ^= (1U << 13);           /* on */
        vTaskDelay(pdMS_TO_TICKS(1000));
        GPIOC->ODR ^= (1U << 13);           /* off */
        vTaskDelay(pdMS_TO_TICKS(1000));
        xSemaphoreGive(xLedMutex);
        /* the mutex is held for the WHOLE 2 s blink, not just each toggle */
    }
}

static void task2_toggle_2hz(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        xSemaphoreTake(xLedMutex, portMAX_DELAY);
        GPIOC->ODR ^= (1U << 13);           /* on */
        vTaskDelay(pdMS_TO_TICKS(500));
        GPIOC->ODR ^= (1U << 13);           /* off */
        vTaskDelay(pdMS_TO_TICKS(500));
        xSemaphoreGive(xLedMutex);
        /* held for the WHOLE 1 s blink, not just each toggle */
    }
}

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    gpio_output_init(GPIOC, 13);

    xLedMutex = xSemaphoreCreateMutex();

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
 * What synchronizing the WHOLE blink actually does to the behaviour:
 *
 * The lost-toggle race from Q V1/V2 is completely gone - correct, and
 * exactly what a mutex is for. But holding the mutex across the full
 * on-delay-off-delay sequence means each task now blocks the OTHER
 * one out of running its own blink at all, for as long as the first
 * one's entire cycle takes:
 *
 *   - Whichever task acquires the mutex first runs its full blink
 *     uninterrupted - task1's full 2 s cycle, or task2's full 1 s
 *     cycle - while the other task sits blocked on xSemaphoreTake(),
 *     unable to touch the pin even though it isn't actually racing
 *     with anything during that time.
 *   - If task1 wins the mutex, task2's intended "twice a second"
 *     schedule is not just delayed slightly - it is blocked for up to
 *     2 full seconds, then it runs its own complete 1 s blink, then
 *     task1 (which has been waiting the whole time) gets its turn.
 *   - The two tasks end up taking turns running whole blinks back to
 *     back, rather than interleaving on their own independent
 *     schedules. Neither task's 1 Hz / 2 Hz timing survives contact
 *     with the other once the WHOLE operation is what's serialized.
 *
 * This is the real trade-off "lock the whole operation" always makes:
 * it buys correctness on the shared resource by giving up the
 * independence the two tasks were supposed to have. A narrower fix -
 * a mutex (or just GPIOC->BSRR) around only the individual toggle,
 * not the delays around it - would remove the race while leaving each
 * task free to keep its own timing. Whether that trade-off is
 * acceptable depends entirely on whether "two independently-timed
 * blinkers" or "one shared correct sequence" is actually the goal.
 */
