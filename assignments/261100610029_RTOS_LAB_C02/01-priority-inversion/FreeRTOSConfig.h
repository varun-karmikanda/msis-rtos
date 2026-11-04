#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configCPU_CLOCK_HZ              8000000UL
#define configTICK_RATE_HZ              1000
#define configUSE_PREEMPTION            1
#define configUSE_TIME_SLICING          1
#define configMAX_PRIORITIES            5
#define configMINIMAL_STACK_SIZE        128
#define configTOTAL_HEAP_SIZE           (5 * 1024)
#define configMAX_TASK_NAME_LEN         16
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1
#define configUSE_MUTEXES               1
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             1   /* used by busy_wait_ms() */
#define configUSE_TIMERS                0

/* configUSE_MUTEXES = 1 but priority inheritance is NOT active by default
 * in FreeRTOS unless configUSE_MUTEXES is 1 AND the mutex is created with
 * xSemaphoreCreateMutex() - which we do. However on this version of the
 * kernel, priority inheritance IS enabled by default when you use a mutex.
 * To demonstrate BARE inversion without any mitigation, this project uses
 * a BINARY semaphore instead of a mutex - see main.c. A binary semaphore
 * has no priority inheritance mechanism at all. */
#define configUSE_COUNTING_SEMAPHORES   0

#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#define configPRIO_BITS                 4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY        15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY   5
#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define vPortSVCHandler      SVC_Handler
#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
