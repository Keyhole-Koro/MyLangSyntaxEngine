#include "main.h"

int main() {
    char *path = "./syntaxSamples/sample1.txt";

    ProductionRule *prods = processSyntaxTxt(path);

    //showProductionRules();

    bool **firstSets = first(prods);

    bool **followSets = follow(prods, firstSets);    

    ///enable_item_debug();
    setStartRule(prods);
    LR1Item *entryItem = constructInitialItemSet();

    GotoTable(firstSets, followSets, entryItem);

    //printMapping_Non_Terminal();

    //printMapping_Terminal();
    
    return 0;
}