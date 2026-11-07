#!/usr/bin/env python3
"""Generate an SDL2 integration test from the branch's unchanged scaler functions."""
from pathlib import Path

source = Path("bflibrary/src/x86-win-sdl2/sscreen.c").read_text()
begin = source.index("static void LbI_SDL_BlitScaled_to8bpp(")
end = source.index("/** Check if the lbDrawSurface", begin)
scaler = source[begin:end]
header = r"""
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char ubyte;
#define LOGERR(...) ((void)fprintf(stderr, __VA_ARGS__))
"""
tests = r"""
static int check(SDL_Surface *src, SDL_Surface *dst, int check_padding)
{
    int bpp = dst->format->BytesPerPixel;
    unsigned char *p=(unsigned char *)dst->pixels;
    int checks=0;
    memset(p, 0xa7, (size_t)dst->pitch*dst->h);
    if (LbI_SDL_BlitScaled(src, dst) != 0) {
        fprintf(stderr, "Blit failure: %s\n", SDL_GetError()); return 1;
    }
    if (check_padding) {
        for (int y=0;y<dst->h;y++) {
            for (int k=dst->w*bpp;k<dst->pitch;k++) {
                if (p[y*dst->pitch+k]!=0xa7) {
                    fprintf(stderr, "Padding corrupted, y=%d offset=%d\n",y,k);return 2;
                }
            }
        }
    }
    /* First source pixel has index 1 and its palette's first channel is 1;
       output should have been written, not left as canary. */
    for(int y=0;y<dst->h;y++)
        for(int x=0;x<dst->w;x++)
            for(int k=0;k<bpp;k++)
                if(p[y*dst->pitch+x*bpp+k]!=0xa7) checks++;
    if(!checks) {fprintf(stderr,"No converted pixels\n");return 3;}
    return 0;
}
int main(int argc,char **argv)
{
    (void)argc;(void)argv;
    SDL_Surface *source=NULL,*dst=NULL,*windowsrc=NULL;
    SDL_Window *window=NULL;
    int count=0;
    if (SDL_Init(SDL_INIT_VIDEO)<0) {fprintf(stderr,"SDL_Init: %s\n",SDL_GetError());return 10;}
    source=SDL_CreateRGBSurface(0,320,200,8,0,0,0,0);
    if(!source) return 11;
    SDL_Color colors[256];
    for(int i=0;i<256;i++)
        colors[i]=(SDL_Color){(Uint8)i,(Uint8)(255-i),(Uint8)(i^0x5a),255};
    if(SDL_SetPaletteColors(source->format->palette,colors,0,256)<0)return 12;
    for(int y=0;y<200;y++)for(int x=0;x<320;x++)
        ((unsigned char *)source->pixels)[y*source->pitch+x]=(unsigned char)(1+(x+y*3)%255);
    for(int bpp=1;bpp<=4;bpp++)
        for(int width=3;width<=7;width+=2) {
            const int height=7;
            int bitdepth=bpp*8;
            dst=SDL_CreateRGBSurface(0,width,height,bitdepth,0,0,0,0);
            if (!dst) {fprintf(stderr,"Create bpp=%d: %s\n",bpp,SDL_GetError());return 13;}
            int fail=check(source,dst,1);
            if(fail) return fail;
            SDL_FreeSurface(dst);dst=NULL;
            count++;
        }
    window=SDL_CreateWindow("isolated wrapper SDL2 integration",0,0,640,400,SDL_WINDOW_HIDDEN);
    if(!window) {fprintf(stderr,"SDL_CreateWindow: %s\n",SDL_GetError());return 14;}
    windowsrc=SDL_GetWindowSurface(window);
    if(!windowsrc) {fprintf(stderr,"SDL_GetWindowSurface: %s\n",SDL_GetError());return 15;}
    if(check(source,windowsrc,0)) return 16;
    printf("PASS: %d source-extracted SDL2 wrapper surface cases plus 640x400 window blit (window bpp=%d pitch=%d)\n",
      count,windowsrc->format->BytesPerPixel,windowsrc->pitch);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(source);
    SDL_Quit();
    return count==12?0:17;
}
"""
Path("graphics_sdl2_wrapper_integration.c").write_text(header+scaler+tests)
print("Generated exact-source SDL2 indexed scaler wrapper integration test")
