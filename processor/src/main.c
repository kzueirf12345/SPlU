#include <stdio.h>

#include "logger/liblogger.h"
#include "processing/processing.h"

int main()
{
    const char* input_filename = "../assets/program_code.bin";
    int getopt_rez = 0;
    while ((getopt_rez = getopt(argc, argv, "i:")) != -1)
    {
        switch (getopt_rez)
        {
        case 'i':
            input_filename = optarg;
            break;
        default:
            fprintf(stderr, "Getopt error: %d\n", (char)getopt_rez);
            break;
        }
    }

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
    
    PROCESSOR_ERROR_HANDLE(processor_ctor(&processor, input_filename), 
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