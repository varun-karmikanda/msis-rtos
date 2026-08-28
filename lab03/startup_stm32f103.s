.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global _estack

/* ---------------- Vector Table ---------------- */
.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object
g_pfnVectors:
    .word _estack             /* Initial stack pointer */
    .word Reset_Handler       /* Reset handler */
    .word Default_Handler     /* NMI */
    .word Default_Handler     /* HardFault */
    .word Default_Handler     /* MemManage */
    .word Default_Handler     /* BusFault */
    .word Default_Handler     /* UsageFault */
    .word 0
    .word 0
    .word 0
    .word 0
    .word Default_Handler     /* SVCall */
    .word Default_Handler     /* DebugMon */
    .word 0
    .word Default_Handler     /* PendSV */
    .word Default_Handler     /* SysTick */
    /* Peripheral IRQs omitted here — add STM32F103 IRQ entries
       (WWDG, PVD, TAMPER, RTC, FLASH, RCC, EXTI0..., etc.)
       as needed for your application. */

.size g_pfnVectors, .-g_pfnVectors

/* ---------------- Reset Handler ---------------- */
.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Copy .data section from FLASH to RAM */
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
copy_data_loop:
    cmp r1, r2
    bge copy_data_done
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data_loop
copy_data_done:

    /* Zero-fill .bss section */
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0
zero_bss_loop:
    cmp r1, r2
    bge zero_bss_done
    str r3, [r1], #4
    b zero_bss_loop
zero_bss_done:

    /* Call main */
    bl main
    b .

.size Reset_Handler, .-Reset_Handler

/* ---------------- Default Handler ---------------- */
.section .text.Default_Handler, "ax", %progbits
Default_Handler:
    b .
.size Default_Handler, .-Default_Handler
