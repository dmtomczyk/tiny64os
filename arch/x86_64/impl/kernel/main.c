#include "idt.h"
#include "keyboard.h"
#include "pic.h"
#include "pit.h"
#include "print.h"
#include "serial.h"
#include "shell.h"

void kernel_main(void)
{
    serial_init();
    print_enable_serial_mirror();

    print_clear();
    print_str("[boot] tiny64os kernel starting\n");
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to tiny64os\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    print_str("Booted into 64-bit long mode. VGA text console is online.\n");
    print_str("Keyboard layout: ");
    print_str(keyboard_layout());
    print_str("\n");

    print_str("About to initialize IDT...\n");
    idt_init();

    print_str("Masking PIC...\n");
    pic_mask_all();

    print_str("Remapping PIC...\n");
    pic_remap(32, 40);

    print_str("Unmasking timer + keyboard IRQs...\n");
    pic_unmask_irq(0);
    pic_unmask_irq(1);

    print_str("Initializing PIT...\n");
    pit_init(100);

    print_str("Enabling interrupts...\n");
    print_disable_serial_mirror();
    shell_init();
    __asm__ volatile ("sti");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
