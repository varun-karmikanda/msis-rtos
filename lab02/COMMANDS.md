# COMMANDS

### Update the local package index
```
> sudo apt update
```

### Install the GNU Arm Embedded Toolchain and build essentials
```

> sudo apt update && sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch build-essential
```

### Compile C source code for an ARM Cortex-M3 microcontroller
```
> arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -g -c <source.c file> -o <target.o file>
```

### Disassemble the object file to inspect the assembly code alongside C source
```
> arm-none-eabi-objdump -S <source.o file>
```
