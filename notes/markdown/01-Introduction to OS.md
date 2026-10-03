# Introduction to OS

Training handout: operating systems and real-time operating systems. Audience: M.Eng. embedded / systems and computer science.

After this handout you can:

- state what an OS provides and how it is organised;
- compare single-processor and multiprocessor systems, and batch, multiprogrammed, time-shared and interactive systems;
- trace a system call across the user/kernel boundary and contrast it with a function call;
- define hard, firm and soft real time and name the properties an RTOS must guarantee.

## 1. Essential features of an OS

An OS is the software layer that (a) **manages hardware resources** (CPU, memory, storage, I/O) and arbitrates between competing programs, and (b) **provides abstractions** (process, file, address space, socket) so programs do not depend on raw hardware.

| Feature | What it does |
| --- | --- |
| Process / thread management | Creation, scheduling, synchronisation, IPC |
| Memory management | Allocation, virtual addressing, protection between processes |
| File system | Named, persistent storage over block devices |
| Device management | Drivers, buffering, interrupt handling |
| Protection and security | Access control; user vs kernel privilege |
| User interface / API | System calls, shell, libraries |

Figure 1. Layered OS structure. The system-call interface is the only sanctioned way from user space into the kernel.

## 2. Single-processor and multiprocessor systems

### Single-processor

One CPU core executes one instruction stream at a time. Concurrency is an illusion made by switching between processes. Special-purpose processors (DMA controller, GPU, disk controller) may exist but do not run general user processes.

### Multiprocessor (parallel)

Two or more cores share the bus, memory and peripherals under one OS. Benefits: higher throughput, economy of scale, graceful degradation if a CPU fails. Speed-up with *N* CPUs is below *N* because of contention and synchronisation overhead (Amdahl's law).

Figure 2. Symmetric multiprocessing (SMP): one OS image, any CPU runs any task, all CPUs see the same memory.

|  | SMP (symmetric) | AMP (asymmetric) |
| --- | --- | --- |
| Roles | All cores peers; one kernel schedules on all | Cores have fixed roles, e.g. a master assigns work to others, or each core runs its own OS/RTOS |
| Typical use | Servers, phones, application processors | Heterogeneous SoCs, e.g. Cortex-A running Linux plus Cortex-M running an RTOS |
| Main issue | Cache coherence, lock contention | Inter-core communication, static partitioning |

Multicore chips put several cores on one die; to the OS they are multiprocessors. Large systems may be **NUMA**: memory access time depends on which node holds the memory.

## 3. Batch, multiprogrammed, time-sharing and interactive systems

### Batch processing

Similar jobs are collected into a batch and run one after another **without user interaction**. The user submits a job (program, data, control cards or job-control language) and collects output later. A **resident monitor** automatically sequences jobs. **Spooling** overlaps I/O of one job with computation of another by staging input and output on disk.

Figure 3. Batch pipeline with spooling.

**Limitations:** long turnaround time, no interaction or debugging during a run, and the CPU idles whenever the running job waits for I/O.

### Multiprogramming

Several jobs are kept in memory at once. When the running job blocks on I/O, the OS switches the CPU to another ready job, so the CPU stays busy. Goal: maximise **CPU utilisation**. It requires job scheduling (which jobs enter memory), CPU scheduling (which ready job runs), memory management and protection between jobs. It is not interactive on its own.

### Time sharing (multitasking)

A logical extension of multiprogramming: the CPU switches among jobs so often that each user perceives a dedicated machine. A **timer interrupt** ends each time slice (quantum, commonly of the order of 10 to 100 ms) and the scheduler (e.g. round robin) picks the next task. Goal: minimise **response time**.

### Interactive systems

The user communicates directly with the running program through keyboard, mouse or touch, and expects a response within about a second or less. Time sharing is the mechanism that makes interactive use of a shared machine possible; interactive programs are mostly event-driven and spend most time blocked waiting for input.

Figure 4. Schematic CPU timelines (not to scale). Multiprogramming removes idle time; time sharing adds pre-emption by timer.

|  | Batch | Multiprogramming | Time sharing / interactive |
| --- | --- | --- | --- |
| Main goal | Throughput of job stream | CPU utilisation | Response time |
| User interaction | None during run | None required | Direct, continuous |
| Switch trigger | Job ends | Job blocks for I/O | Timer interrupt or block |
| Context-switch cost | Negligible | Low | Significant; sets quantum size |

## 4. User mode and kernel mode

Hardware distinguishes at least two privilege levels so a faulty or malicious program cannot corrupt the OS or other programs. A **mode bit** records the current level. Kernel code runs privileged; applications run unprivileged.

- **Privileged instructions** execute only in kernel mode: I/O instructions, changing MMU/MPU settings, enabling or disabling interrupts, loading the timer, changing the mode bit, halting. Executing one in user mode raises an exception.
- **Memory protection** (MMU page tables or an MPU) limits each program to its own regions.
- **Timer**: the kernel arms it before handing the CPU to a user program, so no program can hold the CPU forever.

Figure 5. Entry to the kernel is only through traps, interrupts and exceptions; exit is by a privileged return instruction.

| Architecture | Privilege levels | Where the level lives / entry instruction |
| --- | --- | --- |
| x86 / x86-64 | Rings 0 to 3 (OS uses 0 and 3) | CPL = bits \[1:0\] of `CS`; `SYSCALL`/`SYSENTER`/`INT n` |
| ARMv8-A (AArch64) | EL0 (user), EL1 (kernel), EL2 (hypervisor), EL3 (secure monitor) | `PSTATE.EL`; `SVC` traps EL0 to EL1 |
| ARM Cortex-M | Thread / Handler mode, each privileged or unprivileged | `CONTROL.nPRIV` (bit 0) and `CONTROL.SPSEL` (bit 1); `SVC` raises the SVCall exception |
| RISC-V | M, S, U (S optional) | Current mode held by hardware; `mstatus.MPP` saves previous mode; `ECALL` |

Many small MCUs have no unprivileged mode or MPU, so an RTOS task there can touch any memory. Protection then relies on discipline, or on an MPU-enabled RTOS build.

## 5. Function call vs system call

|  | Function call | System call |
| --- | --- | --- |
| Mechanism | `CALL` / `BL`; pushes return address, jumps | Trap instruction (`SYSCALL`, `SVC`, `ECALL`, `INT 0x80`) into the kernel |
| Privilege | No change | User → kernel mode and back |
| Target | Any address in the program, found by linker | Number indexes a kernel dispatch table; user cannot choose an address |
| Stack | Same stack | Switches to a per-task kernel stack |
| Arguments | ABI registers / stack | Registers, validated by kernel (pointers are checked) |
| Cost | A few cycles | Hundreds of cycles or more: mode switch, argument checks, possible pipeline and TLB effects |
| Can fail by | Program bug | Returns error code (e.g. `-EFAULT`); the kernel survives bad input |

Applications normally call a **library wrapper** (e.g. `write()` in libc), which is an ordinary function call; the wrapper loads the syscall number and arguments and executes the trap.

Figure 6. Path of `write()` on Linux x86-64.

### Reference: Linux calling conventions

| ABI | Instruction | Syscall number | Arguments | Return | write / exit |
| --- | --- | --- | --- | --- | --- |
| x86-64 | `syscall` | `RAX` | `RDI, RSI, RDX, R10, R8, R9` | `RAX` | 1 / 60 |
| x86-32 | `int 0x80` | `EAX` | `EBX, ECX, EDX, ESI, EDI, EBP` | `EAX` | 4 / 1 |
| AArch64 | `svc #0` | `X8` | `X0 to X5` | `X0` | 64 / 93 |

On x86-64 the `SYSCALL` instruction saves the return `RIP` in `RCX` and `RFLAGS` in `R11`, which is why the 4th argument uses `R10` instead of `RCX`.

## 6. Real-time operating systems and real-time embedded systems

A system is **real-time** if correctness depends on *when* a result is produced as well as its value. "Real-time" means bounded and predictable, not fast. An **RTOS** is an OS whose scheduling, interrupt handling and services have bounded, documented worst-case latencies. A **real-time embedded system** is a computer inside a larger device (engine controller, pacemaker, drone) that must react to its environment within deadlines.

| Class | Missing a deadline means | Example |
| --- | --- | --- |
| Hard | System failure; possible harm | Airbag firing, ABS, pacemaker pacing |
| Firm | Result is useless but system continues; occasional misses tolerated | Sensor-fusion frame dropped, factory vision reject |
| Soft | Quality degrades, value falls gradually | Audio/video streaming, UI |

### Timing metrics

Figure 7. In an RTOS, the worst case of each interval is bounded; **jitter** is the variation of a latency between runs.

### Task states and scheduling

Figure 8. Typical RTOS task states (FreeRTOS naming; "Suspended" tasks are not scheduled until resumed).

- **Priority-based pre-emptive scheduling**: the highest-priority ready task always runs; a newly ready higher-priority task pre-empts immediately.
- **Rate-monotonic (RM)**: fixed priorities, shorter period = higher priority. Schedulable if CPU utilisation U ≤ n(21/n − 1): 1.000 (n=1), 0.828 (n=2), 0.780 (n=3), tending to ln 2 ≈ 0.693. This is a sufficient, not necessary, test.
- **Earliest deadline first (EDF)**: dynamic priorities; schedulable on one CPU iff U ≤ 1 (implicit deadlines).
- **Priority inversion**: a low-priority task holding a lock blocks a high-priority one while a medium task runs. Fix with **priority inheritance** or ceiling protocols. This caused the 1997 Mars Pathfinder resets (VxWorks).

### Cortex-M example

RTOS ports use three core exceptions: `SVCall` (exception 11, vector at `0x0000002C`) to start the first task or request kernel services; `PendSV` (14, `0x00000038`) at lowest priority for context switching; `SysTick` (15, `0x0000003C`) for the periodic OS tick. Deferring the switch to PendSV keeps it from pre-empting other interrupt handlers.

### GPOS vs RTOS

|  | General-purpose OS (Linux, Windows) | RTOS (FreeRTOS, Zephyr, VxWorks, QNX, RTEMS) |
| --- | --- | --- |
| Design goal | Throughput, fairness, features | Determinism, bounded latency |
| Scheduler | Time-slice, fairness-oriented | Priority-based, pre-emptive |
| Kernel size | Megabytes | Often a few KB to hundreds of KB |
| Virtual memory | Typical | Often absent or MPU-only |
| Worst-case timing | Not guaranteed (stock kernel) | Documented and tested |

Stock Linux is not hard real time; the PREEMPT_RT patch set (merged into mainline from Linux 6.12) greatly narrows worst-case latency.

## Review questions

1. Why must the instruction that disables interrupts be privileged?
2. A job spends 80% of its time waiting for I/O. Roughly how many such jobs must be in memory to keep one CPU near full use under multiprogramming?
3. Name two reasons a system call costs more than a function call.
4. Three periodic tasks have U = 0.82. Does the RM bound guarantee schedulability? Does EDF?
5. Classify: an airbag controller; a video player; an engine-control task that drops one stale sample.

## Answers

1. **Interrupts.** If a user program could disable interrupts, it could mask the timer interrupt and keep the CPU indefinitely, and it could delay or lose device interrupts. The kernel would lose control of scheduling and I/O, so the instruction is privileged.
2. **Jobs in memory.** If each job waits for I/O a fraction p = 0.8 of the time and waits are independent, CPU utilisation is 1 − pn. n = 10 gives about 0.89, n = 15 about 0.96, n = 20 about 0.99. So roughly 10 to 15 jobs. This is a simplified model; real waits are not independent, and memory limits the degree of multiprogramming.
3. **System call cost.** Any two of: the mode switch and the kernel entry/exit work (saving and restoring registers, switching to the kernel stack); validating arguments and copying data across the user/kernel boundary; lost pipeline, cache or TLB state; extra mitigations against speculative-execution attacks on some CPUs.
4. **U = 0.82, three tasks.** The RM bound for n = 3 is 0.780. Since 0.82 > 0.780, the bound does *not* guarantee schedulability; the set might still be schedulable, which needs exact response-time analysis. EDF is schedulable on one CPU when U ≤ 1 (deadlines equal periods, independent tasks), so 0.82 passes.
5. **Classification.** Airbag controller: hard (a missed deadline can cause harm). Video player: soft (a late frame only degrades quality). Engine-control task dropping one stale sample: firm (the late result is useless and is discarded, but the system continues).