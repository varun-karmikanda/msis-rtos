Here is the complete README.md source you can copy directly:

# STM32 Development, Flashing, and Debugging on Ubuntu

This document describes how to:

1. Compile an STM32 program with debug information.
2. Link the program into an ELF executable.
3. Convert the ELF into a binary (`.bin`) image.
4. Flash the `.bin` image to an STM32 using **ST-Link V2**.
5. Connect GDB to the running STM32 through `st-util`.
6. Debug the firmware using the **ELF file**, which contains the debug symbols and source-level information.

---

## 1. Required Tools

The following tools are used:

- `arm-none-eabi-gcc` — ARM cross-compiler
- `arm-none-eabi-as` — ARM assembler
- `arm-none-eabi-ld` — linker
- `arm-none-eabi-objcopy` — converts ELF into binary
- `st-flash` — flashes the binary to the STM32
- `st-util` — provides a GDB server interface through ST-Link
- `gdb-multiarch` — debugger

The basic build flow is:

```text
add_prog.c
    |
    | arm-none-eabi-gcc
    v
add_prog.o
    |
    | arm-none-eabi-ld + linker script
    | + startup.o
    v
firmware.elf
    |
    | arm-none-eabi-objcopy
    v
firmware.bin
    |
    | st-flash
    v
STM32 Flash Memory
```

The debugging flow is:

```text
firmware.elf
     |
     | Debug information + symbols
     v
    GDB
     |
     | GDB remote protocol
     v
   st-util
     |
     | ST-Link V2
     v
    STM32
```

---

# 2. Compile the C Program

Compile the C source file:

```bash
arm-none-eabi-gcc \
    -mcpu=cortex-m3 \
    -mthumb \
    -O0 \
    -g \
    -c add_prog.c \
    -o add_prog.o
```

### Compiler Options

| Option | Description |
|---|---|
| `-mcpu=cortex-m3` | Target Cortex-M3 |
| `-mthumb` | Generate Thumb instructions |
| `-O0` | Disable optimization |
| `-g` | Generate debug information |
| `-c` | Compile without linking |
| `-o add_prog.o` | Output object file |

The `-g` option is important because it generates debugging information that will be stored in the ELF file.

Using `-O0` is useful during debugging because the generated instructions map more directly to the source code.

---

# 3. Assemble the Startup Code

Assemble the STM32 startup file:

```bash
arm-none-eabi-as \
    -mcpu=cortex-m3 \
    -mthumb \
    startup_stm32f103.s \
    -o startup.o
```

This produces:

```text
startup.o
```

The startup code normally contains:

- Interrupt/vector table
- Initial stack pointer
- `Reset_Handler`
- Exception handlers
- Startup initialization
- Entry into the C program

The vector table is normally located at the beginning of flash:

```text
0x08000000
    |
    +-- Initial Stack Pointer
    |
    +-- Reset_Handler
    |
    +-- NMI_Handler
    |
    +-- HardFault_Handler
    |
    +-- ...
```

---

# 4. Link the Program

Link the object files using the STM32 linker script:

```bash
arm-none-eabi-ld \
    -T STM32F103C8.ld \
    add_prog.o \
    startup.o \
    -o firmware.elf
```

The linker script:

```text
STM32F103C8.ld
```

defines where the program is placed in STM32 memory.

Typical STM32F103 memory locations are:

```text
Flash: 0x08000000
RAM:   0x20000000
```

The linker script determines the locations of sections such as:

```text
.text
.rodata
.data
.bss
```

The result is:

```text
firmware.elf
```

---

# 5. ELF vs BIN

This is an important distinction.

The ELF file contains:

- Machine code
- Memory addresses
- Function symbols
- Variable symbols
- Section information
- Debug information

The binary file contains the raw firmware image that is written to flash.

Therefore:

```text
firmware.elf
    -> Used by GDB for debugging

firmware.bin
    -> Used for flashing the STM32
```

The `.bin` does not normally contain the debug information required by GDB.

For example, GDB can use the ELF to determine that an address corresponds to:

```c
int add(int a, int b)
```

and map machine instructions back to source code.

---

# 6. Generate the Binary

Convert the ELF into a raw binary:

```bash
arm-none-eabi-objcopy \
    -O binary \
    firmware.elf \
    firmware.bin
```

You should now have:

```text
firmware.elf
firmware.bin
```

The two files originate from the same firmware build.

```text
firmware.elf
     |
     | objcopy
     v
firmware.bin
```

---

# 7. Flash the STM32

Connect the STM32 to the computer using **ST-Link V2**.

Flash the binary:

```bash
st-flash write firmware.bin 0x08000000
```

Here:

```text
firmware.bin
```

is the firmware image.

And:

```text
0x08000000
```

is the start address of STM32 flash memory.

The `.bin` is written directly into the STM32 flash.

---

# 8. Start ST-Util

Open another terminal and run:

```bash
st-util
```

`st-util` communicates with the STM32 through ST-Link and provides a GDB remote debugging server.

The default GDB port is:

```text
4242
```

The connection is:

```text
GDB
 |
 | TCP :4242
 v
st-util
 |
 | ST-Link
 v
STM32
```

---

# 9. Start GDB

Open another terminal:

```bash
gdb-multiarch firmware.elf
```

Notice that GDB is given:

```text
firmware.elf
```

and **not**:

```text
firmware.bin
```

This is because GDB needs the symbols and debug information stored in the ELF.

The complete debugging connection is:

```text
             firmware.elf
                  |
                  | Symbols
                  | Debug information
                  v
                GDB
                  |
                  | Remote GDB protocol
                  | TCP :4242
                  v
               st-util
                  |
                  | ST-Link
                  v
                STM32
```

The STM32 executes the firmware that was flashed from:

```text
firmware.bin
```

while GDB uses:

```text
firmware.elf
```

to understand the program.

**The ELF and BIN must come from the same build.**

---

# 10. Connect GDB to ST-Link

Inside GDB:

```gdb
(gdb) target extended-remote :4242
```

This connects GDB to `st-util`.

You should now have:

```text
GDB
 |
 | :4242
 v
st-util
 |
 v
ST-Link V2
 |
 v
STM32
```

---

# 11. Reset and Halt the STM32

Reset the STM32 and halt the CPU:

```gdb
(gdb) monitor reset-halt
```

Set a breakpoint at the reset handler:

```gdb
(gdb) break Reset_Handler
```

Then continue:

```gdb
(gdb) continue
```

or:

```gdb
(gdb) c
```

GDB should stop when `Reset_Handler` is reached.

---

# 12. Single-Step Instructions

To execute exactly one CPU instruction:

```gdb
(gdb) stepi
```

or:

```gdb
(gdb) si
```

For example:

```gdb
(gdb) stepi
```

executes one instruction and stops again.

After executing `stepi`, simply press:

```text
Enter
```

GDB will repeat the previous command.

Therefore:

```gdb
(gdb) stepi
(gdb) <Enter>
(gdb) <Enter>
(gdb) <Enter>
```

will execute one instruction at a time.

---

# 13. Display the Current Instruction

Display the instruction at the current program counter:

```gdb
(gdb) display/i $pc
```

Example:

```text
1: x/i $pc
=> 0x08000124 <Reset_Handler+4>: movs r0, #0
```

The `=>` indicates the current instruction.

---

# 14. Display the Stack Pointer

Display the Stack Pointer:

```gdb
(gdb) display/x $sp
```

Example:

```text
2: /x $sp = 0x20005000
```

The `$sp` register contains the current Stack Pointer.

The stack normally resides in SRAM.

---

# 15. Display Registers

Display register `r7`:

```gdb
(gdb) display/x $r7
```

Display the Link Register:

```gdb
(gdb) display/x $lr
```

Display the Program Counter:

```gdb
(gdb) display/x $pc
```

Display all registers:

```gdb
(gdb) info registers
```

Important Cortex-M registers include:

```text
r0-r12   General-purpose registers
sp       Stack Pointer
lr       Link Register
pc       Program Counter
xPSR     Program Status Register
```

---

# 16. Examine Stack Memory

To examine memory starting at the current Stack Pointer:

```gdb
(gdb) x/8xw $sp
```

Meaning:

```text
x       Examine memory
8       8 units
x       Display in hexadecimal
w       Word-sized units
```

Therefore:

```text
8xw = 8 words = 32 bytes
```

It is **not** 8 bytes.

For example:

```gdb
(gdb) x/8xw $sp
```

might produce:

```text
0x20004fc0: 0x00000000  0x20005000  0x08000124  0x00000000
0x20004fd0: 0x00000001  0x00000002  0x00000003  0x00000004
```

If the stack pointer is close to the end of RAM, examining 32 bytes may cross outside valid RAM.

In that case, examine fewer words:

```gdb
(gdb) x/4xw $sp
```

or:

```gdb
(gdb) x/1xw $sp
```

---

# 17. Automatically Display Stack Memory

If you want the memory contents to be displayed after every stop:

```gdb
(gdb) display/8xw $sp
```

Unlike:

```gdb
x/8xw $sp
```

which performs a one-time examination, `display` automatically repeats the expression whenever the program stops.

---

# 18. Automatically Disassemble the Next Instruction

Enable automatic disassembly:

```gdb
(gdb) set disassemble-next-line on
```

Then execute:

```gdb
(gdb) stepi
```

GDB will show the next instruction after each step.

This is useful when learning how the C program is translated into ARM Thumb assembly.

---

# 19. Useful GDB Commands

### Show all registers

```gdb
info registers
```

### Show the current source location

```gdb
list
```

### Show the current instruction

```gdb
x/i $pc
```

### Disassemble the current function

```gdb
disassemble
```

### Disassemble `Reset_Handler`

```gdb
disassemble Reset_Handler
```

### Set a breakpoint

```gdb
break main
```

or:

```gdb
break Reset_Handler
```

### List breakpoints

```gdb
info breakpoints
```

### Continue execution

```gdb
continue
```

or:

```gdb
c
```

### Execute one instruction

```gdb
stepi
```

or:

```gdb
si
```

### Execute one source line

```gdb
next
```

or:

```gdb
n
```

### Delete a breakpoint

```gdb
delete <breakpoint-number>
```

---

# 20. Complete Workflow

The complete process is:

## Terminal 1 — Compile

```bash
arm-none-eabi-gcc \
    -mcpu=cortex-m3 \
    -mthumb \
    -O0 \
    -g \
    -c add_prog.c \
    -o add_prog.o
```

## Terminal 1 — Assemble

```bash
arm-none-eabi-as \
    -mcpu=cortex-m3 \
    -mthumb \
    startup_stm32f103.s \
    -o startup.o
```

## Terminal 1 — Link

```bash
arm-none-eabi-ld \
    -T STM32F103C8.ld \
    add_prog.o \
    startup.o \
    -o firmware.elf
```

## Terminal 1 — Generate BIN

```bash
arm-none-eabi-objcopy \
    -O binary \
    firmware.elf \
    firmware.bin
```

## Terminal 1 — Flash

```bash
st-flash write firmware.bin 0x08000000
```

---

## Terminal 2 — Start GDB Server

```bash
st-util
```

The server listens on:

```text
localhost:4242
```

---

## Terminal 3 — Start GDB

```bash
gdb-multiarch firmware.elf
```

Then:

```gdb
(gdb) target extended-remote :4242
(gdb) monitor reset-halt

(gdb) break Reset_Handler
(gdb) continue
```

Now configure useful displays:

```gdb
(gdb) display/i $pc
(gdb) display/x $sp
(gdb) display/x $r7
(gdb) display/x $lr
```

Enable automatic disassembly:

```gdb
(gdb) set disassemble-next-line on
```

Single-step:

```gdb
(gdb) stepi
```

Press **Enter** repeatedly to execute the next instruction.

---

# 21. Final Concept

The most important concept is:

```text
                  BUILD
                    |
                    v
             +-------------+
             | firmware.elf|
             +-------------+
               |         |
               |         |
        objcopy|         |GDB
               |         |
               v         v
       +-------------+  +------+
       | firmware.bin|  | GDB  |
       +-------------+  +------+
               |          |
          st-flash         |
               |          |
               v          |
          +---------+     |
          |  STM32  |<----+
          +---------+  ST-Link
```

### `.bin`

Used to **program the STM32**:

```bash
st-flash write firmware.bin 0x08000000
```

### `.elf`

Used to **debug the program**:

```bash
gdb-multiarch firmware.elf
```

The `.elf` contains the debug information that allows GDB to map STM32 memory addresses back to functions, variables, source code, and assembly instructions.

Therefore:

> **Flash the `.bin`, debug using the `.elf`.**

Always make sure that the `.bin` and `.elf` were generated from the **same build**.