#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* This project runs from HSE + PLL at 72 MHz (see clock_setup_hse_pll_72mhz()
 * in main.c) instead of the default 8 MHz HSI every other project in this
 * workspace uses - this MUST match whatever SYSCLK actually ends up at,
 * since FreeRTOS derives the SysTick reload value from it directly. */
#define configCPU_CLOCK_HZ              72000000UL
#define configTICK_RATE_HZ              1000
#define configUSE_PREEMPTION            1
#define configUSE_TIME_SLICING          1
#define configMAX_PRIORITIES            5
#define configMINIMAL_STACK_SIZE        90
#define configTOTAL_HEAP_SIZE           (4 * 1024)   /* Bluepill has 20K RAM total */
#define configMAX_TASK_NAME_LEN         16
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1
#define configUSE_MUTEXES               1
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_IDLE_HOOK             1   /* -> vApplicationIdleHook() sleeps the CPU */
#define configUSE_TICK_HOOK             0
#define configUSE_TIMERS                0

#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskSuspend            1

/* Cortex-M3 NVIC priority config (ARM_CM3 port) */
#define configPRIO_BITS                 4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY        15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY   5
#define configKERNEL_INTERRUPT_PRIORITY \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Our own startup_stm32f103.s vector table uses the CMSIS-style names
 * below (weakly defined, defaulting to Default_Handler). Renaming
 * FreeRTOS's port handlers to these exact names is the standard,
 * documented way to wire the two together - it lets startup.s stay
 * completely generic across every project, RTOS or not. */
#define vPortSVCHandler      SVC_Handler
#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
