.section .data
scan_code_table:
    .byte 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0
    .byte 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0
    .byte 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\'
    .byte 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '


.section .text
.global poll_keyboard
.type poll_keyboard, @function

poll_keyboard:
    inb $0x64, %al                     # read status register
    testb $1, %al                      # is data ready?
    jnz poll 
    xorb %al, %al
    ret
poll:
    inb $0x60, %al                     # read scan code
    movzx %al, %eax
    cmp 58, %eax
    ja not_ascii
    movb scan_code_table(, %eax, 1), %al # convert to ASCII
    ret
not_ascii:
    mov $'?', %al
    ret
