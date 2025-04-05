# include "menu.h"

error_t read(grammar_t **grammar);
error_t generate(grammar_t **grammar);
char *read_filename();
error_t modeling(fsm_t *fsm, set_t **res_set);
char *read_string();

error_t menu()
{
    error_t result = OK;
    printf("\nВас приветствует лабораторная работа №1!!!\n");
    int choice = -1;
    grammar_t *grammar = NULL;
    regexp_t *solution = NULL;
    fsm_t *fsm = NULL;
    set_t *res_modeling = NULL;
    while (choice && result == OK)
    {
        printf("\n\n\nКакое действие Вы хотите совешить?\n");
        printf("1 - ввести грамматику\n");
        printf("2 - сгенерировать файл конфигурации и заполнить грамматику на его основе\n");
        printf("3 - вывести грамматику\n");
        printf("4 - вывести регулярное выражение\n");
        printf("5 - вывести НКА\n");
        printf("6 - промоделировать НКА\n\n");
        printf("0 - выход\n\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);
        char ch = 1;
        while (ch != '\n')
            scanf("%c", &ch);
        switch (choice)
        {
        case 1:
            delete_grammar(grammar);
            regexp_free(solution);
            fsm_free(fsm);
            grammar = NULL;
            solution = NULL;
            fsm = NULL;
            if ((result = read(&grammar)) != OK ||
            (result = solve_system(grammar, &solution)) != OK ||
            (result = fsm_generate(solution, &fsm)) != OK)
            {
                delete_grammar(grammar);
                regexp_free(solution);
                fsm_free(fsm);
                grammar = NULL;
                solution = NULL;
                fsm = NULL;
            }
            choice = -1;
            break;
        case 2:
            delete_grammar(grammar);
            regexp_free(solution);
            fsm_free(fsm);
            grammar = NULL;
            solution = NULL;
            fsm = NULL;
            if ((result = generate(&grammar)) != OK ||
            (result = solve_system(grammar, &solution)) != OK ||
            (result = fsm_generate(solution, &fsm)) != OK)
            {
                delete_grammar(grammar);
                regexp_free(solution);
                fsm_free(fsm);
                grammar = NULL;
                solution = NULL;
                fsm = NULL;
            }
            choice = -1;
            break;
        case 3:
            if (grammar)
                result = print_grammar(stdout, grammar);
            else
                result = STRUCT_DIDNT_INITIALIZED;
            choice = -1;
            break;
        case 4:
            if (solution)
                regexp_print(solution);
            else
                result = STRUCT_DIDNT_INITIALIZED;
            choice = -1;
            break;
        case 5:
            if (fsm)
                result = fsm_visualize(fsm);
            else
                result = STRUCT_DIDNT_INITIALIZED;
            choice = -1;
            break;
        case 6:
            if (fsm)
            {
                set_free(res_modeling);
                res_modeling = NULL;
                result = modeling(fsm, &res_modeling);
                printf("Конечные состояния: ");
                if (result == OK)
                    set_print(res_modeling, stdout);
                bool is_finite = false;
                for (int i = 0; i < fsm->finite_states_length; ++i)
                    if (element_in_set(fsm->finite_states[i], res_modeling))
                        is_finite = true;
                printf("Результат: строка %s достижима\n", is_finite ? "" : "не");
            }
            else
                result = STRUCT_DIDNT_INITIALIZED;
            choice = -1;
            break;
        case 7:
            choice = -1;
            regexp_t *zero;
            regexp_t *one;
            regexp_t *empty;
            regexp_t *epsilon;
            regexp_t *S;
            regexp_t *A;
            regexp_t *B;
            regexp_t *_1;
            regexp_t *_2;
            regexp_t *_3;
            regexp_t *_4;
            regexp_create(SYMBOL, "1", NULL, &one);
            regexp_create(SYMBOL, "0", NULL, &zero);
            regexp_create(EMPTY, NULL, NULL, &empty);
            regexp_create(EPSILON, NULL, NULL, &epsilon);
            regexp_create(SYMBOL, "S", NULL, &S);
            regexp_create(SYMBOL, "A", NULL, &A);
            regexp_create(SYMBOL, "B", NULL, &B);
            regexp_create(CONCAT, zero, A, &_1);
            regexp_create(CONCAT, one, S, &_2);
            regexp_create(OR, _1, _2, &_3);
            regexp_create(OR, _3, epsilon, &_4);
            regexp_print(_4);
            regexp_free(one);
            regexp_free(zero);
            regexp_free(empty);
            regexp_free(epsilon);
            regexp_free(S);
            regexp_free(A);
            regexp_free(B);
            regexp_free(_1);
            regexp_free(_2);
            regexp_free(_3);
            regexp_free(_4);
            break;
        case 0:
            break;
        
        default:
            printf("\nНеверный ввод, повторите попытку!!!\n\n");
            break;
        }
        switch (result)
        {
        case OK:
            break;
        case INCORRECT_FILENAME:
            result = OK;
            printf("Не удалось прочесть файл\n");
            break;
        case STRUCT_DIDNT_INITIALIZED:
            result = OK;
            printf("Грамматика не инициализирована\n");
            break;
        case INVALID_INPUT:
            result = OK;
            printf("Содержимое файла не является грамматикой\n");
            break;
        
        default:
            printf("\nres = %d\n", result);
            printf("Ой-ой, что-то пошло не так\n");
            break;
        }
        if (choice != 0)
        {
            printf("Нажмите enter для продолжения...");
            ch = 1;
            while (ch != '\n')
                scanf("%c", &ch);
        }
    }
    delete_grammar(grammar);
    regexp_free(solution);
    fsm_free(fsm);
    set_free(res_modeling);
    printf("Спасибо, что воспользовались данной программой!!!\n");
    return result;
}

error_t modeling(fsm_t *fsm, set_t **res_set)
{
    printf("Введите строку для моделирования: ");
    char *string = read_string();
    if (!string)
        return MEMORY_ERROR;
    return fsm_modeling(fsm, string, res_set);
}

error_t read(grammar_t **grammar)
{
    printf("Введите имя файла с грамматикой: ");
    char *input_filename = read_filename();
    if (!input_filename)
        return MEMORY_ERROR;

    grammar_t *new_grammar;
    error_t result = OK;
    if ((result = read_grammar(input_filename, &new_grammar)) == OK)
    {
        delete_grammar(*grammar);
        *grammar = new_grammar;
    }
    free(input_filename);
    return result;
}

error_t generate(grammar_t **grammar)
{
    printf("Введите имя файла в который сгенерировать грамматику: ");
    char *output_filename = read_filename();
    if (!output_filename)
        return MEMORY_ERROR;
    FILE *file = fopen(output_filename, "w");
    if (!file)
    {
        free(output_filename);
        return INCORRECT_FILENAME;
    }
    grammar_t *new_grammar = NULL;
    error_t result = OK;
    if ((result = generate_grammar(&new_grammar)) == OK)
    {
        delete_grammar(*grammar);
        *grammar = new_grammar;
        print_grammar(file, *grammar);
        fclose(file);
    }
    free(output_filename);
    return result;
}

char *read_filename()
{
    char *filename = malloc((FILENAME_LEN + 1) * sizeof(char));
    if (!filename)
        return NULL;
    while (scanf("%s", filename) != 1);
    char ch = 1;
    while (ch != '\n')
        scanf("%c", &ch);
    return filename;
}

char *read_string()
{
    char *filename = malloc((FILENAME_LEN + 1) * sizeof(char));
    if (!filename)
        return NULL;
    int i = 0;
    while (i < FILENAME_LEN && scanf("%c", filename + i) == 1 && filename[i] != '\n' && filename[i] != '\r')
        ++i;
    filename[i] = '\0';
    // char ch = 1;
    // while (ch != '\n')
    //     scanf("%c", &ch);
    return filename;
}