/* Syndicate 2026 EG-v0.1: experimental environment capture hook.
 * This is an observation hook, NOT live output substitution or new game art.
 */
#ifndef SYNDICATE_EG_ENVIRONMENT_CAPTURE_H
#define SYNDICATE_EG_ENVIRONMENT_CAPTURE_H
extern unsigned char eg_environment_capture_requested;
unsigned char eg_planar_pixel(const unsigned char *planes,
                              unsigned x,unsigned y);
void eg_scale2x_indices(const unsigned char *src,unsigned w,unsigned h,
                        unsigned char *dst);
void eg_capture_environment_once(const unsigned char *planes,
                                 const unsigned char *palette);
#endif
