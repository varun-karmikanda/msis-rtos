# Real-Time Operating Systems (RTOS)

This repository contains the course exercises for C programming, ARM
startup code, STM32F103 bare-metal development, FreeRTOS, and Linux process
management.

## Repository sections

| Folder | Topics | Instructions |
|---|---|---|
| [`01-c-basics`](./01-c-basics/) | C variables, conditions, loops, recursion, and arithmetic | [`README.md`](./01-c-basics/README.md) |
| [`02-stm32-startup`](./02-stm32-startup/) | ARM compilation, startup assembly, linking, and flashing | [`README.md`](./02-stm32-startup/README.md) |
| [`03-stm32f103`](./03-stm32f103/) | STM32F103 bare-metal and FreeRTOS projects | [`README.md`](./03-stm32f103/README.md) |
| [`04-process-management`](./04-process-management/) | Linux `fork()`, `waitpid()`, and `exec()` | [`README.md`](./04-process-management/README.md) |

## System prerequisites

The Linux and STM32 exercises are easiest to run in Ubuntu or WSL2.

### Install common tools

```bash
sudo apt update
sudo apt install -y build-essential git
```

### Install ARM tools

```bash
sudo apt install -y gcc-arm-none-eabi binutils-arm-none-eabi \
  gdb-multiarch make
```

### Install STM32 flashing and debugging tools

```bash
sudo apt install -y stlink-tools openocd
```

Connect an ST-LINK debugger to the STM32F103 board before running `make flash`.
The board must be powered safely, and the target voltage must be visible to
the debugger.

## Start from the repository root

```bash
cd /path/to/rtos
```

Then follow the README in the section you want to run. Each section includes
its own prerequisites, commands, and expected workflow.