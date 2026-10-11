# Embedded Linux Assignment 01

Linux kernel driver development for ARM-based embedded Linux systems.

## Overview

This project contains two independent Linux kernel modules developed
for Assignment 01 and tested in an ARM QEMU environment.

## Project Structure

```text
task1_ioctl/
  asgn1_driver.c
  asgn1_ioctl.h
  test_asgn1.c
  Makefile
task2_misc/
  counter_driver.c
  Makefile
```

## Requirements

- Linux kernel source tree (version 5.15.0 used during development)
- ARM cross-compiler: arm-linux-gnueabihf-
- GNU Make
- QEMU ARM system emulator for runtime testing

## Task 1: Character Device Driver with ioctl

- Device: /dev/asgn1
- Read and write operations
- Buffer reset, statistics, mode selection, and version ioctls
- Echo mode and kernel-prefix mode
- Procfs and sysfs interfaces

Build the module and test application:

```bash
cd task1_ioctl
make
arm-linux-gnueabihf-gcc -Wall -Wextra -o test_asgn1 test_asgn1.c
```

## Task 2: Miscellaneous Counter Device

- Device: /dev/counter
- Atomic counter operations
- Read increments the counter
- Write sets or resets the counter
- Read and write statistics
- Procfs interface: /proc/counter_info

Build the module:

```bash
cd task2_misc
make
```

## Target Environment

Linux 5.15.0 ARM kernel and QEMU vexpress-a9.
Kernel modules must match the target kernel version and configuration.

## License

Educational assignment project.
