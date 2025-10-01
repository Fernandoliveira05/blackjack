typedef struct {
    char *nome;
    int valor;
} Carta;

typedef struct {
    Carta mao[11];
    int num_cartas;
    int total;
} Jogador;

unsigned int seed = 1;

int pseudo_rand() {
    seed = (seed * 1103515245 + 12345) % 13;
    return seed; 
}

void clear_screen(char *vidptr) {
    unsigned int j = 0;
    while(j < 80*25*2) {
        vidptr[j] = ' ';
        vidptr[j+1] = 0x07;
        j += 2;
    }
}

void print_str(const char *str, char *vidptr, unsigned int *i) {
    unsigned int j = 0;
    while(str[j] != '\0') {
        vidptr[*i] = str[j];
        vidptr[*i + 1] = 0x07;
        j++;
        *i += 2;
    }
    *i = (*i / 160 + 1) * 160; 
}

void adicionar_carta(Jogador *jogador, int valor) {
    if (jogador->num_cartas < 11) {
        jogador->mao[jogador->num_cartas].valor = valor;
        jogador->num_cartas++;
        jogador->total += valor;
    }
}

// Checar vitória
int checar_vitoria(Jogador *jogador) {
    if(jogador->total > 21) return -1; // perdeu
    if(jogador->total == 21) return 1; // ganhou
    return 0; // continua
}

// Mostrar mão do jogador
void mostrar_mao(Jogador *jogador, char *vidptr, unsigned int *i, const char *nome) {
    print_str(nome, vidptr, i);
    print_str(": ", vidptr, i);
    
    for(int j = 0; j < jogador->num_cartas; j++) {
        char buf[4];
        int valor = jogador->mao[j].valor;
        
        if (valor >= 1 && valor <= 9) {
            buf[0] = '0' + valor;
        } else if (valor == 10) {
            buf[0] = '1';
            buf[1] = '0';
            buf[2] = '\0';
            print_str(buf, vidptr, i);
            if (j < jogador->num_cartas - 1) {
                print_str(" + ", vidptr, i);
            }
            continue;
        } else {
            buf[0] = '?';
        }
        buf[1] = '\0';
        
        print_str(buf, vidptr, i);
        if (j < jogador->num_cartas - 1) {
            print_str(" + ", vidptr, i);
        }
    }
    
    print_str(" = ", vidptr, i);
    
    if (jogador->total <= 9) {
        char total_buf[2];
        total_buf[0] = '0' + jogador->total;
        total_buf[1] = '\0';
        print_str(total_buf, vidptr, i);
    } else if (jogador->total <= 21) {
        char total_buf[3];
        total_buf[0] = '0' + (jogador->total / 10);
        total_buf[1] = '0' + (jogador->total % 10);
        total_buf[2] = '\0';
        print_str(total_buf, vidptr, i);
    } else {
        print_str("ESTOURO!", vidptr, i);
    }
}

void mostrar_adversario_oculto(char *vidptr, unsigned int *i) {
    print_str("Adversario: [OCULTO] = ???", vidptr, i);
}

char ler_teclado() {
    unsigned char tecla = 0;
    
    while (1) {
        __asm__ __volatile__("inb $0x64, %%al" : "=a"(tecla));
        if (tecla & 0x01) {
            // Ler scancode
            __asm__ __volatile__("inb $0x60, %%al" : "=a"(tecla));
            
            if (tecla == 0x23) return 'H'; // H pressionado
            if (tecla == 0x1f) return 'S'; // S pressionado  
            if (tecla == 0x1c) return '\n'; // Enter
            
            if (tecla < 0x80) { 
                return 'H'; 
            }
        }
        
        for (volatile int i = 0; i < 1000; i++);
    }
}

char aguardar_tecla() {
    char tecla;
    do {
        tecla = ler_teclado();
    } while (tecla != 'H' && tecla != 'S' && tecla != '\n');
    return tecla;
}

void aguardar_enter() {
    char tecla;
    do {
        tecla = ler_teclado();
    } while (tecla != '\n');
}

void jogada_adversario(Jogador *adversario) {
    while (adversario->total < 17) {
        int carta = (pseudo_rand() % 10) + 1;
        adicionar_carta(adversario, carta);
        
        if (adversario->num_cartas >= 10) break;
    }
}

void mostrar_resultado(Jogador *jogador, Jogador *adversario, int resultado, char *vidptr) {
    unsigned int i = 0;
    
    for (volatile int delay = 0; delay < 1000000; delay++);
    
    clear_screen(vidptr);
    
    print_str("=== RESULTADO FINAL ===", vidptr, &i);
    print_str("", vidptr, &i);
    
    mostrar_mao(jogador, vidptr, &i, "VOCE");
    mostrar_mao(adversario, vidptr, &i, "ADVERSARIO");
    print_str("", vidptr, &i);
    
    // Mostrar resultado
    if (resultado == 1) {
        print_str("*** PARABENS! VOCE GANHOU! ***", vidptr, &i);
        print_str("Voce teve uma mao melhor que o adversario!", vidptr, &i);
    } else if (resultado == -1) {
        print_str("*** VOCE PERDEU! ***", vidptr, &i);
        if (jogador->total > 21) {
            print_str("Voce estourou o limite de 21!", vidptr, &i);
        } else {
            print_str("O adversario teve uma mao melhor!", vidptr, &i);
        }
    } else {
        print_str("*** EMPATE! ***", vidptr, &i);
        print_str("Voces tiveram o mesmo total!", vidptr, &i);
    }
    
    print_str("", vidptr, &i);
    print_str("Pressione ENTER para jogar novamente...", vidptr, &i);
    
    print_str("", vidptr, &i);
    print_str(">>> Aguardando ENTER... <<<", vidptr, &i);
    
    aguardar_enter();
}

// ===================== JOGO PRINCIPAL =====================
void jogar_blackjack() {
    char *vidptr = (char*)0xb8000;
    unsigned int i = 0;
    
    Jogador jogador = { .num_cartas = 0, .total = 0 };
    Jogador adversario = { .num_cartas = 0, .total = 0 };
    
    int carta1 = (pseudo_rand() % 10) + 1;
    int carta2 = (pseudo_rand() % 10) + 1;
    adicionar_carta(&jogador, carta1);
    adicionar_carta(&jogador, carta2);
    
    int adv_inicial = ((pseudo_rand() % 8) + 15);
    adversario.total = adv_inicial;
    adversario.num_cartas = 2; 
    
    clear_screen(vidptr);
    i = 0;
    
    print_str("=== BLACKJACK - NOVA PARTIDA ===", vidptr, &i);
    print_str("", vidptr, &i);
    
    while(1) {
        if (jogador.num_cartas > 2) {
            clear_screen(vidptr);
            i = 0;
            print_str("=== BLACKJACK ===", vidptr, &i);
            print_str("", vidptr, &i);
        }
        
        mostrar_mao(&jogador, vidptr, &i, "VOCE");
        mostrar_adversario_oculto(vidptr, &i);
        print_str("", vidptr, &i);
        
        if (checar_vitoria(&jogador) == -1) {
            clear_screen(vidptr);
            i = 0;
            print_str("=== ESTOURO! ===", vidptr, &i);
            print_str("", vidptr, &i);
            mostrar_resultado(&jogador, &adversario, -1, vidptr);
            return;
        }
        
        if (checar_vitoria(&jogador) == 1) {
            clear_screen(vidptr);
            i = 0;
            print_str("=== BLACKJACK! ===", vidptr, &i);
            print_str("", vidptr, &i);
            
            if (adversario.total == 21) {
                mostrar_resultado(&jogador, &adversario, 0, vidptr); // Empate
            } else {
                mostrar_resultado(&jogador, &adversario, 1, vidptr); // Jogador ganhou
            }
            return;
        }
        
        print_str("[H] Hit (pegar carta) ou [S] Stand (parar)?", vidptr, &i);
        print_str("", vidptr, &i); 
        char tecla = aguardar_tecla();
        
        if(tecla == 'H') {
            int nova_carta = (pseudo_rand() % 10) + 1;
            adicionar_carta(&jogador, nova_carta);
            
            clear_screen(vidptr);
            i = 0;
            print_str("=== CARTA ADICIONADA ===", vidptr, &i);
            print_str("", vidptr, &i);
        } else if(tecla == 'S') {
            clear_screen(vidptr);
            i = 0;
            print_str("=== FINALIZANDO JOGO ===", vidptr, &i);
            print_str("Voce escolheu parar!", vidptr, &i);
            print_str("", vidptr, &i);
            
            if (jogador.total > adversario.total) {
                mostrar_resultado(&jogador, &adversario, 1, vidptr); // Jogador ganhou
            } else if (jogador.total < adversario.total && adversario.total <= 21) {
                mostrar_resultado(&jogador, &adversario, -1, vidptr); // Adversário ganhou
            } else if (jogador.total == adversario.total) {
                mostrar_resultado(&jogador, &adversario, 0, vidptr); // Empate
            } else {
                adversario.total = 22; 
                mostrar_resultado(&jogador, &adversario, 1, vidptr);
            }
            return;
        }
    }
}

// ===================== KMAIN =====================
void kmain(void) {
    char *vidptr = (char*)0xb8000;
    unsigned int i = 0;
    
    // Tela inicial
    clear_screen(vidptr);
    print_str("=== FerBet - A Bet do bem! ===", vidptr, &i);
    print_str("", vidptr, &i);
    print_str("BLACKJACK vs ADVERSARIO", vidptr, &i);
    print_str("", vidptr, &i);
    print_str("REGRAS:", vidptr, &i);
    print_str("- Chegue o mais proximo de 21 sem estourar", vidptr, &i);
    print_str("- O adversario tambem joga (total oculto)", vidptr, &i);
    print_str("- Voce ganha se tiver mais que o adversario", vidptr, &i);
    print_str("- Voce perde se estourar 21 ou ficar abaixo", vidptr, &i);
    print_str("", vidptr, &i);
    print_str("CONTROLES:", vidptr, &i);
    print_str("- H = Hit (pegar carta)", vidptr, &i);
    print_str("- S = Stand (parar)", vidptr, &i);
    print_str("- ENTER = Confirmar/Continuar", vidptr, &i);
    print_str("", vidptr, &i);
    print_str("Pressione ENTER para comecar a jogar!", vidptr, &i);
    
    aguardar_enter();
    
    while(1) {
        jogar_blackjack();
    }
}