#include <stdio.h>
#include <string.h>

#include "labels.h"
#include "logger/liblogger.h"
#include "utils.h"

#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* labels_strerror(const enum LabelsError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(LABELS_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(LABELS_ERROR_FAILURE);
    default:
        return "UNKNOWN_LABELS_ERROR";
    }
    return "UNKNOWN_LABELS_ERROR";
}
#undef CASE_ENUM_TO_STRING_


enum LabelsError labels_ctor(labels_t* const labels, const size_t count_label_names)
{
    lassert(labels, "");
    lassert(count_label_names, "");

    labels->labels = (label_t*)calloc(count_label_names, sizeof(*labels->labels));
    if (!labels->labels)
    {
        perror("Can't calloc labels->labels");
        return LABELS_ERROR_FAILURE;
    }
    labels->size = count_label_names;
    labels->count = 0;

    return LABELS_ERROR_SUCCESS;
}

void labels_dtor(labels_t* const labels)
{
    lassert(labels, "");
    lassert(labels->labels, "");
    lassert(labels->count < labels->size, "");

#ifndef NDEBUG
    for (size_t label_ind = 0; label_ind < labels->size; ++label_ind)
    {
        labels->labels[label_ind].name = NULL;
        labels->labels[label_ind].addr = 0;
    }
    labels->count = 0;
    labels->size  = 0;
#endif /*NDEBUG*/
    free(labels->labels); IF_DEBUG(labels->labels = NULL;)
}


static enum LabelsError labels_resize_(labels_t* labels, const size_t add_size);

enum LabelsError labels_push(labels_t* const labels, label_t label)
{
    lassert(labels, "");
    lassert(labels->labels, "");
    lassert(label.name, "");
    lassert(labels->count < labels->size, "");

    LABELS_ERROR_HANDLE(labels_resize_(labels, 1));

    labels->labels[labels->count].name = label.name;
    labels->labels[labels->count].addr = label.addr;
    ++labels->count;

    return LABELS_ERROR_SUCCESS;
}

static void* recalloc_(void* ptrmem, const size_t old_number, const size_t old_size,
                                     const size_t     number, const size_t     size);

static enum LabelsError labels_resize_(labels_t* labels, const size_t add_size)
{
    lassert(labels, "");

    if (add_size == 0)
        return LABELS_ERROR_SUCCESS;

    if (labels->count + add_size >= labels->size)
    {
        const size_t new_size = MAX(labels->size * 2, labels->count + add_size);
        
        lassert(new_size > labels->size, "");

        void* temp_data = recalloc_(labels->labels, 
                                    labels->size, sizeof(*labels->labels),
                                        new_size, sizeof(*labels->labels));
        if (!temp_data)
        {
            fprintf(stderr, "Can't recalloc_\n");
            return LABELS_ERROR_FAILURE;
        }
        labels->labels = temp_data; IF_DEBUG(temp_data = NULL;)

        labels->size = new_size;
    }

    return LABELS_ERROR_SUCCESS;
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


label_t* labels_find(const labels_t labels, const char* const name)
{
    lassert(labels.labels, "");
    lassert(name, "");
    lassert(labels.count < labels.size, "");

    for (size_t ind = 0; ind < labels.count; ++ind)
    {
        lassert(labels.labels[ind].name, "");

        if (strcmp(name, labels.labels[ind].name) == 0)
        {
            return labels.labels + ind;
        }
    }

    return NULL;
}

bool labels_push_unfinded(labels_t* const labels, label_t label)
{
    lassert(labels, "");
    lassert(labels->labels, "");
    lassert(label.name, "");
    lassert(labels->count < labels->size, "");

    if (!labels_find(*labels, label.name))
    {
        labels_push(labels, label);
        return true;
    }

    return false;
}