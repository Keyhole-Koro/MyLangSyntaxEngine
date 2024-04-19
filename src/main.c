#include "main.h"

int main() {
    char *path = "./syntaxSamples/sample1.txt";

    ProductionRule *prods = processSyntaxTxt(path);
    
    showProductionRules();

    setEntry(prods);
    constructItem();

    //printMapping_Non_Terminal();

    //printMapping_Terminal();
    
    return 0;
}