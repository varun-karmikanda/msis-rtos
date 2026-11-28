#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

SemaphoreHandle_t mutex_A;
SemaphoreHandle_t mutex_B;

int balance = 1000;
int transactions = 0;

/* Task 1: Deposit money */
void task1(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(mutex_A, portMAX_DELAY);

        printf("Task 1: Acquired Mutex A\n");

        vTaskDelay(pdMS_TO_TICKS(100));

        xSemaphoreTake(mutex_B, portMAX_DELAY);

        balance += 500;
        transactions++;

        printf("Task 1: Deposited 500\n");
        printf("Balance: %d\n", balance);
        printf("Transactions: %d\n", transactions);

        xSemaphoreGive(mutex_B);
        xSemaphoreGive(mutex_A);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Task 2: Withdraw money */
void task2(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(mutex_B, portMAX_DELAY);

        printf("Task 2: Acquired Mutex B\n");

        vTaskDelay(pdMS_TO_TICKS(100));

        xSemaphoreTake(mutex_A, portMAX_DELAY);

        if (balance >= 200)
        {
            balance -= 200;
            transactions++;

            printf("Task 2: Withdrew 200\n");
        }

        printf("Balance: %d\n", balance);
        printf("Transactions: %d\n", transactions);

        xSemaphoreGive(mutex_A);
        xSemaphoreGive(mutex_B);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    mutex_A = xSemaphoreCreateMutex();
    mutex_B = xSemaphoreCreateMutex();

    if (mutex_A == NULL || mutex_B == NULL)
    {
        for (;;);
    }

    xTaskCreate(task1, "Deposit", 256, NULL, 1, NULL);

    xTaskCreate(task2, "Withdraw", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;);
}