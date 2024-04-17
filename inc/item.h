#ifndef ITEM_H
#define ITEM_H

#include "syntax.h"

typedef struct Item {
    ProductionRule *production;
    symbol targetSymbol;
    int offset_inheritingItems;
    int len_inheritingItems;
    struct Item **inheritingItems;
    struct Item *next;
} Item;

void setEntry(ProductionRule *entryRule);
Item *constructItem();

#endif
