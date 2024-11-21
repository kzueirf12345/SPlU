#include <stdio.h>
#include <getopt.h>
#include <SDL2/SDL.h>
#include <errno.h>

#include "logger/liblogger.h"
#include "processing/processing.h"

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

    bool is_quit = false;
    while (!is_quit) {
        SDL_Event sdl_event = {};
        while (SDL_PollEvent(&sdl_event))
        {
            switch (sdl_event.type)
            {
            case SDL_QUIT:
                is_quit = true;
                break;
            default:
                break;
            }
        }
    }

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

// #include <stdio.h>
// // #include <SDL2/SDL.h>
// #include <errno.h>
// #include <string.h>

// int main(int argc, char* argv[])
// {
//     fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));

//     return 0;
// }