#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

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
        fixup_error_handler = call_func;                                                            \
        if (fixup_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Fixup error: %s\n",                               \
                            fixup_strerror(fixup_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return ASM_ERROR_FIXUP;                                                                 \
        }                                                                                           \
    } while(0)

#define ASM_ERROR_HANDLE_(call_func, ...)                                                           \
        ASM_ERROR_HANDLE(call_func, fprintf(stderr, "Line: %zu\n", ip + 1); __VA_ARGS__)

enum AsmError assembly(const asm_code_t asm_code, instructs_t* const instructs)
{
    lassert(instructs, "");

    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;
    enum LabelsError labels_error_handler = LABELS_ERROR_SUCCESS;
    enum FixupError fixup_error_handler = FIXUP_ERROR_SUCCESS;

    const size_t LABELS_SIZE     = 2047; //FIXME lol 2048+ not work
    const size_t LABEL_NAME_SIZE = 256;

    labels_t labels = {};
    if (labels_ctor(&labels, LABELS_SIZE, LABEL_NAME_SIZE))
    {
        fprintf(stderr, "Can't labels_ctor. Labels error: %s", 
                labels_strerror(labels_error_handler));
        return ASM_ERROR_LABELS;
    }

    fixup_t fixup = {};
    FIXUP_ERROR_HANDLE_(fixup_ctor(&fixup, LABELS_SIZE), labels_dtor(&labels););

    bool is_hlt = false;
    size_t ip = 0;

    while(!is_hlt && ip < asm_code.comnds_size)
    {
        char* const cmnd_str = asm_code.comnds[ip];
        enum Opcode comnd_code = comnd_str_to_enum_(cmnd_str);

        switch(comnd_code)
        {
            case OPCODE_ZERO:
            {
                // прям как мой телефон - nothing
                break;
            }

            case OPCODE_PUSH:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_instruct_with_operand_(OPCODE_PUSH, operand_str, instructs),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }

            case OPCODE_POP:
            {
                // fprintf(stderr, "lol\n");
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_instruct_with_operand_(OPCODE_POP, operand_str, instructs),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }

            //--------------------------------

            case OPCODE_ADD:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_ADD };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_SUB:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_SUB };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_MUL:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_MUL };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_DIV:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_DIV };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }

            //--------------------------------

            case OPCODE_OUT:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_OUT };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }

            case OPCODE_IN:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_IN };
                instructs_push_back(instructs, &cmnd, 1);
                break;
            }

            //--------------------------------

            case OPCODE_JMP:
            {

                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JMP, operand_str, instructs, labels, &fixup), 
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JL:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JL, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JLE:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JLE, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JG:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JG, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JGE:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JGE, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JE:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JE, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }
            case OPCODE_JNE:
            {
                char* const operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JNE, operand_str, instructs, labels, &fixup),
                                 fixup_dtor(&fixup); labels_dtor(&labels););
                break;
            }

            case OPCODE_LABEL:
            {
                char* const operand_str = cmnd_str + 1;
                const label_t label = {.name = operand_str, .name_size = strlen(operand_str), 
                                       .addr = instructs->counter};
                if (!labels_push_unfinded(&labels, label))
                {
                    fprintf(stderr, "Can't push label %s in %zu line\n", operand_str, ip + 1);
                    fixup_dtor(&fixup);
                    labels_dtor(&labels);
                    return ASM_ERROR_LABELS;
                }
                break;
            }

            //--------------------------------

            case OPCODE_HLT:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_HLT };
                instructs_push_back(instructs, &cmnd, 1);
                is_hlt = true;
                break;
            }

            case OPCODE_UNKNOWN:
            {
                fprintf(stderr, "Incorrect command in %zu line\n", ip + 1);
                fixup_dtor(&fixup);
                labels_dtor(&labels);
                return ASM_ERROR_INCORRECT_CMND;
            }

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
    FIXUP_ERROR_HANDLE_(fixup_processing(&fixup, labels), 
                        fixup_dtor(&fixup); labels_dtor(&labels););

    fixup_dtor(&fixup);
    labels_dtor(&labels);

    return asm_error_handler;
}
#undef ASM_ERROR_HANDLE_

static enum Opcode comnd_str_to_enum_(const char* const cmnd_str)
{
    lassert(cmnd_str, "");

    if (strcmp(cmnd_str, "") == 0 || cmnd_str[0] == '#')
        return OPCODE_ZERO;

    if (strcmp(cmnd_str, "PUSH") == 0)
        return OPCODE_PUSH;
    if (strcmp(cmnd_str, "POP")  == 0)
        return OPCODE_POP;

    if (strcmp(cmnd_str, "ADD")  == 0)
        return OPCODE_ADD;
    if (strcmp(cmnd_str, "SUB")  == 0)
        return OPCODE_SUB;
    if (strcmp(cmnd_str, "MUL")  == 0)
        return OPCODE_MUL;
    if (strcmp(cmnd_str, "DIV")  == 0)
        return OPCODE_DIV;

    if (strcmp(cmnd_str, "OUT")  == 0)
        return OPCODE_OUT;
    if (strcmp(cmnd_str, "IN")   == 0)
        return OPCODE_IN;

    if (strcmp(cmnd_str, "JMP")  == 0)
        return OPCODE_JMP;
    if (strcmp(cmnd_str, "JL")   == 0)
        return OPCODE_JL;
    if (strcmp(cmnd_str, "JLE")  == 0)
        return OPCODE_JLE;
    if (strcmp(cmnd_str, "JG")   == 0)
        return OPCODE_JG;
    if (strcmp(cmnd_str, "JGE")  == 0)
        return OPCODE_JGE;
    if (strcmp(cmnd_str, "JE")   == 0)
        return OPCODE_JE;
    if (strcmp(cmnd_str, "JNE")  == 0)
        return OPCODE_JNE;
    
    if (cmnd_str[0] == ':')
        return OPCODE_LABEL;

 
    if (strcmp(cmnd_str, "HLT")  == 0)
        return OPCODE_HLT;
    
    return OPCODE_UNKNOWN;
}

static enum AsmError push_instruct_with_operand_(enum Opcode opcode, const char* operand_str,
                                                instructs_t* const instructs)
{
    lassert(operand_str, "");
    lassert(instructs, "");

    cmnd_t cmnd = {};
    cmnd.opcode = opcode;

    if (operand_str[0] == '[' && (cmnd.opcode == OPCODE_PUSH || cmnd.opcode == OPCODE_POP))
    {
        cmnd.mem = true;
        ++operand_str;
    }

    if (isdigit(operand_str[0])) 
    {
        cmnd.imm = true;

        if (!cmnd.mem && cmnd.opcode == OPCODE_POP)
        {
            fprintf(stderr, "Incorrect pop argument\n");
            return ASM_ERROR_INCORRECT_ARG;
        }

        if (strchr(operand_str, '+'))
        {
            cmnd.reg = true;
        }
    }
    else if (isalpha(operand_str[0]) && !strchr(operand_str, '+')) 
    {
        cmnd.reg = true;
    }
    else
    {
        fprintf(stderr, "Can't parse argument\n");
        return ASM_ERROR_INCORRECT_ARG;
    }

    instructs_push_back(instructs, &cmnd, 1);

    operand_t imm_num = 0;
    operand_t reg_num = 0;

    if (cmnd.imm & cmnd.reg)
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
    else if (cmnd.imm)
    {
        if (sscanf(operand_str, INOUT_OPERAND_CODE, &imm_num) != 1)
        {
            perror("Can't sscanf imm_num");
            return ASM_ERROR_INCORRECT_ARG;
        }
        instructs_push_back(instructs, &imm_num, sizeof(operand_t));
    }
    else if (cmnd.reg)
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

    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;
    enum FixupError fixup_error_handler = FIXUP_ERROR_SUCCESS;

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
                                             .name_size = strlen(operand_str + 1),
                                             .ip = instructs->data + instructs->counter};

            FIXUP_ERROR_HANDLE_(fixup_push(fixup, label_call));

            size_t temp_zero_elem = 0;
            instructs_push_back(instructs, &temp_zero_elem, sizeof(temp_zero_elem));
        }
    }
    else
    {
        ASM_ERROR_HANDLE(push_instruct_with_operand_(opcode, operand_str, instructs));
    }

    return asm_error_handler;
}
#undef FIXUP_ERROR_HANDLE_