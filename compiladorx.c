/*
COMPILADORES

Usando o padrão POSIX para reconhecimento de REGEX

Expressões Regulares:
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*
LITERAL: "[^"]*" 
KEYWORD: return|from|while|as|elif|with|else|if|break|len|input|print|exec|raise|continue|range|def|for|True|False
OPERATOR: \*\*|!=|<>|==|<=|>=|\+|-|~|\*|\/|%|and|or|not|in|is|<|>
DELIMITER: \(|\)|\[|\]|\{|\}|,|:|\.|=|; 
NUMBER -> [0-9]+
COMMENT -> #.*

**O automato finito responsavel por ler essa linguagem está incluso no arquivo 
FinalSimplificado.jff -> Execute pelo JFlap**

GRAMÁTICA LIVRE DE CONTEXTO
G = (N,T,R,START)

tal que:

N = {START,STATEMENTS,STATEMENTS_PRIME,STATEMENT,ASSIGN_OR_EXPR,ASSIGN_OR_EXPR_TAIL,IF_STATEMENT,IF_START,IF_TAIL,ELIF_STATEMENTS,ELIF_STATEMENT,ELIF_STATEMENTS_PRIME,ELIF_TAIL,WHILE_STATEMENT,FOR_STATEMENT,EXPRESSION,EXPRESSION_PRIME,TERM}
T={IDENTIFIER,NUMBER,LITERAL,OPERATOR,if,elif,else,while,for,in,range,=,:,(,)}

R = {

START -> STATEMENTS

STATEMENTS -> STATEMENT | STATEMENT STATEMENTS

STATEMENT -> IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | ASSIGNMENT | EXPRESSION

START -> STATEMENTS


STATEMENTS -> STATEMENT STATEMENTS_PRIME
STATEMENTS_PRIME -> STATEMENTS | ε


STATEMENT -> IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | ASSIGN_OR_EXPR | COMMAND_STATEMENT


ASSIGN_OR_EXPR -> IDENTIFIER ASSIGN_OR_EXPR_TAIL | NUMBER EXPRESSION_PRIME | LITERAL EXPRESSION_PRIME | ( EXPRESSION ) EXPRESSION_PRIME
ASSIGN_OR_EXPR_TAIL -> = EXPRESSION | EXPRESSION_PRIME

IF_STATEMENT -> IF_START IF_TAIL
IF_START -> if EXPRESSION : STATEMENT

IF_TAIL -> else STATEMENT | ELIF_STATEMENTS STATEMENT ELIF_TAIL | ε
ELIF_TAIL -> else STATEMENT | ε

ELIF_STATEMENTS -> ELIF_STATEMENT ELIF_STATEMENTS_PRIME
ELIF_STATEMENT -> elif EXPRESSION : STATEMENT
ELIF_STATEMENTS_PRIME -> ELIF_STATEMENTS | ε

WHILE_STATEMENT -> while EXPRESSION : STATEMENT 

FOR_STATEMENT -> for IDENTIFIER in range ( EXPRESSION ) : STATEMENT

COMMAND_STATEMENT -> print ( EXPRESSION ) | break | continue | return EXPRESSION | exec EXPRESSION | raise EXPRESSION

EXPRESSION -> TERM EXPRESSION_PRIME
EXPRESSION_PRIME -> OPERATOR TERM  EXPRESSION_PRIME | ε


TERM -> IDENTIFICADOR | NUMERO | LITERAL | TRUE | FALSE | LIST | TUPLE_OR_GROUP | len ( EXPRESSION ) | input ( LITERAL )

LIST -> [ ELEMENTS_OPT ]
TUPLE_OR_GROUP -> ( ELEMENTS_OPT )

ELEMENTS_OPT -> EXPRESSION ELEMENTS_TAIL | ε
ELEMENTS_TAIL -> , EXPRESSION ELEMENTS_TAIL | ε

}
*/

/* ==============================================================================
 * TABELA DO ANALISADOR SINTATICO PREDITIVO LL(1)
 * ==============================================================================
 * Legenda de Simbolos Especiais:
 * [EPSILON] = String Vazia (epsilon)
 * [$]       = Fim de Arquivo / Fim da Entrada (EOF)
 * * Para facilitar a leitura, definimos dois conjuntos base que se repetem:
 * * F_TERM (First de Termos):
 * { IDENTIFIER, NUMBER, LITERAL, TRUE, FALSE, '[', '(', len, input }
 * * F_STMT (First de Statements - Inicio de Comandos):
 * { if, while, for, print, break, continue, return, exec, raise, 
 * IDENTIFIER, NUMBER, LITERAL, '(' }
 * * FL_STMT (Follow de Statements - Fim de Comandos / Blocos):
 * F_STMT U { $, else, elif }
 * ==============================================================================
 *
 * NAO-TERMINAL            | NULO? | CONJUNTO FIRST                | CONJUNTO FOLLOW
 * ------------------------------------------------------------------------------
 * START                   | NAO   | F_STMT                        | { $ }
 * ------------------------------------------------------------------------------
 * STATEMENTS              | NAO   | F_STMT                        | { $ }
 * ------------------------------------------------------------------------------
 * STATEMENTS_PRIME        | SIM   | F_STMT U { [EPSILON] }        | { $ }
 * ------------------------------------------------------------------------------
 * STATEMENT               | NAO   | F_STMT                        | FL_STMT
 * ------------------------------------------------------------------------------
 * ASSIGN_OR_EXPR          | NAO   | { IDENTIFIER, NUMBER,         | FL_STMT
 * |       |   LITERAL, '(' }              | 
 * ------------------------------------------------------------------------------
 * ASSIGN_OR_EXPR_TAIL     | SIM   | { '=', OPERATOR, [EPSILON] }  | FL_STMT
 * ------------------------------------------------------------------------------
 * IF_STATEMENT            | NAO   | { if }                        | FL_STMT
 * ------------------------------------------------------------------------------
 * IF_START                | NAO   | { if }                        | FL_STMT U { else, elif }
 * ------------------------------------------------------------------------------
 * IF_TAIL                 | SIM   | { else, elif, [EPSILON] }     | FL_STMT
 * ------------------------------------------------------------------------------
 * ELIF_TAIL               | SIM   | { else, [EPSILON] }           | FL_STMT
 * ------------------------------------------------------------------------------
 * ELIF_STATEMENTS         | NAO   | { elif }                      | F_STMT
 * ------------------------------------------------------------------------------
 * ELIF_STATEMENT          | NAO   | { elif }                      | F_STMT U { elif }
 * ------------------------------------------------------------------------------
 * ELIF_STATEMENTS_PRIME   | SIM   | { elif, [EPSILON] }           | F_STMT
 * ------------------------------------------------------------------------------
 * WHILE_STATEMENT         | NAO   | { while }                     | FL_STMT
 * ------------------------------------------------------------------------------
 * FOR_STATEMENT           | NAO   | { for }                       | FL_STMT
 * ------------------------------------------------------------------------------
 * COMMAND_STATEMENT       | NAO   | { print, break, continue,     | FL_STMT
 * |       |   return, exec, raise }       | 
 * ------------------------------------------------------------------------------
 * EXPRESSION              | NAO   | F_TERM                        | FL_STMT U { ':', ')', ',' }
 * ------------------------------------------------------------------------------
 * EXPRESSION_PRIME        | SIM   | { OPERATOR, [EPSILON] }       | FL_STMT U { ':', ')', ',' }
 * ------------------------------------------------------------------------------
 * TERM                    | NAO   | F_TERM                        | FL_STMT U { ':', ')', ',', OPERATOR }
 * ------------------------------------------------------------------------------
 * LIST                    | NAO   | { '[' }                       | FL_STMT U { ':', ')', ',', OPERATOR }
 * ------------------------------------------------------------------------------
 * TUPLE_OR_GROUP          | NAO   | { '(' }                       | FL_STMT U { ':', ')', ',', OPERATOR }
 * ------------------------------------------------------------------------------
 * ELEMENTS_OPT            | SIM   | F_TERM U { [EPSILON] }        | { ']', ')' }
 * ------------------------------------------------------------------------------
 * ELEMENTS_TAIL           | SIM   | { ',', [EPSILON] }            | { ']', ')' }
 * ==============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Tipo atomo
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    LITERAL,
    EOS, // Equivalente a FIM_ARQUIVO
    COMMENT,
    OP_RELACIONAL,
    OP_ARITMETICO,
    ATRIBUICAO,
    DELIMITER,
    T_IF, T_ELIF, T_ELSE, T_WHILE, T_FOR, T_DEF,
    T_RETURN, T_BREAK, T_CONTINUE, T_PRINT, T_INPUT,
    T_LEN, T_RANGE, T_TRUE, T_FALSE, T_WITH, T_AS,
    T_FROM, T_EXEC, T_RAISE, T_AND, T_OR, T_NOT, T_IN, T_IS,
    DESCONHECIDO
} TAtomo;

// Token
typedef struct {
    char lexema[100];
    TAtomo tipo;
    int linha;
} Token;

// Tabela de símbolos
typedef struct {
    char valor[100];
    TAtomo tipo;
} SimboloTabela;

// VARS GLOBAIS
FILE *fonte;
int posicaoAtual = 0;
char lexemasArray[512][512];
int linhaAtual = 1;
Token lookahead;
SimboloTabela tabelaSimbolos[1024];
int totalSimbolos = 0;

// FUNCOES AUXILIARES
int isLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int isDigito(char c) {
    return c >= '0' && c <= '9';
}

int isSeparadorValido(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == EOF;
}

// funcao que pega o arquivo formatado e retorna uma array de lexemas
// todos os lexemas obrigatoriamente sao separados por espaco, o leitor
// apenas le os caracteres ate encontrar um espaco, e entao armazena o 
// lexema em um array de strings
int lexemas(const char* filename, char lexemas[512][512]) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return -1;
    }
    
    char buffer[100];
    int count = 0;
    int in_string = 0;
    int in_comment = 0;
    char c;
    int buf_idx = 0;
    
    while ((c = fgetc(file)) != EOF) {
        // Handle comments
        if (!in_string && c == '#') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                strcpy(lexemas[count++], buffer);
                buf_idx = 0;
            }
            // Skip until end of line
            while ((c = fgetc(file)) != EOF && c != '\n');
            continue;
        }
        
        // Handle strings
        if (c == '"') {
            in_string = !in_string;
            buffer[buf_idx++] = c;
            continue;
        }
        
        // Handle whitespace outside strings
        if (!in_string && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                if (count < 512) {
                    strcpy(lexemas[count++], buffer);
                }
                buf_idx = 0;
            }
            continue;
        }
        
        buffer[buf_idx++] = c;
    }
    
    if (buf_idx > 0 && count < 512) {
        buffer[buf_idx] = '\0';
        strcpy(lexemas[count++], buffer);
    }
    
    fclose(file);
    return count;
}

// Função para adicionar à tabela de símbolos (evita duplicatas)
void adicionarSimbolo(char *lexema, TAtomo tipo) {
    for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].valor, lexema) == 0) return;
    }
    if (totalSimbolos < 1024) {
        strncpy(tabelaSimbolos[totalSimbolos].valor, lexema, 99);
        tabelaSimbolos[totalSimbolos].tipo = tipo;
        totalSimbolos++;
    }
}

int encontrarSimbolo(char *lexema) {
    for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].valor, lexema) == 0) return i;
    }
    return -1;
}

Token* obter_atomo(char *lexema) {
    static Token token;
    token.tipo = DESCONHECIDO;
    strncpy(token.lexema, lexema, sizeof(token.lexema) - 1);
    token.linha = linhaAtual;

    // 1. LITERAL (Strings entre aspas)
    if (lexema[0] == '"') {
        token.tipo = LITERAL;
        adicionarSimbolo(lexema, LITERAL);
        return &token;
    }

    // 2. NUMBER (Apenas dígitos)
    if (isDigito(lexema[0])) {
        token.tipo = NUMERO;
        adicionarSimbolo(lexema, NUMERO);
        return &token;
    }

    // 3. KEYWORDS
    if (strcmp(lexema, "return") == 0) token.tipo = T_RETURN;
    else if (strcmp(lexema, "from") == 0) token.tipo = T_FROM;
    else if (strcmp(lexema, "while") == 0) token.tipo = T_WHILE;
    else if (strcmp(lexema, "as") == 0) token.tipo = T_AS;
    else if (strcmp(lexema, "elif") == 0) token.tipo = T_ELIF;
    else if (strcmp(lexema, "with") == 0) token.tipo = T_WITH;
    else if (strcmp(lexema, "else") == 0) token.tipo = T_ELSE;
    else if (strcmp(lexema, "if") == 0) token.tipo = T_IF;
    else if (strcmp(lexema, "break") == 0) token.tipo = T_BREAK;
    else if (strcmp(lexema, "len") == 0) token.tipo = T_LEN;
    else if (strcmp(lexema, "input") == 0) token.tipo = T_INPUT;
    else if (strcmp(lexema, "print") == 0) token.tipo = T_PRINT;
    else if (strcmp(lexema, "exec") == 0) token.tipo = T_EXEC;
    else if (strcmp(lexema, "raise") == 0) token.tipo = T_RAISE;
    else if (strcmp(lexema, "continue") == 0) token.tipo = T_CONTINUE;
    else if (strcmp(lexema, "range") == 0) token.tipo = T_RANGE;
    else if (strcmp(lexema, "def") == 0) token.tipo = T_DEF;
    else if (strcmp(lexema, "for") == 0) token.tipo = T_FOR;
    else if (strcmp(lexema, "True") == 0) token.tipo = T_TRUE;
    else if (strcmp(lexema, "False") == 0) token.tipo = T_FALSE;

    // 4. OPERATORS (Aritméticos e Lógicos conforme o Regex)
    else if (strcmp(lexema, "**") == 0 || strcmp(lexema, "+") == 0 || 
             strcmp(lexema, "-") == 0 || strcmp(lexema, "*") == 0 || 
             strcmp(lexema, "/") == 0 || strcmp(lexema, "%") == 0 || 
             strcmp(lexema, "~") == 0) {
        token.tipo = OP_ARITMETICO;
    }
    else if (strcmp(lexema, "==") == 0 || strcmp(lexema, "!=") == 0 || 
             strcmp(lexema, "<>") == 0 || strcmp(lexema, "<=") == 0 || 
             strcmp(lexema, ">=") == 0 || strcmp(lexema, "<") == 0 || 
             strcmp(lexema, ">") == 0 || strcmp(lexema, "and") == 0 || 
             strcmp(lexema, "or") == 0 || strcmp(lexema, "not") == 0 || 
             strcmp(lexema, "in") == 0 || strcmp(lexema, "is") == 0) {
        token.tipo = OP_RELACIONAL;
    }

    // 5. DELIMITERS
    else if (strcmp(lexema, "=") == 0) token.tipo = ATRIBUICAO;
    else if (strchr("()[]{,}:.;", lexema[0]) && strlen(lexema) == 1) {
        token.tipo = DELIMITER;
    }

    // 6. IDENTIFIER (Se começar com letra/_ e não for Keyword)
    else if (isLetra(lexema[0])) {
        token.tipo = IDENTIFICADOR;
        adicionarSimbolo(lexema, IDENTIFICADOR);
    }

    return &token;
}

char* atomoParaString(TAtomo tipo) {
    switch (tipo) {
        case ERRO: return "ERRO";
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case LITERAL: return "LITERAL";
        case EOS: return "EOS";
        case COMMENT: return "COMMENT";
        case OP_RELACIONAL: return "OP_RELACIONAL";
        case OP_ARITMETICO: return "OP_ARITMETICO";
        case ATRIBUICAO: return "ATRIBUICAO";
        case DELIMITER: return "DELIMITER";
        case T_IF: return "T_IF";
        case T_ELIF: return "T_ELIF";
        case T_ELSE: return "T_ELSE";
        case T_WHILE: return "T_WHILE";
        case T_FOR: return "T_FOR";
        case T_DEF: return "T_DEF";
        case T_RETURN: return "T_RETURN";
        case T_BREAK: return "T_BREAK";
        case T_CONTINUE: return "T_CONTINUE";
        case T_PRINT: return "T_PRINT";
        case T_INPUT: return "T_INPUT";
        case T_LEN: return "T_LEN";
        case T_RANGE: return "T_RANGE";
        case T_TRUE: return "T_TRUE";
        case T_FALSE: return "T_FALSE";
        case T_WITH: return "T_WITH";
        case T_AS: return "T_AS";
        case T_FROM: return "T_FROM";
        case T_EXEC: return "T_EXEC";
        case T_RAISE: return "T_RAISE";
        case T_AND: return "T_AND";
        case T_OR: return "T_OR";
        case T_NOT: return "T_NOT";
        case T_IN: return "T_IN";
        case T_IS: return "T_IS";
        default: return "DESCONHECIDO";
    }
}

char* obterNomeOperadorDelimitador(char *op) {
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
    else if (strcmp(op, "(") == 0) strcpy(nome, "LPAREN");
    else if (strcmp(op, ")") == 0) strcpy(nome, "RPAREN");
    else if (strcmp(op, "[") == 0) strcpy(nome, "LBRACKET");
    else if (strcmp(op, "]") == 0) strcpy(nome, "RBRACKET");
    else if (strcmp(op, "{") == 0) strcpy(nome, "LBRACE");
    else if (strcmp(op, "}") == 0) strcpy(nome, "RBRACE");
    else if (strcmp(op, ",") == 0) strcpy(nome, "COMMA");
    else if (strcmp(op, ":") == 0) strcpy(nome, "COLON");
    else if (strcmp(op, ".") == 0) strcpy(nome, "DOT");
    else if (strcmp(op, "=") == 0) strcpy(nome, "EQUALS");
    else if (strcmp(op, ";") == 0) strcpy(nome, "SEMICOLON");
    else strcpy(nome, "DESCONHECIDO");
    return nome;
}

void printToken(Token *token) {
    if (token->tipo == DESCONHECIDO) {
        printf("%d# ERRO: Lexema '%s' é desconhecido.\n", token->linha, token->lexema);
    }
    else if (token->tipo == LITERAL || token->tipo == IDENTIFICADOR ||
             token->tipo == NUMERO) {
        printf("#%d %s | %d)\n", token->linha, atomoParaString(token->tipo), encontrarSimbolo(token->lexema));
    }
    else if (token->tipo == OP_ARITMETICO || token->tipo == OP_RELACIONAL || token->tipo == DELIMITER) {
        printf("%d# %s | '%s'\n", token->linha, atomoParaString(token->tipo), obterNomeOperadorDelimitador(token->lexema));
    }
    else {
        printf("%d# %s\n", token->linha, atomoParaString(token->tipo));
    }
}

void printTabelaSimbolos() {
    printf("\n===== TABELA DE SIMBOLOS =====\n");
    for (int i = 0; i < totalSimbolos; i++) {
        printf("%d: %s | %s\n", i, tabelaSimbolos[i].valor, atomoParaString(tabelaSimbolos[i].tipo));
    }
}

void arrayPrinter(char lexemas[512][512], int count) {
    Token *token = obter_atomo(lexemas[posicaoAtual++]);
    while (posicaoAtual <= count) {
        printToken(token);
        token = obter_atomo(lexemas[posicaoAtual++]);
    }
}

void STATEMENTS() {
   ;
}

void START(){
    STATEMENTS();
}

void analisadorSintatico() {
    Token* token;
    if(token->tipo == DESCONHECIDO){
        printf("ERRO LEXICO ````");
        exit(0);
    }
    
    // funcao para analisar a sintaxe dos lexemas
    // esta funcao ainda nao foi implementada, mas sera responsavel por verificar
    // se os lexemas formam uma estrutura sintatica valida de acordo com as regras
    // da linguagem de programacao que estamos compilando
}





Token analisadorLexico(){
    Token *token  =  obter_atomo(lexemasArray[posicaoAtual]);
    return *token;

}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    } else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        printf("Analisa lexica e sintaticamente um arquivo fonte em Mini-Python.\n");
        return 0;
    } else if (argc > 2) {
        printf("Erro: Muitos argumentos fornecidos.\n");
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1]; // nome do arquivo de entrada
    
    int totalLexemas = lexemas(filename, lexemasArray);
    arrayPrinter(lexemasArray, totalLexemas); // Descomente para debug do léxico
    printTabelaSimbolos(); // Descomente para ver a tabela de símbolos após o léxico
    /*
    // Processamento Léxico
    int totalLexemas = lexemas(filename, lexemasArray);
    if (totalLexemas >= 0) {
        // arrayPrinter(lexemasArray, totalLexemas); // Descomente para debug do léxico
        
        // Inicializa o Parser
        posicaoAtual = 0;
        lookahead = *obter_atomo(lexemasArray[posicaoAtual++]);
        
        // Dispara a arvore de derivação
        START();
        
        // Verifica se alcançou o fim do arquivo sem lixo
        if (posicaoAtual >= totalLexemas) {
            printf("\n> SUCESSO: Analise Sintatica concluida! O arquivo e valido.\n");
        } else {
            printf("\n> AVISO: Analise concluida, porem existem lexemas soltos no final do arquivo ('%s').\n", lookahead.lexema);
        }
    } else {
        printf("Falha na etapa de analise lexica.\n");
    }
    */
    return 0;
}