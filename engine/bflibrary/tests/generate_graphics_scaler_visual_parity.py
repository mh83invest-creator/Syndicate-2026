#!/usr/bin/env python3
"""Generate Win32 byte-for-byte parity test from original and repaired scaler source."""
from pathlib import Path
import re

current=Path("bflibrary/src/x86-win-sdl2/sscreen.c").read_text()
start=current.index("static void LbI_SDL_BlitScaled_to8bpp(")
end=current.index("/** @internal",start)
new=current[start:end].replace("LbI_SDL_BlitScaled_to8bpp","repaired_to8bpp").replace("LbI_SDL_BlitScaled_totcbpp","repaired_totcbpp")
original=Path("bflibrary/tests/graphics_scaler_legacy_reference.inc").read_text()
original=original.replace("LbI_SDL_BlitScaled_to8bpp","legacy_to8bpp").replace("LbI_SDL_BlitScaled_totcbpp","legacy_totcbpp")
header="""#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef unsigned char ubyte;
typedef struct { ubyte r,g,b,a; } SDL_Color;
"""
test=r"""
static int check(int w,int h,int bpp)
{
    const int sw=320,sh=200,src_pitch=320,pitch=w*bpp;
    const size_t n=(size_t)pitch*h;
    ubyte *src=malloc((size_t)src_pitch*sh),*legacy=malloc(n),*newbuf=malloc(n);
    SDL_Color palette[256];
    if(!src||!legacy||!newbuf) return 11;
    if(sizeof(long)!=4) {fprintf(stderr,"FAIL: comparison requires Windows 32-bit long\n");return 12;}
    for(int i=0;i<src_pitch*sh;i++) src[i]=(ubyte)((i*37+(i/320)*19)%256);
    for(int i=0;i<256;i++) palette[i]=(SDL_Color){(ubyte)i,(ubyte)(255-i),(ubyte)(i^0x5a),255};
    memset(legacy,0xc7,n);memset(newbuf,0xc7,n);
    if(bpp==1) {
        legacy_to8bpp(sw,sh,src,w,h,legacy);
        repaired_to8bpp(sw,sh,src,w,h,src_pitch,pitch,newbuf);
    } else {
        legacy_totcbpp(sw,sh,src,palette,16,8,0,w,h,4,legacy);
        repaired_totcbpp(sw,sh,src,palette,16,8,0,w,h,src_pitch,pitch,4,newbuf);
    }
    if(memcmp(legacy,newbuf,n)!=0){
        size_t k;for(k=0;k<n;k++)if(legacy[k]!=newbuf[k])break;
        fprintf(stderr,"FAIL parity w=%d h=%d bpp=%d offset=%lu old=%u new=%u\n",
          w,h,bpp,(unsigned long)k,(unsigned)legacy[k],(unsigned)newbuf[k]);
        return 13;
    }
    free(src);free(legacy);free(newbuf);
    return 0;
}
int main(void)
{
    const int dims[][2]={{640,400},{400,250},{800,500}};
    int count=0;
    for(size_t i=0;i<sizeof dims/sizeof dims[0];i++)
        for(int bpp=1;bpp<=4;bpp+=3){
            int ret=check(dims[i][0],dims[i][1],bpp);
            if(ret) return ret;
            count++;
        }
    printf("PASS: %d byte-exact original vs repaired SDL2 scaler cases\n",count);
    return 0;
}
"""
Path("graphics_scaler_visual_parity.c").write_text(header+original+new+test)
print("Generated Win32 reference/candidate byte-for-byte parity test")
