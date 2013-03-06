#include "utils.h"
#include "xcoders.h"
#include "libavformat/avformat.h"
#if HAVE_LIBYUV
#include "libyuv.h"
#endif

int get_pixel_format(int colorspace, int *bitsnum)
{
	int bits = 0;
	PixelFormat pmt = PIX_FMT_NONE;
	switch(colorspace) {
	case XCODER_FMT_I420:
		pmt = PIX_FMT_YUV420P;
		bits = 12;
		break;
	case XCODER_FMT_RGB24:
		pmt = PIX_FMT_RGB24;
		bits = 24;
		break;
	case XCODER_FMT_BGR24:
		pmt = PIX_FMT_BGR24;
		bits = 24;
		break;
	case XCODER_FMT_NV21:
		pmt = PIX_FMT_NV21;
		bits = 12;
		break;
	case XCODER_FMT_RGBA:
		pmt = PIX_FMT_RGBA;
		bits = 32;
		break;
	default:
		break;
	};

	if (bitsnum) {
		*bitsnum = bits;
	}

	return (int)pmt;
}

#if HAVE_LIBYUV
bool ScaleYUVFrame(const char *src, unsigned int src_w, unsigned int src_h, int format,
				   char *dst, unsigned int dst_w, unsigned int dst_h)
{
	if (!src || !dst) {
		return false;
	}

	if (src_w < dst_w || src_h < dst_h) 
	{
		return false;
	}

	if (src_w == dst_w && src_h == dst_h) 
	{
		return true;
	}

	if (format == XCODER_FMT_I420) 
	{
		// for src
		unsigned int src_ysize = src_w * src_h;
		unsigned int src_uvsize = (src_ysize>>1);

		unsigned int src_ylinesize = src_w;
		unsigned int src_uvlinesize = src_ylinesize;

		// for dst
		unsigned int dst_ysize = dst_w * dst_h;
		unsigned int dst_uvsize = (dst_ysize>>1);

		unsigned int dst_ylinesize = dst_w;
		unsigned int dst_uvlinesize = dst_ylinesize;

		// scale
		const uint8 * src_y = (const uint8 *)src;
		const uint8 * src_u = src_y + src_ysize;
		const uint8 * src_v = src_y + src_ysize + (src_uvsize>>1);

		uint8 * dst_y = (uint8 *)dst;
		uint8 * dst_u = dst_y + dst_ysize;
		uint8 * dst_v = dst_y + dst_ysize + (dst_uvsize>>1);

		if(libyuv::I420Scale(src_y, src_ylinesize, src_u, (src_uvlinesize>>1), src_v, (src_uvlinesize>>1), 
			src_w, src_h,
			dst_y, dst_ylinesize, dst_u, (dst_uvlinesize>>1), dst_v, (dst_uvlinesize>>1),
			dst_w, dst_h,
			libyuv::kFilterNone) == 0) 
		{
			return true;
		}
	}

	return false;
}
#endif
