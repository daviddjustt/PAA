#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>  // Para usar a função fabs (valor absoluto)

// Estrutura para armazenar dados do contêiner
typedef struct {
    char codigo[20];
    char cnpj[20];
    int peso;
    int ordem; // Ordem de leitura do contêiner
} Container;

// Estrutura para armazenar os resultados das buscas de peso
typedef struct {
    char codigo[20];
    int pesoBusca;
    float diferencaPorcentagem; // Diferença percentual entre os pesos
    int ordem; // Ordem de leitura do contêiner
} BuscaPeso;

// Função para mesclar duas sublistas
void merge(Container arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    Container *L = (Container *)malloc(n1 * sizeof(Container));
    Container *R = (Container *)malloc(n2 * sizeof(Container));

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (strcmp(L[i].codigo, R[j].codigo) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Função para ordenar usando Merge Sort
void mergeSort(Container arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Função de comparação para ordenar o vetor de buscas de peso
int compararPorDiferenca(const void* a, const void* b) {
    BuscaPeso* buscaA = (BuscaPeso*)a;
    BuscaPeso* buscaB = (BuscaPeso*)b;
    return (buscaB->diferencaPorcentagem - buscaA->diferencaPorcentagem) * 100; // Ordena de forma decrescente
}

// Função para calcular a diferença percentual e realizar a busca
void buscaPorPeso(Container arr[], int n, char* codigoBusca, int pesoBusca, FILE* output, BuscaPeso* buscasPeso, int* contBusca) {
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i].codigo, codigoBusca) == 0) {
            // Calcular a diferença percentual
            float diferenca = fabs(arr[i].peso - pesoBusca); // Diferença absoluta
            float diferencaPorcentagem = (diferenca / arr[i].peso) * 100;

            // Debugging: Imprimir as diferenças calculadas no arquivo de saída
            fprintf(output, "Codigo: %s, Peso: %d, Peso Busca: %d, Diferença: %.2f%%\n", arr[i].codigo, arr[i].peso, pesoBusca, diferencaPorcentagem);

            // Se a diferença for menor ou igual a 10%, desconsidera a busca
            if (diferencaPorcentagem <= 10) {
                // Armazenar no vetor
                strcpy(buscasPeso[*contBusca].codigo, codigoBusca);
                buscasPeso[*contBusca].pesoBusca = pesoBusca;
                buscasPeso[*contBusca].diferencaPorcentagem = diferencaPorcentagem;
                buscasPeso[*contBusca].ordem = arr[i].ordem;
                (*contBusca)++;
            }
            return;
        }
    }
}

// Função principal
int main(int argc, char* argv[]) {
    clock_t inicio, fim;
    inicio = clock();

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    int n;
    fscanf(input, "%d", &n);

    Container* containers = (Container*)malloc(n * sizeof(Container));

    // Preencher os contêineres e atribuir a ordem
    for (int i = 0; i < n; i++) {
        fscanf(input, "%s %s %d", containers[i].codigo, containers[i].cnpj, &containers[i].peso);
        containers[i].ordem = i + 1; // Ordem de leitura
    }

    // Ordenar contêineres pela ordem do código
    mergeSort(containers, 0, n - 1);

    int m;
    fscanf(input, "%d", &m);

    // Vetor para armazenar os resultados das buscas de peso
    BuscaPeso* buscasPeso = (BuscaPeso*)malloc(m * sizeof(BuscaPeso));
    int contBusca = 0; // Contador de buscas

    char codigoBusca[20];
    int pesoBusca;
    for (int i = 0; i < m; i++) {
        fscanf(input, "%s %d", codigoBusca, &pesoBusca);
        buscaPorPeso(containers, n, codigoBusca, pesoBusca, output, buscasPeso, &contBusca);
    }

    // Ordenar as buscas por diferença percentual de forma decrescente
    qsort(buscasPeso, contBusca, sizeof(BuscaPeso), compararPorDiferenca);

    // Imprimir as buscas de peso no arquivo de saída
    for (int i = 0; i < contBusca; i++) {
        fprintf(output, "%s:%d<->%.2f%%\n", buscasPeso[i].codigo, buscasPeso[i].pesoBusca, buscasPeso[i].diferencaPorcentagem);
    }

    free(containers);
    free(buscasPeso);
    fclose(input);
    fclose(output);

    fim = clock();
    double tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execução: %.6f segundos\n", tempo_execucao);

    return 0;
}
