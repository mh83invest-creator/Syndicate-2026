#include "eg_environment_face.h"

static uint32_t get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1]<<8)
         | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);
}

/* Scale2x only picks neighbours of the same original mask class as
 * the centre. This avoids introducing a colour from across a face edge.
 * The mask itself is never filtered: exact nearest-neighbour propagation.
 */
static void scale2x(const uint8_t *src, const uint8_t *mask,
                    size_t w, size_t h, uint8_t *dst)
{
    size_t y, x;
    for (y=0; y<h; y++) for (x=0; x<w; x++) {
        const size_t i=y*w+x;
        const uint8_t e=src[i], m=mask[i];
        const uint8_t b=(y>0 && mask[i-w]==m) ? src[i-w] : e;
        const uint8_t d=(x>0 && mask[i-1]==m) ? src[i-1] : e;
        const uint8_t f=(x+1<w && mask[i+1]==m) ? src[i+1] : e;
        const uint8_t hpx=(y+1<h && mask[i+w]==m) ? src[i+w] : e;
        const size_t o=(2*y)*(2*w)+2*x;
        dst[o]=dst[o+1]=dst[o+2*w]=dst[o+2*w+1]=e;
        if (b!=hpx && d!=f) {
            dst[o]       = (d==b) ? d : e;
            dst[o+1]     = (b==f) ? f : e;
            dst[o+2*w]   = (d==hpx) ? d : e;
            dst[o+2*w+1] = (hpx==f) ? f : e;
        }
    }
}

int eg_expand_environment_face(const uint8_t *face, size_t face_bytes,
                               const uint32_t rgba[16],
                               uint32_t *out_pixels, size_t out_pixels_count,
                               uint8_t *out_mask, size_t out_mask_count)
{
    uint8_t idx[EG_FACE_W*EG_FACE_H];
    uint8_t mask[EG_FACE_W*EG_FACE_H];
    uint8_t idx2[64*32],mask2[64*32],idx4[EG_FACE_OUTPUT_PIXELS];
    size_t x,y,k;
    if (!face || !rgba || !out_pixels || !out_mask
        || face_bytes != EG_FACE_BYTES
        || out_pixels_count < EG_FACE_OUTPUT_PIXELS
        || out_mask_count < EG_FACE_OUTPUT_PIXELS) return 0;
    for (y=0; y<EG_FACE_H; y++) {
        uint32_t w[5];
        for (k=0; k<5; k++) w[k]=get_le32(face+y*20+k*4);
        for (x=0; x<EG_FACE_W; x++) {
            uint32_t bit=1u<<(31-x);
            size_t i=y*EG_FACE_W+x;
            idx[i]=(uint8_t)(((w[1]&bit)?1u:0u)|((w[2]&bit)?2u:0u)|
                             ((w[3]&bit)?4u:0u)|((w[4]&bit)?8u:0u));
            mask[i]=(uint8_t)((w[0]&bit)?1:0);
        }
    }
    for (y=0; y<EG_FACE_H; y++) for (x=0; x<EG_FACE_W; x++) {
        const uint8_t v=mask[y*EG_FACE_W+x];
        size_t dy,dx;
        for(dy=0;dy<2;dy++) for(dx=0;dx<2;dx++)
            mask2[(2*y+dy)*64+2*x+dx]=v;
    }
    scale2x(idx,mask,EG_FACE_W,EG_FACE_H,idx2);
    scale2x(idx2,mask2,64,32,idx4);
    for (y=0; y<EG_FACE_H; y++) for (x=0; x<EG_FACE_W; x++) {
        const uint8_t v=mask[y*EG_FACE_W+x];
        size_t dy,dx;
        for(dy=0;dy<4;dy++) for(dx=0;dx<4;dx++)
            out_mask[(4*y+dy)*EG_FACE_OUT_W+4*x+dx]=v;
    }
    for (k=0;k<EG_FACE_OUTPUT_PIXELS;k++)
        out_pixels[k]=rgba[idx4[k]];
    return 1;
}
