#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

// Função para calcular a distância euclidiana entre dois vetores
float calcula_distancia(float *xtrain, float *xtest, int w, int posicao_treino, int posicao_teste) {
    float distancia_total = 0;
    for (int j = 0; j < w; j++) {
        float diff = xtrain[posicao_treino + j] - xtest[posicao_teste + j];
        distancia_total += diff * diff;
    }
    return distancia_total;
}

// Função para encontrar os K menores valores de distância
void encontra_k_menores_distancias(float *distancias, int size, int *indices_k, int k) {
    int *marcado = (int *)calloc(size, sizeof(int));
    for (int i = 0; i < k; i++) {
        float menor = INFINITY;
        int indice_menor = -1;
        for (int j = 0; j < size; j++) {
            if (!marcado[j] && distancias[j] < menor) {
                menor = distancias[j];
                indice_menor = j;
            }
        }
        indices_k[i] = indice_menor;
        marcado[indice_menor] = 1;
    }
    free(marcado);
}

// Versão sequencial do KNN
void knn_sequencial(float *xtrain, float *ytrain, float *xtest, float *predicoes, int w, int qnt_treino, int qnt_teste, int k) {
    float *distancias = (float *)malloc(qnt_treino * sizeof(float));
    int *indices_k = (int *)malloc(k * sizeof(int));

    for (int i = 0; i < qnt_teste; i++) {
        int posicao_teste = i * w;

        // Calcula as distâncias para o ponto de teste atual
        for (int j = 0; j < qnt_treino; j++) {
            int posicao_treino = j * w;
            distancias[j] = calcula_distancia(xtrain, xtest, w, posicao_treino, posicao_teste);
        }

        // Encontra os índices dos K vizinhos mais próximos
        encontra_k_menores_distancias(distancias, qnt_treino, indices_k, k);

        // Calcula a média dos rótulos dos K vizinhos mais próximos
        float soma = 0;
        for (int j = 0; j < k; j++) {
            soma += ytrain[indices_k[j]];
        }
        predicoes[i] = soma / k;
    }

    free(distancias);
    free(indices_k);
}

// Versão paralela do KNN com OpenMP
void knn_paralelo(float *xtrain, float *ytrain, float *xtest, float *predicoes, int w, int qnt_treino, int qnt_teste, int k) {
    #pragma omp parallel
    {
        float *distancias = (float *)malloc(qnt_treino * sizeof(float));
        int *indices_k = (int *)malloc(k * sizeof(int));

        #pragma omp for
        for (int i = 0; i < qnt_teste; i++) {
            int posicao_teste = i * w;

            // Calcula as distâncias para o ponto de teste atual
            for (int j = 0; j < qnt_treino; j++) {
                int posicao_treino = j * w;
                distancias[j] = calcula_distancia(xtrain, xtest, w, posicao_treino, posicao_teste);
            }

            // Encontra os índices dos K vizinhos mais próximos
            encontra_k_menores_distancias(distancias, qnt_treino, indices_k, k);

            // Calcula a média dos rótulos dos K vizinhos mais próximos
            float soma = 0;
            for (int j = 0; j < k; j++) {
                soma += ytrain[indices_k[j]];
            }
            predicoes[i] = soma / k;
        }

        free(distancias);
        free(indices_k);
    }
}

// Função para carregar dados de um arquivo texto
void carregar_arquivo(const char *nome_arquivo, float **dados, int *tamanho) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    int capacidade = 1024;
    *dados = (float *)malloc(capacidade * sizeof(float));
    *tamanho = 0;

    while (fscanf(arquivo, "%f", &(*dados)[*tamanho]) == 1) {
        (*tamanho)++;
        if (*tamanho >= capacidade) {
            capacidade *= 2;
            *dados = (float *)realloc(*dados, capacidade * sizeof(float));
        }
    }

    fclose(arquivo);
}

// Função para salvar tempos em um arquivo
void salvar_tempo(const char *nome_arquivo, const char *metodo, const char *dataset, double tempo) {
    FILE *saida = fopen(nome_arquivo, "a");
    if (!saida) {
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    // Desloca o ponteiro para a posição 8 da string dataset
    const char *dataset_offset = dataset + 8;

    fprintf(saida, "%s, %s, %f\n", metodo, dataset_offset, tempo);
    fclose(saida);
}

int main() {
    const char *arquivos_teste[] = {
        "../data/dados_xtest_10.txt", "../data/dados_xtest_30.txt", "../data/dados_xtest_50.txt",
        "../data/dados_xtest_100.txt", "../data/dados_xtest_1000.txt", "../data/dados_xtest_100000.txt",
        "../data/dados_xtest_1000000.txt", "../data/dados_xtest_10000000.txt"
    };
    const int num_testes = sizeof(arquivos_teste) / sizeof(arquivos_teste[0]);
    const char *arquivo_treino_x = "../data/xtrain.txt";
    const char *arquivo_treino_y = "../data/ytrain.txt";
    const char *arquivo_saida = "../reports/time/tempos_execucao.txt";
    int w = 5;  // Tamanho do subvetor
    int k = 3;   // Número de vizinhos

    // Abrir o arquivo no modo "w" para limpar o conteúdo existente
    FILE *saida = fopen(arquivo_saida, "w");
    if (!saida) {
        printf("Erro ao abrir o arquivo %s\n", arquivo_saida);
        exit(EXIT_FAILURE);
    }
    fclose(saida);

    float *xtrain, *ytrain, *xtest;
    int tamanho_xtrain, tamanho_ytrain, tamanho_xtest;

    // Carregar dados de treinamento
    carregar_arquivo(arquivo_treino_x, &xtrain, &tamanho_xtrain);
    carregar_arquivo(arquivo_treino_y, &ytrain, &tamanho_ytrain);

    if (tamanho_xtrain % w != 0 || tamanho_ytrain != tamanho_xtrain / w) {
        printf("Erro: Tamanhos incompatíveis nos arquivos de treinamento.\n");
        exit(EXIT_FAILURE);
    }

    int qnt_treino = tamanho_xtrain / w;

    // Processar todos os conjuntos de teste
    for (int i = 0; i < num_testes; i++) {
        carregar_arquivo(arquivos_teste[i], &xtest, &tamanho_xtest);
        if (tamanho_xtest % w != 0) {
            printf("Erro: Tamanho do arquivo %s não é divisível por w.\n", arquivos_teste[i]);
            free(xtest);
            continue;
        }

        int qnt_teste = tamanho_xtest / w;
        float *predicoes = (float *)malloc(qnt_teste * sizeof(float));

        // Versão sequencial
        double inicio = omp_get_wtime();
        knn_sequencial(xtrain, ytrain, xtest, predicoes, w, qnt_treino, qnt_teste, k);
        double fim = omp_get_wtime();
        salvar_tempo(arquivo_saida, "Sequencial", arquivos_teste[i], fim - inicio);

        // Versão paralela
        inicio = omp_get_wtime();
        knn_paralelo(xtrain, ytrain, xtest, predicoes, w, qnt_treino, qnt_teste, k);
        fim = omp_get_wtime();
        salvar_tempo(arquivo_saida, "Paralelo", arquivos_teste[i], fim - inicio);

        free(xtest);
        free(predicoes);
    }

    free(xtrain);
    free(ytrain);

    return 0;
}
