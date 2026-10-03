# Introduction to OS

Part 3: Multithreaded programming. Audience: M.Eng. embedded / systems and computer science.

## 1. Introduction

A **thread** is the basic unit of CPU utilisation. It has its own **thread ID, program counter, register set and stack**. Threads of one process share the **code, data (globals), heap and open files**, so they communicate through memory without kernel-mediated IPC. A process with one thread is single-threaded; with several, multithreaded.

Figure 1. Green = shared by all threads; blue = private per thread.

### Benefits

| Benefit | Why |
| --- | --- |
| Responsiveness | A UI thread keeps responding while another thread does a long operation or blocks on I/O |
| Resource sharing | Threads share memory and files by default; no shared-memory or message set-up |
| Economy | Creating a thread and switching between threads of one process is cheaper than for processes: no new address space, no page-table or TLB change on a switch |
| Scalability | Threads can run in parallel on different cores; a single-threaded process uses one core |

### Concurrency, parallelism and Amdahl's law

**Concurrency**: several tasks make progress (possibly by interleaving on one core). **Parallelism**: tasks execute simultaneously on several cores. **Data parallelism** splits the same operation over subsets of data; **task parallelism** runs different operations on different threads.

**Amdahl's law**: with serial fraction *S* and *N* cores, speedup ≤ 1 / (S + (1−S)/N). Example, S = 0.25: N = 2 gives 1.60, N = 4 gives 2.29, and as N → ∞ the limit is 1/S = 4.

**Challenges:** identifying parallel tasks, balancing work, splitting data, handling data dependencies, testing and debugging non-deterministic behaviour. Shared data needs synchronisation, otherwise a **race condition** occurs: two threads each doing `counter++` 1,000,000 times without a lock typically end below 2,000,000 because the increment is a load, add, store sequence.

## 2. Multithreading models

**User threads** are managed by a library above the kernel; **kernel threads** are managed and scheduled by the OS (Linux, Windows, macOS, Solaris). A relationship must exist between them:

Figure 2. Circles are user threads, rectangles are kernel threads. The thick line in the two-level model is a user thread bound to its own kernel thread.

| Model | Pros | Cons | Used by |
| --- | --- | --- | --- |
| Many-to-one | Cheap, thread switching in user space | One blocking system call blocks all threads; no multicore parallelism | Early green-thread libraries |
| One-to-one | Another thread runs when one blocks; true parallelism | Each user thread needs a kernel thread, so creation cost and an OS-imposed limit | Linux, Windows |
| Many-to-many | Any number of user threads; kernel threads run in parallel; blocking affects one kernel thread only | Complex; hard to coordinate two schedulers | Older Solaris, some runtimes |
| Two-level | Many-to-many plus the option to bind a critical thread | Same complexity | IRIX, HP-UX, Tru64 |

Modern runtimes that offer very many lightweight tasks (Go goroutines, Java virtual threads) multiplex them onto a small pool of kernel threads, which is the many-to-many idea in practice.

## 3. Pthreads

**Pthreads** is the POSIX thread API (IEEE 1003.1c), a *specification* implemented by the OS (Linux, macOS, other UNIX; Windows via ports). Types and functions are in `<pthread.h>`; build with `gcc -pthread`.

Figure 3. Fork-join pattern: the creator continues concurrently, then joins to wait for the worker and collect its return value.

```
#include <pthread.h>
#include <stdio.h>
long sum;                               /* shared by all threads */
void *runner(void *p) {                 /* thread start routine */
    long n = (long)p;
    for (long i = 1; i <= n; i++) sum += i;
    pthread_exit(NULL);
}
int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, runner, (void *)10L);   /* default attributes */
    pthread_join(tid, NULL);
    printf("sum = %ld\n", sum);         /* prints 55 */
    return 0;
}
```

| Function | Purpose |
| --- | --- |
| `pthread_create(&tid, &attr, start, arg)` | Create a thread; returns 0 on success, an error number otherwise (does not set `errno`) |
| `pthread_join(tid, &retval)` | Wait for thread termination; frees its resources |
| `pthread_detach(tid)` | Resources freed automatically at exit; cannot be joined |
| `pthread_exit(val)`, `pthread_self()` | Terminate the calling thread; get own ID |
| `pthread_attr_init/setstacksize/setdetachstate` | Configure stack size, detach state, scheduling attributes |
| `pthread_mutex_lock/unlock`, `pthread_cond_wait/signal` | Mutual exclusion and condition synchronisation |

## 4. Win32 threads

Windows implements the one-to-one model. Threads are kernel objects referenced by a `HANDLE`.

```
#include <windows.h>
DWORD Sum;
DWORD WINAPI Summation(LPVOID Param) {  /* thread function signature */
    DWORD Upper = *(DWORD *)Param;
    for (DWORD i = 1; i <= Upper; i++) Sum += i;
    return 0;
}
int main(void) {
    DWORD ThreadId, Param = 10;
    HANDLE h = CreateThread(NULL, 0, Summation, &Param, 0, &ThreadId);
    WaitForSingleObject(h, INFINITE);   /* wait for the thread */
    CloseHandle(h);
    return 0;                           /* Sum == 55 */
}
```

`CreateThread(security attributes, stack size (0 = default), start address, parameter, creation flags, &thread ID)`. Programs using the C runtime should call `_beginthreadex()` so per-thread CRT state is set up correctly.

| Pthreads | Win32 |
| --- | --- |
| `pthread_create` | `CreateThread` |
| `pthread_join` | `WaitForSingleObject` on the thread handle |
| `pthread_exit` | `ExitThread` (or return from the function) |
| `pthread_mutex_t` | `CRITICAL_SECTION` or mutex `HANDLE` |
| `pthread_cancel` | `TerminateThread` (unsafe: no cleanup, can leave locks held) |

## 5. Threading issues

### fork() and exec()

If one thread calls `fork()`, should the child duplicate all threads or only the caller? In POSIX the child has **only the calling thread**; other threads vanish, so the child should call `exec()` soon or use only async-signal-safe functions. `exec()` replaces the whole process image, including all threads.

### Signal handling

A signal is delivered to a process to notify an event. **Synchronous** signals are caused by the thread's own action (illegal memory access, divide by zero) and go to that thread; **asynchronous** ones come from outside (Ctrl-C, timer expiry). Each signal has a default or user-defined handler. Delivery choices in a multithreaded process: to the thread to which it applies, to every thread, to selected threads, or to one designated thread. In POSIX, `kill()` sends to the process (delivered to any thread that does not block it), `pthread_kill()` targets one thread, and `pthread_sigmask()` sets the per-thread blocked set.

### Thread cancellation

Terminating a **target thread** before it finishes, via `pthread_cancel(tid)`.

| Type | Behaviour | Risk |
| --- | --- | --- |
| Asynchronous | Target may be killed at any instant | May die holding a lock or half-way through updating shared data or inside `malloc` |
| Deferred (default) | Target checks a flag and terminates at a **cancellation point** (e.g. `read`, `sleep`, `pthread_testcancel()`) | Cancellation is delayed until a point is reached; cleanup handlers (`pthread_cleanup_push`) release resources |

### Thread-local storage (TLS)

Each thread needs its own copy of some data (e.g. a transaction ID, `errno`). Unlike local variables, TLS persists across function calls and is visible to all functions of that thread, like a per-thread static. Use `pthread_key_create()`/`pthread_setspecific()`, or the compiler keyword `_Thread_local` (C11) / `thread_local` (C++11).

### Scheduler activations

In many-to-many and two-level models an intermediate data structure, the **lightweight process (LWP)**, appears to the user-level library as a virtual processor. The kernel gives each LWP a kernel thread. When a user thread is about to block, the kernel makes an **upcall** to the library's upcall handler, which can schedule another user thread on that LWP, so user and kernel schedulers stay informed of each other.

## 6. Thread pools

Creating a thread per request has costs: creation time, and unbounded threads can exhaust CPU and memory. A **thread pool** creates a fixed number of worker threads at start-up; they wait for work on a queue.

Figure 4. Thread pool: tasks are submitted to a queue and executed by a fixed set of reusable workers.

- **Advantages:** servicing a request with an existing thread is faster than creating one; the number of threads is bounded; the task (what to run) is separated from the execution mechanism, so policies like delayed or periodic execution are possible.
- **Pool size:** heuristic Nthreads = Ncpu × Ucpu × (1 + W/C), where U is target CPU utilisation (0 to 1) and W/C the ratio of wait time to compute time. For CPU-bound work this is about Ncpu; with 8 cores, U = 1, W/C = 3 it gives 32. Pools can also resize dynamically.
- **APIs:** Windows `QueueUserWorkItem()`; Java `java.util.concurrent.Executors`; Apple Grand Central Dispatch; OpenMP and Intel TBB for parallel loops.

## 7. Linux threads

Linux does not distinguish processes from threads internally: both are **tasks**, each a `struct task_struct`. Creation is by `clone()`, whose flags choose what the new task shares with its parent. `fork()` is `clone()` with no sharing (copy-on-write copy); a thread is a `clone()` that shares most things. This gives the one-to-one model.

| clone() flag | Shares |
| --- | --- |
| `CLONE_VM` | Same address space (memory descriptor, `mm_struct`) |
| `CLONE_FILES` | Open file descriptor table |
| `CLONE_FS` | File-system info (current directory, root) |
| `CLONE_SIGHAND` | Signal handler table |
| `CLONE_THREAD` | Same thread group: tasks share a TGID, which is what `getpid()` returns |

Figure 5. Three tasks forming one thread group. Process-wide fields are shared by pointer, not copied.

- **Pthreads on Linux:** implemented by **NPTL** (Native POSIX Thread Library; replaced the older LinuxThreads in kernel 2.6 / glibc 2.3), which calls `clone()` and uses the **futex** (fast userspace mutex) system call so uncontended locks need no kernel entry.
- **IDs:** `getpid()` returns the TGID, shared by all threads; the per-thread ID is returned by `gettid()` (glibc wrapper from 2.30; earlier `syscall(SYS_gettid)`).
- **Stacks:** each thread gets its own user stack; the default size follows `RLIMIT_STACK` (typically 8 MB on x86-64 Linux), configurable with `pthread_attr_setstacksize()`.
- **Scheduling:** the kernel schedules every task independently (CFS for normal tasks; `SCHED_FIFO`/`SCHED_RR` for real-time priorities), so threads run in parallel across cores.

## Review questions

1\. A program is 75% parallelisable. Maximum speedup on 4 cores, and as cores → ∞?

S = 0.25. Speedup = 1 / (0.25 + 0.75/4) = 1 / 0.4375 ≈ 2.29. Limit = 1/S = 4.

2\. In a many-to-one library, one thread calls a blocking `read()`. What happens?

The kernel sees a single schedulable entity, so the whole process blocks and every user thread stops. One-to-one avoids this.

3\. A server has 8 cores, targets 100% CPU use, and requests wait 3 times as long as they compute. Pool size?

8 × 1 × (1 + 3) = 32 threads.

4\. Why is asynchronous cancellation dangerous?

The target may be stopped while holding a mutex or midway through updating shared data, leaving locks held and data inconsistent. Deferred cancellation acts only at cancellation points where cleanup handlers can run.

5\. A 4-thread process calls `fork()`. How many threads does the child have, and why does it usually call `exec()`?

One, the calling thread. Other threads' state (and any locks they held) is not usable in the child, so it normally calls `exec()` immediately.