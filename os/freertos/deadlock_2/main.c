#include <stdint.h>
#include "stm32f103_regs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t mutex_A;
SemaphoreHandle_t mutex_B;

typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
    uint32_t period_ms;
} led_task_args_t;

static const led_task_args_t led_a = {GPIOA, 0, 500};
static const led_task_args_t led_b = {GPIOA, 1, 500};

/* Configure GPIO as output push-pull */
static void gpio_output_init(GPIO_TypeDef *port, uint32_t pin)
{
    volatile uint32_t *cr = (pin < 8) ? &port->CRL : &port->CRH;

    uint32_t shift = (pin % 8U) * 4U;

    *cr &= ~(0xFU << shift);
    *cr |= (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << shift);
}

/* Task 1: Acquires A, then B */
static void led_task_1(void *pvParameters)
{
    const led_task_args_t *args =
        (const led_task_args_t *)pvParameters;

    for (;;)
    {
        xSemaphoreTake(mutex_A, portMAX_DELAY);

        /* Wait indefinitely for mutex B */
        xSemaphoreTake(mutex_B, portMAX_DELAY);

        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));

        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));

        xSemaphoreGive(mutex_B);
        xSemaphoreGive(mutex_A);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* Task 2: Acquires B, then A */
static void led_task_2(void *pvParameters)
{
    const led_task_args_t *args =
        (const led_task_args_t *)pvParameters;

    for (;;)
    {
        xSemaphoreTake(mutex_B, portMAX_DELAY);

        /* Wait indefinitely for mutex A */
        xSemaphoreTake(mutex_A, portMAX_DELAY);

        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));

        args->port->ODR ^= (1U << args->pin);
        vTaskDelay(pdMS_TO_TICKS(args->period_ms));

        xSemaphoreGive(mutex_A);
        xSemaphoreGive(mutex_B);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void)
{
    /* Enable GPIOA clock */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* Configure PA0 and PA1 */
    gpio_output_init(led_a.port, led_a.pin);
    gpio_output_init(led_b.port, led_b.pin);

    /* Create mutexes */
    mutex_A = xSemaphoreCreateMutex();
    mutex_B = xSemaphoreCreateMutex();

    if (mutex_A == NULL || mutex_B == NULL)
    {
        for (;;);
    }

    /* Create tasks */
    xTaskCreate(
        led_task_1,
        "LED_Task1",
        configMINIMAL_STACK_SIZE,
        (void *)&led_a,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    xTaskCreate(
        led_task_2,
        "LED_Task2",
        configMINIMAL_STACK_SIZE,
        (void *)&led_b,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    /* Start scheduler */
    vTaskStartScheduler();

    for (;;);
}

/* Stack overflow hook */
void vApplicationStackOverflowHook(
    TaskHandle_t xTask,
    char *pcTaskName
)
{
    (void)xTask;
    (void)pcTaskName;

    for (;;);
}