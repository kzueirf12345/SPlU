#ifndef ASSEMBLER_SRC_INPUT_H
#define ASSEMBLER_SRC_INPUT_H

#include <stdio.h>
#include <assert.h>

#include "../asm_code/asm_code.h"


enum InputError
{
    INPUT_ERROR_SUCCESS  = 0,
    INPUT_ERROR_FAILURE  = 1
};
static_assert(INPUT_ERROR_SUCCESS == 0);

const char* input_strerror(const enum InputError input_error);

#define input_error_handle(call_func, ...)                                                          \
    do {                                                                                            \
        input_error_handler = call_func;                                                            \
        if (input_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Input error: %s\n",                               \
                            input_strerror(input_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return input_error_handler;                                                             \
        }                                                                                           \
    } while(0)



enum InputError asm_code_ctor(const char* const input_filename, asm_code_t* const asm_code);
void asm_code_dtor(asm_code_t* asm_code);


#endif /*ASSEMBLER_SRC_INPUT_H*/