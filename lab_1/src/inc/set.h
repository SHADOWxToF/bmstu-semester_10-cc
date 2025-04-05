# ifndef SET_H

# define SET_H

# include <stdlib.h>
# include "consts.h"

typedef struct
{
    int *array;
    int count;
    int max_count;
} set_t;


set_t *set_alloc();
void set_free(set_t *set);
error_t set_pop(set_t *set, int *element);
error_t set_push(set_t *set, int element);
bool element_in_set(int element, set_t *set);
error_t set_sum(set_t *first, set_t *second);
void set_print(set_t *set, FILE *stream);

# endif