#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Isolated layout contract test, NOT the actual engine function.
   Pixel sample positions deliberately simple; tests pitch, pixel width and padding. */
static void draw(int sw, int sh, int sp, const unsigned char *src,
  int dw, int dh, int dp, int bpp, unsigned char *dst)
{
    for (int y = 0; y < dh; ++y)
        for (int x = 0; x < dw; ++x) {
            int sx = x * sw / dw, sy = y * sh / dh;
            uint32_t p = (uint32_t)src[sy * sp + sx] * 0x010101u;
            memcpy(dst + y * dp + x * bpp, &p, (size_t)bpp);
        }
}
int main(void)
{
    int tests = 0;
    for (int bpp = 1; bpp <= 4; ++bpp)
        for (int w = 3; w <= 7; w += 2) {
            const int dh = 7, sp = 8, dp = w * bpp + 5;
            unsigned char src[24], *dst = malloc((size_t)dp * dh + 16);
            if (!dst) return 1;
            memset(src, 0, sizeof(src));
            memset(dst, 0xa7, (size_t)dp * dh + 16);
            for (int y = 0; y < 3; ++y)
                for (int x = 0; x < 5; ++x)
                    src[y * sp + x] = (unsigned char)(y * 5 + x + 1);
            draw(5, 3, sp, src, w, dh, dp, bpp, dst);
            for (int y = 0; y < dh; ++y) {
                for (int x = 0; x < w; ++x)
                    for (int b = 0; b < bpp; ++b) {
                        unsigned char expected = b == 3 ? 0 :
                          src[(y * 3 / dh) * sp + (x * 5 / w)];
                        if (dst[y * dp + x * bpp + b] != expected) return 2;
                    }
                for (int k = w * bpp; k < dp; ++k)
                    if (dst[y * dp + k] != 0xa7) return 3;
            }
            for (int k = 0; k < 16; ++k)
                if (dst[dp * dh + k] != 0xa7) return 4;
            free(dst);
            ++tests;
        }
    printf("PASS: %d isolated pitch/width/padding cases\n", tests);
    return tests == 12 ? 0 : 5;
}
