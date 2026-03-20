# tiny64os Software Design Document

This is a living design note for `tiny64os`. It captures what the kernel does today, what design direction it is heading toward, and which subsystems are still intentionally unfinished.

## Purpose

`tiny64os` is meant to be a compact, educational x86_64 operating-system example. The emphasis is on readability, stable interrupt-driven behavior, and clear build/run instructions rather than on becoming a large general-purpose OS.

## Boot strategy

`tiny64os` currently uses GRUB + Multiboot2 rather than a custom bootloader. That keeps early development focused on CPU bring-up and kernel structure instead of boot media details.

## Current implemented areas

- bootstrap into 64-bit long mode
- VGA text console
- hardware cursor updates
- exception and IRQ setup
- PIT timer
- keyboard input
- shell input/editing
- COM1 serial boot logging

## Intentionally unfinished areas

- physical memory manager
- kernel heap
- scheduler
- user mode / syscalls
- storage and filesystems
- generalized driver framework

## Design direction

The design goal is to keep the codebase small enough that a reader can understand the high-level structure quickly:
- boot and mode switch
- interrupt plumbing
- text console
- shell loop
- debugging path

Anything that makes the teaching shape worse should be treated carefully, even if it would make a larger downstream OS more capable.
