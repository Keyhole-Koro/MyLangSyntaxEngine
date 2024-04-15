#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdio.h>

#include "tokenize.h"
#include "symbol.h"
#include "stringMapper.h"
#include "utils.h"

typedef struct ProductionRule {
    symbol nonTerminal;
    int num_symbol;
    symbol *production;
    struct ProductionRule *next;
} symbolArray;

extern ProductionRule *productions;

void processSyntaxTxt(char *file_path);

void showProductionRules();

#endif
