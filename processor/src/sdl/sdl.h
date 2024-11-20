#ifndef SPLU_PROCESSOR_SRC_SDL_SDL_H
#define SPLU_PROCESSOR_SRC_SDL_SDL_H

#include <assert.h>
#include <SDL2/SDL.h>

enum SdlError
{
    SDL_ERROR_SUCCESS           = 0,
    SDL_ERROR_STANDARD_ERRNO    = 1,
    SDL_ERROR_SDL               = 2,
};
static_assert(SDL_ERROR_SUCCESS == 0);

const char* sdl_strerror(const enum SdlError error);

#define SDL_ERROR_HANDLE(call_func, ...)                                                            \
    do {                                                                                            \
        const enum SdlError error_handler = call_func;                                              \
        if (error_handler)                                                                          \
        {                                                                                           \
            fprintf(stderr, "Can't " #call_func". Error: %s\n",                                     \
                            sdl_strerror(error_handler));                                           \
            __VA_ARGS__                                                                             \
            return error_handler;                                                                   \
        }                                                                                           \
    } while(0)

#endif /* SPLU_PROCESSOR_SRC_SDL_SDL_H */

void sdl_perror(const char* const format, ...);

typedef struct SdlObjs
{
    SDL_Window* window;
    SDL_Renderer* renderer;
} sdl_objs_t;

enum SdlError sdl_ctor(sdl_objs_t* const objs, const int screen_width, const int screen_height);
void          sdl_dtor(sdl_objs_t* const objs);