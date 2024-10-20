#include <stdint.h>
#include <stdbool.h>

#include "processing.h"
#include "logger/liblogger.h"
#include "stack_on_array/libstack.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* processor_strerror(const enum ProcessorError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(PROCESSOR_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(PROCESSOR_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(PROCESSOR_ERROR_STACK);
        CASE_ENUM_TO_STRING_(PROCESSOR_ERROR_DIV_BY_ZERO);
        CASE_ENUM_TO_STRING_(PROCESSOR_ERROR_UNKNOWN_INSTRUCT);
    default:
        return "UNKNOWN_PROCESSOR_ERROR";
    }
    return "UNKNOWN_PROCESSOR_ERROR";
}
#undef CASE_ENUM_TO_STRING_


static enum ProcessorError fill_processor_size_(processor_t* const processor, 
                                                FILE** const input_file);
static enum ProcessorError fill_processor_data_(processor_t* const processor,
                                                FILE** const input_file);

enum ProcessorError processor_ctor(processor_t* const processor, const char* const input_filename)
{
    lassert(processor, "");
    lassert(input_filename, "");

    enum ProcessorError processor_error_handler = PROCESSOR_ERROR_SUCCESS;

    FILE* input_file = fopen(input_filename, "rb");
    if (!input_file)
    {
        perror("Can't fopen input file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }
    setbuf(input_file, NULL);

    PROCESSOR_ERROR_HANDLE(fill_processor_size_(processor, &input_file));
    PROCESSOR_ERROR_HANDLE(fill_processor_data_(processor, &input_file));

    if (fclose(input_file))
    {
        perror("Can't fclose input file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }
    IF_DEBUG(input_file = NULL;)

    processor->ip = 0;

    processor->memory = calloc(MEMORY_SIZE_, sizeof(*processor->memory));
    if (!processor->memory)
    {
        perror("Can't calloc processor->memory");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    processor->regs = calloc(REGS_SIZE_, sizeof(*processor->regs));
    if (!processor->regs)
    {
        perror("Can't calloc processor->regs");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    return PROCESSOR_ERROR_SUCCESS;
}

void processor_dtor(processor_t* const processor)
{
    lassert(processor, "");

    free(processor->instructs); IF_DEBUG(processor->instructs = NULL;)
    free(processor->memory);    IF_DEBUG(processor->memory    = NULL;)
    free(processor->regs);      IF_DEBUG(processor->regs      = NULL;) 

    IF_DEBUG(processor->instructs_size = 0;)
    IF_DEBUG(processor->ip             = 0;)
}

static enum ProcessorError fill_processor_size_(processor_t* const processor, 
                                                FILE** const input_file) 
{
    lassert(processor, "");
    lassert(input_file, "");
    lassert(*input_file, "");

    if (fseek(*input_file, 0, SEEK_END))
    {
        perror("Can't fseek to end input file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    long instructs_size = 0;
    if ((instructs_size = ftell(*input_file)) < 0)
    {
        perror("Can't ftell input_file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }
    processor->instructs_size = (size_t)instructs_size;


    if (fseek(*input_file, 0, SEEK_SET))
    {
        perror("Can't fseek to start input file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    return PROCESSOR_ERROR_SUCCESS;
}

static enum ProcessorError fill_processor_data_(processor_t* const processor, FILE** const input_file)
{
    lassert(input_file, "");
    lassert(*input_file, "");
    lassert(processor, "");
    lassert(processor->instructs_size, "");

    processor->instructs = (uint8_t*)calloc(processor->instructs_size, sizeof(*processor->instructs));
    if (!processor->instructs)
    {
        perror("Can't calloc memory for processor->instructs");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    if (fread(processor->instructs, 1, processor->instructs_size, *input_file) 
        != processor->instructs_size)
    {
        perror("Can't fread into input file");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    return PROCESSOR_ERROR_SUCCESS;
}


static void lassert_processor_init_(const processor_t* const processor);
static operand_t* GetOperandAddr_(cmnd_t cmnd, processor_t* const processor);

#define STACK_ERROR_HANDLE_(call_func, ...)                                                         \
    do {                                                                                            \
        if (call_func)                                                                              \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func "\n");                                              \
            __VA_ARGS__                                                                             \
            return PROCESSOR_ERROR_STACK;                                                           \
        }                                                                                           \
    } while(0)

enum ProcessorError processing(processor_t* const processor)
{
    lassert_processor_init_(processor);

    stack_key_t stack = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack, sizeof(operand_t), 0), stack_dtor(&stack););

    bool is_hlt = false;

    while (!is_hlt && processor->ip < processor->instructs_size)
    {
        // fprintf(stderr, "sizeof(cmnd_t): %zu\n", sizeof(cmnd_t));
        // fprintf(stderr, "ip: %zu\n", processor->ip);
        const cmnd_t cmnd = *(cmnd_t*)(processor->instructs + processor->ip);
        // fprintf(stderr, "processor->instructs: 0b%b\n", *(processor->instructs + processor->ip));
        // fprintf(stderr, "cmnd: 0b%b\n", cmnd);
        ++processor->ip;

        // fprintf(stderr, "opcode: %d\n", cmnd.opcode);

        // STACK_DUMB(stack, NULL);

        switch ((enum Opcode)cmnd.opcode)
        {
            case OPCODE_PUSH:
            {
                STACK_ERROR_HANDLE_(stack_push(&stack, GetOperandAddr_(cmnd, processor)),
                                    stack_dtor(&stack););
                break;
            }
            case OPCODE_ADD:
            {
                lassert(stack_size(stack) >= 2, "");

                operand_t first_num = 0, second_num = 0;
                STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const operand_t sum = first_num + second_num;
                STACK_ERROR_HANDLE_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case OPCODE_SUB:
            {
                lassert(stack_size(stack) >= 2, "");

                operand_t first_num = 0, second_num = 0;
                STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const operand_t sum = first_num - second_num;
                STACK_ERROR_HANDLE_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case OPCODE_MUL:
            {
                lassert(stack_size(stack) >= 2, "");

                operand_t first_num = 0, second_num = 0;
                STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const operand_t sum = first_num * second_num;
                STACK_ERROR_HANDLE_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case OPCODE_DIV:
            {
                lassert(stack_size(stack) >= 2, "");

                operand_t first_num = 0, second_num = 0;
                STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                if (second_num == 0)
                {
                    fprintf(stderr, "Can't div by zero\n");
                    stack_dtor(&stack);
                    return PROCESSOR_ERROR_DIV_BY_ZERO;
                }
                const operand_t sum = first_num / second_num;
                STACK_ERROR_HANDLE_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case OPCODE_OUT:
            {
                lassert(stack_size(stack) >= 1, "");

                operand_t out_num = 0;
                STACK_ERROR_HANDLE_(stack_pop(&stack, &out_num), stack_dtor(&stack););

                if (printf("Out: " INOUT_OPERAND_CODE "\n", out_num) <= 0)
                {
                    perror("Can't printf out_num");
                    stack_dtor(&stack);
                    return PROCESSOR_ERROR_STANDARD_ERRNO;
                }
                break;
            }
            case OPCODE_IN:
            {
                operand_t in_num = 0;
                if (scanf("In: " INOUT_OPERAND_CODE, &in_num) != 1)
                {
                    perror("Can't scanf in_num");
                    stack_dtor(&stack);
                    return PROCESSOR_ERROR_STANDARD_ERRNO;
                }
                STACK_ERROR_HANDLE_(stack_push(&stack, &in_num), stack_dtor(&stack););
                break;
            }
            case OPCODE_HLT:
            {
#ifndef NDEBUG
                if (!stack_is_empty(stack))
                    fprintf(stderr, "When program is end, stack wasn't empty\n");
#endif /*NDEBUG*/
                is_hlt = true;
                break;
            }
            case OPCODE_UNKNOWN:
            {
                fprintf(stderr, "UNKNOWN INSTRUCT\n");
                stack_dtor(&stack);
                return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
            }
            default:
            {
                fprintf(stderr, "Pizdec\n");
                stack_dtor(&stack);
                return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
            }
        }
    }

    stack_dtor(&stack);
    return PROCESSOR_ERROR_SUCCESS;
}
#undef STACK_ERROR_HANDLE_

static void lassert_processor_init_(const processor_t* const processor)
{
    lassert(processor, "");
    lassert(processor->instructs, "");
    lassert(processor->instructs_size, "");
    lassert(processor->memory, "");
    lassert(processor->regs, "");
}

static operand_t* GetOperandAddr_(cmnd_t cmnd, processor_t* const processor)
{
    lassert(processor, "");
    lassert(cmnd.imm | cmnd.mem | cmnd.reg, "");

    operand_t* operand_addr = NULL;

    if (cmnd.imm & cmnd.reg)
    {
        const operand_t imm_num = *(operand_t*)(processor->instructs + processor->ip);
        processor->ip += sizeof(operand_t);
        const operand_t reg_num = processor->regs[*(operand_t*)(processor->instructs + processor->ip)];
        processor->ip += sizeof(operand_t);

        static operand_t save_num_ = 0;
        save_num_ = imm_num + reg_num;
        operand_addr = &save_num_;
    }
    else if (cmnd.imm)
    {
        operand_addr = (operand_t*)(processor->instructs + processor->ip);
        processor->ip += sizeof(operand_t);
    }
    else if (cmnd.reg)
    {
        operand_addr = &processor->regs[*(operand_t*)(processor->instructs + processor->ip)];
        processor->ip += sizeof(operand_t);
    }

    if (cmnd.mem)
    {
        operand_addr = &processor->memory[*operand_addr];
    }

    return operand_addr;
}