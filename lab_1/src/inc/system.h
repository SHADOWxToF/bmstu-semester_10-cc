# ifndef SYSTEM_H

# define SYSTEM_H

# include "consts.h"
# include "regexp.h"
# include "grammar.h"

error_t solve_system(grammar_t *grammar, regexp_t **solution);


# endif
