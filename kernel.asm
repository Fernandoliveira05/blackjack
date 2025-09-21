;;kernel.asm
bits 32     ;nasm directive - 32 bits
section .text
    ;multiboot spec
    align 4
    dd 0x1BADB002
    dd 0x00 
    dd - (0x1BADB002 + 0x00)

    

global start 
extern kmain ;kmain é o arquivo .c

start:
    cli     ;interrupção de blocos
    mov esp, stack_space    ;define o ponto do stack 
    call kmain 
    hlt     ;para a CPU

section .bss 
resb 8192    ;8kb para o stack
stack_space