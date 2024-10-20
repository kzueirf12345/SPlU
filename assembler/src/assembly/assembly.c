#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "assembly.h"
#include "../asm_code/asm_code.h"
#include "logger/liblogger.h"
#include "../instructions/instructions.h"
#include "utils.h"

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
        CASE_ENUM_TO_STRING_(ASM_ERROR_UNKNOWN);
    default:
        return "UNKNOWN_ASM_ERROR";
    }
    return "UNKNOWN_ASM_ERROR";
}
#undef CASE_ENUM_TO_STRING_

static enum Opcode comnd_str_to_enum_(const char* const comnd_str);

static enum AsmError push_instruct_with_operand_(enum Opcode opcode, const char* operand_str,
                                         instructs_t* const instructs);

enum AsmError assembly(const asm_code_t asm_code, instructs_t* const instructs)
{
    lassert(instructs, "");

    enum AsmError asm_error_handler = ASM_ERROR_SUCCESS;

    bool is_hlt = false;
    size_t ip = 0;

    while(!is_hlt && ip < asm_code.comnds_size)
    {
        const char* const comnd_str = asm_code.comnds[ip];
        enum Opcode comnd_code = comnd_str_to_enum_(comnd_str);

        switch(comnd_code)
        {
            case OPCODE_PUSH:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_PUSH, operand_str, instructs));
                break;
            }

            case OPCODE_POP:
            {
                // fprintf(stderr, "lol\n");
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_POP, operand_str, instructs));
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
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JMP, operand_str, instructs));
                break;
            }
            case OPCODE_JL:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JL, operand_str, instructs));
                break;
            }
            case OPCODE_JLE:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JLE, operand_str, instructs));
                break;
            }
            case OPCODE_JG:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JG, operand_str, instructs));
                break;
            }
            case OPCODE_JGE:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JGE, operand_str, instructs));
                break;
            }
            case OPCODE_JE:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JE, operand_str, instructs));
                break;
            }
            case OPCODE_JNE:
            {
                const char* operand_str = strchr(comnd_str, '\0') + 1;
                ASM_ERROR_HANDLE(push_instruct_with_operand_(OPCODE_JNE, operand_str, instructs));
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
                fprintf(stderr, "Incorrect command\n");
                return ASM_ERROR_INCORRECT_CMND;
            }

            default:
            {
                fprintf(stderr, "it's soo bad!\n");
                return ASM_ERROR_UNKNOWN;
            }
        }
        ++ip;
    }

    return asm_error_handler;
}
#undef stack_error_handle_


static enum Opcode comnd_str_to_enum_(const char* const comnd_str)
{
    lassert(comnd_str, "");

    if (strcmp(comnd_str, "PUSH") == 0)
        return OPCODE_PUSH;
    if (strcmp(comnd_str, "POP")  == 0)
        return OPCODE_POP;

    if (strcmp(comnd_str, "ADD")  == 0)
        return OPCODE_ADD;
    if (strcmp(comnd_str, "SUB")  == 0)
        return OPCODE_SUB;
    if (strcmp(comnd_str, "MUL")  == 0)
        return OPCODE_MUL;
    if (strcmp(comnd_str, "DIV")  == 0)
        return OPCODE_DIV;

    if (strcmp(comnd_str, "OUT")  == 0)
        return OPCODE_OUT;
    if (strcmp(comnd_str, "IN")   == 0)
        return OPCODE_IN;

    if (strcmp(comnd_str, "JMP")  == 0)
        return OPCODE_JMP;
    if (strcmp(comnd_str, "JL")   == 0)
        return OPCODE_JL;
    if (strcmp(comnd_str, "JLE")  == 0)
        return OPCODE_JLE;
    if (strcmp(comnd_str, "JG")   == 0)
        return OPCODE_JG;
    if (strcmp(comnd_str, "JGE")  == 0)
        return OPCODE_JGE;
    if (strcmp(comnd_str, "JE")   == 0)
        return OPCODE_JE;
    if (strcmp(comnd_str, "JNE")  == 0)
        return OPCODE_JNE;

 
    if (strcmp(comnd_str, "HLT")  == 0)
        return OPCODE_HLT;
    
    return OPCODE_UNKNOWN;
}

static enum AsmError push_instruct_with_operand_(enum Opcode opcode, const char* operand_str,
                                                instructs_t* const instructs)
{
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