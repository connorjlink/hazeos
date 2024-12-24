.section .text
.global shutdown
.type shutdown, @function
shutdown:
    movw $0x5301, %ax
    xorw %bx, %bx
    int $0x15

    movw $0x530e, %ax
    xorw %bx, %bx
    movw $0x0102, %cx
    int $0x15

    movw $0x5307, %ax
    movw $0x0001, %bx
    movw $0x0003, %cx
    int $0x15
    