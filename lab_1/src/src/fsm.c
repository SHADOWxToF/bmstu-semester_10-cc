# include "fsm.h"

void fsm_print(fsm_t *machine);
error_t closure_generate(fsm_t *first, fsm_t **dst);
error_t concat_generate(fsm_t *first, fsm_t *second, fsm_t **dst);
error_t or_generate(fsm_t *first, fsm_t *second, fsm_t **dst);
char **transitions_alloc(int size);
void transitions_free(char **transitions, int size);
error_t eclosure_state(fsm_t *machine, int state, set_t *closure);
error_t eclosure(fsm_t *machine, set_t *set, set_t *closure);
error_t move(fsm_t *machine, set_t *states, char symbol, set_t *move_set);



/**
 * @brief возвращает новый автомат, полностью в новой памяти
 * исходные автоматы не претерпевают изменений и не находятся в новом автомате
 * 
 * @param type тип операции (operation_t)
 * @param first аргумент операции, для SYMBOL - указатель типа (char *)
 * @param second правый аргумент для бинарных операций, NULL в противном случае
 * @param dst указатель на результат
 * @return error_t - OK, INVALID_ARGS, INVALID_OPERATION, MEMORY_ERROR
 */
error_t fsm_create(operation_t type, void *first, fsm_t *second, fsm_t **dst)
{
    if (type == EPSILON)
    {
        fsm_t *new = calloc(1, sizeof(fsm_t));
        int *finite = calloc(1, sizeof(int));
        char **transitions = transitions_alloc(1);
        if (!new || !finite || !transitions)
        {
            free(new);
            free(finite);
            free(transitions);
            return MEMORY_ERROR;
        }
        new->count_of_states = 1;
        new->finite_states = finite;
        new->finite_states_length = 1;
        new->transitions = transitions;
        *dst = new;
        return OK;
    }
    if (type == EMPTY)
    {
        fsm_t *new = calloc(1, sizeof(fsm_t));
        char **transitions = transitions_alloc(1);
        if (!new || !transitions)
        {
            free(new);
            free(transitions);
            return MEMORY_ERROR;
        }
        new->count_of_states = 1;
        new->finite_states = NULL;
        new->finite_states_length = 0;
        new->transitions = transitions;
        *dst = new;
        return OK;
    }
    if (!first)
        return INVALID_ARGS;
    if (type == SYMBOL)
    {
        fsm_t *new = calloc(1, sizeof(fsm_t));
        int *finite = calloc(1, sizeof(int));
        char **transitions = transitions_alloc(2);
        if (!new || !finite || !transitions)
        {
            free(new);
            free(finite);
            free(transitions);
            return MEMORY_ERROR;
        }
        new->count_of_states = 2;
        new->finite_states = finite;
        new->finite_states[0] = 1;
        new->finite_states_length = 1;
        transitions[0][1] = *((char *) first);
        new->transitions = transitions;
        *dst = new;
        return OK;
    }
    if (type == CLOSURE)
    {
        error_t result = closure_generate(first, dst);
        return result;
    }
    if (!second)
        return INVALID_ARGS;
    if (type == CONCAT)
    {
        error_t result = concat_generate(first, second, dst);
        return result;
    }
    if (type == OR)
    {
        error_t result = or_generate(first, second, dst);
        return result;
    }
    return INVALID_OPERATION;
}

error_t closure_generate(fsm_t *first, fsm_t **dst)
{
    // printf("CLOSURE");
    // fsm_print(first);
    fsm_t *new = calloc(1, sizeof(fsm_t));
    int *finite = calloc(1, sizeof(int));
    char **transitions = transitions_alloc(first->count_of_states + 2);
    if (!new || !finite || !transitions)
    {
        free(new);
        free(finite);
        free(transitions);
        return MEMORY_ERROR;
    }
    // копируем данные из старого автомата
    new->count_of_states = first->count_of_states + 2;
    new->finite_states = finite;
    new->finite_states[0] = new->count_of_states - 1;
    new->finite_states_length = 1;
    new->transitions = transitions;
    for (int i = 1; i < new->count_of_states - 1; ++i)
        memcpy(new->transitions[i] + 1, first->transitions[i - 1], first->count_of_states * sizeof(char));
    
    // создаём новые связи
    new->transitions[0][1] = 'e';
    new->transitions[0][new->count_of_states - 1] = 'e';
    for (int i = 0; i < first->finite_states_length; ++i)
    {
        new->transitions[first->finite_states[i] + 1][1] = 'e';
        new->transitions[first->finite_states[i] + 1][new->count_of_states - 1] = 'e';
    }
    *dst = new;
    // fsm_print(new);
    return OK;
}

error_t concat_generate(fsm_t *first, fsm_t *second, fsm_t **dst)
{
    // printf("CONCAT");
    // fsm_print(first);
    // fsm_print(second);
    fsm_t *new = calloc(1, sizeof(fsm_t));
    int *finite = calloc(second->finite_states_length, sizeof(int));
    char **transitions = transitions_alloc(first->count_of_states + second->count_of_states);
    if (!new || !finite || !transitions)
    {
        free(new);
        free(finite);
        free(transitions);
        return MEMORY_ERROR;
    }
    // копируем данные из старых автоматов
    new->count_of_states = first->count_of_states + second->count_of_states;
    new->finite_states = finite;
    memcpy(new->finite_states, second->finite_states, second->finite_states_length * sizeof(int));
    new->finite_states_length = second->finite_states_length;
    for (int i = 0; i < new->finite_states_length; ++i)
        new->finite_states[i] += first->count_of_states;
    new->transitions = transitions;
    for (int i = 0; i < first->count_of_states; ++i)
        memcpy(new->transitions[i], first->transitions[i], first->count_of_states * sizeof(char));
    for (int i = 0; i < second->count_of_states; ++i)
        memcpy(new->transitions[i + first->count_of_states] + first->count_of_states, second->transitions[i], second->count_of_states * sizeof(char));
    
    // создаём новые связи
    for (int i = 0; i < first->finite_states_length; ++i)
        new->transitions[first->finite_states[i]][first->count_of_states] = 'e';
    *dst = new;
    // fsm_print(new);
    return OK;
}

error_t or_generate(fsm_t *first, fsm_t *second, fsm_t **dst)
{
    // printf("OR");
    // fsm_print(first);
    // fsm_print(second);
    fsm_t *new = calloc(1, sizeof(fsm_t));
    int *finite = calloc(first->finite_states_length + second->finite_states_length, sizeof(int));
    char **transitions = transitions_alloc(first->count_of_states + second->count_of_states + 1);
    if (!new || !finite || !transitions)
    {
        free(new);
        free(finite);
        free(transitions);
        return MEMORY_ERROR;
    }
    // копируем данные из старых автоматов
    new->count_of_states = first->count_of_states + second->count_of_states + 1;
    
    new->finite_states = finite;
    memcpy(new->finite_states, first->finite_states, first->finite_states_length * sizeof(int));
    memcpy(new->finite_states + first->finite_states_length, second->finite_states, second->finite_states_length * sizeof(int));
    new->finite_states_length = first->finite_states_length + second->finite_states_length;
    for (int i = 0; i < new->finite_states_length; ++i)
    {
        new->finite_states[i] += 1;
        if (i >= first->finite_states_length)
            new->finite_states[i] += first->count_of_states;
    }
    
    new->transitions = transitions;
    for (int i = 0; i < first->count_of_states; ++i)
        memcpy(new->transitions[i + 1] + 1, first->transitions[i], first->count_of_states * sizeof(char));
    for (int i = 0; i < second->count_of_states; ++i)
        memcpy(new->transitions[i + first->count_of_states + 1] + first->count_of_states + 1, second->transitions[i], second->count_of_states * sizeof(char));
    
    // создаём новые связи
    new->transitions[0][1] = 'e';
    new->transitions[0][first->count_of_states + 1] = 'e';
    *dst = new;
    // fsm_print(new);
    return OK;
}

void fsm_free(fsm_t *machine)
{
    if (machine)
    {
        transitions_free(machine->transitions, machine->count_of_states);
        free(machine->finite_states);
        free(machine);
    }
}

char **transitions_alloc(int size)
{
    char **new = calloc(size, sizeof(char *));
    if (new)
    {
        for (int i = 0; i < size; ++i)
        {
            new[i] = calloc(size, sizeof(char));
            if (!(new[i]))
            {
                transitions_free(new, size);
                return NULL;
            }
        }
    }
    return new;
}

void transitions_free(char **transitions, int size)
{
    for (int i = 0; i < size; ++i)
        free(transitions[i]);
    free(transitions);
}

error_t fsm_visualize(fsm_t *machine)
{
    if (!machine)
        return INVALID_ARGS;
    FILE *file = fopen("nfsm.gv", "w");
    if (!file)
        return MEMORY_ERROR;
    setbuf(file, NULL);
    fprintf(file, "digraph nfsm {\nrankdir=\"LR\"\n");
    
    fprintf(file, "\"pre_S\" [style=invis];\n");
    fprintf(file, "\"Sq0\";\n");
    fprintf(file, "\"pre_S\" -> \"Sq0\";\n");
    for (int i = 0; i < machine->finite_states_length; ++i)
    {
        fprintf(file, "\"Fq%d\";\n", machine->finite_states[i]);
        fprintf(file, "\"post_Fq%d\" [style=invis];\n", machine->finite_states[i]);
        fprintf(file, "\"Fq%d\" -> \"post_Fq%d\";\n", machine->finite_states[i], machine->finite_states[i]);
    }
    for (int i = 1; i < machine->count_of_states; ++i)
    {
        int j = 0;
        while (j < machine->finite_states_length && i != machine->finite_states[j])
            ++j;
        if (j == machine->finite_states_length)
            fprintf(file, "\"q%d\";\n", i);
    }
    for (int j = 0; j < machine->count_of_states; ++j)
    {
        if (machine->transitions[0][j])
        {
            char *right_finite = "";
            int k = 0;
            while (k < machine->finite_states_length && j != machine->finite_states[k])
                ++k;
            if (k != machine->finite_states_length)
                right_finite = "F";
            fprintf(file, "\"Sq0\" -> \"%sq%d\" [label=\"%c\"];\n", right_finite, j, machine->transitions[0][j]);
        }
    }
    for (int i = 1; i < machine->count_of_states; ++i)
    {
        char *left_finite = "";
        int k = 0;
        while (k < machine->finite_states_length && i != machine->finite_states[k])
            ++k;
        if (k != machine->finite_states_length)
            left_finite = "F";
        for (int j = 0; j < machine->count_of_states; ++j)
        {
            char *right_finite = "";
            if (machine->transitions[i][j])
            {
                int k = 0;
                while (k < machine->finite_states_length && j != machine->finite_states[k])
                    ++k;
                if (k != machine->finite_states_length)
                    right_finite = "F";
                fprintf(file, "\"%sq%d\" -> \"%sq%d\" [label=\"%c\"];\n", left_finite, i, right_finite, j, machine->transitions[i][j]);
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    system("dot -Tjpg nfsm.gv -onfsm.jpg");
    system("mspaint nfsm.jpg");
    return OK;
}


error_t fsm_generate(regexp_t *expression, fsm_t **dst)
{
    if (!expression)
    {
        *dst = NULL;
        return OK;
    }
    
    if (expression->operation == SYMBOL || expression->operation == EMPTY || expression->operation == EPSILON)
        return fsm_create(expression->operation, &(expression->symbol), NULL, dst);
    
    fsm_t *left = NULL;
    fsm_t *right = NULL;
    error_t result = OK;
    if ((result = fsm_generate(expression->left, &left)) != OK ||
    (result = fsm_generate(expression->right, &right)) != OK)
    {
        fsm_free(left);
        fsm_free(right);
        return result;
    }
    result = fsm_create(expression->operation, left, right, dst);
    fsm_free(left);
    fsm_free(right);
    return result;
}

void fsm_print(fsm_t *machine)
{
    printf("\nАвтомат\n");
    for (int i = 0; i < machine->count_of_states; ++i)
    {
        for(int j = 0; j < machine->count_of_states; ++j)
        {
            char res = (machine->transitions[i][j]) ? machine->transitions[i][j] : '_';
            printf("%c ", res);
        }
        printf("\n");
    }
    printf("Конечные состояния: ");
    for (int i = 0; i < machine->finite_states_length; ++i)
        printf("%d ", machine->finite_states[i]);
    printf("\n");
}

error_t fsm_modeling(fsm_t *machine, char *string, set_t **res_states)
{
    set_t *states = set_alloc();
    if (!states)
        return MEMORY_ERROR;
    set_push(states, 0);
    error_t result = OK;
    while (*string)
    {
        set_t *eclosure_set = set_alloc();
        set_t *move_set = set_alloc();
        if (!eclosure_set || !move_set)
        {
            free(eclosure_set);
            free(move_set);
            free(states);
            return MEMORY_ERROR;
        }
        if ((result = eclosure(machine, states, eclosure_set)) != OK ||
        (result = move(machine, eclosure_set, *string, move_set)) != OK)
        {
            set_free(eclosure_set);
            set_free(move_set);
            set_free(states);
            return result;
        }
        set_free(eclosure_set);
        set_free(states);
        states = move_set;
        string += 1;
    }
    set_t *eclosure_set = set_alloc();
    if (!eclosure_set)
    {
        free(states);
        return MEMORY_ERROR;
    }
    if ((result = eclosure(machine, states, eclosure_set)) != OK)
    {
        free(states);
        return result;
    }
    *res_states = eclosure_set;
    
    return result;
}


error_t eclosure_state(fsm_t *machine, int state, set_t *closure)
{
    if (element_in_set(state, closure))
        return OK;
    
    error_t result = OK;
    if ((result = set_push(closure, state)) != OK)
        return result;

    for (int j = 0; j < machine->count_of_states; ++j)
    {
        if (machine->transitions[state][j] == 'e')
        {
            if ((result = eclosure_state(machine, j, closure)) != OK)
                return result;
        }
    }
    return result;
}


error_t eclosure(fsm_t *machine, set_t *set, set_t *closure)
{
    error_t result = OK;
    for (int i = 0; i < set->count; ++i)
    {
        if ((result = eclosure_state(machine, set->array[i], closure)) != OK)
            return result;
    }
    return result;
}

error_t move(fsm_t *machine, set_t *states, char symbol, set_t *move_set)
{
    error_t result = OK;
    for (int i = 0; i < states->count; ++i)
        for (int j = 0; j < machine->count_of_states; ++j)
            if (machine->transitions[states->array[i]][j] == symbol)
                if ((result = set_push(move_set, j)) != OK)
                    return result;
    return result;
}