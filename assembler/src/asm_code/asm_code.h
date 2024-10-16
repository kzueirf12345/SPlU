#ifndef ASSEMBLER_SRC_ASM_CODE_STRUCTS_H
#define ASSEMBLER_SRC_ASM_CODE_STRUCTS_H

#include <stdio.h>

#include "stack_on_array/libstack.h"

typedef struct AsmCode
{
    char*  code;
    size_t code_size;
    size_t comnds_size;
    char** comnds;
} asm_code_t;

#endif /*ASSEMBLER_SRC_ASM_CODE_STRUCTS_H*/