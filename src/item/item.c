
#include "item.h"

#define UNSET_ID (-1)

ProductionRule entryProduction = {UNSET_ID, 0, 0, NULL, 0, NULL};
Item *entryItems = NULL;

int numItems = 0;

Item *constructSubItem(Item *item);
void addInheritingItem(Item *item, Item *inheritingItem);
Item *findItem(ProductionRule *production, symbol targetSymbol);
Item *findItemInSubItems(Item *item, Item *expectedItem);
Item *traverseItems(Item *item, Item *expectedItem);
symbol *extractTargetSymbols(ProductionRule *production);
symbol readNextSymbol(ProductionRule *rule);
ProductionRule *filterRulesBySymbol(symbol expectedSymbol);
ProductionRule *copyProductionRule(ProductionRule *rule);

Item *constructItem() {
    if (entryProduction.id == UNSET_ID) {
        fprintf(stderr, "The entry rule has not been set.\n");
        fprintf(stderr, "Set the entry by using setEntry(ProductionRule*) before running constructItem\n");
        exit(EXIT_FAILURE);
    }

    Item *entryItem = malloc(sizeof(Item));
    if (!entryItem) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    entryItem->production = copyProductionRule(&entryProduction);

    constructSubItem(entryItem);

    return entryItems;
}

Item *constructSubItem(Item *item) {
    if (!entryItems) entryItems = item;

    symbol *targetSymbols = extractTargetSymbols(item->production);

    int i = 0;
    symbol sym = 0;
    while ((sym = targetSymbols[i]) != END_SYMBOL_ARRAY) {
        ProductionRule *gatheredRules = filterRulesBySymbol(sym);

        Item *foundItem = findItem(gatheredRules, sym);
        if (foundItem) {
            addInheritingItem(item, foundItem);
            continue;
        }

        Item *newItem = malloc(sizeof(Item));
        if (!newItem) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        numItems++;
        newItem->targetSymbol = sym;
        newItem->production = gatheredRules;
        newItem->maxInheritingItems = 5;
        newItem->inheritingItems = calloc(newItem->maxInheritingItems, sizeof(Item *));
        if (!newItem->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        constructSubItem(newItem);
        addInheritingItem(item, newItem);

        i++;
    }
    
    free(targetSymbols);
    return NULL;
}

void addInheritingItem(Item *item, Item *inheritingItem) {
    if (item->numInheritingItems >= item->maxInheritingItems) {
        item->maxInheritingItems *= 2;
        item->inheritingItems = realloc(item->inheritingItems, item->maxInheritingItems * sizeof(Item *));
        if (!item->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        for (int j = item->numInheritingItems; j < item->maxInheritingItems; j++) {
            item->inheritingItems[j] = NULL;
        }
    }
    item->inheritingItems[item->numInheritingItems++] = inheritingItem;
}

Item *findItem(ProductionRule *production, symbol targetSymbol) {
    Item item = {production, targetSymbol, 0, 0, NULL};
    return findItemInSubItems(entryItems, &item);
}

Item *findItemInSubItems(Item *item, Item *expectedItem) {
    Item *stack[numItems];
    int top = -1;

    stack[++top] = item;

    while (top >= 0) {
        item = stack[top--];

        if (item->targetSymbol != expectedItem->targetSymbol) continue;

        bool found = true;
        ProductionRule *expectedRule = expectedItem->production;

        while (expectedRule) {
            found = false;
            ProductionRule *rule = item->production;
            while (rule) {
                if (rule->id == expectedRule->id) {
                    found = true;
                    break;
                }
                rule = rule->next;
            }
            if (!found) break;
            expectedRule = expectedRule->next;
        }

        if (found) return item;

        for (int i = 0; i < item->numInheritingItems; i++) {
            stack[++top] = item->inheritingItems[i];
        }
    }

    return NULL;
}

symbol *extractTargetSymbols(ProductionRule *prod) {
    int existenceArrLength = 50;
    int reviser = existenceArrLength / 2;
    bool symbolExistenceArray[existenceArrLength];
    for (int i = 0; i < existenceArrLength; i++) {
        symbolExistenceArray[i] = false;
    }

    int symArrayLength = 10;
    int numSymbols = 0;
    symbol *symbols = malloc(symArrayLength * sizeof(symbol));
    symbols[0] = END_SYMBOL_ARRAY;
    for  (ProductionRule *rule = prod; rule; rule = rule->next) {
        symbol readSym = readNextSymbol(rule);
        int revisedOffset = reviser + readSym;
        if (revisedOffset >= 0 && revisedOffset < existenceArrLength) {
            if (symbolExistenceArray[revisedOffset]) continue;
            symbolExistenceArray[revisedOffset] = true;
        } else {
            bool found = false;
            for (int j = 0; j < numSymbols; j++) {
                if (symbols[j] == readSym) {
                    found = true;
                    break;
                }
            }
            if (found) continue;
        }

        if (numSymbols + 1 > symArrayLength) {
            symArrayLength *= 2;
            symbols = realloc(symbols, symArrayLength * sizeof(symbol));
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

symbol readNextSymbol(ProductionRule *rule) {
    return rule->production[rule->dot_pos++];
}

ProductionRule *filterRulesBySymbol(symbol expectedSymbol) {
    ProductionRule *startRule = NULL;
    ProductionRule **ppRule = &startRule;

    for (ProductionRule *rule = prod_rules; rule; rule = rule->next) {
        if (rule->nonTerminal == expectedSymbol) {
            *ppRule = copyProductionRule(rule);
            ppRule = &((*ppRule)->next);
        }
    }
    return startRule;
}   

ProductionRule *copyProductionRule(ProductionRule *rule) {
    ProductionRule *copy = malloc(sizeof(ProductionRule));
    if (!copy) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, rule, sizeof(ProductionRule));
    copy->dot_pos = 0;
    copy->next = NULL;
    return copy;
}

void setEntry(ProductionRule *entryRule) {
    entryProduction = *entryRule;
    entryProduction.next = NULL;
}
