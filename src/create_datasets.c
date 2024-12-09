#include <stdio.h>
#include <stdlib.h>

void moving_window(const float *data, int data_len, int w, int h, float **X, int *x_rows, float **Y, int *y_rows) {
    int total_windows = data_len - w - h + 1;

    if (total_windows <= 0) {
        *X = NULL;
        *Y = NULL;
        *x_rows = 0;
        *y_rows = 0;
        return;
    }

    *X = (float *)malloc(total_windows * w * sizeof(float));
    *Y = (float *)malloc(total_windows * h * sizeof(float));

    *x_rows = total_windows;
    *y_rows = total_windows;

    for (int i = 0; i < total_windows; i++) {
        // Preencher X
        for (int j = 0; j < w; j++) {
            (*X)[i * w + j] = data[i + j];
        }

        // Preencher Y
        for (int k = 0; k < h; k++) {
            (*Y)[i * h + k] = data[i + w + k];
        }
    }
}

void save_to_file(const char *filename, const float *data, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows * cols; i++) {
        fprintf(file, "%.6f\n", data[i]);
    }

    fclose(file);
}

int main() {
    const char *input_file = "../data/dados_xtrain.txt";
    const char *x_output_file = "../data/xtrain.txt";
    const char *y_output_file = "../data/ytrain.txt";

    // Ler dados do arquivo input.txt
    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Erro ao abrir dados_xtrain.txt");
        return EXIT_FAILURE;
    }

    // Contar número de linhas no arquivo
    int data_len = 0;
    float temp;
    while (fscanf(file, "%f", &temp) == 1) {
        data_len++;
    }

    rewind(file);

    // Alocar e carregar dados
    float *data = (float *)malloc(data_len * sizeof(float));
    for (int i = 0; i < data_len; i++) {
        fscanf(file, "%f", &data[i]);
    }
    fclose(file);

    // Parâmetros
    int w = 5; // Tamanho da janela
    int h = 1; // Número de valores previstos

    // Gerar matrizes X e Y
    float *X = NULL, *Y = NULL;
    int x_rows = 0, y_rows = 0;
    moving_window(data, data_len, w, h, &X, &x_rows, &Y, &y_rows);

    // Salvar X e Y nos arquivos de saída
    save_to_file(x_output_file, X, x_rows, w);
    save_to_file(y_output_file, Y, y_rows, h);

    // Liberar memória
    free(data);
    free(X);
    free(Y);

    printf("Janelas salvas em %s e %s\n", x_output_file, y_output_file);
    return 0;
}