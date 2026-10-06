# AegisOS — Intelligent Unix Shell and Process Management

AegisOS is a lightweight Unix-like command shell implemented in **C on Linux/Ubuntu**. The project demonstrates core Operating Systems concepts through a practical, interactive terminal environment.

The project combines command interpretation with demonstrations of process management, job control, signals, pipes, I/O redirection, POSIX threads, synchronization, inter-process communication, memory management, virtual memory, process monitoring, system information, file-system operations, diagnostics, and live process inspection.

> **Important:** AegisOS is an educational Unix-like shell and Operating Systems concept demonstration project. It is not a complete operating-system kernel.

---

## 1. Project Vision

> **Transforming Operating Systems theory into observable, interactive system behavior.**

Operating Systems concepts are often studied as separate theoretical topics and isolated programs.

AegisOS brings several of these mechanisms together into one lightweight shell so that users can directly execute commands and observe system behavior.

The project focuses on:

- Practical experimentation.
- Direct interaction with Linux/POSIX mechanisms.
- Modular C implementation.
- Reproducible demonstrations.
- Interactive observation of OS behavior.
- Educational accessibility.

---

## 2. Project Objectives

The main objectives are to:

- Build a functional command-line shell from scratch.
- Understand command parsing and execution.
- Demonstrate process creation and execution.
- Demonstrate foreground and background processes.
- Demonstrate job tracking and signals.
- Demonstrate pipes and I/O redirection.
- Demonstrate POSIX threads.
- Demonstrate synchronization mechanisms.
- Demonstrate multiple IPC mechanisms.
- Demonstrate dynamic and virtual memory.
- Demonstrate Linux process monitoring.
- Display CPU and system memory information.
- Demonstrate file-system operations.
- Demonstrate error handling and diagnostics.
- Provide a built-in help system.
- Provide unified live process inspection.
- Integrate multiple OS concepts into one working environment.

---

## 3. Technology Stack

| Technology | Purpose |
|---|---|
| C | Core implementation language |
| GCC | Compilation |
| GNU Make | Build automation |
| Linux / Ubuntu | Runtime environment |
| POSIX APIs | Operating-system interfaces |
| `/proc` | Process and system information |
| POSIX threads | Thread demonstrations |
| POSIX IPC | Inter-process communication |
| `mmap()` | Virtual memory demonstration |
| POSIX synchronization | Thread/process coordination |

---

## 4. Features

### Shell Functionality

- Interactive command prompt.
- Command parsing.
- External command execution.
- Child process creation.
- Foreground process execution.
- Background process execution.
- Job tracking.
- Signal handling.
- Built-in commands.
- Input/output redirection.
- Pipeline execution.
- Built-in help system.
- Live process inspection.

### Operating-System Demonstrations

- Process creation using `fork()`.
- Program execution using `exec()`.
- Process synchronization using `wait()` / `waitpid()`.
- Background processes and job control.
- Linux signals.
- Anonymous pipes.
- Bidirectional pipe IPC.
- POSIX shared memory.
- POSIX semaphores.
- POSIX condition variables.
- POSIX message queues.
- POSIX threads.
- Thread scheduling demonstration.
- Dynamic memory allocation.
- Virtual memory using `mmap()`.
- Process monitoring.
- CPU and system memory information.
- File-system operations.
- Error handling and diagnostics.
- Live process inspection.
- Open file-descriptor inspection.

---

## 5. Project Structure

```text
AegisOS/
├── include/
│   ├── builtin.h
│   ├── condition.h
│   ├── diagnostics.h
│   ├── filesystem.h
│   ├── help.h
│   ├── inspect.h
│   ├── ipc.h
│   ├── jobs.h
│   ├── memory.h
│   ├── message_queue.h
│   ├── monitor.h
│   ├── parser.h
│   ├── pipes.h
│   ├── process.h
│   ├── process_monitor.h
│   ├── shared_memory.h
│   ├── shell.h
│   ├── signals.h
│   ├── sync.h
│   ├── system_info.h
│   ├── threads.h
│   └── virtual_memory.h
│
├── src/
│   ├── main.c
│   ├── shell.c
│   ├── parser.c
│   ├── process.c
│   ├── signals.c
│   ├── pipes.c
│   ├── jobs.c
│   ├── threads.c
│   ├── monitor.c
│   ├── builtin.c
│   ├── ipc.c
│   ├── shared_memory.c
│   ├── sync.c
│   ├── condition.c
│   ├── message_queue.c
│   ├── memory.c
│   ├── virtual_memory.c
│   ├── process_monitor.c
│   ├── system_info.c
│   ├── filesystem.c
│   ├── diagnostics.c
│   ├── help.c
│   └── inspect.c
│
├── tests/
│   └── integration_test.sh
│
├── Makefile
├── README.md
└── aegis
