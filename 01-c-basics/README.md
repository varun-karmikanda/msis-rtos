# C Basics

These exercises use standard C and can be compiled and run on Linux, WSL2,
macOS, or Windows with GCC.

## Prerequisites

### Ubuntu or WSL2

```bash
sudo apt update
sudo apt install -y build-essential gcc
```

Check the installation:

```bash
gcc --version
```

## Run an exercise

Start in the repository root and enter this folder:

```bash
cd 01-c-basics
```

Each exercise is independent. Change into its folder, compile the source file,
and run the generated program:

```bash
cd 01-addition
gcc -Wall -Wextra -std=c11 addition.c -o addition
./addition
cd ..
```

The remaining exercises use the same pattern:

```bash
cd 02-if
gcc -Wall -Wextra -std=c11 if-statement.c -o if-statement
./if-statement
cd ..

cd 03-if-else
gcc -Wall -Wextra -std=c11 if-else-statement.c -o if-else-statement
./if-else-statement
cd ..

cd 04-recursion
gcc -Wall -Wextra -std=c11 recursion.c -o recursion
./recursion
cd ..

cd 05-sum
gcc -Wall -Wextra -std=c11 sum.c -o sum
./sum
cd ..

cd 06-while-loop
gcc -Wall -Wextra -std=c11 while-loop.c -o while-loop
./while-loop
cd ..

cd 07-for-loop
gcc -Wall -Wextra -std=c11 for-loop.c -o for-loop
./for-loop
```

## Compile with debug information

Use `-g` when the program will be inspected with GDB:

```bash
gcc -Wall -Wextra -std=c11 -O0 -g addition.c -o addition
gdb ./addition
```

The existing [`COMMANDS.md`](./COMMANDS.md) contains ARM cross-compilation
and disassembly commands used in later exercises.
