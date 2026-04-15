/*
-------------------------------------------------------------------------------
-                       COMPILADOR MINI-PYTHON                             -
-------------------------------------------------------------------------------

DESENVOLVIDO POR:
Henrique Brainer Costa 10420717
João Pedro Queiroz de Andrade 10425822
João Victor Vidal Barbosa 10410165

Para Compilar:
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador

Para Executar:
./compilador code.mp

DOCUMENTACA0 FORMAL:

A entrega 1 do projeto consiste na implementacao de um analisador sintatico e lexico
da linguagem Mini-Python, que e um subconjunto da linguagem Python. 
O analisador lexico e responsavel por ler o codigo fonte e gerar uma sequencia de tokens, 
que sao as unidades lexicais da linguagem. O analisador sintatico, por sua vez, recebe essa
sequencia de tokens e verifica se ela esta de acordo com a gramatica da linguagem, 
construindo uma arvore sintatica  ou reportando erros sintaticos caso haja alguma violacao.

DEFINICAO DA GRAMATICA FORMAL:

Usando o padrão POSIX para reconhecimento de REGEX

Expressões Regulares:
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*
LITERAL: "[^"]*" 
KEYWORD: return|from|while|as|elif|with|else|if|break|len|input|print|exec|raise|continue|range|def|for|True|False
OPERATOR: \*\*|!=|<>|==|<=|>=|\+|-|~|\*|\/|%|and|or|not|in|is|<|>
DELIMITER: \(|\)|\[|\]|\{|\}|,|:|\.|=|; 
NUMBER -> [0-9]+
NEWLINE -> (\r\n|\r|\n)
INDENT -> ^ {4}
COMMENT -> #[^\r\n]*

**O automato finito responsavel por ler essa linguagem está incluso no arquivo 
FinalSimplificado.jff -> Execute pelo JFlap**

GRAMÁTICA LIVRE DE CONTEXTO
G = (N,T,R,START)

tal que:

N = {START,STATEMENTS,STATEMENTS_PRIME,STATEMENT,ASSIGN_OR_EXPR,ASSIGN_OR_EXPR_TAIL,IF_STATEMENT,IF_START,IF_TAIL,ELIF_STATEMENTS,ELIF_STATEMENT,ELIF_STATEMENTS_PRIME,ELIF_TAIL,WHILE_STATEMENT,FOR_STATEMENT,EXPRESSION,EXPRESSION_PRIME,TERM}
T={IDENTIFIER,NUMBER,LITERAL,OPERATOR,NEWLINE,INDENT,if,elif,else,while,for,in,range,=,:,(,)}

R = {

START -> STATEMENTS

STATEMENTS -> STATEMENT | STATEMENT NEWLINE STATEMENTS

STATEMENT -> IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | ASSIGNMENT | EXPRESSION

START -> STATEMENTS


STATEMENTS -> STATEMENT STATEMENTS_PRIME
STATEMENTS_PRIME -> NEWLINE STATEMENTS | ε


STATEMENT -> IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | ASSIGN_OR_EXPR | COMMAND_STATEMENT


ASSIGN_OR_EXPR -> IDENTIFIER ASSIGN_OR_EXPR_TAIL | NUMBER EXPRESSION_PRIME | LITERAL EXPRESSION_PRIME | ( EXPRESSION ) EXPRESSION_PRIME
ASSIGN_OR_EXPR_TAIL -> = EXPRESSION | EXPRESSION_PRIME

IF_STATEMENT -> IF_START IF_TAIL
IF_START -> if EXPRESSION : NEWLINE INDENT STATEMENT

IF_TAIL -> NEWLINE INDENT else STATEMENT NEWLINE INDENT | ELIF_STATEMENTS NEWLINE INDENT STATEMENT NEWLINE INDENT ELIF_TAIL | ε
ELIF_TAIL -> else STATEMENT NEWLINE INDENT | ε

ELIF_STATEMENTS -> ELIF_STATEMENT ELIF_STATEMENTS_PRIME
ELIF_STATEMENT -> elif EXPRESSION : NEWLINE INDENT STATEMENT
ELIF_STATEMENTS_PRIME -> ELIF_STATEMENTS | ε

WHILE_STATEMENT -> while EXPRESSION : NEWLINE INDENT STATEMENT 

FOR_STATEMENT -> for IDENTIFIER in range ( EXPRESSION ) : NEWLINE INDENT STATEMENT

COMMAND_STATEMENT -> print ( EXPRESSION ) | break | continue | return EXPRESSION | exec EXPRESSION | raise EXPRESSION

EXPRESSION -> TERM EXPRESSION_PRIME
EXPRESSION_PRIME -> OPERATOR TERM NEWLINE EXPRESSION_PRIME | ε


TERM -> IDENTIFICADOR | NUMERO | LITERAL | TRUE | FALSE | LIST | TUPLE_OR_GROUP | len ( EXPRESSION ) | input ( )

LIST -> [ ELEMENTS_OPT ]
TUPLE_OR_GROUP -> ( ELEMENTS_OPT )

ELEMENTS_OPT -> EXPRESSION ELEMENTS_TAIL | ε
ELEMENTS_TAIL -> , EXPRESSION ELEMENTS_TAIL | ε

}

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definição do Enum de Tokens (TAtomo)
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    LITERAL,
    EOS, // Equivalente a FIM_ARQUIVO
    NEWLINE,
    COMMENT,
    OP_RELACIONAL,
    OP_ARITMETICO,
    ATRIBUICAO,
    DELIMITER,
    KEYWORD,
    // Palavras-chave específicas
    T_IF, T_ELIF, T_ELSE, T_WHILE, T_FOR, T_DEF,
    T_RETURN, T_BREAK, T_CONTINUE, T_PRINT, T_INPUT,
    T_LEN, T_RANGE, T_TRUE, T_FALSE,
    DESCONHECIDO
} TAtomo;

// Definição de tipos
typedef struct {
    char lexema[100];
    TAtomo tipo;
    int linha;
} Token;

// Variáveis globais do analisador léxico
FILE *fonte;
int linhaAtual = 1;
// variável global para controle de linhas em branco
int ultimo_token_foi_newline = 1; // Começa em 1 para ignorar newlines logo na primeira linha do arquivo

// Tabela de símbolos
typedef struct {
    char valor[100];
    TAtomo tipo;
} SimboloTabela;

#define MAX_SIMBOLOS 1000
#define MAX_TOKENS 5000
SimboloTabela tabelaSimbolos[MAX_SIMBOLOS];
int totalSimbolos = 0;

// Buffer de tokens para o analisador sintático
Token bufferTokens[MAX_TOKENS];
int totalTokens = 0;
int posicaoAtual = 0;

// Lookahead para o analisador sintático
Token lookahead;

// Funções do analisador léxico
// Funções do analisador léxico
void iniciarAnalisador(FILE *arquivo);
int fimDoArquivo();
Token _proximoTokenInterno(); // <-- Nome alterado aqui
Token proximoToken();         // <-- Novo protótipo adicionado
TAtomo classificarLexema(char *lexema);
void fecharAnalisador();
const char* atomoParaString(TAtomo atomo);

// Funções auxiliares léxicas
int isLetra(char c);
int isDigito(char c);
int isSeparadorValido(char c);

// Funções do analisador sintático
void iniciarAnalisadorSintaticoComTokens();
void proximoTokeneAtualizarLookahead();
void consome(TAtomo tipoEsperado);
void erroSintatico(TAtomo tipoEsperado);

// Funções recursivas do analisador sintático
void START();
void STATEMENTS();
void STATEMENTS_PRIME();
void STATEMENT();
void ASSIGN_OR_EXPR();
void ASSIGN_OR_EXPR_TAIL();
void IF_STATEMENT();
void IF_START();
void IF_TAIL();
void ELIF_STATEMENTS();
void ELIF_STATEMENT();
void ELIF_STATEMENTS_PRIME();
void ELIF_TAIL();
void WHILE_STATEMENT();
void FOR_STATEMENT();
void EXPRESSION();
void EXPRESSION_PRIME();
void TERM();
void LIST();
void ELEMENTS_OPT();
void ELEMENTS_TAIL();

// Implementação das funções auxiliares
const char* atomoParaString(TAtomo atomo) {
    switch(atomo) {
        case ERRO: return "ERRO_LEXICO";
        case IDENTIFICADOR: return "ID";
        case NUMERO: return "NUMERO_INTEIRO";
        case LITERAL: return "LITERAL";
        case EOS: return "FIM_ARQUIVO";
        case NEWLINE: return "NEWLINE";
        case COMMENT: return "COMMENT";
        case OP_RELACIONAL: return "OP_RELACIONAL";
        case OP_ARITMETICO: return "OP_ARITMETICO";
        case ATRIBUICAO: return "ATRIBUICAO";
        case DELIMITER: return "DELIMITER";
        case KEYWORD: return "KEYWORD";
        case T_IF: return "IF";
        case T_ELIF: return "ELIF";
        case T_ELSE: return "ELSE";
        case T_WHILE: return "WHILE";
        case T_FOR: return "FOR";
        case T_DEF: return "DEF";
        case T_RETURN: return "RETURN";
        case T_BREAK: return "BREAK";
        case T_CONTINUE: return "CONTINUE";
        case T_PRINT: return "PRINT";
        case T_INPUT: return "INPUT";
        case T_LEN: return "LEN";
        case T_RANGE: return "RANGE";
        case T_TRUE: return "TRUE";
        case T_FALSE: return "FALSE";
        case DESCONHECIDO: return "DESCONHECIDO";
        default: return "INDEFINIDO";
    }
}

int isLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int isDigito(char c) {
    return c >= '0' && c <= '9';
}

int isSeparadorValido(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == EOF;
}

char* obterNomeOperador(char *op) {
    static char nome[20];
    if (strcmp(op, ">") == 0) strcpy(nome, "GT");
    else if (strcmp(op, "<") == 0) strcpy(nome, "LT");
    else if (strcmp(op, ">=") == 0) strcpy(nome, "GE");
    else if (strcmp(op, "<=") == 0) strcpy(nome, "LE");
    else if (strcmp(op, "==") == 0) strcpy(nome, "EQ");
    else if (strcmp(op, "!=") == 0) strcpy(nome, "NE");
    else if (strcmp(op, "<>") == 0) strcpy(nome, "NE");
    else if (strcmp(op, "+") == 0) strcpy(nome, "SOMA");
    else if (strcmp(op, "-") == 0) strcpy(nome, "SUBTRACAO");
    else if (strcmp(op, "*") == 0) strcpy(nome, "MULTIPLICACAO");
    else if (strcmp(op, "**") == 0) strcpy(nome, "POTENCIA");
    else if (strcmp(op, "/") == 0) strcpy(nome, "DIVISAO");
    else if (strcmp(op, "%") == 0) strcpy(nome, "MODULO");
    else if (strcmp(op, "~") == 0) strcpy(nome, "NEGACAO");
    else strcpy(nome, op);
    return nome;
}

int adicionarSimbolo(char *valor, TAtomo tipo) {
    for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].valor, valor) == 0) {
            return i;
        }
    }

    if (totalSimbolos < MAX_SIMBOLOS) {
        strcpy(tabelaSimbolos[totalSimbolos].valor, valor);
        tabelaSimbolos[totalSimbolos].tipo = tipo;
        return totalSimbolos++;
    }
    return -1;
}

TAtomo classificarLexema(char *lexema) {
    if (isDigito(lexema[0])) return NUMERO;
    
    if (strcmp(lexema, "if") == 0) return T_IF;
    if (strcmp(lexema, "elif") == 0) return T_ELIF;
    if (strcmp(lexema, "else") == 0) return T_ELSE;
    if (strcmp(lexema, "while") == 0) return T_WHILE;
    if (strcmp(lexema, "for") == 0) return T_FOR;
    if (strcmp(lexema, "def") == 0) return T_DEF;
    if (strcmp(lexema, "return") == 0) return T_RETURN;
    if (strcmp(lexema, "break") == 0) return T_BREAK;
    if (strcmp(lexema, "continue") == 0) return T_CONTINUE;
    if (strcmp(lexema, "print") == 0) return T_PRINT;
    if (strcmp(lexema, "input") == 0) return T_INPUT;
    if (strcmp(lexema, "len") == 0) return T_LEN;
    if (strcmp(lexema, "range") == 0) return T_RANGE;
    if (strcmp(lexema, "True") == 0) return T_TRUE;
    if (strcmp(lexema, "False") == 0) return T_FALSE;
    
    if (strcmp(lexema, "as") == 0 || strcmp(lexema, "from") == 0 ||
        strcmp(lexema, "with") == 0 || strcmp(lexema, "exec") == 0 ||
        strcmp(lexema, "raise") == 0 || strcmp(lexema, "and") == 0 ||
        strcmp(lexema, "or") == 0 || strcmp(lexema, "not") == 0 ||
        strcmp(lexema, "in") == 0 || strcmp(lexema, "is") == 0) {
        return KEYWORD;
    }
    
    return IDENTIFICADOR;
}

void iniciarAnalisador(FILE *arquivo) {
    fonte = arquivo;
    linhaAtual = 1;
}

int fimDoArquivo() {
    return feof(fonte);
}

Token _proximoTokenInterno() {
    Token token;
    token.tipo = DESCONHECIDO;
    strcpy(token.lexema, "");
    token.linha = linhaAtual;

    int i = 0;
    char c;

    while ((c = fgetc(fonte)) != EOF && (c == ' ' || c == '\t')) {}

    if (c == EOF) {
        strcpy(token.lexema, "EOF");
        token.tipo = EOS;
        return token;
    }

    if (c == '\n') {
        linhaAtual++;
        strcpy(token.lexema, "\n");
        token.tipo = NEWLINE;
        return token;
    }

    if (c == '\r') {
        char next = fgetc(fonte);
        if (next == '\n') linhaAtual++;
        else {
            if (next != EOF) ungetc(next, fonte);
            linhaAtual++;
        }
        strcpy(token.lexema, "\n");
        token.tipo = NEWLINE;
        return token;
    }

    if (c == '#') {
        token.lexema[i++] = c;
        while ((c = fgetc(fonte)) != EOF && c != '\n') token.lexema[i++] = c;
        if (c == '\n') ungetc(c, fonte);
        token.lexema[i] = '\0';
        token.tipo = COMMENT;
        return token;
    }

    if (c == '"') {
        token.lexema[i++] = c;
        while ((c = fgetc(fonte)) != EOF && c != '"') token.lexema[i++] = c;
        if (c == '"') token.lexema[i++] = c;
        token.lexema[i] = '\0';
        token.tipo = LITERAL;
        return token;
    }

    if (isDigito(c)) {
        token.lexema[i++] = c;
        while ((c = fgetc(fonte)) != EOF && isDigito(c)) token.lexema[i++] = c;

        if (c != EOF && !isSeparadorValido(c)) {
            token.tipo = ERRO;
            token.lexema[i] = '\0';
            sprintf(token.lexema + i, " [ERRO: tokens nao separados]");
            return token;
        }

        if (c != EOF) ungetc(c, fonte);
        token.lexema[i] = '\0';
        token.tipo = NUMERO;
        return token;
    }

    if (isLetra(c)) {
        token.lexema[i++] = c;
        while ((c = fgetc(fonte)) != EOF && (isLetra(c) || isDigito(c))) token.lexema[i++] = c;

        if (c != EOF && !isSeparadorValido(c)) {
            token.tipo = ERRO;
            token.lexema[i] = '\0';
            sprintf(token.lexema + i, " [ERRO: tokens nao separados]");
            return token;
        }

        if (c != EOF) ungetc(c, fonte);
        token.lexema[i] = '\0';
        token.tipo = classificarLexema(token.lexema);
        return token;
    }

    token.lexema[i++] = c;
    char next = fgetc(fonte);

    if ((c == '*' && next == '*') || (c == '!' && next == '=') ||
        (c == '<' && next == '>') || (c == '=' && next == '=') ||
        (c == '<' && next == '=') || (c == '>' && next == '=')) {
        token.lexema[i++] = next;
        token.lexema[i] = '\0';

        char after = fgetc(fonte);
        if (after != EOF && !isSeparadorValido(after)) {
            token.tipo = ERRO;
            strcpy(token.lexema, "[ERRO: tokens nao separados]");
            return token;
        }
        if (after != EOF) ungetc(after, fonte);

        if (strncmp(token.lexema, "<", 1) == 0 || strncmp(token.lexema, ">", 1) == 0 ||
            strcmp(token.lexema, "==") == 0 || strcmp(token.lexema, "!=") == 0 ||
            strcmp(token.lexema, "<>") == 0) {
            token.tipo = OP_RELACIONAL;
        } else {
            token.tipo = OP_ARITMETICO;
        }
        return token;
    }

    if (c == '+' || c == '-' || c == '~' || c == '*' || c == '/' ||
        c == '%' || c == '<' || c == '>' || c == '=') {
        token.lexema[i] = '\0';

        if (next != EOF && !isSeparadorValido(next)) {
            token.tipo = ERRO;
            strcpy(token.lexema, "[ERRO: tokens nao separados]");
            return token;
        }

        if (next != EOF) ungetc(next, fonte);

        if (c == '<' || c == '>') token.tipo = OP_RELACIONAL;
        else if (c == '=') token.tipo = ATRIBUICAO;
        else token.tipo = OP_ARITMETICO;
        
        return token;
    }

    if (next != EOF) ungetc(next, fonte);

    if (c == '(' || c == ')' || c == '[' || c == ']' ||
        c == '{' || c == '}' || c == ',' || c == ':' ||
        c == '.' || c == ';') {
        token.lexema[i] = '\0';

        char after = fgetc(fonte);
        if (after != EOF && !isSeparadorValido(after)) {
            token.tipo = ERRO;
            strcpy(token.lexema, "[ERRO: tokens nao separados]");
            return token;
        }
        if (after != EOF) ungetc(after, fonte);

        token.tipo = DELIMITER;
        return token;
    }

    token.lexema[i] = '\0';
    token.tipo = DESCONHECIDO;
    return token;
}

Token proximoToken() {
    Token token;
    
    while (1) {
        token = _proximoTokenInterno();
        
        if (token.tipo == NEWLINE) {
            // Se o último token também foi NEWLINE (ou se estamos no topo do arquivo),
            // ignoramos este token e rodamos o loop para pegar o próximo.
            if (ultimo_token_foi_newline) {
                continue; 
            }
            ultimo_token_foi_newline = 1; // Marca que acabamos de emitir um NEWLINE estrutural
        } else if (token.tipo != COMMENT) {
            // Qualquer token de código válido (ID, LITERAL, NUMERO, etc) zera a flag.
            // O tipo COMMENT não zera a flag, o que garante que linhas com apenas comentários
            // não quebrem a compressão de linhas vazias.
            ultimo_token_foi_newline = 0;
        }
        
        return token;
    }
}

void fecharAnalisador() {
    if (fonte != NULL) fclose(fonte);
}

// ============ ANALISADOR SINTÁTICO ============

void iniciarAnalisadorSintaticoComTokens() {
    posicaoAtual = 0;
    if (totalTokens > 0) lookahead = bufferTokens[posicaoAtual];
}

void proximoTokeneAtualizarLookahead() {
    posicaoAtual++;
    if (posicaoAtual < totalTokens) {
        lookahead = bufferTokens[posicaoAtual];
    } else {
        lookahead.tipo = EOS;
        strcpy(lookahead.lexema, "EOF");
    }
}

void consome(TAtomo tipoEsperado) {
    if (lookahead.tipo == tipoEsperado) {
        proximoTokeneAtualizarLookahead();
    } else {
        erroSintatico(tipoEsperado);
    }
}

void erroSintatico(TAtomo tipoEsperado) {
    printf("ERRO SINTATICO NA LINHA %d: esperado [%s], encontrado [%s] lexema: %s\n",
           lookahead.linha, atomoParaString(tipoEsperado), atomoParaString(lookahead.tipo), lookahead.lexema);
    exit(1);
}

// Implementação das funções sintáticas usando Enum
void START() {
    STATEMENTS();
}

void STATEMENTS() {
    STATEMENT();
    STATEMENTS_PRIME();
}

void STATEMENTS_PRIME() {
    if (lookahead.tipo == NEWLINE) {
        consome(NEWLINE);
        STATEMENTS();
    }
}

void STATEMENT() {
    if (lookahead.tipo == T_IF) IF_STATEMENT();
    else if (lookahead.tipo == T_WHILE) WHILE_STATEMENT();
    else if (lookahead.tipo == T_FOR) FOR_STATEMENT();
    else if (lookahead.tipo == T_PRINT) {
        consome(T_PRINT);
        consome(DELIMITER); 
        EXPRESSION();
        consome(DELIMITER); 
    } 
    else if (lookahead.tipo == T_BREAK) consome(T_BREAK);
    else if (lookahead.tipo == T_CONTINUE) consome(T_CONTINUE);
    else if (lookahead.tipo == T_RETURN) {
        consome(T_RETURN);
        EXPRESSION();
    } 
    else if (lookahead.tipo == KEYWORD && (strcmp(lookahead.lexema, "exec") == 0 || strcmp(lookahead.lexema, "raise") == 0)) {
        consome(KEYWORD);
        EXPRESSION();
    } 
    else {
        ASSIGN_OR_EXPR();
    }
}

void ASSIGN_OR_EXPR() {
    if (lookahead.tipo == IDENTIFICADOR) {
        consome(IDENTIFICADOR);
        ASSIGN_OR_EXPR_TAIL();
    } else if (lookahead.tipo == NUMERO) {
        consome(NUMERO);
        EXPRESSION_PRIME();
    } else if (lookahead.tipo == LITERAL) {
        consome(LITERAL);
        EXPRESSION_PRIME();
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0) {
        consome(DELIMITER); // '('
        ELEMENTS_OPT();
        consome(DELIMITER); // ')'
        EXPRESSION_PRIME();
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0) {
        LIST();
        EXPRESSION_PRIME();
    } else {
        erroSintatico(IDENTIFICADOR);
    }
}

void ASSIGN_OR_EXPR_TAIL() {
    if (lookahead.tipo == ATRIBUICAO) {
        consome(ATRIBUICAO);
        EXPRESSION();
    } else {
        EXPRESSION_PRIME();
    }
}

void IF_STATEMENT() {
    IF_START();
    IF_TAIL();
}

void IF_START() {
    consome(T_IF);
    EXPRESSION();
    consome(DELIMITER);
    consome(NEWLINE);
    STATEMENT();
}

void IF_TAIL() {
    if (lookahead.tipo == NEWLINE) {
        consome(NEWLINE);
        if (lookahead.tipo == T_ELSE) {
            consome(T_ELSE);
            consome(DELIMITER);
            consome(NEWLINE);
            STATEMENT();
            consome(NEWLINE);
        } else if (lookahead.tipo == T_ELIF) {
            ELIF_STATEMENTS();
            consome(NEWLINE);
            consome(T_ELSE);
            consome(DELIMITER);
            consome(NEWLINE);
            STATEMENT();
            consome(NEWLINE);
            ELIF_TAIL();
        }
    }
}

void ELIF_STATEMENTS() {
    ELIF_STATEMENT();
    ELIF_STATEMENTS_PRIME();
}

void ELIF_STATEMENT() {
    consome(T_ELIF);
    EXPRESSION();
    consome(DELIMITER);
    consome(NEWLINE);
    STATEMENT();
}

void ELIF_STATEMENTS_PRIME() {
    if (lookahead.tipo == T_ELIF) {
        ELIF_STATEMENTS();
    }
}

void ELIF_TAIL() {
    if (lookahead.tipo == T_ELSE) {
        consome(T_ELSE);
        consome(DELIMITER);
        consome(NEWLINE);
        STATEMENT();
        consome(NEWLINE);
    }
}

void WHILE_STATEMENT() {
    consome(T_WHILE);
    EXPRESSION();
    consome(DELIMITER);
    consome(NEWLINE);
    STATEMENT();
}

void FOR_STATEMENT() {
    consome(T_FOR);
    consome(IDENTIFICADOR);
    consome(KEYWORD);
    consome(T_RANGE);
    consome(DELIMITER);
    EXPRESSION();
    consome(DELIMITER);
    consome(DELIMITER);
    consome(NEWLINE);
    STATEMENT();
}

void EXPRESSION() {
    TERM();
    EXPRESSION_PRIME();
}

void EXPRESSION_PRIME() {
    if (lookahead.tipo == OP_RELACIONAL || lookahead.tipo == OP_ARITMETICO) {
        consome(lookahead.tipo);
        TERM();
        EXPRESSION_PRIME();
    }
}

void TERM() {
    if (lookahead.tipo == IDENTIFICADOR) {
        consome(IDENTIFICADOR);
    } else if (lookahead.tipo == NUMERO) {
        consome(NUMERO);
    } else if (lookahead.tipo == LITERAL) {
        consome(LITERAL);
    } else if (lookahead.tipo == T_TRUE) {
        consome(T_TRUE);
    } else if (lookahead.tipo == T_FALSE) {
        consome(T_FALSE);
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0) {
        consome(DELIMITER); // '('
        ELEMENTS_OPT();     // Trata tanto expressao isolada (x) quanto tupla (x,y) ou ()
        consome(DELIMITER); // ')'
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0) {
        LIST();             // Chama a nova regra de lista
    } else if (lookahead.tipo == T_LEN) {
        consome(T_LEN);
        consome(DELIMITER); 
        EXPRESSION();
        consome(DELIMITER);
    } else if (lookahead.tipo == T_INPUT) {
        consome(T_INPUT);
        consome(DELIMITER); 
        consome(DELIMITER);
    } else {
        erroSintatico(IDENTIFICADOR);
    }
}

void LIST() {
    consome(DELIMITER); // Consome '['
    ELEMENTS_OPT();
    consome(DELIMITER); // Consome ']'
}

void ELEMENTS_OPT() {
    // Verifica se o lookahead faz parte do conjunto FIRST(EXPRESSION)
    if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || 
        lookahead.tipo == LITERAL || lookahead.tipo == T_TRUE || 
        lookahead.tipo == T_FALSE || lookahead.tipo == T_LEN || 
        lookahead.tipo == T_INPUT || 
        (lookahead.tipo == DELIMITER && (strcmp(lookahead.lexema, "(") == 0 || strcmp(lookahead.lexema, "[") == 0))) {
        EXPRESSION();
        ELEMENTS_TAIL();
    }
    // ε (Vazio: permite listas e tuplas vazias como [] e ())
}

void ELEMENTS_TAIL() {
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, ",") == 0) {
        consome(DELIMITER); // Consome ','
        EXPRESSION();
        ELEMENTS_TAIL();
    }
    // ε
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        printf("Analisa lexica e sintaticamente um arquivo fonte em Mini-Python.\n");
        return 0;
    }else if (argc > 2) {
        printf("Erro: Muitos argumentos fornecidos.\n");
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    }

    fonte = fopen(argv[1], "r");
    if (fonte == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    iniciarAnalisador(fonte);

    printf("===== ANALISE LEXICA =====\n");
    while (!fimDoArquivo()) {
        Token token = proximoToken();

        if (token.tipo == COMMENT || token.tipo == DESCONHECIDO || token.tipo == EOS) {
            if (token.tipo == EOS) {
                bufferTokens[totalTokens++] = token;
            }
            continue;
        }

        if (token.tipo == ERRO) {
            printf("ERRO LEXICO NA LINHA %d: %s\n", token.linha, token.lexema);
            fecharAnalisador();
            return 1;
        }

        bufferTokens[totalTokens++] = token;

        if (token.tipo == IDENTIFICADOR || token.tipo == NUMERO || token.tipo == LITERAL) {
            int indice = adicionarSimbolo(token.lexema, token.tipo);
            printf("%d# %s | %d\n", token.linha, atomoParaString(token.tipo), indice);
        } else if (token.tipo == OP_RELACIONAL || token.tipo == OP_ARITMETICO) {
            char *nomeOp = obterNomeOperador(token.lexema);
            adicionarSimbolo(token.lexema, token.tipo);
            printf("%d# %s | %s\n", token.linha, atomoParaString(token.tipo), nomeOp);
        } else {
            printf("%d# %s\n", token.linha, atomoParaString(token.tipo));
        }
    }

    fecharAnalisador();

    printf("\n===== ANALISE SINTATICA =====\n");

    iniciarAnalisadorSintaticoComTokens();
    START();

    printf("\nANALISE SINTATICA CONCLUIDA COM SUCESSO!\n");

    printf("\n===== TABELA DE SIMBOLOS =====\n");
    printf("%-5s | %-20s | %-20s\n", "ID", "VALOR", "TIPO");
    printf("%-5s-+-%-20s-+-%-20s\n", "-----", "--------------------", "--------------------");
    for (int i = 0; i < totalSimbolos; i++) {
        printf("%-5d | %-20s | %-20s\n", i, tabelaSimbolos[i].valor, atomoParaString(tabelaSimbolos[i].tipo));
    }

    return 0;
}