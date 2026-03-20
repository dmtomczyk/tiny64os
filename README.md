# tiny64os

`tiny64os` is a small educational x86_64 operating system built with freestanding C and bootstrap assembly.

It is intentionally compact, but no longer just a "hello world" kernel. It boots through GRUB/Multiboot2, enters 64-bit long mode, handles timer and keyboard interrupts, provides useful exception diagnostics, and exposes a tiny interactive shell in VGA text mode.

The goal of this repository is to be a modern, understandable, buildable example of a minimal 64-bit hobby OS.

![tiny64os GRUB Screenshot](/pics/tiny64os-grub.png)

![tiny64os Screenshot](/pics/tiny64os-shell.png)

## Current features

- GRUB / Multiboot2 boot flow
- 32-bit bootstrap with CPU checks and long-mode transition
- 64-bit kernel entry
- VGA text console output
- VGA hardware cursor tracking
- IDT setup
- Exception diagnostics with vector, error code, RIP, CS, RFLAGS, and CR2 for page faults
- PIC remap
- PIT timer running in the background
- PS/2 keyboard interrupt handling
- COM1 serial boot logging
- Tiny shell with line editing and command history

## Current shell commands

- `help`
- `about`
- `version`
- `uptime`
- `ticks`
- `echo`
- `clear`
- `cls`
- `reboot`
- `panic`

## What this repo is

`tiny64os` is meant to sit in a useful middle ground:
- small enough to read and understand quickly
- real enough to demonstrate interrupts, input, fault handling, and a shell
- narrow enough to stay teachable

What it does **not** have yet:
- physical memory management
- kernel heap
- scheduler / multitasking
- user mode / syscalls
- filesystem / storage stack
- general driver model

## Prerequisites

### Native build on Debian

Install the packages needed to build the ISO and run it in QEMU:

```bash
sudo apt update
sudo apt install -y \
  build-essential make \
  grub-common grub-pc-bin \
  xorriso mtools \
  qemu-system-x86
```

Notes:
- If `x86_64-elf-gcc` is installed, the Makefile will prefer it.
- Otherwise the project falls back to host `gcc` / `ld` with freestanding flags.
- If an ISO creator is missing, the build can still produce `kernel.bin`, but `kernel.iso` is the easiest way to boot under QEMU.

### Docker build environment

Requirements:
- Docker
- QEMU (optional, if you want to run the built image outside the container)

## Build

### Option 1: native host build

```bash
make build-x86_64
```

### Option 2: Docker build environment

Build the image:

```bash
docker build buildenv -t myos-buildenv
```

Build the kernel inside the container:

```bash
docker run --rm -it -v "$(pwd)":/root/env myos-buildenv make build-x86_64
```

Windows (CMD):

```bat
docker run --rm -it -v "%cd%":/root/env myos-buildenv make build-x86_64
```

Windows (PowerShell):

```powershell
docker run --rm -it -v "${pwd}:/root/env" myos-buildenv make build-x86_64
```

## Run in QEMU

### Standard VGA run

```bash
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
```

### With serial boot logs in your terminal

```bash
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial stdio
```

Or, if you want QEMU monitor + serial on stdio:

```bash
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial mon:stdio
```

### Helpful debug flags

```bash
qemu-system-x86_64 \
  -cdrom dist/x86_64/kernel.iso \
  -no-reboot \
  -no-shutdown \
  -d int,cpu_reset
```

## Expected behavior

On boot, `tiny64os` should:
- load through GRUB
- enter 64-bit mode
- initialize interrupts and the timer
- present a clean shell screen
- accept keyboard input in VGA text mode

The shell currently supports:
- history recall with Up/Down
- in-line editing with Left/Right, Home/End, Delete, and Backspace
- `Ctrl-C` to cancel the current line

## Project layout

```text
arch/x86_64/impl/       Architecture-specific implementation code
arch/x86_64/intf/       Headers / subsystem interfaces
targets/x86_64/         Linker script and ISO boot tree
buildenv/               Docker build environment
dist/x86_64/            Built kernel artifacts
pics/                   Screenshots
ROADMAP.md              Longer-term development plan for this teaching repo
```

There is also a small architecture/file-layout note at:
- `docs/architecture.md`

## Verification

If you only want to verify the kernel image is recognized as Multiboot2:

```bash
grub-file --is-x86-multiboot2 dist/x86_64/kernel.bin
```

## Cleanup

```bash
make clean
```

To remove the Docker build image:

```bash
docker rmi myos-buildenv -f
```

## Scope

`tiny64os` is intended to stay focused on being a minimal, approachable x86_64 operating-system example.

Larger experiments such as deeper memory management, scheduling, storage, and broader OS architecture can continue independently in other downstream or sibling projects.
