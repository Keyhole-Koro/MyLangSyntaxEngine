#include "follow.h"
#include <stdlib.h>

void setFollowSets(ExistenceArray *isLhsDone, symbol targetSymbol);

void addSet(ExistenceArray *isLhsDone, symbol aheadSym, symbol curSym, symbol lhs);

void addFirstSetToFollowSet(symbol aheadSym, symbol whereToAdd);

void addSetToAnother(symbol from, symbol to);

bool **firstSetsTable = NULL;
bool **followSetsTable = NULL;

ProductionRule *entryRule = NULL;

// when an ahead symbol is null

bool **follow(ProductionRule *_entryRule, bool **_firstSetsTable) {
    int numNonTerminal = table_max_non_terminal + 1;
    int numTerminal = table_max_terminal + 1;

    followSetsTable = (bool **)malloc(numNonTerminal * sizeof(bool *));
    for (int i = 0; i < numNonTerminal; ++i) {
        followSetsTable[i] = (bool *)malloc(numTerminal * sizeof(bool));
        for (int j = 0; j < numTerminal; ++j) {
            followSetsTable[i][j] = false;
        }
    }

    firstSetsTable = _firstSetsTable;
    entryRule = _entryRule;

    ExistenceArray *isLhsDone = createExistenceArray(numNonTerminal, reviseNonTerminal);

    setFollowSets(isLhsDone, entryRule->lhs);
    
    for (int i = 1; i < numNonTerminal; i++) {
        printf("%s: {", exchangeSymbol(-1 * i));
        for (int j = 1; j < numTerminal; j++) {
            if (!followSetsTable[i][j]) continue;
            printf("%s ", exchangeSymbol(j));
        }
        printf("}\n");
    }

    return followSetsTable;
}

void setFollowSets(ExistenceArray *isLhsDone, symbol targetSymbol) {
    if (checkAndSetExistence(isLhsDone, targetSymbol)) return;

    for (ProductionRule *rule = entryRule; rule; rule = rule->next) {
        symbol lhs = rule->lhs;
        if (lhs != targetSymbol) continue;

        for (int i = 0; i < rule->numSymbols; i++) {
            symbol curSym = rule->rhs[i];
            if (isTerminal(curSym)) continue;
            addSet(isLhsDone
                , (i + 1 == rule->numSymbols) ? 0 : rule->rhs[i + 1]
                , curSym
                , lhs);
        }
    }
}

void addSet(ExistenceArray *isLhsDone, symbol aheadSym, symbol curSym, symbol lhs) {
    if (isTerminal(curSym)) {
        followSetsTable[abs(curSym)][aheadSym] = true;
    } else if (isNonTerminal(curSym)) {
        addFirstSetToFollowSet(curSym, aheadSym);
        setFollowSets(isLhsDone, curSym);
    } else {
        addSetToAnother(aheadSym, lhs);
    }
}

void addFirstSetToFollowSet(symbol aheadSym, symbol whereToAdd) {
    for (int i = 0; i < table_max_terminal; i++) {
        if (!firstSetsTable[abs(aheadSym)][i]) continue;

        followSetsTable[abs(whereToAdd)][i] = true;
    }
}

void addSetToAnother(symbol from, symbol to) {
    for (int i = 0; i < table_max_terminal; i++) {
        if (followSetsTable[from][i]) followSetsTable[to][i] = true;
    }
}
