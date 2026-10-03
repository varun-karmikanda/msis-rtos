# Introduction to OS

Part 2: Process management and process scheduling. Audience: M.Eng. embedded / systems and computer science. Every scheduling algorithm has a worked problem; try it first, then open the answer.

## 1. Process management

### A process in memory

A **process** is a program in execution: code plus current activity (program counter, registers, stack, data). A program is passive (a file on disk); a process is active.

**Process vs thread.** A process owns an address space and resources (open files, PCB). A thread is a unit of execution inside it with its own PC, registers and stack, sharing the process's code, data and heap. **Creation:** a parent creates children (`fork()`), forming a process tree rooted at `init`/`systemd` (PID 1). **Termination:** a process calls `exit()` or is killed; the kernel frees its resources but keeps the PCB's exit status until the parent calls `wait()`. A terminated child not yet waited for is a **zombie**; a child whose parent exited is an **orphan** and is adopted by PID 1.

**Process behaviour.** An **I/O-bound** process has many short CPU bursts between I/O requests; a **CPU-bound** process has few, long bursts. The scheduler design below exists to serve both well.

Figure 1. Typical virtual address space of a process (layout varies by OS and architecture).

### Process state

Figure 2. Five-state process model. Only one process per CPU core is Running at an instant.

### Process control block (PCB)

The kernel's per-process record (`task_struct` in Linux). A **context switch** saves the running process's CPU state into its PCB and loads the next one's; it is pure overhead.

| PCB field | Content |
| --- | --- |
| Process ID, parent ID | Unique PID; PPID |
| State | New, Ready, Running, Waiting, Terminated |
| Program counter, CPU registers | Saved on a switch so execution can resume exactly |
| Scheduling information | Priority, queue pointers, time used |
| Memory-management information | Page-table base, segment limits |
| Accounting, I/O status | CPU time used, open files, allocated devices |

#### Context switch

On an interrupt or system call the kernel (1) saves the running process's PC and registers into its PCB, (2) updates its state and moves the PCB to the proper queue, (3) selects the next process, (4) loads that process's registers, page-table base and PC from its PCB, and (5) resumes it. No useful work is done meanwhile; typical cost is a few microseconds, rising with cache and TLB refill. Hardware with multiple register sets can cut it.

### Scheduling queues and schedulers

Figure 3. Queueing diagram. Also: a forked child waits in the ready queue; a process waiting for an interrupt sits in a wait queue.

| Scheduler | Decides | Frequency | Notes |
| --- | --- | --- | --- |
| Long-term (job) | Which jobs are admitted to memory | Seconds to minutes | Controls degree of multiprogramming; aims for a mix of I/O-bound and CPU-bound |
| Short-term (CPU) | Which ready process gets the CPU next | Milliseconds | Must be fast; followed by the **dispatcher** (context switch, switch to user mode, jump to saved PC) |
| Medium-term | Which processes to swap out and back in | Occasional | Reduces memory pressure and load |

**Queues.** The **job queue** holds all processes in the system; the **ready queue** holds those in memory and ready to run (usually a linked list of PCBs with head and tail pointers); each I/O device has a **device queue** of processes waiting for it. A process migrates between these queues throughout its life.

**Scheduler balance.** If the long-term scheduler admits only I/O-bound jobs the CPU idles; only CPU-bound jobs leave devices idle. A good mix keeps both busy. The **medium-term scheduler** swaps a process out to disk and later back in (swapping) to improve the mix or free memory. The **dispatcher** time between stopping one process and starting another is **dispatch latency**.

### Process system calls (POSIX)

```
pid_t pid = fork();            /* 0 in child, child's PID in parent, -1 on error */
if (pid == 0) { execlp("ls", "ls", (char*)0); _exit(127); }   /* replace image */
else if (pid > 0) { int st; waitpid(pid, &st, 0); }          /* parent reaps child */
```

`fork()` duplicates the process; `exec*()` replaces its program image; `wait()/waitpid()` blocks until a child ends and reaps it (otherwise it stays a zombie); `exit()` terminates.

Processes are isolated by default, yet cooperating processes need to exchange data for **information sharing**, **computation speed-up** and **modularity**. There are two models: **shared memory** (processes read and write a common region) and **message passing** (the kernel carries messages with `send`/`receive`; sockets, pipes and message queues are examples). Shared memory is faster for large data; message passing is simpler to get right and works across machines.

### IPC using shared memory

Processes map the same physical pages into their own address spaces. After setup there is no kernel involvement per access, so it is the fastest IPC, but the processes must synchronise themselves (semaphore, mutex).

Figure 4. Shared memory.

POSIX call sequence: `shm_open()` (create/open object) → `ftruncate()` (set size) → `mmap(…, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)` → use → `munmap()`, `shm_unlink()`.

**Classic example: producer-consumer.** A producer fills a bounded buffer in shared memory, a consumer empties it. The producer must wait when the buffer is full, the consumer when it is empty, and both must not update the buffer indices at once. Without synchronisation (semaphores, mutex plus condition variables) the result is a race condition.

### IPC using sockets

A socket is a communication endpoint for message passing, between processes on one host (`AF_UNIX`) or across a network (`AF_INET`). The kernel copies data between buffers, so it is slower than shared memory but works across machines.

Figure 5. TCP (`SOCK_STREAM`) sequence. UDP (`SOCK_DGRAM`) skips listen/accept/connect and uses `sendto()/recvfrom()`.

A socket is identified by an IP address plus a port number (0 to 65535; ports below 1024 are well-known and privileged). **TCP** gives a reliable, ordered byte stream with connection set-up; **UDP** gives unreliable, unordered datagrams with no set-up. Sockets are treated as file descriptors, so `read()/write()/close()` also work on them.

## 2. Process scheduling

The short-term scheduler picks the next ready process. Decisions occur when a process (1) moves Running→Waiting, (2) Running→Ready, (3) Waiting→Ready, (4) terminates. If scheduling happens only at 1 and 4 the scheme is **non-preemptive**; otherwise it is **pre-emptive**.

| Criterion | Goal | Definition (AT = arrival, BT = CPU burst, CT = completion) |
| --- | --- | --- |
| CPU utilisation | Maximise | Fraction of time the CPU is busy |
| Throughput | Maximise | Processes completed per unit time |
| Turnaround time (TAT) | Minimise | CT − AT |
| Waiting time (WT) | Minimise | TAT − BT (time spent in the ready queue) |
| Response time (RT) | Minimise | First time on CPU − AT |

**CPU-I/O burst cycle.** Execution alternates between a CPU burst and an I/O wait. Most bursts are short (a few ms); very few are long, so the burst-length distribution is heavy at the short end. This is why favouring short bursts (SJF, feedback queues) works well. The scheduler is invoked at the four decision points above, and the dispatcher then performs the switch.

Conventions for all problems: time unit = ms, context-switch time = 0, single CPU, one CPU burst per process, no I/O. Ties are broken by earlier arrival, then lower process number.

### Scheduling algorithms

| Algorithm | Rule | Pre-emptive? | Main weakness |
| --- | --- | --- | --- |
| FCFS | Queue order of arrival | No | Convoy effect: short jobs wait behind a long one |
| SJF | Shortest next CPU burst first; minimises average WT | No (SJF) / yes (SRTF) | Burst length unknown; estimated as τn+1 = α·tn + (1−α)·τn; long jobs may starve |
| Priority (PS) | Highest priority first (here lower number = higher priority) | Either | Starvation; fix by **aging** (raise priority with waiting time) |
| Round robin (RR) | FCFS with time quantum *q*; pre-empt at expiry | Yes | Large *q* → FCFS; tiny *q* → switch overhead. With *n* processes, no one waits more than (n−1)·q for its turn |
| Multilevel queue | Ready queue split by process class; each queue has its own algorithm; fixed priority (or time share) between queues | Yes | Rigid: no movement between queues |
| Multilevel feedback queue | Like MLQ, but processes move between queues based on behaviour | Yes | Many parameters to tune |

#### FCFS theory

Processes get the CPU in order of arrival, implemented with a FIFO queue; a running process keeps the CPU until it finishes or blocks (non-preemptive). Simple and starvation-free, but average waiting time depends heavily on arrival order. Bursts 24, 3, 3 arriving in that order give average WT = (0+24+27)/3 = 17 ms; arriving as 3, 3, 24 gives (0+3+6)/3 = 3 ms. Short processes stuck behind one long CPU-bound process is the **convoy effect**, which also leaves I/O devices idle. Unsuitable for time-sharing.

### Problem 1: FCFS

Draw the Gantt chart and compute CT, TAT, WT, RT for each process and their averages.

Show answer

Average WT = 5.75 ms. P4 needs only 2 ms but waits 13 ms behind P3: the convoy effect.

#### SJF and SRTF theory

Each process is associated with the length of its next CPU burst and the shortest runs first. SJF is **provably optimal** for minimum average WT: moving a short job ahead of a long one reduces the short job's wait by more than it increases the long job's. The difficulty is that the next burst is unknown, so it is predicted by **exponential averaging**: τn+1 = α·tn + (1−α)·τn, where tn is the measured last burst and τn the previous prediction (0 ≤ α ≤ 1; α = 0 ignores recent behaviour, α = 1 uses only the last burst; α = ½ is common). Example: α = ½, τ0 = 10, t0 = 6 gives τ1 = 8; then t1 = 4 gives τ2 = 6.

**SRTF** is the pre-emptive version: when a process arrives with a burst shorter than the running process's *remaining* time, it pre-empts. Long processes may starve if short ones keep arriving.

### Problem 2: SJF (non-preemptive)

Schedule with non-preemptive SJF. The same workload is reused in Problem 3.

Show answer

At t = 7 the ready processes are P2 (4), P3 (1), P4 (4); P3 runs, then the P2/P4 tie goes to earlier arrival. Average WT = 4.00 ms.

### Problem 3: SRTF (pre-emptive SJF)

Same workload as Problem 2, now with shortest-remaining-time-first: a new arrival pre-empts the running process if its burst is shorter than the remaining time. Compare average WT with Problem 2.

Show answer

P2 pre-empts P1 at t = 2 (4 \< 5); P3 pre-empts P2 at t = 4 (1 \< 2). Average WT = 3.00 ms versus 4.00 ms non-preemptive, with the cost of more context switches.

#### Priority scheduling theory

Each process has a priority and the CPU goes to the highest. SJF is a special case where priority = 1/predicted burst. Priorities are **internal** (computed from time limits, memory needs, I/O-to-CPU burst ratio) or **external** (importance, user, payment). It can be non-preemptive (the new arrival waits) or pre-emptive (a higher-priority arrival takes the CPU at once). **Indefinite blocking (starvation)** of low-priority processes is its chief problem; **aging** gradually raises the priority of waiting processes. Equal-priority processes are often scheduled round robin. RTOSes use fixed-priority pre-emptive scheduling.

### Problem 4: Priority scheduling (non-preemptive)

All processes arrive at t = 0. Lower number = higher priority. Find the Gantt chart and average WT. Then name the risk for P4 if high-priority work keeps arriving, and the remedy.

Show answer

Average WT = 8.20 ms. Risk: **starvation** of low-priority processes (P4). Remedy: **aging**, e.g. raise priority by 1 for every 10 ms spent waiting.

#### Round robin theory

FCFS plus a **time quantum** q (commonly 10 to 100 ms), designed for time-sharing. The ready queue is circular. A timer interrupts after q; if the burst is shorter than q the process releases the CPU voluntarily, otherwise it is pre-empted and goes to the queue tail. With n ready processes each gets 1/n of the CPU and waits at most (n−1)·q before its turn, which gives good response time. Choice of q: very large → behaves as FCFS; very small → context-switch overhead dominates. Overhead fraction = cs / (q + cs), e.g. cs = 0.1 ms and q = 10 ms gives about 1%. Rule of thumb: about 80% of bursts should be shorter than q. Average turnaround is often worse than SJF.

### Problem 5: Round robin, q = 4

Schedule with RR, q = 4. A process arriving at the same instant a quantum expires joins the queue before the pre-empted process. Also count the context switches.

Show answer

Ready queue after t = 4 is P2, P3, P4, P1. Average WT = 8.75 ms. Seven CPU slices, so 6 context switches. Response time is bounded by the queue position, not by burst length.

### Multilevel queue and multilevel feedback queue

#### Multilevel queue (MLQ) theory

The ready queue is **partitioned** by process type, for example foreground (interactive) and background (batch). A process is permanently assigned to one queue and each queue has its own algorithm (foreground RR, background FCFS). Scheduling *between* queues is either **fixed priority** (serve foreground until empty; risk of starvation) or **time slicing** (e.g. 80% of CPU to foreground RR, 20% to background FCFS). Low overhead, but inflexible.

#### Multilevel feedback queue (MLFQ) theory

Allows processes to **move between queues**, separating them by observed CPU-burst behaviour. A process that uses too much CPU is demoted; one that waits too long in a low queue is promoted (aging). Interactive and I/O-bound processes stay in high queues. It is defined by five parameters: number of queues; algorithm per queue; rule to upgrade a process; rule to demote it; and which queue a new process enters. Most general and most complex scheduler; it approximates SJF without knowing burst lengths.

Figure 6. A three-level feedback queue. Short, interactive bursts finish in Q0; CPU-bound work sinks to Q2. A lower queue is pre-empted when a higher one becomes non-empty.

### Problem 6: Multilevel queue

Two queues. Q1 (system processes): RR, q = 2. Q2 (batch): FCFS. Q1 has absolute priority over Q2 and pre-empts it. Find the Gantt chart and averages.

Show answer

P3 arrives at t = 1 and queues behind P1 in Q1. Q2 starts only at t = 7 when Q1 is empty, so P2 (arrived at 0) waits 7 ms. Average WT = 5.00 ms.

### Problem 7: Multilevel feedback queue

Queues as in Figure 6 (Q0 RR q = 2, Q1 RR q = 4, Q2 FCFS). A process that uses a full quantum is demoted one level. All arrive at t = 0 in order P1, P2, P3. Give the Gantt chart, and state which queue each process finishes in.

Show answer

Q0 (t = 0 to 6): each runs 2. P1 has 5 left, P2 1, P3 7 → all to Q1. Q1: P1 runs 4 (1 left → Q2), P2 finishes in Q1 at t = 11, P3 runs 4 (3 left → Q2). Q2 (FCFS): P1 finishes at 16, P3 at 19. So P2 finishes in Q1; P1 and P3 in Q2. Average WT = 9.00 ms.

## 3. Scheduling evaluation

| Method | Idea | Limit |
| --- | --- | --- |
| Deterministic modelling | Run each algorithm on a fixed workload and compare metrics (as in these problems) | Exact but only for that workload |
| Queueing models | Use arrival and service distributions; Little's formula **n = λ × W** | Assumptions often unrealistic |
| Simulation | Model the system; drive with random or trace data | Costly; trace must be representative |
| Implementation | Build it in a real kernel and measure | Most accurate; highest cost and risk |

**Choosing criteria.** Evaluation starts by fixing what to optimise, often as a constraint such as "maximise CPU utilisation subject to maximum response time ≤ 1 s". **Little's formula** n = λ × W relates average queue length n, arrival rate λ and average wait W; it holds for any scheduling algorithm and arrival distribution in steady state. **Simulation** uses a clock variable and models of arrivals and bursts, either random with fitted distributions or taken from a recorded trace of a real system.

### Problem 8: Deterministic modelling

Processes P1, P2, P3 all arrive at t = 0. Compare FCFS, non-preemptive SJF and RR (q = 4) on average WT and average RT. Which is best for each metric?

Show answer

Average WT: FCFS 7.33, SJF 3.33, RR 7.33. Average RT: FCFS 7.33, SJF 3.33, RR 3.33. SJF is best on waiting time. RR ties FCFS on waiting time but matches SJF on response time, which is why it suits interactive systems.

### Problem 9: Little's formula

On average 14 processes are in the ready queue and 7 processes arrive per second. What is the average time a process spends in the queue?

Show answer

W = n / λ = 14 / 7 = 2 s. (Valid in steady state, when the arrival rate equals the departure rate.)