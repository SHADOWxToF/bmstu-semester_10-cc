# ifndef GRAMMAR

# define GRAMMAR

# include "consts.h"
# include <stdlib.h>
# include <string.h>

typedef struct
{
    char left;
    char right[3];
} rule_t;

typedef struct
{
    char *terminals;
    int terminal_length;
    int terminal_maxlength;

    char *nonterminals;
    int nonterminal_length;
    int nonterminal_maxlength;
    
    rule_t *rules;
    int rules_length;
    int rules_maxlength;
    
    char start;
} grammar_t;


error_t read_grammar(char *filename, grammar_t **new_grammar);
error_t delete_grammar(grammar_t *self);

error_t print_grammar(FILE *fstream, grammar_t *self);
error_t generate_grammar(grammar_t **new_grammar);

# endif