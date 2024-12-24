.section .text
.global set_cursor
.type set_cursor, @function

.global disable_cursor
.type disable_cursor, @function
disable_cursor:
    pushf
    pushl %eax
    pushl %edx
    
    movw $0x3D4, %dx 
    movb $0xA, %al
    outb %al, %dx 
    incw %dx
    mov $0x20, %al
    outb %al, %dx

    popl %edx
    popl %eax
    popf
    ret

set_cursor:

