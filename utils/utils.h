#ifndef SPLU_UTILS_H
#define SPLU_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "concole.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth
#else /*NDEBUG*/
#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth
#endif /*NDEBUG*/

#define MAX_OPCODE_BITS 5

enum Opcode 
{
    OPCODE_ZERO     = 0,
    OPCODE_PUSH     = 1,
    OPCODE_POP      = 2,

    OPCODE_ADD      = 3,
    OPCODE_SUB      = 4,
    OPCODE_MUL      = 5,
    OPCODE_DIV      = 6,

    OPCODE_OUT      = 7,
    OPCODE_IN       = 8,

    OPCODE_JMP      = 9,
    OPCODE_JL       = 10,
    OPCODE_JLE      = 11,
    OPCODE_JG       = 12,
    OPCODE_JGE      = 13,
    OPCODE_JE       = 14,
    OPCODE_JNE      = 15,

    OPCODE_LABEL    = 16,

    OPCODE_HLT      = 17,

    OPCODE_UNKNOWN  = 18
};
static_assert(OPCODE_UNKNOWN < (1 << MAX_OPCODE_BITS));

typedef struct Cmnd
{
    unsigned char opcode: MAX_OPCODE_BITS; //REVIEW сучка
    bool mem: 1;
    bool reg: 1;
    bool imm: 1;
} cmnd_t;
static_assert(sizeof(cmnd_t) == 1);

typedef int64_t operand_t;
#define INOUT_OPERAND_CODE "%ld"

static const size_t REGS_SIZE_    = 9;
static const size_t MEMORY_SIZE_ = 2048;

#endif /*SPLU_UTILS_H*/