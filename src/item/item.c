
#include "item.h"

#define UNSET_ID (-1)

ProductionRule *prod_rules;
ProductionRule entryProduction = {UNSET_ID, 0, 0, NULL, 0, NULL};
Item *entryItems = NULL;

int numItems = 0;

Item *constructSubItem(Item *item);
void addInheritingItem(Item *item, Item *inheritingItem);
Item *findItem(ProductionRule *production, symbol targetSymbol);
Item *findItemInSubItems(Item *item, Item *expectedItem);
Item *traverseItems(Item *item, Item *expectedItem);
symbol *readSymbols(ProductionRule *production);
symbol consumeSymbol(ProductionRule *rule);
ProductionRule *filterRulesBySymbol(ProductionRule *rulesHasExisted, symbol expectedSymbol);
ProductionRule *cloneProduction(ProductionRule *prod);
ProductionRule *cloneProductionRule(ProductionRule *rule);
ProductionRule *combineProds(ProductionRule **prod1, ProductionRule **prod2);
bool isEndOfItem(Item *item);

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
    entryItem->production = &entryProduction;

    constructSubItem(entryItem);

    return entryItems;
}

#include <unistd.h>

Item *constructSubItem(Item *item) {
    if (isEndOfItem(item)) printf("End Of Item\n");
    if (isEndOfItem(item)) return NULL; 
    if (!entryItems) entryItems = item;

    printf("---------\n");
    printf("num of item: %d\n", numItems);
    printf("1\n");

    ProductionRule *clonedProd = cloneProduction(item->production);

    symbol *targetSymbols = readSymbols(clonedProd);
    
    printf("2\n");
    int i = 0;
    symbol sym = 0;

    while ((sym = targetSymbols[i++]) != END_SYMBOL_ARRAY) {
        printf("sym: %d\n", sym);
    }
    i = 0;
    while ((sym = targetSymbols[i++]) != END_SYMBOL_ARRAY) {
        printf("10\n");
        sleep(1);

        ProductionRule *extractedRules = filterRules(clonedProd, cmpCurSymbol);
        ProductionRule *gatheredRules = filterRules(entryProduction, cmpLeft);
        ProductionRule *eliminatedRules = eliminateProds(gatheredRules, extractedRules, cmpId);

        ProductionRule *allItemNeeded = combineProds(extractedRules, gatheredRules);

        printf("3\n");
        Item *foundItem = findItem(clonedProd, sym);
        if (foundItem) {
            addInheritingItem(item, foundItem);
            continue;
        }
        printf("4\n");
        Item *newItem = malloc(sizeof(Item));
        if (!newItem) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        printf("5\n");
        numItems++;
        newItem->targetSymbol = sym;
        newItem->production = clonedProd;
        newItem->numInheritingItems = 0;
        newItem->maxInheritingItems = 5;
        newItem->inheritingItems = calloc(newItem->maxInheritingItems, sizeof(Item *));
        if (!newItem->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        printf("6\n");
        constructSubItem(newItem);
        printf("7\n");
        addInheritingItem(item, newItem);
        printf("8\n");
    }
    
    free(targetSymbols);
    return NULL;
}

void addInheritingItem(Item *item, Item *inheritingItem) {
    printf("71\n");
    printf("%d %d\n", item->numInheritingItems, item->maxInheritingItems);
    if (item->numInheritingItems >= item->maxInheritingItems) {
        printf("72\n");
        item->inheritingItems = realloc(item->inheritingItems, (item->maxInheritingItems *= 2) * sizeof(Item *));
        printf("73\n");
        if (!item->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        printf("74\n");
        for (int j = item->numInheritingItems; j < item->maxInheritingItems; j++) {
            item->inheritingItems[j] = NULL;
        }
    }
    printf("75\n");
    item->inheritingItems[item->numInheritingItems++] = inheritingItem;
}

ProductionRule *combineProds(ProductionRule **prod1, ProductionRule **prod2) {
    ProductionRule *entry = *prod1;
    while ((*prod1)->next) {
        *prod1 = (*prod1)->next;
    }
    (*prod1)->next = *prod2;
    return entry;
}

bool isEndOfItem(Item *item) {
    ProductionRule *prod = item->production;
    return (item->numInheritingItems == 1
        && prod->numSymbols == prod->dotPos + 1);
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

#include <math.h>
#define ReviseOffset(n) (n + abs(number_nonTerminal))

// messy
symbol *readSymbols(ProductionRule *prod) {
    int existenceArrLength = abs(number_nonTerminal) + number_Terminal;
    bool symbolExistenceArray[existenceArrLength];
    for (int i = 0; i < existenceArrLength; i++) {
        symbolExistenceArray[i] = false;
    }

    int symArrayLength = 10;
    int numSymbols = 0;
    symbol *symbols = malloc(symArrayLength * sizeof(symbol));
    symbols[0] = END_SYMBOL_ARRAY;
    for  (ProductionRule *rule = prod; rule; rule = rule->next) {
        printf("rule id: %d target symbols: %d num of symbols: %d dot pos: %d\n", rule->id, rule->production[rule->dotPos], rule->numSymbols, rule->dotPos);
        if (rule->numSymbols == rule->dotPos) continue;

        symbol readSym = consumeSymbol(rule);

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

symbol consumeSymbol(ProductionRule *rule) {
    if (rule->dotPos > rule->numSymbols) {
        fprintf(stderr, "Out Of Production\n");
        exit(EXIT_FAILURE);
    }
    return rule->production[rule->dotPos++];
}

// compare speeds of this and nested for
ProductionRule *filterRulesBySymbol(ProductionRule *rulesHasExisted, symbol expectedSymbol) {
    bool overlapCheckArray[curLatestId];

    for (int i = 0; i < curLatestId; i++) {
        overlapCheckArray[i] = false;
    }

    for (ProductionRule *existedRule = rulesHasExisted; existedRule; existedRule = existedRule->next) {
        overlapCheckArray[existedRule->id] = true;
    }

    ProductionRule *startRule = NULL;
    ProductionRule **ppRule = &startRule;

    for (ProductionRule *rule = prod_rules; rule; rule = rule->next) {
        if (overlapCheckArray[rule->id]) continue;
        if (rule->nonTerminal == expectedSymbol) {
            ProductionRule *copy = cloneProductionRule(rule);
            copy->dotPos = 0;
            *ppRule = copy;
            ppRule = &((*ppRule)->next);
        }
    }
    return startRule;
}   

ProductionRule *cloneProduction(ProductionRule *prod) {
    ProductionRule *current = prod;
    ProductionRule *entry_clonedProd = cloneProductionRule(current);
    ProductionRule *copied_prod = entry_clonedProd;

    while (current->next) {
        current = current->next;
        copied_prod->next = cloneProductionRule(current);
        copied_prod = copied_prod->next;
    }

    return entry_clonedProd;
}


ProductionRule *cloneProductionRule(ProductionRule *rule) {
    ProductionRule *copy = malloc(sizeof(ProductionRule));
    if (!copy) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, rule, sizeof(ProductionRule));
    copy->next = NULL;
    return copy;
}

void setEntry(ProductionRule *entryRule) {
    entryProduction = *entryRule;
    entryProduction.next = NULL;
}
