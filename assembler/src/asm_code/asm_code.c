#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
static enum AsmCodeError fill_asm_code_ (const char* const input_filename, 
                                         asm_code_t* const asm_code);


enum AsmCodeError asm_code_ctor(const char* const input_filename, asm_code_t* const asm_code)
{
    lassert(input_filename, "");
    lassert(asm_code, "");

    ASM_CODE_ERROR_HANDLE(fill_asm_code_(input_filename, asm_code));

    ASM_CODE_ERROR_HANDLE(fill_asm_code_string_count_and_split_(asm_code));
    ASM_CODE_ERROR_HANDLE(fill_asm_code_string_ptrs_           (asm_code),
                          free(asm_code->comnds); IF_DEBUG(asm_code->comnds = NULL;));

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_size_ (asm_code_t* const asm_code, const int fd);

static enum AsmCodeError fill_asm_code_ (const char* const input_filename, 
                                         asm_code_t* const asm_code)
{
    int fd = open(input_filename, O_RDWR);
    if (fd == -1)
    {
        perror("Can't fopen input file");
        return ASM_CODE_ERROR_FAILURE;
    }

    ASM_CODE_ERROR_HANDLE(fill_asm_code_size_(asm_code, fd));

    asm_code->code = mmap(NULL, asm_code->code_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);

    if (asm_code->code == MAP_FAILED)
    {
        perror("Can't mmap");
        return ASM_CODE_ERROR_FAILURE;
    }

    if (close(fd))
    {
        perror("Can't fclose input file");
        return ASM_CODE_ERROR_FAILURE;
    }
    IF_DEBUG(fd = -1;)

    asm_code->code[asm_code->code_size - 1] = '\0';

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_size_ (asm_code_t* const asm_code, const int fd)
{
    lassert(asm_code, "");
    lassert(fd != -1, "");

    struct stat fd_stat = {};

    if (fstat(fd, &fd_stat))
    {
        perror("Can't fstat");
        return ASM_CODE_ERROR_FAILURE;
    }

    asm_code->code_size = (size_t)fd_stat.st_size + 1;

    return ASM_CODE_ERROR_SUCCESS;
}

static enum AsmCodeError fill_asm_code_string_count_and_split_(asm_code_t* const asm_code)
{
    lassert(asm_code, "");
    lassert(asm_code->code_size, "");
    lassert(asm_code->code, "");

    asm_code->comnds_size = 1;
    const char* code_ptr = asm_code->code + 1;

    while ((code_ptr = strchr(code_ptr, '\n')))
    {
        ++asm_code->comnds_size;
        ++code_ptr;
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
         && (size_t)(finded_symbol_ptr - asm_code->code + 1) < asm_code->code_size)
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

    // free(asm_code->code);   IF_DEBUG(asm_code->code    = NULL;)
    free(asm_code->comnds); IF_DEBUG(asm_code->comnds  = NULL;)

    IF_DEBUG(asm_code->code_size   = 0;)
    IF_DEBUG(asm_code->comnds_size  = 0;)
}