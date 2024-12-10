# KNN - Sequencial e Paralelo com Gráficos

Este repositório contém o código-fonte para compilar e executar implementações do algoritmo KNN (K-Nearest Neighbors) nas versões sequencial e paralela, além de um script Python para gerar gráficos comparativos de desempenho.

## Pré-requisitos

Certifique-se de ter os seguintes itens instalados no seu ambiente:

- **Compilador C (gcc ou outro equivalente)** para compilar os códigos C.
- **Python 3.x** com as bibliotecas `pandas` e `matplotlib` instaladas para gerar gráficos.

## Estrutura de Arquivos Executáveis na pasta src/
- create_datasets.c: Código responsável por criar os datasets de treino e teste.
- knn_sequencial.c: Implementação do algoritmo KNN na versão sequencial.
- knn_paralelo.c: Implementação do algoritmo KNN com paralelismo utilizando OpenMP.
- graphics.py: Script Python para gerar gráficos comparativos dos tempos de execução.

## Como Executar

### 1. Criar os Datasets

1. Navegue até o diretório `src`:

```
cd src/
```

2. **Compile o arquivo `create_datasets.c`**:

```
gcc create_datasets.c -o create_datasets
```

3. **Execute o programa para criar os datasets**:

```
./create_datasets
```

### 2. Executar o KNN Sequencial

4. **Compile o arquivo knn_sequencial.c**:

```
gcc knn_sequencial.c -o knn_sequencial -lm
```

5. **Execute o programa sequencial:**:

```
./knn_sequencial
```

### 3. Executar o KNN Paralelo


6. **Compile o arquivo knn_paralelo.c**:

```
gcc knn_paralelo.c -o knn_paralelo -fopenmp -lm
```


7. **Execute o programa paralelo:**:
```
./knn_paralelo
```

### 4. Gerar Gráficos de Comparação

Certifique-se de que os arquivos tempos_execucao_sequencial.txt e tempos_execucao_paralelo.txt foram gerados nos diretórios esperados após a execução dos programas.


8. **Execute o script Python para gerar os gráficos**:
```
python graphics.py
```