.
├── data # dados disponibilizados pelo professor e gerados (xtrain e ytrain)
│   ├── dados_xtest_10000000.txt
│   ├── dados_xtest_1000000.txt
│   ├── dados_xtest_100000.txt
│   ├── dados_xtest_1000.txt
│   ├── dados_xtest_100.txt
│   ├── dados_xtest_10.txt
│   ├── dados_xtest_30.txt
│   ├── dados_xtest_50.txt
│   ├── dados_xtrain.txt
│   ├── xtrain.txt
│   └── ytrain.txt
├── estrutura-projeto.md
├── README.md # instruções para a execução do código
├── reports # saídas dos código
│   ├── graphics # graficos que estão no relatório
│   │   ├── comparacao_tempos_execucao_log.png
│   │   ├── comparacao_tempos_execucao.png
│   │   ├── grafico_ dados_xtest_10000000.png
│   │   ├── grafico_ dados_xtest_1000000.png
│   │   ├── grafico_ dados_xtest_100000.png
│   │   ├── grafico_ dados_xtest_1000.png
│   │   ├── grafico_ dados_xtest_100.png
│   │   ├── grafico_ dados_xtest_10.png
│   │   ├── grafico_ dados_xtest_30.png
│   │   └── grafico_ dados_xtest_50.png
│   ├── predictions # predições
│   │   ├── paralelo # predições do código em paralelo
│   │   │   ├── pred_10000000_w5_k3.txt
│   │   │   ├── pred_1000000_w5_k3.txt
│   │   │   ├── pred_100000_w5_k3.txt
│   │   │   ├── pred_1000_w5_k3.txt
│   │   │   ├── pred_100_w5_k3.txt
│   │   │   ├── pred_10_w5_k3.txt
│   │   │   ├── pred_30_w5_k3.txt
│   │   │   └── pred_50_w5_k3.txt
│   │   └── sequencial # predições do código sequencial
│   │       ├── pred_10000000_w5_k3.txt
│   │       ├── pred_1000000_w5_k3.txt
│   │       ├── pred_100000_w5_k3.txt
│   │       ├── pred_1000_w5_k3.txt
│   │       ├── pred_100_w5_k3.txt
│   │       ├── pred_10_w5_k3.txt
│   │       ├── pred_30_w5_k3.txt
│   │       └── pred_50_w5_k3.txt
│   └── time # dados de tempo de execução
│       ├── tempos_execucao_paralelo.txt
│       └── tempos_execucao_sequencial.txt
└── src # código fonte
    ├── create_datasets
    ├── create_datasets.c
    ├── graphics.py
    ├── knn_paralelo
    ├── knn_paralelo.c
    ├── knn_sequencial
    └── knn_sequencial.c

8 directories, 48 files
