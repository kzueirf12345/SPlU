/* eslint-disable */
#define ARG_WITH_COMMA(...) __VA_ARGS__

COMAND_HANDLE
(
    ZERO,
    ARG_WITH_COMMA
    (

    ),
    ARG_WITH_COMMA
    (
        fprintf(stderr, "ZERO INSTRUCT\n");
        stack_dtor(&stack);
        return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
    )
)

COMAND_HANDLE 
(
    PUSH, 
    ARG_WITH_COMMA
    (
        const char* operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_instruct_with_operand_(comnd_code, operand_str, instructs),
                                                         fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        STACK_ERROR_HANDLE_(stack_push(&stack, get_operand_addr_(cmnd, processor)),
                                                       stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE 
(
    POP, 
    ARG_WITH_COMMA
    (
        const char* operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_instruct_with_operand_(comnd_code, operand_str, instructs),
                                                         fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 1, "");

        operand_t pop_elem = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &pop_elem), 
                                                       stack_dtor(&stack); stack_dtor(&stack_ret););

        memcpy(get_operand_addr_(cmnd, processor), &pop_elem, sizeof(pop_elem));
    )
)

COMAND_HANDLE
(
    ADD,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t sum = first_num + second_num;
        STACK_ERROR_HANDLE_(stack_push(&stack, &sum), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    SUB,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t div = first_num - second_num;
        STACK_ERROR_HANDLE_(stack_push(&stack, &div), 
                            stack_dtor(&stack);
                            stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    MUL,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t mul = first_num * second_num;
        STACK_ERROR_HANDLE_(stack_push(&stack, &mul), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    DIV,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););


        if (second_num == 0)
        {
            fprintf(stderr, "Can't div by zero\n");
            stack_dtor(&stack);
            stack_dtor(&stack_ret);
            return PROCESSOR_ERROR_DIV_BY_ZERO;
        }
        const operand_t div = first_num / second_num;
        STACK_ERROR_HANDLE_(stack_push(&stack, &div), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        
    )
)

COMAND_HANDLE
(
    POW,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (operand_t)pow((double)first_num, (double)second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    EQ,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num == second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    NEQ,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num != second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    LESS,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num < second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    LEQ,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num <= second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    GREAT,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num > second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    GEQ,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t res = (first_num >= second_num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &res), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    MOD,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        if (second_num == 0)
        {
            fprintf(stderr, "Can't mod by zero\n");
            stack_dtor(&stack);
            stack_dtor(&stack_ret);
            return PROCESSOR_ERROR_DIV_BY_ZERO;
        }
        const operand_t mod = first_num % second_num;
        STACK_ERROR_HANDLE_(stack_push(&stack, &mod), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        
    )
)

COMAND_HANDLE
(
    SQR,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 1, "");

        operand_t num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const operand_t sqr = (operand_t)sqrt((double)num);
        STACK_ERROR_HANDLE_(stack_push(&stack, &sqr), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        
    )
)

COMAND_HANDLE
(
    OUT,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 1, "");

        operand_t out_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &out_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        if (printf("Out: " INOUT_OPERAND_CODE "\n", out_num) <= 0)
        {
            perror("Can't printf out_num");
            stack_dtor(&stack);
            stack_dtor(&stack_ret);
            return PROCESSOR_ERROR_STANDARD_ERRNO;
        }
    )
)

COMAND_HANDLE
(
    IN,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        operand_t in_num = 0;
        printf("Input: ");
        if (scanf(INOUT_OPERAND_CODE, &in_num) != 1)
        {
            perror("Can't scanf in_num");
            stack_dtor(&stack);
            stack_dtor(&stack_ret);
            return PROCESSOR_ERROR_STANDARD_ERRNO;
        }
        STACK_ERROR_HANDLE_(stack_push(&stack, &in_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        
    )
)

COMAND_HANDLE
(
    JMP,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        jmp_condition_handle_(true, processor);
    )
)

COMAND_HANDLE
(
    JL,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num < second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    JLE,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num <= second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    JG,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num > second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    JGE,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num >= second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    JE,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num == second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    JNE,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        lassert(stack_size(stack) >= 2, "");

        operand_t first_num = 0, second_num = 0;
        STACK_ERROR_HANDLE_(stack_pop(&stack, &second_num), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        STACK_ERROR_HANDLE_(stack_pop(&stack, &first_num),  
                            stack_dtor(&stack); stack_dtor(&stack_ret););

        const bool condition = first_num != second_num;
        jmp_condition_handle_(condition, processor);
    )
)

COMAND_HANDLE
(
    LABEL,
    ARG_WITH_COMMA
    (
        char* const operand_str = cmnd_str + 1;
        const label_t label = {.name = operand_str, .addr = instructs->counter};
        if (!labels_push_unfinded(&labels, label))
        {
            fprintf(stderr, "Can't push label %s in %zu line\n", operand_str, ip + 1);
            fixup_dtor(&fixup);
            labels_dtor(&labels);
            return ASM_ERROR_LABELS;
        }
    ),
    ARG_WITH_COMMA
    (
        fprintf(stderr, "LABEL INSTRUCT. It's not valid in processor\n");
        stack_dtor(&stack);
        stack_dtor(&stack_ret);
        return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
    )
)

COMAND_HANDLE
(
    CALL,
    ARG_WITH_COMMA
    (
        char* const operand_str = strchr(cmnd_str, '\0') + 1;
        ASM_ERROR_HANDLE_(push_jmp_(comnd_code, operand_str, instructs, labels, &fixup), 
                            fixup_dtor(&fixup); labels_dtor(&labels););
    ),
    ARG_WITH_COMMA
    (
        const size_t ret_ip = processor->ip + sizeof(operand_t);
        STACK_ERROR_HANDLE_(stack_push(&stack_ret, &ret_ip),
                            stack_dtor(&stack); stack_dtor(&stack_ret););
        jmp_condition_handle_(true, processor);
    )
)

COMAND_HANDLE
(
    RET,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        lassert(!stack_is_empty(stack_ret), "");

        STACK_ERROR_HANDLE_(stack_pop(&stack_ret, &processor->ip), 
                            stack_dtor(&stack); stack_dtor(&stack_ret););
    )
)

COMAND_HANDLE
(
    DRAW,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        PROCESSOR_ERROR_HANDLE(draw_(*processor, sdl_objs), 
                               stack_dtor(&stack); stack_dtor(&stack_ret););
                
    )

)

COMAND_HANDLE
(
    DRAWT,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
        PROCESSOR_ERROR_HANDLE(drawt_(*processor), 
                               stack_dtor(&stack); stack_dtor(&stack_ret););
                
    )

)

COMAND_HANDLE
(
    HLT,
    ARG_WITH_COMMA
    (
        cmnd_t cmnd = {.imm = 0, .reg = 0, .mem = 0, .opcode = comnd_code };
        instructs_push_back(instructs, &cmnd, 1);
    ),
    ARG_WITH_COMMA
    (
#ifndef NDEBUG
        if (!stack_is_empty(stack))
            fprintf(stderr, "When program is end, stack wasn't empty\n");
#endif /*NDEBUG*/
        is_hlt = true;
    )
)


COMAND_HANDLE
(
    UNKNOWN,
    ARG_WITH_COMMA
    (
        fprintf(stderr, "str: '%s'\n", cmnd_str);
        fprintf(stderr, "Incorrect command in %zu line\n", ip + 1);
        fixup_dtor(&fixup);
        labels_dtor(&labels);
        return ASM_ERROR_INCORRECT_CMND;
    ),
    ARG_WITH_COMMA
    (
        fprintf(stderr, "UNKNOWN INSTRUCT\n");
        stack_dtor(&stack);
        stack_dtor(&stack_ret);
        return PROCESSOR_ERROR_UNKNOWN_INSTRUCT;
    )
)