#include "item.h"

ProductionRule *prod_rules; // introduct from syntax.h

ProductionRule entry_prod = NULL;
ProductionRule entry_items = NULL;

Item *constructItem() {
    if (!entry_prod) {
        printf("The entry of rule hasnt been set.\n
            Set the entry by using setEntry(ProductionRule*) before you run constructItem");
        exit(EXIT_FAILURE);
    }

    Item *entryItem = malloc(sizeof(Item));
    entryItem->production = copyRule(&entry_prod);

    constructItem_(entryItem);
}

Item *constructItem_(Item *item) {
    if (!entry_items) entry_items = item;

    symbol *extractedSymbols = getReadSymbol(item->production);

    int i = 0;
    symbol sym = 0; // initialization
    while ((sym = extractedSymbols[i]) != END_SYMBOL_ARRAY) {
        ProductionRule *gatheredRules = gatherRulesMatchesWithLeft(sym);

        Item *foundItem = findItem(gatheredRules, sym);
        if (foundItem) {
            item->inheritingItems = foundItem;
            continue;
        }

        Item *newItem = malloc(sizeof(Item));
        if (newItem == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        newItem->readSymbol = sym;
        newItem->production = gatheredRules;
        newItem->inheritingItems = constructItem_(newItem);

        i++;
    }
    
    free(extractedSymbols);
    return NULL;
}

Item *findItem(symbol *production, symbol readSymbol) {
    Item item = {production, readSymbol, NULL};
    return traverseItems(&entry_items, &item);
}

// still odd
Item *traverseItems(Item *item, Item *expectedItem) {

    if (item == NULL) return NULL;

    if (item->readSymbol != expectedItem->readSymbol) return traverseItems(item->anotherInherits, expectedItem);

    ProductionRule *expectedRule = expectedItem->production;
    ProductionRule *rule = item->production;
    while (expectedRule != NULL && rule != NULL) {
        if (rule->nonTerminal != expectedRule->nonTerminal || rule->read_pos != expectedRule->read_pos) {
            return traverseItems(item->anotherInherits, expectedItem);
        }
        symbol expectedSym;
        symbol sym;
        int = 0;
        bool same = true; // temporary
        while (((sym = rule->production[i]) != END_SYMBOL_ARRAY
                && expectedSym = expectedRule->production[i]) != END_SYMBOL_ARRAY) {
            if (sym != expectedSym) {
                same = false;
                break;
            }
            i++;
        }
        if (same) return item;

        expectedRule = expectedRule->anotherInherits;
        rule = rule->anotherInherits;
    }

    if (expectedRule == NULL && rule == NULL) return item;
    else return traverseItems(item->anotherInherits, expectedItem);
}

symbol *getReadSymbol(ProductionRule *prod) {
    /** 
        @brief Algorithm improvement:
        The symbolExistanceArray is used to check whether an element already exists.
        True indicates existence, while false indicates absence.
        In this case, the symbol is an integer.
        It offers a time complexity of O(1).
        Otherwise, in the worst-case scenario, the nested loop results in a time complexity of O(x^2).

        @brief reviser
        since symbol can has minus indicates non_terminal, by using revisor,
        revise the number(symbol) by adding the half of (length of the array),
        for example, symbol = -25 -> revised symbol = 0
    */
    int len_existanceArr = 50;
    int reviser = len_existanceArr / 2;
    bool symbolExistanceArray[len_existanceArr];
    for (int i = 0; i < len_existanceArr; i++) {
        symbolExistanceArray[i] = false;
    }

    int len_syms = 10;
    int num_syms = 0;
    symbol *syms = malloc(len_syms * sizeof(symbol));
    syms[0] = END_SYMBOL_ARRAY;
    for (ProductionRule *rule = prod; rule; rule = rule->anotherInherits) {
        symbol readSym = readSymbol(rule);
        // if overlap array can handle the symbol
        int revisedOffset = reviser + readSym
        if (revisedOffset >= 0 && revisedOffset < len_existanceArr) {
            if (symbolExistanceArray[revisedOffset]) continue;
            symbolExistanceArray[revisedOffset] = true;
        } else {
            bool found = false;
            for (int j = 0; i < num_syms; i++) {
                if (syms[i] == readSym) {
                    found = true;
                    break;
                }
            }
            if (found) continue;
        }

        // +1 to reserve a space for END_SYMBOL_ARRAY
        if (num_syms + 1 > len_syms) {
            len_syms *= 2;
            syms = realloc(syms, len_syms * sizeof(symbol));
        }
        
        syms[num_syms++]
    }
    syms[num_syms] = END_SYMBOL_ARRAY;
    
    return syms;
}

/** @brief get the symbol and !!! increment read_pos !!! */
symbol readSymbol(ProductionRule *rule) {
    return rule->production[rule->read_pos++];
}

// send a copy
ProductionRule *gatherRulesMatchesWithLeft(symbol expectedSymbol) {
    ProductionRule *start_rule = NULL;
    ProductionRule **ppRule = &start_rule;

    for (ProductionRule *rule = prod_rules; rule; rule = rule->anotherInherits) {
        if (rule->nonTerminal == expectedSymbol) {
            *ppRule = copyRule(rule);
            ppRule = &((*ppRule)->anotherInherits);
        }
    }
    return start_rule;
}   

ProductionRule *copyRule(ProductionRule *rule) {
    ProductionRule *copy = malloc(sizeof(ProductionRule));
    memcpy(copy, rule, sizeof(ProductionRule));
    /** 
        basically the original producitons mustn't be changed
        (which means read_pos shouldn't be changed)
        but just in case
    */
    copy->read_pos = 0;
    copy->anotherInherits = NULL;
    return copy;
}

void setEntry(ProductionRule *entryRule) {
    entry_prod = *entryRule;
    entry_prod->anotherInherits = NULL; // in case, not to use
}