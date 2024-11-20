#include <stdio.h>
#include <getopt.h>
#include <SDL2/SDL.h>

#include "logger/liblogger.h"
#include "processing/processing.h"
// #include "sdl/sdl.h"

int main(int argc, char* argv[])
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

    sdl_objs_t sdl_objs = {};
    SDL_ERROR_HANDLE(sdl_ctor(&sdl_objs, MEMORY_WIDTH * PIXEL_SIZE, MEMORY_HEIGHT * PIXEL_SIZE), 
                     sdl_dtor(&sdl_objs););

    if (logger_ctor())
    {
        fprintf(stderr, "Can't logger_ctor()\n");
        sdl_dtor(&sdl_objs);
        return EXIT_FAILURE;
    }
    if (logger_set_level_details(LOG_LEVEL_DETAILS_ALL))
    {
        fprintf(stderr, "Can't logger_set_level_details()\n");
        sdl_dtor(&sdl_objs);
        return EXIT_FAILURE;
    }
    printf(GREEN_TEXT("\nHello processor!\n"));

    //==============================

    processor_t processor = {}; 
    
    PROCESSOR_ERROR_HANDLE(processor_ctor(&processor, input_filename), 
                           logger_dtor(); processor_dtor(&processor); sdl_dtor(&sdl_objs););
    
    PROCESSOR_ERROR_HANDLE(processing(&processor, sdl_objs), 
                           logger_dtor(); processor_dtor(&processor); sdl_dtor(&sdl_objs););

    processor_dtor(&processor);
    //==============================

    if (logger_dtor())
    {
        fprintf(stderr, "Can't logger_dtor()\n");
        sdl_dtor(&sdl_objs);
        return EXIT_FAILURE;
    }

    sdl_dtor(&sdl_objs);

    return 0;
}