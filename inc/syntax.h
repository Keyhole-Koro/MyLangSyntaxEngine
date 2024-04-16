#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdio.h>

#include "tokenize.h"
#include "symbol.h"
#include "stringMapper.h"
#include "utils.h"

/** @brief use 0 indicates an end of the array
    0 is not used for symbol */
#define END_SYMBOL_ARRAY (0)

typedef struct ProductionRule ProductionRule;

struct ProductionRule {
    symbol nonTerminal; // left
    symbol *production; // right
    int read_pos;
    ProductionRule *next;
};

extern ProductionRule *prod_rules;

void processSyntaxTxt(char *file_path);

void showProductionRules();

/** @brief Terms
    production:
    a whole of rules
    (example)
        expression    : term
              | expression '+' term
              | expression '-' term
        factor        : NUMBER
              | '(' expression ')'
    rule:
    syntaxes individually
    (example)
        expression    : term
        expression    : expression '+' term
        factor        : NUMBER
*/

#endif
