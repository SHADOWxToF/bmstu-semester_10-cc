# include "test.h"


// create_sp
int test_create_sp()
{
    int errors = 0;
    sp_t *sp = NULL;
    errors += !(create_sp(&sp) == OK && sp != NULL);
    errors += !(delete_sp(&sp) == OK && sp == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// print_sp
int test_print_sp()
{
    int errors = 0;
    sp_t *sp = NULL;
    errors += !(create_sp(&sp) == OK && sp != NULL);
    errors += !(print_sp(sp) == OK && sp != NULL);
    errors += !(delete_sp(&sp) == OK && sp == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// permutate_sp
int test_permutate_sp()
{
    int errors = 0;
    sp_t *sp = NULL;
    errors += !(create_sp(&sp) == OK && sp != NULL);
    errors += !(permutate_sp(sp, 'A') == 'A');
    errors += !(delete_sp(&sp) == OK && sp == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// add_connection
int test_add_connection()
{
    int errors = 0;
    sp_t *sp = NULL;
    errors += !(create_sp(&sp) == OK && sp != NULL);
    errors += !(add_connection(sp, 'A', 'B') == OK);
    errors += !(permutate_sp(sp, 'A') == 'B');
    errors += !(permutate_sp(sp, 'B') == 'A');
    errors += !(delete_connection(sp, 'A') == OK);
    errors += !(permutate_sp(sp, 'A') == 'A');
    errors += !(delete_sp(&sp) == OK && sp == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}





// create_rf
int test_create_rf()
{
    int errors = 0;
    reflector_t *rf = NULL;
    errors += !(create_rf(&rf, 1) == OK && rf != NULL);
    errors += !(delete_rf(&rf) == OK && rf == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// permutate_rf
int test_permutate_rf()
{
    int errors = 0;
    reflector_t *rf = NULL;
    errors += !(create_rf(&rf, 1) == OK && rf != NULL);
    errors += !(permutate_rf(rf, 'A') == 'Y');
    errors += !(delete_rf(&rf) == OK && rf == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// create_rt
int test_create_rt()
{
    int errors = 0;
    rotor_t *rt = NULL;
    errors += !(create_rt(&rt, 1) == OK && rt != NULL);
    errors += !(delete_rt(&rt) == OK && rt == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// input_rt
int test_input_rt()
{
    int errors = 0;
    rotor_t *rt = NULL;
    errors += !(create_rt(&rt, 1) == OK && rt != NULL);
    errors += !(input_rt(rt, 'A') == 'E');
    errors += !(delete_rt(&rt) == OK && rt == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// output_rt
int test_output_rt()
{
    int errors = 0;
    rotor_t *rt = NULL;
    errors += !(create_rt(&rt, 1) == OK && rt != NULL);
    errors += !(output_rt(rt, 'A') == 'U');
    errors += !(delete_rt(&rt) == OK && rt == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// get_state_rt
int test_get_state_rt()
{
    int errors = 0;
    rotor_t *rt = NULL;
    errors += !(create_rt(&rt, 1) == OK && rt != NULL);
    errors += !(get_state_rt(rt) == 'A');
    errors += !(delete_rt(&rt) == OK && rt == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// set_state_rt
int test_set_state_rt()
{
    int errors = 0;
    rotor_t *rt = NULL;
    errors += !(create_rt(&rt, 1) == OK && rt != NULL);
    errors += !(set_state_rt(rt, 'F') == OK);
    errors += !(get_state_rt(rt) == 'F');
    errors += !(delete_rt(&rt) == OK && rt == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// create_ngm
int test_create_ngm()
{
    int errors = 0;
    enigma_t *enigma = NULL;
    errors += !(create_ngm(&enigma) == OK && enigma != NULL);
    errors += !(delete_ngm(&enigma) == OK && enigma == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// print_ngm
int test_print_ngm()
{
    int errors = 0;
    enigma_t *enigma = NULL;
    errors += !(create_ngm(&enigma) == OK && enigma != NULL);
    errors += !(print_ngm(enigma) == OK);
    errors += !(delete_ngm(&enigma) == OK && enigma == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

// encode_ngm
int test_encode_ngm()
{
    int errors = 0;
    enigma_t *enigma = NULL;
    errors += !(create_ngm(&enigma) == OK && enigma != NULL);
    errors += !(encode_ngm(enigma, 'H') == 'F');
    errors += !(delete_ngm(&enigma) == OK && enigma == NULL);
    if (errors) printf("Ошибка в %s\n", __func__);
    else printf("Пройден %s\n", __func__);
    return errors;
}

int main()
{
    int errors = 0;
    errors += test_create_sp();
    errors += test_print_sp();
    errors += test_permutate_sp();
    errors += test_add_connection();
    
    errors += test_create_rf();
    errors += test_permutate_rf();
    
    errors += test_create_rt();
    errors += test_input_rt();
    errors += test_output_rt();
    errors += test_get_state_rt();
    errors += test_set_state_rt();    

    errors += test_create_ngm();
    errors += test_print_ngm();
    errors += test_encode_ngm();

    printf("Количество ошибок: %d", errors);
    return errors;
}