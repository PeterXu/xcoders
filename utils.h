#ifndef _UTILS_XX_H_
#define _UTILS_XX_H_

#include <stdlib.h>

int get_pixel_format(int colorspace, int *bitsnum=NULL);

#if HAVE_LIBYUV
bool ScaleYUVFrame(const char *src, unsigned int src_w, unsigned int src_h, int format,
						 char *dst, unsigned int dst_w, unsigned int dst_h);
#endif

#endif