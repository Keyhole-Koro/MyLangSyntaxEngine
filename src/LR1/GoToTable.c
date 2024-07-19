#include "GoToTable.h"

int len_column = 0;
int len_row = 0;
const int CELL_WIDTH = 3;

GoTo **gotoTable = NULL;
bool **_firstSetsTable = NULL;
bool **_followSetsTable = NULL;

GoToHead *table_head = NULL;
GoToSide *table_side = NULL;

void traverseLR1Item(LR1Item *item);
void setShift_Goto(LR1Item *item);
void setReduce(LR1Item *item);
void setAcc(LR1Item *item);
int getGoToColumn(symbol target);
void setGoToHead();
void setGotoSide();
void printCell(const char *str, int width);

GoTo **GotoTable(bool **first, bool **follow, LR1Item *entryItem) {
    _firstSetsTable = first;
    _followSetsTable = follow;

    len_column = getNumNonTerminal() + getNumTerminal() + 1; // +1 includes terminate symbol
    len_row = getNumItemSets() + 1; // +1 includes state 0
    
    setGoToHead();
    setGotoSide();

    gotoTable = (GoTo**)malloc(len_row * sizeof(GoTo*));
    for (int i = 0; i < len_row; ++i) {
        gotoTable[i] = (GoTo*)calloc(len_column, sizeof(GoTo));
    }

    traverseLR1Item(entryItem);

    for (int i = 0; i < CELL_WIDTH; i++) {
        printf(" ");
    }
    printf("|");
    for (int i = 0; i < len_column; ++i) {
        char symbolStr[CELL_WIDTH];
        if (table_head[i].kind == _TERMINATE) snprintf(symbolStr, CELL_WIDTH, "$");
        else snprintf(symbolStr, CELL_WIDTH, "%s", exchangeSymbol(table_head[i].sym));
        printCell(symbolStr, CELL_WIDTH);
    }
    printf("\n");

    for (int i = 0; i < len_row; ++i) {
        char stateStr[CELL_WIDTH];
        snprintf(stateStr, CELL_WIDTH, "%i", i);
        printCell(stateStr, CELL_WIDTH);
        for (int j = 0; j < len_column; ++j) {
            char actionStr[CELL_WIDTH];
            switch (gotoTable[i][j].act) {
            case SHIFT:
                snprintf(actionStr, CELL_WIDTH, "S%d", gotoTable[i][j].state);
                break;
            case REDUCE:
                snprintf(actionStr, CELL_WIDTH, "R%d", gotoTable[i][j].state);
                break;
            case ACCEPT:
                snprintf(actionStr, CELL_WIDTH, "acc");
                break;
            case GOTO:
                snprintf(actionStr, CELL_WIDTH, "%d", gotoTable[i][j].state);
                break;
            default:
                snprintf(actionStr, CELL_WIDTH, "");
                break;
            }
            printCell(actionStr, CELL_WIDTH);
        }
        printf("\n");
    }

    return gotoTable;
}

void traverseLR1Item(LR1Item *item) {
    setReduce(item);
    setShift_Goto(item);
    for (int i = 0; i < item->numGotoItems; i++) {
        traverseLR1Item(item->gotoItems[i]);
    }
}

void setShift_Goto(LR1Item *item) {
    for (int i = 0; i < item->numGotoItems; i++) {
        LR1Item *gtItem = item->gotoItems[i];

        GoTo *gt = &gotoTable[item->stateId][getGoToColumn(gtItem->lookahead)];
        gt->act = isTerminal(gtItem->lookahead) ? SHIFT : GOTO;
        gt->state = gtItem->stateId;
    }
}

void setReduce(LR1Item *item) {
    for (ProductionRule *prod = item->production; prod; prod = prod->next) {
        if (prod->numSymbols != prod->dotPos) continue;

        symbol lastSym = prod->rhs[prod->dotPos - 1];
        if (isTerminal(lastSym)) lastSym = prod->lhs;

        for (int i = 0; i < arrayLengthTerminal(); i++) {
            if (!_followSetsTable[abs(lastSym)][i]) continue;
            GoTo *gt = &gotoTable[item->stateId][i-1];
            gt->act = (table_head[i-1].kind == _TERMINAL) ? REDUCE : GOTO;
            gt->state = prod->id;
        }
    }
}

/*
void setAcc(LR1Item *item) {

}
*/

int getGoToColumn(symbol target) {
    if (isTerminal(target)) return target - 1;
    else if (isNonTerminal(target)) return getNumTerminal() + abs(target) + 1 - 1;
    else if (target == 0) return getNumTerminal() + 1 - 1;

    exit(EXIT_FAILURE);
}

void setGoToHead() {
    table_head = (GoToHead*)calloc(len_column, sizeof(GoToHead));

    int cur = -1;
    for (int i = 1; i < getNumTerminal() + 1; i++) {
        GoToHead *terminal = &table_head[++cur];
        terminal->kind = _TERMINAL;
        terminal->sym = i;
    }

    GoToHead *terminate = &table_head[++cur];
    terminate->kind = _TERMINATE;

    for (int i = 1; i < getNumNonTerminal() + 1; i++) {
        GoToHead *nonTerminal = &table_head[++cur];
        nonTerminal->kind = _NONTERMINAL;
        nonTerminal->sym = -1 * i;
    }
}

void setGotoSide() {
    table_side = (GoToSide*)malloc(len_row * sizeof(GoToSide));
    for (int i = 0; i < len_row; i++) {
        table_side[i] = i;
    }
}

void printCell(const char *str, int width) {
    printf("%-*s|", width, str);
}
