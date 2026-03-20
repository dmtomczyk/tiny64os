ARCH := x86_64
BUILD_DIR := build/$(ARCH)
DIST_DIR := dist/$(ARCH)
ISO_DIR := targets/$(ARCH)/iso
LINKER_SCRIPT := targets/$(ARCH)/linker.ld
KERNEL_BIN := $(DIST_DIR)/kernel.bin
KERNEL_ISO := $(DIST_DIR)/kernel.iso

C_SOURCE_FILES := $(shell find arch/$(ARCH)/impl -name '*.c')
ASM_SOURCE_FILES := $(shell find arch/$(ARCH)/impl -name '*.asm')
OBJECT_FILES := \
	$(patsubst arch/$(ARCH)/impl/%.c,$(BUILD_DIR)/%.o,$(C_SOURCE_FILES)) \
	$(patsubst arch/$(ARCH)/impl/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCE_FILES))

ifeq ($(shell command -v x86_64-elf-gcc 2>/dev/null),)
	CC := gcc
	LD := ld
	CFLAGS := -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -Wall -Wextra -std=c11
	ASFLAGS := -m64 -c -x assembler-with-cpp
	LDFLAGS := -m elf_x86_64 -nostdlib -z max-page-size=0x1000 -T $(LINKER_SCRIPT)
else
	CC := x86_64-elf-gcc
	LD := x86_64-elf-ld
	CFLAGS := -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -Wall -Wextra -std=c11
	ASFLAGS := -c -x assembler-with-cpp
	LDFLAGS := -nostdlib -z max-page-size=0x1000 -T $(LINKER_SCRIPT)
endif

GRUB_FILE := $(shell command -v grub-file 2>/dev/null)
ISO_CREATOR := $(shell command -v xorriso 2>/dev/null || command -v mkisofs 2>/dev/null || command -v genisoimage 2>/dev/null)

.PHONY: all build-x86_64 clean check-multiboot
all: build-x86_64

$(BUILD_DIR)/%.o: arch/$(ARCH)/impl/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I arch/$(ARCH)/intf -c $< -o $@

$(BUILD_DIR)/%.o: arch/$(ARCH)/impl/%.asm
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $< -o $@

$(KERNEL_BIN): $(OBJECT_FILES)
	mkdir -p $(DIST_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECT_FILES)
	cp $@ $(ISO_DIR)/boot/kernel.bin

check-multiboot: $(KERNEL_BIN)
ifndef GRUB_FILE
	@echo "grub-file is not installed; skipping multiboot validation"
else
	$(GRUB_FILE) --is-x86-multiboot2 $(KERNEL_BIN)
endif

build-x86_64: $(KERNEL_BIN) check-multiboot
ifdef ISO_CREATOR
	grub-mkrescue -o $(KERNEL_ISO) $(ISO_DIR)
else
	@echo "ISO tool (xorriso/mkisofs/genisoimage) not installed; built kernel.bin but skipped kernel.iso"
endif

clean:
	rm -rf build dist
