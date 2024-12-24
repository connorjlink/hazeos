.section .data
    .align 4
idt:
    .skip 256 * 8  # Allocate space for 256 entries, each 8 bytes (64 bits)

.section .bss
    .align 4
idt_descriptor:
    .space 6        # IDT descriptor to load into IDTR

.section .text
    .global load_idt
    .extern exception_handler

# Macro to create an IDT entry
.macro set_idt_entry index, handler
    # Set offset_low and offset_high with handler address
    movl $\handler, %eax
    movw %ax, idt + \index * 8       # Lower 16 bits of handler address
    movw $0x08, idt + \index * 8 + 2 # Code segment selector (typically 0x08)
    movb $0, idt + \index * 8 + 4    # Zero byte
    movb $0x8E, idt + \index * 8 + 5 # Type/attribute (interrupt gate, present, ring 0)
    shr $16, %eax
    movw %ax, idt + \index * 8 + 6   # Upper 16 bits of handler address
.endm

# Initialize the IDT with stub handlers for each exception
load_idt:
    # Set each entry to point to the common exception handler stub
    .set i, 0
1:
    set_idt_entry i, exception_stub
    .set i, i + 1
    cmpl $32, i
    jne 1b

    # Set up the IDT descriptor (limit and base address)
    movw $256 * 8 - 1, idt_descriptor      # IDT limit
    movl $idt, idt_descriptor + 2          # IDT base address
    lidt idt_descriptor                    # Load IDT into IDTR
    ret

# Stub for handling exceptions
exception_stub:
    # Push the interrupt number (stored in EAX) onto the stack
    push %eax
    call exception_handler  # Call the C exception handler
    add $4, %esp            # Clean up the stack
    iret                    # Return from interrupt

# IDT descriptor format for the `lidt` instruction
.section .data
    .align 4
