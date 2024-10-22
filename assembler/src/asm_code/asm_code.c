#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm_code.h"
#include "utils.h"
#include "logger/liblogger.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* asm_code_strerror(const enum AsmCodeError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(ASM_CODE_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(ASM_CODE_ERROR_FAILURE);
    default:
        return "UNKNOWN_ASM_CODE_ERROR";
    }
    return "UNKNOWN_ASM_CODE_ERROR";
}
#undef CASE_ENUM_TO_STRING_


static enum AsmCodeError fill_asm_code_string_count_and_split_ (asm_code_t* const asm_code);
static enum AsmCodeError fill_asm_code_string_ptrs_            (asm_code_t* const asm_code);
static enum AsmCodeError fill_asm_code_size_ (asm_code_t* const asm_code, FILE** const input_file);
static enum AsmCodeError fill_asm_code_data_ (asm_code_t* const asm_code, FILE** const input_file);


enum AsmCodeError asm_code_ctor(const char* const input_filename, asm_code_t* const asm_code)
{
    lassert(input_filename, "");
    lassert(asm_code, "");

    FILE* input_file = fopen(input_filename, "rb");
    if (!input_file)
    {
        perror("Can't fopen input file");
        return ASM_CODE_ERROR_FAILURE;
    }
    setbuf(input_file, NULL); // TODO mmap

    ASM_CODE_ERROR_HANDLE(fill_asm_code_size_(asm_code, &input_file));
    ASM_CODE_ERROR_HANDLE(fill_asm_code_data_(asm_code, &input_file), 
                          free(asm_code->code); IF_DEBUG(asm_code->code = NULL););

    if (fclose(input_file))
    {
        perror("Can't fclose input file");
        return ASM_CODE_ERROR_FAILURE;
    }
    IF_DEBUG(input_file = NULL;)

    ASM_CODE_ERROR_HANDLE(fill_asm_code_string_count_and_split_(asm_code),
                          free(asm_code->code); IF_DEBUG(asm_code->code = NULL;));

    ASM_CODE_ERROR_HANDLE(fill_asm_code_string_ptrs_           (asm_code),
                          free(asm_code->code);   IF_DEBUG(asm_code->code   = NULL;)
                          free(asm_code->comnds); IF_DEBUG(asm_code->comnds = NULL;));

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_size_(asm_code_t* const asm_code, FILE** const input_file) 
{
    lassert(asm_code, "");
    lassert(input_file, "");
    lassert(*input_file, "");

    if (fseek(*input_file, 0, SEEK_END))
    {
        perror("Can't fseek to end input file");
        return ASM_CODE_ERROR_FAILURE;
    }

    long code_size = 0;
    if ((code_size = ftell(*input_file)) < 0)
    {
        perror("Can't ftell input_file");
        return ASM_CODE_ERROR_FAILURE;
    }
    asm_code->code_size = (size_t)code_size + 1;


    if (fseek(*input_file, 0, SEEK_SET))
    {
        perror("Can't fseek to start input file");
        return ASM_CODE_ERROR_FAILURE;
    }

    return ASM_CODE_ERROR_SUCCESS;
}

enum AsmCodeError fill_asm_code_data_(asm_code_t* const asm_code, FILE** const input_file)
{
    lassert(input_file, "");
    lassert(*input_file, "");
    lassert(asm_code, "");
    lassert(asm_code->code_size, "");

    asm_code->code = (char*)calloc(asm_code->code_size, sizeof(*asm_code->code));
    if (!asm_code->code)
    {
        perror("Can't calloc memory for asm_code->code");
        return ASM_CODE_ERROR_FAILURE;
    }

    if (fread(asm_code->code, 1, asm_code->code_size - 1, *input_file) != (asm_code->code_size - 1))
    {
        perror("Can't fread into input file");
        return ASM_CODE_ERROR_FAILURE;
    }

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_string_count_and_split_(asm_code_t* const asm_code)
{
    lassert(asm_code, "");
    lassert(asm_code->code_size, "");
    lassert(asm_code->code, "");

    asm_code->comnds_size = 1;
    for (size_t ind = 0; ind < asm_code->code_size; ++ind)
    {
        if (asm_code->code[ind] == '\n')
          ++asm_code->comnds_size;
    }

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_string_ptrs_(asm_code_t* const asm_code)
{
    lassert(asm_code, "");
    lassert(asm_code->code_size, "");
    lassert(asm_code->code, "");
    lassert(asm_code->comnds_size, "");

    asm_code->comnds = (char**)calloc(asm_code->comnds_size, sizeof(*asm_code->comnds));
    if (!asm_code->comnds)
    {
        perror("Can't calloc memory for comnds");
        return ASM_CODE_ERROR_FAILURE;
    }

    *asm_code->comnds = asm_code->code;
    lassert(*asm_code->comnds, "");


    const char * const SOUGHTABLE_SYMBOLS = " \n";
    char* finded_symbol_ptr = asm_code->code;
    char** string_ptr = asm_code->comnds + 1;

    while ((finded_symbol_ptr = strpbrk(finded_symbol_ptr, SOUGHTABLE_SYMBOLS)) 
         && (size_t)(finded_symbol_ptr - asm_code->code + 1) != asm_code->code_size)
    {
        lassert(finded_symbol_ptr >= asm_code->code, "");
        
        if (*finded_symbol_ptr == '\n')
        {
            *string_ptr = finded_symbol_ptr + 1; 
            ++string_ptr;
        }

        *finded_symbol_ptr = '\0';
        ++finded_symbol_ptr;
    }

    return ASM_CODE_ERROR_SUCCESS;
}


void asm_code_dtor(asm_code_t* asm_code)
{
    lassert(asm_code, "");

    free(asm_code->code);   IF_DEBUG(asm_code->code    = NULL;)
    free(asm_code->comnds); IF_DEBUG(asm_code->comnds  = NULL;)

    IF_DEBUG(asm_code->code_size   = 0;)
    IF_DEBUG(asm_code->comnds_size  = 0;)
}