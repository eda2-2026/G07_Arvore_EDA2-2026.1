# G07_Arvore_EDA2-2026.1

## Alunos
| Matricula | Aluno |
| -- | -- |
| 222021826 | Victor Leandro Rocha de Assis |
| 231036980 | Pedro Luiz Fonseca da Silva |

## Sobre
Projeto em linguagem C que implementa e compara estruturas de dados baseadas em
arvores usando a base `02-cados-abertos-preco-gasolina-etanol.csv`. O programa
carrega o CSV uma vez, executa o benchmark de ordenacao e em seguida o benchmark
de arvores AVL e Rubro-Negra sobre os mesmos registros.

## Arvores implementadas

### AVL
Arvore de busca binaria auto-balanceada. A altura e armazenada em cada no e o
rebalanceamento e feito via rotacoes simples e duplas (LL, RR, LR, RL) sempre
que o fator de balanco sair do intervalo {-1, 0, 1}. Operacoes com custo
`O(log n)`: insercao, busca e remocao (por sucessor in-order).

### Rubro-Negra
Arvore de busca binaria com coloracao de nos (vermelho/preto). Usa no sentinela
`nil` para eliminar verificacoes de NULL. O balanceamento e garantido pelas
propriedades da coloracao; fixup de insercao e remocao seguem os casos do CLRS.
Operacoes com custo `O(log n)`.

### Chave de busca
Os registros sao indexados por `valor_venda` (double); em caso de empate a
desambiguacao segue estado → municipio → produto → revenda.

## Metricas do benchmark de arvores
- tempo de insercao de todos os registros (ms);
- tempo de busca em `n/10` registros (ms);
- tempo de remocao de `n/10` registros (ms);
- altura final da arvore;
- numero total de rotacoes;
- numero total de comparacoes.

A AVL tende a ter altura menor (balanceamento mais rigido) e mais rotacoes que
a Rubro-Negra, que tolera maior desequilibrio mas rotaciona menos.

Alem da saida no terminal, o benchmark de arvores gera:
- `benchmark_arvore_resultados.csv`: tabela para planilha;
- `benchmark_arvore_relatorio.html`: painel visual.
## Vídeo de explicação:
_em breve_
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
