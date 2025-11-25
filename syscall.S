.text
.globl syscall3
.type syscall3, @function

syscall3:
    movq %rdi, %rax
    movq %rsi, %rdi
    movq %rdx, %rsi
    movq %rcx, %rdx
    syscall
    ret

.size syscall3, .-syscall3

.section .note.GNU-stack,"",@progbits
