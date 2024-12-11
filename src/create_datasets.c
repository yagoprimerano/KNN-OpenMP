#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <bits/getopt_core.h>

/**
 * Função para criar janelas deslizantes (moving window) a partir de uma série temporal.
 *
 * Parâmetros:
 * - data: Vetor com os dados da série temporal.
 * - data_len: Tamanho total do vetor de entrada.
 * - w: Tamanho da janela (quantidade de valores no passado usados como entrada).
 * - h: Tamanho da previsão (quantidade de valores no futuro usados como saída).
 * - X: Ponteiro de saída para a matriz X (janelas de entrada).
 * - x_rows: Ponteiro para armazenar o número de linhas da matriz X.
 * - Y: Ponteiro de saída para a matriz Y (valores previstos).
 * - y_rows: Ponteiro para armazenar o número de linhas da matriz Y.
 */
void moving_window(const float *data, int data_len, int w, int h, float **X, int *x_rows, float **Y, int *y_rows) {
    // Calcula o número total de janelas que podem ser criadas com os parâmetros w e h.
    int total_windows = data_len - w - h + 1;

    // Caso não seja possível criar janelas, retorna sem alocar memória.
    if (total_windows <= 0) {
        *X = NULL;
        *Y = NULL;
        *x_rows = 0;
        *y_rows = 0;
        return;
    }

    // Aloca memória para a matriz X (janelas de entrada) e matriz Y (valores previstos).
    *X = (float *)malloc(total_windows * w * sizeof(float)); // Cada janela tem w valores.
    *Y = (float *)malloc(total_windows * h * sizeof(float)); // Cada previsão tem h valores.

    // Atualiza o número de linhas geradas em X e Y.
    *x_rows = total_windows; // Número de janelas criadas.
    *y_rows = total_windows; // Cada janela de entrada tem uma saída associada.

    // Preenche as matrizes X e Y com as janelas deslizantes.
    for (int i = 0; i < total_windows; i++) {
        // Preenche a linha i de X com w valores consecutivos da série temporal.
        for (int j = 0; j < w; j++) {
            (*X)[i * w + j] = data[i + j];
        }

        // Preenche a linha i de Y com h valores consecutivos após a janela X.
        for (int k = 0; k < h; k++) {
            (*Y)[i * h + k] = data[i + w + k];
        }
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

/**
 * Função para salvar os valores de uma matriz em um arquivo, um valor por linha.
 *
 * Parâmetros:
 * - filename: Caminho para o arquivo onde os dados serão salvos.
 * - data: Ponteiro para os valores a serem salvos.
 * - rows: Número de linhas na matriz.
 * - cols: Número de colunas na matriz.
 */
void save_to_file(const char *filename, const float *data, int rows, int cols) {
    // Abre o arquivo em modo de escrita.
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    // Escreve os valores no arquivo, um por linha.
    for (int i = 0; i < rows * cols; i++) {
        fprintf(file, "%.6f\n", data[i]); // Formata os valores com 6 casas decimais.
    }

    // Fecha o arquivo.
    fclose(file);
}

int main(int argc, char *argv[]) {
    // Caminhos para os arquivos de entrada e saída.
    const char *input_file = "../data/dados_xtrain.txt"; // Arquivo de entrada com a série temporal.
    const char *x_output_file = "../data/xtrain.txt";    // Arquivo para salvar a matriz X.
    const char *y_output_file = "../data/ytrain.txt";    // Arquivo para salvar a matriz Y.

    // Parâmetros necessários para execução do código
    int w = -1, h = -1; // Inicialmente inválidos

    // Leitura dos parâmetros pela CLI
    int opt;
    while ((opt = getopt(argc, argv, "w:h:")) != -1) {
        switch (opt) {
        case 'w':
            w = atoi(optarg);
            break;
        case 'h':
            h = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Uso: %s -w <tamanho janela> -h <tamanho previsão>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // ----------------------------------- Verificação de se os parâmetros são válidos ---------------
    if (w <= 0 || h <= 0) {
        fprintf(stderr, "Parâmetros w e h devem ser fornecidos e maiores que 0.\n");
        fprintf(stderr, "Uso: %s -w <tamanho janela> -h <tamanho previsão>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    float *xtest; // Ponteiros para armazenar os dados
    int tamanho_xtest;

    // Processa todos os arquivos de teste e verifica se w é válido
    for (int i = 0; i < num_testes; i++) {
        carregar_arquivo(arquivos_teste[i], &xtest, &tamanho_xtest);
        if (tamanho_xtest % w != 0) {
            printf("Erro: Tamanho do arquivo %s não é divisível por w. Escolha um valor de w que seja divisível pelo tamanho dos arquivos de teste\n", arquivos_teste[i]);
            free(xtest);
            continue;
        }
    }

    // ----------------------------------- Fim da verificação de se os parâmetros são válidos ---------------

    // Abre o arquivo de entrada para leitura.
    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Erro ao abrir dados_xtrain.txt");
        return EXIT_FAILURE;
    }

    // Conta o número de linhas (valores) no arquivo de entrada.
    int data_len = 0;
    float temp;
    while (fscanf(file, "%f", &temp) == 1) {
        data_len++;
    }

    // Retorna ao início do arquivo para carregar os valores.
    rewind(file);

    // Aloca memória para armazenar os dados da série temporal.
    float *data = (float *)malloc(data_len * sizeof(float));
    for (int i = 0; i < data_len; i++) {
        fscanf(file, "%f", &data[i]); // Lê os valores do arquivo.
    }
    fclose(file); // Fecha o arquivo de entrada.

    // Define os parâmetros w (tamanho da janela) e h (tamanho da previsão).
    //int w = 5; // Tamanho da janela: considera os últimos 5 valores como entrada.
    //int h = 1; // Previsão: considera apenas o próximo valor como saída.

    // Declara os ponteiros para as matrizes X e Y e os contadores de linhas.
    float *X = NULL, *Y = NULL;
    int x_rows = 0, y_rows = 0;

    // Gera as janelas deslizantes com base nos dados de entrada.
    moving_window(data, data_len, w, h, &X, &x_rows, &Y, &y_rows);

    // Salva as matrizes X e Y nos arquivos de saída.
    save_to_file(x_output_file, X, x_rows, w); // Salva X (janelas de entrada).
    save_to_file(y_output_file, Y, y_rows, h); // Salva Y (valores previstos).

    // Libera a memória alocada.
    free(data);
    free(X);
    free(Y);

    // Mensagem de confirmação.
    printf("Janelas salvas em %s e %s\n", x_output_file, y_output_file);
    return 0;
}
