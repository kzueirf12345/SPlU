#ifndef ASSEMBLER_SRC_TOKENS_H
#define ASSEMBLER_SRC_TOKENS_H

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

enum InstructsError
{
    INSTRUCTS_ERROR_SUCCESS  = 0,
    INSTRUCTS_ERROR_FAILURE  = 1
};
static_assert(INSTRUCTS_ERROR_SUCCESS == 0);

const char* instructs_strerror(const enum InstructsError instruct_error);

#define INSTRUCTS_ERROR_HANDLE(call_func, ...)                                                         \
    do {                                                                                            \
        const enum InstructsError instructs_error_handler = call_func;                                                     \
        if (instructs_error_handler)                                                             \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Instructs error: %s\n",                        \
                            instructs_strerror(instructs_error_handler));                           \
            __VA_ARGS__                                                                             \
            return instructs_error_handler;                                                      \
        }                                                                                           \
    } while(0)


typedef struct Instructs
{
    uint8_t* data;
    size_t counter;
    size_t size;
} instructs_t;

enum InstructsError instructs_ctor(instructs_t* const instructs, const size_t size);
void instructs_dtor(instructs_t* const instructs);

enum InstructsError instructs_push_back(instructs_t* const instructs, const void* const elem, 
                                        const size_t elem_size);

enum InstructsError instructs_output(const char* const output_filename, instructs_t instructs);

#endif /*ASSEMBLER_SRC_TOKENS_H*/