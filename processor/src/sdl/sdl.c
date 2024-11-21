#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <SDL2/SDL.h>

#include "sdl.h"
#include "logger/liblogger.h"
#include "../../../utils/utils.h"


#define CASE_ENUM_TO_STRING_(error) case error: return #error
const char* sdl_strerror(const enum SdlError error)
{
    switch (error)
    {
        CASE_ENUM_TO_STRING_(SDL_ERROR_SUCCESS);
        CASE_ENUM_TO_STRING_(SDL_ERROR_STANDARD_ERRNO);
        CASE_ENUM_TO_STRING_(SDL_ERROR_SDL);
    default:
        return "UNKNOWN_SDL_ERROR";
    }
    return "UNKNOWN_SDL_ERROR";
}
#undef CASE_ENUM_TO_STRING_

void sdl_perror(const char* const format, ...)
{
    lassert(format, "");

    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);
    fprintf(stderr, ": %s\n", SDL_GetError());

    va_end(args);
}

enum SdlError sdl_ctor(sdl_objs_t* const objs, const int screen_width, const int screen_height)
{
    lassert(objs, "");

    if (SDL_Init(SDL_INIT_EVERYTHING)) 
    {
        sdl_perror("Can't SDl init");
        return SDL_ERROR_SDL;
    }

    fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));


    objs->window = SDL_CreateWindow("SPLU", 
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                    screen_width, screen_height, 
                                    SDL_WINDOW_SHOWN);

    errno = 0;

    fprintf(stderr, "errno: %d, %s\n", errno, strerror(errno));

    if (!objs->window)
    {
        sdl_perror("Can't SDL_CreateWindow");
        return SDL_ERROR_SDL;
    }

    objs->renderer = SDL_CreateRenderer(objs->window, -1, SDL_RENDERER_ACCELERATED);
    if (!objs->window)
    {
        sdl_perror("Can't SDL_CreateRenderer");
        return SDL_ERROR_SDL;
    }

    return SDL_ERROR_SUCCESS;
}

void sdl_dtor(sdl_objs_t* const objs)
{
    lassert(objs, "");

    SDL_DestroyWindow(objs->window);
    IF_DEBUG(objs->window = NULL;)

    SDL_DestroyRenderer(objs->renderer);
    IF_DEBUG(objs->renderer = NULL;)

    SDL_Quit();
}