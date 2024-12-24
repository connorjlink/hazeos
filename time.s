.section .text
.global timestamp
.type timestamp, @function
timestamp:
    pushl %edx
    rdtsc
    popl %edx
    ret
