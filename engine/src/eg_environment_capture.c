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

static int eg_write_ppm(const char *name,const unsigned char *idx,
                         unsigned w,unsigned h,const unsigned char *palette)
{
    FILE *f=fopen(name,"wb");
    unsigned char scan[EG_WIDTH*4u*3u];
    unsigned x,y;
    if(!f) return 0;
    if(fprintf(f,"P6\n%u %u\n255\n",w,h)<0){fclose(f);return 0;}
    for(y=0;y<h;y++){
        for(x=0;x<w;x++){
            unsigned color=(unsigned)idx[(size_t)y*w+x];
            scan[3u*x+0]=(unsigned char)((palette[color*3u+0]&63u)<<2);
            scan[3u*x+1]=(unsigned char)((palette[color*3u+1]&63u)<<2);
            scan[3u*x+2]=(unsigned char)((palette[color*3u+2]&63u)<<2);
        }
        if(fwrite(scan,1,(size_t)w*3u,f)!=(size_t)w*3u){
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
    original_ok=eg_write_ppm("EG_environment_original.ppm",native,
                              EG_WIDTH,EG_HEIGHT,palette);
    enhanced_ok=eg_write_ppm("EG_environment_enhanced_4x.ppm",four,
                              EG_WIDTH*4u,EG_HEIGHT*4u,palette);
    fprintf(stderr,"EG-v0.1: original environment capture %s; 4x comparison %s\n",
            original_ok?"saved":"FAILED",enhanced_ok?"saved":"FAILED");
cleanup:
    free(four);free(two);free(native);
}
