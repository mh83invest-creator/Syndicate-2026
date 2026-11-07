#include "eg_environment_legacy_compose.h"
static uint32_t read_le32(const uint8_t *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8)
        |((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
int eg_compose_legacy_face(const uint8_t *src, size_t size,
                           uint32_t *planes, size_t word_count) {
    size_t y,k;
    if (!src || !planes || size!=EG_LEGACY_FACE_BYTES
        || word_count<EG_LEGACY_PLANE_WORDS) return 0;
    for (y=0;y<16;y++) {
        const uint8_t *row=src+y*20u;
        const uint32_t mask=read_le32(row);
        for(k=0;k<4;k++)
            planes[k*16u+y]=(planes[k*16u+y]&mask)
                            |read_le32(row+4u*(k+1u));
    }
    return 1;
}
