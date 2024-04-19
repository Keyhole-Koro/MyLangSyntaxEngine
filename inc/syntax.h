#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdio.h>

#include "tokenize.h"
#include "symbol.h"
#include "stringMapper.h"
#include "utils.h"

extern int curLatestId;

/** @brief Use 0 to indicate an end of the array */
#define END_SYMBOL_ARRAY (0)

typedef struct ProductionRule ProductionRule;

struct ProductionRule {
    int id;
    symbol nonTerminal; // Left side
    int numSymbols; // the number of elements of production below
    symbol *production; // Right side
    int dotPos;
    ProductionRule *next;
};

extern ProductionRule *prod_rules;

ProductionRule *processSyntaxTxt(char *file_path);
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
