#include <stdio.h>

#include "stack_on_array/libstack.h"
#include "input/input.h"
#include "assembly/assembly.h"

int main()
{
    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;

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

    asm_error_handle(assembly(asm_code), asm_code_dtor(&asm_code); logger_dtor(););

    asm_code_dtor(&asm_code);

// ----------------------    

    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        return EXIT_FAILURE;
    }
    return 0;
}