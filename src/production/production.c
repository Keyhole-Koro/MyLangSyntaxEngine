#include "production.h"

int getNumProductionRuleSets();// defined in syntax.c

void advanceDot(ProductionRule *prod) {
    for (ProductionRule *rule = prod; rule; rule = rule->next) {
        if (rule->dotPos < rule->numSymbols) rule->dotPos++;
    }
}

symbol consumeSymbol(ProductionRule *rule) {
    if (rule->dotPos > rule->numSymbols) {
        DEBUG_PRINT("Out Of Production\n");
        exit(EXIT_FAILURE);
    }
    return rule->rhs[rule->dotPos++];
}

int getCurrentSymbol(ProductionRule *prod) {
    return prod->rhs[prod->dotPos];
}

int getLeftNonTerminal(ProductionRule *prod) {
    return prod->lhs;
}

ProductionRule *filterProductions(ProductionRule *sourceProd, int (*condition)(ProductionRule*), int expectedValue) {
    bool overlapCheckArray[getNumProductionRuleSets()];
    for (int i = 0; i < getNumProductionRuleSets(); i++) {
        overlapCheckArray[i] = false;
    }

    ProductionRule *startRule = NULL;
    ProductionRule **ppRule = &startRule;

    for (ProductionRule *rule = sourceProd; rule; rule = rule->next) {
        if (rule->dotPos > rule->numSymbols) continue;
        if (condition(rule) != expectedValue) continue;
        if (overlapCheckArray[rule->id]) continue;
        overlapCheckArray[rule->id] = true;

        ProductionRule *copy = cloneProductionRules(rule);
        *ppRule = copy;
        ppRule = &((*ppRule)->next);
    }
    return startRule;
}

ProductionRule *cloneProduction(ProductionRule *prod) {
    if (!prod) return prod;

    ProductionRule *current = prod;
    ProductionRule *entryClonedProd = cloneProductionRules(current);
    ProductionRule *copiedProd = entryClonedProd;

    while (current->next) {
        current = current->next;
        copiedProd->next = cloneProductionRules(current);
        copiedProd = copiedProd->next;
    }

    return entryClonedProd;
}

ProductionRule *cloneProductionRules(ProductionRule *rule) {
    ProductionRule *copy = malloc(sizeof(ProductionRule));
    if (!copy) {
        DEBUG_PRINT("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, rule, sizeof(ProductionRule));
    copy->next = NULL;
    return copy;
}

bool compareProductionRules(ProductionRule *rule1, ProductionRule *rule2) {
    bool found = false;
    while (rule1) {
        found = false;
        while (rule2) {
            if (rule1->id == rule2->id && rule1->dotPos == rule2->dotPos + 1) {
                found = true;
                break;
            }
            rule2 = rule2->next;
        }
        if (!found) break;
        rule1 = rule1->next;
    }
    if (found) printf("Found\n");
    return found;
}

ProductionRule *combineProductions(ProductionRule *prod1, ProductionRule *prod2) {
    ProductionRule *copyProd1 = cloneProduction(prod1);
    ProductionRule *copyProd2 = cloneProduction(prod2);

    if (!copyProd1 && !copyProd2) return NULL;
    if (!copyProd1) return copyProd2;
    if (!copyProd2) return copyProd1;

    ProductionRule *entry = copyProd1;
    while (copyProd1->next) {
        copyProd1 = copyProd1->next;
    }
    copyProd1->next = copyProd2;

    return entry;
}