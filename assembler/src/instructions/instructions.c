#include <string.h>

#include "instructions.h"
#include "logger/liblogger.h"
#include "utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* instructs_strerror(const enum InstructsError instructs_error)
{
    switch (instructs_error)
    {
        CASE_ENUM_TO_STRING_(INSTRUCTS_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(INSTRUCTS_ERROR_FAILURE);
    default:
        return "UNKNOWN_INSTRUCTS_ERROR";
    }
    return "UNKNOWN_INSTRUCTS_ERROR";
}
#undef CASE_ENUM_TO_STRING_


enum InstructsError instructs_ctor(instructs_t* const instructs, const size_t size)
{
    lassert(instructs, "");

    instructs->data = calloc(size, 1);
    if (!instructs->data)
    {
        perror("Can't calloc insturcts");
        return INSTRUCTS_ERROR_FAILURE;
    }

    instructs->size = size;
    instructs->counter = 0;

    return INSTRUCTS_ERROR_SUCCESS;
}

void instructs_dtor(instructs_t* const instructs)
{
    lassert(instructs, "");
    lassert(instructs->data, "");

    free(instructs->data); IF_DEBUG(instructs->data = NULL);
    IF_DEBUG(instructs->counter = 0);
    IF_DEBUG(instructs->size = 0);
}

void instructs_push(instructs_t* const instructs, const void* const elem, const size_t elem_size)
{
    lassert(instructs, "");
    lassert(elem, "");
    lassert(elem_size, "");
    // fprintf(stderr, "size: %zu", instructs->size);
    lassert(instructs->counter + elem_size < instructs->size, "");

    memcpy(instructs->data + instructs->counter, elem, elem_size);
    instructs->counter += elem_size;
}

enum InstructsError instructs_output(const char* const output_filename, instructs_t instructs)
{
    lassert(output_filename, "");

    FILE* output_file = fopen(output_filename, "wb");
    if (!output_file)
    {
        perror("Can't fopen output file");
        return INSTRUCTS_ERROR_FAILURE;
    }
    setbuf(output_file, NULL);

    if (fwrite(instructs.data, 1, instructs.size, output_file) != instructs.size)
    {
        perror("Can't fwrite instructs.data");
        return INSTRUCTS_ERROR_FAILURE;
    }

    if (fclose(output_file))
    {
        perror("Can't close output file");
        return INSTRUCTS_ERROR_FAILURE;
    }

    return INSTRUCTS_ERROR_SUCCESS;
}