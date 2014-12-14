// xcoders.cpp : 定义 DLL 应用程序的导出函数。
//

#include "xcoders.h"

#include "ffencoder.h"
#include "ffdecoder.h"
#include "log.h"
#include "utils.h"

#ifndef returnv_if_fail
#define returnv_if_fail(p, v)  {if(!(p)) return (v); }
#endif

#ifndef safe_release
#define safe_release(ptr) {if(ptr) {delete (ptr); (ptr) = NULL;}}
#endif

typedef struct xcoder_encoder_t {
	FFEncoder *encoder;
	FFVideoParam *param;
}xcoder_encoder_t;

typedef struct xcoder_decoder_t {
	FFDecoder  *decoder;
	FFVideoParam *param;
	int outcsp;
}xcoder_decoder_t;

typedef struct xcoder_coder_t {
	int ctype;
	xcoder_callback_t cb;
	uint8_t *cache;
	int cache_size;
	union {
		xcoder_encoder_t *enc;
		xcoder_decoder_t *dec;
	};
}xcoder_coder_t;


int xcoder_create(xcoder_t *ppcoder, int ctype)
{
	xcoder_coder_t *pcoder = NULL;
	pcoder = new xcoder_coder_t;
	memset(pcoder, 0, sizeof(xcoder_coder_t));
	pcoder->ctype = ctype;
		
	if (ctype == XCODER_ENCODER) {
		pcoder->enc = new xcoder_encoder_t;
		memset(pcoder->enc, 0, sizeof(xcoder_encoder_t));
	}else if (ctype == XCODER_DECODER) {
		pcoder->dec = new xcoder_decoder_t;
		memset(pcoder->dec, 0, sizeof(xcoder_decoder_t));
	}else {
		delete pcoder;
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
		pcoder->enc->param = new FFVideoParam(format.width, format.height, PIX_FMT_YUV420P, bitrate, fps, "libx264");
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		pcoder->dec->param = new FFVideoParam(format.width, format.height, PIX_FMT_YUV420P, bitrate, fps, "h264");
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
		returnv_if_fail(pcoder->enc->encoder == NULL, -1);

		FFVideoParam videoParam(
			pcoder->enc->param->width, 
			pcoder->enc->param->height, 
			pcoder->enc->param->pixelFmt,
			pcoder->enc->param->bitRate,
			pcoder->enc->param->frameRate,
			pcoder->enc->param->codecName);
		pcoder->enc->encoder = new FFEncoder(videoParam);
		if (pcoder->enc->encoder->open() != 0) {
			LOGE("[%s] cannot open coder!", __FUNCTION__);
			return -1;
		}
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->param, -1);
		returnv_if_fail(pcoder->dec->decoder == NULL, -1);

		FFVideoParam videoParam(
			pcoder->dec->param->width, 
			pcoder->dec->param->height, 
			pcoder->dec->param->pixelFmt,
			pcoder->dec->param->bitRate,
			pcoder->dec->param->frameRate,
			pcoder->dec->param->codecName);
		pcoder->dec->decoder = new FFDecoder(videoParam);
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

		// convert to x264's required format
		uint8_t *pi420data = NULL;
		int i420size = pcoder->enc->param->width * pcoder->enc->param->height * 12 / 8;
		if (inpmt != pcoder->enc->param->pixelFmt ||
			format.width != pcoder->enc->param->width || 
			format.height != pcoder->enc->param->height) {
			if (pcoder->cache == NULL || pcoder->cache_size < i420size) {	
				if (pcoder->cache)
					delete pcoder->cache;
				pcoder->cache_size = i420size;
				pcoder->cache = new uint8_t[i420size];
			}

			int ret = FFEncoder::convertPixFmt(data, size, format.width, format.height, inpmt,
				pcoder->cache, i420size, pcoder->enc->param->width, pcoder->enc->param->height, pcoder->enc->param->pixelFmt);
			if (ret != i420size) {
				LOGE("[%s] failed to convertPixFmt", __FUNCTION__);
				return -1;
			}
			pi420data = pcoder->cache;
		}else{
			pi420data = data;
			if (size != i420size) {
				return -1;
			}
		}
		
		int out_size = 0;
		out_size = pcoder->enc->encoder->encodeVideoFrame((const uint8_t *)pi420data, 
				pcoder->enc->param->pixelFmt,
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
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->param, -1);

		int used_size = pcoder->dec->decoder->decodeVideoFrame((const uint8_t *)data, size);
		//LOGI("used_size=%d, decoded size=%d", used_size, pcoder->dec->decoder->getVideoFrameSize());
		if (used_size > 0) {
			int bitsnum = 0;
			PixelFormat outpmt = (PixelFormat)get_pixel_format(pcoder->dec->outcsp, &bitsnum);

			//LOGI("outpmt=%d, bitnum=%d", outpmt, bitsnum);
			int outlen = 0;
			uint8_t *poutdata = NULL;
			if (outpmt != PIX_FMT_NONE && outpmt != PIX_FMT_YUV420P) {
				outlen = pcoder->dec->param->width * pcoder->dec->param->height * bitsnum / 8;
				if (pcoder->cache == NULL || pcoder->cache_size < outlen) {	
					if (pcoder->cache)
						delete pcoder->cache;
					pcoder->cache_size = outlen;
					pcoder->cache = new uint8_t[outlen];
				}

				int ret = FFEncoder::convertPixFmt(
					pcoder->dec->decoder->getVideoFrame(), 
					pcoder->dec->decoder->getVideoFrameSize(), 
					pcoder->dec->param->width, 
					pcoder->dec->param->height, 
					PIX_FMT_YUV420P,
					pcoder->cache, 
					outlen, 
					pcoder->enc->param->width, 
					pcoder->enc->param->height, 
					outpmt);
				//LOGI("outlen=%d, ret=%d", outlen, ret);
				if (ret != outlen) {
					LOGE("[%s] failed to convertPixFmt", __FUNCTION__);
					return -1;
				}
				poutdata = pcoder->cache;
			}else {
				poutdata = (uint8_t *)pcoder->dec->decoder->getVideoFrame();
				outlen = pcoder->dec->decoder->getVideoFrameSize();
			}
			//LOGI("outdata=0x%d, outlen=%d", (int)poutdata, outlen);
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
		safe_release(pcoder->enc->encoder);
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		returnv_if_fail(pcoder->dec->decoder, -1);
		pcoder->dec->decoder->close();
		safe_release(pcoder->dec->decoder);
	}
	safe_release(pcoder->cache);
	pcoder->cache_size = 0;

	return 0;
}

int xcoder_destroy(xcoder_t *ppcoder)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)(*ppcoder);
	returnv_if_fail(pcoder, -1);

	xcoder_close(pcoder);
	
	if (pcoder->ctype == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		safe_release(pcoder->enc->param);
		safe_release(pcoder->enc);
	}else if (pcoder->ctype == XCODER_DECODER) {
		returnv_if_fail(pcoder->dec, -1);
		safe_release(pcoder->dec->param);
		safe_release(pcoder->dec);
	}

	safe_release(pcoder);

	return 0;
}


