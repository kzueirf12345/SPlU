#ifndef ASSEMBLER_SRC_ASM_CODE_STRUCTS_H
#define ASSEMBLER_SRC_ASM_CODE_STRUCTS_H

#include <stdio.h>
#include <assert.h>


enum AsmCodeError
{
    ASM_CODE_ERROR_SUCCESS  = 0,
    ASM_CODE_ERROR_FAILURE  = 1
};
static_assert(ASM_CODE_ERROR_SUCCESS == 0);

const char* asm_code_strerror(const enum AsmCodeError error);

#define ASM_CODE_ERROR_HANDLE(call_func, ...)                                                          \
    do {                                                                                            \
        asm_code_error_handler = call_func;                                                            \
        if (asm_code_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Asm_code error: %s\n",                               \
                            asm_code_strerror(asm_code_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return asm_code_error_handler;                                                             \
        }                                                                                           \
    } while(0)

typedef struct AsmCode
{
    char*  code;
    size_t code_size;
    size_t comnds_size;
    char** comnds;
} asm_code_t;

enum AsmCodeError asm_code_ctor(const char* const input_filename, asm_code_t* const asm_code);
void asm_code_dtor(asm_code_t* asm_code);


#endif /*ASSEMBLER_SRC_ASM_CODE_STRUCTS_H*/