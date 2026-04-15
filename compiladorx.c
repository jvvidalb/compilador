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
*/

#include <stdio.h>
#include <string.h>



// VARS GLOBAIS
FILE *fonte;
int posicaoAtual = 0;
char lexemasArray[512][512];
int linhaAtual = 1;

// Tipo atomo
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

Token* obter_atomo(){
    Token token;
    token.tipo = DESCONHECIDO;
    strcpy(token.lexema, "");
    token.linha = linhaAtual;



     // Classificar o lexema para determinar seu tipo

    return &token;
}


void arrayPrinter(char lexemas[512][512], int count) {
    printf("Lexemas encontrados:\n");
    for (int i = 0; i < count; i++) {
        printf("%s\n", lexemas[i]);
    }
}

void STATEMENTS() {
   ;
}

void START(){
    STATEMENTS();
}

void analizadorSintatico() {

    // funcao para analisar a sintaxe dos lexemas
    // esta funcao ainda nao foi implementada, mas sera responsavel por verificar
    // se os lexemas formam uma estrutura sintatica valida de acordo com as regras
    // da linguagem de programacao que estamos compilando
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
    fclose(fonte);


    const char* filename = argv[1]; // nome do arquivo de entrada
     // array para armazenar os lexemas
    int count = lexemas(filename, lexemasArray);
    if (count >= 0) {
        arrayPrinter(lexemasArray, count);
    }
    return 0;
}