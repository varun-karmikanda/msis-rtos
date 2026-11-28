# Linux Process Management – Commands & Notes

## 1. Compiling C Programs

Use `gcc` to compile C source files into executable programs.

```bash
gcc hello.c -o hello
gcc pwd.c -o pwd
gcc main.c -o main
```

**Syntax:**
```bash
gcc source.c -o executable
```

- `gcc`: C compiler.
- `source.c`: C source file.
- `-o`: Specifies the output executable name.
- `hello`, `pwd`, `main`: Executable files.

## 2. Running Programs

### Foreground execution

```bash
./main
```

- Executes the program in the foreground.
- Terminal waits until the program finishes.
- You cannot enter another command in that terminal while it is running.

### Background execution

```bash
./main &
```

- `&` runs the program in the background.
- Terminal becomes available immediately.
- Useful when processes contain `sleep(120)`.

**Important:** `&` is a shell operator, not a C function.

## 3. Getting Process IDs

### Get PID of the last background process

```bash
echo $!
```

- `$!` stores the PID of the most recently started background process in the current shell.
- Useful after running `./main &`.

Example:

```bash
./main &
echo $!
```

### Other useful shell variables

```bash
echo $$
```
Displays the PID of the current shell.

```bash
echo $?
```
Displays the exit status of the last command.

| Variable | Meaning |
|---|---|
| `$$` | Current shell PID |
| `$!` | Last background process PID |
| `$?` | Exit status of last command |

## 4. Viewing Processes

### ps -ef

```bash
ps -ef
```

Displays all processes with detailed information.

| Column | Meaning |
|---|---|
| UID | User who owns the process |
| PID | Process ID |
| PPID | Parent Process ID |
| C | CPU utilization indicator |
| STIME | Process start time |
| TTY | Terminal associated with process |
| TIME | CPU time used |
| CMD | Command being executed |

### ps -ef --forest

```bash
ps -ef --forest
```

Displays all processes in a tree-like hierarchy.

Options:

- `-e`: Display all processes.
- `-f`: Full-format listing.
- `--forest`: Display parent-child relationships using indentation.

Useful for tracing process creation.

### pstree -p

```bash
pstree -p
```

Displays the process hierarchy with PIDs.

### pstree -p PID

```bash
pstree -p 59583
```

Displays the process tree starting from the specified PID.

### pstree -p $!

```bash
./main &
pstree -p $!
```

Automatically displays the process tree of the last background process.

## 5. Managing Background Jobs

### jobs

```bash
jobs
```

Lists background and suspended jobs started from the current shell.

### fg

```bash
fg
```

Brings the most recent background or suspended job into the foreground.

### bg

```bash
bg
```

Resumes a suspended job in the background.

### kill

```bash
kill PID
```

Sends the default termination signal (SIGTERM) to the specified process.

Example:

```bash
kill 59583
```

To forcefully terminate a process:

```bash
kill -9 59583
```

**Note:** Killing a parent does not automatically guarantee that all its children are terminated.

## 6. Important C Process Management Functions

### fork()

```c
pid_t pid = fork();
```

Creates a new child process.

Return values:

- `0`: Executed by the child.
- Positive value: Executed by the parent; value is the child's PID.
- `-1`: Process creation failed.

### exec()

Example:

```c
execl("./hello", "hello", NULL);
```

Replaces the current process image with another executable.

Important:
- Does not create a new process.
- PID remains unchanged.
- Code after successful `exec()` does not execute.

### getpid()

```c
getpid();
```

Returns the PID of the current process.

### getppid()

```c
getppid();
```

Returns the PID of the parent process.

### wait()

```c
wait(NULL);
```

Makes the parent wait for one of its child processes to terminate.

Header:

```c
#include <sys/wait.h>
```

### sleep()

```c
sleep(120);
```

Suspends the current process for 120 seconds (2 minutes).

Header:

```c
#include <unistd.h>
```

### exit()

```c
exit(0);
```

Terminates the current process.

- `0`: Successful termination.
- Non-zero: Indicates an error or abnormal termination.

Header:

```c
#include <stdlib.h>
```

## 7. Output Buffering

### setbuf()

```c
setbuf(stdout, NULL);
```

Disables buffering for standard output.

Useful in fork programs to avoid duplicated output caused by inherited, unflushed output buffers.

### fflush()

```c
fflush(stdout);
```

Explicitly flushes the output buffer.

Unlike `setbuf(stdout, NULL)`, this does not disable buffering.

## 8. Getting the Current Working Directory

### getcwd()

```c
#include <unistd.h>
#include <limits.h>

char cwd[PATH_MAX];

getcwd(cwd, sizeof(cwd));

printf("%s\n", cwd);
```

- `getcwd()`: Gets the current working directory.
- `cwd`: Buffer that stores the directory path.
- `PATH_MAX`: System-defined pathname limit where available.
- `sizeof(cwd)`: Size of the allocated buffer.

Recommended error handling:

```c
if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd failed");
    return 1;
}
```

## 9. Complete Workflow for Tracing a Process Tree

Compile the programs:

```bash
gcc hello.c -o hello
gcc pwd.c -o pwd
gcc main.c -o main
```

Run the main program in the background:

```bash
./main &
```

Get the parent PID:

```bash
echo $!
```

Trace the family tree:

```bash
pstree -p $!
```

View all processes:

```bash
ps -ef --forest
```

Inspect specific processes:

```bash
ps -o pid,ppid,stat,comm -p PID
```

## 10. Quick Revision Table

| Command / Function | Purpose |
|---|---|
| `gcc` | Compile C programs |
| `./main` | Run program in foreground |
| `./main &` | Run program in background |
| `echo $!` | Get last background process PID |
| `echo $$` | Get current shell PID |
| `echo $?` | Get last command's exit status |
| `ps -ef` | Display all processes |
| `ps -ef --forest` | Display process hierarchy |
| `pstree -p` | Display process tree with PIDs |
| `jobs` | List shell background jobs |
| `fg` | Bring job to foreground |
| `bg` | Resume job in background |
| `kill PID` | Send termination signal |
| `fork()` | Create child process |
| `exec()` | Replace process image |
| `getpid()` | Get current PID |
| `getppid()` | Get parent PID |
| `wait()` | Wait for child termination |
| `sleep()` | Suspend process |
| `exit()` | Terminate process |
| `getcwd()` | Get current directory |
| `setbuf()` | Configure stream buffering |
| `fflush()` | Flush output buffer |

---

**Key exam concepts:**

1. `fork()` creates a process; `exec()` replaces its program.
2. Every process has a PID and a PPID.
3. A child inherits its parent's executable when created using `fork()`.
4. Successful `exec()` does not change the PID.
5. `wait()` prevents a parent from finishing before the child it waits for.
6. `sleep()` suspends only the calling process.
7. `&` is a shell operator for background execution.
8. `pstree -p` is particularly useful for visualizing process family trees.