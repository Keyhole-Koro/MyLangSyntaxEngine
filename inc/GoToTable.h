#ifndef GOTO_TABLE_H
#define GOTO_TABLE_H

#include "production.h"
#include "stringMapper.h"
#include "item.h"
#include "existanceArray.h"

typedef enum {
    SHIFT = 1,
    REDUCE,
    ACCEPT,
    GOTO
} Action;

typedef enum {
    _TERMINAL = 1,
    _NONTERMINAL,
    _TERMINATE
} ColumnKind;

typedef struct {
    Action act;
    int state;
} GoTo;

typedef struct {
    ColumnKind kind;
    symbol sym;
} GoToHead;

typedef int GoToSide;

GoTo **GotoTable(bool **first, bool **follow, LR1Item *entryItem);

#endif