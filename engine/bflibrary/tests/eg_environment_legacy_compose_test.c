#include "eg_environment_legacy_compose.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static void le32(uint8_t *p,uint32_t n) {
    p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);
    p[2]=(uint8_t)(n>>16);p[3]=(uint8_t)(n>>24);
}
int main(void) {
    uint8_t face[320]={0}, transparent[320]={0};
    uint32_t planes[66], snapshot[64];
    for(int i=0;i<66;i++) planes[i]=0xffffffffu;
    planes[64]=0x12345678u; planes[65]=0x87654321u;
    CHECK(!eg_compose_legacy_face(face,319,planes,66));
    CHECK(!eg_compose_legacy_face(face,320,planes,63));
    CHECK(!eg_compose_legacy_face(NULL,320,planes,66));
    CHECK(!eg_compose_legacy_face(face,320,NULL,66));
    for(int row=0;row<16;row++) {
        le32(face+row*20,~1u);
        le32(face+row*20+4,1u);
        for(int col=2;col<5;col++) le32(face+row*20+4*col,0);
        le32(transparent+row*20,0xffffffffu);
    }
    CHECK(eg_compose_legacy_face(face,320,planes,66));
    for(int plane=0;plane<4;plane++)for(int row=0;row<16;row++) {
        const uint32_t expected=plane==0?0xffffffffu:0xfffffffeu;
        CHECK(planes[plane*16+row]==expected);
    }
    CHECK(planes[64]==0x12345678u && planes[65]==0x87654321u);
    memcpy(snapshot,planes,sizeof(snapshot));
    CHECK(eg_compose_legacy_face(transparent,320,planes,66));
    CHECK(memcmp(snapshot,planes,sizeof(snapshot))==0);
    for(int row=0;row<16;row++) {
        le32(face+row*20,0u);
        for(int col=1;col<5;col++) le32(face+row*20+4*col,
            col==3?0xffffffffu:0u);
    }
    CHECK(eg_compose_legacy_face(face,320,planes,66));
    for(int plane=0;plane<4;plane++)for(int row=0;row<16;row++)
        CHECK(planes[plane*16+row]==(plane==2?0xffffffffu:0u));
    CHECK(planes[64]==0x12345678u && planes[65]==0x87654321u);
    puts("PASS: original x86 environment block mask-AND colour-OR layer order and bounds");
    return 0;
}
