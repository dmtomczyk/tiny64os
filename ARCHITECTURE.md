# tiny64os Architecture Overview

This is a small map of how the current system boots and where the major pieces live.

## Boot flow

1. **GRUB** loads the kernel through **Multiboot2**.
2. The 32-bit bootstrap code:
   - verifies Multiboot handoff
   - checks CPU support
   - sets up the early page tables
   - enables long mode
3. Control transfers into 64-bit assembly startup.
4. 64-bit startup calls `kernel_main()`.
5. The C kernel initializes:
   - console / boot output
   - IDT
   - PIC remap
   - PIT timer
   - keyboard IRQ support
   - shell

## Major subsystems

### Boot / CPU bring-up

Files:
- `arch/x86_64/impl/boot/header.asm`
- `arch/x86_64/impl/boot/main.asm`
- `arch/x86_64/impl/boot/main64.asm`

Responsibilities:
- Multiboot2 header
- early CPU checks
- initial paging setup
- transition to long mode
- transfer into C

### Kernel entry

File:
- `arch/x86_64/impl/kernel/main.c`

Responsibilities:
- high-level bring-up order
- boot log flow
- shell handoff

### VGA console / text output

Files:
- `arch/x86_64/impl/print.c`
- `arch/x86_64/intf/print.h`

Responsibilities:
- writing to VGA text memory
- newline / scrolling behavior
- cursor updates
- integer / hex helpers
- optional serial mirroring during boot

### Interrupts and faults

Files:
- `arch/x86_64/impl/idt.c`
- `arch/x86_64/impl/interrupts.asm`
- `arch/x86_64/intf/idt.h`
- `arch/x86_64/intf/interrupts.h`

Responsibilities:
- IDT setup
- exception entry stubs
- IRQ entry stubs
- fault reporting

### PIC and PIT

Files:
- `arch/x86_64/impl/pic.c`
- `arch/x86_64/impl/pit.c`
- `arch/x86_64/intf/pic.h`
- `arch/x86_64/intf/pit.h`

Responsibilities:
- PIC remap
- IRQ masking / unmasking
- PIT initialization
- tick counting

### Keyboard input

Files:
- `arch/x86_64/impl/keyboard.c`
- `arch/x86_64/intf/keyboard.h`

Responsibilities:
- PS/2 scancode handling
- modifier tracking
- shell navigation keys
- Ctrl-C handling

### Shell

Files:
- `arch/x86_64/impl/shell.c`
- `arch/x86_64/intf/shell.h`

Responsibilities:
- prompt and command loop
- line editing
- history recall
- built-in commands

### Serial logging

Files:
- `arch/x86_64/impl/serial.c`
- `arch/x86_64/intf/serial.h`

Responsibilities:
- COM1 initialization
- serial character / string output
- boot diagnostics outside VGA

### Port I/O helpers

Files:
- `arch/x86_64/impl/io.c`
- `arch/x86_64/intf/io.h`

Responsibilities:
- `inb`
- `outb`
- tiny I/O wait helper

## Directory layout

```text
arch/x86_64/impl/
  boot/         bootstrap asm and long-mode entry
  kernel/       high-level kernel startup
  *.c           subsystem implementations
  *.asm         interrupt / low-level assembly glue

arch/x86_64/intf/
  *.h           subsystem interfaces

targets/x86_64/
  linker.ld     linker script
  iso/          GRUB boot tree used to build kernel.iso

buildenv/
  Dockerfile    reproducible build environment

pics/
  screenshots / images
```

## Current architectural shape

`tiny64os` is best thought of as:
- a bootable x86_64 kernel
- a text console environment
- a hardware-interrupt example that is already usable
- a compact base for learning how these pieces fit together

The goal here is not to become huge quickly. The goal is to stay readable while still showing a real interrupt-driven system with a basic shell.
