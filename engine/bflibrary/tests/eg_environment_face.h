/* EG-v0.1 experimental, environment-only offline block face expander.
 * This module is NOT connected to the production renderer.
 * The 320-byte source layout is the audited candidate: 16 rows of five
 * little-endian 32-bit x86 words (mask, then four colour bitplanes).
 * Mask bits are preserved as uninterpreted bits, not assumed opacity.
 */
#ifndef EG_ENVIRONMENT_FACE_H
#define EG_ENVIRONMENT_FACE_H
#include <stdint.h>
#include <stddef.h>
#define EG_FACE_W 32
#define EG_FACE_H 16
#define EG_FACE_OUT_W 128
#define EG_FACE_OUT_H 64
#define EG_FACE_BYTES 320
#define EG_FACE_OUTPUT_PIXELS (EG_FACE_OUT_W * EG_FACE_OUT_H)
/* Returns 1 on success, 0 on invalid arguments.
 * rgba = caller-supplied exact 16-entry RGBA lookup table; this function
 * does not decide which mission palette is active.
 * out_mask preserves the source mask bits at 4x resolution, independently
 * of the colour enhancer. Never use this mask as alpha without checking
 * its actual original assembly composition semantics.
 */
int eg_expand_environment_face(const uint8_t *face, size_t face_bytes,
                               const uint32_t rgba[16],
                               uint32_t *out_pixels, size_t out_pixels_count,
                               uint8_t *out_mask, size_t out_mask_count);
#endif
