# include "regexp.h"

int regexp_compare(regexp_t *first, regexp_t *second);
void regexp_free(regexp_t *regexp);
error_t closure_simplify(regexp_t *first, regexp_t **dst);
error_t or_simplify(regexp_t *first, regexp_t *second, regexp_t **dst);
error_t concat_simplify(regexp_t *first, regexp_t *second, regexp_t **dst);
int get_number_of_terms(regexp_t *tree);
void split_into_terms(regexp_t *tree, regexp_t **summa_terms, int *count, int *max_count);
void delete_same_terms(regexp_t **summa_terms, int count);
void print_recursive_polish(regexp_t *tree);
void print_recursive_arithmetic(regexp_t *tree);

/**
 * @brief возвращает новое дерево, полностью в новой памяти
 * исходные деревья не претерпевают изменений и не находятся в новом дереве
 * деревья first и second не должны иметь узлы с одинаковым адресом
 * 
 * @param type тип операции (operation_t)
 * @param first аргумент операции, для SYMBOL - указатель типа (char *)
 * @param second правый аргумент для бинарных операций, NULL в противном случае
 * @param dst указатель на результат
 * @return error_t - OK, INVALID_ARGS, INVALID_OPERATION, MEMORY_ERROR
 */
error_t regexp_create(operation_t type, void *first, regexp_t *second, regexp_t **dst)
{
    if (type == EPSILON || type == EMPTY)
    {
        regexp_t *new_exp = calloc(1, sizeof(regexp_t));
        if (!new_exp)
            return MEMORY_ERROR;
        new_exp->operation = type;
        new_exp->symbol = 'o';
        if (type == EPSILON)
            new_exp->symbol = 'e';
        *dst = new_exp;
        return OK;
    }
    if (!first)
        return INVALID_ARGS;
    if (type == SYMBOL)
    {
        regexp_t *new_exp = calloc(1, sizeof(regexp_t));
        if (!new_exp)
            return MEMORY_ERROR;
        new_exp->operation = type;
        new_exp->symbol = *((char *) first);
        *dst = new_exp;
        return OK;
    }
    if (type == CLOSURE)
    {
        error_t result = closure_simplify(first, dst);
        return result;
    }
    if (!second)
        return INVALID_ARGS;
    if (type == CONCAT)
    {
        error_t result = concat_simplify(first, second, dst);
        return result;
    }
    if (type == OR)
    {
        error_t result = or_simplify(first, second, dst);
        return result;
    }
    return INVALID_OPERATION;
}

int regexp_compare(regexp_t *first, regexp_t *second)
{
    if (first == second)
        return 0;
    if (!first || !second)
        return 1;
    if (first->symbol != second->symbol || first->operation != second->operation)
        return 1;
    return regexp_compare(first->left, second->left) || regexp_compare(first->right, second->right);
}

error_t closure_simplify(regexp_t *first, regexp_t **dst)
{
    regexp_t *copy_first = NULL;
    error_t result = OK;
    if ((result = regexp_copy(first, &copy_first)) != OK)
        return result;
    first = copy_first;
    // правило 2
    if (first->operation == EMPTY)
    {
        first->operation = EPSILON;
        *dst = first;
        return OK;
    }
    // правило 10
    if (first->operation == CLOSURE)
    {
        *dst = first;
        return OK;
    }
    regexp_t *closure = calloc(1, sizeof(regexp_t));
    if (!closure)
    {
        regexp_free(first);
        return MEMORY_ERROR;
    }
    closure->operation = CLOSURE;
    closure->symbol = '*';
    closure->left = first;
    closure->right = NULL;
    *dst = closure;
    return OK;

}

error_t concat_simplify(regexp_t *first, regexp_t *second, regexp_t **dst)
{
    regexp_t *copy_first = NULL;
    regexp_t *copy_second = NULL;
    error_t result = OK;
    if ((result = regexp_copy(first, &copy_first)) != OK)
        return result;
    if ((result = regexp_copy(second, &copy_second)) != OK)
    {
        regexp_free(copy_first);
        return result;
    }
    // не затрагиваем исходные данные, указатель на них остаётся вне этой функции
    first = copy_first;
    second = copy_second;
    // правило 7
    if (first->operation == EPSILON)
    {
        *dst = second;
        regexp_free(first);
        return OK;
    }
    if (second->operation == EPSILON)
    {
        *dst = first;
        regexp_free(second);
        return OK;
    }
    // правило 8
    if (first->operation == EMPTY)
    {
        *dst = first;
        regexp_free(second);
        return OK;
    }
    if (second->operation == EMPTY)
    {
        *dst = second;
        regexp_free(first);
        return OK;
    }
    int first_max_count = get_number_of_terms(first);
    int second_max_count = get_number_of_terms(first);
    // если не раскрывается по правилу 5 и 6
    if (first_max_count == 1 && second_max_count == 1)
    {
        regexp_t *concat = calloc(1, sizeof(regexp_t));
        if (!concat)
        {
            regexp_free(first);
            regexp_free(second);
            return MEMORY_ERROR;
        }
        concat->operation = CONCAT;
        concat->symbol = '^';
        concat->left = first;
        concat->right = second;
        *dst = concat;
        return OK;
    }
    // правила 5 и 6 
    int first_count = 0;
    int second_count = 0;
    int result_max_count = first_max_count * second_max_count;
    regexp_t **first_summa_terms = calloc(first_max_count, sizeof(regexp_t *));
    regexp_t **second_summa_terms = calloc(second_max_count, sizeof(regexp_t *));
    regexp_t **result_summa_terms = calloc(result_max_count, sizeof(regexp_t *));
    if (!first_summa_terms || !second_summa_terms || !result_summa_terms)
    {
        regexp_free(first);
        regexp_free(second);
        free(first_summa_terms);
        free(second_summa_terms);
        free(result_summa_terms);
        return MEMORY_ERROR;
    }
    split_into_terms(first, first_summa_terms, &first_count, &first_max_count);
    split_into_terms(second, second_summa_terms, &second_count, &second_max_count);
    int result_count = 0;
    for (int i = 0; i < first_count; ++i)
    {
        for (int j = 0; j < second_count; ++j)
        {
            if ((result = regexp_create(CONCAT, first_summa_terms[i], second_summa_terms[j], &(result_summa_terms[result_count]))) != OK)
            {
                for (int k = 0; k < first_count; ++k)
                    regexp_free(first_summa_terms[k]);
                free(first_summa_terms);
                for (int k = 0; k < second_count; ++k)
                    regexp_free(second_summa_terms[k]);
                free(second_summa_terms);
                for (int k = 0; k < result_count; ++k)
                    regexp_free(result_summa_terms[k]);
                free(result_summa_terms);
                return result;
            }
            ++result_count;
        }
    }
    result = construct_tree(result_summa_terms, first_count * second_count, dst);
    free(result_summa_terms);

    for (int i = 0; i < first_count; ++i)
        regexp_free(first_summa_terms[i]);
    free(first_summa_terms);
    for (int i = 0; i < second_count; ++i)
        regexp_free(second_summa_terms[i]);
    free(second_summa_terms);

    return result;
}

error_t or_simplify(regexp_t *first, regexp_t *second, regexp_t **dst)
{
    regexp_t *copy_first = NULL;
    regexp_t *copy_second = NULL;
    error_t result = OK;
    if ((result = regexp_copy(first, &copy_first)) != OK)
        return result;
    if ((result = regexp_copy(second, &copy_second)) != OK)
    {
        regexp_free(copy_first);
        return result;
    }
    // не затрагиваем исходные данные, указатель на них остаётся вне этой функции
    first = copy_first;
    second = copy_second;
    // если равны, то + не нужен
    // правило 11
    if (!regexp_compare(first, second))
    {
        *dst = first;
        regexp_free(second);
        return OK;
    }
    // правило 12
    if (first->operation == EMPTY)
    {
        *dst = second;
        regexp_free(first);
        return OK;
    }
    // строка + пустое множество
    // правило 12
    if (second->operation == EMPTY)
    {
        *dst = first;
        regexp_free(second);
        return OK;
    }
    // равные узлы в глубине дерева и сумма узла и его замыкания в глубине дерева
    // правило 9 и 11 (иерархические)
    int count = 0;
    int max_count = get_number_of_terms(first);
    max_count += get_number_of_terms(second);
    regexp_t **summa_terms = calloc(max_count, sizeof(regexp_t *));
    if (!summa_terms)
    {
        regexp_free(first);
        regexp_free(second);
        return MEMORY_ERROR;
    }
    result = OK;
    split_into_terms(first, summa_terms, &count, &max_count);
    split_into_terms(second, summa_terms, &count, &max_count);
    delete_same_terms(summa_terms, count);
    result = construct_tree(summa_terms, count, dst);
    free(summa_terms);
    return result;
}

void regexp_free(regexp_t *regexp)
{
    if (regexp)
    {
        regexp_free(regexp->left);
        regexp_free(regexp->right);
        free(regexp);
    }
}

int get_number_of_terms(regexp_t *tree)
{
    if (tree->operation == OR)
    {
        int number = get_number_of_terms(tree->left);
        number += get_number_of_terms(tree->right);
        return number;
    }
    return 1;
}

void split_into_terms(regexp_t *tree, regexp_t **summa_terms, int *count, int *max_count)
{
    if (tree->operation == OR)
    {
        split_into_terms(tree->left, summa_terms, count, max_count);
        split_into_terms(tree->right, summa_terms, count, max_count);
        free(tree);
    }
    else
        summa_terms[(*count)++] = tree;
}

void delete_same_terms(regexp_t **summa_terms, int count)
{
    for (int i = count - 1; i >= 0; --i)
    {
        int j = i - 1;
        while (j >= 0)
        {
            // правило 9 и 11
            if (!regexp_compare(summa_terms[i], summa_terms[j]) || (summa_terms[j]->operation == CLOSURE && !regexp_compare(summa_terms[i], summa_terms[j]->left)))
            {
                regexp_free(summa_terms[i]);
                summa_terms[i] = NULL;
                j = 0;
            }
            else if (summa_terms[i]->operation == CLOSURE && !regexp_compare(summa_terms[i]->left, summa_terms[j]))
            {
                regexp_free(summa_terms[j]);
                // меняем местами, чтобы был удалён правый элемент
                summa_terms[j] = summa_terms[i];
                summa_terms[i] = NULL;
                j = 0;
            }
            --j;
        }
    }
}

error_t construct_tree(regexp_t **summa_terms, int count, regexp_t **result)
{
    *result = summa_terms[0];
    for (int i = 1; i < count; ++i)
    {
        if (summa_terms[i])
        {
            regexp_t *plus = calloc(1, sizeof(regexp_t));
            if (!plus)
            {
                regexp_free(*result);
                *result = NULL;
                while (i < count)
                {
                    free(summa_terms[i]);
                    ++i;
                }
                return MEMORY_ERROR;
            }
            plus->operation = OR;
            plus->symbol = '+';
            plus->left = *result;
            plus->right = summa_terms[i];
            *result = plus;
            summa_terms[i] = NULL;
        }
    }
    return OK;
}

error_t regexp_copy(regexp_t *real, regexp_t **copy)
{
    if (real)
    {
        *copy = calloc(1, sizeof(regexp_t));
        if (!copy)
            return MEMORY_ERROR;
        (*copy)->symbol = real->symbol;
        (*copy)->operation = real->operation;
        regexp_copy(real->left, &((*copy)->left));
        regexp_copy(real->right, &((*copy)->right));
        return OK;
    }
    *copy = NULL;
    return OK;
}

void regexp_print(regexp_t *tree)
{
    printf("\nПольская нотация: ");
    print_recursive_polish(tree);
    printf("\nАрифметическая нотация: ");
    print_recursive_arithmetic(tree);
    printf("\n");
}

void print_recursive_polish(regexp_t *tree)
{
    if (tree)
    {
        printf("%c", tree->symbol);
        print_recursive_polish(tree->left);
        print_recursive_polish(tree->right);
    }
}

void print_recursive_arithmetic(regexp_t *tree)
{
    if (tree)
    {
        switch (tree->operation)
        {
        case OR:
            // printf("(");
            print_recursive_arithmetic(tree->left);
            printf("+");
            print_recursive_arithmetic(tree->right);
            // printf(")");
            break;
        case CONCAT:
            print_recursive_arithmetic(tree->left);
            print_recursive_arithmetic(tree->right);
            break;
        case CLOSURE:
            if (tree->left->operation == SYMBOL)
            {
                print_recursive_arithmetic(tree->left);
                printf("*");
            }
            else
            {
                printf("(");
                print_recursive_arithmetic(tree->left);
                // print_recursive_arithmetic(tree->right);
                printf(")*");
            }
            break;
        case SYMBOL:
            printf("%c", tree->symbol);
            break;
        case EPSILON:
            printf("%c", tree->symbol);
            break;
        case EMPTY:
            printf("%c", tree->symbol);
            break;
        default:
            break;
        }
    }
}