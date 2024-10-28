#include <stdio.h>

#include "logger/liblogger.h"
#include "processing/processing.h"

int main()
{
    if (logger_ctor())
    {
        fprintf(stderr, "Can't logger_ctor()\n");
        return EXIT_FAILURE;
    }
    if (logger_set_level_details(LOG_LEVEL_DETAILS_ALL))
    {
        fprintf(stderr, "Can't logger_set_level_details()\n");
        return EXIT_FAILURE;
    }
    printf(GREEN_TEXT("\nHello processor!\n"));

    //==============================

    processor_t processor = {}; 
    
    PROCESSOR_ERROR_HANDLE(processor_ctor(&processor, "../assets/program_code.bin"), 
                           logger_dtor(); processor_dtor(&processor););
    
    PROCESSOR_ERROR_HANDLE(processing(&processor), logger_dtor(); processor_dtor(&processor););

    processor_dtor(&processor);
    //==============================

    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        return EXIT_FAILURE;
    }
    return 0;
}