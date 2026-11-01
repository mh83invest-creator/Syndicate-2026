#!/usr/bin/env python3
"""Compile and exercise the ACTUAL SDL2 indexed scaler helpers, extracted unchanged."""
from pathlib import Path
src=Path("bflibrary/src/x86-win-sdl2/sscreen.c").read_text()
start=src.index("static void LbI_SDL_BlitScaled_to8bpp(")
end=src.index("/** @internal", start)
functions=src[start:end]
head="""#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char ubyte;
typedef struct { ubyte r,g,b,a; } SDL_Color;
"""
tail=r"""
static int verify(int bpp,int width,int dst_pitch) {
    enum { SW=5, SH=3, DH=7, SRC_PITCH=8 };
    unsigned char src[SRC_PITCH*SH];
    SDL_Color pal[256];
    unsigned char *dst=(unsigned char*)malloc(dst_pitch*DH+32);
    int failures=0;
    if (!dst) return 100;
    memset(src,0,sizeof src); memset(dst,0xa7,dst_pitch*DH+32);
    for(int y=0;y<SH;y++) for(int x=0;x<SW;x++) src[y*SRC_PITCH+x]=(unsigned char)(1+x+y*SW);
    for(int i=0;i<256;i++) pal[i]=(SDL_Color){(ubyte)i,(ubyte)i,(ubyte)i,255};
    if (bpp==1)
        LbI_SDL_BlitScaled_to8bpp(SW,SH,src,width,DH,SRC_PITCH,dst_pitch,dst);
    else
        LbI_SDL_BlitScaled_totcbpp(SW,SH,src,pal,0,8,16,width,DH,SRC_PITCH,dst_pitch,bpp,dst);
    for(int y=0;y<DH;y++){
        for(int j=width*bpp;j<dst_pitch;j++) if(dst[y*dst_pitch+j]!=0xa7) failures++;
    }
    for(int j=0;j<32;j++) if(dst[dst_pitch*DH+j]!=0xa7) failures++;
    /* Nontrivial source contents should produce coloured / indexed output. */
    for(int y=0;y<DH;y++) for(int x=0;x<width;x++)
        if (dst[y*dst_pitch+x*bpp]==0xa7) failures++;
    free(dst);
    return failures;
}
int main(void) {
    int cases=0;
    for (int bpp=1;bpp<=4;bpp++)
      for(int w=3;w<=7;w+=2){
        int pitch=w*bpp+5;
        int fail=verify(bpp,w,pitch);
        if(fail){fprintf(stderr,"FAIL actual scaler bpp=%d width=%d errors=%d\n",bpp,w,fail);return 1;}
        cases++;
      }
    printf("PASS: %d actual engine scaler pitch, pixel and canary cases\n",cases);
    return cases==12?0:2;
}
"""
Path("graphics_actual_scaler_test.c").write_text(head+functions+tail)
print("Extracted indexed scaler functions directly from sscreen.c")
