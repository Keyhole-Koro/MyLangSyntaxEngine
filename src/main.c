#include "main.h"

int main() {
    char *path = "./syntaxSamples/sample1.txt";

    const ProductionRule *prods = processSyntaxTxt(path);

    first(prods);
    
    //showProductionRules();

    //enable_item_debug();
    //setStartRule(prods);
    //constructInitialItemSet();

    //printMapping_Non_Terminal();

    //printMapping_Terminal();
    
    return 0;
}