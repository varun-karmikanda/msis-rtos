/*
 * three-thread-blink/main.c
 *
 * Three independent FreeRTOS tasks, each blinking its own LED at its
 * own fixed interval:
 *   - PC13 (the Bluepill's onboard LED, active-low)  -> 1 s
 *   - PA0  (external LED + ~330R resistor to ground)  -> 2 s
 *   - PA1  (external LED + ~330R resistor to ground)  -> 3 s
 *
 * Only PC13 exists on the board itself; wire the other two LEDs up
 * yourself, or just change led_b/led_c to reuse GPIOC/13 to see all
 * three task periods overlaid on the one onboard LED instead.
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
