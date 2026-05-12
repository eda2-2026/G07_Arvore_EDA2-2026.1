# G07_Ordenacao_EDA2-2026.1

## Alunos
| Matricula | Aluno |
| -- | -- |
| 222021826 | Victor Leandro Rocha de Assis |
| 231036980 | Pedro Luiz Fonseca da Silva |

## Sobre
Projeto em linguagem C para comparar metodos de ordenacao usando a base
`02-cados-abertos-preco-gasolina-etanol.csv`, com precos de gasolina e etanol.

O programa carrega o CSV uma unica vez, guarda os registros em um vetor de
`Registro` e executa cada algoritmo sobre uma copia identica desse vetor. Isso
deixa claro o que e custo de leitura do arquivo e o que e custo de ordenacao.
## Vídeo de explicação:
https://youtu.be/RQWCVE7FE5E
## Metodos implementados
- **Insertion Sort**: metodo simples, bom para explicar a insercao em uma parte
  ja ordenada. Executado em amostra limitada porque tem custo medio `O(n^2)`.
- **Selection Sort**: metodo simples que escolhe o menor elemento a cada passo.
  Tambem usa amostra limitada por ter custo medio `O(n^2)`.
- **Merge Sort**: divide o vetor e intercala subvetores ordenados. Custo medio
  `O(n log n)` e uso extra de memoria.
- **Quick Sort**: particiona o vetor em torno de um pivo. Custo medio
  `O(n log n)`.
- **Heap Sort**: usa a ideia de heap maximo para ordenar em `O(n log n)`.
- **Radix Sort**: ordena por digitos ou partes da chave sem comparacoes diretas.
  Custo linear `O(n * k)`, onde `k` e o tamanho maximo da chave.

## KPIs exibidos
O executavel mostra uma tabela com:
- tempo em milissegundos;
- quantidade de comparacoes;
- quantidade de movimentacoes;
- memoria aproximada dos dados usados;
- confirmacao se o vetor terminou ordenado;
- observacao se o metodo usou base completa ou amostra.

Alem da saida no terminal, a execucao gera:
- `benchmark_resultados.csv`: tabela simples para anexar ou abrir em planilha;
- `benchmark_relatorio.html`: painel visual com os KPIs principais da
  apresentacao.

## Compilacao e execucao
Com GCC instalado, rode na raiz do projeto:

```bash
make run
```

Para usar outro CSV:

```bash
./ordenacao_combustivel caminho/do/arquivo.csv
```

No Windows com MinGW:

```cmd
mingw32-make run
```

## Chave de ordenacao
Os registros sao ordenados por:
1. valor de venda;
2. estado;
3. municipio;
4. produto;
5. revenda.

Essa chave facilita a demonstracao porque o preco e o foco da base, mas os
campos seguintes deixam a ordem deterministica quando dois postos possuem o
mesmo valor.
