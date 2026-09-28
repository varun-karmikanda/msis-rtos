# stm32f103 workspace

```
stm32f103/
├── FreeRTOS/                  <- you add this (git clone), source only
├── common/
│   ├── startup_stm32f103.s     <- vector table + Reset_Handler (self-written)
│   ├── stm32f103.ld            <- linker script (self-written)
│   ├── stm32f103_regs.h        <- minimal RCC/GPIO/SysTick register defs
│   └── common.mk                <- shared build rules
├── led-blink/                  <- baremetal, no RTOS, no libraries
│   ├── Makefile
│   └── main.c
└── three-thread-blink/         <- 3 FreeRTOS tasks blinking 3 LEDs
    ├── Makefile
    ├── main.c
    └── FreeRTOSConfig.h
```

Verified: both projects were built end-to-end with `arm-none-eabi-gcc
13.2.1` during preparation of this workspace - `led-blink` compiles and
links with zero warnings, and `three-thread-blink` builds against a real
clone of FreeRTOS-Kernel with zero warnings in any of our own code
(FreeRTOS's own `port.c` prints one harmless unused-variable warning
upstream, unrelated to anything here).

## One-time setup

```sh
cd stm32f103
git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

That's it - no libopencm3, no CMSIS, no HAL to install. `led-blink`
doesn't even need that step.

## Building

```sh
cd led-blink            # or three-thread-blink
make                     # -> build/<project>.elf/.bin/.hex
make flash               # st-flash write build/<project>.bin 0x8000000
make size
make clean
```

## Hardware for three-thread-blink

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
mkdir stm32f103/my-project
cd stm32f103/my-project
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
`three-thread-blink/Makefile`) if the project also needs FreeRTOS.
