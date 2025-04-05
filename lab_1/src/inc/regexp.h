# ifndef REGEXP

# define REGEXP

# include "consts.h"
# include <stdlib.h>
# include <string.h>
# include "grammar.h"

typedef enum {CONCAT, CLOSURE, OR, SYMBOL, EPSILON, EMPTY} operation_t;

typedef struct regexp_t
{
    char symbol;
    operation_t operation;
    struct regexp_t *left;
    struct regexp_t *right;
} regexp_t;

error_t regexp_create(operation_t type, void *first, regexp_t *second, regexp_t **dst);
void regexp_free(regexp_t *regexp);
void regexp_print(regexp_t *tree);
void print_recursive_arithmetic(regexp_t *tree);
int get_number_of_terms(regexp_t *tree);
void split_into_terms(regexp_t *tree, regexp_t **summa_terms, int *count, int *max_count);
error_t construct_tree(regexp_t **summa_terms, int count, regexp_t **result);
error_t regexp_copy(regexp_t *real, regexp_t **copy);
// 1*(01*0(01*01*0+1)*01*+e) =

// ^ - конкатинация

// = ^*1+^0^*1^0^*+^0^*1^0^*101^0*1e
# endif