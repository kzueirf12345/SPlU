#ifndef PROCESSOR_SRC_PROCESSING_H
#define PROCESSOR_SRC_PROCESSING_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "../sdl/sdl.h"

enum ProcessorError
{
    PROCESSOR_ERROR_SUCCESS             = 0,
    PROCESSOR_ERROR_STACK               = 1,
    PROCESSOR_ERROR_DIV_BY_ZERO         = 2,
    PROCESSOR_ERROR_UNKNOWN_INSTRUCT    = 3,
    PROCESSOR_ERROR_STANDARD_ERRNO      = 4
};
static_assert(PROCESSOR_ERROR_SUCCESS == 0);

const char* processor_strerror(const enum ProcessorError error);

#define PROCESSOR_ERROR_HANDLE(call_func, ...)                                                      \
    do {                                                                                            \
        const enum ProcessorError processor_error_handler = call_func;                                                        \
        if (processor_error_handler)                                                                \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Asm_code error: %s\n",                            \
                            processor_strerror(processor_error_handler));                           \
            __VA_ARGS__                                                                             \
            return processor_error_handler;                                                         \
        }                                                                                           \
    } while(0)


typedef struct Processor
{
    uint8_t* instructs;
    size_t instructs_size;
    size_t ip;
    operand_t* regs;
    operand_t* memory;
} processor_t;

enum ProcessorError processor_ctor(processor_t* const processor, const char* const input_filename);
void                processor_dtor(processor_t* const processor);

enum ProcessorError processing(processor_t* const processor, const sdl_objs_t sdl_objs);

#endif /*PROCESSOR_SRC_PROCESSING_H*/

