import pandas as pd
import matplotlib.pyplot as plt


def gerar_graficos_barras():
    """
    Gera gráficos de barras para comparar os tempos de execução para cada dataset.
    Os gráficos são salvos no diretório especificado.
    """
    # Colunas esperadas nos arquivos de entrada
    colunas = ['Metodo', 'Dataset', 'Tempo']

    # Leitura dos arquivos separados
    df_sequencial = pd.read_csv("../reports/time/tempos_execucao_sequencial.txt", header=None, names=colunas)
    df_paralelo = pd.read_csv("../reports/time/tempos_execucao_paralelo.txt", header=None, names=colunas)

    # Combinação dos dois DataFrames
    dados = pd.concat([df_sequencial, df_paralelo])

    # Obter os nomes únicos dos datasets
    datasets = dados['Dataset'].unique()

    # Geração de gráficos para cada dataset
    for dataset in datasets:
        df_dataset = dados[dados['Dataset'] == dataset]

        # Criar o gráfico de barras
        plt.figure(figsize=(8, 6))
        plt.bar(df_dataset['Metodo'], df_dataset['Tempo'], color=['blue', 'orange'])
        plt.title(f"Tempo de Execução: {dataset}")
        plt.xlabel("Método")
        plt.ylabel("Tempo de Execução (s)")
        plt.xticks(rotation=45)
        plt.tight_layout()

        # Salvar o gráfico no diretório especificado
        nome_imagem = f"../reports/graphics/grafico_{dataset.replace('.txt', '')}.png"
        plt.savefig(nome_imagem)
        plt.close()  # Fecha a figura para liberar memória


def gerar_graficos_comparativos(log_y=False):
    """
    Gera gráficos comparativos dos tempos de execução (Sequencial vs Paralelo).
    O gráfico utiliza escala logarítmica no eixo X e opcionalmente no eixo Y.

    Args:
        log_y (bool): Se True, aplica escala logarítmica também no eixo Y.
    """
    # Leitura dos arquivos separados
    df_sequencial = pd.read_csv("../reports/time/tempos_execucao_sequencial.txt", header=None,
                                names=["Execucao", "Arquivo", "Tempo"])
    df_paralelo = pd.read_csv("../reports/time/tempos_execucao_paralelo.txt", header=None,
                              names=["Execucao", "Arquivo", "Tempo"])

    # Combinação dos DataFrames
    df = pd.concat([df_sequencial, df_paralelo])

    # Extrair o tamanho do conjunto de dados a partir do nome do arquivo
    df["Tamanho"] = df["Arquivo"].str.extract(r'(\d+)').astype(int)

    # Calcular a média dos tempos para cada combinação de Execução e Tamanho
    summary = df.groupby(["Execucao", "Tamanho"]).agg(Media_Tempo=("Tempo", "mean")).reset_index()

    # Criar o gráfico comparativo
    plt.figure(figsize=(12, 6))

    # Separar os dados por tipo de execução e plotar as curvas
    for execucao in summary["Execucao"].unique():
        subset = summary[summary["Execucao"] == execucao]
        plt.plot(subset["Tamanho"], subset["Media_Tempo"], marker='o', label=execucao)

    # Configurações do gráfico
    plt.xlabel("Tamanho do Conjunto de Teste")
    plt.ylabel("Duração da Execução (s)")
    plt.title("Comparação de Tempos de Execução: Sequencial vs Paralelo")
    plt.legend(title="Tipo de Execução")
    plt.xscale("log")  # Escala logarítmica no eixo X
    if log_y:
        plt.yscale("log")  # Escala logarítmica no eixo Y (se especificado)
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.tight_layout()

    # Nome do arquivo de saída
    output_file = "../reports/graphics/comparacao_tempos_execucao_log.png" if log_y else "../reports/graphics/comparacao_tempos_execucao.png"
    plt.savefig(output_file, dpi=300, bbox_inches="tight")
    plt.close()  # Fecha a figura para liberar memória


if __name__ == "__main__":
    # Gera gráficos de barras para cada dataset
    gerar_graficos_barras()

    # Gera gráfico comparativo com escala logarítmica no eixo X
    gerar_graficos_comparativos(log_y=False)

    # Gera gráfico comparativo com escala logarítmica em ambos os eixos
    gerar_graficos_comparativos(log_y=True)

    # Mensagem final indicando sucesso
    print("Todos os gráficos foram gerados e salvos com sucesso!")
