#include "item.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ReviseOffset(n) (n + abs(id_nonTerminal))
#define UNSET_ID (-1)

ProductionRule *grammarRules;
ProductionRule startProductionRule = {UNSET_ID, 0, 0, NULL, 0, NULL};
const ProductionRule *startProductionRuleCopy = NULL;

LR1Item *startItemSet = NULL;
int numItemSets = 0;

LR1Item *constructItemSet(LR1Item *item);
void addGotoItem(LR1Item *item, LR1Item *gotoItem);
LR1Item *findItemSet(ProductionRule *production, symbol lookahead);
LR1Item *findItemInGotoSets(LR1Item *expectedItem);
symbol *extractLookaheadSymbols(ProductionRule *production);
void advanceDot(ProductionRule *prod);
ProductionRule *closure(ProductionRule *targetProd);

void addLR1ItemList(LR1Item *item);
LR1Item *findItemFromList(LR1Item *targetItem, bool (*cmpMethod)(LR1Item*, LR1Item*));

LR1Item *constructInitialItemSet() {
    if (startProductionRule.id == UNSET_ID) {
        fprintf(stderr, "The start rule has not been set.\n");
        fprintf(stderr, "Set the start by using setStartRule(ProductionRule*) before running constructInitialItemSet\n");
        exit(EXIT_FAILURE);
    }

    LR1Item *startItem = malloc(sizeof(LR1Item));
    if (!startItem) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    startItem->stateId = numItemSets++;
    startItem->production = &startProductionRule;
    startItem->lookahead = 0;
    startItem->numGotoItems = 0;
    startItem->maxGotoItems = 5;
    startItem->gotoItems = calloc(startItem->maxGotoItems, sizeof(LR1Item *));

    constructItemSet(startItem);

    return startItemSet;
}

LR1Item *constructItemSet(LR1Item *item) {
    if (!startItemSet) startItemSet = item;

    printf("------------------------------------\n");
    printf("Number of item sets: %d\n", numItemSets);

    ProductionRule *clonedProd = cloneProduction(item->production);
    symbol *lookaheadSymbols = extractLookaheadSymbols(clonedProd);

    int i = 0;
    symbol sym = 0;

    while ((sym = lookaheadSymbols[i++]) != END_SYMBOL_ARRAY) {
        ProductionRule *commonRules = filterProductions(clonedProd, getCurrentSymbol, sym);
        ProductionRule *closureRules = closure(commonRules);

        if (!commonRules && !closureRules) continue;

        advanceDot(commonRules);
       
        ProductionRule *allRequiredProductions = combineProductions(closureRules, commonRules);
        LR1Item *foundItemSet = findItemSet(allRequiredProductions, sym);
        if (foundItemSet) {
            addGotoItem(item, foundItemSet);
            continue;
        }

        printf(">Next lookahead symbol: %s\n", exchangeSymbol(sym));
        LR1Item *newItemSet = calloc(1, sizeof(LR1Item));
        if (!newItemSet) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        newItemSet->stateId = numItemSets++;
        newItemSet->production = allRequiredProductions;
        newItemSet->lookahead = sym;
        newItemSet->numGotoItems = 0;
        newItemSet->maxGotoItems = 6;
        newItemSet->gotoItems = calloc(newItemSet->maxGotoItems, sizeof(LR1Item *));
        if (!newItemSet->gotoItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        for (ProductionRule *rule = allRequiredProductions; rule; rule = rule->next) {
            printf("Rule ID: %d, lookahead symbol: '%s', number of symbols: %d, dot position: %d\n",
                rule->id,
                exchangeSymbol(rule->production[rule->dotPos]),
                rule->numSymbols,
                rule->dotPos);
            
            printf("[");
            for (int i = 0; i < rule->numSymbols; i++) {
                printf("%s", exchangeSymbol(rule->production[i]));
                if (i < rule->numSymbols - 1) {
                    printf(" ");
                }
            }
            printf("]\n");
        }

        addLR1ItemList(newItemSet);
        
        addGotoItem(item, newItemSet);
        constructItemSet(newItemSet);
    }
    
    free(lookaheadSymbols);
    return NULL;
}

void addGotoItem(LR1Item *item, LR1Item *gotoItem) {
    if (item->numGotoItems > item->maxGotoItems) {
        item->gotoItems = realloc(item->gotoItems, (item->maxGotoItems *= 2) * sizeof(LR1Item *));
        if (!item->gotoItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    item->gotoItems[item->numGotoItems++] = gotoItem;
}

int list_numLR1Items = 0;
int list_maxLR1Items = 20;
LR1Item **LR1ItemList = NULL;
LR1Item **setLR1ItemList() {
    LR1ItemList = calloc(list_maxLR1Items, sizeof(LR1Item *));
    return LR1ItemList;
}

void addLR1ItemList(LR1Item *item) {
    if (!LR1ItemList) {
        fprintf(stderr, "LR1 item list hasnt been initialized; use setLR1ItemList");
        exit(EXIT_FAILURE);
    }

    if (list_numLR1Items > list_maxLR1Items) {
        LR1ItemList = realloc(LR1ItemList, (list_maxLR1Items *= 2) * sizeof(LR1Item *));
        if (!LR1ItemList) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    LR1ItemList[list_numLR1Items++] = item;
}

LR1Item *findItemFromList(LR1Item *targetItem, bool (*cmpMethod)(LR1Item*, LR1Item*)) {
    for (int i = 0; i < list_numLR1Items; i++) {
        if (cmpMethod(LR1ItemList[i], targetItem)) return LR1ItemList[i];
    }
    return NULL;
}

LR1Item *findItemSet(ProductionRule *production, symbol lookahead) {
    LR1Item tempItem = {0, production, lookahead, 0, 0, NULL};
    return findItemInGotoSets(&tempItem);
}

bool ifItemTargetExists(LR1Item *item1, LR1Item *item2) {
    if (item1->lookahead != item2->lookahead) return false;
    if ((item1->production->numSymbols != item2->production->numSymbols)) return false;

    ExistenceArray *exArray = createExistenceArray(getNumProductionRuleSets(), noRevise);

    for (ProductionRule *prod = item1->production; prod; prod = prod->next) {
        exArray->array[prod->id] = true;
    }

    for (ProductionRule *prod = item2->production; prod; prod = prod->next) {
        if (!checkAndSetExistence(exArray, prod->id)) {
            freeExistenceArray(exArray);
            return false;
        }
    }

    return true;
}

LR1Item *findItemInGotoSets(LR1Item *expectedItem) {
    return findItemFromList(expectedItem, ifItemTargetExists);
}

symbol *extractLookaheadSymbols(ProductionRule *prod) {
    int existenceArrLength = abs(id_nonTerminal) + id_Terminal;
    bool symbolExistenceArray[existenceArrLength];
    for (int i = 0; i < existenceArrLength; i++) {
        symbolExistenceArray[i] = false;
    }

    int symArrayLength = 10;
    int numSymbols = 0;
    symbol *symbols = malloc(symArrayLength * sizeof(symbol));
    symbols[0] = END_SYMBOL_ARRAY;
    for (ProductionRule *rule = prod; rule; rule = rule->next) {
        if (rule->dotPos >= rule->numSymbols) continue;

        symbol readSym = getCurrentSymbol(rule);

        int revisedOffset = ReviseOffset(readSym);
        if (symbolExistenceArray[revisedOffset]) continue;
        symbolExistenceArray[revisedOffset] = true;

        if (numSymbols + 1 > symArrayLength) {
            symbols = realloc(symbols, (symArrayLength *= 2) * sizeof(symbol));
            if (!symbols) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        
        symbols[numSymbols++] = readSym;
    }
    symbols[numSymbols] = END_SYMBOL_ARRAY;
    
    return symbols;
}


void closure_(symbol targetSymbol, ExistenceArray symbolExistenceArray[], ExistenceArray ruleExistenceArray[]) {

    for (ProductionRule *curRule = &startProductionRule; curRule; curRule = curRule->next) {

        symbol left = curRule->nonTerminal;

        if (left != targetSymbol) continue;

        symbol firstRightSymbol = curRule->production[0];

        checkAndSetExistence(ruleExistenceArray, curRule->id);

        if (!checkAndSetExistence(
                symbolExistenceArray
                , firstRightSymbol))
                            closure_(
                                firstRightSymbol
                                , symbolExistenceArray
                                , ruleExistenceArray);        
    }
}

int reviseNonTerminal(int n) {
    return abs(n);
}

ProductionRule *closure(ProductionRule *targetProd) {
    ExistenceArray *symbolExistenceArray = createExistenceArray(abs(id_nonTerminal), reviseNonTerminal);
    ExistenceArray *ruleExistenceArray = createExistenceArray(getNumProductionRuleSets(), noRevise);
    ExistenceArray *ruleHasExistedArray = createExistenceArray(getNumProductionRuleSets(), noRevise);

    for (ProductionRule *prod = targetProd; prod; prod = prod->next) {
        symbol lookaheadSymbol = prod->production[prod->dotPos + 1];
        if (isTerminal(lookaheadSymbol)) continue;

        checkAndSetExistence(ruleHasExistedArray, prod->id);

        if (!checkAndSetExistence(
                symbolExistenceArray
                , lookaheadSymbol))
                            closure_(
                                lookaheadSymbol
                                , symbolExistenceArray
                                , ruleExistenceArray);
    }

    // remove prods has already been in targetProd (the arg of this function)
    eliminateOverlapsExstanceArray(ruleHasExistedArray, ruleExistenceArray);

    ProductionRule *start = NULL;
    ProductionRule **ppRule = &start;
    for (ProductionRule *curRule = &startProductionRule; curRule; curRule = curRule->next) {

        if (!ruleExistenceArray->array[curRule->id]) continue;

        *ppRule = cloneProductionRules(curRule);
        ppRule = &(*ppRule)->next;
    }

    return start;
}

void setStartRule(const ProductionRule *entryRule) {
    startProductionRule = *entryRule;
    setLR1ItemList();
}
