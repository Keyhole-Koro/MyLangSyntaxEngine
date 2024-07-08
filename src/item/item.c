
#include "item.h"

#define UNSET_ID (-1)

ProductionRule *prod_rules;
ProductionRule entryProduction = {UNSET_ID, 0, 0, NULL, 0, NULL};
Item *entryItems = NULL;

int numItems = 0;

Item *constructSubItem(Item *item);

void addInheritingItem(Item *item, Item *inheritingItem);

Item *lookForItem(ProductionRule *production, symbol targetSymbol);
Item *lookForItemInSubItems(Item *item, Item *expectedItem);

symbol *extractTargetSymbols(ProductionRule *production);
symbol consumeSymbol(ProductionRule *rule);
void readCurSymbols(ProductionRule *prod);

ProductionRule *cloneProduction(ProductionRule *prod);
ProductionRule *cloneProductionRule(ProductionRule *rule);

ProductionRule *filterRules(ProductionRule *sourceProd, int (*referMethod)(ProductionRule*), int expectedValue);
ProductionRule *combineProds(ProductionRule *prod1, ProductionRule *prod2);
void eliminateOverlapRules(ProductionRule **eliminatedProd, ProductionRule *referredProd);

bool isTerminal(symbol sym);

bool isEndOfItem(Item *item);

int referCurSymbol(ProductionRule *prod);
int referLeft(ProductionRule *prod);

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
    entryItem->targetSymbol = 0;
    entryItem->numInheritingItems = 0;
    entryItem->maxInheritingItems = 5;

    constructSubItem(entryItem);

    return entryItems;
}

#include <unistd.h>

Item *constructSubItem(Item *item) {
    if (isEndOfItem(item)) printf("End Of Item\n");
    if (isEndOfItem(item)) return NULL; 
    if (!entryItems) entryItems = item;

    printf("---------\n");
    printf("the number of item: %d\n", numItems);

    ProductionRule *clonedProd = cloneProduction(item->production);
    
    // extract current symbols from clonedProd, eliminating overlapping symbols
    symbol *targetSymbols = extractTargetSymbols(clonedProd);

    int i = 0;
    symbol sym = 0;

    while ((sym = targetSymbols[i++]) != END_SYMBOL_ARRAY) {
        sleep(1);

        printf(">next target symbol: %s\n", exchangeSymbol(sym));

        // filters rules by a target symbol of targetSymbols
        ProductionRule *filteredRules = filterRules(clonedProd, referCurSymbol, sym);
        ProductionRule *gatheredRules = NULL;

        if (!isTerminal(sym)) /*skip filtering since no rules have terminal on the left side*/
            gatheredRules = filterRules(&entryProduction, referLeft, sym);
        //eliminateOverlapRules(&gatheredRules, filteredRules);

        if (!filteredRules && !gatheredRules) continue;

        // if prods from each args, filteredRules will be prioritized
        ProductionRule *allItemNeeded = combineProds(gatheredRules, filteredRules);

        // look for item to avoid making existing itemsh
        // the second arg, sym is used to avoid excess comparing
        Item *foundItem = lookForItem(allItemNeeded, sym);
        if (foundItem) {
            addInheritingItem(item, foundItem);
            continue;
        }

        Item *newItem = calloc(1, sizeof(Item));
        if (!newItem) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        numItems++;
        newItem->production = allItemNeeded;
        newItem->targetSymbol = sym;
        newItem->numInheritingItems = 0;
        newItem->maxInheritingItems = 6;/*just ramdom*/
        newItem->inheritingItems = calloc(newItem->maxInheritingItems, sizeof(Item *));
        if (!newItem->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        readCurSymbols(allItemNeeded);

        addInheritingItem(item, newItem);

        constructSubItem(newItem);
        
    }
    
    free(targetSymbols);
    return NULL;
}

void addInheritingItem(Item *item, Item *inheritingItem) {
    if (item->numInheritingItems > item->maxInheritingItems) {
        item->inheritingItems = realloc(item->inheritingItems, (item->maxInheritingItems *= 2) * sizeof(Item *));
        if (!item->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    item->inheritingItems[item->numInheritingItems++] = inheritingItem;
}

bool isTerminal(symbol sym) {
    return sym > 0;
}

void eliminateOverlapRules(ProductionRule **eliminatedProd, ProductionRule *referredProd) {
    if (!eliminatedProd || !referredProd) return;

    bool overlapCheckArray[latestProdId];
    for (int i = 0; i < latestProdId; i++) {
        overlapCheckArray[i] = false;
    }

    while (referredProd->next) {
        overlapCheckArray[referredProd->id] = true;
        referredProd = referredProd->next;
    }

    ProductionRule *head = *eliminatedProd;

    ProductionRule *cur = *eliminatedProd;
    ProductionRule *prev = *eliminatedProd;
    while (cur->next) {
        if (overlapCheckArray[cur->id]) {
            if (cur == head) {
                head = head->next;
                *eliminatedProd = head;
            } else {
                prev->next = cur->next;
            }
            free(cur);
            cur = prev->next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

ProductionRule *combineProds(ProductionRule *prod1, ProductionRule *prod2) {
    ProductionRule *copy_prod1 = cloneProduction(prod1);
    ProductionRule *copy_prod2 = cloneProduction(prod2);

    if (!copy_prod1 && !copy_prod2) return NULL;
    if (!copy_prod1) return copy_prod2;
    if (!copy_prod2) return copy_prod1;

    ProductionRule *entry = copy_prod1;
    while (copy_prod1) {
        copy_prod1 = copy_prod1->next;
    }

    if (copy_prod1) copy_prod1->next = copy_prod2;

    return entry;
}

bool isEndOfItem(Item *item) {
    ProductionRule *prod = item->production;
    return (item->numInheritingItems == 1
        && prod->numSymbols == prod->dotPos);
}

Item *lookForItem(ProductionRule *production, symbol targetSymbol) {
    Item item = {production, targetSymbol, 0, 0, NULL};
    return lookForItemInSubItems(entryItems, &item);
}

Item *lookForItemInSubItems(Item *item, Item *expectedItem) {
    Item *stack[numItems];
    int top = -1;

    stack[++top] = item;

    while (top >= 0) {
        //sleep(1);
        printf("top %d %d\n", top, item->numInheritingItems);
        item = stack[top--];

        
        printf("cmp target symbol %s, %s\n", exchangeSymbol(item->targetSymbol), exchangeSymbol(expectedItem->targetSymbol));
        if (item->targetSymbol != expectedItem->targetSymbol) continue;

        bool found = true;
        ProductionRule *expectedRule = expectedItem->production;

        while (expectedRule) {
            //sleep(1);
            found = false;
            ProductionRule *rule = item->production;
            while (rule) {
                printf("cmp id %d, %d\n", rule->id, expectedRule->id);
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
    }

    for (int i = 0; i < item->numInheritingItems; i++) {
        stack[++top] = item->inheritingItems[i];
    }

    return NULL;
}

#include <math.h>
#define ReviseOffset(n) (n + abs(number_nonTerminal))

// messy
symbol *extractTargetSymbols(ProductionRule *prod) {
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
        printf("rule id: %d, target symbols: '%s', Num of Symbols: %d, dot pos: %d\n", rule->id, exchangeSymbol(rule->production[rule->dotPos]), rule->numSymbols, rule->dotPos);
        if (rule->dotPos >= rule->numSymbols) continue;

        symbol readSym = rule->production[rule->dotPos];

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

void readCurSymbols(ProductionRule *prod) {
    for (ProductionRule *rule = prod; rule; rule = rule->next) {
        if (rule->dotPos < rule->numSymbols)
        rule->dotPos++;
    }
}

symbol consumeSymbol(ProductionRule *rule) {
    if (rule->dotPos > rule->numSymbols) {
        fprintf(stderr, "Out Of Production\n");
        exit(EXIT_FAILURE);
    }
    return rule->production[rule->dotPos++];
}

int referCurSymbol(ProductionRule *prod) {
    return prod->production[prod->dotPos];
}

int referLeft(ProductionRule *prod) {
    return prod->nonTerminal;
}

// compare speeds of this and nested for
ProductionRule *filterRules(ProductionRule *sourceProd, int (*referMethod)(ProductionRule*), int expectedValue) {
    bool overlapCheckArray[latestProdId];
    for (int i = 0; i < latestProdId; i++) {
        overlapCheckArray[i] = false;
    }

    ProductionRule *startRule = NULL;
    ProductionRule **ppRule = &startRule;

    for (ProductionRule *rule = sourceProd; rule; rule = rule->next) {
        if (rule->dotPos >= rule->numSymbols) continue;
        if (referMethod(rule) != expectedValue) continue;
        if (overlapCheckArray[rule->id]) continue;
        overlapCheckArray[rule->id] = true;
        ProductionRule *copy = cloneProductionRule(rule);
        *ppRule = copy;
        ppRule = &((*ppRule)->next);
    }
    return startRule;
}

ProductionRule *cloneProduction(ProductionRule *prod) {
    if (!prod) return prod;

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
}
