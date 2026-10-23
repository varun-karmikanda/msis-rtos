#include <stdint.h>
#include "stm32f103_regs.h"
#include "FreeRTOS.h"
#include "task.h"

typedef struct {
    GPIO_TypeDef    *port;
    uint32_t        pin;
    uint32_t        period_ms;
} led_task_args_t;

static const led_task_args_t led_a = { GPIOA, 0, 500 };
static const led_task_args_t led_b = { GPIOA, 1, 500 };

static void gpio_output_init(GPIO_TypeDef *port, uint32_t pin){
    volatile uint32_t *cr = (pin < 8) ? &port->CRL : &port->CRH;
    uint32_t shift = (pin % 8U) * 4U;

    *cr &= ~(0xFU << shift);
    *cr |= (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << shift);
}

static void led_task_1(void *pvParameters){
    const led_task_args_t *args = (const led_task_args_t *)pvParameters;

    for (;;)
    {
        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));
    }
}

static void led_task_2(void *pvParameters){
    const led_task_args_t *args = (const led_task_args_t *)pvParameters;

    for (;;)
    {
        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));
    }
}

int main(void){
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    gpio_output_init(led_a.port, led_a.pin);
    gpio_output_init(led_b.port, led_b.pin);

    xTaskCreate(led_task_1, "thread_1", configMINIMAL_STACK_SIZE, (void *)&led_a, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(led_task_2, "thread_2", configMINIMAL_STACK_SIZE, (void *)&led_b, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();

    for(;;){}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    for (;;) {}
}
