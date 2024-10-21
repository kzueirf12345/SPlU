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

static enum AsmError push_jmp_(enum Opcode opcode, const char* operand_str,
                               instructs_t* const instructs, labels_t* const labels);

#define ASM_ERROR_HANDLE_(call_func, ...)                                                           \
        ASM_ERROR_HANDLE(call_func, fprintf(stderr, "Line: %zu\n", ip + 1); __VA_ARGS__)

enum AsmError assembly(const asm_code_t asm_code, instructs_t* const instructs)
{
    lassert(instructs, "");

    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;
    enum LabelsError labels_error_handler = LABELS_ERROR_SUCCESS;

    labels_t labels = {};
    const size_t LABELS_SIZE     = 2048;
    const size_t LABEL_NAME_SIZE = 256;

    if (labels_ctor(&labels, LABELS_SIZE, LABEL_NAME_SIZE))
    {
        fprintf(stderr, "Can't labels_ctor. Labels error: %s", 
                labels_strerror(labels_error_handler));
        return ASM_ERROR_LABELS;
    }

    bool is_hlt = false;
    size_t ip = 0;

    while(!is_hlt && ip < asm_code.comnds_size)
    {
        const char* const cmnd_str = asm_code.comnds[ip];
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
                                 labels_dtor(&labels););
                break;
            }

            case OPCODE_POP:
            {
                // fprintf(stderr, "lol\n");
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_instruct_with_operand_(OPCODE_POP, operand_str, instructs),
                                 labels_dtor(&labels););
                break;
            }

            //--------------------------------

            case OPCODE_ADD:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_ADD };
                instructs_push(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_SUB:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_SUB };
                instructs_push(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_MUL:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_MUL };
                instructs_push(instructs, &cmnd, 1);
                break;
            }
            case OPCODE_DIV:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_DIV };
                instructs_push(instructs, &cmnd, 1);
                break;
            }

            //--------------------------------

            case OPCODE_OUT:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_OUT };
                instructs_push(instructs, &cmnd, 1);
                break;
            }

            case OPCODE_IN:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_IN };
                instructs_push(instructs, &cmnd, 1);
                break;
            }

            //--------------------------------

            case OPCODE_JMP:
            {

                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JMP, operand_str, instructs, &labels), 
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JL:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JL, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JLE:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JLE, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JG:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JG, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JGE:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JGE, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JE:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JE, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }
            case OPCODE_JNE:
            {
                const char* operand_str = strchr(cmnd_str, '\0') + 1;
                ASM_ERROR_HANDLE_(push_jmp_(OPCODE_JNE, operand_str, instructs, &labels),
                                 labels_dtor(&labels););
                break;
            }

            case OPCODE_LABEL:
            {
                const char* const operand_str = cmnd_str + 1;
                if (!labels_push_unfinded(&labels, operand_str, strlen(operand_str), 
                                         instructs->counter))
                {
                    fprintf(stderr, "Can't push label %s in %zu line\n", operand_str, ip + 1);
                    labels_dtor(&labels);
                    return ASM_ERROR_LABELS;
                }
                break;
            }

            //--------------------------------

            case OPCODE_HLT:
            {
                cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = OPCODE_HLT };
                instructs_push(instructs, &cmnd, 1);
                is_hlt = true;
                break;
            }

            case OPCODE_UNKNOWN:
            {
                fprintf(stderr, "Incorrect command in %zu line\n", ip + 1);
                labels_dtor(&labels);
                return ASM_ERROR_INCORRECT_CMND;
            }

            default:
            {
                fprintf(stderr, "it's soo bad in %zu line!\n", ip + 1);
                labels_dtor(&labels);
                return ASM_ERROR_UNKNOWN;
            }
        }
        ++ip;
    }

    labels_dtor(&labels);

    return asm_error_handler;
}


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

    instructs_push(instructs, &cmnd, 1);

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

        instructs_push(instructs, &imm_num, sizeof(operand_t));
        instructs_push(instructs, &reg_num, sizeof(operand_t));
    }
    else if (cmnd.imm)
    {
        if (sscanf(operand_str, INOUT_OPERAND_CODE, &imm_num) != 1)
        {
            perror("Can't sscanf imm_num");
            return ASM_ERROR_INCORRECT_ARG;
        }
        instructs_push(instructs, &imm_num, sizeof(operand_t));
    }
    else if (cmnd.reg)
    {
        if (sscanf(operand_str, "R" INOUT_OPERAND_CODE, &reg_num) != 1)
        {
            perror("Can't sscanf reg_str");
            return ASM_ERROR_INCORRECT_ARG;
        }
        instructs_push(instructs, &reg_num, sizeof(operand_t));
    }
    else
    {
        fprintf(stderr, "Can't handle argument flag\n");
        return ASM_ERROR_INCORRECT_ARG;
    }

    return ASM_ERROR_SUCCESS;
}

static enum AsmError push_jmp_(enum Opcode opcode, const char* operand_str,
                               instructs_t* const instructs, labels_t* const labels)
{
    lassert(operand_str, "");
    lassert(instructs, "");
    lassert(labels, "");

    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;

    if (operand_str[0] == ':')
    {
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = opcode };
        instructs_push(instructs, &cmnd, 1);

        label_t* const finded_label = labels_find(labels, operand_str + 1);
        if (finded_label)
        {
            instructs_push(instructs, &finded_label->addr, sizeof(finded_label->addr));
        }
        else //TODO switch to fixat
        {
            fprintf(stderr, "Can't find label %s\n", operand_str + 1);
            return ASM_ERROR_LABELS;
        }
    }
    else
    {
        ASM_ERROR_HANDLE(push_instruct_with_operand_(opcode, operand_str, instructs));
    }

    return asm_error_handler;
}