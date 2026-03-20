# tiny64os Roadmap

A living checklist for keeping `tiny64os` small, teachable, and useful as a modern x86_64 hobby-OS example.

## Current status

Implemented so far:
- GRUB / Multiboot2 boot flow
- 64-bit long mode entry
- VGA text console output
- VGA hardware cursor tracking
- IDT setup and basic exception handling
- Better exception diagnostics (vector, error code, RIP, CS, RFLAGS, CR2 for page faults)
- PIC remap
- PIT timer running in the background
- PS/2 keyboard IRQ handling
- COM1 serial logging for boot diagnostics
- Shell prompt with history and in-line editing
- Ctrl-C line cancellation

Current shell commands:
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

## v0.1 RC checklist

This repo is meant to represent a clean public `v0.1`-style educational release.

- [x] Boots reliably in QEMU from ISO
- [x] GRUB menu is sane
- [x] 64-bit long mode entry works
- [x] VGA text console works
- [x] Hardware cursor tracks correctly
- [x] Timer and keyboard IRQs are stable
- [x] Exception diagnostics are readable
- [x] Serial boot logs work
- [x] Shell editing is usable
- [x] Shell history / recall works
- [x] README matches current behavior
- [x] Architecture / file-layout doc exists
- [x] Roadmap exists
- [ ] Fresh-machine verification against the documented Debian install path

## Near-term priorities

### 1. Stability and teachability
- Run and document a final fresh-machine verification
- Keep build and run instructions exact and minimal
- Favor clarity over feature sprawl

### 2. Logging and debugging
- Split VGA output and serial logging more cleanly
- Add explicit kernel log helpers instead of mirroring all print output
- Add more structured panic / fault dumps
- Consider QEMU-friendly debug output helpers

### 3. Input and console improvements
- Better wrapped-line behavior for advanced editing cases
- Tabs / control character handling polish
- Optional split between log area and input area
- Keyboard modifiers beyond Shift/Ctrl

## Out-of-scope for the teaching version

These are interesting, but they risk making `tiny64os` harder to grasp quickly:
- full physical memory management stack
- kernel heap and allocator evolution
- scheduler / multitasking
- user mode / syscalls
- storage stack and filesystem work

Those are better suited for a larger downstream OS project.

## Nice-to-have commands
- `meminfo`
- `history`
- `time`
- paging debug helpers

## Notes
- Prefer small, testable features.
- If a change affects IRQ behavior, verify timer and keyboard both still work before committing.
- Be careful about doing heavy console or serial work directly inside IRQ handlers.
- This repo should stay curated and beginner-friendly rather than trying to become a full OS on its own.
