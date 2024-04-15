#include "syntax.h"

#define terminal 1
#define non_terminal 0

int current_num_syntax = 0;
int max_size_syntax = 32;
ProductionRule *prod_rules;

static ProductionRule *constructRule(symbol left, symbol *right, int size_right);
static void registerSyntax(symbol left, symbol *right, int num_symbol);
symbol *processRightBuffer(Token *cur, int *len_right, Token **rest);

void processSyntaxTxt(char *file_path) {
    FILE *file;
    char line[50];

    file = fopen(file_path, "r");
    if (file == NULL) {
        exit(EXIT_FAILURE);
    }

    Token head;
    head.next = NULL;
    Token *cur = &head;

    // tokenize
    while (fgets(line, sizeof(line), file) != NULL) {
        cur = tokenizeLine(line, cur);
    }

    printTokenList(&head);

    symbol left = 0;
    Token *rest = NULL;

    // register syntax (messy)
    for (Token *current = &head; current; current = current->next) {

        Token *next= current->next;

        if (current->kind == NON_TERMINAL && next->kind == COLON) {
            left = mapString(current->value, non_terminal);
            continue;
        } else if (current->kind == COLON || current->kind == PIPE) {
            int len_right = 0;
            symbol *right = processRightBuffer(next, &len_right, &rest);

            registerSyntax(left, right, len_right);
            current = rest;

            continue;
        }
    }

    fclose(file);
}

static ProductionRule *constructRule(symbol left, symbol *right, int size_right) {
    ProductionRule *rule = (ProductionRule *)malloc(sizeof(ProductionRule));
    if (rule == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    rule->nonTerminal = left;
    rule->num_symbol = size_right;
    rule->production = right;
    rule->next = NULL;
    return rule;
}

static void registerSyntax(symbol left, symbol *right, int num_symbol) {
    ProductionRule *newRule = constructRule(left, right, num_symbol);
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
symbol *processRightBuffer(Token *cur, int *len_right, Token **rest) {
    int size_right_buffer = 5;
    int num_tk = 0;
    symbol *newRight = malloc(size_right_buffer * sizeof(symbol));
    Token *current;
    for (current = cur; current->next; current = current->next) {
        if (current->kind == NEWLINE) break;
    	
        if (current->kind != TERMINAL && current->kind != NON_TERMINAL) continue;
        newRight[num_tk++] = current->kind == TERMINAL ? 
            mapString(current->value, terminal) : mapString(current->value, non_terminal);
    }
            
    *len_right = num_tk;

    *rest = current;

    return newRight;
}

void showProductionRules() {
    ProductionRule *current = prod_rules;
    for (; current; current = current->next) {
        printf("----\n");
        printf("nonTerminal: %d\n", current->nonTerminal);
        printf("number of productions: %d\n", current->num_symbol);
        printf("production:");
        for (int i = 0; i < current->num_symbol; i++) {
            printf(" %d", current->production[i]);
        }
        printf("\n");
        printf("----\n");
    }
}
