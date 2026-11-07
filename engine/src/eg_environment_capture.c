/* Syndicate 2026 EG-v0.1 — opt-in environment-only draw-stage capture.
 * No renderer pixel changes. Captures 640x400 four-plane map buffer at
 * the completed full map redraw boundary, before any later draw work.
 * Output is PPM so this requires no third-party image libraries.
 */
#include "eg_environment_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char eg_environment_capture_requested = 0;

#define EG_WIDTH 640u
#define EG_HEIGHT 400u
#define EG_PITCH 80u
#define EG_PLANE_BYTES 32000u

unsigned char eg_planar_pixel(const unsigned char *planes,
                              unsigned x, unsigned y)
{
    unsigned char value=0;
    size_t offset=(size_t)y*EG_PITCH+x/8u;
    unsigned bit=7u-(x%8u);
    unsigned plane;
    if (!planes || x>=EG_WIDTH || y>=EG_HEIGHT) return 0;
    for (plane=0;plane<4;plane++)
        value|=(unsigned char)((((planes[plane*EG_PLANE_BYTES+offset]>>bit)&1u))<<plane);
    return value;
}

void eg_scale2x_indices(const unsigned char *src, unsigned w, unsigned h,
                        unsigned char *dst)
{
    unsigned x,y;
    if (!src || !dst || w==0 || h==0) return;
    for(y=0;y<h;y++)for(x=0;x<w;x++){
        size_t i=(size_t)y*w+x;
        unsigned char e=src[i];
        unsigned char b=y?src[i-w]:e;
        unsigned char d=x?src[i-1]:e;
        unsigned char f=x+1<w?src[i+1]:e;
        unsigned char hpx=y+1<h?src[i+w]:e;
        size_t o=(size_t)(y*2u)*(w*2u)+x*2u;
        dst[o]=dst[o+1]=dst[o+w*2u]=dst[o+w*2u+1]=e;
        if (b!=hpx && d!=f) {
            dst[o]=(b==d)?d:e;
            dst[o+1]=(b==f)?f:e;
            dst[o+w*2u]=(d==hpx)?d:e;
            dst[o+w*2u+1]=(hpx==f)?f:e;
        }
    }
}

static void eg_le16(unsigned char *p,unsigned value)
{
    p[0]=(unsigned char)value;
    p[1]=(unsigned char)(value>>8);
}
static void eg_le32(unsigned char *p,unsigned value)
{
    p[0]=(unsigned char)value;
    p[1]=(unsigned char)(value>>8);
    p[2]=(unsigned char)(value>>16);
    p[3]=(unsigned char)(value>>24);
}

/* Uncompressed 24-bit Windows BMP: bottom-up BGR rows, 4-byte row alignment. */
static int eg_write_bmp(const char *name,const unsigned char *idx,
                        unsigned w,unsigned h,const unsigned char *palette)
{
    FILE *f;
    unsigned char header[54]={0},scan[EG_WIDTH*4u*3u+3u];
    unsigned rowbytes=(w*3u+3u)&~3u;
    unsigned x,y;
    if(!idx||!palette||w==0||h==0||w>EG_WIDTH*4u||h>EG_HEIGHT*4u)
        return 0;
    f=fopen(name,"wb");
    if(!f) return 0;
    header[0]='B';header[1]='M';
    eg_le32(header+2u,54u+rowbytes*h);
    eg_le32(header+10u,54u);
    eg_le32(header+14u,40u);
    eg_le32(header+18u,w);
    eg_le32(header+22u,h);
    eg_le16(header+26u,1u);
    eg_le16(header+28u,24u);
    eg_le32(header+34u,rowbytes*h);
    if(fwrite(header,1,sizeof(header),f)!=sizeof(header)){
        fclose(f);remove(name);return 0;
    }
    for(y=h;y>0;y--){
        unsigned source_y=y-1u;
        memset(scan,0,rowbytes);
        for(x=0;x<w;x++){
            unsigned color=(unsigned)idx[(size_t)source_y*w+x];
            if(color>15u){fclose(f);remove(name);return 0;}
            scan[3u*x+0u]=(unsigned char)((palette[color*3u+2u]&63u)<<2);
            scan[3u*x+1u]=(unsigned char)((palette[color*3u+1u]&63u)<<2);
            scan[3u*x+2u]=(unsigned char)((palette[color*3u+0u]&63u)<<2);
        }
        if(fwrite(scan,1,rowbytes,f)!=rowbytes){
            fclose(f);remove(name);return 0;
        }
    }
    if(fclose(f)!=0){remove(name);return 0;}
    return 1;
}

void eg_capture_environment_once(const unsigned char *planes,
                                 const unsigned char *palette)
{
    unsigned char *native=NULL,*two=NULL,*four=NULL;
    unsigned x,y;
    int original_ok=0,enhanced_ok=0;
    if(!eg_environment_capture_requested) return;
    eg_environment_capture_requested=0;
    if(!planes||!palette) {
        fprintf(stderr,"EG-v0.1: environment capture unavailable (null buffers)\n");
        return;
    }
    native=(unsigned char *)malloc((size_t)EG_WIDTH*EG_HEIGHT);
    two=(unsigned char *)malloc((size_t)EG_WIDTH*EG_HEIGHT*4u);
    four=(unsigned char *)malloc((size_t)EG_WIDTH*EG_HEIGHT*16u);
    if(!native||!two||!four){
        fprintf(stderr,"EG-v0.1: capture allocation failed\n");
        goto cleanup;
    }
    for(y=0;y<EG_HEIGHT;y++)for(x=0;x<EG_WIDTH;x++)
        native[(size_t)y*EG_WIDTH+x]=eg_planar_pixel(planes,x,y);
    eg_scale2x_indices(native,EG_WIDTH,EG_HEIGHT,two);
    eg_scale2x_indices(two,EG_WIDTH*2u,EG_HEIGHT*2u,four);
    original_ok=eg_write_bmp("EG_environment_original.bmp",native,
                              EG_WIDTH,EG_HEIGHT,palette);
    enhanced_ok=eg_write_bmp("EG_environment_enhanced_4x.bmp",four,
                              EG_WIDTH*4u,EG_HEIGHT*4u,palette);
    fprintf(stderr,"EG-v0.1: original environment capture %s; 4x comparison %s\n",
            original_ok?"saved":"FAILED",enhanced_ok?"saved":"FAILED");
cleanup:
    free(four);free(two);free(native);
}
