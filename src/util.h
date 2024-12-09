// util.h
#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

float* le_array_x(const char* nome_arquivo, int *N, int *w) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nome_arquivo);
        exit(1);
    }

    fscanf(f, "%d", N);   // número de pontos
    fscanf(f, "%d", w);   // dimensão w

    int total = (*N) * (*w);
    float *arr = (float*)malloc(total * sizeof(float));
    if (!arr) {
        printf("Erro ao alocar memoria para %s\n", nome_arquivo);
        exit(1);
    }

    for (int i = 0; i < total; i++) {
        fscanf(f, "%f", &arr[i]);
    }
    fclose(f);
    return arr;
}

float* le_array_y(const char* nome_arquivo, int *N) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) {
        printf("Erro ao abrir arquivo %s\n", nome_arquivo);
        exit(1);
    }

    fscanf(f, "%d", N);

    float *arr = (float*)malloc((*N) * sizeof(float));
    if (!arr) {
        printf("Erro ao alocar memoria para %s\n", nome_arquivo);
        exit(1);
    }

    for (int i = 0; i < *N; i++) {
        fscanf(f, "%f", &arr[i]);
    }
    fclose(f);
    return arr;
}

#endif
