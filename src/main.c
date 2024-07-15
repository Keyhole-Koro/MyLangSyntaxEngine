#include "main.h"

int main() {
    char *path = "./syntaxSamples/sample1.txt";

    ProductionRule *prods = processSyntaxTxt(path);

    showProductionRules();

    bool **firstSets = first(prods);

    follow(prods, firstSets);
    

    //enable_item_debug();
    //setStartRule(prods);
    //constructInitialItemSet();

    //printMapping_Non_Terminal();

    //printMapping_Terminal();
    
    return 0;
}