#ifndef SPLU_UTILS_H
#define SPLU_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifndef NDEBUG
#define IF_DEBUG(...) __VA_ARGS__
#define IF_ELSE_DEBUG(smth, other_smth) smth
#else /*NDEBUG*/
#define IF_DEBUG(...)
#define IF_ELSE_DEBUG(smth, other_smth) other_smth
#endif /*NDEBUG*/


enum Opcode 
{
    OPCODE_UNKNOWN  = 0,
    OPCODE_PUSH     = 1,
    //TODO POP

    OPCODE_ADD      = 2,
    OPCODE_SUB      = 3,
    OPCODE_MUL      = 4,
    OPCODE_DIV      = 5,

    OPCODE_OUT      = 6,
    OPCODE_IN       = 7,

    OPCODE_HLT      = 8
};

typedef struct Cmnd
{
    enum Opcode opcode: 5;
    bool mem: 1;
    bool reg: 1;
    bool imm: 1;
} cmnd_t;

typedef int64_t operand_t;
#define INOUT_OPERAND_CODE "%ld"

/*!SECTION
PUSH 30
PUSH 70
ADD
PUSH 60
PUSH 40
SUB
DIV
PUSH 13
PUSH 3
SUB
ADD
OUT
HLT
*/

#endif /*SPLU_UTILS_H*/