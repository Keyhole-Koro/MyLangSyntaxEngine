#include "syntax.h"

#define terminal 1
#define non_terminal 0

int current_num_syntax = 0;
int max_size_syntax = 32;
ProductionRule *prod_rules;

static ProductionRule *constructRule(symbol left, symbol *right);
static void registerSyntax(symbol left, symbol *right);
symbol *processRightBuffer(IN Token *cur, OUT Token **rest);

void processSyntaxTxt(char *file_path) {
    FILE *file;
    int len_line = 50;
    char line[len_line];

    // initialize the array
    for (int i = 0; i < len_line; i++) {
        line[i] = 0;
    }

    file = fopen(file_path, "r");
    if (file == NULL) {
        printf("failed to open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    Token head;
    head.next = NULL;
    Token *cur = &head;

    // tokenize
    while (fgets(line, sizeof(line), file) != NULL) {
        cur = tokenizeLine(line, cur);
    }

    // register syntax (messy)
    for (Token *current = &head; current; current = current->next) {
        symbol left = 0;
        Token *rest = NULL;

        Token *next= current->next;

        /** 
            @brief example sample1.txt
            term          :
        */
        if (current->kind == NON_TERMINAL && next->kind == COLON) {
            left = mapString(current->value, non_terminal);
            continue;
        /**
            @brief example sample1.txt
                : factor
                | term '*' factor
        */
        } else if (current->kind == COLON || current->kind == PIPE) {
            symbol *right = processRightBuffer(next, &rest);

            registerSyntax(left, right);
            current = rest;

            continue;
        }
    }

    fclose(file);
}

static ProductionRule *constructRule(symbol left, symbol *right) {
    ProductionRule *rule = (ProductionRule *)malloc(sizeof(ProductionRule));
    if (rule == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    rule->nonTerminal = left;
    rule->production = right;
    rule->read_pos = 0;
    rule->next = NULL;
    return rule;
}

static void registerSyntax(symbol left, symbol *right) {
    ProductionRule *newRule = constructRule(left, right);
    if (prod_rules == NULL) {
        prod_rules = newRule;
    } else {
        ProductionRule *current = prod_rules;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newRule;
    }
}
symbol *processRightBuffer(Token *cur, Token **rest) {
    int len_right = 5;
    int num_tk = 0;
    symbol *newRight = malloc(len_right * sizeof(symbol));
    newRight[0] = END_SYMBOL_ARRAY;
    Token *current;
    for (current = cur; current->next; current = current->next) {
        if (current->kind == NEWLINE) break;
    	
        if (current->kind != TERMINAL && current->kind != NON_TERMINAL) continue;

        if (num_tk + 1 > len_right) {
            len_right *= 2;
            newRight = realloc(newRight, len_right * sizeof(symbol));
        }

        newRight[num_tk++] = current->kind == TERMINAL ? 
            mapString(current->value, terminal) : mapString(current->value, non_terminal);
    }

    newRight[num_tk] = END_SYMBOL_ARRAY;

    *rest = current;

    return newRight;
}

void showProductionRules() {
    ProductionRule *current = prod_rules;
    for (; current; current = current->next) {
        printf("----\n");
        printf("nonTerminal: %d\n", current->nonTerminal);
        printf("production:");
        symbol sym = 0; // initialization
        int i = 0;
        while ((sym = current->production[i]) != END_SYMBOL_ARRAY) {
            printf(" %d", sym);
            i++;
        }
        printf("\n");
        printf("----\n");
    }
}
