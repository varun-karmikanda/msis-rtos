/*
 * priority-inversion/main.c
 *
 * Demonstrates the classic priority-inversion problem:
 *
 *   Task1  HIGH  (tskIDLE_PRIORITY + 3)  blinks PA0, 500 ms on/off
 *   Task2  LOW   (tskIDLE_PRIORITY + 1)  blinks PA1, 500 ms on/off
 *   Task3  MED   (tskIDLE_PRIORITY + 2)  blinks PC13, busy-wait 500 ms x 5
 *                                         cycles then sleeps 500 ms once
 *
 * PA0 and PA1 are collectively protected by a BINARY SEMAPHORE
 * (not a mutex).  PC13 is unprotected and Task3 never touches the
 * semaphore.
 *
 * Why a binary semaphore, not xSemaphoreCreateMutex()?
 * A FreeRTOS mutex has priority inheritance: the instant a high-priority
 * task blocks on it, the kernel temporarily raises the holder to the
 * high-priority task's level so it can run and release the mutex.
 * That's the FIX for priority inversion, not the bug.  A binary
 * semaphore has zero awareness of task priorities - it will never
 * bump anyone's priority.  Using it here lets the inversion play out
 * without any automatic mitigation.
 *
 * THE INVERSION SEQUENCE
 * ----------------------
 * T=0   Task2 (LOW) wakes, takes the semaphore, starts blinking PA1.
 *        It immediately vTaskDelay(500) - now it is Blocked.
 *
 * T~0   Task3 (MED) wakes, preempts Task2 (MED > LOW), and enters its
 *        busy-wait loop.  It spins for ~500 ms without ever calling
 *        vTaskDelay, so it stays in the Running/Ready state and the
 *        scheduler never picks Task2.
 *
 * T~500 Task1 (HIGH) wakes, tries to take the semaphore.
 *        Task2 still holds it (Task2 has been unable to run and
 *        release it because Task3 is ahead of it).
 *        Task1 blocks on the semaphore.
 *
 * Result: HIGH is blocked behind LOW, and LOW cannot run because
 *         MED is busy-waiting.  MED effectively blocks HIGH.
 *         This is priority inversion.
 *
 * Task3 loops 5 times (5 x 500 ms = ~5 s of busy-wait) then finally
 * calls vTaskDelay(500).  At that point Task2 is the highest Ready
 * task, runs, toggles PA1, then gives the semaphore.  Task1 unblocks
 * immediately - it has been waiting the entire 5 s.
 *
 * HARDWARE
 * --------
 * PA0  - LED1, active-high, external LED + ~330R to GND
 * PA1  - LED2, active-high, external LED + ~330R to GND
 * PC13 - onboard LED, active-low
 */
#include <stdint.h>
#include "stm32f103_regs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* ------------------------------------------------------------------ */
/* Priorities                                                           */
/* ------------------------------------------------------------------ */
#define PRIO_LOW    (tskIDLE_PRIORITY + 1)
#define PRIO_MED    (tskIDLE_PRIORITY + 2)
#define PRIO_HIGH   (tskIDLE_PRIORITY + 3)

/* ------------------------------------------------------------------ */
/* Shared resource protection - BINARY SEMAPHORE, not a mutex          */
/* A mutex would silently fix the inversion via priority inheritance.  */
/* ------------------------------------------------------------------ */
static SemaphoreHandle_t xGpioABinarySem;

/* ------------------------------------------------------------------ */
/* Busy-wait: spins on SysTick for the requested milliseconds.         */
/* Does NOT call vTaskDelay, so the calling task never leaves Running. */
/* This is the mechanism that lets Task3 starve Task2.                 */
/* ------------------------------------------------------------------ */
static volatile uint32_t g_ticks = 0;   /* incremented by tick hook */

void vApplicationTickHook(void)
{
    g_ticks++;
}

static void busy_wait_ms(uint32_t ms)
{
    uint32_t start = g_ticks;
    while ((g_ticks - start) < ms) {
        /* spin - deliberately consuming the CPU */
    }
}

/* ------------------------------------------------------------------ */
/* GPIO helpers                                                         */
/* ------------------------------------------------------------------ */
static void gpio_output_init(GPIO_TypeDef *port, uint32_t pin)
{
    volatile uint32_t *cr = (pin < 8) ? &port->CRL : &port->CRH;
    uint32_t shift = (pin % 8U) * 4U;
    *cr &= ~(0xFU << shift);
    *cr |=  (GPIO_CNF_MODE_OUTPUT_PP_2MHZ << shift);
}

/* ------------------------------------------------------------------ */
/* Task1 - HIGH priority - blinks PA0 - needs the semaphore            */
/* ------------------------------------------------------------------ */
static void task1_high(void *pvParameters)
{
    (void)pvParameters;
    /* give Task2 a head-start so it can acquire the semaphore first */
    vTaskDelay(pdMS_TO_TICKS(10));

    for (;;) {
        /* --- WAIT for the shared GPIO-A semaphore --- */
        xSemaphoreTake(xGpioABinarySem, portMAX_DELAY);

        /* PA0 on */
        GPIOA->ODR |=  (1U << 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        /* PA0 off */
        GPIOA->ODR &= ~(1U << 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        xSemaphoreGive(xGpioABinarySem);
    }
}

/* ------------------------------------------------------------------ */
/* Task2 - LOW priority - blinks PA1 - needs the semaphore             */
/* ------------------------------------------------------------------ */
static void task2_low(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        /* Task2 acquires the semaphore first (it starts before Task1
         * and Task3 because of the 10 ms head-start above).
         * Once Task3 wakes and busy-waits, Task2 cannot run to give
         * the semaphore back - trapping Task1. */
        xSemaphoreTake(xGpioABinarySem, portMAX_DELAY);

        /* PA1 on */
        GPIOA->ODR |=  (1U << 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        /* PA1 off */
        GPIOA->ODR &= ~(1U << 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        xSemaphoreGive(xGpioABinarySem);
    }
}

/* ------------------------------------------------------------------ */
/* Task3 - MED priority - blinks PC13 - no semaphore                   */
/* Busy-waits 500 ms x 5 cycles (5 s total) then sleeps 500 ms once.  */
/* The busy-wait is what blocks Task2 from running and releasing the   */
/* semaphore that Task1 is waiting on.                                 */
/* ------------------------------------------------------------------ */
static void task3_med(void *pvParameters)
{
    (void)pvParameters;
    for (;;) {
        /* --- 5 busy-wait cycles of 500 ms each -------------------- */
        for (uint32_t i = 0; i < 5; i++) {
            /* PC13 toggle (active-low, so XOR gives a visible blink) */
            GPIOC->ODR ^= (1U << 13);

            /* BUSY-WAIT: does NOT yield the CPU - this is the key.
             * While spinning here, Task2 (which holds the semaphore)
             * cannot run because Task3 (MED) outranks Task2 (LOW).
             * Task1 (HIGH) is blocked on the semaphore.
             * => MED is blocking HIGH through LOW: inversion. */
            busy_wait_ms(500);
        }

        /* --- one cooperative sleep to eventually let things flow --- */
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */
int main(void)
{
    /* enable clocks for GPIOA and GPIOC */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    gpio_output_init(GPIOA, 0);   /* PA0 - Task1 LED */
    gpio_output_init(GPIOA, 1);   /* PA1 - Task2 LED */
    gpio_output_init(GPIOC, 13);  /* PC13 - Task3 onboard LED */

    /* BINARY semaphore - deliberately not a mutex */
    xGpioABinarySem = xSemaphoreCreateBinary();
    xSemaphoreGive(xGpioABinarySem);   /* start it available */

    xTaskCreate(task1_high, "T1_HIGH", configMINIMAL_STACK_SIZE, NULL, PRIO_HIGH, NULL);
    xTaskCreate(task2_low,  "T2_LOW",  configMINIMAL_STACK_SIZE, NULL, PRIO_LOW,  NULL);
    xTaskCreate(task3_med,  "T3_MED",  configMINIMAL_STACK_SIZE, NULL, PRIO_MED,  NULL);

    vTaskStartScheduler();

    for (;;) {}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask; (void)pcTaskName;
    for (;;) {}
}
