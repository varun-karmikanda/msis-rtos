# COMMANDS

### 
```
> arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -g -c add_prog.c -o add_prog.o
```

### 
```
> arm-none-eabi-as -mcpu=cortex-m3 -mthumb startup_stm32f103.s -o startup.o
```

### 
```
> arm-none-eabi-ld -T STM32F103C8.ld add_prog.o startup.o -o firmware.elf
```

###  
```
> arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
```

### 
```
> st-flash write firmware.bin 0x08000000
```

```
> st-util // port at 4242 for debugger link
```
bridge from stlink v2 to gdb

in another terminal
```
> gdb-multiarch firmware.elf
```
We are running this as the .bin doesnt have the debug info
to give the gdb we are adding the firmware.elf


From gdb to st-link v2
```
(gdb) target extended-remote :4242

(gdb) break Reset_Handler
(gdb) monitor reset-halt
(gdb) stepi

(gdb) display/i $pc

(gdb) display $sp

(gdb) display/x $r7

(gdb) display/8xw $sp   // No access as the 8 bytes is out of RAM

(gdb) display/x $lr

(gdb) set disassemble-next-line on

(gdb) stepi     // 1 assembly instruction at a time
```
// Just clicking enter run prev code