/*
DESENVOLVIDO POR:
- Henrique Brainer Costa 10420717
- João Pedro Queiroz de Andrade 10425822
- João Victor Vidal Barbosa 10410165

COMPILADORES

Usando o padrão POSIX para reconhecimento de REGEX

Expressões Regulares:
IDENTIFIER: [a-zA-Z_][a-zA-Z0-9_]*
LITERAL: "[^"]*" 
KEYWORD: return|from|while|as|elif|with|else|if|break|len|input|print|exec|raise|continue|range|def|for|True|False
OPERATOR: \*\*|!=|<>|==|<=|>=|\+|-|~|\*|\/|%|and|or|not|in|is|<|>
DELIMITER: \(|\)|\[|\]|\{|\}|,|:|\.|=|; 
NUMBER -> [0-9]+

**O automato finito responsavel por ler essa linguagem está incluso no arquivo 
FinalSimplificado.jff -> Execute pelo JFlap**

GRAMÁTICA LIVRE DE CONTEXTO
G = (N,T,R,START)

tal que:

N = {START,STATEMENTS,STATEMENTS_PRIME,STATEMENT,ASSIGN_OR_EXPR,ASSIGN_OR_EXPR_TAIL,IF_STATEMENT,IF_START,IF_TAIL,ELIF_STATEMENTS,ELIF_STATEMENT,ELIF_STATEMENTS_PRIME,ELIF_TAIL,WHILE_STATEMENT,FOR_STATEMENT,EXPRESSION,EXPRESSION_PRIME,TERM,LIST,TUPLE_OR_GROUP,ELEMENTS_OPT,ELEMENTS_TAIL,INDEX_OPT,COMMAND_STATEMENT}
T={IDENTIFIER,NUMBER,LITERAL,OPERATOR,if,elif,else,while,for,in,range,=,:,(,)}

R = {

START -> STATEMENTS


STATEMENTS -> STATEMENT STATEMENTS_PRIME
STATEMENTS_PRIME -> STATEMENTS | ε


STATEMENT -> IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | ASSIGN_OR_EXPR | COMMAND_STATEMENT


ASSIGN_OR_EXPR -> IDENTIFICADOR INDEX_OPT ASSIGN_OR_EXPR_TAIL | NUMERO EXPRESSION_PRIME | LITERAL EXPRESSION_PRIME | ( EXPRESSION ) EXPRESSION_PRIME
ASSIGN_OR_EXPR_TAIL -> = EXPRESSION | EXPRESSION_PRIME

IF_STATEMENT -> IF_START IF_TAIL
IF_START -> if EXPRESSION : STATEMENT

IF_TAIL -> else : STATEMENT | ELIF_STATEMENTS STATEMENT ELIF_TAIL | ε
ELIF_TAIL -> else : STATEMENT | ε

ELIF_STATEMENTS -> ELIF_STATEMENT ELIF_STATEMENTS_PRIME
ELIF_STATEMENT -> elif EXPRESSION : STATEMENT
ELIF_STATEMENTS_PRIME -> ELIF_STATEMENTS | ε

WHILE_STATEMENT -> while EXPRESSION : STATEMENT 

FOR_STATEMENT -> for IDENTIFIER in range ( EXPRESSION ) : STATEMENT

COMMAND_STATEMENT -> print ( ELEMENTS_OPT ) | break | continue | return EXPRESSION | exec EXPRESSION | raise EXPRESSION

EXPRESSION -> TERM EXPRESSION_PRIME
EXPRESSION_PRIME -> OPERATOR TERM  EXPRESSION_PRIME | ε

INDEX_OPT -> [ EXPRESSION ] INDEX_OPT | ε

TERM -> IDENTIFICADOR INDEX_OPT | NUMERO | LITERAL | TRUE | FALSE | LIST | TUPLE_OR_GROUP | len ( EXPRESSION ) | input ( LITERAL )

LIST -> [ ELEMENTS_OPT ]
TUPLE_OR_GROUP -> ( ELEMENTS_OPT )

ELEMENTS_OPT -> EXPRESSION ELEMENTS_TAIL | ε
ELEMENTS_TAIL -> , EXPRESSION ELEMENTS_TAIL | ε

}
*/

/* ==============================================================================
 * TABELA DO ANALISADOR SINTATICO PREDITIVO
 * ==============================================================================
 * Legenda de Simbolos Especiais:
 * [EPSILON] = String Vazia (epsilon)
 * [$]       = Fim de Arquivo / Fim da Entrada (EOF)
 * * * F_TERM (First de Termos):
 * { IDENTIFIER, NUMBER, LITERAL, TRUE, FALSE, '[', '(', len, input }
 * * * F_STMT (First de Statements - Inicio de Comandos):
 * { if, while, for, print, break, continue, return, exec, raise, input, 
 * IDENTIFIER, NUMBER, LITERAL, '(' }
 * * * FL_STMT (Follow de Statements - Fim de Comandos / Blocos):
 * F_STMT U { $, else, elif }
 * ==============================================================================
 *
 * NAO-TERMINAL            | NULO? | CONJUNTO FIRST                | CONJUNTO FOLLOW
 * ------------------------------------------------------------------------------
 * START                   | NAO   | F_STMT                        | { $ }
 * STATEMENTS              | NAO   | F_STMT                        | { $ }
 * STATEMENTS_PRIME        | SIM   | F_STMT U { [EPSILON] }        | { $ }
 * STATEMENT               | NAO   | F_STMT                        | FL_STMT
 * ASSIGN_OR_EXPR          | NAO   | { IDENTIFIER, NUMBER,         | FL_STMT
 * |       |   LITERAL, '(' }              | 
 * ASSIGN_OR_EXPR_TAIL     | SIM   | { '=', OPERATOR, [EPSILON] }  | FL_STMT
 * IF_STATEMENT            | NAO   | { if }                        | FL_STMT
 * IF_START                | NAO   | { if }                        | FL_STMT
 * IF_TAIL                 | SIM   | { else, elif, [EPSILON] }     | FL_STMT
 * ELIF_TAIL               | SIM   | { else, [EPSILON] }           | FL_STMT
 * ELIF_STATEMENTS         | NAO   | { elif }                      | F_STMT
 * ELIF_STATEMENT          | NAO   | { elif }                      | F_STMT U { elif }
 * ELIF_STATEMENTS_PRIME   | SIM   | { elif, [EPSILON] }           | F_STMT
 * WHILE_STATEMENT         | NAO   | { while }                     | FL_STMT
 * FOR_STATEMENT           | NAO   | { for }                       | FL_STMT
 * COMMAND_STATEMENT       | NAO   | { print, break, continue,     | FL_STMT
 * |       |   return, exec, raise, input} | 
 * EXPRESSION              | NAO   | F_TERM                        | FL_STMT U { ':', ')', ']', ',' }
 * EXPRESSION_PRIME        | SIM   | { OPERATOR, [EPSILON] }       | FL_STMT U { ':', ')', ']', ',' }
 * INDEX_OPT               | SIM   | { '[', [EPSILON] }            | FL_STMT U { '=', ':', ')', ']', ',', OPERATOR }
 * TERM                    | NAO   | F_TERM                        | FL_STMT U { ':', ')', ']', ',', OPERATOR }
 * LIST                    | NAO   | { '[' }                       | FL_STMT U { ':', ')', ']', ',', OPERATOR }
 * TUPLE_OR_GROUP          | NAO   | { '(' }                       | FL_STMT U { ':', ')', ']', ',', OPERATOR }
 * ELEMENTS_OPT            | SIM   | F_TERM U { [EPSILON] }        | { ']', ')' }
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
    EOS, // Fim do arquivo
    COMMENT,
    OP_RELACIONAL,
    OP_ARITMETICO,
    ATRIBUICAO,
    DELIMITER,
    T_IF, T_ELIF, T_ELSE, T_WHILE, T_FOR, T_DEF,
    T_RETURN, T_BREAK, T_CONTINUE, T_PRINT, T_INPUT,
    T_LEN, T_RANGE, T_TRUE, T_FALSE, T_WITH, T_AS,
    T_FROM, T_EXEC, T_RAISE,
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
int totalLexemasGlob = 0;
char lexemasArray[512][512];
int mapaLinhasGlobais[512]; // Adicione esta linha

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
int lexemas(const char* filename, char lexemas[512][512], int mapaLinhas[512]) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return -1;
    }
    
    char buffer[100];
    int count = 0;
    int in_string = 0;
    int c;
    int buf_idx = 0;
    
    // Rastreadores de linha
    int linha_atual = 1;
    int linha_inicio_lexema = 1;
    
    while ((c = fgetc(file)) != EOF) {
        // Atualiza a contagem global de linhas
        if (c == '\n') {
            linha_atual++;
        }

        // Ignora comentários e avança até o fim da linha
        if (!in_string && c == '#') {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                strcpy(lexemas[count], buffer);
                mapaLinhas[count] = linha_inicio_lexema; // Salva a linha do lexema
                count++;
                buf_idx = 0;
            }
            // Pula os caracteres até achar a quebra de linha ou o fim do arquivo
            while ((c = fgetc(file)) != EOF) {
                if (c == '\n') {
                    linha_atual++;
                    break;
                }
            }
            continue;
        }
        
        // Se estamos começando a ler um novo lexema, registramos em qual linha ele começou
        if (buf_idx == 0 && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            linha_inicio_lexema = linha_atual;
        }

        // Trata strings literais
        if (c == '"') {
            in_string = !in_string;
            buffer[buf_idx++] = (char)c;
            continue;
        }
        
        // Trata espaços em branco fora de strings (separadores)
        if (!in_string && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                if (count < 512) {
                    strcpy(lexemas[count], buffer);
                    mapaLinhas[count] = linha_inicio_lexema; // Salva a linha
                    count++;
                }
                buf_idx = 0;
            }
            continue;
        }
        
        // Acumula o caractere no buffer do lexema atual
        buffer[buf_idx++] = (char)c;
    }
    
    // Captura o último lexema caso o arquivo termine sem uma quebra de linha no final
    if (buf_idx > 0 && count < 512) {
        buffer[buf_idx] = '\0';
        strcpy(lexemas[count], buffer);
        mapaLinhas[count] = linha_inicio_lexema;
        count++;
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
        int eh_numero_valido = 1;
        
        // Verifica do segundo caractere em diante
        for (int i = 1; lexema[i] != '\0'; i++) {
            if (!isDigito(lexema[i])) {
                eh_numero_valido = 0;
                break; 
            }
        }
        
        if (eh_numero_valido) {
            token.tipo = NUMERO;
            adicionarSimbolo(lexema, NUMERO);
        } else {
            token.tipo = DESCONHECIDO; 
        }
        
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
        int eh_identificador_valido = 1;
        
        // Verifica do segundo caractere em diante
        for (int i = 1; lexema[i] != '\0'; i++) {
            // Um identificador válido só pode conter letras, '_' (já coberto pelo isLetra) ou dígitos
            if (!isLetra(lexema[i]) && !isDigito(lexema[i])) {
                eh_identificador_valido = 0;
                break; // Encontrou um intruso (ex: '$', '-'), para a verificação
            }
        }
        
        if (eh_identificador_valido) {
            token.tipo = IDENTIFICADOR;
            adicionarSimbolo(lexema, IDENTIFICADOR);
        } else {
            // Rejeita tokens malformados como "val$" ou "company-name"
            token.tipo = DESCONHECIDO; 
            
        }
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
    else if (strcmp(op, "+") == 0) strcpy(nome, "SUM");
    else if (strcmp(op, "-") == 0) strcpy(nome, "SUBTRACTION");
    else if (strcmp(op, "*") == 0) strcpy(nome, "MULTIPLICATION");
    else if (strcmp(op, "**") == 0) strcpy(nome, "POWER");
    else if (strcmp(op, "/") == 0) strcpy(nome, "DIVISION");
    else if (strcmp(op, "%") == 0) strcpy(nome, "MODULUS");
    else if (strcmp(op, "~") == 0) strcpy(nome, "NOT");
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
    else if (strcmp(op, "in") == 0) strcpy(nome, "IN");
    else if (strcmp(op, "is") == 0) strcpy(nome, "IS");
    else if (strcmp(op, "and") == 0) strcpy(nome, "AND");
    else if (strcmp(op, "or") == 0) strcpy(nome, "OR");
    else if (strcmp(op, "not") == 0) strcpy(nome, "NOT");
    else strcpy(nome, "DESCONHECIDO");
    return nome;
}

void printToken(Token *token) {
    if (token->tipo == DESCONHECIDO) {
        printf("%d# ERRO: Lexema '%s' desconhecido.\n", token->linha, token->lexema);
        exit(1);
    }
    else if (token->tipo == LITERAL || token->tipo == IDENTIFICADOR ||
             token->tipo == NUMERO) {
        printf("%d# %s | %d\n", token->linha, atomoParaString(token->tipo), encontrarSimbolo(token->lexema));
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

void erroSintatico(const char *mensagem) {
    printf("\n[ERRO SINTATICO - LINHA %d] %s. (Lookahead atual: '%s' | Tipo: %s)\n", 
           lookahead.linha, mensagem, lookahead.lexema, atomoParaString(lookahead.tipo));
    exit(1);
}

Token analisadorLexico() {
    if (posicaoAtual < totalLexemasGlob) {
        Token *t = obter_atomo(lexemasArray[posicaoAtual]);
        t->linha = mapaLinhasGlobais[posicaoAtual]; // linha salva pelo mapa

        if (t->tipo == DESCONHECIDO) {
            printf("%d# ERRO: Lexema '%s' desconhecido.\n", t->linha, t->lexema);
            exit(1);
        }
        return *t;
    }
    
    // Tratamento para EOF
    Token fim;
    fim.tipo = EOS;
    strcpy(fim.lexema, "EOF");
    // Se o arquivo acabou, a linha do EOF é a última linha lida
    fim.linha = totalLexemasGlob > 0 ? mapaLinhasGlobais[totalLexemasGlob - 1] : 1; 
    return fim;
}

// Verifica e consome um tipo específico de átomo
// Função única para consumir tokens: verifica o tipo e, opcionalmente, o lexema exato.
void consome(TAtomo tipo_esperado, const char *lexema_esperado) {
    printToken(&lookahead); // Imprime o token atual antes de consumir
    if (lookahead.tipo == tipo_esperado) {
        // Se lexema_esperado não for NULL, precisamos garantir que a string exata bate
        if (lexema_esperado != NULL) {
            if (strcmp(lookahead.lexema, lexema_esperado) == 0) {
                posicaoAtual++;
                lookahead = analisadorLexico();
            } else {
                char msg[256];
                sprintf(msg, "Esperava o simbolo/atributo '%s' (%s), mas encontrou '%s'", 
                        lexema_esperado, atomoParaString(tipo_esperado), lookahead.lexema);
                erroSintatico(msg);
            }
        } else {
            // Se for NULL, apenas a classe do token (o tipo) importa
            posicaoAtual++;
            lookahead = analisadorLexico();
        }
    } else {
        // Erro de tipo
        char msg[256];
        if (lexema_esperado != NULL) {
            sprintf(msg, "Esperava %s ('%s')", atomoParaString(tipo_esperado), lexema_esperado);
        } else {
            sprintf(msg, "Esperava token do tipo %s", atomoParaString(tipo_esperado));
        }
        erroSintatico(msg);
    }
}



// FIRST(STATEMENT)
int in_F_STMT() {
    return (lookahead.tipo == T_IF || lookahead.tipo == T_WHILE || lookahead.tipo == T_FOR || 
            lookahead.tipo == T_PRINT || lookahead.tipo == T_BREAK || lookahead.tipo == T_CONTINUE || 
            lookahead.tipo == T_RETURN || lookahead.tipo == T_EXEC || lookahead.tipo == T_RAISE || 
            lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || lookahead.tipo == LITERAL ||
            (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0));
}

// FIRST(TERM)
int in_F_TERM() {
    return (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || lookahead.tipo == LITERAL ||
            lookahead.tipo == T_TRUE || lookahead.tipo == T_FALSE || 
            (lookahead.tipo == DELIMITER && (strcmp(lookahead.lexema, "[") == 0 || strcmp(lookahead.lexema, "(") == 0)) ||
            lookahead.tipo == T_LEN || lookahead.tipo == T_INPUT);
}

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
void COMMAND_STATEMENT();
void INDEX_OPT();
void TERM();
void LIST();
void TUPLE_OR_GROUP();
void ELEMENTS_OPT();
void ELEMENTS_TAIL();


// ==============================================================================
// ANALISADOR SINTÁTICO RECURSIVO DESCENDENTE PREDITIVO
// ==============================================================================

void analisadorSintatico() {
    posicaoAtual = 0;
    lookahead = analisadorLexico();
    START();
}

void START() {
    // FIRST(START) = F_STMT. FOLLOW(START) = { EOS }. Deriva: STATEMENTS EOS
    STATEMENTS();
    consome(EOS, NULL);
}

void STATEMENTS() {
    // FIRST(STATEMENTS) = F_STMT. Deriva: STATEMENT STATEMENTS_PRIME
    STATEMENT();
    STATEMENTS_PRIME();
}

void STATEMENTS_PRIME() {
    // FIRST(STATEMENTS_PRIME) = F_STMT U { EPSILON }. FOLLOW(STATEMENTS_PRIME) = { EOS }
    if (in_F_STMT()) {
        STATEMENTS();
    }
    // else EPSILON
}

void STATEMENT() {
    // FIRST(STATEMENT) = F_STMT. Deriva: IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | COMMAND_STATEMENT | ASSIGN_OR_EXPR
    if (lookahead.tipo == T_IF) {
        IF_STATEMENT();
    } else if (lookahead.tipo == T_WHILE) {
        WHILE_STATEMENT();
    } else if (lookahead.tipo == T_FOR) {
        FOR_STATEMENT();
    } else if (lookahead.tipo == T_PRINT || lookahead.tipo == T_BREAK || 
               lookahead.tipo == T_CONTINUE || lookahead.tipo == T_RETURN || 
               lookahead.tipo == T_EXEC || lookahead.tipo == T_RAISE ||
               lookahead.tipo == T_INPUT) {
        COMMAND_STATEMENT();
    } else if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || 
               lookahead.tipo == LITERAL || 
               (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0)) {
        ASSIGN_OR_EXPR();
    } else {
        erroSintatico("Token inesperado no inicio de um statement");
    }
}

void ASSIGN_OR_EXPR() {
    // FIRST(ASSIGN_OR_EXPR) = { IDENTIFICADOR, NUMERO, LITERAL, '(' }
    if (lookahead.tipo == IDENTIFICADOR) {
        consome(IDENTIFICADOR, NULL);
        INDEX_OPT();
        ASSIGN_OR_EXPR_TAIL();
    } else if (lookahead.tipo == NUMERO) {
        consome(NUMERO, NULL);
        EXPRESSION_PRIME();
    } else if (lookahead.tipo == LITERAL) {
        consome(LITERAL, NULL);
        EXPRESSION_PRIME();
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0) {
        consome(DELIMITER, "(");
        EXPRESSION();
        consome(DELIMITER, ")");
        EXPRESSION_PRIME();
    }
}

void ASSIGN_OR_EXPR_TAIL() {
    // FIRST(ASSIGN_OR_EXPR_TAIL) = { '=', OPERATOR, EPSILON }
    if (lookahead.tipo == ATRIBUICAO) {
        consome(ATRIBUICAO, NULL);
        EXPRESSION();
    } else {
        EXPRESSION_PRIME(); 
    }
}

void IF_STATEMENT() {
    // FIRST(IF_STATEMENT) = { if }. Deriva: IF_START IF_TAIL
    IF_START();
    IF_TAIL();
}

void IF_START() {
    // FIRST(IF_START) = { if }. Deriva: if EXPRESSION : STATEMENT
    consome(T_IF, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void IF_TAIL() {
    // FIRST(IF_TAIL) = { else, elif, EPSILON }. FOLLOW(IF_TAIL) = FL_STMT
    if (lookahead.tipo == T_ELSE) {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        STATEMENT();
    } else if (lookahead.tipo == T_ELIF) {
        ELIF_STATEMENTS();
        STATEMENT();
        ELIF_TAIL();
    }
    // else EPSILON
}

void ELIF_TAIL() {
    // FIRST(ELIF_TAIL) = { else, EPSILON }. FOLLOW(ELIF_TAIL) = FL_STMT
    if (lookahead.tipo == T_ELSE) {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        STATEMENT();
    }
    // else EPSILON
}

void ELIF_STATEMENTS() {
    // FIRST(ELIF_STATEMENTS) = { elif }. Deriva: ELIF_STATEMENT ELIF_STATEMENTS_PRIME
    ELIF_STATEMENT();
    ELIF_STATEMENTS_PRIME();
}

void ELIF_STATEMENT() {
    // FIRST(ELIF_STATEMENT) = { elif }. Deriva: elif EXPRESSION : STATEMENT
    consome(T_ELIF, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void ELIF_STATEMENTS_PRIME() {
    // FIRST(ELIF_STATEMENTS_PRIME) = { elif, EPSILON }. FOLLOW(ELIF_STATEMENTS_PRIME) = F_STMT
    if (lookahead.tipo == T_ELIF) {
        ELIF_STATEMENTS();
    }
    // else EPSILON
}

void WHILE_STATEMENT() {
    // FIRST(WHILE_STATEMENT) = { while }. Deriva: while EXPRESSION : STATEMENT
    consome(T_WHILE, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void FOR_STATEMENT() {
    // FIRST(FOR_STATEMENT) = { for }. Deriva: for IDENTIFICADOR in range ( EXPRESSION ) : STATEMENT
    consome(T_FOR, NULL);
    consome(IDENTIFICADOR, NULL);
    consome(OP_RELACIONAL, "in");
    consome(T_RANGE, NULL);
    consome(DELIMITER, "(");
    EXPRESSION();
    consome(DELIMITER, ")");
    consome(DELIMITER, ":");
    STATEMENT();
}

void COMMAND_STATEMENT() {
    // FIRST(COMMAND_STATEMENT) = { print, break, continue, return, exec, raise, input }
    if (lookahead.tipo == T_PRINT) {
        consome(T_PRINT, NULL);
        consome(DELIMITER, "(");
        ELEMENTS_OPT();
        consome(DELIMITER, ")");
    } else if (lookahead.tipo == T_INPUT) {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
    } else if (lookahead.tipo == T_BREAK) {
        consome(T_BREAK, NULL);
    } else if (lookahead.tipo == T_CONTINUE) {
        consome(T_CONTINUE, NULL);
    } else if (lookahead.tipo == T_RETURN) {
        consome(T_RETURN, NULL);
        EXPRESSION();
    } else if (lookahead.tipo == T_EXEC) {
        consome(T_EXEC, NULL);
        EXPRESSION();
    } else if (lookahead.tipo == T_RAISE) {
        consome(T_RAISE, NULL);
        EXPRESSION();
    }
}

void INDEX_OPT() {
    // FIRST(INDEX_OPT) = { '[', EPSILON }. FOLLOW(INDEX_OPT) = FL_STMT U { '=', ':', ')', ']', ',', OPERATOR }
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0) {
        consome(DELIMITER, "[");
        EXPRESSION();
        consome(DELIMITER, "]");
        INDEX_OPT(); 
    }
    // else EPSILON 
}

void EXPRESSION() {
    // FIRST(EXPRESSION) = F_TERM. Deriva: TERM EXPRESSION_PRIME
    TERM();
    EXPRESSION_PRIME();
}

void EXPRESSION_PRIME() {
    // FIRST(EXPRESSION_PRIME) = { OPERATOR, EPSILON }. FOLLOW(EXPRESSION_PRIME) = FL_STMT U { ':', ')', ']', ',' }
    if (lookahead.tipo == OP_ARITMETICO || lookahead.tipo == OP_RELACIONAL) {
        consome(lookahead.tipo, NULL); 
        TERM();
        EXPRESSION_PRIME();
    }
    // else EPSILON
}

void TERM() {
    // FIRST(TERM) = F_TERM = { IDENTIFICADOR, NUMERO, LITERAL, TRUE, FALSE, '[', '(', len, input }
    if (lookahead.tipo == IDENTIFICADOR) {
        consome(IDENTIFICADOR, NULL);
        INDEX_OPT();
    } else if (lookahead.tipo == NUMERO) {
        consome(NUMERO, NULL);
    } else if (lookahead.tipo == LITERAL) {
        consome(LITERAL, NULL);
    } else if (lookahead.tipo == T_TRUE) {
        consome(T_TRUE, NULL);
    } else if (lookahead.tipo == T_FALSE) {
        consome(T_FALSE, NULL);
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0) {
        LIST();
    } else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0) {
        TUPLE_OR_GROUP();
    } else if (lookahead.tipo == T_LEN) {
        consome(T_LEN, NULL);
        consome(DELIMITER, "(");
        EXPRESSION();
        consome(DELIMITER, ")");
    } else if (lookahead.tipo == T_INPUT) {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
    } else {
        erroSintatico("Esperado um TERM (Identificador, Numero, String, Tupla, Lista, etc)");
    }
}

void LIST() {
    // FIRST(LIST) = { '[' }. Deriva: [ ELEMENTS_OPT ]
    consome(DELIMITER, "[");
    ELEMENTS_OPT();
    consome(DELIMITER, "]");
}

void TUPLE_OR_GROUP() {
    // FIRST(TUPLE_OR_GROUP) = { '(' }. Deriva: ( ELEMENTS_OPT )
    consome(DELIMITER, "(");
    ELEMENTS_OPT();
    consome(DELIMITER, ")");
}

void ELEMENTS_OPT() {
    // FIRST(ELEMENTS_OPT) = F_TERM U { EPSILON }. FOLLOW(ELEMENTS_OPT) = { ']', ')' }
    if (in_F_TERM()) {
        EXPRESSION();
        ELEMENTS_TAIL();
    }
    // else EPSILON
}

void ELEMENTS_TAIL() {
    // FIRST(ELEMENTS_TAIL) = { ',', EPSILON }. FOLLOW(ELEMENTS_TAIL) = { ']', ')' }
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, ",") == 0) {
        consome(DELIMITER, ",");
        EXPRESSION();
        ELEMENTS_TAIL();
    }
    // else EPSILON
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

    const char* filename = argv[1]; 
    
    // Carrega os lexemas do arquivo na array e suas linhas correspondentes no mapa
    totalLexemasGlob = lexemas(filename, lexemasArray, mapaLinhasGlobais);
    
   if (totalLexemasGlob >= 0) {
        printf("===== INICIANDO ANALISE LEXICA E SINTATICA =====\n");
        
        // Inicia a análise sintática
        analisadorSintatico();

        printf("\n> SUCESSO: Analise Sintatica concluida! A gramatica do arquivo eh valida.\n");
        printTabelaSimbolos();
        
    } else {
        printf("Falha na etapa de analise lexica.\n");
    }

    return 0;
}