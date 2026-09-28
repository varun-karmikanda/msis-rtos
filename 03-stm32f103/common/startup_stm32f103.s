/*
 * startup_stm32f103.s
 *
 * Self-written startup file for STM32F103C8T6 (Cortex-M3):
 *   - the interrupt vector table, placed at the start of flash
 *   - Reset_Handler: copies .data from flash to RAM, zeros .bss, calls main()
 *   - weak default handlers for every exception/interrupt, so a project
 *     that never uses a given interrupt still links cleanly
 *
 * This file is identical for every project in this workspace, RTOS or
 * not. FreeRTOS's ARM_CM3 port supplies its own strong definitions of
 * SVC_Handler / PendSV_Handler / SysTick_Handler (via the renaming in
 * FreeRTOSConfig.h) which simply override the weak defaults below at
 * link time — nothing here has to know FreeRTOS exists.
 */
    .syntax unified
    .cpu cortex-m3
    .thumb

/* ------------------------------------------------------------------ */
/* Vector table                                                        */
/* ------------------------------------------------------------------ */
    .section .isr_vector, "a", %progbits
    .align 2
    .global g_pfnVectorTable
g_pfnVectorTable:
    .word _estack                  /* initial stack pointer            */
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0                        /* reserved                          */
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0                        /* reserved                          */
    .word PendSV_Handler
    .word SysTick_Handler
    /* --- STM32F103 (medium-density) peripheral interrupts, RM0008 Table 63 --- */
    .word WWDG_IRQHandler           /* 0  */
    .word PVD_IRQHandler            /* 1  */
    .word TAMPER_IRQHandler         /* 2  */
    .word RTC_IRQHandler            /* 3  */
    .word FLASH_IRQHandler          /* 4  */
    .word RCC_IRQHandler            /* 5  */
    .word EXTI0_IRQHandler          /* 6  */
    .word EXTI1_IRQHandler          /* 7  */
    .word EXTI2_IRQHandler          /* 8  */
    .word EXTI3_IRQHandler          /* 9  */
    .word EXTI4_IRQHandler          /* 10 */
    .word DMA1_Channel1_IRQHandler  /* 11 */
    .word DMA1_Channel2_IRQHandler  /* 12 */
    .word DMA1_Channel3_IRQHandler  /* 13 */
    .word DMA1_Channel4_IRQHandler  /* 14 */
    .word DMA1_Channel5_IRQHandler  /* 15 */
    .word DMA1_Channel6_IRQHandler  /* 16 */
    .word DMA1_Channel7_IRQHandler  /* 17 */
    .word ADC1_2_IRQHandler         /* 18 */
    .word USB_HP_CAN1_TX_IRQHandler /* 19 */
    .word USB_LP_CAN1_RX0_IRQHandler/* 20 */
    .word CAN1_RX1_IRQHandler       /* 21 */
    .word CAN1_SCE_IRQHandler       /* 22 */
    .word EXTI9_5_IRQHandler        /* 23 */
    .word TIM1_BRK_IRQHandler       /* 24 */
    .word TIM1_UP_IRQHandler        /* 25 */
    .word TIM1_TRG_COM_IRQHandler   /* 26 */
    .word TIM1_CC_IRQHandler        /* 27 */
    .word TIM2_IRQHandler           /* 28 */
    .word TIM3_IRQHandler           /* 29 */
    .word TIM4_IRQHandler           /* 30 */
    .word I2C1_EV_IRQHandler        /* 31 */
    .word I2C1_ER_IRQHandler        /* 32 */
    .word I2C2_EV_IRQHandler        /* 33 */
    .word I2C2_ER_IRQHandler        /* 34 */
    .word SPI1_IRQHandler           /* 35 */
    .word SPI2_IRQHandler           /* 36 */
    .word USART1_IRQHandler         /* 37 */
    .word USART2_IRQHandler         /* 38 */
    .word USART3_IRQHandler         /* 39 */
    .word EXTI15_10_IRQHandler      /* 40 */
    .word RTCAlarm_IRQHandler       /* 41 */
    .word USBWakeUp_IRQHandler      /* 42 */
    /* extend here (see RM0008 Table 63) if you need TIM5+/USART3+/etc. */

/* ------------------------------------------------------------------ */
/* Reset_Handler — the very first code that runs                       */
/* ------------------------------------------------------------------ */
    .section .text.Reset_Handler
    .thumb_func
    .global Reset_Handler
Reset_Handler:
    /* copy .data initial values out of flash into RAM */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
copy_data_loop:
    cmp r1, r2
    bcs copy_data_done
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data_loop
copy_data_done:

    /* zero-fill .bss */
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0
zero_bss_loop:
    cmp r1, r2
    bcs zero_bss_done
    str r3, [r1], #4
    b zero_bss_loop
zero_bss_done:

    bl main
loop_forever:
    b loop_forever
    .size Reset_Handler, . - Reset_Handler

/* ------------------------------------------------------------------ */
/* Default handler + weak aliases                                      */
/* ------------------------------------------------------------------ */
    .section .text.Default_Handler, "ax", %progbits
    .thumb_func
Default_Handler:
    b .
    .size Default_Handler, . - Default_Handler

    .macro def_weak name
    .weak \name
    .thumb_set \name, Default_Handler
    .endm

    def_weak NMI_Handler
    def_weak HardFault_Handler
    def_weak MemManage_Handler
    def_weak BusFault_Handler
    def_weak UsageFault_Handler
    def_weak SVC_Handler
    def_weak DebugMon_Handler
    def_weak PendSV_Handler
    def_weak SysTick_Handler
    def_weak WWDG_IRQHandler
    def_weak PVD_IRQHandler
    def_weak TAMPER_IRQHandler
    def_weak RTC_IRQHandler
    def_weak FLASH_IRQHandler
    def_weak RCC_IRQHandler
    def_weak EXTI0_IRQHandler
    def_weak EXTI1_IRQHandler
    def_weak EXTI2_IRQHandler
    def_weak EXTI3_IRQHandler
    def_weak EXTI4_IRQHandler
    def_weak DMA1_Channel1_IRQHandler
    def_weak DMA1_Channel2_IRQHandler
    def_weak DMA1_Channel3_IRQHandler
    def_weak DMA1_Channel4_IRQHandler
    def_weak DMA1_Channel5_IRQHandler
    def_weak DMA1_Channel6_IRQHandler
    def_weak DMA1_Channel7_IRQHandler
    def_weak ADC1_2_IRQHandler
    def_weak USB_HP_CAN1_TX_IRQHandler
    def_weak USB_LP_CAN1_RX0_IRQHandler
    def_weak CAN1_RX1_IRQHandler
    def_weak CAN1_SCE_IRQHandler
    def_weak EXTI9_5_IRQHandler
    def_weak TIM1_BRK_IRQHandler
    def_weak TIM1_UP_IRQHandler
    def_weak TIM1_TRG_COM_IRQHandler
    def_weak TIM1_CC_IRQHandler
    def_weak TIM2_IRQHandler
    def_weak TIM3_IRQHandler
    def_weak TIM4_IRQHandler
    def_weak I2C1_EV_IRQHandler
    def_weak I2C1_ER_IRQHandler
    def_weak I2C2_EV_IRQHandler
    def_weak I2C2_ER_IRQHandler
    def_weak SPI1_IRQHandler
    def_weak SPI2_IRQHandler
    def_weak USART1_IRQHandler
    def_weak USART2_IRQHandler
    def_weak USART3_IRQHandler
    def_weak EXTI15_10_IRQHandler
    def_weak RTCAlarm_IRQHandler
    def_weak USBWakeUp_IRQHandler
