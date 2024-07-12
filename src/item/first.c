#include "first.h"

ProductionRule *probeRuleNonTerminal(ProductionRule *entryRule, ProductionRule *commonLhsRules);

void first(ProductionRule *entryRule) {
    int numNonTerminal = getNumNonTerminal();
    int numTerminal = getNumTerminal();
    symbol firstSets[numNonTerminal][numTerminal];

    ExistenceArray *isLhsDone = createExistenceArray(numNonTerminal, reviseNonTerminal);

    for (int i = 0; i < numNonTerminal; ++i) {
        for (int j = 0; j < numTerminal; ++j) {
            firstSets[i][j] = 0;
        }
    }
    
    for (ProductionRule *rule = entryRule; rule; rule = rule->next) {
        symbol lhs = rule->lhs;
        if (checkAndSetExistence(isLhsDone, lhs)) continue;
        printf("\nlhs %s ", exchangeSymbol(lhs));

        ProductionRule *commonLhsRules = filterProductions(entryRule, getLhs, lhs);

        ProductionRule *probedRules = probeRuleNonTerminal(entryRule, commonLhsRules);

        for (ProductionRule *rule = probedRules; rule; rule = rule->next) {
            symbol sym = rule->rhs[0];
            if (isNonTerminal(sym)) continue;
            firstSets[abs(lhs)][sym] = true;
        }
    }

    for (int i = 1; i < numNonTerminal; i++) {
        printf("%s: {", exchangeSymbol(-1 * i));
        for (int j = 1; j < numTerminal; j++) {
            if (!firstSets[i][j]) continue;
            printf("%s ", exchangeSymbol(j));
        }
        printf("}\n");
    }
}

void probeRuleNonTerminal_(ProductionRule *startProductionRule, symbol targetSymbol, ExistenceArray symbolExistenceArray[], ExistenceArray ruleExistenceArray[]) {

    for (ProductionRule *curRule = startProductionRule; curRule; curRule = curRule->next) {

        symbol left = curRule->lhs;

        if (left != targetSymbol) continue;

        symbol firstRightSymbol = curRule->rhs[0];

        checkAndSetExistence(ruleExistenceArray, curRule->id);

        if (isTerminal(firstRightSymbol)) continue;

        if (!checkAndSetExistence(
                symbolExistenceArray
                , firstRightSymbol))
                            probeRuleNonTerminal_(
                                startProductionRule
                                , firstRightSymbol
                                , symbolExistenceArray
                                , ruleExistenceArray);        
    }
}


ProductionRule *probeRuleNonTerminal(ProductionRule *entryRule, ProductionRule *commonLhsRules) {
    ExistenceArray *symbolExistenceArray = createExistenceArray(getNumNonTerminal(), reviseNonTerminal);
    ExistenceArray *ruleExistenceArray = createExistenceArray(getNumProductionRuleSets(), noRevise);

    for (ProductionRule *rule = commonLhsRules; rule; rule = rule->next) {
        checkAndSetExistence(ruleExistenceArray, rule->id);
        probeRuleNonTerminal_(entryRule, rule->rhs[0], symbolExistenceArray, ruleExistenceArray);
    }

    ProductionRule *start = NULL;
    ProductionRule **ppRule = &start;
    for (ProductionRule *curRule = entryRule; curRule; curRule = curRule->next) {

        if (!ruleExistenceArray->array[curRule->id]) continue;

        *ppRule = cloneProductionRules(curRule);
        ppRule = &(*ppRule)->next;
    }

    return start;
}