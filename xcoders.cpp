// xcoders.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "xcoders.h"

#include "FFmpegEncoder.h"
#include "FFmpegDecoder.h"

#ifndef returnv_if_fail
#define returnv_if_fail(p, v)  {if(!(p)) return (v); }
#endif

enum {
	XCODER_ENCODER,
	XCODER_DECODER,
};

typedef struct xcoder_encoder_t {
	FFmpegEncoder *encoder;
    FFmpegVideoParam *inparam;
	FFmpegVideoParam *outparam;
}xcoder_encoder_t;

typedef struct xcoder_decoder_t {
	FFmpegDecoder  *decoder;
	FFmpegVideoParam *param;
}xcoder_decoder_t;

typedef struct xcoder_coder_t {
	int code_type;
	xcoder_callback_t cb;
	union {
		xcoder_encoder_t *enc;
		xcoder_decoder_t *dec;
	};
}xcoder_coder_t;


int xcoder_create(xcoder_t *ppcoder)
{
	*ppcoder = (xcoder_t)(new xcoder_coder_t);
	return (*ppcoder == NULL) ? -1 : 0;
}

int xcoder_set_input(xcoder_t coder, int width, int height, int format, int fec)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	switch(format) {
	case XCODER_FMT_I420: // for encoder
		pcoder->code_type = XCODER_ENCODER;
		pcoder->enc = new xcoder_encoder_t;
		pcoder->enc->inparam = new FFmpegVideoParam(width, height, PIX_FMT_YUV420P, 0, 30, "lib264");
		break;
	case XCODER_FMT_AVC:  // for decoder
		pcoder->code_type = XCODER_DECODER;
		pcoder->dec = new xcoder_decoder_t;
		break;
	default:
		return -1;
	};


	return 0;
}

int xcoder_set_output(xcoder_t coder, int width, int height, int format, int framerate, int bitrate)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	switch(format) {
	case XCODER_FMT_I420: // for decoder
		break;
	case XCODER_FMT_AVC:  // for encoder
		returnv_if_fail(pcoder->enc, -1);
		pcoder->enc->outparam = new FFmpegVideoParam(width, height, PIX_FMT_NONE, bitrate, framerate, "lib264");
		break;
	default:
		return -1;
	};
	return 0;
}

int xcoder_open(xcoder_t coder, xcoder_callback_t cb, void *priv)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(cb, -1);
	pcoder->cb = cb;

	if (pcoder->code_type == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		returnv_if_fail(pcoder->enc->inparam, -1);
		returnv_if_fail(pcoder->enc->outparam, -1);

		FFmpegVideoParam videoParam(
			pcoder->enc->outparam->width, 
			pcoder->enc->outparam->height, 
			pcoder->enc->inparam->pixelFormat,
			pcoder->enc->outparam->bitRate,
			pcoder->enc->outparam->frameRate,
			pcoder->enc->outparam->videoCodecName);
		pcoder->enc->encoder = new FFmpegEncoder(videoParam);
		if (pcoder->enc->encoder->open() != 0) {
			//LOGE("cannot open coder!");
			return -1;
		}
	}else if (pcoder->code_type == XCODER_DECODER) {
	}

	return 0;
}

int xcoder_code_frame(xcoder_t coder, unsigned char *data, int size)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	if (pcoder->code_type == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		returnv_if_fail(pcoder->enc->inparam, -1);
		returnv_if_fail(pcoder->enc->outparam, -1);

		int size = pcoder->enc->encoder->encodeVideoFrame((const uint8_t *)data, 
				pcoder->enc->inparam->pixelFormat,
				pcoder->enc->inparam->width, 
				pcoder->enc->inparam->height);
		if (size > 0) {
			pcoder->cb(XCODER_CB_ENCODED_FRAME, 
				pcoder->enc->encoder->getVideoEncodedBuffer(),
				size, 
				NULL);
		}
	}
	return 0;
}

int xcoder_close(xcoder_t coder)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);

	if (pcoder->code_type == XCODER_ENCODER) {
		returnv_if_fail(pcoder->enc, -1);
		pcoder->enc->encoder->close();
	}

	return 0;
}

int xcoder_destroy(xcoder_t *ppcoder)
{
	xcoder_coder_t * pcoder = (xcoder_coder_t *)(*ppcoder);
	returnv_if_fail(pcoder, -1);

	if (pcoder->code_type == XCODER_ENCODER) {
		delete pcoder->enc->inparam;
		delete pcoder->enc->outparam;
		delete pcoder->enc->encoder;
		delete pcoder->enc;

	}

	return 0;
}

