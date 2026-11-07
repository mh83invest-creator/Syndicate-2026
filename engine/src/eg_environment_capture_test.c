#include "eg_environment_capture.h"
#include <stdio.h>
#include <string.h>
#define EG_CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    unsigned char planes[4*32000u]={0};
    unsigned char pixels[3*3]={0,1,1,0,1,0,1,1,0};
    unsigned char scaled[6*6]={0};
    /* Leftmost pixel of first 32-bit planar word uses bit7 of byte0. */
    planes[0]=0x80;
    planes[32000]=0x40;
    planes[64000]=0x20;
    planes[96000]=0x10;
    EG_CHECK(eg_planar_pixel(planes,0,0)==1);
    EG_CHECK(eg_planar_pixel(planes,1,0)==2);
    EG_CHECK(eg_planar_pixel(planes,2,0)==4);
    EG_CHECK(eg_planar_pixel(planes,3,0)==8);
    EG_CHECK(eg_planar_pixel(planes,4,0)==0);
    EG_CHECK(eg_planar_pixel(planes,640,0)==0);
    EG_CHECK(eg_planar_pixel(planes,0,400)==0);
    EG_CHECK(eg_planar_pixel(NULL,0,0)==0);
    eg_scale2x_indices(pixels,3,3,scaled);
    EG_CHECK(scaled[0]==0);
    EG_CHECK(scaled[5]==1);
    EG_CHECK(scaled[30]==1);
    /* An all-uniform surface remains unchanged after enlargement. */
    memset(pixels,7,sizeof(pixels));
    eg_scale2x_indices(pixels,3,3,scaled);
    for(int i=0;i<36;i++)EG_CHECK(scaled[i]==7);
    EG_CHECK(eg_environment_capture_requested==0);
    puts("PASS: EG-v0.1 original 4-plane map extraction and scale2x");
    return 0;
}
