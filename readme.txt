====================================================================
   COMPILADOR MINI-PYTHON - ANALISADOR LÉXICO, SINTÁTICO E SEMÂNTICO
====================================================================

1. O QUE FOI IMPLEMENTADO
--------------------------------------------------------------------
O projeto consiste nas três primeiras fases de um compilador para uma
linguagem baseada em Python (Mini-Python), desenvolvidas em C puro,
além da geração de código intermediário (fase 4):

* Analisador Léxico:
  - Carregamento do arquivo em memória em um array baseado nos espaços.
  - Reconhecimento e classificação de tokens usando lógica de autômatos
    finitos (sem uso de bibliotecas de regex externas).
  - Suporte a tokens: ID, NUMERO_INTEIRO, LITERAL (strings), KEYWORD,
    OP_RELACIONAL, OP_ARITMETICO, DELIMITER.
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
  - Construção simultânea do DAG (Directed Acyclic Graph) / AST
    durante o parsing, nó a nó, conforme os tokens são consumidos.

* Analisador Semântico:
  Estrutura utilizada:
  - Fila FIFO (First-In First-Out).
  - Garante o processamento linear e na ordem de leitura dos tokens.
  - Mantém a ordem do consumo dos tokens da esquerda para a direita.

  Objetivo:
  - Validar o significado do código após a garantia estrutural do sintático.
  - Verificar tipos e detectar incompatibilidades.
  - Controlar escopo de variáveis.
  - Detectar inconsistências semânticas (uso antes da definição, variáveis
    não utilizadas, mistura de tipos em listas/tuplas, etc.).

  Características:
  - Processamento da esquerda para a direita, linha por linha.
  - O sintático garante a estrutura da linha; o semântico valida significado.
  - Tokens são acumulados na fila durante o parsing e processados ao fim
    de cada linha (detecção de mudança de linha via tagLine) ou ao EOS.

  Fluxo:
      Sintático -> Fila FIFO -> Semântico

  Regras implementadas:
  - Separação entre lado esquerdo e lado direito da atribuição.
  - Inferência de tipos a partir de literais e valores conhecidos.
  - Propagação de tipos ao longo de expressões.
  - Verificação de tipos incompatíveis em operações e atribuições.
  - Detecção de uso antes da definição (tipo UNKNOWN ou NAO_DEFINIDO).
  - Controle de mini-escopo local (if, while, for).
  - Detecção de mistura de tipos dentro de listas e tuplas.

  Funcionamento passo a passo:
  - O sintático consome o token e chama `consome()`.
  - `consome()` enfileira o token na fila semântica, agrupando por linha.
  - Ao detectar mudança de linha (token.linha != tagLine), chama
    `analisadorSemantico()` para processar os tokens da linha anterior.
  - O semântico desenfileira os tokens um a um e realiza as validações.
  - Ao fim da fila, propaga o tipo do lado direito para a variável do
    lado esquerdo (apontada pela `flag`).

  Sistema de tipos:
  - UNKNOWN      → tipo inicial; símbolo ainda não visto pelo semântico.
  - NAO_DEFINIDO → variável declarada à esquerda, mas sem valor ainda.
  - INTEIRO      → números e iteradores de for.
  - BOOLEANO     → True, False.

  Estruturas reconhecidas:
  - VARIAVEL  → identificador comum.
  - VALOR     → número ou booleano literal.
  - LISTA     → estrutura iniciada por '[' após '='.
  - TUPLA     → estrutura iniciada por '(' após '='.
  - ITERATIVO → variável de controle do for.

  Listas e tuplas:
  - Detectadas após o operador '='.
  - '[' define estrutura LISTA; '(' define estrutura TUPLA.
  - Não permitem mistura de tipos internamente. (APENAS)
  - Não permitem troca do tipo já definido da estrutura. (APENAS)

  Por exemplo:
      x = [ True ]
      x = True  # válido: mesmo tipo; porém perde-se a estrutura LISTA

  Endereçamento:
  - Variáveis recebem endereço de memória atribuído incrementalmente
    pela variável global `end`.
  - Iteradores do for também recebem endereço.
  - Valores literais não recebem endereço (endereco = -1).
  - Objetivos: preparação para geração de código, organização de
    memória e identificação única de variáveis.

  Escopo:
  - Escopo global único para todo o programa.
  - Mini-escopo para blocos: if, while, for.
  - Regras:
      - Variáveis declaradas dentro de um bloco tornam-se INACESSIVEL
        ao fim deste (controladas por `noLocal`).
      - Iteradores do for são marcados como ITERATIVO e tornam-se
        INACESSIVEL após o bloco do for.

  Funções ignoradas na tipagem:
  - input(): ao detectar "input" na fila, o semântico esvazia a fila
    inteira e retorna sem fazer validação de tipos para aquela linha.

* Geração de Código Intermediário (DAG / Código de 3 Endereços):
  - Detalhado na seção 3.


2. COMO EXECUTAR O CÓDIGO
--------------------------------------------------------------------
Pré-requisitos: Ter um compilador C (como o GCC) instalado.

    gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador

    ./compilador arquivo.txt

A saída no terminal mostrará primeiramente a lista de tokens e símbolos
encontrados (Análise Léxica), seguida pelo resultado da Análise
Sintática, a Tabela de Símbolos final (caso não haja erro) e a
confirmação da geração do arquivo `codigo_intermediario.txt`.


3. DECISÕES DE DESIGN E IMPLEMENTAÇÃO
--------------------------------------------------------------------

* Coleta de tokens sob demanda:
  O analisador sintático chama o analisador léxico conforme consome
  tokens durante a análise. Isso não impacta performance, pois todos
  os lexemas já foram carregados em memória no início da execução
  (array `lexemasArray[512][512]`), e o léxico apenas percorre esse
  array via ponteiro de posição (`posicaoAtual`).

* Divisão de palavras-chave na gramática:
  Para acomodar as diversas palavras-chave da linguagem, elas foram
  divididas sintaticamente em duas categorias:
  - COMMAND_STATEMENTS: palavras reservadas que representam uma instrução
    completa (ex: print, break, return, exec, raise, continue).
  - TERMS: funções intrínsecas que retornam valor e devem ser processadas
    dentro de expressões (ex: len, input).

* Integração sintático-semântica via linha:
  A função `consome()` é o ponto central de integração. Cada token
  consumido pelo sintático é enfileirado. A detecção de mudança de
  linha (comparando `lookahead.linha` com `tagLine`) dispara
  automaticamente o `analisadorSemantico()` para processar a linha
  anterior antes de prosseguir. Isso garante que o semântico sempre
  atua sobre uma linha sintaticamente já validada.

--------------------------------------------------------------------
* DAG e Geração de Código Intermediário
--------------------------------------------------------------------

  O DAG é a estrutura de dados central para a fase de geração de
  código intermediário (código de 3 endereços). Ele é construído
  incrementalmente durante a análise sintática: cada função do parser
  retorna um ponteiro para um `DAGnode`, e os nós são conectados
  formando uma árvore que representa a estrutura do programa.

  Estrutura do nó (DAGnode):
  +------------------+------------------------------------------------+
  | Campo            | Propósito                                      |
  +------------------+------------------------------------------------+
  | type (NodeType)  | Tipo do nó (operação, variável, if, while...)  |
  | lexema[100]      | Valor do nó (nome da variável, operador, etc.) |
  | *esq             | Filho esquerdo (condição, índice, operando)    |
  | *dir             | Filho direito (corpo, operando, valor)         |
  | *body_else       | Corpo do else (reservado para extensão)        |
  | *prox            | Próximo statement na lista encadeada           |
  +------------------+------------------------------------------------+

  Tipos de nó (NodeType):
  +------------------------+------------------------------------------+
  | Tipo                   | Quando é criado                          |
  +------------------------+------------------------------------------+
  | NODE_OPERACAO          | Operador aritmético ou relacional        |
  |                        | (ex: +, -, *, <, ==)                     |
  | NODE_VARIAVEL          | Identificador ou literal de string       |
  | NODE_NUMERO            | Número inteiro, True ou False            |
  | NODE_ATRIBUICAO        | x = expr  (atribuição simples)           |
  | NODE_ATRIBUICAO_INDEXADA| x[i] = expr (atribuição com índice)     |
  | NODE_INDEX_GET         | x[i]  (leitura com índice)               |
  | NODE_IF                | Estrutura if (condição + corpo)          |
  | NODE_WHILE             | Estrutura while (condição + corpo)       |
  | NODE_FOR               | Estrutura for (iterador + limite + corpo)|
  | NODE_PRINT             | Chamada de print com lista de args       |
  | NODE_IGNORAR           | Nós descartados: len, input, lista, etc. |
  +------------------------+------------------------------------------+

  Como o DAG é construído:
  - Expressões aritméticas e relacionais são representadas como
    árvores binárias: o nó da operação tem o operador no lexema,
    o operando esquerdo em `*esq` e o direito em `*dir`.
  - Statements (atribuições, ifs, whiles, prints) são encadeados
    linearmente via `*prox`, formando a lista de instruções do programa.
  - A função `ASSIGN_OR_EXPR_TAIL` decide se cria NODE_ATRIBUICAO ou
    NODE_ATRIBUICAO_INDEXADA com base na presença de `INDEX_OPT`.
  - NODE_IF armazena a condição em `*esq` e o corpo em `*dir`.
  - NODE_WHILE armazena a condição em `*esq` e o corpo em `*dir`.
  - NODE_FOR armazena o limite do range em `*esq`, o corpo em `*dir`
    e o nome do iterador no `lexema`.
  - NODE_PRINT armazena seus argumentos como lista encadeada via `*prox`
    dentro de `*dir`.

  Geração de código de 3 endereços (gerarCodigoIntermediario):
  A função percorre o DAG em pós-ordem para expressões e em ordem
  para statements, emitindo instruções no arquivo
  `codigo_intermediario.txt`. As principais emissões são:

  - NODE_OPERACAO:
        t1 = esq OP dir
    Gera um temporário novo (t1, t2, ...) para cada operação.

  - NODE_ATRIBUICAO:
        x = valor
    Atribui diretamente o resultado da subárvore direita à variável.

  - NODE_ATRIBUICAO_INDEXADA:
        x [i] = valor
    Atribuição com índice.

  - NODE_INDEX_GET:
        t1 = x [i]
    Leitura indexada em temporário.

  - NODE_PRINT:
        param arg1
        param arg2
        call print, N
    Cada argumento gera um "param" antes da chamada.

  - NODE_IF:
        ifFalse cond goto L1
        <corpo>
        L1:

  - NODE_WHILE:
        L1:
        ifFalse cond goto L2
        <corpo>
        goto L1
        L2:

  - NODE_FOR (for x in range(limite)):
        x = 0
        L1:
        t1 = x < limite
        ifFalse t1 goto L2
        <corpo>
        t2 = x + 1
        x = t2
        goto L1
        L2:

  Temporários e rótulos:
  - Temporários são gerados pela função `novo_temp()` com contador
    global `temp_counter` (t1, t2, t3...).
  - Rótulos são gerados por `proximo_rotulo()` com contador global
    `rotulo_counter` (L1, L2, L3...).
  - Ambos os contadores são globais e incrementam monotonicamente ao
    longo de todo o programa, garantindo unicidade.

--------------------------------------------------------------------
* Complexidade do Analisador Semântico — O(n)
--------------------------------------------------------------------

  Sendo n = número total de tokens no programa fonte.

  Fase de pré-análise (dentro de analisadorSemantico):
  - `buscarNaFila()` para "input", "=", "if", "while", "for":
    realiza até 5 buscas lineares na fila da linha atual.
    Se a linha tem k tokens: O(5k) = O(k).
  - `ladoDireitoVazio`: uma varredura adicional da fila: O(k).
  - Total da pré-análise por linha: O(k).

  Loop principal de desenfileiramento:
  - Cada token é desenfileirado exatamente uma vez: O(k) por linha.
  - Para cada token, as operações são O(1) (comparações de enum e
    chamadas a `encontrarSimbolo`, que é O(s) onde s = tamanho da
    tabela de símbolos, limitado pelo número de identificadores únicos).
  - Na prática, s << n, sendo s o número de variáveis distintas do
    programa.

  Pós-processamento de escopo:
  - Varredura da tabela de símbolos para marcar ITERATIVO como
    INACESSIVEL: O(s), executado apenas ao fim de blocos de controle.

  Verificação final (possuiVarNaoUtilizada):
  - Varredura única da tabela de símbolos: O(s).

  Resumo da complexidade:

  +---------------------------------------+-------------------+
  | Operação                              | Complexidade      |
  +---------------------------------------+-------------------+
  | Processamento de cada linha (k tokens)| O(k · s)          |
  | Total para todo o programa (n tokens) | O(n · s)          |
  | Busca na tabela de símbolos           | O(s) por token    |
  | Varredura de escopo (por bloco)       | O(s)              |
  | Verificação de vars não utilizadas    | O(s)              |
  | Geral (s << n, ou s constante)        | O(n)              |
  +---------------------------------------+-------------------+

  Na prática, como s (número de identificadores únicos) é muito menor
  que n (total de tokens), o comportamento observado é linear O(n).
  O gargalo real é a busca na tabela de símbolos mantida como
  lista encadeada nesta implementação por obrigatoriedade.

  Complexidade do DAG e da geração de código:
  - Construção do DAG: O(n), um nó por token relevante.
  - Geração de código (gerarCodigoIntermediario): percorre cada nó
    do DAG exatamente uma vez em travessia recursiva: O(n).

  Complexidade espacial:
  - Fila semântica: O(k) por linha (liberada ao fim de cada linha).
  - DAG: O(n) nós alocados dinamicamente.
  - Tabela de símbolos: O(s) nós, onde s = identificadores únicos.


4. RESUMO DAS FLAGS E VARIÁVEIS DE CONTROLE
--------------------------------------------------------------------

+----------------------+----------------+----------------------------------------------+
| Variável             | Tipo           | Propósito                                    |
+----------------------+----------------+----------------------------------------------+
| flag                 | TNo *          | Aponta para o nó do lado esquerdo da         |
|                      |                | atribuição em andamento.                     |
+----------------------+----------------+----------------------------------------------+
| flagReativada        | int            | Indica que 'flag' foi redeclarada após       |
|                      |                | estar INACESSIVEL.                           |
+----------------------+----------------+----------------------------------------------+
| noLocal              | TNo *          | Aponta para variável declarada dentro        |
|                      |                | de um bloco de controle.                     |
+----------------------+----------------+----------------------------------------------+
| proxLine             | int            | Valor 1 indica que a próxima linha           |
|                      |                | pertence ao corpo de um bloco.               |
+----------------------+----------------+----------------------------------------------+
| tipoUltimaExpressao  | TipoDado       | Armazena o tipo acumulado do lado direito    |
|                      |                | durante a análise semântica.                 |
+----------------------+----------------+----------------------------------------------+
| ladoDireito          | int (local)    | 0 = lado esquerdo da atribuição              |
|                      |                | 1 = lado direito da atribuição               |
+----------------------+----------------+----------------------------------------------+
| ehLinhaDeControle    | int (local)    | Indica presença de if, while ou for          |
|                      |                | na linha atual.                              |
+----------------------+----------------+----------------------------------------------+
| tagLine              | int            | Linha atualmente acumulada na fila.          |
|                      |                | Utilizada para detectar mudança de linha.    |
+----------------------+----------------+----------------------------------------------+
| ehListaouTupla       | int (local)    | 0 = não é lista/tupla                        |
|                      |                | 1 = é lista/tupla                            |
+----------------------+----------------+----------------------------------------------+
| end                  | int            | Contador global de endereços de memória.     |
|                      |                | Incrementado a cada nova variável alocada.   |
+----------------------+----------------+----------------------------------------------+
| raizPrograma         | DAGnode *      | Raiz da árvore DAG construída pelo sintático.|
|                      |                | Passada para gerarCodigoIntermediario().     |
+----------------------+----------------+----------------------------------------------+
| temp_counter         | int            | Contador global de temporários (t1, t2...).  |
+----------------------+----------------+----------------------------------------------+
| rotulo_counter       | int            | Contador global de rótulos (L1, L2...).      |
+----------------------+----------------+----------------------------------------------+

Exemplo de Funcionamento Completo:

[ Código Fonte: x = 5 ]
        │
        ▼
┌─────────────────────────┐
│   Analisador Sintático  │ ──► Garante que a gramática está correta (ID = NUM)
│   + Construção do DAG   │     Cria: NODE_ATRIBUICAO("x") -> dir: NODE_NUMERO("5")
└─────────────────────────┘
        │ consome() enfileira na ordem de leitura
        ▼
┌─────────────────────────┐
│   Fila FIFO Semântica   │ ──► [ "x" ] ──► [ "=" ] ──► [ "5" ]
└─────────────────────────┘
        │ analisadorSemantico() ao detectar mudança de linha
        ▼
┌─────────────────────────────────────────────────────────────────────┐
│  Analisador Semântico                                               │
│                                                                     │
│  Pré-análise:                                                       │
│    - buscarNaFila("input") → NULL (não ignora)                      │
│    - buscarNaFila("=")     → encontrado → temAtribuicao = 1         │
│    - buscarNaFila("if/while/for") → NULL → ehLinhaDeControle = 0    │
│                                                                     │
│  Loop principal:                                                    │
│  1. Token "x" (IDENTIFICADOR, lado esquerdo)                        │
│     → UNKNOWN → vira NAO_DEFINIDO                                   │
│     → flag = &x, endereco = end++ (end vira 1)                      │
│  2. Token "=" (ATRIBUICAO)                                          │
│     → ladoDireito = 1                                               │
│     → próximo token não é '[' nem '(' → ehListaouTupla = 0          │
│  3. Token "5" (NUMERO, lado direito)                                │
│     → no da tabela: tipo UNKNOWN → vira INTEIRO, struc = VALOR      │
│     → tipoUltimaExpressao = INTEIRO                                 │
│                                                                     │
│  Pós-loop (propagação):                                             │
│    → flag != NULL, struc != LISTA/TUPLA                             │
│    → flag->tipo = tipoUltimaExpressao → x vira INTEIRO              │
│    → flag = NULL                                                    │
└─────────────────────────────────────────────────────────────────────┘
        │
        ▼ (após todas as linhas e analisadorSemantico)
┌─────────────────────────┐
│  Geração de Código      │ ──► gerarCodigoIntermediario(raizPrograma)
│  Intermediário (DAG)    │
│                         │     Percorre: NODE_ATRIBUICAO("x")
│                         │       dir → NODE_NUMERO("5") → retorna "5"
│                         │     Emite: "x = 5"
└─────────────────────────┘

* Tabela de Símbolos:
  Estrutura utilizada:
  - Lista encadeada simples (linked list), global e única.

  Informações armazenadas por nó (TNo):
  - Cadeia   → lexema (nome da variável, número ou literal)
  - Atomo    → classe do token (IDENTIFICADOR, NUMERO, LITERAL...)
  - Tipo     → UNKNOWN, NAO_DEFINIDO, INTEIRO, BOOLEANO
  - Estado   → ACESSIVEL ou INACESSIVEL (controle de escopo)
  - Estrutura→ VARIAVEL, VALOR, LISTA, TUPLA ou ITERATIVO
  - Usado    → 1 se foi lida no lado direito de alguma expressão
  - Endereco → posição de memória (-1 se não endereçada)

  Características:
  - Integração com léxico (inserção na adicionarSimbolo) e semântico
    (atualização dinâmica de tipos, estado e endereço).
  - Deduplicação: símbolos duplicados são ignorados na inserção.
  - Atualização dinâmica de tipos realizada pelo semântico.
  - Controle de acessibilidade por escopo realizado ao fim de blocos.


5. BUGS, ERROS CONHECIDOS E LIMITAÇÕES
--------------------------------------------------------------------
Após avaliação inicial pela professora, o código falhou diversos
testes, entre eles:
  - input() com argumento (literal).
  - print() com tupla de argumentos: o print deve aceitar um literal
    seguido de uma tupla de argumentos para indicar variáveis.
  - Tabela de símbolos mostrava apenas identificadores. Agora a tabela
    possui números e identificadores.