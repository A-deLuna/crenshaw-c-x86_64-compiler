        extern printf
        section .data
fmt:    db "x: %d", 10, 0
x:      dq 5

        section .text
        global main
main:
        push rbp
        mov rax , 0; for x = 0
        dec rax
        ;mov qword [x], rax 
        mov rax, 10; ; x <= 10
        push rax
L00:
        mov rax, qword [x]
        inc rax
        mov qword[x], rax
        cmp rax, qword [rsp] 
        jg L01

        mov rdi, fmt
        mov rsi, qword [x]
        xor rax, rax
        call printf
        jmp L00

L01:
        add rsp, 8
        pop rbp
        xor rax, rax
        ret
