#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "assembly.h"
#include "stack_on_array/libstack.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* asm_strerror(const enum AsmError input_error)
{
    switch (input_error)
    {
        CASE_ENUM_TO_STRING_(ASM_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(ASM_ERROR_STACK);
        CASE_ENUM_TO_STRING_(ASM_ERROR_DIV_BY_ZERO);
        CASE_ENUM_TO_STRING_(ASM_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(ASM_ERROR_INCORRECT_CMND);
        CASE_ENUM_TO_STRING_(ASM_ERROR_UNKNOWN);
    default:
        return "UNKNOWN_ASM_ERROR";
    }
    return "UNKNOWN_ASM_ERROR";
}
#undef CASE_ENUM_TO_STRING_

enum ComndCode 
{
    COMND_CODE_UNKNOWN  = 0,
    COMND_CODE_PUSH     = 1,

    COMND_CODE_ADD      = 10,
    COMND_CODE_SUB      = 11,
    COMND_CODE_MUL      = 12,
    COMND_CODE_DIV      = 13,

    COMND_CODE_IN       = 20,
    COMND_CODE_OUT      = 21,

    COMND_CODE_HLT      = 100
};

static enum ComndCode comnd_str_to_enum_(const char* const comnd_str);

#define stack_error_handle_(call_func, ...)                                                         \
    do {                                                                                            \
        stack_error_handler = call_func;                                                            \
        if (stack_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Stack error: %s\n",                               \
                            stack_strerror(stack_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return ASM_ERROR_STACK;                                                                 \
        }                                                                                           \
    } while(0)

enum AsmError assembly(const asm_code_t asm_code)
{
    enum StackError stack_error_handler = STACK_ERROR_SUCCESS;

    stack_key_t stack;
    stack_error_handle_(STACK_CTOR(&stack, sizeof(instruction_t), 10), 
                        stack_dtor(&stack););

    bool is_hlt = false;
    size_t IP = 0;
    size_t CP = 0;

    while(!is_hlt)
    {
        lassert(IP < asm_code.comnds_size * 10, "Check infinity cycle");

        const char* const comnd = asm_code.comnds[CP];
        enum ComndCode comnd_code = comnd_str_to_enum_(comnd);

        switch(comnd_code)
        {
            case COMND_CODE_PUSH:
            {
                instruction_t push_num = atoll(strchr(comnd, '\0') + 1);
                stack_error_handle_(stack_push(&stack, &push_num), stack_dtor(&stack););
                break;
            }

            case COMND_CODE_ADD:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t sum = first_num + second_num;
                stack_error_handle_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case COMND_CODE_SUB:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t sub = first_num - second_num;
                stack_error_handle_(stack_push(&stack, &sub), stack_dtor(&stack););
                break;
            }
            case COMND_CODE_MUL:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t mul = first_num * second_num;
                stack_error_handle_(stack_push(&stack, &mul), stack_dtor(&stack););
                break;
            }
            case COMND_CODE_DIV:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                if (second_num == 0)
                {
                    fprintf(stderr, "Can't div by zero\n");
                    stack_dtor(&stack);
                    return ASM_ERROR_DIV_BY_ZERO;
                }
                const instruction_t sum = first_num / second_num;
                stack_error_handle_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }

            case COMND_CODE_IN:
            {
                
                break;
            }
            case COMND_CODE_OUT:
            {
                lassert(stack_size(stack) >= 1, "");

                instruction_t out_num = 0;
                stack_error_handle_(stack_pop(&stack, &out_num), stack_dtor(&stack););

                if (printf("out: %ld\n", out_num) <= 0)
                {
                    perror("Can't printf out");
                    return ASM_ERROR_STANDARD_ERRNO;
                }

                break;
            }

            case COMND_CODE_HLT:
            {
                is_hlt = true;
                break;
            }

            case COMND_CODE_UNKNOWN:
            {
                fprintf(stderr, "Incorrect command\n");
                return ASM_ERROR_INCORRECT_CMND;
            }

            default:
            {
                fprintf(stderr, "it's soo bad!\n");
                return ASM_ERROR_UNKNOWN;
            }
        }
        ++CP;
    }

    stack_dtor(&stack);
    return ASM_ERROR_SUCCESS;
}
#undef stack_error_handle_


static enum ComndCode comnd_str_to_enum_(const char* const comnd_str)
{
    lassert(comnd_str, "");

    if (strcmp(comnd_str, "PUSH") == 0)
        return COMND_CODE_PUSH;

    if (strcmp(comnd_str, "ADD")  == 0)
        return COMND_CODE_ADD;
    if (strcmp(comnd_str, "SUB")  == 0)
        return COMND_CODE_SUB;
    if (strcmp(comnd_str, "MUL")  == 0)
        return COMND_CODE_MUL;
    if (strcmp(comnd_str, "DIV")  == 0)
        return COMND_CODE_DIV;

    if (strcmp(comnd_str, "IN")   == 0)
        return COMND_CODE_IN;
    if (strcmp(comnd_str, "OUT")  == 0)
        return COMND_CODE_OUT;

    if (strcmp(comnd_str, "HLT")  == 0)
        return COMND_CODE_HLT;
    
    return COMND_CODE_UNKNOWN;
}