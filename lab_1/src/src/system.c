# include "system.h"

void print_system(char *nonterminals, regexp_t **system, int len);
error_t get_solution(char symbol, regexp_t *expression, regexp_t **solution);
bool is_end_symbol(char symbol, regexp_t *tree);
void exclude_symbol(char symbol, regexp_t **tree);
error_t substitute_solution(regexp_t **equations, int len, char symbol, regexp_t *solution);
error_t substitute(regexp_t **tree, char symbol, regexp_t *substitution);
error_t had_substituted(regexp_t **tree, char symbol, regexp_t *substitution, bool *had_been);


error_t solve_system(grammar_t *grammar, regexp_t **solution)
{
    if (!grammar)
        return STRUCT_DIDNT_INITIALIZED;
    regexp_t **equations = calloc(grammar->nonterminal_length, sizeof(regexp_t *));
    if (!equations)
        return MEMORY_ERROR;
    error_t result = OK;
    operation_t type_1, type_2;
    regexp_t *first, *second, *concat, *new_solution;

    // создать уравнения
    for (int i = 0; i < grammar->nonterminal_length; ++i)
    {
        if ((result = regexp_create(EMPTY, NULL, NULL, &(equations[i]))) != OK)
        {
            for (int k = 0; k < grammar->nonterminal_length; ++k)
                regexp_free(equations[k]);
            free(equations);
            return result;
        }
        for (int j = 0; j < grammar->rules_length; ++j)
        {
            if (grammar->rules[j].left == grammar->nonterminals[i])
            {
                type_1 = (grammar->rules[j].right[0]) ? SYMBOL : EPSILON;
                type_2 = (grammar->rules[j].right[1]) ? SYMBOL : EPSILON;
                first = NULL;
                second = NULL;
                concat = NULL;
                new_solution = NULL;
                if ((result = regexp_create(type_1, &(grammar->rules[j].right[0]), NULL, &first)) != OK ||
                (result = regexp_create(type_2, &(grammar->rules[j].right[1]), NULL, &second)) != OK || 
                (result = regexp_create(CONCAT, first, second, &concat)) != OK ||
                (result = regexp_create(OR, equations[i], concat, &new_solution)) != OK)
                {
                    regexp_free(first);
                    regexp_free(second);
                    regexp_free(concat);
                    for (int k = 0; k < grammar->nonterminal_length; ++k)
                        regexp_free(equations[k]);
                    free(equations);
                    return result;
                }
                regexp_free(first);
                regexp_free(second);
                regexp_free(concat);
                regexp_free(equations[i]);
                equations[i] = new_solution;
            }
        }
    }
    print_system(grammar->nonterminals, equations, grammar->nonterminal_length);

    for (int i = 1; i < grammar->nonterminal_length; ++i)
    {
        regexp_t *solution_i = NULL;
        if ((result = get_solution(grammar->nonterminals[i], equations[i], &solution_i)) != OK)
        {
            for (int k = 0; k < grammar->nonterminal_length; ++k)
                regexp_free(equations[k]);
            free(equations);
            return result;
        }
        regexp_free(equations[i]);
        equations[i] = solution_i;

        if ((result = substitute_solution(equations, grammar->nonterminal_length, grammar->nonterminals[i], solution_i)) != OK)
        {
            for (int k = 0; k < grammar->nonterminal_length; ++k)
                regexp_free(equations[k]);
            free(equations);
            return result;
        }
    }
    result = get_solution(grammar->nonterminals[0], equations[0], solution);
    for (int k = 0; k < grammar->nonterminal_length; ++k)
        regexp_free(equations[k]);
    free(equations);
    return result;
}

void print_system(char *nonterminals, regexp_t **system, int len)
{
    printf("\nСистема с регулярными коэффициентами:\n");
    for (int i = 0; i < len; ++i)
    {
        printf("%c = ", nonterminals[i]);
        print_recursive_arithmetic(system[i]);
        printf("\n");
    }
}

error_t get_solution(char symbol, regexp_t *expression, regexp_t **solution)
{
    regexp_t *copy_expression = NULL;
    error_t result = OK;
    if ((result = regexp_copy(expression, &copy_expression)) != OK)
        return result;
    expression = copy_expression;
    int count = 0;
    int max_count = get_number_of_terms(expression);
    regexp_t **summa_terms = calloc(max_count, sizeof(regexp_t *));
    if (!summa_terms)
    {
        regexp_free(expression);
        return MEMORY_ERROR;
    }
    
    regexp_t *closure = NULL;
    if ((result = regexp_create(EMPTY, NULL, NULL, &closure)) != OK)
    {
        regexp_free(expression);
        free(summa_terms);
        return result;
    }
    split_into_terms(expression, summa_terms, &count, &max_count);
    for (int i = 0; i < count; ++i)
    {
        if (is_end_symbol(symbol, summa_terms[i]))
        {
            exclude_symbol(symbol, &(summa_terms[i]));
            regexp_t *or = NULL;
            if ((result = regexp_create(OR, closure, summa_terms[i], &or)) != OK)
            {
                regexp_free(closure);
                for (int i = 0; i < count; ++i)
                    regexp_free(summa_terms[i]);
                free(summa_terms);
                return result;
            }
            regexp_free(closure);
            closure = or;
            regexp_free(summa_terms[i]);
            summa_terms[i] = NULL;
        }
    }
    result = OK;
    if (!(summa_terms[0]))
    {
        int i = 0;
        while (i < count && !(summa_terms[i]))
            ++i;
        if (i == count)
            result = regexp_create(EMPTY, NULL, NULL, &(summa_terms[0]));
        else 
        {
            summa_terms[0] = summa_terms[i];
            summa_terms[i] = NULL;
        }
    }
    regexp_t *left_half = NULL;
    regexp_t *right_half = NULL;
    if (result == OK && (result = construct_tree(summa_terms, count, &right_half)) == OK && (result = regexp_create(CLOSURE, closure, NULL, &left_half)) == OK)
        result = regexp_create(CONCAT, left_half, right_half, solution);
    regexp_free(left_half);
    regexp_free(right_half);
    regexp_free(closure);
    free(summa_terms);
    return result;
}

bool is_end_symbol(char symbol, regexp_t *tree)
{
    if (!tree)
        return false;
    regexp_t *right = tree->right;
    while (right)
    {
        tree = right;
        right = tree->right;
    }
    return tree->operation == SYMBOL && tree->symbol == symbol;
}

void exclude_symbol(char symbol, regexp_t **tree)
{
    if (tree && *tree)
    {
        if ((*tree)->operation == SYMBOL && (*tree)->symbol == symbol)
        {
            (*tree)->operation = EPSILON;
            (*tree)->symbol = 'e';
            (*tree)->left = NULL;
            (*tree)->right = NULL;
        }
        else if ((*tree)->right->operation == SYMBOL && (*tree)->right->symbol == symbol)
        {
            regexp_t *new = (*tree)->left;
            (*tree)->left = NULL;
            regexp_free(*tree);
            *tree = new;
        }
        else
        {
            regexp_t *current = *tree;
            regexp_t *right = current->right->right;
            while (right->operation != SYMBOL)
            {
                current = current->right;
                right = right->right;
            }
            right = current->right;
            current->right = right->left;
            right->left = NULL;
            regexp_free(right);
        }
    }
}

error_t substitute_solution(regexp_t **equations, int len, char symbol, regexp_t *solution)
{
    error_t result = OK;
    for (int i = 0; i < len; ++i)
    {
        if ((result = substitute(&(equations[i]), symbol, solution)) != OK)
            return result;
    }
    return result;
}

error_t substitute(regexp_t **tree, char symbol, regexp_t *substitution)
{
    bool had_been = false;
    return had_substituted(tree, symbol, substitution, &had_been);
}

error_t had_substituted(regexp_t **tree, char symbol, regexp_t *substitution, bool *had_been)
{
    *had_been = false;
    error_t result = OK;
    if (tree && *tree)
    {
        if ((*tree)->operation == SYMBOL && (*tree)->symbol == symbol)
        {
            *had_been = true;
            regexp_t *copy = NULL;
            if ((result = regexp_copy(substitution, &copy)) != OK)
                return result;
            regexp_free(*tree);
            *tree = copy;
        }
        else
        {
            bool had_been_left = false;
            bool had_been_right = false;
            if ((result = had_substituted(&((*tree)->left), symbol, substitution, &had_been_left)) != OK ||
            (result = had_substituted(&((*tree)->right), symbol, substitution, &had_been_right)) != OK)
                return result;
            *had_been = had_been_left || had_been_right;
            if (*had_been)
            {
                regexp_t *new = NULL;
                result = regexp_create((*tree)->operation, (*tree)->left, (*tree)->right, &new);
                if (result == OK)
                {
                    regexp_free(*tree);
                    *tree = new;
                }
            }
            return result;
        }
    }
    return result;
}
/*
S = 0A+1S+e
A = 0B+1A
B = 0S+1B

B = 1*0S
A = 01*0S+1A = 1*01*0S
S = 01*01*0S + 1S + e
S = (01*01*0+1)*

B = 1*0S = ^*1^0S
A = 1*0B = ^*1^0B
0A = ^0A = ^0^*1^0^*1^0S = 01*01*0S
01*01*0S = 
*/