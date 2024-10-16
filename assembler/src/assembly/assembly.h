#ifndef ASSEMBLER_SRC_ASSEMBLY_H
#define ASSEMBLER_SRC_ASSEMBLY_H

#include <assert.h>
#include <stdint.h>

#include "../asm_code/asm_code.h"

enum AsmError
{
    ASM_ERROR_SUCCESS           = 0,
    ASM_ERROR_STACK             = 1,
    ASM_ERROR_DIV_BY_ZERO       = 2,
    ASM_ERROR_STANDARD_ERRNO    = 3,
    ASM_ERROR_INCORRECT_CMND    = 4,
    ASM_ERROR_UNKNOWN           = 5
};
static_assert(ASM_ERROR_SUCCESS == 0);

const char* asm_strerror(const enum AsmError asm_error);

#define asm_error_handle(call_func, ...)                                                            \
    do {                                                                                            \
        asm_error_handler = call_func;                                                              \
        if (asm_error_handler)                                                                      \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Asm error: %s\n",                                 \
                            asm_strerror(asm_error_handler));                                       \
            __VA_ARGS__                                                                             \
            return asm_error_handler;                                                               \
        }                                                                                           \
    } while(0)

typedef int64_t instruction_t; // TODO 

enum AsmError assembly(const asm_code_t asm_code);

#endif /*ASSEMBLER_SRC_ASSEMBLY_H*/