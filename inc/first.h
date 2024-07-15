#ifndef FIRST_H
#define FIRST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "production.h"
#include "existanceArray.h"
#include "syntax.h"

#define table_max_terminal getNumTerminal() + 1
#define table_max_non_terminal getNumNonTerminal() + 1

bool **first(ProductionRule *entryRule);

#endif