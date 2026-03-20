.intel_syntax noprefix

/* https://www.gnu.org/software/grub/manual/multiboot2/html_node/Header-layout.html#Header-layout */
/* Multiboot2 header layout
 * Offset | Type | Field Name
 * 0      | u32  | magic
 * 4      | u32  | architecture
 * 8      | u32  | header_length
 * 12     | u32  | checksum
 * 16-XX  | u32  | tags
 */

.section .multiboot_header
.align 8
header_start:
    .set MULTIBOOT2_MAGIC, 0xe85250d6
    .set MULTIBOOT2_ARCHITECTURE_I386, 0
    .set MULTIBOOT2_HEADER_LENGTH, header_end - header_start

    .long MULTIBOOT2_MAGIC
    .long MULTIBOOT2_ARCHITECTURE_I386
    .long MULTIBOOT2_HEADER_LENGTH
    .long -(MULTIBOOT2_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + MULTIBOOT2_HEADER_LENGTH)

    /* end tag */
    .short 0
    .short 0
    .long 8
header_end:
