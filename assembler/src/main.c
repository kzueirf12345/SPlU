#include <stdio.h>
#include <stdlib.h>

#include "instructions/instructions.h"
#include "logger/liblogger.h"
#include "asm_code/asm_code.h"
#include "assembly/assembly.h"
#include "utils.h"
#include "stack_on_array/libstack.h"

int main()
{
    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;
    enum AsmCodeError asm_code_error_handler = ASM_CODE_ERROR_SUCCESS;
    enum InstructsError instructs_error_handler = INSTRUCTS_ERROR_SUCCESS;

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
    
    printf(GREEN_TEXT("\nHello assembly!\n"));

// ----------------------

    asm_code_t asm_code = {};
    ASM_CODE_ERROR_HANDLE(asm_code_ctor("../assets/program.asm", &asm_code), 
                          logger_dtor(); asm_code_dtor(&asm_code););

    instructs_t instructs = {};
    INSTRUCTS_ERROR_HANDLE(instructs_ctor(&instructs, asm_code.comnds_size * 3 * sizeof(operand_t)),
                           logger_dtor(); instructs_dtor(&instructs);); //TODO count size

    ASM_ERROR_HANDLE(assembly(asm_code, &instructs),
                     logger_dtor(); asm_code_dtor(&asm_code); instructs_dtor(&instructs););

    INSTRUCTS_ERROR_HANDLE(instructs_output("../assets/program_code.bin", instructs),
                           logger_dtor(); asm_code_dtor(&asm_code); instructs_dtor(&instructs););
    
    instructs_dtor(&instructs);
    asm_code_dtor(&asm_code);

// ----------------------    

    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        return EXIT_FAILURE;
    }
    return 0;
}