#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // Incluindo para o uso de fgets

// Estrutura para armazenar os pacotes
typedef struct {
    int numero_pacote;
    int tamanho;
    unsigned char* dados;
} Pacote;

// Função de comparação para o qsort
int comparar_pacotes(const void* a, const void* b) {
    return ((Pacote*)a)->numero_pacote - ((Pacote*)b)->numero_pacote;
}

// Função para ler o arquivo de entrada
Pacote* ler_entrada(FILE* entrada, int* total_pacotes, int* limite_processamento) {
    if (fscanf(entrada, "%d %d", total_pacotes, limite_processamento) != 2) {
        fprintf(stderr, "Erro ao ler o número total de pacotes e limite de processamento.\n");
        return NULL;
    }

    Pacote* pacotes = (Pacote*)malloc((*total_pacotes) * sizeof(Pacote));
    if (!pacotes) {
        fprintf(stderr, "Erro ao alocar memória para os pacotes.\n");
        return NULL;
    }

    for (int i = 0; i < *total_pacotes; i++) {
        if (fscanf(entrada, "%d %d", &pacotes[i].numero_pacote, &pacotes[i].tamanho) != 2) {
            fprintf(stderr, "Erro ao ler o cabeçalho do pacote %d.\n", i);
            free(pacotes);
            return NULL;
        }

        pacotes[i].dados = (unsigned char*)malloc(pacotes[i].tamanho * sizeof(unsigned char));
        if (!pacotes[i].dados) {
            fprintf(stderr, "Erro ao alocar memória para os dados do pacote %d.\n", i);
            for (int j = 0; j < i; j++) free(pacotes[j].dados);
            free(pacotes);
            return NULL;
        }

        for (int j = 0; j < pacotes[i].tamanho; j++) {
            int byte;
            if (fscanf(entrada, "%x", &byte) != 1) {
                fprintf(stderr, "Erro ao ler o byte %d do pacote %d.\n", j, i);
                for (int k = 0; k <= i; k++) free(pacotes[k].dados);
                free(pacotes);
                return NULL;
            }
            pacotes[i].dados[j] = (unsigned char)byte;
        }
    }
    return pacotes;
}

// Função para escrever no arquivo de saída
void escrever_saida(FILE* saida, Pacote* pacotes, int n) {
    int intervalo_atual = -1;
    for (int i = 0; i < n; i++) {
        int num_pacote = pacotes[i].numero_pacote;
        int intervalo_atual_pacote = -1;

        if (num_pacote == 0) intervalo_atual_pacote = 0;
        else if (num_pacote <= 2) intervalo_atual_pacote = 1;
        else if (num_pacote <= 27) intervalo_atual_pacote = 2;
        else if (num_pacote <= 30) intervalo_atual_pacote = 3;
        else if (num_pacote <= 78) intervalo_atual_pacote = 4;
        else if (num_pacote <= 135) intervalo_atual_pacote = 5;
        else if (num_pacote <= 279) intervalo_atual_pacote = 6;
        else if (num_pacote <= 288) intervalo_atual_pacote = 7;
        else if (num_pacote <= 553) intervalo_atual_pacote = 8;
        else if (num_pacote <= 1265) intervalo_atual_pacote = 9;
        else intervalo_atual_pacote = 10;

        if (intervalo_atual != intervalo_atual_pacote) {
            if (intervalo_atual != -1) fprintf(saida, "|\n");
            intervalo_atual = intervalo_atual_pacote;
        }

        fprintf(saida, "|");
        for (int j = 0; j < pacotes[i].tamanho; j++) {
            fprintf(saida, "%02X", pacotes[i].dados[j]);
            if (j < pacotes[i].tamanho - 1) fprintf(saida, ",");
        }
        fprintf(saida, " ");
    }
    fprintf(saida, "|\n");
}

// Função principal
int main(int argc, char* argv[]) {
    clock_t inicio = clock();

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE* saida = fopen(argv[2], "w");
    if (!saida) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(entrada);
        return 1;
    }

    int total_pacotes, limite_processamento;
    Pacote* pacotes = ler_entrada(entrada, &total_pacotes, &limite_processamento);
    if (!pacotes) {
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    qsort(pacotes, total_pacotes, sizeof(Pacote), comparar_pacotes);
    escrever_saida(saida, pacotes, total_pacotes);

    for (int i = 0; i < total_pacotes; i++) {
        free(pacotes[i].dados);
    }
    free(pacotes);

    fclose(entrada);
    fclose(saida);

    clock_t fim = clock();
    double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de execução: %.6f segundos\n", tempo_execucao);

    return 0;
}
