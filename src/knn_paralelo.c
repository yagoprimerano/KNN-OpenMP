#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>   // Biblioteca necessária para implementar o paralelismo
#include <string.h>
#include <unistd.h>
#include <bits/getopt_core.h>

// Função para calcular a distância euclidiana entre dois vetores
// Entrada: dois vetores (xtrain e xtest), tamanho da janela (w) e suas posições
// Retorna: a soma dos quadrados das diferenças entre os elementos dos dois vetores
float calcula_distancia(float *xtrain, float *xtest, int w, int posicao_treino, int posicao_teste) {
    float distancia_total = 0; // Variável para acumular a distância total
    for (int j = 0; j < w; j++) { // Itera pelos elementos dos dois vetores
        float diff = xtrain[posicao_treino + j] - xtest[posicao_teste + j]; // Calcula a diferença entre elementos correspondentes
        distancia_total += diff * diff; // Soma o quadrado da diferença à distância total
    }
    return distancia_total; // Retorna a distância calculada
}

// Função para encontrar os K menores valores de distância
// Entrada: vetor de distâncias, tamanho do vetor, vetor de índices e valor de K
// Modifica: preenche o vetor de índices com os K menores valores de distância
void encontra_k_menores_distancias(float *distancias, int size, int *indices_k, int k) {
    int *marcado = (int *)calloc(size, sizeof(int)); // Vetor para marcar índices já processados
    for (int i = 0; i < k; i++) { // Itera para selecionar os K menores valores
        float menor = INFINITY;  // Inicializa com o maior valor possível
        int indice_menor = -1;   // Variável para armazenar o índice do menor valor
        for (int j = 0; j < size; j++) { // Itera pelo vetor de distâncias
            if (!marcado[j] && distancias[j] < menor) { // Procura o menor valor ainda não processado
                menor = distancias[j]; // Atualiza o menor valor encontrado
                indice_menor = j;      // Atualiza o índice correspondente
            }
        }
        indices_k[i] = indice_menor; // Armazena o índice do menor valor no vetor de índices
        marcado[indice_menor] = 1;  // Marca o índice como processado
    }
    free(marcado); // Libera a memória alocada para o vetor de marcação
}

// Função para implementar o algoritmo KNN em paralelo usando OpenMP
// Entrada: vetores de treino e teste, vetor de predições, parâmetros do KNN
void knn_paralelo(float *xtrain, float *ytrain, float *xtest, float *predicoes, int w, int qnt_treino, int qnt_teste, int k) {
    #pragma omp parallel // Define que o bloco será executado em paralelo
    {
        // Arrays locais para armazenar distâncias e índices para cada thread
        float *distancias = (float *)malloc(qnt_treino * sizeof(float));
        int *indices_k = (int *)malloc(k * sizeof(int));

        // Paraleliza o loop principal que itera sobre os pontos de teste
        #pragma omp for
        for (int i = 0; i < qnt_teste; i++) { // Itera sobre cada ponto de teste
            int posicao_teste = i * w;       // Calcula a posição inicial do ponto de teste no vetor xtest

            // Calcula a distância entre o ponto de teste atual e todos os pontos de treino
            for (int j = 0; j < qnt_treino; j++) {
                int posicao_treino = j * w; // Calcula a posição inicial do ponto de treino no vetor xtrain
                distancias[j] = calcula_distancia(xtrain, xtest, w, posicao_treino, posicao_teste);
            }

            // Identifica os índices dos K vizinhos mais próximos
            encontra_k_menores_distancias(distancias, qnt_treino, indices_k, k);

            // Calcula a média dos rótulos (ytrain) dos K vizinhos mais próximos
            float soma = 0;
            for (int j = 0; j < k; j++) {
                soma += ytrain[indices_k[j]]; // Soma os valores correspondentes aos índices encontrados
            }
            predicoes[i] = soma / k; // Calcula a predição como a média dos valores dos vizinhos
        }

        free(distancias); // Libera o array local de distâncias
        free(indices_k);  // Libera o array local de índices
    }
}

// Função para carregar dados de um arquivo texto
// Entrada: nome do arquivo, ponteiro para armazenar os dados e ponteiro para o tamanho
// Modifica: aloca memória e preenche o vetor de dados com os valores do arquivo
void carregar_arquivo(const char *nome_arquivo, float **dados, int *tamanho) {
    FILE *arquivo = fopen(nome_arquivo, "r"); // Abre o arquivo em modo leitura
    if (!arquivo) { // Verifica se o arquivo foi aberto com sucesso
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    int capacidade = 1024; // Capacidade inicial do vetor de dados
    *dados = (float *)malloc(capacidade * sizeof(float)); // Aloca memória para o vetor de dados
    *tamanho = 0; // Inicializa o tamanho como zero

    // Lê os valores do arquivo e armazena no vetor de dados
    while (fscanf(arquivo, "%f", &(*dados)[*tamanho]) == 1) {
        (*tamanho)++;
        if (*tamanho >= capacidade) { // Realoca memória se a capacidade for excedida
            capacidade *= 2;
            *dados = (float *)realloc(*dados, capacidade * sizeof(float));
        }
    }

    fclose(arquivo); // Fecha o arquivo
}

// Função para salvar os tempos de execução em um arquivo
// Entrada: nome do arquivo, método utilizado, nome do dataset e o tempo medido
void salvar_tempo(const char *nome_arquivo, const char *metodo, const char *dataset, double tempo) {
    FILE *saida = fopen(nome_arquivo, "a"); // Abre o arquivo em modo append
    if (!saida) { // Verifica se o arquivo foi aberto com sucesso
        printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    const char *dataset_offset = dataset + 8; // Ajusta o nome do dataset para exibição no arquivo
    fprintf(saida, "%s, %s, %f\n", metodo, dataset_offset, tempo); // Escreve o método, dataset e tempo no arquivo
    fclose(saida); // Fecha o arquivo
}

int main(int argc, char *argv[]) {

    // Listagem dos arquivos de teste a serem processados
    const char *arquivos_teste[] = {
        "../data/dados_xtest_10.txt", "../data/dados_xtest_30.txt", "../data/dados_xtest_50.txt",
        "../data/dados_xtest_100.txt", "../data/dados_xtest_1000.txt", "../data/dados_xtest_100000.txt",
        "../data/dados_xtest_1000000.txt", "../data/dados_xtest_10000000.txt"
    };
    const int num_testes = sizeof(arquivos_teste) / sizeof(arquivos_teste[0]); // Número de arquivos de teste
    const char *arquivo_treino_x = "../data/xtrain.txt"; // Arquivo com os dados de treino (entrada)
    const char *arquivo_treino_y = "../data/ytrain.txt"; // Arquivo com os rótulos de treino
    const char *arquivo_saida = "../reports/time/tempos_execucao_paralelo.txt"; // Arquivo de saída para tempos
    
    int w = -1, k = -1;

    int opt;
    while ((opt = getopt(argc, argv, "w:k:")) != -1) {
        switch (opt) {
        case 'w':
            w = atoi(optarg);
            break;
        case 'k':
            k = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Uso: %s -w <tamanho janela> -k <numero de vizinhos>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (w <= 0 || k <= 0) {
        fprintf(stderr, "Parâmetros w e k devem ser fornecidos e maiores que 0.\n");
        fprintf(stderr, "Uso: %s -w <tamanho janela> -k <numero de vizinhos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    

    int max_threads = omp_get_max_threads(); // Obtém o número máximo de threads disponíveis
    omp_set_num_threads(max_threads);       // Configura o número de threads para o máximo disponível

    printf("Número máximo de threads disponíveis e configuradas: %d\n", max_threads);

    FILE *saida = fopen(arquivo_saida, "w"); // Abre o arquivo de saída para limpar o conteúdo existente
    if (!saida) {
        printf("Erro ao abrir o arquivo %s\n", arquivo_saida);
        exit(EXIT_FAILURE);
    }
    fclose(saida);

    float *xtrain, *ytrain, *xtest; // Ponteiros para armazenar os dados
    int tamanho_xtrain, tamanho_ytrain, tamanho_xtest;

    // Carrega os dados de treino
    carregar_arquivo(arquivo_treino_x, &xtrain, &tamanho_xtrain);
    carregar_arquivo(arquivo_treino_y, &ytrain, &tamanho_ytrain);

   //if (tamanho_xtrain % w != 0 || tamanho_ytrain != tamanho_xtrain / w) {
    //    printf("Erro: Tamanhos incompatíveis nos arquivos de treinamento.\n");
     //   exit(EXIT_FAILURE);
    //}

    int qnt_treino = tamanho_xtrain / w; // Calcula o número de exemplos no conjunto de treino

    // Processa todos os arquivos de teste
    for (int i = 0; i < num_testes; i++) {
        carregar_arquivo(arquivos_teste[i], &xtest, &tamanho_xtest);
        if (tamanho_xtest % w != 0) {
            printf("Erro: Tamanho do arquivo %s não é divisível por w. Escolha um valor de w que seja divisível pelo tamanho dos arquivos de teste\n", arquivos_teste[i]);
            free(xtest);
            continue;
        }

        int qnt_teste = tamanho_xtest / w; // Calcula o número de exemplos no conjunto de teste
        float *predicoes = (float *)malloc(qnt_teste * sizeof(float)); // Aloca memória para as predições

        // Mede o tempo de execução da versão paralela usando omp_get_wtime()
        double inicio = omp_get_wtime();
        knn_paralelo(xtrain, ytrain, xtest, predicoes, w, qnt_treino, qnt_teste, k);
        double fim = omp_get_wtime();

        // Calcula o tempo total de execução para o conjunto de teste atual
        double tempo = fim - inicio;

        // Salva o tempo de execução no arquivo de saída
        salvar_tempo(arquivo_saida, "Paralelo", arquivos_teste[i], tempo);

        // Salvar as predições em um arquivo txt
        char nome_arquivo_predicao[256];
        snprintf(nome_arquivo_predicao, sizeof(nome_arquivo_predicao), 
                 "../reports/predictions/paralelo/pred_%d_w%d_k%d.txt", 
                 tamanho_xtest, w, k);

        FILE *arquivo_predicao = fopen(nome_arquivo_predicao, "w");
        if (!arquivo_predicao) {
            printf("Erro ao abrir o arquivo de predições %s\n", nome_arquivo_predicao);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < qnt_teste; j++) {
            fprintf(arquivo_predicao, "%f\n", predicoes[j]); // Escreve cada predição em uma nova linha
        }
        fclose(arquivo_predicao); // Fecha o arquivo de predições

        // Libera a memória alocada para os dados de teste e predições
        free(xtest);
        free(predicoes);
    }

    // Libera a memória alocada para os dados de treino
    free(xtrain);
    free(ytrain);

    return 0; // Finaliza a execução do programa
}
