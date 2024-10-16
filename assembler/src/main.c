#include <stdio.h>

#include "stack_on_array/libstack.h"
#include "input/input.h"
#include "assembly/assembly.h"

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
    
    printf("Hello assembly!\n");

// ----------------------

    asm_code_t asm_code = {};
    asm_code_ctor("../assets/program.asm", &asm_code);

    assembly(asm_code);

    asm_code_dtor(&asm_code);

// ----------------------    

    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        return EXIT_FAILURE;
    }
    return 0;
}