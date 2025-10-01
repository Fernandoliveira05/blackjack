# 🃏 FerBet - Blackjack no Kernel

Este projeto implementa um **jogo de Blackjack** (21) rodando em um **kernel simples**, desenvolvido em **C** e **Assembly x86**.  
O objetivo é demonstrar conceitos de **sistemas operacionais**, **desenvolvimento bare-metal** e **interação direta com hardware** (vídeo e teclado) através de um jogo clássico.

---

## Assista ao vídeo:
- A imagem abaixo te redireciona para o vídeo explicativo do projeto:
<div align="center">
  <a href="https://www.youtube.com/watch?v=FdRqLgt-Rgo">
    <img src="https://sharkpokerreviews.com.br/blog/wp-content/uploads/2020/07/como-jogar-blackjack-21.jpg" alt="Vídeo do Projeto ULA Completa">
  </a>
</div>

---

## Sobre o Projeto

O FerBet é um mini-jogo de Blackjack que roda diretamente sobre o processador, sem um sistema operacional intermediário.  
Ele inicializa um kernel em **modo protegido de 32 bits**, exibe texto na tela utilizando o **buffer de vídeo** (`0xb8000`) e lê o **teclado PS/2** via instruções Assembly (`inb`).  

O jogador pode:  
- **Hit (H):** Pegar uma nova carta  
- **Stand (S):** Parar e encerrar a jogada  
- O adversário joga automaticamente com base em regras predefinidas  

---

## Bootloader, o que é?

Um **bootloader** é um programa que é executado sempre que um dispositivo é ligado ou reiniciado.  
Ele é o **intermediário entre o hardware e o sistema operacional**, garantindo que o dispositivo inicie corretamente.  

De forma simples, o bootloader prepara o terreno para que o sistema operacional seja carregado e executado.  
No nosso caso, o bootloader:

- Configura o **modo protegido de 32 bits**  
- Define o **stack** inicial  
- Carrega o **microkernel** para a memória  
- Transfere o controle para a função principal (`kmain`) do kernel  

---


---

## Estrutura do Projeto

projeto-blackjack
├── kernel.asm # Inicialização do kernel em Assembly
├── kernel.c # Lógica principal do jogo em C
└── README.md # Este arquivo


### kernel.asm
Responsável por:
- Configurar o **stack**  
- Entrar em modo protegido  
- Chamar a função `kmain` definida em C  
- Encerrar com `hlt`  

### kernel.c
Contém toda a lógica do jogo:
- Estruturas de **Jogador** e **Carta**  
- Funções de **print em tela** (via memória de vídeo)  
- Função de **entrada do teclado** (via instruções de I/O em Assembly inline)  
- Implementação do **Blackjack** (cartas, soma, checagem de vitória, regras)  
- Loop principal (`kmain`) exibindo tela inicial e chamando o jogo  

---

## Como Rodar

### 1. Compile o Assembly
```bash
nasm -f elf32 kernel.asm -o kasm.o
```
### 2. Complie o C

```bash
gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
    -fno-stack-check -fno-pie -ffreestanding -nostartfiles \
    -nodefaultlibs -Wall -Wextra -c kernel.c -o kc.o
```
### 3.Linke tudo
```bash
ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o
```

### 4. Execute no QEmu 
```bash
qemu-system-i386 -kernel kernel
```
---
### Requisitos

- Compilador GCC com suporte a 32 bits (-m32)

- NASM (assembler)

- QEMU (emulador para rodar o kernel)

- Sistema Linux ou WSL (Windows Subsystem for Linux)

---


## Como Jogar

Na tela inicial são mostradas as regras e controles:

- **H** → Hit (pegar carta)  
- **S** → Stand (parar)  
- **ENTER** → Confirmar / Continuar  

### Objetivo
- Chegar o mais próximo possível de **21** sem ultrapassar  
- Vencer o adversário, que também possui um valor oculto  

---

## Fluxo do Jogo
```mermaid
flowchart TD
    A[Bootloader (kernel.asm)]
    B[Configura stack e entra em modo protegido]
    C[kmain (kernel.c)]
    D[Exibe tela inicial]
    E[Jogo Blackjack]
    F[Adiciona carta]
    G[Finaliza jogo]
    H[Mostra resultado final]

    A --> B
    B --> C
    C --> D
    D --> E
    E -->|H = Hit| F
    E -->|S = Stand| G
    F --> E
    G --> H
    H --> D
```

---


## Autoria

Projeto desenvolvido como estudo de baixo nível e sistemas operacionais:

Estrutura de boot com Assembly

Kernel mínimo em C

Execução direta no QEMU

Fundamentos de microkernel

---


