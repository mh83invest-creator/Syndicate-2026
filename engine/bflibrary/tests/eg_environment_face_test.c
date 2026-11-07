#include "eg_environment_face.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ASSERT(p) do { if (!(p)) {     fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#p); return 1; } } while(0)
#define CANARY 0xABEF2931u
static void put_le32(uint8_t *p,uint32_t n){
    p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);
    p[2]=(uint8_t)(n>>16);p[3]=(uint8_t)(n>>24);
}
int main(void)
{
    uint8_t face[EG_FACE_BYTES]={0};
    uint32_t lut[16],pixels[EG_FACE_OUTPUT_PIXELS+2];
    uint8_t mask[EG_FACE_OUTPUT_PIXELS+2];
    uint32_t pixels_again[EG_FACE_OUTPUT_PIXELS];
    uint8_t mask_again[EG_FACE_OUTPUT_PIXELS];
    int x,y,k;
    for(k=0;k<16;k++)lut[k]=(uint32_t)(0xFF000000u|((uint32_t)k<<16)|((uint32_t)k<<8)|k);
    ASSERT(!eg_expand_environment_face(face,EG_FACE_BYTES-1,lut,pixels,EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS));
    ASSERT(!eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels,EG_FACE_OUTPUT_PIXELS-1,mask,EG_FACE_OUTPUT_PIXELS));
    ASSERT(!eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels,EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS-1));
    ASSERT(!eg_expand_environment_face(NULL,EG_FACE_BYTES,lut,pixels,EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS));
    for (y=0;y<EG_FACE_H;y++) {
        /* Uniform index 10, all four bit planes deterministically defined. */
        put_le32(face+y*20+0,0xAAAAAAAAu); /* raw alternating mask */
        put_le32(face+y*20+4,0);
        put_le32(face+y*20+8,0xFFFFFFFFu);
        put_le32(face+y*20+12,0);
        put_le32(face+y*20+16,0xFFFFFFFFu);
    }
    pixels[EG_FACE_OUTPUT_PIXELS]=pixels[EG_FACE_OUTPUT_PIXELS+1]=CANARY;
    mask[EG_FACE_OUTPUT_PIXELS]=mask[EG_FACE_OUTPUT_PIXELS+1]=0x7B;
    ASSERT(eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels,EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS));
    for (k=0;k<EG_FACE_OUTPUT_PIXELS;k++) {
        ASSERT(pixels[k]==lut[10]);
        ASSERT(mask[k]==(((k/4)%EG_FACE_W)%2==0));
    }
    ASSERT(pixels[EG_FACE_OUTPUT_PIXELS]==CANARY && pixels[EG_FACE_OUTPUT_PIXELS+1]==CANARY);
    ASSERT(mask[EG_FACE_OUTPUT_PIXELS]==0x7B && mask[EG_FACE_OUTPUT_PIXELS+1]==0x7B);
    /* Deterministic edge pattern and varying mask; no out-of-bounds writes */
    for (y=0;y<EG_FACE_H;y++) for (x=0;x<EG_FACE_W;x++) {
        const uint32_t bit=1u<<(31-x);
        int idx=(x+y*7)&15, m=(x<16);
        int b;
        for (b=0;b<5;b++) {
            uint8_t *p=face+y*20+b*4;
            uint32_t n=(uint32_t)p[0]|((uint32_t)p[1]<<8)|
                       ((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
            if ((b==0 && m) || (b>0 && (idx&(1<<(b-1))))) n|=bit;
            else n&=~bit;
            put_le32(p,n);
        }
    }
    ASSERT(eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels,EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS));
    ASSERT(eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels_again,EG_FACE_OUTPUT_PIXELS,mask_again,EG_FACE_OUTPUT_PIXELS));
    ASSERT(memcmp(pixels,pixels_again,sizeof(pixels_again))==0);
    ASSERT(memcmp(mask,mask_again,sizeof(mask_again))==0);
    for(y=0;y<EG_FACE_H;y++)for(x=0;x<EG_FACE_W;x++){
        for(int dy=0;dy<4;dy++)for(int dx=0;dx<4;dx++)
            ASSERT(mask[(4*y+dy)*EG_FACE_OUT_W+4*x+dx]==(x<16));
    }
    ASSERT(pixels[EG_FACE_OUTPUT_PIXELS]==CANARY && pixels[EG_FACE_OUTPUT_PIXELS+1]==CANARY);
    /* Byte-order regression: x86 mov loads little-endian 32-bit words.
     * A single source byte 0x80 at byte +7 sets plane 0 bit 31 (x=0),
     * NOT bit 7 (x=24) as a big-endian decoder would imply.
     */
    memset(face,0,sizeof(face));
    face[7]=0x80; /* row 0, first colour plane, highest address byte */
    ASSERT(eg_expand_environment_face(face,EG_FACE_BYTES,lut,pixels,
           EG_FACE_OUTPUT_PIXELS,mask,EG_FACE_OUTPUT_PIXELS));
    for (int dy=0;dy<4;dy++) for (int dx=0;dx<4;dx++)
        ASSERT(pixels[dy*EG_FACE_OUT_W+dx]==lut[1]);
    ASSERT(pixels[0*EG_FACE_OUT_W+24*4]==lut[0]);
    ASSERT(pixels[0*EG_FACE_OUT_W+31*4]==lut[0]);
    ASSERT(pixels[4*EG_FACE_OUT_W+0]==lut[0]);
    ASSERT(pixels[EG_FACE_OUTPUT_PIXELS]==CANARY);
    puts("PASS: EG-v0.1 environment face x86 little-endian plane order, exact 4x mask, palette, boundaries and deterministic Scale2x");
    return 0;
}
