#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BYTES 512

// Estrutura para armazenar os pacotes
typedef struct Pacote {
    int numero;       // Número do pacote
    int tamanho;      // Tamanho do pacote
    unsigned char dados[MAX_BYTES]; // Dados do pacote (até 512 bytes)
    struct Pacote *prox; // Ponteiro para o próximo pacote na lista
} Pacote;

// Função para criar um novo pacote
Pacote* criar_pacote(int numero, int tamanho, unsigned char *dados) {
    Pacote *novo = (Pacote *)malloc(sizeof(Pacote));
    novo->numero = numero;
    novo->tamanho = tamanho;
    memcpy(novo->dados, dados, tamanho);
    novo->prox = NULL;
    return novo;
}

// Função para adicionar um pacote à lista em ordem crescente pelo número do pacote
void adicionar_pacote_ordenado(Pacote **lista, Pacote *novo) {
    if (*lista == NULL || (*lista)->numero > novo->numero) {
        novo->prox = *lista;
        *lista = novo;
        return;
    }
    Pacote *atual = *lista;
    while (atual->prox != NULL && atual->prox->numero < novo->numero) {
        atual = atual->prox;
    }
    novo->prox = atual->prox;
    atual->prox = novo;
}

// Função para exibir os dados parcialmente ordenados
void exibir_dados_parcial(Pacote *lista, int quantidade, FILE *saida) {
    int pacotes_processados = 0;
    fprintf(saida, "Dados processados:\n");
    while (lista != NULL && pacotes_processados < quantidade) {
        fprintf(saida, "%d |", lista->numero);
        for (int i = 0; i < lista->tamanho; i++) {
            fprintf(saida, "%02X", lista->dados[i]);
            if (i < lista->tamanho - 1) fprintf(saida, ",");
        }
        fprintf(saida, "| ");
        pacotes_processados++;
        lista = lista->prox;
    }
    fprintf(saida, "\n");
}

// Função para liberar a memória da lista
void liberar_lista(Pacote *lista) {
    while (lista != NULL) {
        Pacote *temp = lista;
        lista = lista->prox;
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Abrir os arquivos de entrada e saída
    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *saida = fopen(argv[2], "w");
    if (!saida) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(entrada);
        return 1;
    }

    int total_pacotes, quantidade_processamento;
    fscanf(entrada, "%d %d", &total_pacotes, &quantidade_processamento);

    Pacote *lista = NULL;

    // Ler os pacotes do arquivo de entrada
    for (int i = 0; i < total_pacotes; i++) {
        int numero, tamanho;
        fscanf(entrada, "%d %d", &numero, &tamanho);
        unsigned char dados[MAX_BYTES];
        for (int j = 0; j < tamanho; j++) {
            int byte;
            fscanf(entrada, "%x", &byte);
            dados[j] = (unsigned char)byte;
        }
        Pacote *novo = criar_pacote(numero, tamanho, dados);
        adicionar_pacote_ordenado(&lista, novo);
    }

    // Processar os dados parcialmente ordenados
    exibir_dados_parcial(lista, quantidade_processamento, saida);

    // Liberar a memória e fechar os arquivos
    liberar_lista(lista);
    fclose(entrada);
    fclose(saida);

    return 0;
}
