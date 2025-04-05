# include "grammar.h"

error_t read_terminals(FILE *file, char **terminal_set, int *length, int *maxlength);
error_t read_nonterminals(FILE *file, char *terminal_set, int terminal_len, char **nonterminal_set, int *length, int *maxlength);
error_t read_rules(FILE *file, grammar_t *grammar);
error_t read_start(FILE *file, grammar_t *grammar);
error_t add_rule(grammar_t *grammar, rule_t *rule);
void print_right(FILE *fstream, char *right);

error_t read_grammar(char *filename, grammar_t **new_grammar)
{
    grammar_t *grammar = calloc(1, sizeof(grammar_t));
    if (!grammar)
        return MEMORY_ERROR;
    FILE *file = fopen(filename, "r");
    if (!file)
        return INCORRECT_FILENAME;
    error_t result = OK;
    result = read_terminals(file, &(grammar->terminals), &(grammar->terminal_length), &(grammar->terminal_maxlength));
    if (result != OK)
    {
        delete_grammar(grammar);
        fclose(file);
        return result;
    }
    result = read_nonterminals(file, grammar->terminals, grammar->terminal_length, 
                               &(grammar->nonterminals), &(grammar->nonterminal_length),
                               &(grammar->nonterminal_maxlength));
    if (result != OK)
    {
        delete_grammar(grammar);
        fclose(file);
        return result;
    }
    result = read_rules(file, grammar);
    if (result != OK)
    {
        delete_grammar(grammar);
        fclose(file);
        return result;
    }
    result = read_start(file, grammar);
    if (result != OK)
    {
        delete_grammar(grammar);
        fclose(file);
        return result;
    }
    *new_grammar = grammar;
    fclose(file);
    return result;
}

error_t read_terminals(FILE *file, char **terminal_set, int *length, int *maxlength)
{
    *terminal_set = calloc(10, sizeof(char));
    if (!terminal_set)
        return MEMORY_ERROR;
    
    *length = 0;
    *maxlength = 10;
    char c = 0;
    while (fscanf(file, "%c", &c) == 1 && c != 'T');
    if (fscanf(file, "erminals: %c", &c) != 1 || c != '{')
    {
        free(*terminal_set);
        *terminal_set = NULL;
        return INVALID_INPUT;
    }
    int is_readed = false;
    while (fscanf(file, "%c", &c) == 1 && c != '}')
    {
        // между запятыми нет символа
        if (!is_readed && c == ',')
        {
            free(*terminal_set);
            *terminal_set = NULL;
            return INVALID_INPUT;
        }        
        // новый символ
        if (c == ',')
            is_readed = false;
        // читаем символ
        if (!strchr(", \r\n\t", c))
        {
            // 2 символа между запятыми
            if (is_readed)
            {
                free(*terminal_set);
                *terminal_set = NULL;
                return INVALID_INPUT;
            }
            is_readed = true;
            
            // проверка на наличие данного символа в множестве
            int i;
            for (i = 0; i != *length; ++i)
                if (c == (*terminal_set)[i])
                    break;
            if (i != *length)
                continue;
            
            // перевыделение памяти
            if (*length == *maxlength - 1)
            {
                char *new = realloc(*terminal_set, *maxlength * 2 * sizeof(char));
                if (!new)
                {
                    free(*terminal_set);
                    *terminal_set = NULL;
                    return MEMORY_ERROR;
                }
                *terminal_set = new;
                *maxlength *= 2;
                memset((*terminal_set) + *length, 0, *maxlength - *length);
            }
            // сохраняем символ
            (*terminal_set)[(*length)++] = c;
        }
    }
    return OK;
}

error_t read_nonterminals(FILE *file, char *terminal_set, int terminal_len, char **nonterminal_set, int *length, int *maxlength)
{
    *nonterminal_set = calloc(10, sizeof(char));
    if (!nonterminal_set)
        return MEMORY_ERROR;
    
    *length = 0;
    *maxlength = 10;
    char c = 0;
    while (fscanf(file, "%c", &c) == 1 && c != 'N');
    if (fscanf(file, "onterminals: %c", &c) != 1 || c != '{')
    {
        free(*nonterminal_set);
        *nonterminal_set = NULL;
        return INVALID_INPUT;
    }
    int is_readed = false;
    while (fscanf(file, "%c", &c) == 1 && c != '}')
    {
        // между запятыми нет символа
        if (!is_readed && c == ',')
        {
            free(*nonterminal_set);
            *nonterminal_set = NULL;
            return INVALID_INPUT;
        }        
        // новый символ
        if (c == ',')
            is_readed = false;
        // читаем символ
        if (!strchr(", \r\n\t", c))
        {
            // 2 символа между запятыми
            if (is_readed)
            {
                free(*nonterminal_set);
                *nonterminal_set = NULL;
                return INVALID_INPUT;
            }
            is_readed = true;
            
            // проверка на наличие данного символа в множестве
            int i;
            for (i = 0; i != *length; ++i)
                if (c == (*nonterminal_set)[i])
                    break;
            if (i != *length)
                continue;

            // проверка на наличие данного символа в множестве терминалов
            for (i = 0; i != terminal_len; ++i)
                if (c == terminal_set[i])
                {
                    free(*nonterminal_set);
                    *nonterminal_set = NULL;
                    return INVALID_INPUT;
                }
            
            // перевыделение памяти
            if (*length == *maxlength)
            {
                char *new = realloc(*nonterminal_set, *maxlength * 2 * sizeof(char));
                if (!new)
                {
                    free(*nonterminal_set);
                    *nonterminal_set = NULL;
                    return MEMORY_ERROR;
                }
                *nonterminal_set = new;
                *maxlength *= 2;
                memset((*nonterminal_set) + *length, 0, *maxlength - *length);
            }
            // сохраняем символ
            (*nonterminal_set)[(*length)++] = c;
        }
    }
    return OK;
}

error_t read_rules(FILE *file, grammar_t *grammar)
{
    grammar->rules = calloc(10, sizeof(rule_t));
    if (!(grammar->rules))
        return MEMORY_ERROR;
    
    grammar->rules_length = 0;
    grammar->rules_maxlength = 10;
    char c = 0;
    while (fscanf(file, "%c", &c) == 1 && c != 'R');
    fscanf(file, "ules: {");
    rule_t new_rule;
    // добавляем терминирующий ноль, чтобы рассматривать правую часть как строку
    new_rule.right[2] = 0;
    int scan_result = 0;
    int stop_reading = false;
    while (!stop_reading)
    {
        while ((scan_result = fscanf(file, "%c", &c)) == 1 && strchr(" \r\n\t", c));
        if (c == '}')
        {
            stop_reading = true;
            break;
        }
        if (strchr(",|->", c))
        {
            free(grammar->rules);
            grammar->rules = NULL;
            return INVALID_INPUT;
        }
        // нашли левую часть правила
        new_rule.left = c;
        // читаем до стрелки
        while ((scan_result = fscanf(file, "%c", &c)) == 1 && strchr(" \r\n\t", c));
        if (c != '-')
        {
            free(grammar->rules);
            grammar->rules = NULL;
            return INVALID_INPUT;
        }
        // читаем стрелку
        if ((scan_result = fscanf(file, "%c", &c)) != 1 || c != '>')
        {
            free(grammar->rules);
            grammar->rules = NULL;
            return INVALID_INPUT;
        }
        int stop_reading_right = false;
        while (!stop_reading_right)
        {
            new_rule.right[0] = 0;
            new_rule.right[1] = 0;
            while ((scan_result = fscanf(file, "%c", &c)) == 1 && strchr(" \r\n\t", c));
            // читаем правую часть
            if (strchr(",|->}", c))
            {
                free(grammar->rules);
                grammar->rules = NULL;
                return INVALID_INPUT;
            }
            // проверяем что первый символ это терминал
            if (!strchr(grammar->terminals, c) && c != 'e')
            {
                free(grammar->rules);
                grammar->rules = NULL;
                return INVALID_INPUT;
            }
            else if (c == 'e')
            {
                new_rule.right[0] = 0;
                if ((scan_result = fscanf(file, "%c", &c)) != 1)
                {
                    free(grammar->rules);
                    grammar->rules = NULL;
                    return INVALID_INPUT;   
                }
            }
            else
            {
                new_rule.right[0] = c;
                // читаем второй символ
                if ((scan_result = fscanf(file, "%c", &c)) != 1 || strchr("->", c))
                {
                    free(grammar->rules);
                    grammar->rules = NULL;
                    return INVALID_INPUT;
                }
                // второй символ это терминал-нетерминал или иной символ
                if (strchr(grammar->nonterminals, c) || strchr(grammar->terminals, c))
                {
                    new_rule.right[1] = c;
                    if ((scan_result = fscanf(file, "%c", &c)) != 1)
                    {
                        free(grammar->rules);
                        grammar->rules = NULL;
                        return INVALID_INPUT;   
                    }
                }
            }
            error_t result = add_rule(grammar, &new_rule);
            if (result != OK)
            {
                free(grammar->rules);
                grammar->rules = NULL;
                return result;
            }
            while (strchr(" \t\r\n", c) && (scan_result = fscanf(file, "%c", &c)) == 1);
            if (c == ',')
                stop_reading_right = true;
            else if (c == '}')
            {
                stop_reading = true;
                stop_reading_right = true;
            }
            else if (c != '|')
            {
                free(grammar->rules);
                grammar->rules = NULL;
                return INVALID_INPUT;
            }
        }
    }
    return OK;
}

error_t read_start(FILE *file, grammar_t *grammar)
{   
    char c = 0;
    while (fscanf(file, "%c", &c) == 1 && c != 'S');
    if (fscanf(file, "tart symbol: %c", &c) != 1 || !strchr(grammar->nonterminals, c))
        return INVALID_INPUT;
    grammar->start = c;
    // положить стартовый нетерминал на первое место
    int i = 0;
    while (grammar->nonterminals[i++] != grammar->start);
    --i;
    grammar->nonterminals[i] = grammar->nonterminals[0];
    grammar->nonterminals[0] = grammar->start;
    return OK;
}


error_t add_rule(grammar_t *grammar, rule_t *rule)
{
    // проверка на наличие такого правила
    int i;
    for (i = 0; i < grammar->rules_length; ++i)
    {
        if ((grammar->rules)[i].left == (*rule).left && !strcmp(grammar->rules[i].right, (*rule).right))
            break;
    }
    if (i < grammar->rules_length)
        return OK;
    // перевыделение памяти
    if (grammar->rules_length == grammar->rules_maxlength)
    {
        rule_t *new = realloc(grammar->rules, grammar->rules_maxlength * 2 * sizeof(rule_t));
        if (!new)
            return MEMORY_ERROR;
        grammar->rules = new;
        grammar->rules_maxlength *= 2;
    }
    // сохраняем символ
    (grammar->rules)[(grammar->rules_length)++] = *rule;
    return OK;
}

error_t delete_grammar(grammar_t *self)
{
    if (self)
    {
        free(self->terminals);
        free(self->nonterminals);
        free(self->rules);
        free(self);
    }
    return OK;
}

error_t print_grammar(FILE *fstream, grammar_t *self)
{
    if (!self)
        return STRUCT_DIDNT_INITIALIZED;
    fprintf(fstream, "\nTerminals: {");
    char *terminal = self->terminals;
    for (; terminal < self->terminals + self->terminal_length - 1; ++terminal)
        fprintf(fstream, "%c, ", *terminal);
    fprintf(fstream, "%c}\n", *terminal);
    
    fprintf(fstream, "\nNonterminals: {");
    char *nonterminal = self->nonterminals;
    for (; nonterminal < self->nonterminals + self->nonterminal_length - 1; ++nonterminal)
        fprintf(fstream, "%c, ", *nonterminal);
    fprintf(fstream, "%c}\n", *nonterminal);

    fprintf(fstream, "\nRules: {");
    rule_t *rule = self->rules;
    while (rule != self->rules + self->rules_length)
    {
        fprintf(fstream, "%c -> ", rule->left);
        print_right(fstream, rule->right);
        rule_t *next = rule + 1;
        while (next != self->rules + self->rules_length && next->left == rule->left)
        {
            fprintf(fstream, " | ");
            print_right(fstream, next->right);
            rule = next;
            ++next;
        }
        ++rule;
        if (rule != self->rules + self->rules_length)
            fprintf(fstream, ",\n");
    }
    fprintf(fstream, "}\n");
    fprintf(fstream, "\nStart symbol: %c\n", self->start);
    return OK;
}

void print_right(FILE *fstream, char *right)
{
    if (right[0])
    {
        fprintf(fstream, "%c", right[0]);
        if (right[1])
            fprintf(fstream, "%c", right[1]);
    }
    else
        fprintf(fstream, "e");
}

error_t generate_grammar(grammar_t **new_grammar)
{
    grammar_t *grammar = calloc(1, sizeof(grammar_t));
    if (!grammar)
        return MEMORY_ERROR;
    grammar->terminals = calloc(10, sizeof(char));
    grammar->nonterminals = calloc(10, sizeof(char));
    grammar->rules = calloc(10, sizeof(rule_t));
    if (!(grammar->terminals) || !(grammar->nonterminals) || !(grammar->rules))
    {
        delete_grammar(grammar);
        grammar = NULL;
        return MEMORY_ERROR;
    }
    grammar->terminal_length = 2;
    grammar->terminal_maxlength = 10;
    grammar->terminals[0] = '0';
    grammar->terminals[1] = '1';
    grammar->nonterminal_maxlength = 10;
    grammar->nonterminal_length = 3;
    grammar->nonterminals[0] = 'S';
    grammar->nonterminals[1] = 'A';
    grammar->nonterminals[2] = 'B';
    grammar->rules_maxlength = 10;
    grammar->rules_length = 7;
    grammar->rules[0].left = 'S';
    grammar->rules[0].right[0] = '0';
    grammar->rules[0].right[1] = 'A';
    grammar->rules[1].left = 'S';
    grammar->rules[1].right[0] = '1';
    grammar->rules[1].right[1] = 'S';
    grammar->rules[2].left = 'S';
    grammar->rules[3].left = 'A';
    grammar->rules[3].right[0] = '0';
    grammar->rules[3].right[1] = 'B';
    grammar->rules[4].left = 'A';
    grammar->rules[4].right[0] = '1';
    grammar->rules[4].right[1] = 'A';
    grammar->rules[5].left = 'B';
    grammar->rules[5].right[0] = '0';
    grammar->rules[5].right[1] = 'S';
    grammar->rules[6].left = 'B';
    grammar->rules[6].right[0] = '1';
    grammar->rules[6].right[1] = 'B';

    grammar->start = 'S';
    *new_grammar = grammar;
    return OK;
}