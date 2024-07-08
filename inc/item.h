#ifndef ITEM_H
#define ITEM_H

#include "syntax.h"

#include "stringMapper.h"

typedef struct Item {
    ProductionRule *production;
    symbol targetSymbol;
    int numInheritingItems;
    int maxInheritingItems;
    struct Item **inheritingItems;
} Item;

void setEntry(ProductionRule *entryRule);
Item *constructItem();

#endif