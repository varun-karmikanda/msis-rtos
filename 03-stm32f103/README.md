# STM32F103 Workspace

## Prerequisites

Use Ubuntu or WSL2 and install the ARM compiler, Make, Git, and ST-LINK
utilities:

```bash
sudo apt update
sudo apt install -y build-essential make git gcc-arm-none-eabi \
  binutils-arm-none-eabi gdb-multiarch stlink-tools openocd
```

Verify the installation:

```bash
arm-none-eabi-gcc --version
make --version
st-info --probe
```

Connect an ST-LINK debugger and an STM32F103 board before using `make flash`.

```
03-stm32f103/
├── FreeRTOS/                  <- you add this (git clone), source only
├── common/
│   ├── startup_stm32f103.s     <- vector table + Reset_Handler (self-written)
│   ├── stm32f103.ld            <- linker script (self-written)
│   ├── stm32f103_regs.h        <- minimal RCC/GPIO/SysTick register defs
│   └── common.mk                <- shared build rules
├── 01-led-blink/               <- bare-metal, no RTOS, no libraries
│   ├── Makefile
│   └── main.c
├── 02-three-thread-blink/      <- 3 FreeRTOS tasks blinking 3 LEDs
├── 03-three-thread-blink_hse/  <- HSE clock version
├── 04-priority-inversion/     <- priority inversion demonstration
├── 05-shared-led-mutex/       <- shared LED protected by a mutex
├── 06-shared-led-no-sync/     <- shared LED without synchronization
└── 07-three-thread-blink-priority/ <- task-priority demonstration
    ├── Makefile
    ├── main.c
    └── FreeRTOSConfig.h
```

The projects use `arm-none-eabi-gcc` and the shared build rules in
`common/common.mk`. The FreeRTOS projects build against a local clone of
FreeRTOS-Kernel.
(FreeRTOS's own `port.c` prints one harmless unused-variable warning
upstream, unrelated to anything here).

## One-time setup

```sh
cd 03-stm32f103
git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

Run the clone command only once. No libopencm3, CMSIS, or HAL installation is
required. The bare-metal `01-led-blink` project does not use FreeRTOS.

## Build and flash any project

Start from the repository root and enter one project folder:

```sh
cd 03-stm32f103/01-led-blink
make                     # build ELF, BIN, and HEX files
make size                # display firmware size
make flash               # flash the BIN at 0x08000000
make clean               # remove build files
```

The same commands work in every numbered project:

```sh
cd ../02-three-thread-blink
make
make flash
```

For the priority project:

```sh
cd ../07-three-thread-blink-priority
make
make flash
```

The complete project list is:

```text
01-led-blink
02-three-thread-blink
03-three-thread-blink_hse
04-priority-inversion
05-shared-led-mutex
06-shared-led-no-sync
07-three-thread-blink-priority
```

From `03-stm32f103`, build a specific project with:

```sh
cd 01-led-blink                    # change this to any folder above
make
make size
make flash
```

For example, to build the mutex demonstration:

```sh
cd 03-stm32f103/05-shared-led-mutex
make
make flash
make size
make clean
```

## Hardware for LED projects

- PC13 -> onboard LED (active-low, already on the board), 1 s period
- PA0  -> external LED + ~330R resistor to GND, 2 s period
- PA1  -> external LED + ~330R resistor to GND, 3 s period

No external LEDs on hand? Change `led_b`/`led_c` in `main.c` to also
point at `GPIOC, 13` and you'll see all three periods XOR-toggling the
one onboard LED instead - still three real independent tasks, just
sharing one visible output.

## Why no libraries

Every register access in both projects goes through
`common/stm32f103_regs.h`, which defines only the handful of
RCC/GPIO/SysTick registers these two projects actually touch, with
addresses and bit positions taken directly from RM0008 and the ARMv7-M
reference manual - no libopencm3, no ST HAL, no CMSIS device headers.
`startup_stm32f103.s` and `stm32f103.ld` are equally self-written, not
vendor-generated.

The only "library" in the whole workspace is FreeRTOS itself, for
`three-thread-blink` - and even there, `common/startup_stm32f103.s`
has no knowledge FreeRTOS exists. The three vector-table entries it
needs (`SVC_Handler`, `PendSV_Handler`, `SysTick_Handler`) are ordinary
weak symbols pointing at a default handler; FreeRTOS's port layer
supplies strong definitions under those exact names via three `#define`
lines in `FreeRTOSConfig.h`, which is FreeRTOS's own documented way of
bridging to a non-CMSIS startup file. `led-blink` never links any of
that in, so it stays a pure baremetal binary.

## Adding a new project

```sh
mkdir my-project
cd my-project
# write main.c
cat > Makefile <<'EOF'
ROOT_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
PROJECT  := my-project
SRCS := main.c
include $(ROOT_DIR)/common/common.mk
EOF
make
```

Add `EXTRA_CFLAGS`/extra `SRCS` lines in that Makefile (following
`02-three-thread-blink/Makefile`) if the project also needs FreeRTOS.
