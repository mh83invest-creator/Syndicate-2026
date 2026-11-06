/* Reference functions from the unmodified graphics audit branch sscreen.c. */
static void LbI_SDL_BlitScaled_to8bpp(long src_w, long src_h, ubyte *src_buf,
  long dst_w, long dst_h, ubyte *dst_buf)
{
    // denominator of a (source) pixel's fraction part
    const long denom_i = 2 * dst_h;
    const long denom_j = 2 * dst_w;

    // number of whole units in each (source) step
    const long dsrc_i = 2 * src_h / denom_i;
    const long dsrc_j = 2 * src_w / denom_j;

    // numerator of fractional part of each (source) step
    const long dsrc_num_i = (2 * src_h) - dsrc_i * denom_i;
    const long dsrc_num_j = (2 * src_w) - dsrc_j * denom_j;

    // number of whole units in a (source) half-step
    const long halfdsrc_i = src_h / denom_i;
    const long halfdsrc_j = src_w / denom_j;

    long dst_offset = 0;
    long src_offset = halfdsrc_i * src_w + halfdsrc_j;

    // start at fractional part of each (source) half-step
    long src_num_i =  src_h - halfdsrc_i * denom_i;
    long src_num_j = src_w - halfdsrc_j * denom_j;

    for (long i = 0; i != dst_h; ++i) {
        if (src_num_i > denom_i) {
            src_num_i -= denom_i;
            src_offset += src_w;
        }
        for (long j = 0; j != dst_w; ++j) {
            if (src_num_j > denom_j) {
                src_num_j -= denom_j;
                ++src_offset;
            }
            dst_buf[dst_offset] = src_buf[src_offset];
            dst_offset++;
            src_offset += dsrc_j;
            src_num_j += dsrc_num_j;
        }
        src_offset += (dsrc_i - 1) * src_w;
        src_num_i += dsrc_num_i;
    }
}

static void LbI_SDL_BlitScaled_totcbpp(long src_w, long src_h, ubyte *src_buf,
  SDL_Color *pal, long rshift, long gshift, long bshift,
  long dst_w, long dst_h, long dst_bpp, ubyte *dst_buf)
{
    // denominator of a (source) pixel's fraction part
    const long denom_i = 2 * dst_h;
    const long denom_j = 2 * dst_w;

    // number of whole units in each (source) step
    const long dsrc_i = 2 * src_h / denom_i;
    const long dsrc_j = 2 * src_w / denom_j;

    // numerator of fractional part of each (source) step
    const long dsrc_num_i = (2 * src_h) - dsrc_i * denom_i;
    const long dsrc_num_j = (2 * src_w) - dsrc_j * denom_j;

    // number of whole units in a (source) half-step
    const long halfdsrc_i = src_h / denom_i;
    const long halfdsrc_j = src_w / denom_j;

    long dst_offset = 0;
    long src_offset = halfdsrc_i * src_w + halfdsrc_j;

    // start at fractional part of each (source) half-step
    long src_num_i =  src_h - halfdsrc_i * denom_i;
    long src_num_j = src_w - halfdsrc_j * denom_j;

    for (long i = 0; i != dst_h; ++i) {
        if (src_num_i > denom_i) {
            src_num_i -= denom_i;
            src_offset += src_w;
        }
        for (long j = 0; j != dst_w; ++j) {
            SDL_Color c;
            if (src_num_j > denom_j) {
                src_num_j -= denom_j;
                ++src_offset;
            }
            c = pal[src_buf[src_offset]];
            *((long *)(dst_buf+dst_offset)) = (c.r << rshift) + (c.g << gshift) + (c.b << bshift);
            dst_offset += dst_bpp;
            src_offset += dsrc_j;
            src_num_j += dsrc_num_j;
        }
        src_offset += (dsrc_i - 1) * src_w;
        src_num_i += dsrc_num_i;
    }
}

