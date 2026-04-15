====================================================================
          COMPILADOR MINI-PYTHON - ANALISADOR LÉXICO E SINTÁTICO
====================================================================

1. O QUE FOI IMPLEMENTADO ATÉ O MOMENTO
--------------------------------------------------------------------
O projeto consiste nas duas primeiras fases de um compilador para uma 
linguagem baseada em Python (Mini-Python), desenvolvidas em C puro:

* Analisador Léxico:
  - Leitura do arquivo fonte caractere a caractere.
  - Reconhecimento e classificação de tokens usando lógica de autômatos
    finitos (sem uso de bibliotecas de regex externas).
  - Suporte a tokens: ID, NUMERO_INTEIRO, LITERAL (strings), KEYWORD,
    OP_RELACIONAL, OP_ARITMETICO, DELIMITER, COMMENT e NEWLINE.
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
* Bufferização de Tokens: Em vez de acoplar estritamente o léxico ao 
  sintático solicitando um token por vez sob demanda, optou-se por 
  coletar todos os tokens válidos em um `bufferTokens` na função main. 
  Isso facilita a gerência do lookahead e a separação de responsabilidades.

* Divisão de Palavras-chave na Gramática: Para acomodar as diversas 
  palavras-chave da linguagem, elas foram divididas sintaticamente em 
  duas categorias:
  - COMMAND_STATEMENTS: Funções e palavras reservadas que representam 
    uma instrução completa (ex: print, break).
  - TERMS: Funções intrínsecas que devem ser processadas dentro de 
    expressões por retornarem valores (ex: len, input).

4. BUGS, ERROS CONHECIDOS E LIMITAÇÕES
--------------------------------------------------------------------
Ao longo da implementação, diversos bugs foram encontrados e corrigidos, mas no estado atual,
os analisadores aparentam corretamente definer a linguagem nas limitações que ela possui.