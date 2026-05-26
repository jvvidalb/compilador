# Compilador Mini-Python
## Analisador Léxico, Sintático e Semântico

[cite_start]Este projeto consiste no desenvolvimento das três primeiras fases de um compilador para uma linguagem baseada em Python (Mini-Python), desenvolvidas em C puro, além da geração de código intermediário (fase 4)[cite: 1].

---

## 1. O Que Foi Implementado

### 🔍 Analisador Léxico
* [cite_start]**Carregamento em Memória:** O arquivo fonte é carregado em um array baseado nos espaços[cite: 1].
* [cite_start]**Reconhecimento de Tokens:** Classificação de tokens utilizando a lógica de autômatos finitos, sem depender de bibliotecas externas de regex[cite: 2].
* [cite_start]**Tokens Suportados:** `ID`, `NUMERO_INTEIRO`, `LITERAL` (strings), `KEYWORD`, `OP_RELACIONAL`, `OP_ARITMETICO` e `DELIMITER`[cite: 3].
* [cite_start]**Tabela de Símbolos:** Preenchimento inicial para identificadores, números, literais e operadores[cite: 4].
* [cite_start]**Tratamento de Erros:** Reporte de erros léxicos, como caracteres não separados de forma válida[cite: 5].

### 📐 Analisador Sintático
* [cite_start]**Abordagem:** Implementação *top-down* (descendente recursivo) baseada em uma Gramática Livre de Contexto (GLC)[cite: 6].
* [cite_start]**Estruturas de Fluxo:** Reconhecimento de `IF`, `ELIF`, `ELSE`, `WHILE` e `FOR`[cite: 7].
* [cite_start]**Expressões e Atribuições:** Reconhecimento de atribuições e expressões matemáticas/relacionais[cite: 8].
* [cite_start]**Comandos e Funções:** Suporte a comandos de linha única (*Statements*: `print`, `break`, `continue`, `return`, `exec`, `raise`) e funções integradas (*Terms*: `len`, `input`)[cite: 8].
* [cite_start]**Construção do DAG/AST:** Geração simultânea do Grafo Acíclico Direcionado (DAG) / Árvore Sintática Abstrata (AST) nó a nó à medida que os tokens são consumidos[cite: 9].

### 🧠 Analisador Semântico
[cite_start]O analisador utiliza uma estrutura de **Fila FIFO (First-In First-Out)** para garantir o processamento linear e o consumo dos tokens da esquerda para a direita na ordem exata de leitura[cite: 10, 11, 12].

* [cite_start]**Objetivo:** Validar o significado do código após a garantia estrutural do sintático, verificando tipos, controlando escopo e detectando inconsistências (como uso de variáveis antes da definição ou não utilizadas)[cite: 13, 14].
* [cite_start]**Funcionamento por Linha:** Os tokens são acumulados na fila durante o *parsing* e processados ao fim de cada linha (identificado pela mudança na flag `tagLine`) ou ao atingir o fim do arquivo (EOS)[cite: 16].