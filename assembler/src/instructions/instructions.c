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

    instructs->data = calloc(size, sizeof(*instructs->data));
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

//============================

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

static enum InstructsError instructs_resize_(instructs_t* instructs, const size_t add_size);

enum InstructsError instructs_push_back(instructs_t* const instructs, const void* const elem, 
                         const size_t elem_size)
{
    lassert(instructs, "");
    lassert(elem, "");
    lassert(elem_size, "");

    INSTRUCTS_ERROR_HANDLE(instructs_resize_(instructs, elem_size));

    memcpy(instructs->data + instructs->counter, elem, elem_size);
    instructs->counter += elem_size;

    return INSTRUCTS_ERROR_SUCCESS;
}

static enum InstructsError instructs_resize_(instructs_t* instructs, const size_t add_size)
{
    lassert(instructs, "");

    if (add_size == 0)
        return INSTRUCTS_ERROR_SUCCESS;

    size_t new_size = 0;
    if (instructs->counter + add_size >= instructs->size)
    {
        new_size = MAX(instructs->size * 2, instructs->counter + add_size);
    }

    if (new_size)
    {
        lassert(new_size > instructs->size, "");

        void* temp_data = recalloc_(instructs->data, 
                                    instructs->size, sizeof(*instructs->data),
                                           new_size, sizeof(*instructs->data));
        if (!temp_data)
        {
            fprintf(stderr, "Can't recalloc_");
            return INSTRUCTS_ERROR_FAILURE;
        }
        instructs->data = temp_data; IF_DEBUG(temp_data = NULL;)

        instructs->size = new_size;
    }

    return INSTRUCTS_ERROR_SUCCESS;
}

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size)
{
    lassert(ptrmem, "");
    lassert(number, "");
    lassert(size  , "");

    if (number * size == old_number * old_size)
        return ptrmem;

    if (!(ptrmem = realloc(ptrmem, number * size)))
    {
        perror("Can't realloc in recalloc_");
        return NULL;
    }

    if (number * size > old_number * old_size
        && !memset((char*)ptrmem + old_number * old_size, 0, number * size - old_number * old_size))
    {
        perror("Can't memset in recalloc_");
        return NULL;
    }

    return ptrmem;
}

//===============================

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