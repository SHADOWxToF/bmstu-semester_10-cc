# include <stdio.h>
# include "consts.h"
# include "menu.h"

error_t error_func(error_t error);

int main()
{
    return error_func(menu());
}


error_t error_func(error_t error)
{
    switch (error)
    {
    case OK:
        break;
    case MEMORY_ERROR:
        printf("проблемы при выделении памяти\n");
        break;
    case INVALID_INPUT:
        printf("неверный ввод\n");
        break;
    case INCORRECT_FILENAME:
        printf("неверное имя файла\n");
        break;
    default:
        break;
    }
    return error;
}