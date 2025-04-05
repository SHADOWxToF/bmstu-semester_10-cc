# include "set.h"



set_t *set_alloc()
{
    set_t *set = calloc(1, sizeof(set_t));
    if (set)
    {
        set->max_count = 10;
        set->array = calloc(set->max_count, sizeof(int));
        if (!(set->array))
        {
            set_free(set);
            set = NULL;
        }
    }
    return set;
}


error_t set_pop(set_t *set, int *element)
{
    if (set->count)
    {
        *element = set->array[(set->count)-- - 1];
        return OK;
    }
    return EMPTY_SET;
}

error_t set_push(set_t *set, int element)
{
    if (element_in_set(element, set))
        return OK;
    if (set->count == set->max_count)
    {
        int *new = realloc(set->array, set->max_count * 2);
        if (!new)
            return MEMORY_ERROR;
        set->array = new;
        set->max_count *= 2;
    }
    set->array[(set->count)++] = element;
    return OK;

}

bool element_in_set(int element, set_t *set)
{
    int i = 0;
    while (i < set->count && set->array[i] != element)
        ++i;
    return i != set->count;
}

error_t set_sum(set_t *first, set_t *second)
{
    error_t result = OK;
    for (int i = 0; i < second->count; ++i)
        if ((result = set_push(first, second->array[i])) != OK)
            return result;
    return result;
}


void set_free(set_t *set)
{
    if (set)
    {
        free(set->array);
        free(set);
    }
}

void set_print(set_t *set, FILE *stream)
{
    fprintf(stream, "{");
    for (int i = 0; i < set->count; ++i)
        fprintf(stream, "%d ", set->array[i]);
    fprintf(stream, "}\n");
}