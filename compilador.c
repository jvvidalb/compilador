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
    novo->endereco = 0;
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

    // Detecta se a linha atual processada é um comando de controle loop/condicional
    int ehLinhaDeControle = (buscarNaFila(&filaSemantica, "if") != NULL ||
                             buscarNaFila(&filaSemantica, "while") != NULL ||
                             buscarNaFila(&filaSemantica, "for") != NULL);

    // Flag para saber se a variável do lado esquerdo foi reativada (era INACESSIVEL)
    int flagReativada = 0;

    while (!filaVazia(&filaSemantica))
    {
        Token tokenConsumido = desenfileirar(&filaSemantica);

        if (tokenConsumido.tipo == NUMERO)
        {
            TNo *no = encontrarSimbolo(tokenConsumido.lexema);
            if (no != NULL && no->tipo == UNKNOWN){
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
                printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' nao encontrada.\n", tokenConsumido.linha, tokenConsumido.lexema);
                exit(1);
            }

            if (!ladoDireito)
            {
                // Lado esquerdo da atribuição (Declaração/Escrita)
                int ehNovaDeclaracao = 0;

                // REGRA: Variável INACESSIVEL no lado esquerdo é reativada com novo tipo
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
                // reatribuição de variável já existente: não invalida ao sair do bloco

                // Só guarda noLocal se for declaração nova dentro de um bloco
                if (proxLine == 1 && ehNovaDeclaracao)
                    noLocal = no;
                if(no->struc != VALOR){
                    no->struc = VARIAVEL;
                }
                

                flag = no;
            }
            else
            {
                // REGRA: Impedir uso de variáveis inacessíveis no lado DIREITO
                if (no->temp == INACESSIVEL)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' nao pode ser acessada (fora do escopo local).\n", tokenConsumido.linha, no->cadeia);
                    exit(1);
                }

                // Lado direito (Uso/Leitura)
                if (no->tipo == UNKNOWN || no->tipo == NAO_DEFINIDO)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' usada antes de ser definida.\n",
                           tokenConsumido.linha, no->cadeia);
                    exit(1);
                }
                if (tipoUltimaExpressao != UNKNOWN && tipoUltimaExpressao != no->tipo)
                {
                    printf("[ERRO SEMANTICO - LINHA %d] Tipo incompativel: '%s' eh %s, esperava %s.\n",
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
                printf("[ERRO SEMANTICO - LINHA %d] Operacao aritmetica requer INTEIRO, encontrou %s.\n",
                       tokenConsumido.linha, tipoDadoParaString(tipoUltimaExpressao));
                exit(1);
            }
        }
        else if (tokenConsumido.tipo == OP_RELACIONAL)
        {
            if (tipoUltimaExpressao == UNKNOWN)
            {
                printf("[ERRO SEMANTICO - LINHA %d] Operacao relacional sem operando definido.\n", tokenConsumido.linha);
                exit(1);
            }
            tipoUltimaExpressao = UNKNOWN;
        }
        else if (tokenConsumido.tipo == ATRIBUICAO)
        {
            ladoDireito = 1;

            // Detecta se o lado direito é uma lista [ ou tupla (
            // O próximo token na fila revela a estrutura
            if (filaSemantica.head != NULL)
            {
                char *prox = filaSemantica.head->token.lexema;
                if (strcmp(prox, "[") == 0 && flag != NULL)
                {
                    flag->struc = LISTA;
                    flag->tipo = INTEIRO; // tipo provisório; será validado pelo conteúdo
                }
                else if (strcmp(prox, "(") == 0 && flag != NULL)
                {
                    flag->struc = TUPLA;
                    flag->tipo = INTEIRO; // tipo provisório
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

            // O próximo token na fila do 'for' obrigatoriamente é o Identificador de iteração (garantido pelo sintático)
            NodoFila *nodoVar = filaSemantica.head;
            if (nodoVar != NULL && nodoVar->token.tipo == IDENTIFICADOR)
            {
                TNo *noIter = encontrarSimbolo(nodoVar->token.lexema);
                if (noIter != NULL)
                {
                    // CASO INVALIDO: variavel ja declarada com tipo incompativel (ex: i = True, for i ...)
                    if (noIter->tipo != UNKNOWN && noIter->tipo != NAO_DEFINIDO && noIter->tipo != INTEIRO)
                    {
                        printf("[ERRO SEMANTICO - LINHA %d] Variavel '%s' ja declarada como %s, nao pode ser usada como iterador (requer INTEIRO).\n",
                               nodoVar->token.linha, noIter->cadeia, tipoDadoParaString(noIter->tipo));
                        exit(1);
                    }

                    // CASO ACESSIVEL: variavel ja era INTEIRO antes do for — mantem acessivel apos o loop
                    // Marcamos com struc = ITERATIVO apenas se era UNKNOWN/NAO_DEFINIDO (nova declaracao do for)
                    int jaEraInteiroExterno = (noIter->tipo == INTEIRO && noIter->temp == ACESSIVEL);

                    noIter->tipo = INTEIRO;
                    noIter->temp = ACESSIVEL;
                    if (!jaEraInteiroExterno)
                    {
                        // Declarada pelo proprio for: invalida apos o bloco
                        noIter->struc = ITERATIVO;
                        noIter->endereco = end++;
                    }
                    // Se ja era INTEIRO externo, mantem struc = VARIAVEL e nao invalida apos o bloco
                }
            }
        }
    }

    if (flag != NULL)
    {
        // Para LISTA e TUPLA, o tipo já foi definido pela estrutura — não precisa checar
        if (flag->struc == LISTA || flag->struc == TUPLA)
        {
            flag = NULL;
            flagReativada = 0;
        }
        else
        {
            if (tipoUltimaExpressao == UNKNOWN)
            {
                printf("[ERRO SEMANTICO] Lado direito da atribuicao sem tipo definido.\n");
                exit(1);
            }
            if (!flagReativada && flag->tipo != NAO_DEFINIDO && flag->tipo != tipoUltimaExpressao)
            {
                printf("[ERRO SEMANTICO] Atribuicao incompativel: '%s' eh %s mas lado direito eh %s.\n",
                       flag->cadeia,
                       tipoDadoParaString(flag->tipo),
                       tipoDadoParaString(tipoUltimaExpressao));
                exit(1);
            }
            flag->tipo = tipoUltimaExpressao;
            flag = NULL;
            flagReativada = 0;
        }
    }

    // Gerenciamento de Escopo ao fim da linha avaliada
    if (ehLinhaDeControle)
    {
        proxLine = 1;
    }
    else if (proxLine == 1)
    {
        // Invalida a variável declarada no corpo do bloco (identificada por noLocal)
        if (noLocal != NULL)
        {
            noLocal->temp = INACESSIVEL;
            noLocal = NULL;
        }
        // Invalida também a variável iteradora do for (identificada por struc == ITERATIVO)
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

void analisadorSintatico()
{
    posicaoAtual = 0;
    lookahead = analisadorLexico();
    START();
}

void START()
{
    // FIRST(START) = F_STMT. FOLLOW(START) = { EOS }. Deriva: STATEMENTS EOS
    STATEMENTS();
    consome(EOS, NULL);
}

void STATEMENTS()
{
    // FIRST(STATEMENTS) = F_STMT. Deriva: STATEMENT STATEMENTS_PRIME
    STATEMENT();
    STATEMENTS_PRIME();
}

void STATEMENTS_PRIME()
{
    // FIRST(STATEMENTS_PRIME) = F_STMT U { EPSILON }. FOLLOW(STATEMENTS_PRIME) = { EOS }
    if (in_F_STMT())
    {
        STATEMENTS();
    }
    // else EPSILON
}

void STATEMENT()
{
    // FIRST(STATEMENT) = F_STMT. Deriva: IF_STATEMENT | WHILE_STATEMENT | FOR_STATEMENT | COMMAND_STATEMENT | ASSIGN_OR_EXPR
    if (lookahead.tipo == T_IF)
    {
        IF_STATEMENT();
    }
    else if (lookahead.tipo == T_WHILE)
    {
        WHILE_STATEMENT();
    }
    else if (lookahead.tipo == T_FOR)
    {
        FOR_STATEMENT();
    }
    else if (lookahead.tipo == T_PRINT || lookahead.tipo == T_BREAK ||
             lookahead.tipo == T_CONTINUE || lookahead.tipo == T_RETURN ||
             lookahead.tipo == T_EXEC || lookahead.tipo == T_RAISE ||
             lookahead.tipo == T_INPUT)
    {
        COMMAND_STATEMENT();
    }
    else if (lookahead.tipo == IDENTIFICADOR || lookahead.tipo == NUMERO ||
             lookahead.tipo == LITERAL ||
             (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0))
    {

        ASSIGN_OR_EXPR();
    }
    else
    {
        erroSintatico("Token inesperado no inicio de um statement");
    }
}

void ASSIGN_OR_EXPR()
{
    // FIRST(ASSIGN_OR_EXPR) = { IDENTIFICADOR, NUMERO, LITERAL, '(' }

    // ASSIGN_OR_EXPR.TYPEDATA = IDENTIFICADOR.TYPEDATA INDEX_OPT.TYPEDATA ASSIGN_OR_EXPR_TAIL.TYPEDATA
    if (lookahead.tipo == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR, NULL);
        INDEX_OPT();
        ASSIGN_OR_EXPR_TAIL();
    }
    else if (lookahead.tipo == NUMERO)
    {
        consome(NUMERO, NULL);
        EXPRESSION_PRIME();
    }
    else if (lookahead.tipo == LITERAL)
    {
        consome(LITERAL, NULL);
        EXPRESSION_PRIME();
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0)
    {
        consome(DELIMITER, "(");
        EXPRESSION();
        consome(DELIMITER, ")");
        EXPRESSION_PRIME();
    }
}

void ASSIGN_OR_EXPR_TAIL()
{
    // FIRST(ASSIGN_OR_EXPR_TAIL) = { '=', OPERATOR, EPSILON }
    if (lookahead.tipo == ATRIBUICAO)
    {
        consome(ATRIBUICAO, NULL);
        EXPRESSION();
    }
    else
    {
        EXPRESSION_PRIME();
    }
}

void IF_STATEMENT()
{
    // FIRST(IF_STATEMENT) = { if }. Deriva: IF_START IF_TAIL
    IF_START();
    IF_TAIL();
}

void IF_START()
{
    // FIRST(IF_START) = { if }. Deriva: if EXPRESSION : STATEMENT
    consome(T_IF, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void IF_TAIL()
{
    // FIRST(IF_TAIL) = { else, elif, EPSILON }. FOLLOW(IF_TAIL) = FL_STMT
    if (lookahead.tipo == T_ELSE)
    {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        STATEMENT();
    }
    else if (lookahead.tipo == T_ELIF)
    {
        ELIF_STATEMENTS();
        STATEMENT();
        ELIF_TAIL();
    }
    // else EPSILON
}

void ELIF_TAIL()
{
    // FIRST(ELIF_TAIL) = { else, EPSILON }. FOLLOW(ELIF_TAIL) = FL_STMT
    if (lookahead.tipo == T_ELSE)
    {
        consome(T_ELSE, NULL);
        consome(DELIMITER, ":");
        STATEMENT();
    }
    // else EPSILON
}

void ELIF_STATEMENTS()
{
    // FIRST(ELIF_STATEMENTS) = { elif }. Deriva: ELIF_STATEMENT ELIF_STATEMENTS_PRIME
    ELIF_STATEMENT();
    ELIF_STATEMENTS_PRIME();
}

void ELIF_STATEMENT()
{
    // FIRST(ELIF_STATEMENT) = { elif }. Deriva: elif EXPRESSION : STATEMENT
    consome(T_ELIF, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void ELIF_STATEMENTS_PRIME()
{
    // FIRST(ELIF_STATEMENTS_PRIME) = { elif, EPSILON }. FOLLOW(ELIF_STATEMENTS_PRIME) = F_STMT
    if (lookahead.tipo == T_ELIF)
    {
        ELIF_STATEMENTS();
    }
    // else EPSILON
}

void WHILE_STATEMENT()
{
    // FIRST(WHILE_STATEMENT) = { while }. Deriva: while EXPRESSION : STATEMENT
    consome(T_WHILE, NULL);
    EXPRESSION();
    consome(DELIMITER, ":");
    STATEMENT();
}

void FOR_STATEMENT()
{
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

void COMMAND_STATEMENT()
{
    // FIRST(COMMAND_STATEMENT) = { print, break, continue, return, exec, raise, input }
    if (lookahead.tipo == T_PRINT)
    {
        consome(T_PRINT, NULL);
        consome(DELIMITER, "(");
        ELEMENTS_OPT();
        consome(DELIMITER, ")");
    }
    else if (lookahead.tipo == T_INPUT)
    {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
    }
    else if (lookahead.tipo == T_BREAK)
    {
        consome(T_BREAK, NULL);
    }
    else if (lookahead.tipo == T_CONTINUE)
    {
        consome(T_CONTINUE, NULL);
    }
    else if (lookahead.tipo == T_RETURN)
    {
        consome(T_RETURN, NULL);
        EXPRESSION();
    }
    else if (lookahead.tipo == T_EXEC)
    {
        consome(T_EXEC, NULL);
        EXPRESSION();
    }
    else if (lookahead.tipo == T_RAISE)
    {
        consome(T_RAISE, NULL);
        EXPRESSION();
    }
}

void INDEX_OPT()
{
    // FIRST(INDEX_OPT) = { '[', EPSILON }. FOLLOW(INDEX_OPT) = FL_STMT U { '=', ':', ')', ']', ',', OPERATOR }
    // INDEX_OPT.TYPEDATA = [ EXPRESSION.TYPEDATA ] INDEX_OPT.TYPEDATA
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0)
    {
        consome(DELIMITER, "[");
        EXPRESSION(); // EXPRESSION.TYPEDATA
        consome(DELIMITER, "]");
        INDEX_OPT(); // INDEX_OPT.TYPEDATA
    }
    // else EPSILON
}

void EXPRESSION()
{
    // FIRST(EXPRESSION) = F_TERM. Deriva: TERM EXPRESSION_PRIME
    // EXPRESSION.TYPEDATA = TERM.TYPEDATA EXPRESSION_PRIME.TYPEDATA
    TERM();             // TERM.TYPEDATA
    EXPRESSION_PRIME(); // EXPRESSION_PRIME.TYPEDATA
}

void EXPRESSION_PRIME()
{
    // FIRST(EXPRESSION_PRIME) = { OPERATOR, EPSILON }. FOLLOW(EXPRESSION_PRIME) = FL_STMT U { ':', ')', ']', ',' }
    if (lookahead.tipo == OP_ARITMETICO || lookahead.tipo == OP_RELACIONAL)
    {
        consome(lookahead.tipo, NULL);
        TERM();
        EXPRESSION_PRIME();
    }
    // else EPSILON
}

void TERM()
{
    // FIRST(TERM) = F_TERM = { IDENTIFICADOR, NUMERO, LITERAL, TRUE, FALSE, '[', '(', len, input }
    /*
    TERM.TYPEDATA = IDENTIFICADOR.TYPEDATA INDEX_OPT.TYPEDATA
    TERM.INTEIRO = NUMERO.INTEIRO
    TERM.BOOLEANO = TRUE.BOOLEANO
    TERM.BOOLEANO = FALSE.BOOLEANO
    TERM.TYPEDATA = LIST.TYPEDATA
    TERM.TYPEDATA = TUPLE_OR_GROUP.TYPEDATA
    */
    if (lookahead.tipo == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR, NULL);
        INDEX_OPT(); // INDEX_OPT.TYPEDATA
    }
    else if (lookahead.tipo == NUMERO)
    {
        consome(NUMERO, NULL);
    }
    else if (lookahead.tipo == LITERAL)
    {
        consome(LITERAL, NULL);
    }
    else if (lookahead.tipo == T_TRUE)
    {
        consome(T_TRUE, NULL);
    }
    else if (lookahead.tipo == T_FALSE)
    {
        consome(T_FALSE, NULL);
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "[") == 0)
    {
        LIST(); // TERM.TYPEDATA = LIST.TYPEDATA
    }
    else if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, "(") == 0)
    {
        TUPLE_OR_GROUP(); // TUPLE_OR_GROUP.TYPEDATA
    }
    else if (lookahead.tipo == T_LEN)
    {
        consome(T_LEN, NULL);
        consome(DELIMITER, "(");
        EXPRESSION();
        consome(DELIMITER, ")");
    }
    else if (lookahead.tipo == T_INPUT)
    {
        consome(T_INPUT, NULL);
        consome(DELIMITER, "(");
        consome(LITERAL, NULL);
        consome(DELIMITER, ")");
    }
    else
    {
        erroSintatico("Esperado um TERM (Identificador, Numero, String, Tupla, Lista, etc)");
    }
}

void LIST()
{
    // FIRST(LIST) = { '[' }. Deriva: [ ELEMENTS_OPT ]
    consome(DELIMITER, "[");

    ELEMENTS_OPT();
    consome(DELIMITER, "]");
}

void TUPLE_OR_GROUP()
{
    // FIRST(TUPLE_OR_GROUP) = { '(' }. Deriva: ( ELEMENTS_OPT )
    consome(DELIMITER, "(");
    ELEMENTS_OPT();
    consome(DELIMITER, ")");
}

void ELEMENTS_OPT()
{
    // FIRST(ELEMENTS_OPT) = F_TERM U { EPSILON }. FOLLOW(ELEMENTS_OPT) = { ']', ')' }
    if (in_F_TERM())
    {
        EXPRESSION();
        ELEMENTS_TAIL();
    }
    // else EPSILON
}

void ELEMENTS_TAIL()
{
    // FIRST(ELEMENTS_TAIL) = { ',', EPSILON }. FOLLOW(ELEMENTS_TAIL) = { ']', ')' }
    if (lookahead.tipo == DELIMITER && strcmp(lookahead.lexema, ",") == 0)
    {
        consome(DELIMITER, ",");
        EXPRESSION();
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
        printf("Analisa lexica e sintaticamente um arquivo fonte em Mini-Python.\n");
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

    // arrayPrinter(lexemasArray,totalLexemasGlob);

    if (totalLexemasGlob >= 0)
    {
        printf("===== INICIANDO ANALISE LEXICASIN, SINTATICA E SEMANTICA =====\n");

        // Inicia a análise sintática
        analisadorSintatico();

        printf("\n> SUCESSO: Analise Sintatica concluida! A gramatica do arquivo eh valida.\n");
        printTabelaSimbolos();

        // Verifica se há variáveis declaradas mas não utilizadas
        possuiVarNaoUtilizada();
    }
    else
    {
        printf("Falha na etapa de analise lexica.\n");
    }

    return 0;
}