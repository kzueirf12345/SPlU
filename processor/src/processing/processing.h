
/*!SECTION

enum AsmError assembly(const asm_code_t asm_code, 
                       instruction_t* const instructions, const size_t instructions_size)
{
    lassert(instructions, "");
    lassert(instructions_size, "");

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
        enum Opcode comnd_code = comnd_str_to_enum_(comnd);

        switch(comnd_code)
        {
            case OPCODE_PUSH:
            {
                instructions[IP++] = (instruction_t)OPCODE_PUSH;
                const instruction_t push_num = atoll(strchr(comnd, '\0') + 1);
                instructions[IP]   = push_num;
                stack_error_handle_(stack_push(&stack, &push_num), stack_dtor(&stack););
                break;
            }

            case OPCODE_ADD:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t sum = first_num + second_num;
                stack_error_handle_(stack_push(&stack, &sum), stack_dtor(&stack););
                break;
            }
            case OPCODE_SUB:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t sub = first_num - second_num;
                stack_error_handle_(stack_push(&stack, &sub), stack_dtor(&stack););
                break;
            }
            case OPCODE_MUL:
            {
                lassert(stack_size(stack) >= 2, "");

                instruction_t first_num = 0, second_num = 0;
                stack_error_handle_(stack_pop(&stack, &second_num), stack_dtor(&stack););
                stack_error_handle_(stack_pop(&stack, &first_num),  stack_dtor(&stack););

                const instruction_t mul = first_num * second_num;
                stack_error_handle_(stack_push(&stack, &mul), stack_dtor(&stack););
                break;
            }
            case OPCODE_DIV:
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

            case OPCODE_IN:
            {
                
                break;
            }
            case OPCODE_OUT:
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

            case OPCODE_HLT:
            {
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
        ++CP;
    }

    stack_dtor(&stack);
    return ASM_ERROR_SUCCESS;
}
#undef stack_error_handle_


static enum Opcode comnd_str_to_enum_(const char* const comnd_str)
{
    lassert(comnd_str, "");

    if (strcmp(comnd_str, "PUSH") == 0)
        return OPCODE_PUSH;

    if (strcmp(comnd_str, "ADD")  == 0)
        return OPCODE_ADD;
    if (strcmp(comnd_str, "SUB")  == 0)
        return OPCODE_SUB;
    if (strcmp(comnd_str, "MUL")  == 0)
        return OPCODE_MUL;
    if (strcmp(comnd_str, "DIV")  == 0)
        return OPCODE_DIV;

    if (strcmp(comnd_str, "IN")   == 0)
        return OPCODE_IN;
    if (strcmp(comnd_str, "OUT")  == 0)
        return OPCODE_OUT;

    if (strcmp(comnd_str, "HLT")  == 0)
        return OPCODE_HLT;
    
    return OPCODE_UNKNOWN;
}
*/