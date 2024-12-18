#ifndef ASSEMBLER_SRC_LABELS_H
#define ASSEMBLER_SRC_LABELS_H

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>


enum LabelsError
{
    LABELS_ERROR_SUCCESS  = 0,
    LABELS_ERROR_FAILURE  = 1
};
static_assert(LABELS_ERROR_SUCCESS == 0);

const char* labels_strerror(const enum LabelsError error);

#define LABELS_ERROR_HANDLE(call_func, ...)                                                          \
    do {                                                                                            \
        const enum LabelsError labels_error_handler = call_func;                                                            \
        if (labels_error_handler)                                                                    \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Labels error: %s\n",                               \
                            labels_strerror(labels_error_handler));                                   \
            __VA_ARGS__                                                                             \
            return labels_error_handler;                                                             \
        }                                                                                           \
    } while(0)


typedef struct Label
{
    char* name;
    size_t addr;
} label_t;

typedef struct Labels
{
    label_t* labels;
    size_t size;
    size_t count;
} labels_t;

enum LabelsError labels_ctor(labels_t* const labels, const size_t count_label_names);
void labels_dtor(labels_t* const labels);

enum LabelsError labels_push(labels_t* const labels, label_t label);

label_t* labels_find(const labels_t labels, const char* const name);

bool labels_push_unfinded(labels_t* const labels, label_t label);



#endif /*ASSEMBLER_SRC_LABELS_H*/