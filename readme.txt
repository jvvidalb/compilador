====================================================================
          COMPILADOR MINI-PYTHON - ANALISADOR LÉXICO E SINTÁTICO E SEMANTICO
====================================================================

1. O QUE FOI IMPLEMENTADO ATÉ O MOMENTO
--------------------------------------------------------------------
O projeto consiste nas duas primeiras fases de um compilador para uma 
linguagem baseada em Python (Mini-Python), desenvolvidas em C puro:

* Analisador Léxico:
  - Carregamento do arquivo em memória, em um array baseado nos espaços.
  - Reconhecimento e classificação de tokens usando lógica de autômatos
    finitos (sem uso de bibliotecas de regex externas).
  - Suporte a tokens: ID, NUMERO_INTEIRO, LITERAL (strings), KEYWORD,
    OP_RELACIONAL, OP_ARITMETICO, DELIMITER..
  - Preenchimento de uma Tabela de Símbolos para identificadores, 
    números, literais e operadores.
  - Tratamento e reporte de erros léxicos (ex: caracteres não separados).

* Analisador Sintático:
  - Implementação top-down (descendente recursivo) baseada em uma 
    Gramática Livre de Contexto.
  - Reconhecimento de estruturas de controle de fluxo: IF, ELIF, ELSE, 
    WHILE e FOR.
  - Reconhecimento de atribuições e expressões matemáticas/relacionais.
  - Reconhecimento de comandos de linha única (Statements: print, 
    break, continue, return, exec, raise) e funções integradas que 
    retornam valor (Terms: len, input).
  
* Analisador Semantico:
Estrutura utilizada:
- Fila FIFO (First-In First-Out).
- Pois garante a destruição linear
- Mantem a ordem do consumo dos tokens

Objetivo:
- Validar significado do código.
- Verificar tipos.
- Controlar escopo.
- Detectar inconsistências semânticas.

Características:
- Processamento da esquerda para direita.
- O sintático garante a estrutura da linha.
- O semântico valida significado e tipos.
- Tokens são processados linha por linha.

Fluxo:
    Sintático -> Fila -> Semântico

Regras implementadas:
- Separação entre lado esquerdo e direito.
- Inferência de tipos.
- Propagação de tipos.
- Verificação de tipos incompatíveis.
- Uso antes da definição.
- Controle de mini escopo local.

Funcionamento:
- O sintático consome o token.
- O token é enfileirado.
- Ao final da linha:
    - o semântico desenfileira os tokens;
    - realiza validações;
    - propaga tipos.

Sistema de tipos:
- UNKNOWN
- NAO_DEFINIDO
- INTEIRO
- BOOLEANO

Estruturas reconhecidas:
- VARIAVEL
- VALOR
- LISTA
- TUPLA
- ITERATIVO

Listas e tuplas:
- Detectadas após operador '='.
- '[' define estrutura LISTA.
- '(' define estrutura TUPLA.
- Não tem mistura de tipos (APENAS)
- Não permite a troca do tipo da lista (APENAS)

Por exemplo:
x = [ True ]
x = True # é valido pois o tipo é o mesmo 
         # Porem perde-se a estrutura

Endereçamento:
- Variáveis recebem endereço de memória.
- Endereços são atribuídos incrementalmente.
- Controle realizado pela variável 'end'.

Objetivos do endereçamento:
- Preparação para geração de código.
- Organização de memória.
- Identificação única de variáveis.

Regras:
- Novas variáveis recebem novo endereço.
- Iteradores do for também recebem endereço.
- Valores literais não recebem endereço.


Escopo:
- Escopo global.
- Mini escopo para:
    - if
    - while
    - for

Regras de escopo:
- Variáveis locais tornam-se inacessíveis.
- Iteradores do for são ITERATIVO.
- Iteradores tornam-se inacessíveis após o bloco.

Funções ignoradas na tipagem:
- input()

====================================================================
            RESUMO DAS FLAGS E VARIÁVEIS DE CONTROLE
====================================================================

+----------------------+----------------+----------------------------------------------+
| Variável             | Tipo           | Propósito                                   |
+----------------------+----------------+----------------------------------------------+
| flag                 | TNo *          | Aponta para o nó do lado esquerdo da        |
|                      |                | atribuição em andamento.                    |
+----------------------+----------------+----------------------------------------------+
| flagReativada        | int            | Indica que 'flag' foi redeclarada após      |
|                      |                | estar INACESSIVEL.                          |
+----------------------+----------------+----------------------------------------------+
| noLocal              | TNo *          | Aponta para variável declarada dentro       |
|                      |                | de um bloco de controle.                    |
+----------------------+----------------+----------------------------------------------+
| proxLine             | int            | Valor 1 indica que a próxima linha          |
|                      |                | pertence ao corpo de um bloco.              |
+----------------------+----------------+----------------------------------------------+
| tipoUltimaExpressao  | TipoDado       | Armazena o tipo acumulado do lado direito   |
|                      |                | durante a análise semântica.                |
+----------------------+----------------+----------------------------------------------+
| ladoDireito          | int (local)    | 0 = lado esquerdo                           |
|                      |                | 1 = lado direito                            |
+----------------------+----------------+----------------------------------------------+
| ehLinhaDeControle    | int (local)    | Indica presença de if, while ou for         |
|                      |                | na linha atual.                             |
+----------------------+----------------+----------------------------------------------+
| tagLine              | int            | Linha atualmente acumulada na fila.         |
|                      |                | Utilizada para detectar mudança de linha.   |
+----------------------+----------------+----------------------------------------------+
+----------------------+----------------+----------------------------------------------+
| ehListaouTupla       | int (local)    | 0 = não é lista/Tupla                       |
|                      |                | 1 = lista/Tupla                             |
+----------------------+----------------+----------------------------------------------+
Exemplo de Funcionamento:

[ Código Fonte: x = 5 ]
        │
        ▼
┌─────────────────────────┐
│   Analisador Sintático  │ ──► Garante que a gramática está correta (ID = NUM)
└─────────────────────────┘
        │ consome();
        ▼ (Enfileira na ordem de leitura)
┌─────────────────────────┐
│   Fila FIFO Semântica   │ ──► [ "x" ] ──► [ "=" ] ──► [ "5" ]
└─────────────────────────┘
        │ analisadorSemantico();
        ▼ (Processamento linear / "Destruição")
┌─────────────────────────┐
│  Analisador Semântico   │
│                         │
│  1. Token "x"           │ ──► Lado Esquerdo. ID não definido. Aloca end 0. flag = &x
│  2. Token "="           │ ──► Seta ladoDireito = 1
│  3. Token "5"           │ ──► NUMERO (INTEIRO). tipoUltimaExpressao = INTEIRO
│                         │
│  4. Fim da Fila         │ ──► Propagação: flag->tipo = tipoUltimaExpressao (x vira INTEIRO)
└─────────────────────────┘

* Tabela de símbolos:
Estrutura utilizada:
- Tabela global única.

Informações armazenadas:
- Cadeia ( lexema )
- Atomo ( token )
- Tipo (BOOLEANO OU INTEIRO)
- Estado (ACESSÍVEL OU INACESSÍVEL)
- Estrutura (VARIAVEL, VALOR, LISTA OU TUPLA)
- Usado ( 1 - USADO, 0 - NÃO USADO)
- Endereços (-1 não possui endereçamento)

Motivos da escolha:
- Linguagem possui escopo simplificado.
- Apenas mini escopo local.
- Evita complexidade desnecessária.
- Não necessita pilha de tabelas.

Características:
- Integração com léxico e semântico.
- Atualização dinâmica de tipos.
- Controle de acessibilidade.



2. COMO EXECUTAR O CÓDIGO
--------------------------------------------------------------------
Pré-requisitos: Ter um compilador C (como o GCC) instalado.

gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador

./compilador arquivo.txt

A saída no terminal mostrará primeiramente a lista de tokens e símbolos 
encontrados (Análise Léxica), seguida pelo resultado da Análise 
Sintática e a Tabela de Símbolos final (case não haja erro)

3. DECISÕES DE DESIGN E IMPLEMENTAÇÃO
--------------------------------------------------------------------
* Coleta de tokens sob demanda: o analisador sintático chama o analisador léxico
  conforme consome tokens durante a análise sintática. ISSO NAO IMPACTA PERFORMANCE,
  pois os lexemas são todos carregados em memória ao início da execução.

* Divisão de Palavras-chave na Gramática: Para acomodar as diversas 
  palavras-chave da linguagem, elas foram divididas sintaticamente em 
  duas categorias:
  - COMMAND_STATEMENTS: Funções e palavras reservadas que representam 
    uma instrução completa (ex: print, break).
  - TERMS: Funções intrínsecas que devem ser processadas dentro de 
    expressões por retornarem valores (ex: len, input).

4. BUGS, ERROS CONHECIDOS E LIMITAÇÕES
--------------------------------------------------------------------
Após avaliação inicial pela professora, o código falhou diversos testes, entre eles:
  - input(), com argumento (literal). 
  - print() com tupla de argumentos: o print deve aceitar um literal, seguido e uma
  tupla de argumentos para indicar variáveis.
  - Tabela de símbolos mostrava apenas identificadores. Agora a tabela possui literais, 
  numeros e identificadores.