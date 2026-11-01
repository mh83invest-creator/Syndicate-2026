#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    SDL_Window *window;
    SDL_Surface *screen, *draw;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("Syndicate graphics isolated diagnostics", 0, 0, 640, 400, SDL_WINDOW_HIDDEN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    screen = SDL_GetWindowSurface(window);
    draw = SDL_CreateRGBSurface(0, 320, 200, 8, 0, 0, 0, 0);
    if (!screen || !draw) {
        fprintf(stderr, "Surface creation: %s\n", SDL_GetError());
        if (draw) SDL_FreeSurface(draw);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    printf("WINDOW surface width=%d height=%d bpp=%d bytes=%d pitch=%d format=%s\n",
      screen->w, screen->h, screen->format->BitsPerPixel,
      screen->format->BytesPerPixel, screen->pitch, SDL_GetPixelFormatName(screen->format->format));
    printf("DRAW surface width=%d height=%d bpp=%d bytes=%d pitch=%d format=%s\n",
      draw->w, draw->h, draw->format->BitsPerPixel,
      draw->format->BytesPerPixel, draw->pitch, SDL_GetPixelFormatName(draw->format->format));
    SDL_FreeSurface(draw);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
