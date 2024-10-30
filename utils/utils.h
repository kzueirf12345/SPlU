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
    OPCODE_MOD     = 7,

    OPCODE_OUT      = 8,
    OPCODE_IN       = 9,

    OPCODE_JMP      = 10,
    OPCODE_JL       = 11,
    OPCODE_JLE      = 12,
    OPCODE_JG       = 13,
    OPCODE_JGE      = 14,
    OPCODE_JE       = 15,
    OPCODE_JNE      = 16,

    OPCODE_LABEL    = 17,

    OPCODE_CALL     = 18,
    OPCODE_RET      = 19,

    OPCODE_DRAW     = 20,

    OPCODE_HLT      = 21,

    OPCODE_UNKNOWN  = 22
};
static_assert(OPCODE_UNKNOWN < (1 << MAX_OPCODE_BITS));

typedef struct Cmnd
{
    unsigned char opcode: MAX_OPCODE_BITS;
    bool mem: 1;
    bool reg: 1;
    bool imm: 1;
} cmnd_t;
static_assert(sizeof(cmnd_t) == 1);

typedef uint64_t operand_t;
#define INOUT_OPERAND_CODE "%lu"

static const size_t REGS_SIZE    = 9;

static const size_t MEMORY_HEIGHT = 30;
static const size_t MEMORY_WIDTH  = 60;
static const size_t MEMORY_SIZE = MEMORY_HEIGHT * MEMORY_WIDTH;

static const char MIN_VALID_OUTPUT_CHAR  = 34;
static const char MAX_VALID_OUTPUT_CHAR  = 126;
static const char NVALID_OUTPUT_CHAR_VAL = '.'; 

#endif /*SPLU_UTILS_H*/

/*!SECTION
# INIT
PUSH 1
POP R1

# cycle begin
:METKA
PUSH R1
PUSH R1
MUL

OUT
# increment
PUSH R1
PUSH 1
ADD
POP R1

# condition
PUSH R1
PUSH 10
JL :METKA

JMP :METKA21234

:METKA21234

HLT
*/