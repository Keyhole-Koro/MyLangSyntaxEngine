#include <stdio.h>
#include <stdlib.h>

#include "syntax_engine/syntax_engine.h"

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        fprintf(stderr, "MyLang grammar test failure: %s (%s:%d)\n", message, __FILE__, __LINE__); \
        exit(1); \
    } \
} while (0)

static void expect_status(
    SyntaxTable *table,
    const char *name,
    const char **tokens,
    size_t token_count,
    SyntaxStatus expected
) {
    SyntaxResult result = syntax_parse_token_names(table, tokens, token_count);
    if (result.status != expected) {
        fprintf(
            stderr,
            "%s: expected status %d, got %d at token %zu\n",
            name,
            expected,
            result.status,
            result.token_index
        );
        syntax_result_free(&result);
        exit(1);
    }
    syntax_result_free(&result);
}

#define EXPECT_OK(table, name, tokens) \
    expect_status(table, name, tokens, sizeof(tokens) / sizeof((tokens)[0]), SYNTAX_OK)

int main(void) {
    const char *fixture = "tests/fixtures/grammars/mylang_lsp.grammar";
    SyntaxGrammar *grammar = syntax_load_grammar(fixture);
    CHECK(grammar != NULL, "MyLang grammar must load");
    SyntaxTable *table = syntax_build_lr1_table(grammar);
    CHECK(table != NULL, "MyLang grammar table must build");

    const char *generic_struct[] = {
        "STRUCT", "IDENTIFIER", "LT", "IDENTIFIER", "COMMA", "IDENTIFIER", "GT",
        "L_BRACE", "IDENTIFIER", "IDENTIFIER", "SEMICOLON",
        "IDENTIFIER", "IDENTIFIER", "SEMICOLON", "R_BRACE", "SEMICOLON"
    };
    EXPECT_OK(table, "generic struct", generic_struct);

    const char *generic_function[] = {
        "IDENTIFIER", "IDENTIFIER", "LT", "IDENTIFIER", "GT", "L_PARENTHESES",
        "IDENTIFIER", "IDENTIFIER", "R_PARENTHESES", "L_BRACE",
        "RETURN", "IDENTIFIER", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "generic function", generic_function);

    const char *generic_prototype[] = {
        "IDENTIFIER", "IDENTIFIER", "LT", "IDENTIFIER", "GT", "L_PARENTHESES",
        "IDENTIFIER", "IDENTIFIER", "R_PARENTHESES", "SEMICOLON"
    };
    EXPECT_OK(table, "generic function prototype", generic_prototype);

    const char *nested_generic_type[] = {
        "IDENTIFIER", "LT", "IDENTIFIER", "LT", "IDENTIFIER", "GT", "GT",
        "IDENTIFIER", "SEMICOLON"
    };
    EXPECT_OK(table, "nested generic type", nested_generic_type);

    const char *generic_call[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "GENERIC_LT", "I32", "GENERIC_GT", "L_PARENTHESES", "NUMBER", "R_PARENTHESES",
        "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "generic call", generic_call);

    const char *nested_generic_call[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "GENERIC_LT", "IDENTIFIER", "LT", "I32", "GT", "GENERIC_GT",
        "L_PARENTHESES", "NUMBER", "R_PARENTHESES", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "nested generic call", nested_generic_call);

    const char *relational_expression[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "LT", "IDENTIFIER", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "relational expression", relational_expression);

    const char *chained_relational_expression[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "LT", "IDENTIFIER", "GT", "IDENTIFIER", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "chained relational expression", chained_relational_expression);

    const char *shift_expression[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "RSH", "NUMBER", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "right shift expression", shift_expression);

    const char *split_shift_expression[] = {
        "I32", "IDENTIFIER", "L_PARENTHESES", "R_PARENTHESES", "L_BRACE", "RETURN",
        "IDENTIFIER", "GT", "GT", "NUMBER", "SEMICOLON", "R_BRACE"
    };
    EXPECT_OK(table, "split right shift expression", split_shift_expression);

    const char *trailing_type_param[] = {
        "STRUCT", "IDENTIFIER", "LT", "IDENTIFIER", "COMMA", "GT",
        "L_BRACE", "R_BRACE", "SEMICOLON"
    };
    expect_status(
        table,
        "trailing type parameter comma",
        trailing_type_param,
        sizeof(trailing_type_param) / sizeof(trailing_type_param[0]),
        SYNTAX_ERROR
    );

    syntax_free_table(table);
    syntax_free_grammar(grammar);
    puts("MyLang grammar tests passed");
    return 0;
}
