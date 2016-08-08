        extern printf
        section .data
fmt:    db "A: %d", 10, 0
fmt2:   db "[rsp]: %d", 10, 0

        section .bss
A:      resq 1
        section .text
        global main
main:
        push rbp
        ;call print

        mov rax, 0
        mov [A], rax

L00:   
        mov rax, [A]
        push rax
        mov rax, 9
        cmp rax, [rsp]
        setg al
        add rsp, 8
        cmp rax, 0
        jz L01
        mov rbx, [rsp]
        mov rax, [A]
        push rax
        mov rax, 1
        pop rbx
        add rax, rbx
        mov [A], rax
        jmp L00

L01:
        call print

        pop rbp
        xor rax, rax
        mov rax, 0
        ret

print: 
        push rbp
        mov rbp, rsp
        mov rdi, fmt
        mov rsi, qword [A]
        xor rax, rax
        call printf
        pop rbp
        ret

print2: 
        push rbp
        mov rbp, rsp
        mov rdi, fmt2
        mov rsi, rax 
        xor rax, rax
        call printf
        pop rbp
        ret

