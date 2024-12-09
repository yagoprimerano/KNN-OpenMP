# KNN Parallel - Projeto de Predição

Este repositório contém o código-fonte necessário para compilar e executar um programa que implementa um KNN paralelo. O fluxo de execução começa com a criação dos datasets e, em seguida, a execução do algoritmo de KNN paralelo.

## Pré-requisitos

Certifique-se de ter os seguintes itens instalados no seu ambiente:

- **Compilador C (gcc ou outro equivalente)**

## Como Executar

1. **Navegue até o diretório `src`**:

```
cd src/
```

2. **Compile o arquivo `create_datasets.c`**:

```
gcc create_datasets.c -o create_datasets
```

3. **Execute o arquivo compilado para criar os datasets**:

```
./create_datasets
```

4. **Compile o arquivo knn_parallel.c**:

```
gcc knn_parallel.c -o knn_parallel -fopenmp
```

5. **Execute o programa principal**:

```
./knn_parallel
```

