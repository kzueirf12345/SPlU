#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

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

    processor->memory = calloc(MEMORY_SIZE, sizeof(*processor->memory));
    if (!processor->memory)
    {
        perror("Can't calloc processor->memory");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }
    if (!memset(processor->memory, NVALID_OUTPUT_CHAR_VAL, 
                MEMORY_SIZE * sizeof(*processor->memory)))
    {
        perror("Can't memset '.' processor->memory");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    processor->regs = calloc(REGS_SIZE, sizeof(*processor->regs));
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


static void       lassert_processor_init_(                const processor_t* const processor);
static operand_t* get_operand_addr_      (cmnd_t cmnd,          processor_t* const processor);
static void       jmp_condition_handle_  (const bool condition, processor_t* const processor);
static enum ProcessorError drawt_ (processor_t processor);
static enum ProcessorError draw_  (processor_t processor, const sdl_objs_t sdl_objs);

#define STACK_ERROR_HANDLE_(call_func, ...)                                                         \
    do {                                                                                            \
        if (call_func)                                                                              \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func "\n");                                              \
            __VA_ARGS__                                                                             \
            return PROCESSOR_ERROR_STACK;                                                           \
        }                                                                                           \
    } while(0)

#define COMAND_HANDLE(cmd_name, ndo_it, do_it)                                                      \
        case OPCODE_##cmd_name:                                                                     \
        {                                                                                           \
            do_it                                                                                   \
            break;                                                                                  \
        } 
        
enum ProcessorError processing(processor_t* const processor, const sdl_objs_t sdl_objs)
{
    lassert_processor_init_(processor);
    
    SDL_Event sdl_event = {};
    bool is_quit_sdl = false;

    stack_key_t stack = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack, sizeof(operand_t), 0), stack_dtor(&stack););

    stack_key_t stack_ret = 0;
    STACK_ERROR_HANDLE_(STACK_CTOR(&stack_ret, sizeof(size_t), 0), 
                        stack_dtor(&stack); stack_dtor(&stack_ret););

    bool is_hlt = false;

    while (!is_hlt && processor->ip < processor->instructs_size && !is_quit_sdl)
    {
        const cmnd_t cmnd = *(cmnd_t*)(processor->instructs + processor->ip);
        ++processor->ip;

        switch ((enum Opcode)cmnd.opcode)
        {
            #include "commands.h"

            default:
            {
                fprintf(stderr, "Pizdec\n");
                stack_dtor(&stack);
                stack_dtor(&stack_ret);
                return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
            }
        }

        if (SDL_PollEvent(&sdl_event))
        {
            switch (sdl_event.type)
            {
            case SDL_QUIT:
                is_quit_sdl = true;
                break;
            default:
                break;
            }
        }
    }

    stack_dtor(&stack);
    stack_dtor(&stack_ret);
    return PROCESSOR_ERROR_SUCCESS;
}
#undef STACK_ERROR_HANDLE_
#undef COMAND_HANDLE

static void lassert_processor_init_(const processor_t* const processor)
{
    lassert(processor, "");
    lassert(processor->instructs, "");
    lassert(processor->instructs_size, "");
    lassert(processor->memory, "");
    lassert(processor->regs, "");
}

static operand_t* get_operand_addr_(cmnd_t cmnd, processor_t* const processor)
{
    lassert(processor, "");
    lassert(cmnd.imm | cmnd.mem | cmnd.reg, "");

    operand_t* operand_addr = NULL;

    if (cmnd.imm && cmnd.reg)
    {
        operand_t imm_num = 0;
        memcpy(&imm_num, processor->instructs + processor->ip, sizeof(imm_num));
        processor->ip += sizeof(operand_t);

        operand_t reg_num = 0;
        lassert(processor->instructs[processor->ip] < REGS_SIZE, "Register overflow");
        memcpy(&reg_num, processor->regs + processor->instructs[processor->ip], sizeof(reg_num));
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
        lassert(processor->instructs[processor->ip] < REGS_SIZE, "Register overflow");
        operand_addr = processor->regs + processor->instructs[processor->ip];
        processor->ip += sizeof(operand_t);
    }

    if (cmnd.mem)
    {
        // const __useconds_t SLEEP_TIME_ = 5000;
        // if (usleep(SLEEP_TIME_) != 0)
        // {
        //     fprintf(stderr, "Can't sleep :)\n");
        //     return NULL;
        // }

        operand_t temp = 0;
        memcpy(&temp, operand_addr, sizeof(*operand_addr));
        lassert((size_t)temp < MEMORY_SIZE, "Memory overflow");
        operand_addr = processor->memory + temp;
    }

    return operand_addr;
}

static void jmp_condition_handle_(const bool condition, processor_t* const processor)
{
    lassert(processor, "");

    if (condition)
    {
        memcpy(&processor->ip,  processor->instructs + processor->ip, sizeof(processor->ip));
        lassert(processor->ip < processor->instructs_size, "Instructs overflow. Ip = 0x%lx", 
                processor->ip);
    }
    else
    {
        processor->ip += sizeof(operand_t);
    }
}

static enum ProcessorError draw_ (processor_t processor, const sdl_objs_t sdl_objs)
{
    lassert(processor.memory, "");
    static_assert(MEMORY_HEIGHT * MEMORY_WIDTH <= MEMORY_SIZE);

    SDL_SetRenderDrawColor(sdl_objs.renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(sdl_objs.renderer);

    for (size_t row = 0; row < MEMORY_HEIGHT; ++row)
    {
        for (size_t col = 0; col < MEMORY_WIDTH; ++col)
        {
            const char out_sym = *(char*)(processor.memory + row * MEMORY_WIDTH + col);

            if (MIN_VALID_OUTPUT_CHAR <= out_sym && out_sym <= MAX_VALID_OUTPUT_CHAR)
            {
                SDL_SetRenderDrawColor(sdl_objs.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            else
            {
                SDL_SetRenderDrawColor(sdl_objs.renderer, 0x20, 0x20, 0x20, 0xFF);
            }

            SDL_Rect rect1 = {(int)(PIXEL_SIZE * col), (int)(PIXEL_SIZE * row), 9, 9};
            SDL_RenderFillRect(sdl_objs.renderer, &rect1);
        }
    }

    SDL_RenderPresent(sdl_objs.renderer);

    return PROCESSOR_ERROR_SUCCESS;
}

static enum ProcessorError drawt_ (processor_t processor)
{
    lassert(processor.memory, "");
    static_assert(MEMORY_HEIGHT * MEMORY_WIDTH <= MEMORY_SIZE);

    if (putc('\n', stdout) != '\n')
    {
        perror("Can't putc \\n");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    for (size_t row = 0; row < MEMORY_HEIGHT; ++row)
    {
        for (size_t col = 0; col < MEMORY_WIDTH; ++col)
        {
            const char out_sym = *(char*)(processor.memory + row * MEMORY_WIDTH + col);

            if (printf("%-3d", (int)out_sym) <= 0)
            {
                perror("Can't printf outsym");
                return PROCESSOR_ERROR_STANDARD_ERRNO;
            }
        }

        if (putc('\n', stdout) != '\n')
        {
            perror("Can't putc \\n");
            return PROCESSOR_ERROR_STANDARD_ERRNO;
        }
    }

    if (putc('\n', stdout) != '\n')
    {
        perror("Can't putc \\n");
        return PROCESSOR_ERROR_STANDARD_ERRNO;
    }

    return PROCESSOR_ERROR_SUCCESS;
}