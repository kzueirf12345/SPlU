#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "assembly.h"
#include "../asm_code/asm_code.h"
#include "logger/liblogger.h"
#include "../instructions/instructions.h"
#include "utils.h"
#include "../labels/labels.h"
#include "../labels/fixup.h"

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
        CASE_ENUM_TO_STRING_(ASM_ERROR_INCORRECT_ARG);
        CASE_ENUM_TO_STRING_(ASM_ERROR_LABELS);
        CASE_ENUM_TO_STRING_(ASM_ERROR_FIXUP);
        CASE_ENUM_TO_STRING_(ASM_ERROR_UNKNOWN);
    default:
        return "UNKNOWN_ASM_ERROR";
    }
    return "UNKNOWN_ASM_ERROR";
}
#undef CASE_ENUM_TO_STRING_


static enum Opcode comnd_str_to_enum_(const char* const cmnd_str);
static enum AsmError push_instruct_with_operand_(enum Opcode opcode, const char* operand_str,
                                                 instructs_t* const instructs);
static enum AsmError push_jmp_(enum Opcode opcode, char* operand_str,
                               instructs_t* const instructs, const labels_t labels,
                               fixup_t* const fixup);

#define FIXUP_ERROR_HANDLE_(call_func, ...)                                                         \
    do {                                                                                            \
        const enum FixupError fixup_error_handler = call_func;                                      \
        if (fixup_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Fixup error: %s\n",                               \
                            fixup_strerror(fixup_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return ASM_ERROR_FIXUP;                                                                 \
        }                                                                                           \
    } while(0)

#define LABELS_ERROR_HANDLE_(call_func, ...)                                                        \
    do {                                                                                            \
        const enum LabelsError labels_error_handler = call_func;                                    \
        if (labels_error_handler)                                                                   \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Labels error: %s\n",                              \
                            labels_strerror(labels_error_handler));                                 \
            __VA_ARGS__                                                                             \
            return ASM_ERROR_LABELS;                                                                \
        }                                                                                           \
    } while(0)

#define ASM_ERROR_HANDLE_(call_func, ...)                                                           \
        ASM_ERROR_HANDLE(call_func, fprintf(stderr, "Line: %zu\n", ip + 1); __VA_ARGS__)

#define COMAND_HANDLE(cmd_name, do_it, ndo_it)                                                      \
        case OPCODE_##cmd_name:                                                                     \
        {                                                                                           \
            do_it                                                                                   \
            break;                                                                                  \
        }   

enum AsmError assembly(const asm_code_t asm_code, instructs_t* const instructs)
{
    lassert(instructs, "");

    const size_t LABELS_SIZE = 2048;

    labels_t labels = {};
    LABELS_ERROR_HANDLE_(labels_ctor(&labels, 1), 
                         labels_dtor(&labels););

    fixup_t fixup = {};
    FIXUP_ERROR_HANDLE_(fixup_ctor(&fixup, LABELS_SIZE), 
                        fixup_dtor(&fixup); labels_dtor(&labels););

    size_t ip = 0;

    while(ip < asm_code.comnds_size)
    {
        char* const cmnd_str = asm_code.comnds[ip];
        enum Opcode comnd_code = comnd_str_to_enum_(cmnd_str);

        switch(comnd_code)
        {
            #include "commands.h"

            default:
            {
                fprintf(stderr, "it's soo bad in %zu line!\n", ip + 1);
                fixup_dtor(&fixup);
                labels_dtor(&labels);
                return ASM_ERROR_UNKNOWN;
            }
        }
        ++ip;
    }

    FIXUP_ERROR_HANDLE_(fixup_processing(&fixup, instructs, labels), 
                        fixup_dtor(&fixup); labels_dtor(&labels););

    fixup_dtor(&fixup);
    labels_dtor(&labels);

    return ASM_ERROR_SUCCESS;
}
#undef ASM_ERROR_HANDLE_
#undef LABELS_ERROR_HANDLE_
#undef COMAND_HANDLE

#define COMAND_HANDLE(cmd_name, ...) \
        if (strcmp(cmnd_str, #cmd_name) == 0) return OPCODE_##cmd_name;
static enum Opcode comnd_str_to_enum_(const char* const cmnd_str)
{
    lassert(cmnd_str, "");

    static const char COMMENT_SYMBOL = ';';

    if (strcmp(cmnd_str, "") == 0 || cmnd_str[0] == COMMENT_SYMBOL) return OPCODE_ZERO;

    if (cmnd_str[0] == ':') return OPCODE_LABEL;

    #include "commands.h"
    
    return OPCODE_UNKNOWN;
}
#undef COMAND_HANDLE

static enum AsmError fill_cmnd_   (cmnd_t* const cmnd, enum Opcode opcode, 
                                   const char** const operand_str);
static enum AsmError handle_cmnd_ (cmnd_t* const cmnd, instructs_t* const instructs, 
                                   const char* const operand_str);

static enum AsmError push_instruct_with_operand_(enum Opcode opcode, const char* operand_str,
                                                instructs_t* const instructs)
{
    lassert(operand_str, "");
    lassert(instructs, "");

    cmnd_t cmnd = {};

    ASM_ERROR_HANDLE(fill_cmnd_(&cmnd, opcode, &operand_str));

    instructs_push_back(instructs, &cmnd, 1);

    ASM_ERROR_HANDLE(handle_cmnd_(&cmnd, instructs, operand_str));

    return ASM_ERROR_SUCCESS;
}


static enum AsmError fill_cmnd_(cmnd_t* const cmnd, enum Opcode opcode, 
                                const char** const operand_str)
{
    lassert(cmnd, "");
    lassert(operand_str, "");

    cmnd->opcode = opcode;

    if ((*operand_str)[0] == '[' && (cmnd->opcode == OPCODE_PUSH || cmnd->opcode == OPCODE_POP))
    {
        cmnd->mem = true;
        ++*operand_str;
    }

    if (isdigit((*operand_str)[0])) 
    {
        cmnd->imm = true;

        if (!cmnd->mem && cmnd->opcode == OPCODE_POP)
        {
            fprintf(stderr, "Incorrect pop argument\n");
            return ASM_ERROR_INCORRECT_ARG;
        }

        if (strchr((*operand_str), '+'))
        {
            cmnd->reg = true;
        }
    }
    else if (isalpha((*operand_str)[0]) && !strchr((*operand_str), '+')) 
    {
        cmnd->reg = true;
    }
    else
    {
        fprintf(stderr, "Can't parse argument\n");
        return ASM_ERROR_INCORRECT_ARG;
    }

    return ASM_ERROR_SUCCESS;
}

static enum AsmError handle_cmnd_ (cmnd_t* const cmnd, instructs_t* const instructs, 
                                   const char* const operand_str)
{
    lassert(cmnd, "");
    lassert(instructs, "");
    lassert(operand_str, "");

    operand_t imm_num = 0;
    operand_t reg_num = 0;

    if (cmnd->imm && cmnd->reg)
    {
        if (sscanf(operand_str, INOUT_OPERAND_CODE "+R" INOUT_OPERAND_CODE, 
                                &imm_num, &reg_num) != 2)
        {
            perror("Can't sscanf imm_num and reg_str");
            return ASM_ERROR_INCORRECT_ARG;
        }

        instructs_push_back(instructs, &imm_num, sizeof(operand_t));
        instructs_push_back(instructs, &reg_num, sizeof(operand_t));
    }
    else if (cmnd->imm)
    {
        if (sscanf(operand_str, INOUT_OPERAND_CODE, &imm_num) <= 0)
        {
            perror("Can't sscanf imm_num");
            return ASM_ERROR_INCORRECT_ARG;
        }
        instructs_push_back(instructs, &imm_num, sizeof(operand_t));
    }
    else if (cmnd->reg)
    {
        if (sscanf(operand_str, "R" INOUT_OPERAND_CODE, &reg_num) != 1)
        {
            perror("Can't sscanf reg_str");
            return ASM_ERROR_INCORRECT_ARG;
        }
        instructs_push_back(instructs, &reg_num, sizeof(operand_t));
    }
    else
    {
        fprintf(stderr, "Can't handle argument flag\n");
        return ASM_ERROR_INCORRECT_ARG;
    }

    return ASM_ERROR_SUCCESS;
}

static enum AsmError push_jmp_(enum Opcode opcode, char* operand_str,
                               instructs_t* const instructs, const labels_t labels,
                               fixup_t* const fixup)
{
    lassert(operand_str, "");
    lassert(instructs, "");

    if (operand_str[0] == ':')
    {
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = opcode };
        instructs_push_back(instructs, &cmnd, 1);

        label_t* const finded_label = labels_find(labels, operand_str + 1);
        if (finded_label)
        {
            instructs_push_back(instructs, &finded_label->addr, sizeof(finded_label->addr));
        }
        else
        {
            const label_call_t label_call = {.name = operand_str + 1,
                                             .ip = instructs->counter};

            FIXUP_ERROR_HANDLE_(fixup_push(fixup, label_call));

            size_t temp_zero_elem = 0;
            instructs_push_back(instructs, &temp_zero_elem, sizeof(temp_zero_elem));
        }
    }
    else
    {
        ASM_ERROR_HANDLE(push_instruct_with_operand_(opcode, operand_str, instructs));
    }

    return ASM_ERROR_SUCCESS;
}
#undef FIXUP_ERROR_HANDLE_