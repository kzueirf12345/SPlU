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


void labels_push(labels_t* const labels, label_t label)
{
    lassert(labels, "");
    lassert(labels->labels, "");
    lassert(label.name, "");
    lassert(labels->count < labels->size, "");

    labels->labels[labels->count].name = label.name; // TODO check count (not assert)
    labels->labels[labels->count].addr = label.addr;
    ++labels->count;
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