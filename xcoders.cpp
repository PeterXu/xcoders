// xcoders.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "xcoders.h"

#include "FFmpegEncoder.h"
#include "FFmpegDecoder.h"
#include "LogTrace.h"
#include "utils.h"

#ifndef returnv_if_fail
#define returnv_if_fail(p, v)  {if(!(p)) return (v); }
#endif

typedef struct xcoder_encoder_t {
	FFmpegEncoder *encoder;
	FFmpegVideoParam *param;
}xcoder_encoder_t;

typedef struct xcoder_decoder_t {
	FFmpegDecoder  *decoder;
	FFmpegVideoParam *param;
	int outcsp;
	uint8_t *cache;
	int cache_size;
}xcoder_decoder_t;

typedef struct xcoder_coder_t {
	int ctype;
	xcoder_callback_t cb;
	union {
		xcoder_encoder_t *enc;
		xcoder_decoder_t *dec;
	};
}xcoder_coder_t;


int xcoder_create(xcoder_t *ppcoder, int ctype)
{
	xcoder_coder_t *pcoder = NULL;
	if (ctype == XCODER_ENCODER) {
		pcoder = new xcoder_coder_t;
		pcoder->enc = new xcoder_encoder_t;
		pcoder->ctype = ctype;
		memset(pcoder->enc, 0, sizeof(xcoder_encoder_t));
	}else if (ctype == XCODER_DECODER) {
		pcoder = new xcoder_coder_t;
		pcoder->dec = new xcoder_decoder_t;
		pcoder->ctype = ctype;
		memset(pcoder->dec, 0, sizeof(xcoder_decoder_t));
	}else {
		LOGE("[%s] invalid ctype:%d", __FUNCTION__, ctype);
		return -1;
	}
	
	*ppcoder = (xcoder_t)pcoder;
	return 0;
}

int xcoder_set_options(xcoder_t coder, xcoder_format_t format, int fps, int bitrate, int fec)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	if (pcoder->ctype == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		pcoder->enc->param = new FFmpegVideoParam(format.width, format.height, PIX_FMT_YUV420P, bitrate, fps, "libx264");
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		pcoder->dec->param = new FFmpegVideoParam(format.width, format.height, PIX_FMT_YUV420P, bitrate, fps, "h264");
		// if set to XCODER_FMT_NONE, then decoded to default decoder's seetings, else use it
		pcoder->dec->outcsp = format.colorspace;
	}else {
		LOGE("[%s] invalid ctype!", __FUNCTION__);
		return -1;
	}
	
	return 0;
}

int xcoder_open(xcoder_t coder, xcoder_callback_t cb, void *priv)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(cb, -1);
	pcoder->cb = cb;

	if (pcoder->ctype == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		returnv_if_fail(pcoder->enc->param, -1);

		FFmpegVideoParam videoParam(
			pcoder->enc->param->width, 
			pcoder->enc->param->height, 
			pcoder->enc->param->pixelFormat,
			pcoder->enc->param->bitRate,
			pcoder->enc->param->frameRate,
			pcoder->enc->param->videoCodecName);
		pcoder->enc->encoder = new FFmpegEncoder(videoParam);
		if (pcoder->enc->encoder->open() != 0) {
			LOGE("[%s] cannot open coder!", __FUNCTION__);
			return -1;
		}
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->param, -1);

		FFmpegVideoParam videoParam(
			pcoder->dec->param->width, 
			pcoder->dec->param->height, 
			pcoder->dec->param->pixelFormat,
			pcoder->dec->param->bitRate,
			pcoder->dec->param->frameRate,
			pcoder->dec->param->videoCodecName);
		pcoder->dec->decoder = new FFmpegDecoder(videoParam);
		if (pcoder->dec->decoder->open() != 0) {
			LOGE("[%s] cannot open coder!", __FUNCTION__);
			return -1;
		}
	}else{
		LOGE("[%s] invalid ctype!", __FUNCTION__);
		return -1;
	}

	return 0;
}

int xcoder_code_frame(xcoder_t coder, unsigned char *data, int size, xcoder_format_t format)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	if (pcoder->ctype == XCODER_ENCODER) {
		int bitsnum = 0;
		PixelFormat inpmt = (PixelFormat)get_pixel_format(format.colorspace, &bitsnum);
		returnv_if_fail(inpmt != PIX_FMT_NONE, -1);

		returnv_if_fail(pcoder->enc, -1);
		returnv_if_fail(pcoder->enc->param, -1);

		bool ballocbuffer = false;

		// convert to x264's required format
		uint8_t *pi420data = NULL;
		int i420size = pcoder->enc->param->width * pcoder->enc->param->height * 12 / 8;
		if (inpmt != pcoder->enc->param->pixelFormat ||
			format.width != pcoder->enc->param->width || 
			format.height != pcoder->enc->param->height) {
			pi420data = new uint8_t[i420size];
			int ret = FFmpegEncoder::convertPixFmt(data, size, format.width, format.height, inpmt,
				pi420data, i420size, pcoder->enc->param->width, pcoder->enc->param->height, pcoder->enc->param->pixelFormat);
			if (ret != i420size) {
				delete []pi420data;
				LOGE("[%s] failed to convertPixFmt", __FUNCTION__);
				return -1;
			}
			ballocbuffer = true;
		}else{
			pi420data = data;
			if (size != i420size) {
				return -1;
			}
		}

		int out_size = pcoder->enc->encoder->encodeVideoFrame((const uint8_t *)pi420data, 
				pcoder->enc->param->pixelFormat,
				pcoder->enc->param->width, 
				pcoder->enc->param->height);		
		if (out_size > 0) {
			pcoder->cb(XCODER_CB_ENCODED_FRAME, 
				pcoder->enc->encoder->getVideoEncodedBuffer(),
				out_size, 
				NULL);
		}else {
			LOGE("[%s] encoded ret: %d", __FUNCTION__, out_size);
		}
		if (ballocbuffer) {
			delete []pi420data;
		}
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->param, -1);

		int used_size = pcoder->dec->decoder->decodeVideoFrame((const uint8_t *)data, size);
		if (used_size > 0) {
			int bitsnum = 0;
			PixelFormat outpmt = (PixelFormat)get_pixel_format(pcoder->dec->outcsp, &bitsnum);

			int outlen = 0;
			uint8_t *poutdata = NULL;
			if (outpmt != PIX_FMT_NONE && outpmt != PIX_FMT_YUV420P) {
				int outlen = pcoder->dec->param->width * pcoder->dec->param->height * bitsnum / 8;
				if (pcoder->dec->cache == NULL || pcoder->dec->cache_size < outlen) {	
					if (pcoder->dec->cache)
						delete pcoder->dec->cache;
					pcoder->dec->cache_size = outlen;
					pcoder->dec->cache = new uint8_t[outlen];
				}

				int ret = FFmpegEncoder::convertPixFmt(
					pcoder->dec->decoder->getVideoFrame(), 
					pcoder->dec->decoder->getVideoFrameSize(), 
					pcoder->dec->param->width, 
					pcoder->dec->param->height, 
					PIX_FMT_YUV420P,
					pcoder->dec->cache, 
					outlen, 
					pcoder->enc->param->width, 
					pcoder->enc->param->height, 
					outpmt);
				if (ret != outlen) {
					LOGE("[%s] failed to convertPixFmt", __FUNCTION__);
					return -1;
				}
				poutdata = pcoder->dec->cache;
			}else {
				poutdata = (uint8_t *)pcoder->dec->decoder->getVideoFrame();
				outlen = pcoder->dec->decoder->getVideoFrameSize();
			}
			pcoder->cb(XCODER_CB_DECODED_FRAME, poutdata, outlen, NULL);
		}else {
			LOGE("[%s] decoded ret: %d", __FUNCTION__, used_size);
		}
	}else{
		LOGE("[%s] invalid ctype!", __FUNCTION__);
		return -1;
	}

	return 0;
}

int xcoder_close(xcoder_t coder)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	if (pcoder->ctype == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		returnv_if_fail(pcoder->enc->encoder, -1);
		pcoder->enc->encoder->close();
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->decoder, -1);
		pcoder->dec->decoder->close();
	}

	return 0;
}

int xcoder_destroy(xcoder_t *ppcoder)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)(*ppcoder);
	returnv_if_fail(pcoder, -1);

	if (pcoder->ctype == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		if (pcoder->enc->param)
			delete pcoder->enc->param;
		if (pcoder->enc->encoder)
			delete pcoder->enc->encoder;
		delete pcoder->enc;
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		if (pcoder->dec->param)
			delete pcoder->dec->param;
		if (pcoder->dec->decoder)
			delete pcoder->dec->decoder;
		if (pcoder->dec->cache)
			delete pcoder->dec->cache;
		delete pcoder->dec;
	}

	delete pcoder;
	pcoder = NULL;

	return 0;
}


