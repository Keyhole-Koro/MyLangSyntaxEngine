#ifndef ITEM_H
#define ITEM_H

#include "syntax.h"

typedef struct {
    ProductionRule *production;
    symbol readSymbol;
    /** @brief a list of items inherit from this struct */
    struct Item *inheritingItems;
    struct Item *anotherInherits;
} Item;

void setEntry(ProductionRule *entryRule);

/*
 /\: inheritation
 -: anotherInherits

 Item6~8 is inheritated from Item5 and Item3

example
    Item1
    /    \
Item4   Item2-Item3
    \            /
     Item5      /
          \    /
          Item6-Item7-Item8
*/

#endif