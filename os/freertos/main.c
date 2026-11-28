#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t mutex_A;
SemaphoreHandle_t mutex_B;

/* Task 1 */
void task1(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(mutex_A, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));

        /* Wait forever for B */
        xSemaphoreTake(mutex_B, portMAX_DELAY);

        /* Critical section */

        xSemaphoreGive(mutex_B);
        xSemaphoreGive(mutex_A);
    }
}

/* Task 2 */
void task2(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(mutex_B, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));

        /* Wait forever for A */
        xSemaphoreTake(mutex_A, portMAX_DELAY);

        /* Critical section */

        xSemaphoreGive(mutex_A);
        xSemaphoreGive(mutex_B);
    }
}

int main(void)
{
    /* Create mutexes */
    mutex_A = xSemaphoreCreateMutex();
    mutex_B = xSemaphoreCreateMutex();

    if (mutex_A == NULL || mutex_B == NULL)
    {
        for (;;);
    }

    /* Create two tasks */
    xTaskCreate(task1, "Task1", configMINIMAL_STACK_SIZE,
                NULL, 1, NULL);

    xTaskCreate(task2, "Task2", configMINIMAL_STACK_SIZE,
                NULL, 1, NULL);

    /* Start scheduler */
    vTaskStartScheduler();

    for (;;);
}