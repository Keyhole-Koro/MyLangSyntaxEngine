#include "main.h"

int main() {
    char *path = "./syntaxSamples/sample1.txt";

    processSyntaxTxt(path);

    showProductionRules();

    printMapping_Non_Terminal();

    printMapping_Terminal();

    return 0;
}