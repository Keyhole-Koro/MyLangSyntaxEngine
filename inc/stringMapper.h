#ifndef STRING_MAPPER_H
#define STRING_MAPPER_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "symbol.h"
#include "utils.h"

/**
    @brief This method enables to find string efficiently,
    by using number but string, calculation of comparing is improved 
*/

typedef struct StringMapping {
    char *string;
    int number;
    struct StringMapping *next;
} StringMapping;

/**
    @param isTerminal if false, non terminal
*/
symbol mapString(char *str, bool isTerminal);

void printMapping_Termianl();
void printMapping_Non_Termianl();
#endif
