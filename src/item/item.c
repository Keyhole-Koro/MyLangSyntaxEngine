#include "item.h"

ProductionRule entry_prod = {-1, 0, 0, NULL, 0, NULL};
Item *entry_items = NULL;

int cur_num_item = 0;

Item *constructItem_(Item *item);
void addInheritingItem(Item *item, Item *inheritingItem);
Item *findItem_(ProductionRule *production, symbol targetSymbol);
Item *traverseItems(Item *item, Item *expectedItem);
symbol *getTargetSymbol(ProductionRule *prod);
symbol readSymbol(ProductionRule *rule);
ProductionRule *gatherRulesMatchesWithLeft(symbol expectedSymbol);
ProductionRule *copyRule(ProductionRule *rule);

Item *constructItem() {
    // checks if entry prod has been set by referencing an element
    if (entry_prod.production == -1) {
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
        cur_num_item++;
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
    return findItem_(entry_items, &item);
}

Item *findItem_(Item *item, Item *expectedItem) {
    Item *stack[cur_num_item];
    int top = -1;

    stack[++top] = item;

    while (top >= 0) {
        item = stack[top--];

        if (item->targetSymbol != expectedItem->targetSymbol) continue;

        bool found = true;
        ProductionRule *expectedRule = expectedItem->production;
        while (expectedRule != NULL) {
            found = false;
            ProductionRule *rule = item->production;
            while (rule != NULL) {
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

        for (int i = 0; i < item->offset_inheritingItems; i++) {
            stack[++top] = item->inheritingItems[i];
        }
    }

    return NULL;
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
    return rule->production[rule->dot_pos++];
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
    copy->dot_pos = 0;
    copy->next = NULL;
    return copy;
}

void setEntry(ProductionRule *entryRule) {
    entry_prod = *entryRule;
    entry_prod.next = NULL; // in case, not to use
}
