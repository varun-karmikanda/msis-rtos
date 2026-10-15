# STM32F103 Startup and Linking

This folder demonstrates how a C program is compiled for an ARM Cortex-M3,
combined with a startup assembly file, linked with an STM32F103 linker script,
and converted to a flashable binary.

## Prerequisites

Run these commands in Ubuntu or WSL2:

```bash
sudo apt update
sudo apt install -y build-essential gcc-arm-none-eabi \
  binutils-arm-none-eabi gdb-multiarch make stlink-tools
```

Verify the tools:

```bash
arm-none-eabi-gcc --version
arm-none-eabi-as --version
arm-none-eabi-ld --version
arm-none-eabi-objcopy --version
st-flash --version
```

Connect an ST-LINK debugger to the STM32F103 board before flashing. Do not
flash the board until the target voltage and wiring have been checked.

## Build the firmware step by step

Start from the repository root:

```bash
cd 02-stm32-startup
```

### 1. Compile the C source

```bash
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -g -c \
  addition.c -o addition.o
```

### 2. Assemble the startup file

```bash
arm-none-eabi-as -mcpu=cortex-m3 -mthumb \
  startup_stm32f103.s -o startup.o
```

### 3. Link the object files

```bash
arm-none-eabi-ld -T STM32F103C8.ld \
  addition.o startup.o -o firmware.elf
```

### 4. Create binary and Intel HEX files

```bash
arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
arm-none-eabi-objcopy -O ihex firmware.elf firmware.hex
```

### 5. Inspect the output

```bash
arm-none-eabi-size firmware.elf
arm-none-eabi-objdump -S firmware.elf > firmware-disassembly.txt
```

### 6. Flash the STM32

```bash
st-info --probe
st-flash write firmware.bin 0x08000000
```

## Debug with GDB and ST-LINK

In one terminal:

```bash
st-util
```

In another terminal:

```bash
arm-none-eabi-gdb firmware.elf
(gdb) target extended-remote :4242
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

The original command examples are also available in
[`commands.md`](./commands.md) and [`build-commands.md`](./build-commands.md).
