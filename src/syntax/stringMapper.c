#include "stringMapper.h"

bool is_terminal = true;

int number_nonTerminal = -1;
int number_Terminal = 1;

int current_num_strmap = 0;
int max_size_strmap = 32;
StringMapping mappings_terminal = {NULL, 0, NULL};
StringMapping mappings_nonTerminal = {NULL, 0, NULL};

StringMapping *string_mappings;

StringMapping *registerStringMapping(const char *str);

symbol mapString(char *str, bool isTerminal) {
    if (isTerminal) {
        is_terminal = true;
        string_mappings = &mappings_terminal;
    } else {        
        is_terminal = false;
        string_mappings = &mappings_nonTerminal;
    }

	// checks if the same name has existed
    for (StringMapping *current = string_mappings; current != NULL; current = current->next) {
        if (current->string && strcmp(current->string, str) == 0) {
            return current->number;
        }
    }
    
    StringMapping *new_mapping = registerStringMapping(str);
    return new_mapping->number;
}

StringMapping *registerStringMapping(const char *str) {
    StringMapping *new_mapping = (StringMapping *)malloc(sizeof(StringMapping));
    if (new_mapping == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    new_mapping->string = my_strdup(str);
    if (new_mapping->string == NULL) {
        fprintf(stderr, "String duplication failed\n");
        exit(EXIT_FAILURE);
    }

    new_mapping->number = is_terminal ? number_Terminal++ : number_nonTerminal--;
    new_mapping->next = NULL;

    StringMapping **ppCurrent = &string_mappings;
    while (*ppCurrent != NULL) {
        ppCurrent = &((*ppCurrent)->next);
    }
    *ppCurrent = new_mapping;


    return new_mapping;
}

void printMapping_Terminal() {
    StringMapping *current = &mappings_terminal;
    while (current) {
        printf("String: %s, number: %d\n", current->string, current->number);
        current = current->next;
    }
}

void printMapping_Non_Terminal() {
    StringMapping *current = &mappings_nonTerminal;
    while (current) {
        printf("String: %s, number: %d\n", current->string, current->number);
        current = current->next;
    }
}