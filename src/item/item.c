#include "item.h"

ProductionRule entry_prod = {0, NULL, 0, NULL};
Item *entry_items = NULL;

Item *constructItem_(Item *item);
void addInheritingItem(Item *item, Item *inheritingItem);
Item *findItem(ProductionRule *production, symbol targetSymbol);
Item *traverseItems(Item *item, Item *expectedItem);
symbol *getTargetSymbol(ProductionRule *prod);
symbol readSymbol(ProductionRule *rule);
ProductionRule *gatherRulesMatchesWithLeft(symbol expectedSymbol);
ProductionRule *copyRule(ProductionRule *rule);

Item *constructItem() {
    // checks if entry prod has been set by referencing an element
    if (entry_prod.production == NULL) {
        fprintf(stderr, "The entry rule has not been set.\n");
        fprintf(stderr, "Set the entry by using setEntry(ProductionRule*) before running constructItem\n");
        exit(EXIT_FAILURE);
    }

    Item *entryItem = malloc(sizeof(Item));
    if (!entryItem) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    entryItem->production = copyRule(&entry_prod);

    constructItem_(entryItem);

    return entry_items;
}

Item *constructItem_(Item *item) {
    if (!entry_items) entry_items = item;

    symbol *extractedSymbols = getTargetSymbol(item->production);

    int i = 0;
    symbol sym = 0; // initialization
    while ((sym = extractedSymbols[i]) != END_SYMBOL_ARRAY) {
        ProductionRule *gatheredRules = gatherRulesMatchesWithLeft(sym);

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
        newItem->targetSymbol = sym;
        newItem->production = gatheredRules;
        newItem->len_inheritingItems = 5;
        newItem->inheritingItems = calloc(newItem->len_inheritingItems, sizeof(Item *));
        if (!newItem->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        constructItem_(newItem);
        addInheritingItem(item, newItem);

        i++;
    }
    
    free(extractedSymbols);
    return NULL;
}

void addInheritingItem(Item *item, Item *inheritingItem) {
    if (item->offset_inheritingItems >= item->len_inheritingItems) {
        item->len_inheritingItems *= 2;
        item->inheritingItems = realloc(item->inheritingItems, item->len_inheritingItems * sizeof(Item *));
        if (!item->inheritingItems) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        for (int j = item->offset_inheritingItems; j < item->len_inheritingItems; j++) {
            item->inheritingItems[j] = NULL;
        }
    }
    item->inheritingItems[item->offset_inheritingItems++] = inheritingItem;
}

Item *findItem(ProductionRule *production, symbol targetSymbol) {
    Item item = {production, targetSymbol, 0, 0, NULL, NULL};
    return traverseItems(entry_items, &item);
}

Item *traverseItems(Item *item, Item *expectedItem) {
    if (item == NULL) return NULL;

    if (item->targetSymbol != expectedItem->targetSymbol) return traverseItems(item->next, expectedItem);

    ProductionRule *expectedRule = expectedItem->production;
    ProductionRule *rule = item->production;
    while (expectedRule != NULL && rule != NULL) {
        if (rule->nonTerminal != expectedRule->nonTerminal || rule->read_pos != expectedRule->read_pos) {
            return traverseItems(item->next, expectedItem);
        }
        symbol expectedSym = 0;
        symbol sym = 0;
        int i = 0;
        bool same = true; // temporary
        while (((sym = rule->production[i]) != END_SYMBOL_ARRAY
                && expectedSym == expectedRule->production[i]) != END_SYMBOL_ARRAY) {
            if (sym != expectedSym) {
                same = false;
                break;
            }
            i++;
        }
        if (same) return item;

        expectedRule = expectedRule->next;
        rule = rule->next;
    }

    if (expectedRule == NULL && rule == NULL) return item;
    else return traverseItems(item->next, expectedItem);
}

symbol *getTargetSymbol(ProductionRule *prod) {
    int len_existenceArr = 50;
    int reviser = len_existenceArr / 2;
    bool symbolExistenceArray[len_existenceArr];
    for (int i = 0; i < len_existenceArr; i++) {
        symbolExistenceArray[i] = false;
    }

    int len_syms = 10;
    int num_syms = 0;
    symbol *syms = malloc(len_syms * sizeof(symbol));
    syms[0] = END_SYMBOL_ARRAY;
    for (ProductionRule *rule = prod; rule; rule = rule->next) {
        symbol readSym = readSymbol(rule);
        int revisedOffset = reviser + readSym;
        if (revisedOffset >= 0 && revisedOffset < len_existenceArr) {
            if (symbolExistenceArray[revisedOffset]) continue;
            symbolExistenceArray[revisedOffset] = true;
        } else {
            bool found = false;
            for (int j = 0; j < num_syms; j++) {
                if (syms[j] == readSym) {
                    found = true;
                    break;
                }
            }
            if (found) continue;
        }

        if (num_syms + 1 > len_syms) {
            len_syms *= 2;
            syms = realloc(syms, len_syms * sizeof(symbol));
            if (!syms) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        
        syms[num_syms++] = readSym;
    }
    syms[num_syms] = END_SYMBOL_ARRAY;
    
    return syms;
}

symbol readSymbol(ProductionRule *rule) {
    return rule->production[rule->read_pos++];
}

ProductionRule *gatherRulesMatchesWithLeft(symbol expectedSymbol) {
    ProductionRule *start_rule = NULL;
    ProductionRule **ppRule = &start_rule;

    for (ProductionRule *rule = prod_rules; rule; rule = rule->next) {
        if (rule->nonTerminal == expectedSymbol) {
            *ppRule = copyRule(rule);
            ppRule = &((*ppRule)->next);
        }
    }
    return start_rule;
}   

ProductionRule *copyRule(ProductionRule *rule) {
    ProductionRule *copy = malloc(sizeof(ProductionRule));
    if (!copy) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, rule, sizeof(ProductionRule));
    copy->read_pos = 0;
    copy->next = NULL;
    return copy;
}

void setEntry(ProductionRule *entryRule) {
    entry_prod = *entryRule;
    entry_prod.next = NULL; // in case, not to use
}
