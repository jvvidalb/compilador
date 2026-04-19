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
  - Tabela de símbolos tem apenas identificadores. Agora a tabela possui literais, 
  numeros e identificadores.