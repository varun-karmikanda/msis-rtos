/*
 * three-thread-blink_priority/main.c
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


static volatile uint32_t g_msticks;
typedef struct {
    GPIO_TypeDef *port;
    uint32_t      pin;
    uint32_t      period_ms;
} led_task_args_t;

void vApplicationTickHook(void)
{
    g_msticks++;
}

static const led_task_args_t led_a = { GPIOA, 3, 500};
static const led_task_args_t led_b = { GPIOA, 0, 450};
static const led_task_args_t led_c = { GPIOA, 1, 400};
static const led_task_args_t led_d = { GPIOA, 2, 350};
static void delay_ms(uint32_t ms)
{
    uint32_t target = g_msticks + ms;
    while (g_msticks < target) {
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
        delay_ms(args->period_ms);
        // vTaskDelay(pdMS_TO_TICKS(args->period_ms));
    }
}

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    gpio_output_init(led_a.port, led_a.pin);
    gpio_output_init(led_b.port, led_b.pin);
    gpio_output_init(led_c.port, led_c.pin);
    gpio_output_init(led_d.port, led_d.pin);

    xTaskCreate(led_task, "led_1s", configMINIMAL_STACK_SIZE, (void *)&led_a, tskIDLE_PRIORITY + 3, NULL);
    xTaskCreate(led_task, "led_2s", configMINIMAL_STACK_SIZE, (void *)&led_b, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(led_task, "led_3s", configMINIMAL_STACK_SIZE, (void *)&led_c, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(led_task, "led_4s", configMINIMAL_STACK_SIZE, (void *)&led_d, tskIDLE_PRIORITY + 0, NULL);

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

void vApplicationIdleHook(void)
{
    __asm volatile ("wfi");
}