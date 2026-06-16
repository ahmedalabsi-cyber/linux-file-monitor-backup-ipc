# Linux File Monitoring, Backup and IPC System

A Linux-based C application that scans directories for text files, logs file information, creates backup copies, and demonstrates Inter-Process Communication (IPC) using pipes and forked processes.

## Features

- Directory scanning
- File metadata collection
- Automatic backup creation
- Logging and alert system
- Dynamic memory allocation
- Shell script automation
- IPC using pipes
- Parent-child communication using fork()
- Signal handling (SIGINT)
- Makefile build system

## Technologies

- C
- Linux
- Bash
- Makefile
- GCC
- IPC (fork, pipe)
- Dynamic Memory Allocation

## Project Structure

```text
main.c
scanner.c
backup.c
logger.c
file_info.c

scanner.h
backup.h
logger.h
file_info.h

monitor_ipc.c
Makefile
run_system.sh
```

## Build

```bash
make
```

## Run

```bash
./monitor src/
```

## IPC Version

```bash
./monitor_ipc src/
```

## Learning Outcomes

- Linux CLI
- Bash Scripting
- Modular C Programming
- Dynamic Memory Management
- Inter-Process Communication
- Process Management
- Debugging with GDB
```
