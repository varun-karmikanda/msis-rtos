# `fork()` / `exec()` Example Programs

These programs demonstrate process creation and process replacement on Linux.
All programs can be compiled with `gcc`.

## Prerequisites

Install GCC and the Linux process-inspection utilities:

```bash
sudo apt update
sudo apt install -y build-essential procps psmisc
```

Verify the installation:

```bash
gcc --version
pstree --version
pgrep --version
```

Start from the repository root:

```bash
cd 04-process-management
```

Every exercise is independent. Enter its folder before compiling and running
it. The commands below are complete, step-by-step examples.

## 1. `01-basic-fork/basic_fork.c`

Demonstrates basic `fork()` usage. The parent and child processes print
different messages.

```bash
cd 01-basic-fork
gcc -o basic_fork basic_fork.c
./basic_fork
```

## 2. `02-waitpid/fork_waitpid.c`

The parent waits specifically for its child using `waitpid()` instead of
`wait()`.

```bash
cd 02-waitpid
gcc -o fork_waitpid fork_waitpid.c
./fork_waitpid
```

## 3. `03-fork-with-sleep/fork_sleep.c`

The child sleeps for 20 seconds before exiting. The parent blocks in
`waitpid()` during this time.

```bash
cd 03-fork-with-sleep
gcc -o fork_sleep fork_sleep.c
./fork_sleep
```

## 4. `04-multiple-children/fork_three_children.c`

The parent creates three children. The children sleep for 10, 20, and
30 seconds respectively, and the parent waits for all of them in order.

```bash
cd 04-multiple-children
gcc -o fork_three_children fork_three_children.c
./fork_three_children
```

## 5. `05-recursive-fork/recursive_fork.c`

Demonstrates a four-level process chain:

```text
original parent -> child1 -> child2 -> child3
```

Each process creates the next process in the chain and waits for it. Every
process also sleeps for 10 seconds after its child exits, so the total runtime
is approximately 30 seconds.

```bash
cd 05-recursive-fork
gcc -o recursive_fork recursive_fork.c
./recursive_fork
```

While the program is running, inspect the process ancestry from another
terminal:

```bash
pstree -p $(pgrep -f recursive_fork | head -1)
```

## 6. `06-fork-and-exec/fork_exec.c` and `exec_program.c`

`fork_exec.c` creates a child process. The child then uses `exec()` to run
`exec_prog`, which counts from 1 to 10, printing one number per second.

Build both programs from the same directory:

```bash
cd 06-fork-and-exec
gcc -o exec_prog exec_program.c
gcc -o fork_exec fork_exec.c
./fork_exec
```

While the program is running, inspect the parent-child relationship and
confirm that the child PID remains the same after `exec()`:

```bash
ps -o pid,ppid,cmd -C exec_prog
pstree -p $(pgrep fork_exec)
cat /proc/$(pgrep exec_prog)/status | grep -E 'Pid|PPid'
```

## Notes

- `fork_exec.c` and `exec_program.c` must be built and run from the
  `06-fork-and-exec` directory. The source file is named `exec_program.c`,
  but it is compiled as `exec_prog` because `fork_exec.c` executes
  `./exec_prog` using a relative path.
- All other programs are standalone and can be built and run independently.
