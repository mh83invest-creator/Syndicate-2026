/* EG-v0.1 diagnostic only: original x86 block-face AND/OR model.
 * Not connected to the production renderer.
 */
#ifndef EG_ENVIRONMENT_LEGACY_COMPOSE_H
#define EG_ENVIRONMENT_LEGACY_COMPOSE_H
#include <stddef.h>
#include <stdint.h>
#define EG_LEGACY_FACE_BYTES 320u
#define EG_LEGACY_PLANE_WORDS 64u
/* Four destination bitplanes with 16 words each, plane-major order. */
int eg_compose_legacy_face(const uint8_t *src, size_t size,
                           uint32_t *planes, size_t word_count);
#endif
