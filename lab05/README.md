# fork() / exec() Example Programs

All programs compile with `gcc` on Linux. Build each program, then run it from a terminal.

---

## 1. fork_print.c

Basic `fork()` — parent and child print different text.

### Compile

    gcc -o fork_print fork_print.c

### Run

    ./fork_print

---


## 2. fork_waitpid.c

Parent waits specifically for its child's PID using `waitpid()` instead of `wait()`.

### Compile

    gcc -o fork_waitpid fork_waitpid.c

### Run

    ./fork_waitpid

---
