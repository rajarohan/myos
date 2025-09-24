// boot.S - Multiboot header and entry point
.set ALIGN,    1<<0             // align loaded modules on page boundaries
.set MEMINFO,  1<<1             // provide memory map
.set FLAGS,    ALIGN | MEMINFO  // this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       // 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) // checksum of above, to prove we are multiboot

// Multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

// Stack for our kernel
.section .bss
.align 16
stack_bottom:
.skip 16384 // 16 KiB stack
stack_top:

// Entry point
.section .text
.global _start
.type _start, @function
_start:
    // Set up the stack
    mov $stack_top, %esp
    
    // Call the kernel main function
    call kmain
    
    // In case kmain returns, halt the system
    cli
1:  hlt
    jmp 1b

// Set the size of the _start symbol to the current location '.' minus its start.
.size _start, . - _start