#include "tokenize.h"

Token *makeToken(Token* cur, TokenKind kind, char *value);

Token *tokenizeLine(char *ipt, Token *cur) {
    char *rest;

    while (*ipt) {
        if (*ipt == '\n') {
            cur = makeToken(cur, NEWLINE, NULL);
            ipt++;
        }

        if (*ipt == ' ') {
            ipt++;
            continue;
        }

        if (*ipt == '|') {
            cur = makeToken(cur, PIPE, NULL);
            ipt++;
            continue;
        }

        if (*ipt == '\'') {

            cur = makeToken(cur, TERMINAL, readUntil(isSingleQuote, ++ipt, &rest));
            ipt = rest;
            continue;
        }
        
        if (isAlphabet(*ipt)) {
            cur = makeToken(cur, NON_TERMINAL, readUntil(isSpace, ipt, &rest));
            ipt = rest;
            continue;
        }

        ipt++;
    }
    
    return cur;
}

Token *makeToken(Token* prev, TokenKind kind, char *value) {
    Token *new_tk = malloc(sizeof(Token));
    if (new_tk == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_tk->kind = kind;
    new_tk->value = value;
    new_tk->next = NULL;
    prev->next = new_tk;
    
    return new_tk;
}
