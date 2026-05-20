====================================================================
          COMPILADOR MINI-PYTHON - ANALISADOR LÉXICO E SINTÁTICO
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
  - Utilizamos uma estrutura FIFO (First-In First-Out), ou seja, uma fila, para auxiliar o processo de análise.
  - A escolha dessa estrutura reflete a forma natural de leitura do código: da esquerda para a direita.
  - Como o analisador sintático já garantiu que não há erros estruturais, o semântico assume essa etapa concluída e processa cada linha em busca de significado, separando a lógica em lado esquerdo e lado direito da expressão.
  - O processo semântico se inicia quando o sintático consome um token e o enfileira para o semântico.
  - Ao término da linha, com a análise sintática confirmada, o analisador semântico começa a desenfileirar os tokens.
  - Utilizamos uma flag para referenciar o nó do lado esquerdo da atribuição (ou o escopo local vigente).
  - Por se tratar de uma linguagem não tipada, a variável do lado esquerdo é registrada inicialmente como NAO_DEFINIDO, aguardando a avaliação do lado direito. 
  - A lógica de lados funciona da seguinte forma: o lado esquerdo é definido pelo lado direito. Por exemplo, em x = 5: x recebe o estado NAO_DEFINIDO e a flag aponta para ele; ao encontrar =, o modo muda para lado direito; ao encontrar 5, o tipo INTEIRO é propagado para a flag.
  - Toda expressão do lado direito é verificada para garantir que todos os operandos possuam o mesmo tipo — mistura de tipos ou uso de variáveis não definidas gera erro. 
  - Chamadas a funções como input são ignoradas na verificação de tipos.
  - Implementamos um mini escopo como exceção ao escopo global: ao encontrar uma estrutura de controle (if, while, for), a próxima linha passa a ser tratada como corpo do bloco.
  - Caso uma variável seja declarada dentro desse bloco, ela é registrada como noLocal.
  - Ao sair do bloco, esse conceito de acessível e inacessível entra em ação: a variável declarada localmente se torna inacessível — como se deixasse de existir — mas pode ser sobrescrita por uma nova declaração externa.  - ignora funções como input 
  - No caso do for, o iterador i é presumido como INTEIRO e marcado como ITERATIVO durante o bloco; ao término, torna-se inacessível.

* Tabela de símbolos:
  - Como descrito na análise semântica, o único caso de escopo restrito é o mini escopo das estruturas de controle. 
  - Por isso, não faz sentido criar tabelas de símbolos separadas por bloco, o que apenas aumentaria a complexidade com ponteiros adicionais
  - Por se tratar de uma linguagem com muitas restrições — especialmente a regra de que toda variável, uma vez atribuída, deve sempre receber o mesmo tipo — uma única tabela de símbolos global é suficiente para atender a todos os casos.
  - Esses fatores levaram à decisão de implementar uma única tabela de símbolos global, sem necessidade de estrutura em pilha.



2. COMO EXECUTAR O CÓDIGO
--------------------------------------------------------------------
Pré-requisitos: Ter um compilador C (como o GCC) instalado.

gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador

./compilador code.mp

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