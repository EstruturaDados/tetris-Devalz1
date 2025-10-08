#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// ===============================
//  Gerenciador de Peças
//  - Fila circular (capacidade 5)
//  - Pilha linear (capacidade 3)
//  Operações: jogar, reservar, usar reservada, trocar 1x1, trocar 3x3, exibir
//  Autor: Curadoria de TI
// ===============================

// -------------------------------
//        Definições
// -------------------------------
#define CAP_FILA 5
#define CAP_PILHA 3

typedef struct {
    char nome;   // 'I', 'O', 'T', 'L'
    int id;      // ordem de criação
} Peca;

// -------------------------------
//        Fila circular
// -------------------------------
typedef struct {
    Peca dados[CAP_FILA];
    int frente; // índice do primeiro elemento
    int tras;   // índice da próxima posição de inserção
    int tamanho;// quantidade atual de elementos
} Fila;

void fila_init(Fila *f) {
    f->frente = 0;
    f->tras = 0;
    f->tamanho = 0;
}

bool fila_cheia(Fila *f) { return f->tamanho == CAP_FILA; }
bool fila_vazia(Fila *f) { return f->tamanho == 0; }

bool fila_enqueue(Fila *f, Peca p) {
    if (fila_cheia(f)) return false;
    f->dados[f->tras] = p;
    f->tras = (f->tras + 1) % CAP_FILA;
    f->tamanho++;
    return true;
}

bool fila_dequeue(Fila *f, Peca *out) {
    if (fila_vazia(f)) return false;
    if (out) *out = f->dados[f->frente];
    f->frente = (f->frente + 1) % CAP_FILA;
    f->tamanho--;
    return true;
}

// Acesso somente-leitura ao i-ésimo elemento lógico (0 = frente)
Peca fila_get(Fila *f, int i) {
    int idx = (f->frente + i) % CAP_FILA;
    return f->dados[idx];
}

// Atribui no i-ésimo elemento lógico (0 = frente)
void fila_set(Fila *f, int i, Peca p) {
    int idx = (f->frente + i) % CAP_FILA;
    f->dados[idx] = p;
}

// -------------------------------
//            Pilha
// -------------------------------
typedef struct {
    Peca dados[CAP_PILHA];
    int topo; // -1 vazio, 0..CAP_PILHA-1 ocupado
} Pilha;

void pilha_init(Pilha *p) { p->topo = -1; }

bool pilha_vazia(Pilha *p) { return p->topo == -1; }
bool pilha_cheia(Pilha *p) { return p->topo == CAP_PILHA - 1; }

bool pilha_push(Pilha *p, Peca x) {
    if (pilha_cheia(p)) return false;
    p->dados[++p->topo] = x;
    return true;
}

bool pilha_pop(Pilha *p, Peca *out) {
    if (pilha_vazia(p)) return false;
    if (out) *out = p->dados[p->topo];
    p->topo--;
    return true;
}

bool pilha_top(Pilha *p, Peca *out) {
    if (pilha_vazia(p)) return false;
    if (out) *out = p->dados[p->topo];
    return true;
}

// -------------------------------
//     Geração de peças (aleatório)
// -------------------------------
Peca gerarPeca(void) {
    static int prox_id = 0; // contador único
    const char tipos[] = {'I', 'O', 'T', 'L'};
    int idx = rand() % 4;
    Peca p = { .nome = tipos[idx], .id = prox_id++ };
    return p;
}

// -------------------------------
//     Exibição amigável no console
// -------------------------------
void exibir_estado(Fila *f, Pilha *p) {
    printf("\n================== ESTADO ATUAL ==================\n");
    printf("Fila de peças\t");
    for (int i = 0; i < f->tamanho; ++i) {
        Peca x = fila_get(f, i);
        printf("[%c %d] ", x.nome, x.id);
    }
    printf("\n");

    printf("Pilha de reserva\t(Topo -> base): ");
    for (int i = p->topo; i >= 0; --i) {
        printf("[%c %d] ", p->dados[i].nome, p->dados[i].id);
    }
    printf("\n===================================================\n\n");
}

void exibir_menu(void) {
    printf("Opções disponíveis:\n\n");
    printf("Código\tAção\n");
    printf("1\tJogar peça da frente da fila\n");
    printf("2\tEnviar peça da fila para a pilha de reserva\n");
    printf("3\tUsar peça da pilha de reserva\n");
    printf("4\tTrocar peça da frente da fila com o topo da pilha\n");
    printf("5\tTrocar os 3 primeiros da fila com as 3 peças da pilha\n");
    printf("6\tVisualizar estado atual (fila e pilha)\n");
    printf("0\tSair\n\n");
}

// -------------------------------
//     Operações de alto nível
// -------------------------------
void acao_jogar(Fila *f) {
    Peca removida;
    if (!fila_dequeue(f, &removida)) {
        printf("Ação: fila vazia. Nada a jogar.\n");
        return;
    }
    printf("Ação: jogou a peça da frente da fila [%c %d].\n", removida.nome, removida.id);
    // mantém a fila cheia quando possível
    Peca nova = gerarPeca();
    if (fila_enqueue(f, nova)) {
        printf("Nova peça gerada e colocada na fila: [%c %d].\n", nova.nome, nova.id);
    }
}

void acao_reservar(Fila *f, Pilha *p) {
    if (pilha_cheia(p)) {
        printf("Ação: pilha cheia. Não foi possível reservar a peça.\n");
        return;
    }
    Peca frente;
    if (!fila_dequeue(f, &frente)) {
        printf("Ação: fila vazia. Nada para reservar.\n");
        return;
    }
    pilha_push(p, frente);
    printf("Ação: peça [%c %d] enviada para a pilha de reserva.\n", frente.nome, frente.id);
    // mantém a fila cheia quando possível
    Peca nova = gerarPeca();
    if (fila_enqueue(f, nova)) {
        printf("Nova peça gerada e colocada na fila: [%c %d].\n", nova.nome, nova.id);
    }
}

void acao_usar_reservada(Pilha *p) {
    Peca usada;
    if (!pilha_pop(p, &usada)) {
        printf("Ação: pilha vazia. Não há peça reservada para usar.\n");
        return;
    }
    printf("Ação: usou a peça reservada do topo [%c %d].\n", usada.nome, usada.id);
    // Observação: não gera nova peça aqui; a regra de "manter a fila cheia" vale para remoções/transferências da fila.
}

void acao_trocar_atual(Fila *f, Pilha *p) {
    if (pilha_vazia(p)) {
        printf("Ação: pilha vazia. Não é possível trocar com a fila.\n");
        return;
    }
    if (fila_vazia(f)) {
        printf("Ação: fila vazia. Nada para trocar.\n");
        return;
    }
    // obter frente e topo
    Peca frente = fila_get(f, 0);
    Peca topo; pilha_top(p, &topo);
    // efetuar troca
    fila_set(f, 0, topo);
    p->dados[p->topo] = frente; // substitui topo pela antiga frente
    printf("Ação: troca realizada entre frente da fila [%c %d] e topo da pilha [%c %d].\n",
           frente.nome, frente.id, topo.nome, topo.id);
}

void acao_trocar_multiplas(Fila *f, Pilha *p) {
    if (p->topo + 1 < 3) {
        printf("Ação: pilha não possui 3 peças. Troca múltipla não realizada.\n");
        return;
    }
    if (f->tamanho < 3) {
        printf("Ação: fila não possui 3 peças. Troca múltipla não realizada.\n");
        return;
    }
    // Salva 3 primeiras da fila
    Peca q3[3];
    for (int i = 0; i < 3; ++i) q3[i] = fila_get(f, i);

    // Salva topo->base da pilha (3 itens)
    Peca s3[3];
    for (int i = 0; i < 3; ++i) s3[i] = p->dados[p->topo - i];

    // Escreve na fila: s3[0..2] (na mesma ordem topo->base)
    for (int i = 0; i < 3; ++i) fila_set(f, i, s3[i]);

    // Escreve na pilha: q3, respeitando LIFO (novo topo deve ser q3[2])
    p->dados[p->topo]     = q3[2];
    p->dados[p->topo - 1] = q3[1];
    p->dados[p->topo - 2] = q3[0];

    printf("Ação: troca realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
}

// -------------------------------
//                Main
// -------------------------------
int main(void) {
    // Inicialização do RNG e estruturas
    srand((unsigned)time(NULL));

    Fila fila;
    Pilha pilha;
    fila_init(&fila);
    pilha_init(&pilha);

    // Preenche a fila com 5 peças iniciais
    while (!fila_cheia(&fila)) {
        fila_enqueue(&fila, gerarPeca());
    }

    int opcao;
    bool executando = true;

    printf("=== Gerenciador de Peças (Fila 5 / Pilha 3) ===\n");
    exibir_estado(&fila, &pilha);

    while (executando) {
        exibir_menu();
        printf("Opção escolhida: ");
        if (scanf("%d", &opcao) != 1) {
            // consumo de entrada inválida
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Entrada inválida. Tente novamente.\n\n");
            continue;
        }

        switch (opcao) {
            case 1:
                acao_jogar(&fila);
                exibir_estado(&fila, &pilha);
                break;
            case 2:
                acao_reservar(&fila, &pilha);
                exibir_estado(&fila, &pilha);
                break;
            case 3:
                acao_usar_reservada(&pilha);
                exibir_estado(&fila, &pilha);
                break;
            case 4:
                acao_trocar_atual(&fila, &pilha);
                exibir_estado(&fila, &pilha);
                break;
            case 5:
                acao_trocar_multiplas(&fila, &pilha);
                exibir_estado(&fila, &pilha);
                break;
            case 6:
                exibir_estado(&fila, &pilha);
                break;
            case 0:
                executando = false;
                printf("Encerrando o programa. Peças removidas não retornam ao jogo.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    }

    return 0;
}
