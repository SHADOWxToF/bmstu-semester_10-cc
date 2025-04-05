# ifndef FSM

# define FSM

# include <stdlib.h>
# include "regexp.h"
# include "set.h"

typedef struct
{
    int count_of_states;
    char **transitions;
    int *finite_states;
    int finite_states_length;
} fsm_t;

error_t fsm_create(operation_t type, void *first, fsm_t *second, fsm_t **dst);
void fsm_free(fsm_t *machine);
error_t fsm_visualize(fsm_t *machine);
error_t fsm_generate(regexp_t *expression, fsm_t **dst);
error_t fsm_modeling(fsm_t *machine, char *string, set_t **res_states);

# endif