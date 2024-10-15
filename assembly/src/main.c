#include <stdio.h>

#include "stack_on_array/libstack.h"
#include "input/input.h"

int main()
{
    if (logger_ctor())
    {
        fprintf(stderr, "Can't logger_ctor()\n");
        return EXIT_FAILURE;
    }
    if (logger_set_level_details(LOG_LEVEL_DETAILS_ALL))
    {
        fprintf(stderr, "Can't logger_set_level_details()\n");
        return EXIT_FAILURE;
    }

    stack_key_t stack = 0;
    STACK_CTOR(&stack, 8, 0);
    stack_dtor(&stack);

    asm_code_t asm_code = {};
    asm_code_ctor("../assets/program.asm", &asm_code);
    for (size_t i = 0; i < asm_code.comnd_size; ++i)
    {
        fprintf(stderr, "%s\n", asm_code.comnds[i]);
    }
    asm_code_dtor(&asm_code);

    printf("Hello assembly!\n");
    
    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        return EXIT_FAILURE;
    }
    return 0;
}