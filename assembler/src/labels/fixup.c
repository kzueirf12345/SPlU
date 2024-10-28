#include <assert.h>
#include <stdio.h>

#include "fixup.h"
#include "stack_on_array/libstack.h"
#include "logger/liblogger.h"
#include "../instructions/instructions.h"
#include "utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* fixup_strerror(const enum FixupError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(FIXUP_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(FIXUP_ERROR_STACK);
        CASE_ENUM_TO_STRING_(FIXUP_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(FIXUP_ERROR_FIND);
        CASE_ENUM_TO_STRING_(FIXUP_ERROR_NFIND_LABEL);
    default:
        return "UNKNOWN_FIXUP_ERROR";
    }
    return "UNKNOWN_FIXUP_ERROR";
}
#undef CASE_ENUM_TO_STRING_

#define STACK_ERROR_HANDLE_(call_func, ...)                                                         \
    do {                                                                                            \
        const enum StackError stack_error_handler = call_func;                                      \
        if (stack_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Stack error: %s\n",                               \
                            stack_strerror(stack_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return FIXUP_ERROR_STACK;                                                               \
        }                                                                                           \
    } while(0)

enum FixupError fixup_ctor(fixup_t* const fixup, const size_t count_label_names)
{
    lassert(fixup, "");
    lassert(count_label_names, "");

    fixup->label_calls = calloc(count_label_names, sizeof(*fixup->label_calls));
    if (!fixup->label_calls)
    {
        perror("Can't calloc fixup->label_calls");
        return FIXUP_ERROR_STANDARD_ERRNO;
    }
    fixup->size = count_label_names;

    for (size_t ind = 0; ind < fixup->size; ++ind)
    {
        fixup->label_calls[ind] = 0;
        STACK_ERROR_HANDLE_(STACK_CTOR(&fixup->label_calls[ind], sizeof(label_call_t), 0), 
                            stack_dtor(&fixup->label_calls[ind]););
    }

    return FIXUP_ERROR_SUCCESS;
}

void fixup_dtor(fixup_t* const fixup)
{
    lassert(fixup, "");
    for (size_t ind = 0; ind < fixup->size; ++ind)
    {
        stack_dtor(&fixup->label_calls[ind]);        
    }
    free(fixup->label_calls); IF_DEBUG(fixup->label_calls = NULL;)
    IF_DEBUG(fixup->size = 0;)
}


stack_key_t* fixup_find_(fixup_t* const fixup, const char* const label_name);

enum FixupError fixup_push(fixup_t* const fixup, label_call_t label_call)
{
    lassert(fixup, "");
    lassert(label_call.name, "");

    stack_key_t* stack_ptr = fixup_find_(fixup, label_call.name);
    if (stack_ptr == NULL)
    {
        fprintf(stderr, "Can't fixup_find_\n");
        return FIXUP_ERROR_FIND;
    }

    STACK_ERROR_HANDLE_(stack_push(stack_ptr, &label_call));

    return FIXUP_ERROR_SUCCESS;
}

stack_key_t* fixup_find_(fixup_t* const fixup, const char* const label_name)
{
    lassert(fixup, "");
    lassert(fixup->label_calls, "");
    lassert(fixup->size, "");
    lassert(label_name, "");

    size_t ind = 0;
    for(; ind < fixup->size && !stack_is_empty(fixup->label_calls[ind]); ++ind)
    {
        label_call_t back_label_call = {};

        if (stack_back(fixup->label_calls[ind], &back_label_call))
        {
            fprintf(stderr, "Can't stack_back label_call.\nInd: %zu. label_name: %s\n",
                    ind, label_name);
            return NULL;
        }

        lassert               (back_label_call.name, "");
        if (strcmp(label_name, back_label_call.name) == 0)
        {
            return fixup->label_calls + ind;
        }
    }

    if (ind >= fixup->size)
    {
        fprintf(stderr, "Fixup stack buffer overflow. Label name: %s\n", label_name);
        return NULL;
    }

    return fixup->label_calls + ind;
}


enum FixupError fixup_processing(const fixup_t* const fixup, instructs_t* const instructs, 
                                 const labels_t labels)
{
    lassert(fixup, "");
    lassert(fixup->label_calls, "");
    lassert(fixup->size, "");
    lassert(labels.labels, "");
    lassert(labels.size, "");
    lassert(instructs, "");
    lassert(instructs->data, "");
    lassert(instructs->size, "");

    for (size_t stack_ind = 0; 
         stack_ind < fixup->size && !stack_is_empty(fixup->label_calls[stack_ind]); 
         ++stack_ind)
    {
        stack_key_t stack = fixup->label_calls[stack_ind];

        label_call_t stack_back_elem = {};
        STACK_ERROR_HANDLE_(stack_back(stack, &stack_back_elem));

        const char* const label_name = stack_back_elem.name;
        const label_t* label_ptr = labels_find(labels, label_name);
        if (!label_ptr)
        {
            fprintf(stderr, "Can't find label %s", label_name);
            return FIXUP_ERROR_NFIND_LABEL;
        }
        const size_t label_addr = label_ptr->addr;


        while (!stack_is_empty(stack))
        {
            STACK_ERROR_HANDLE_(stack_pop(&stack, &stack_back_elem));

            static_assert(sizeof(operand_t) == sizeof(size_t));

            memcpy(&instructs->data[stack_back_elem.ip], &label_addr, sizeof(label_addr));
        }
    }

    return FIXUP_ERROR_SUCCESS;
}
#undef STACK_ERROR_HANDLE_