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

/*
 *==============================================================================
 * GRAMATICA ESTENDIDA (deu errado, god save me!!!!)
 * ==============================================================================
 *
 * ASSIGN_OR_EXPR.TYPEDATA = IDENTIFICADOR.TYPEDATA INDEX_OPT.TYPEDATA ASSIGN_OR_EXPR_TAIL.TYPEDATA
 * ASSIGN_OR_EXPR.INTEIRO = NUMERO.INTEIRO EXPRESSION_PRIME.INTEIRO
 * ASSIGN_OR_EXPR.TYPEDATA = ( EXPRESSION.TYPEDATA ) EXPRESSION_PRIME.TYPEDATA
 *
 * INDEX_OPT.TYPEDATA = [ EXPRESSION.TYPEDATA ] INDEX_OPT.TYPEDATA
 *
 * ASSIGN_OR_EXPR_TAIL.TYPEDATA = EXPRESSION.TYPEDATA
 *
 * EXPRESSION.TYPEDATA = TERM.TYPEDATA EXPRESSION_PRIME.TYPEDATA
 *
 * EXPRESSION_PRIME.TYPEDATA  = OPERATOR.TYPEDATA TERM.TYPEDATA EXPRESSION_PRIME.TYPEDATA
 *
 * TERM.TYPEDATA = IDENTIFICADOR.TYPEDATA INDEX_OPT.TYPEDATA
 * TERM.INTEIRO = NUMERO.INTEIRO
 * TERM.BOOLEANO = TRUE.BOOLEANO
 * TERM.BOOLEANO = FALSE.BOOLEANO
 * TERM.TYPEDATA = LIST.TYPEDATA
 * TERM.TYPEDATA = TUPLE_OR_GROUP.TYPEDATA
 * LIST.TYPEDATA =  ELEMENTS_OPT.TYPEDATA
 * TUPLE_OR_GROUP.TYPEDATA = ELEMENTS_OPT.TYPEDATA
 * ELEMENTS_OPT.TYPEDATA = EXPRESSION.TYPEDATA ELEMENTS_TAIL.TYPEDATA
 * ELEMENTS_TAIL.TYPEDATA = EXPRESSION.TYPEDATA ELEMENTS_TAIL.TYPEDATA
 */

/*
 * ANÁLISE SEMÂNTICA
 *
 * Utilizamos uma FILA para fazer a análise semântica
 * Os tokens de cada linha são enfileirados durante a análise sintática
 * e processados em ordem (esquerda para direita) pelo analisador semântico
 * ao fim de cada linha ou ao encontrar EOS
 * Assim garantimos que analise sintatica termina e verifica a sintaxe
 * chamamos o Semantico para avaliar o significado desse trecho
 *
 *
 * TIPOS:
 * - Inicialmente todos os símbolos são UNKNOWN, pois a linguagem não é tipada.
 * - Ao ser declarada (lado esquerdo de uma atribuição), a variável passa para
 *   NAO_DEFINIDO — foi iniciada, mas ainda não tem tipo.
 * - O tipo é definido com base no lado direito da atribuição.
 *
 * FLAG:
 * - A flag aponta para o nó da tabela de símbolos do lado esquerdo da atribuição.
 * - Ao fim do processamento da linha, o tipo do lado direito é atribuído à flag.
 *
 * REGRAS:
 * - Variável usada no lado direito com tipo UNKNOWN ou NAO_DEFINIDO → ERRO.
 * - Operação aritmética com tipo diferente de INTEIRO → ERRO.
 * - Tipos incompatíveis na mesma expressão → ERRO.
 * - Atribuição com tipos incompatíveis (ex: variável já era INTEIRO, lado direito é BOOLEANO) → ERRO.
 *
 * EXEMPLO: x = 1 + b * 3
 * Fila (ordem de processamento): x → = → 1 → + → b → * → 3
 *
 * x   → UNKNOWN: vira NAO_DEFINIDO, flag = x
 * =   → ladoDireito = 1
 * 1   → INTEIRO, tipoUltimaExpressao = INTEIRO
 * +   → tipoUltimaExpressao == INTEIRO? sim → operação válida
 * b   → lado direito: b é NAO_DEFINIDO → ERRO: variável não definida
 *
 * EXEMPLO VÁLIDO: x = 1 + 3
 * x   → NAO_DEFINIDO, flag = x
 * =   → ladoDireito = 1
 * 1   → tipoUltimaExpressao = INTEIRO
 * +   → operação válida
 * 3   → tipoUltimaExpressao = INTEIRO
 * fim → flag->tipo = INTEIRO, flag = NULL
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tipo atomo
typedef enum
{
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
    T_IF,
    T_ELIF,
    T_ELSE,
    T_WHILE,
    T_FOR,
    T_DEF,
    T_RETURN,
    T_BREAK,
    T_CONTINUE,
    T_PRINT,
    T_INPUT,
    T_LEN,
    T_RANGE,
    T_TRUE,
    T_FALSE,
    T_WITH,
    T_AS,
    T_FROM,
    T_EXEC,
    T_RAISE,
    DESCONHECIDO
} TAtomo;

typedef enum
{
    INTEIRO,
    BOOLEANO,
    UNKNOWN,
    NAO_DEFINIDO
} TipoDado;

typedef enum
{
    ACESSIVEL,
    INACESSIVEL
} Temp;

typedef enum
{
    VARIAVEL,
    ITERATIVO,
    LISTA,
    TUPLA,
    VALOR
} Estrutura;

// Token
typedef struct
{
    char lexema[100];
    TAtomo tipo;
    int linha;
} Token;
;

typedef struct NodoFila
{
    Token token;
    struct NodoFila *proximo;
} NodoFila;

typedef struct
{
    NodoFila *head;
    NodoFila *tail;
} FilaSemantica;

// Funções auxiliares
void enfileirar(FilaSemantica *fila, Token t)
{
    NodoFila *novo = (NodoFila *)malloc(sizeof(NodoFila));
    novo->token = t;
    novo->proximo = NULL;
    if (fila->tail == NULL)
    {
        fila->head = fila->tail = novo;
    }
    else
    {
        fila->tail->proximo = novo;
        fila->tail = novo;
    }
}

Token desenfileirar(FilaSemantica *fila)
{
    if (fila->head == NULL)
    {
        printf("[ERRO] Fila vazia!\n");
        exit(1);
    }
    NodoFila *aux = fila->head;
    Token t = aux->token;
    fila->head = aux->proximo;
    if (fila->head == NULL)
        fila->tail = NULL; // fila ficou vazia
    free(aux);
    return t;
}

NodoFila *buscarNaFila(FilaSemantica *fila, char *lexema)
{
    NodoFila *atual = fila->head;
    while (atual != NULL)
    {
        if (strcmp(atual->token.lexema, lexema) == 0)
        {
            return atual;
        }
        atual = atual->proximo;
    }

    return NULL;
}

int filaVazia(FilaSemantica *fila)
{
    return fila->head == NULL;
}

// Old Tabela de símbolos
/*typedef struct {
    char valor[100];
    TAtomo tipo;
} SimboloTabela;*/

typedef struct _TNo
{
    char cadeia[100]; // id
    TAtomo atomo;
    TipoDado tipo;
    int usado;
    int endereco;
    Temp temp;
    Estrutura struc;
    struct _TNo *prox;
} TNo;

typedef struct
{
    TNo *head;
    TNo *tail;
    int totalSimbolos;
} SimboloTabela;

// VARS GLOBAIS
FILE *fonte;
int posicaoAtual = 0;
char lexemasArray[512][512];
int linhaAtual = 1;
Token lookahead;
FilaSemantica filaSemantica = {NULL, NULL};
// SimboloTabela tabelaSimbolos[1024];
// int totalSimbolos = 0;
SimboloTabela globalTabela = {NULL, NULL, 0};
TNo *flag = NULL;    // pega linha da tabela
TNo *noLocal = NULL; // aponta para a variável declarada dentro de um bloco de controle
int tagLine = 1;
int totalLexemasGlob = 0;
int mapaLinhasGlobais[512]; // Adicione esta linha
TipoDado tipoUltimaExpressao = UNKNOWN;
int end = 0;
int proxLine = 0;

// FUNCOES AUXILIARES
int isLetra(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int isDigito(char c)
{
    return c >= '0' && c <= '9';
}

int isSeparadorValido(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == EOF;
}

// funcao que pega o arquivo formatado e retorna uma array de lexemas
// todos os lexemas obrigatoriamente sao separados por espaco, o leitor
// apenas le os caracteres ate encontrar um espaco, e entao armazena o
// lexema em um array de strings
int lexemas(const char *filename, char lexemas[512][512], int mapaLinhas[512])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
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

    while ((c = fgetc(file)) != EOF)
    {
        // Atualiza a contagem global de linhas
        if (c == '\n')
        {
            linha_atual++;
        }

        // Ignora comentários e avança até o fim da linha
        if (!in_string && c == '#')
        {
            if (buf_idx > 0)
            {
                buffer[buf_idx] = '\0';
                strcpy(lexemas[count], buffer);
                mapaLinhas[count] = linha_inicio_lexema; // Salva a linha do lexema
                count++;
                buf_idx = 0;
            }
            // Pula os caracteres até achar a quebra de linha ou o fim do arquivo
            while ((c = fgetc(file)) != EOF)
            {
                if (c == '\n')
                {
                    linha_atual++;
                    break;
                }
            }
            continue;
        }

        // Se estamos começando a ler um novo lexema, registramos em qual linha ele começou
        if (buf_idx == 0 && c != ' ' && c != '\t' && c != '\n' && c != '\r')
        {
            linha_inicio_lexema = linha_atual;
        }

        // Trata strings literais
        if (c == '"')
        {
            in_string = !in_string;
            buffer[buf_idx++] = (char)c;
            continue;
        }

        // Trata espaços em branco fora de strings (separadores)
        if (!in_string && (c == ' ' || c == '\t' || c == '\n' || c == '\r'))
        {
            if (buf_idx > 0)
            {
                buffer[buf_idx] = '\0';
                if (count < 512)
                {
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
    if (buf_idx > 0 && count < 512)
    {
        buffer[buf_idx] = '\0';
        strcpy(lexemas[count], buffer);
        mapaLinhas[count] = linha_inicio_lexema;
        count++;
    }

    fclose(file);
    return count;
}

// Função para adicionar à tabela de símbolos (evita duplicatas)
void adicionarSimbolo(char *lexema, TAtomo tipo)
{
    TNo *atual = globalTabela.head;

    // Verifica se já existe
    while (atual != NULL)
    {
        if (strcmp(atual->cadeia, lexema) == 0)
        {
            return;
        }
        atual = atual->prox;
    }

    // Cria novo nó — calloc zera todos os campos, evitando lixo de memória
    TNo *novo = (TNo *)calloc(1, sizeof(TNo));

    if (novo == NULL)
    {
        printf("Erro de alocacao.\n");
        return;
    }

    // Preenche os dados
    strncpy(novo->cadeia, lexema, sizeof(novo->cadeia) - 1);
    novo->cadeia[sizeof(novo->cadeia) - 1] = '\0';

    novo->atomo = tipo;
    novo->tipo = UNKNOWN;
    novo->temp = ACESSIVEL;
    novo->struc = VARIAVEL;
    novo->usado = 0;
    novo->endereco = -1; // Endereço indefinido inicialmente
    globalTabela.totalSimbolos++;

    novo->prox = NULL;
    if (globalTabela.head == NULL)
    {
        // Se a lista está vazia, novo é o head e o tail
        globalTabela.head = novo;
        globalTabela.tail = novo;
    }
    else
    {
        // Se não está vazia, o atual tail aponta para o novo
        globalTabela.tail->prox = novo;
        // E o novo vira o novo tail
        globalTabela.tail = novo;
    }

    /*
    for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].valor, lexema) == 0) return;
    }
    if (totalSimbolos < 1024) {
        strncpy(tabelaSimbolos[totalSimbolos].valor, lexema, 99);
        tabelaSimbolos[totalSimbolos].tipo = tipo;
        totalSimbolos++;
    }*/
}

void removerSimbolo(char *lexema)
{
    TNo *atual = globalTabela.head;
    TNo *anterior = NULL;

    while (atual != NULL)
    {
        if (strcmp(atual->cadeia, lexema) == 0)
        {
            if (anterior == NULL)
            {
                // Remover o head
                globalTabela.head = atual->prox;
            }
            else
            {
                // Pular o nó atual
                anterior->prox = atual->prox;
            }

            if (atual == globalTabela.tail)
            {
                // Se for o tail, atualizar o tail
                globalTabela.tail = anterior;
            }

            free(atual);
            return;
        }
        anterior = atual;
        atual = atual->prox;
    }
}

TNo *encontrarSimbolo(char *lexema)
{
    TNo *atual = globalTabela.head;
    while (atual != NULL)
    {
        if (strcmp(atual->cadeia, lexema) == 0)
        {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;

    /*for (int i = 0; i < totalSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].valor, lexema) == 0) return i;
    }
    return -1;*/
}

Token *obter_atomo(char *lexema)
{

    static Token token;
    token.tipo = DESCONHECIDO;
    strncpy(token.lexema, lexema, sizeof(token.lexema) - 1);
    token.linha = linhaAtual;

    // 1. LITERAL (Strings entre aspas)
    if (lexema[0] == '"')
    {
        token.tipo = LITERAL;
        return &token;
    }

    // 2. NUMBER (Apenas dígitos)
    if (isDigito(lexema[0]))
    {
        int eh_numero_valido = 1;

        // Verifica do segundo caractere em diante
        for (int i = 1; lexema[i] != '\0'; i++)
        {
            if (!isDigito(lexema[i]))
            {
                eh_numero_valido = 0;
                break;
            }
        }

        if (eh_numero_valido)
        {
            token.tipo = NUMERO;
            adicionarSimbolo(lexema, NUMERO);
        }
        else
        {
            token.tipo = DESCONHECIDO;
        }

        return &token;
    }

    // 3. KEYWORDS
    if (strcmp(lexema, "return") == 0)
        token.tipo = T_RETURN;
    else if (strcmp(lexema, "from") == 0)
        token.tipo = T_FROM;
    else if (strcmp(lexema, "while") == 0)
        token.tipo = T_WHILE;
    else if (strcmp(lexema, "as") == 0)
        token.tipo = T_AS;
    else if (strcmp(lexema, "elif") == 0)
        token.tipo = T_ELIF;
    else if (strcmp(lexema, "with") == 0)
        token.tipo = T_WITH;
    else if (strcmp(lexema, "else") == 0)
        token.tipo = T_ELSE;
    else if (strcmp(lexema, "if") == 0)
        token.tipo = T_IF;
    else if (strcmp(lexema, "break") == 0)
        token.tipo = T_BREAK;
    else if (strcmp(lexema, "len") == 0)
        token.tipo = T_LEN;
    else if (strcmp(lexema, "input") == 0)
        token.tipo = T_INPUT;
    else if (strcmp(lexema, "print") == 0)
        token.tipo = T_PRINT;
    else if (strcmp(lexema, "exec") == 0)
        token.tipo = T_EXEC;
    else if (strcmp(lexema, "raise") == 0)
        token.tipo = T_RAISE;
    else if (strcmp(lexema, "continue") == 0)
        token.tipo = T_CONTINUE;
    else if (strcmp(lexema, "range") == 0)
        token.tipo = T_RANGE;
    else if (strcmp(lexema, "def") == 0)
        token.tipo = T_DEF;
    else if (strcmp(lexema, "for") == 0)
        token.tipo = T_FOR;
    else if (strcmp(lexema, "True") == 0)
        token.tipo = T_TRUE;
    else if (strcmp(lexema, "False") == 0)
        token.tipo = T_FALSE;

    // 4. OPERATORS (Aritméticos e Lógicos conforme o Regex)
    else if (strcmp(lexema, "**") == 0 || strcmp(lexema, "+") == 0 ||
             strcmp(lexema, "-") == 0 || strcmp(lexema, "*") == 0 ||
             strcmp(lexema, "/") == 0 || strcmp(lexema, "%") == 0 ||
             strcmp(lexema, "~") == 0)
    {
        token.tipo = OP_ARITMETICO;
    }
    else if (strcmp(lexema, "==") == 0 || strcmp(lexema, "!=") == 0 ||
             strcmp(lexema, "<>") == 0 || strcmp(lexema, "<=") == 0 ||
             strcmp(lexema, ">=") == 0 || strcmp(lexema, "<") == 0 ||
             strcmp(lexema, ">") == 0 || strcmp(lexema, "and") == 0 ||
             strcmp(lexema, "or") == 0 || strcmp(lexema, "not") == 0 ||
             strcmp(lexema, "in") == 0 || strcmp(lexema, "is") == 0)
    {
        token.tipo = OP_RELACIONAL;
    }

    // 5. DELIMITERS
    else if (strcmp(lexema, "=") == 0)
        token.tipo = ATRIBUICAO;
    else if (strchr("()[]{,}:.;", lexema[0]) && strlen(lexema) == 1)
    {
        token.tipo = DELIMITER;
    }

    // 6. IDENTIFIER (Se começar com letra/_ e não for Keyword)
    else if (isLetra(lexema[0]))
    {
        int eh_identificador_valido = 1;

        // Verifica do segundo caractere em diante
        for (int i = 1; lexema[i] != '\0'; i++)
        {
            // Um identificador válido só pode conter letras, '_' (já coberto pelo isLetra) ou dígitos
            if (!isLetra(lexema[i]) && !isDigito(lexema[i]))
            {
                eh_identificador_valido = 0;
                break; // Encontrou um intruso (ex: '$', '-'), para a verificação
            }
        }

        if (eh_identificador_valido)
        {
            token.tipo = IDENTIFICADOR;
            adicionarSimbolo(lexema, IDENTIFICADOR);
        }
        else
        {
            // Rejeita tokens malformados como "val$" ou "company-name"
            token.tipo = DESCONHECIDO;
        }
    }

    return &token;
}

char *tempParaString(Temp t)
{
    switch (t)
    {
    case ACESSIVEL:
        return "ACESSIVEL";
    case INACESSIVEL:
        return "INACESSIVEL";
    default:
        return "DESCONHECIDO";
    }
}

char *estruturaParaString(Estrutura e)
{
    switch (e)
    {
    case VALOR:
        return "VALOR";
    case VARIAVEL:
        return "VARIAVEL";
    case ITERATIVO:
        return "ITERATIVO";
    case LISTA:
        return "LISTA";
    case TUPLA:
        return "TUPLA";
    default:
        return "DESCONHECIDO";
    }
}
char *tipoDadoParaString(TipoDado tipo)
{
    switch (tipo)
    {
    case NAO_DEFINIDO:
        return "NAO_DEFINIDO";
    case INTEIRO:
        return "INTEIRO";
    case BOOLEANO:
        return "BOOLEANO";
    default:
        return "UNKNOWN";
    }
}

char *atomoParaString(TAtomo tipo)
{
    switch (tipo)
    {
    case ERRO:
        return "ERRO";
    case IDENTIFICADOR:
        return "IDENTIFICADOR";
    case NUMERO:
        return "NUMERO";
    case LITERAL:
        return "LITERAL";
    case EOS:
        return "EOS";
    case COMMENT:
        return "COMMENT";
    case OP_RELACIONAL:
        return "OP_RELACIONAL";
    case OP_ARITMETICO:
        return "OP_ARITMETICO";
    case ATRIBUICAO:
        return "ATRIBUICAO";
    case DELIMITER:
        return "DELIMITER";
    case T_IF:
        return "T_IF";
    case T_ELIF:
        return "T_ELIF";
    case T_ELSE:
        return "T_ELSE";
    case T_WHILE:
        return "T_WHILE";
    case T_FOR:
        return "T_FOR";
    case T_DEF:
        return "T_DEF";
    case T_RETURN:
        return "T_RETURN";
    case T_BREAK:
        return "T_BREAK";
    case T_CONTINUE:
        return "T_CONTINUE";
    case T_PRINT:
        return "T_PRINT";
    case T_INPUT:
        return "T_INPUT";
    case T_LEN:
        return "T_LEN";
    case T_RANGE:
        return "T_RANGE";
    case T_TRUE:
        return "T_TRUE";
    case T_FALSE:
        return "T_FALSE";
    case T_WITH:
        return "T_WITH";
    case T_AS:
        return "T_AS";
    case T_FROM:
        return "T_FROM";
    case T_EXEC:
        return "T_EXEC";
    case T_RAISE:
        return "T_RAISE";
    default:
        return "DESCONHECIDO";
    }
}

char *obterNomeOperadorDelimitador(char *op)
{
    static char nome[20];
    if (strcmp(op, ">") == 0)
        strcpy(nome, "GT");
    else if (strcmp(op, "<") == 0)
        strcpy(nome, "LT");
    else if (strcmp(op, ">=") == 0)
        strcpy(nome, "GE");
    else if (strcmp(op, "<=") == 0)
        strcpy(nome, "LE");
    else if (strcmp(op, "==") == 0)
        strcpy(nome, "EQ");
    else if (strcmp(op, "!=") == 0)
        strcpy(nome, "NE");
    else if (strcmp(op, "<>") == 0)
        strcpy(nome, "NE");
    else if (strcmp(op, "+") == 0)
        strcpy(nome, "SUM");
    else if (strcmp(op, "-") == 0)
        strcpy(nome, "SUBTRACTION");
    else if (strcmp(op, "*") == 0)
        strcpy(nome, "MULTIPLICATION");
    else if (strcmp(op, "**") == 0)
        strcpy(nome, "POWER");
    else if (strcmp(op, "/") == 0)
        strcpy(nome, "DIVISION");
    else if (strcmp(op, "%") == 0)
        strcpy(nome, "MODULUS");
    else if (strcmp(op, "~") == 0)
        strcpy(nome, "NOT");
    else if (strcmp(op, "(") == 0)
        strcpy(nome, "LPAREN");
    else if (strcmp(op, ")") == 0)
        strcpy(nome, "RPAREN");
    else if (strcmp(op, "[") == 0)
        strcpy(nome, "LBRACKET");
    else if (strcmp(op, "]") == 0)
        strcpy(nome, "RBRACKET");
    else if (strcmp(op, "{") == 0)
        strcpy(nome, "LBRACE");
    else if (strcmp(op, "}") == 0)
        strcpy(nome, "RBRACE");
    else if (strcmp(op, ",") == 0)
        strcpy(nome, "COMMA");
    else if (strcmp(op, ":") == 0)
        strcpy(nome, "COLON");
    else if (strcmp(op, ".") == 0)
        strcpy(nome, "DOT");
    else if (strcmp(op, "=") == 0)
        strcpy(nome, "EQUALS");
    else if (strcmp(op, ";") == 0)
        strcpy(nome, "SEMICOLON");
    else if (strcmp(op, "in") == 0)
        strcpy(nome, "IN");
    else if (strcmp(op, "is") == 0)
        strcpy(nome, "IS");
    else if (strcmp(op, "and") == 0)
        strcpy(nome, "AND");
    else if (strcmp(op, "or") == 0)
        strcpy(nome, "OR");
    else if (strcmp(op, "not") == 0)
        strcpy(nome, "NOT");
    else
        strcpy(nome, "DESCONHECIDO");
    return nome;
}

void printToken(Token *token)
{
    if (token->tipo == DESCONHECIDO)
    {
        printf("%d# ERRO: Lexema '%s' desconhecido.\n", token->linha, token->lexema);
        exit(1);
    }
    else if (token->tipo == LITERAL || token->tipo == IDENTIFICADOR ||
             token->tipo == NUMERO)
    {
        TNo *simbolo = encontrarSimbolo(token->lexema);

        printf("%d# %s | %s\n", token->linha, atomoParaString(token->tipo), simbolo != NULL ? simbolo->cadeia : "NULL");
    }
    else if (token->tipo == OP_ARITMETICO || token->tipo == OP_RELACIONAL || token->tipo == DELIMITER)
    {
        printf("%d# %s | '%s'\n", token->linha, atomoParaString(token->tipo), obterNomeOperadorDelimitador(token->lexema));
    }
    else
    {
        printf("%d# %s\n", token->linha, atomoParaString(token->tipo));
    }
}

void printTabelaSimbolos()
{
    printf("\n===== TABELA DE SIMBOLOS =====\n");

    TNo *atual = globalTabela.head;
    int i = 0;

    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("%-5s | %-20s | %-18s | %-12s | %-12s | %-12s | %-6s | %-8s\n",
           "Idx", "Cadeia", "Atomo", "Tipo", "Estado", "Estrutura", "Usado", "Endereco");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    while (atual != NULL)
    {
        printf("%-5d | %-20s | %-18s | %-12s | %-12s | %-12s | %-6d | %-8d\n",
               i,
               atual->cadeia,
               atomoParaString(atual->atomo),
               tipoDadoParaString(atual->tipo),
               tempParaString(atual->temp),
               estruturaParaString(atual->struc),
               atual->usado,
               atual->endereco);

        atual = atual->prox;
        i++;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");
}

void arrayPrinter(char lexemas[512][512], int count)
{
    Token *token = obter_atomo(lexemas[posicaoAtual++]);
    while (posicaoAtual <= count)
    {
        printToken(token);
        token = obter_atomo(lexemas[posicaoAtual++]);
    }
}

void erroSintatico(const char *mensagem)
{
    printf("\n[ERRO SINTATICO - LINHA %d] %s. (Lookahead atual: '%s' | Tipo: %s)\n",
           lookahead.linha, mensagem, lookahead.lexema, atomoParaString(lookahead.tipo));
    exit(1);
}

Token analisadorLexico()
{
    if (posicaoAtual < totalLexemasGlob)
    {
        Token *t = obter_atomo(lexemasArray[posicaoAtual]);
        t->linha = mapaLinhasGlobais[posicaoAtual]; // linha salva pelo mapa

        if (t->tipo == DESCONHECIDO)
        {
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

void analisadorSemantico()
{
    tipoUltimaExpressao = UNKNOWN;

    if (buscarNaFila(&filaSemantica, "input") != NULL)
    {
        while (!filaVazia(&filaSemantica))
            desenfileirar(&filaSemantica);
        return;
    }

    int temAtribuicao = buscarNaFila(&filaSemantica, "=") != NULL ? 1 : 0;
    int ladoDireito = !temAtribuicao ? 1 : 0;

    int ehLinhaDeControle = (buscarNaFila(&filaSemantica, "if") != NULL ||
                             buscarNaFila(&filaSemantica, "while") != NULL ||
                             buscarNaFila(&filaSemantica, "for") != NULL);

    int flagReativada = 0;
    int linhaFlag = 0; // linha do token do lado esquerdo (para erros no bloco flag)

    // Detecta "x =" sem lado direito:
    // Percorre a fila; se o último token for '=' (ATRIBUICAO), não há lado direito.
    int ladoDireitoVazio = 0;
    if (temAtribuicao)
    {
        NodoFila *n = filaSemantica.head;
        NodoFila *ultimo = NULL;
        while (n != NULL)
        {
            ultimo = n;
            n = n->proximo;
        }
        if (ultimo != NULL && ultimo->token.tipo == ATRIBUICAO)
            ladoDireitoVazio = 1;
    }

    while (!filaVazia(&filaSemantica))
    {
        Token tokenConsumido = desenfileirar(&filaSemantica);

        if (tokenConsumido.tipo == NUMERO)
        {
            TNo *no = encontrarSimbolo(tokenConsumido.lexema);
            if (no != NULL && no->tipo == UNKNOWN)
            {
                no->tipo = INTEIRO;
                no->struc = VALOR;
            }
            tipoUltimaExpressao = INTEIRO;
        }
        else if (tokenConsumido.tipo == T_TRUE || tokenConsumido.tipo == T_FALSE)
        {
            tipoUltimaExpressao = BOOLEANO;
        }
        else if (tokenConsumido.tipo == IDENTIFICADOR)
        {
            TNo *no = encontrarSimbolo(tokenConsumido.lexema);
            if (no == NULL)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' nao encontrada na tabela de simbolos.\n",
                       tokenConsumido.linha, tokenConsumido.lexema);
                exit(1);
            }

            if (!ladoDireito)
            {
                int ehNovaDeclaracao = 0;

                if (no->temp == INACESSIVEL)
                {
                    no->tipo = NAO_DEFINIDO;
                    no->temp = ACESSIVEL;
                    no->struc = VARIAVEL;
                    no->usado = 0;
                    flagReativada = 1;
                    ehNovaDeclaracao = 1;
                }
                else if (no->tipo == UNKNOWN)
                {
                    no->tipo = NAO_DEFINIDO;
                    no->endereco = end++;
                    ehNovaDeclaracao = 1;
                }

                if (proxLine == 1 && ehNovaDeclaracao)
                    noLocal = no;
                if (no->struc != VALOR)
                    no->struc = VARIAVEL;

                flag = no;
                linhaFlag = tokenConsumido.linha;
                // lado direito vazio → declara como NAO_DEFINIDO, encerra processamento
                if (ladoDireitoVazio)
                {
                    if (no->endereco == -1)
                        no->endereco = end++;
                    while (!filaVazia(&filaSemantica))
                        desenfileirar(&filaSemantica);
                    flag = NULL;
                    flagReativada = 0;
                    // Sai do while mas ainda executa o gerenciamento de escopo abaixo
                    break;
                }
            }
            else
            {
                if (no->temp == INACESSIVEL)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' nao pode ser acessada fora do escopo local onde foi declarada.\n",
                           tokenConsumido.linha, no->cadeia);
                    exit(1);
                }

                if (no->tipo == UNKNOWN || no->tipo == NAO_DEFINIDO)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' usada antes de ser definida (tipo: %s).\n",
                           tokenConsumido.linha, no->cadeia,
                           tipoDadoParaString(no->tipo));
                    exit(1);
                }
                if (tipoUltimaExpressao != UNKNOWN && tipoUltimaExpressao != no->tipo)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Tipo incompativel na expressao: '%s' eh %s, mas o contexto esperava %s.\n",
                           tokenConsumido.linha, no->cadeia,
                           tipoDadoParaString(no->tipo),
                           tipoDadoParaString(tipoUltimaExpressao));
                    exit(1);
                }
                tipoUltimaExpressao = no->tipo;
                no->usado = 1;
            }
        }
        else if (tokenConsumido.tipo == OP_ARITMETICO)
        {
            if (tipoUltimaExpressao != INTEIRO)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Operacao aritmetica com operador '%s' requer operandos do tipo INTEIRO, mas encontrou %s.\n",
                       tokenConsumido.linha, tokenConsumido.lexema,
                       tipoDadoParaString(tipoUltimaExpressao));
                exit(1);
            }
        }
        else if (tokenConsumido.tipo == OP_RELACIONAL)
        {
            if (tipoUltimaExpressao == UNKNOWN)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Operacao relacional com operador '%s' sem operando definido antes dele.\n",
                       tokenConsumido.linha, tokenConsumido.lexema);
                exit(1);
            }
            // Se for um operador de magnitude (>, <, >=, <=), ambos os lados devem ser INTEIRO
            if (strcmp(tokenConsumido.lexema, ">") == 0 || strcmp(tokenConsumido.lexema, "<") == 0 ||
                strcmp(tokenConsumido.lexema, ">=") == 0 || strcmp(tokenConsumido.lexema, "<=") == 0)
            {

                if (tipoUltimaExpressao != INTEIRO)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Operador '%s' requer operando do tipo INTEIRO.\n", tokenConsumido.linha, tokenConsumido.lexema);
                    exit(1);
                }
                // Mantemos INTEIRO para forçar que o próximo operando (o da direita) também seja INTEIRO
                tipoUltimaExpressao = INTEIRO;
            }
            else
            {
                // Para operadores como == e !=, você pode permitir qualquer tipo,
                // mas eles devem ser iguais. Nesse caso, salve em uma variável auxiliar ou trate adequadamente.
                tipoUltimaExpressao = UNKNOWN;
            }
        }
        else if (tokenConsumido.tipo == ATRIBUICAO)
        {
            ladoDireito = 1;

            if (filaSemantica.head != NULL)
            {
                char *prox = filaSemantica.head->token.lexema;
                if (strcmp(prox, "[") == 0 && flag != NULL)
                {
                    flag->struc = LISTA;
                    flag->tipo = INTEIRO;
                }
                else if (strcmp(prox, "(") == 0 && flag != NULL)
                {
                    flag->struc = TUPLA;
                    flag->tipo = INTEIRO;
                }
            }
        }
        else if (tokenConsumido.tipo == T_IF || tokenConsumido.tipo == T_WHILE)
        {
            proxLine = 1;
            ladoDireito = 1;
        }
        else if (tokenConsumido.tipo == T_FOR)
        {
            proxLine = 1;
            ladoDireito = 1;

            NodoFila *nodoVar = filaSemantica.head;
            if (nodoVar != NULL && nodoVar->token.tipo == IDENTIFICADOR)
            {
                TNo *noIter = encontrarSimbolo(nodoVar->token.lexema);
                if (noIter != NULL)
                {
                    if (noIter->tipo != UNKNOWN && noIter->tipo != NAO_DEFINIDO && noIter->tipo != INTEIRO)
                    {
                        printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' ja declarada como %s e nao pode ser usada como iterador do 'for' (iterador requer tipo INTEIRO).\n",
                               nodoVar->token.linha, noIter->cadeia,
                               tipoDadoParaString(noIter->tipo));
                        exit(1);
                    }

                    int jaEraInteiroExterno = (noIter->tipo == INTEIRO && noIter->temp == ACESSIVEL);
                    noIter->tipo = INTEIRO;
                    noIter->temp = ACESSIVEL;
                    if (!jaEraInteiroExterno)
                    {
                        noIter->struc = ITERATIVO;
                        noIter->endereco = end++;
                    }
                }
            }
        }
    }

    if (flag != NULL)
    {
        if (flag->struc == LISTA || flag->struc == TUPLA)
        {
            flag = NULL;
            flagReativada = 0;
        }
        else
        {
            if (tipoUltimaExpressao == UNKNOWN)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Lado direito da atribuicao de '%s' nao possui tipo definido (expressao vazia ou invalida).\n",
                       linhaFlag, flag->cadeia);
                exit(1);
            }
            if (!flagReativada && flag->tipo != NAO_DEFINIDO && flag->tipo != tipoUltimaExpressao)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Atribuicao incompativel: variavel '%s' eh do tipo %s mas o lado direito da expressao eh %s.\n",
                       linhaFlag, flag->cadeia,
                       tipoDadoParaString(flag->tipo),
                       tipoDadoParaString(tipoUltimaExpressao));
                exit(1);
            }
            flag->tipo = tipoUltimaExpressao;
            flag = NULL;
            flagReativada = 0;
        }
    }

    tipoUltimaExpressao = UNKNOWN;

    if (ehLinhaDeControle)
    {
        proxLine = 1;
    }
    else if (proxLine == 1)
    {
        if (noLocal != NULL)
        {
            noLocal->temp = INACESSIVEL;
            noLocal = NULL;
        }
        TNo *atual = globalTabela.head;
        while (atual != NULL)
        {
            if (atual->struc == ITERATIVO)
                atual->temp = INACESSIVEL;
            atual = atual->prox;
        }
        proxLine = 0;
    }

    tipoUltimaExpressao = UNKNOWN;
}

void possuiVarNaoUtilizada()
{
    TNo *atual = globalTabela.head;
    while (atual != NULL)
    {
        if (atual->atomo == IDENTIFICADOR && atual->usado == 0)
        {
            printf("[ERRO SEMANTICO] Variavel '%s' declarada mas nao utilizada.\n", atual->cadeia);
        }
        atual = atual->prox;
    }
}

/*switch (tipo)
{
    case VARIAVEL:
        TNo* no = encontrarSimbolo(lookahead.lexema);
        if(no->tipo == UNKNOWN){
            if(!tag){
                no->tipo = NAO_DEFINIDO;
                flag = no;
                tag = 0;
            }else{
                printf("\n%s não pode ser atibuído, pois não foi definido e nem declarado antes\n", no->cadeia);
                exit(1);
            }
        }

        break;
    case INTEIRO:
        TNo* t = encontrarSimbolo(lookahead.lexema);
        if(t->tipo == UNKNOWN){
            t->tipo = INTEIRO;
        }
        if(flag->tipo == NAO_DEFINIDO){
            printf("to aq");
            flag->tipo = INTEIRO;
        }
        break;
    case BOOLEANO:
        if(flag->tipo == NAO_DEFINIDO){
            flag->tipo = BOOLEANO;
        }
        break;
    default:
        break;
}*/

/*if (tipo == VARIAVEL) {
    TNo* no = encontrarSimbolo(lookahead.lexema);

    if (tag == 0) {
        // LADO ESQUERDO: x = ...
        if (no->tipo == UNKNOWN) {
            no->tipo = NAO_DEFINIDO;
        }
        flag = no;
    }
    else {

        // verifica lado direto
        if (no->tipo == UNKNOWN || no->tipo == NAO_DEFINIDO) {
            printf("\n[ERRO SEMANTICO] Variavel '%s' nao inicializada na linha %d\n", no->cadeia, lookahead.linha);
            exit(1);
        }
        // Se passou, a variável é válida. O tipo da expressão vira o tipo dela.
        tipoUltimaExpressao = no->tipo;
    }
}
else if (tipo == INTEIRO) {
    tipoUltimaExpressao = INTEIRO;
    // tipamos a flag
    if (flag != NULL && tag == 0) {
        flag->tipo = INTEIRO;
    }
}
else if (tipo == BOOLEANO) {
    tipoUltimaExpressao = BOOLEANO;
    if (flag != NULL && tag == 0) {
        flag->tipo = BOOLEANO;
    }
}*/

/*if(no == NULL){

}
if(no->tipo == NAO_DEFINIDO){ //x = x + 5

}else if(no->tipo == INTEIRO){ // X = A + 5
    flag->tipo = INTEIRO;
}
else{
    no->tipo = NAO_DEFINIDO;
    flag = no;
}
}else if(tipo == INTEIRO){
flag->tipo = INTEIRO;
}else if(tipo == BOOLEANO){
flag->tipo = BOOLEANO;
}*/

// Verifica e consome um tipo específico de átomo
// Função única para consumir tokens: verifica o tipo e, opcionalmente, o lexema exato.
void consome(TAtomo tipo_esperado, const char *lexema_esperado)
{

    if (lookahead.tipo != EOS)
        printToken(&lookahead); // Imprime o token atual antes de consumir
    if (lookahead.tipo == tipo_esperado)
    {
        // Se lexema_esperado não for NULL, precisamos garantir que a string exata bate
        if (lexema_esperado != NULL)
        {
            if (strcmp(lookahead.lexema, lexema_esperado) == 0)
            {
                posicaoAtual++;
                if (lookahead.linha == tagLine && lookahead.tipo != EOS)
                {

                    enfileirar(&filaSemantica, lookahead);
                }
                else
                {

                    if (!filaVazia(&filaSemantica))
                    {
                        analisadorSemantico();
                    }
                    tagLine = lookahead.linha;
                    enfileirar(&filaSemantica, lookahead);
                }
                lookahead = analisadorLexico();
            }
            else
            {
                char msg[256];
                sprintf(msg, "Esperava o simbolo/atributo '%s' (%s), mas encontrou '%s'",
                        lexema_esperado, atomoParaString(tipo_esperado), lookahead.lexema);
                erroSintatico(msg);
            }
        }
        else
        {
            // Se for NULL, apenas a classe do token (o tipo) importa
            posicaoAtual++;
            if (lookahead.linha == tagLine && lookahead.tipo != EOS)
            {

                enfileirar(&filaSemantica, lookahead);
            }
            else
            {
                if (!filaVazia(&filaSemantica))
                {
                    analisadorSemantico();
                }
                tagLine = lookahead.linha;
                enfileirar(&filaSemantica, lookahead);
            }
            lookahead = analisadorLexico();
        }
    }
    else
    {
        // Erro de tipo
        char msg[256];
        if (lexema_esperado != NULL)
        {
            sprintf(msg, "Esperava %s ('%s')", atomoParaString(tipo_esperado), lexema_esperado);
        }
        else
        {
            sprintf(msg, "Esperava token do tipo %s", atomoParaString(tipo_esperado));
        }
        erroSintatico(msg);
    }
}

// Funcoes auxiliares da geracao de codigo intermediario

int temp_counter = 1;
int rotulo_counter = 1;
FILE *code_output = NULL; // O PDF pede para salvar em um arquivo .txt

// Gera uma nova variável temporária (t1, t2, t3...)
void novo_temp(char *buffer)
{
    sprintf(buffer, "t%d", temp_counter++);
}

// Gera um novo rótulo (L1, L2, L3...) conforme exigido pelo PDF
void proximo_rotulo(char *buffer)
{
    sprintf(buffer, "L%d", rotulo_counter++);
}

// Função para emitir código para o arquivo texto
void emitir(const char *instrucao)
{
    fprintf(code_output, "%s\n", instrucao);
    // printf("%s\n", instrucao);
}

typedef enum
{
    NODE_OPERACAO,
    NODE_VARIAVEL,
    NODE_NUMERO,
    NODE_ATRIBUICAO,
    NODE_ATRIBUICAO_INDEXADA,
    NODE_INDEX_GET,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_IGNORAR
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    char lexema[100];
    struct ASTNode *esq;
    struct ASTNode *dir;
    struct ASTNode *body_else;
    struct ASTNode *prox;
} ASTNode;

ASTNode *criarNo(NodeType type, const char *lexema)
{
    ASTNode *no = (ASTNode *)calloc(1, sizeof(ASTNode));
    no->type = type;
    if (lexema)
        strcpy(no->lexema, lexema);
    return no;
}

char *gerarCodigoIntermediario(ASTNode *no)
{
    if (no == NULL)
        return NULL;

    ASTNode *atual = no;
    char *last_val = NULL; // Armazena o último valor gerado (ex: t1, t2)

    while (atual != NULL)
    {
        char *current_val = NULL;

        if (atual->type == NODE_IGNORAR)
        {
            // Não faz nada, apenas prossegue
        }
        else if (atual->type == NODE_VARIAVEL || atual->type == NODE_NUMERO)
        {
            current_val = (char *)malloc(100);
            strcpy(current_val, atual->lexema);
        }
        else if (atual->type == NODE_OPERACAO)
        {
            char *esq = gerarCodigoIntermediario(atual->esq);
            char *dir = gerarCodigoIntermediario(atual->dir);

            if (!esq)
            {
                esq = (char *)malloc(2);
                strcpy(esq, "0");
            }
            if (!dir)
            {
                dir = (char *)malloc(2);
                strcpy(dir, "0");
            }

            current_val = (char *)malloc(100);
            novo_temp(current_val);

            char inst[200];
            sprintf(inst, "%s = %s %s %s", current_val, esq, atual->lexema, dir);
            emitir(inst);
            free(esq);
            free(dir);
        }
        else if (atual->type == NODE_ATRIBUICAO)
        {
            char *dir = gerarCodigoIntermediario(atual->dir);
            if (!dir)
            {
                dir = (char *)malloc(2);
                strcpy(dir, "0");
            }

            char inst[200];
            sprintf(inst, "%s = %s", atual->lexema, dir);
            emitir(inst);
            free(dir);
        }
        else if (atual->type == NODE_INDEX_GET)
        {
            char *index = gerarCodigoIntermediario(atual->esq);
            if (!index)
            {
                index = (char *)malloc(2);
                strcpy(index, "0");
            }

            current_val = (char *)malloc(100);
            novo_temp(current_val);

            char inst[200];
            sprintf(inst, "%s = %s [%s]", current_val, atual->lexema, index);
            emitir(inst);
            free(index);
        }
        else if (atual->type == NODE_ATRIBUICAO_INDEXADA)
        {
            char *index = gerarCodigoIntermediario(atual->esq);
            char *val_dir = gerarCodigoIntermediario(atual->dir);

            if (!index)
            {
                index = (char *)malloc(2);
                strcpy(index, "0");
            }
            if (!val_dir)
            {
                val_dir = (char *)malloc(2);
                strcpy(val_dir, "0");
            }

            char inst[200];
            sprintf(inst, "%s [%s] = %s", atual->lexema, index, val_dir);
            emitir(inst);
            free(index);
            free(val_dir);
        }
        // ==========================================================
        // FLUXO DE CONTROLE (IF, WHILE, FOR) - AGORA ELES SERÃO LIDOS!
        // ==========================================================
        else if (atual->type == NODE_IF)
        {
            char *cond = gerarCodigoIntermediario(atual->esq);
            if (!cond)
            {
                cond = (char *)malloc(2);
                strcpy(cond, "0");
            }

            char rotuloFim[20];
            proximo_rotulo(rotuloFim);

            char inst[200];
            sprintf(inst, "ifFalse %s goto %s", cond, rotuloFim);
            emitir(inst);
            free(cond);

            gerarCodigoIntermediario(atual->dir); // Avalia o que tá dentro do IF

            sprintf(inst, "%s:", rotuloFim);
            emitir(inst);
        }
        else if (atual->type == NODE_WHILE)
        {
            char rotuloInicio[20], rotuloFim[20];
            proximo_rotulo(rotuloInicio);
            proximo_rotulo(rotuloFim);

            char inst[200];
            sprintf(inst, "%s:", rotuloInicio);
            emitir(inst);

            char *cond = gerarCodigoIntermediario(atual->esq);
            if (!cond)
            {
                cond = (char *)malloc(2);
                strcpy(cond, "0");
            }

            sprintf(inst, "ifFalse %s goto %s", cond, rotuloFim);
            emitir(inst);
            free(cond);

            gerarCodigoIntermediario(atual->dir);

            sprintf(inst, "goto %s", rotuloInicio);
            emitir(inst);

            sprintf(inst, "%s:", rotuloFim);
            emitir(inst);
        }
        else if (atual->type == NODE_FOR)
        {
            char rotuloInicio[20], rotuloFim[20];
            proximo_rotulo(rotuloInicio);
            proximo_rotulo(rotuloFim);

            char inst[200];

            sprintf(inst, "%s = 0", atual->lexema);
            emitir(inst);

            sprintf(inst, "%s:", rotuloInicio);
            emitir(inst);

            char *limite = gerarCodigoIntermediario(atual->esq);
            if (!limite)
            {
                limite = (char *)malloc(2);
                strcpy(limite, "0");
            }

            char *temp_cond = (char *)malloc(100);
            novo_temp(temp_cond);
            sprintf(inst, "%s = %s < %s", temp_cond, atual->lexema, limite);
            emitir(inst);

            sprintf(inst, "ifFalse %s goto %s", temp_cond, rotuloFim);
            emitir(inst);

            gerarCodigoIntermediario(atual->dir);

            char *temp_inc = (char *)malloc(100);
            novo_temp(temp_inc);
            sprintf(inst, "%s = %s + 1", temp_inc, atual->lexema);
            emitir(inst);
            sprintf(inst, "%s = %s", atual->lexema, temp_inc);
            emitir(inst);

            sprintf(inst, "goto %s", rotuloInicio);
            emitir(inst);
            sprintf(inst, "%s:", rotuloFim);
            emitir(inst);

            free(limite);
            free(temp_cond);
            free(temp_inc);
        }

        // Limpa a memória pra não vazar e salva o valor final deste nó
        if (last_val)
            free(last_val);
        last_val = current_val;

        // Pula pra a próxima instrução (if, while, etc.) sem abortar!
        atual = atual->prox;
    }

    // Só envia o valor pra cima quando a lista inteira terminar
    return last_val;
}

// FIRST(STATEMENT)
int in_F_STMT()
{
    return (lookahead.tipo == T_IF || lookahead.tipo == T_WHILE || lookahead.tipo == T_FOR ||
            lookahead.tipo == T_PRINT || lookahead.tipo == T_BREAK || lookahead.tipo == T_CONTINUE ||
            lookahead.tipo == T_RETURN || lookahead.tipo == T_EXEC || lookahead.tipo == T_RAISE ||
            lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || lookahead.tipo == LITERAL ||
            (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0));
}

// FIRST(TERM)
int in_F_TERM()
{
    return (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO || lookahead.tipo == LITERAL ||
            lookahead.tipo == T_TRUE || lookahead.tipo == T_FALSE ||
            (lookahead.tipo == DELIMITER && (strcmp(lookahead.lexema, "[") == 0 || strcmp(lookahead.lexema, "(") == 0)) ||
            lookahead.tipo == T_LEN || lookahead.tipo == T_INPUT);
}

ASTNode *START();
ASTNode *STATEMENTS();
ASTNode *STATEMENTS_PRIME();
ASTNode *STATEMENT();
ASTNode *ASSIGN_OR_EXPR();
ASTNode *ASSIGN_OR_EXPR_TAIL(char *ladoEsquerdo, ASTNode *index);
ASTNode *IF_STATEMENT();
ASTNode *IF_START();
void IF_TAIL(ASTNode *no_if);
ASTNode *ELIF_STATEMENTS();
void ELIF_TAIL(ASTNode *no_elif);
ASTNode *WHILE_STATEMENT();
ASTNode *FOR_STATEMENT();
ASTNode *EXPRESSION();
ASTNode *EXPRESSION_PRIME(ASTNode *ladoEsquerdo);
ASTNode *TERM();
ASTNode *COMMAND_STATEMENT();
ASTNode *INDEX_OPT();
void LIST();
void TUPLE_OR_GROUP();
void ELEMENTS_OPT();
void ELEMENTS_TAIL();
// ==============================================================================
// ANALISADOR SINTÁTICO RECURSIVO DESCENDENTE PREDITIVO
// ==============================================================================

ASTNode *raizPrograma = NULL;

void analisadorSintatico()
{
    posicaoAtual = 0;
    lookahead = analisadorLexico();
    raizPrograma = START();
}

ASTNode *START()
{
    ASTNode *raiz = STATEMENTS();
    consome(EOS, NULL);
    return raiz;
}

ASTNode *STATEMENTS()
{
    ASTNode *primeiro = STATEMENT();
    ASTNode *resto = STATEMENTS_PRIME();

    if (primeiro != NULL)
    {
        primeiro->prox = resto;
        return primeiro;
    }
    return resto;
}

ASTNode *STATEMENTS_PRIME()
{
    if (in_F_STMT())
    {
        return STATEMENTS();
    }
    return NULL; // EPSILON
}

ASTNode *STATEMENT()
{
    if (lookahead.tipo == T_IF)
    {
        return IF_STATEMENT();
    }
    else if (lookahead.tipo == T_WHILE)
    {
        return WHILE_STATEMENT();
    }
    else if (lookahead.tipo == T_FOR)
    {
        return FOR_STATEMENT();
    }
    else if (lookahead.tipo == T_PRINT || lookahead.tipo == T_BREAK ||
             lookahead.tipo == T_CONTINUE || lookahead.tipo == T_RETURN ||
             lookahead.tipo == T_EXEC || lookahead.tipo == T_RAISE ||
             lookahead.tipo == T_INPUT)
    {
        return COMMAND_STATEMENT();
    }
    else if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO ||
             lookahead.tipo == LITERAL ||
             (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0))
    {
        return ASSIGN_OR_EXPR();
    }
    else
    {
        erroSintatico("Token inesperado no inicio de um statement");
        return NULL;
    }
}

ASTNode *ASSIGN_OR_EXPR()
{
    if (lookahead.tipo == IDENTIFICADOR)
    {
        char id_lexema[100];
        strcpy(id_lexema, lookahead.lexema);
        consome(IDENTIFICADOR, NULL);

        // Verifica se há acesso a vetor (ex: x[i])
        ASTNode *index = INDEX_OPT();
        return ASSIGN_OR_EXPR_TAIL(id_lexema, index);
    }
    else if (lookahead.tipo == NUMERO)
    {
        ASTNode *num = criarNo(NODE_NUMERO, lookahead.lexema);
        consome(NUMERO, NULL);
        return EXPRESSION_PRIME(num);
    }
    else if (lookahead.tipo == LITERAL)
    {
        ASTNode *lit = criarNo(NODE_VARIAVEL, lookahead.lexema);
        consome(LITERAL, NULL);
        return EXPRESSION_PRIME(lit);
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0)
    {
        consome(DELIMITER, "(");
        ASTNode *expr = EXPRESSION();
        consome(DELIMITER, ")");
        return EXPRESSION_PRIME(expr);
    }
    return NULL;
}

ASTNode *ASSIGN_OR_EXPR_TAIL(char *ladoEsquerdo, ASTNode *index)
{
    if (lookahead.tipo == ATRIBUICAO)
    {
        consome(ATRIBUICAO, NULL);
        ASTNode *valDir = EXPRESSION();

        if (index != NULL)
        {
            // Atribuição Indexada: x[i] = y
            ASTNode *noAtribIdx = criarNo(NODE_ATRIBUICAO_INDEXADA, ladoEsquerdo);
            noAtribIdx->esq = index;
            noAtribIdx->dir = valDir;
            return noAtribIdx;
        }
        else
        {
            // Cópia Normal: x = y
            ASTNode *noAtrib = criarNo(NODE_ATRIBUICAO, ladoEsquerdo);
            noAtrib->dir = valDir;
            return noAtrib;
        }
    }
    else
    {
        // Apenas expressão matemática
        ASTNode *noEsq;
        if (index != NULL)
        {
            // Leitura indexada: t1 = y[i]
            noEsq = criarNo(NODE_INDEX_GET, ladoEsquerdo);
            noEsq->esq = index;
        }
        else
        {
            // Variável simples: y
            noEsq = criarNo(NODE_VARIAVEL, ladoEsquerdo);
        }
        return EXPRESSION_PRIME(noEsq);
    }
}

ASTNode *IF_STATEMENT()
{
    return IF_START();
}

ASTNode *IF_START()
{
    consome(T_IF, NULL);
    ASTNode *no_if = criarNo(NODE_IF, "if");
    no_if->esq = EXPRESSION();
    consome(DELIMITER, ":");
    no_if->dir = STATEMENT();
    IF_TAIL(no_if);
    return no_if;
}

void IF_TAIL(ASTNode *no_if)
{
    if (lookahead.tipo == T_ELSE)
    {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        no_if->body_else = STATEMENT();
    }
    else if (lookahead.tipo == T_ELIF)
    {
        no_if->body_else = ELIF_STATEMENTS();
    }
    // else EPSILON
}

ASTNode *ELIF_STATEMENTS()
{
    ASTNode *no_elif = criarNo(NODE_IF, "elif");
    consome(T_ELIF, NULL);
    no_elif->esq = EXPRESSION();
    consome(DELIMITER, ":");
    no_elif->dir = STATEMENT();
    ELIF_TAIL(no_elif);
    return no_elif;
}

void ELIF_TAIL(ASTNode *no_elif)
{
    if (lookahead.tipo == T_ELSE)
    {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        no_elif->body_else = STATEMENT();
    }
    else if (lookahead.tipo == T_ELIF)
    {
        no_elif->body_else = ELIF_STATEMENTS();
    }
    // else EPSILON
}

ASTNode *WHILE_STATEMENT()
{
    consome(T_WHILE, NULL);
    ASTNode *no_while = criarNo(NODE_WHILE, "while");
    no_while->esq = EXPRESSION();
    consome(DELIMITER, ":");
    no_while->dir = STATEMENT();
    return no_while;
}

ASTNode *FOR_STATEMENT()
{
    consome(T_FOR, NULL);
    char id_iterador[100];
    strcpy(id_iterador, lookahead.lexema);
    consome(IDENTIFICADOR, NULL);
    consome(OP_RELACIONAL, "in");
    consome(T_RANGE, NULL);
    consome(DELIMITER, "(");

    ASTNode *no_for = criarNo(NODE_FOR, id_iterador);
    no_for->esq = EXPRESSION();

    consome(DELIMITER, ")");
    consome(DELIMITER, ":");
    no_for->dir = STATEMENT();
    return no_for;
}

ASTNode *COMMAND_STATEMENT()
{
    // Comandos ignorados na Geração de Código de 3-Endereços (não há funções)
    if (lookahead.tipo == T_PRINT)
    {
        consome(T_PRINT, NULL);
        consome(DELIMITER, "(");
        ELEMENTS_OPT();
        consome(DELIMITER, ")");
        return criarNo(NODE_IGNORAR, "print");
    }
    else if (lookahead.tipo == T_INPUT)
    {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
        return criarNo(NODE_IGNORAR, "input");
    }
    else if (lookahead.tipo == T_BREAK)
    {
        consome(T_BREAK, NULL);
        return criarNo(NODE_IGNORAR, "break");
    }
    else if (lookahead.tipo == T_CONTINUE)
    {
        consome(T_CONTINUE, NULL);
        return criarNo(NODE_IGNORAR, "continue");
    }
    else if (lookahead.tipo == T_RETURN)
    {
        consome(T_RETURN, NULL);
        ASTNode *expr = EXPRESSION();
        if (expr)
            free(expr); // Ignora a avaliação para evitar vazamento
        return criarNo(NODE_IGNORAR, "return");
    }
    else if (lookahead.tipo == T_EXEC)
    {
        consome(T_EXEC, NULL);
        ASTNode *expr = EXPRESSION();
        if (expr)
            free(expr);
        return criarNo(NODE_IGNORAR, "exec");
    }
    else if (lookahead.tipo == T_RAISE)
    {
        consome(T_RAISE, NULL);
        ASTNode *expr = EXPRESSION();
        if (expr)
            free(expr);
        return criarNo(NODE_IGNORAR, "raise");
    }
    return NULL;
}

ASTNode *INDEX_OPT()
{
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0)
    {
        consome(DELIMITER, "[");
        ASTNode *exprIndex = EXPRESSION();
        consome(DELIMITER, "]");
        INDEX_OPT();
        return exprIndex;
    }
    return NULL; // EPSILON
}

ASTNode *EXPRESSION()
{
    ASTNode *termo_esq = TERM();
    return EXPRESSION_PRIME(termo_esq);
}

ASTNode *EXPRESSION_PRIME(ASTNode *ladoEsquerdo)
{
    if (lookahead.tipo == OP_ARITMETICO || lookahead.tipo == OP_RELACIONAL)
    {
        ASTNode *noOp = criarNo(NODE_OPERACAO, lookahead.lexema);
        consome(lookahead.tipo, NULL);

        noOp->esq = ladoEsquerdo;
        noOp->dir = TERM();

        return EXPRESSION_PRIME(noOp);
    }
    return ladoEsquerdo; // EPSILON
}

ASTNode *TERM()
{
    if (lookahead.tipo == IDENTIFICADOR)
    {
        char lexema[100];
        strcpy(lexema, lookahead.lexema);
        consome(IDENTIFICADOR, NULL);

        ASTNode *index = INDEX_OPT();
        if (index != NULL)
        {
            ASTNode *noIndexGet = criarNo(NODE_INDEX_GET, lexema);
            noIndexGet->esq = index;
            return noIndexGet;
        }
        else
        {
            return criarNo(NODE_VARIAVEL, lexema);
        }
    }
    else if (lookahead.tipo == NUMERO)
    {
        ASTNode *no = criarNo(NODE_NUMERO, lookahead.lexema);
        consome(NUMERO, NULL);
        return no;
    }
    else if (lookahead.tipo == LITERAL)
    {
        ASTNode *no = criarNo(NODE_VARIAVEL, lookahead.lexema);
        consome(LITERAL, NULL);
        return no;
    }
    else if (lookahead.tipo == T_TRUE)
    {
        ASTNode *no = criarNo(NODE_NUMERO, "1"); // Booleano vira numérico
        consome(T_TRUE, NULL);
        return no;
    }
    else if (lookahead.tipo == T_FALSE)
    {
        ASTNode *no = criarNo(NODE_NUMERO, "0");
        consome(T_FALSE, NULL);
        return no;
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0)
    {
        LIST();
        return criarNo(NODE_IGNORAR, "lista");
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0)
    {
        consome(DELIMITER, "(");
        ASTNode *expr = NULL;

        if (in_F_TERM())
        {
            expr = EXPRESSION(); // Pega a matemática dentro dos parênteses!

            // Se for uma Tupla (ex: True, False), descarta o resto para o código intermediário
            while (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, ",") == 0)
            {
                consome(DELIMITER, ",");
                ASTNode *extra = EXPRESSION();
                if (extra)
                    free(extra);
            }
        }
        consome(DELIMITER, ")");
        return expr ? expr : criarNo(NODE_IGNORAR, "grupo_vazio");
    }
    else if (lookahead.tipo == T_LEN)
    {
        consome(T_LEN, NULL);
        consome(DELIMITER, "(");
        ASTNode *expr = EXPRESSION();
        if (expr)
            free(expr); // Ignora a leitura do parâmetro interno no código 3D
        consome(DELIMITER, ")");
        return criarNo(NODE_IGNORAR, "len");
    }
    else if (lookahead.tipo == T_INPUT)
    {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
        return criarNo(NODE_IGNORAR, "input");
    }
    else
    {
        erroSintatico("Esperado um TERM (Identificador, Numero, String, Tupla, Lista, etc)");
        return NULL;
    }
}

void LIST()
{
    consome(DELIMITER, "[");
    ELEMENTS_OPT();
    consome(DELIMITER, "]");
}

void TUPLE_OR_GROUP()
{
    consome(DELIMITER, "(");
    ELEMENTS_OPT();
    consome(DELIMITER, ")");
}

void ELEMENTS_OPT()
{
    if (in_F_TERM())
    {
        ASTNode *exp = EXPRESSION();
        if (exp)
            free(exp); // Previne vazamentos de memória nas coisas ignoradas
        ELEMENTS_TAIL();
    }
    // else EPSILON
}

void ELEMENTS_TAIL()
{
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, ",") == 0)
    {
        consome(DELIMITER, ",");
        ASTNode *exp = EXPRESSION();
        if (exp)
            free(exp);
        ELEMENTS_TAIL();
    }
    // else EPSILON
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        printf("Analisa lexica, sintatica e semanticamente um arquivo fonte em Mini-Python.\n");
        return 0;
    }
    else if (argc > 2)
    {
        printf("Erro: Muitos argumentos fornecidos.\n");
        printf("Uso: %s <arquivo_fonte.mp>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // Carrega os lexemas do arquivo na array e suas linhas correspondentes no mapa
    totalLexemasGlob = lexemas(filename, lexemasArray, mapaLinhasGlobais);

    if (totalLexemasGlob >= 0)
    {
        printf("===== INICIANDO ANALISE LEXICA, SINTATICA E SEMANTICA =====\n");

        // 1. INICIA O PARSER E A FILA SEMÂNTICA
        // Aqui, a Árvore de Sintaxe Abstrata (AST) será construída e armazenada na global 'raizPrograma'
        // Ao mesmo tempo, a Fila Semântica validará os tipos linha por linha.
        analisadorSintatico();

        // 2. VERIFICAÇÃO SEMÂNTICA FINAL
        // Checa se alguma variável recebeu valor e nunca foi lida.
        possuiVarNaoUtilizada();

        printf("\n> SUCESSO: Analises Sintatica e Semantica concluidas!\n");
        printf("> Nenhuma quebra de tipagem encontrada. A gramatica do arquivo e valida.\n");

        printTabelaSimbolos();

        // 3. GERAÇÃO DO CÓDIGO INTERMEDIÁRIO (FASE 3)
        // Se o código chegou até aqui, significa que a AST é 100% segura para gerar código.
        code_output = fopen("codigo_intermediario.txt", "w");

        if (code_output == NULL)
        {
            printf("Erro ao criar o arquivo de codigo intermediario.\n");
            return 1;
        }

        if (raizPrograma != NULL)
        {
            gerarCodigoIntermediario(raizPrograma);
        }

        fclose(code_output);
        printf("\n> O codigo intermediario (3 enderecos) foi gerado no arquivo 'codigo_intermediario.txt'.\n");
    }
    else
    {
        printf("Falha na etapa de analise lexica.\n");
    }

    return 0;
}