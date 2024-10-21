#ifndef ASSEMBLER_SRC_FIXUP_FIXUP_H
#define ASSEMBLER_SRC_FIXUP_FIXUP_H

#include <stdio.h>

#include "stack_on_array/libstack.h"
#include "labels.h"

enum FixupError
{
    FIXUP_ERROR_SUCCESS         = 0,
    FIXUP_ERROR_STACK           = 1,
    FIXUP_ERROR_STANDARD_ERRNO  = 2,
    FIXUP_ERROR_FIND            = 3,
    FIXUP_ERROR_NFIND_LABEL     = 4,
};
static_assert(FIXUP_ERROR_SUCCESS == 0);

const char* fixup_strerror(const enum FixupError error);

#define FIXUP_ERROR_HANDLE(call_func, ...)                                                          \
    do {                                                                                            \
        fixup_error_handler = call_func;                                                            \
        if (fixup_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Fixup error: %s\n",                               \
                            fixup_strerror(fixup_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return fixup_error_handler;                                                             \
        }                                                                                           \
    } while(0)


typedef struct LabelCall
{
    char* name;
    size_t name_size;
    uint8_t* ip;
} label_call_t;

typedef struct Fixup
{
    stack_key_t* label_calls;
    size_t size;
} fixup_t;

enum FixupError fixup_ctor(fixup_t* const fixup, const size_t count_label_names);
void            fixup_dtor(fixup_t* const fixup);

enum FixupError fixup_push      (fixup_t* const fixup, label_call_t label_call);
enum FixupError fixup_processing(const fixup_t* const fixup, const labels_t labels);



#endif /*ASSEMBLER_SRC_FIXUP_FIXUP_H*/