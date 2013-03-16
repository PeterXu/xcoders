#include "xacoders.h"
#include "ilbc/interface/ilbc.h"
#include <stdlib.h>
#include <stdio.h>

#define LOGD(...) {fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}
#define LOGI(...) {fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}
#define LOGW(...) {fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}
#define LOGE(...) {fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}

#ifndef returnv_if_fail
#define returnv_if_fail(p, v)  {if(!(p)) return (v); }
#endif

typedef struct xacoder_encoder_t {
	iLBC_encinst_t *encoder;
}xcoder_encoder_t;

typedef struct xacoder_decoder_t {
	iLBC_decinst_t  *decoder;
}xcoder_decoder_t;

typedef struct xacoder_coder_t {
	int ctype;
	union {
		xacoder_encoder_t *enc;
		xacoder_decoder_t *dec;
	};
}xcoder_coder_t;


int xacoder_create(xacoder_t *ppcoder, int ctype)
{
	xacoder_coder_t *pcoder = NULL;
	if (XA_ILBC_ENCDOER == ctype) {
		pcoder = new xacoder_coder_t;
		pcoder->enc = new xacoder_encoder_t;
		pcoder->ctype = ctype;
	}else if (XA_ILBC_DECODER == ctype) {
		pcoder = new xacoder_coder_t;
		pcoder->dec = new xacoder_decoder_t;
		pcoder->ctype = ctype;
	}else {
		LOGE("[%s] invalid coder type", __FUNCTION__);
		return -1;
	}
	
	*ppcoder = pcoder;
	return 0;
}

int xacoder_init(xacoder_t coder, int framelen)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	
	if (XA_ILBC_ENCDOER == pcoder->ctype) {
		returnv_if_fail(pcoder->enc, -1)
		if (WebRtcIlbcfix_EncoderCreate(&pcoder->enc->encoder) != 0) {
			LOGE("[%s] failed to WebRtcIlbcfix_EncoderCreate", __FUNCTION__);
			return -1;
		}
		if (WebRtcIlbcfix_EncoderInit(pcoder->enc->encoder, framelen) != 0) {
			LOGE("[%s] failed to WebRtcIlbcfix_EncoderInit", __FUNCTION__);
			return -1;
		}
	}else if (XA_ILBC_DECODER == pcoder->ctype) {
		returnv_if_fail(pcoder->dec, -1)
		if (WebRtcIlbcfix_DecoderCreate(&pcoder->dec->decoder) != 0) {
			LOGE("[%s] failed to WebRtcIlbcfix_DecoderCreate", __FUNCTION__);
			return -1;
		}
		if (WebRtcIlbcfix_DecoderInit(pcoder->dec->decoder, framelen) != 0) {
			LOGE("[%s] failed to WebRtcIlbcfix_DecoderInit", __FUNCTION__);
			return -1;
		}
	}else {
		LOGE("[%s] invalid coder type", __FUNCTION__);
		return -1;
	}
	
	return 0;
}

int xacoder_encode(xacoder_t coder, const uword16_t *speechin, uword16_t len, uword16_t *encoded)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(pcoder->enc, -1);
	returnv_if_fail(pcoder->enc->encoder, -1);
	
	return WebRtcIlbcfix_Encode(pcoder->enc->encoder, (const WebRtc_Word16 *)speechin, (WebRtc_Word16)len, (WebRtc_Word16 *)encoded);
}

int xacoder_decode(xacoder_t coder, const uword16_t *encoded, uword16_t len, uword16_t *decoded, uword16_t *speechtype)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(pcoder->dec, -1);
	returnv_if_fail(pcoder->dec->decoder, -1);
	
	return WebRtcIlbcfix_Decode(pcoder->dec->decoder, (const WebRtc_Word16 *)encoded, (WebRtc_Word16)len, (WebRtc_Word16 *)decoded, (WebRtc_Word16 *)speechtype);
}
		
int xacoder_decodeplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(pcoder->dec, -1);
	returnv_if_fail(pcoder->dec->decoder, -1);
	
	return WebRtcIlbcfix_DecodePlc(pcoder->dec->decoder, (WebRtc_Word16 *)decoded, (WebRtc_Word16)noOfLostFrames);
}

int xacoder_neteqplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)coder;
	returnv_if_fail(pcoder, -1);
	returnv_if_fail(pcoder->dec, -1);
	returnv_if_fail(pcoder->dec->decoder, -1);
	
	return WebRtcIlbcfix_NetEqPlc(pcoder->dec->decoder, (WebRtc_Word16 *)decoded, (WebRtc_Word16)noOfLostFrames);
}

int xacoder_destroy(xacoder_t *ppcoder)
{
	xacoder_coder_t * pcoder = (xacoder_coder_t *)(*ppcoder);
	returnv_if_fail(pcoder, -1);
		
	if (XA_ILBC_ENCDOER == pcoder->ctype) {
		returnv_if_fail(pcoder->enc, -1);
		if (pcoder->enc->encoder)
			WebRtcIlbcfix_EncoderFree(pcoder->enc->encoder);
		delete pcoder->enc;
	}else if (XA_ILBC_DECODER == pcoder->ctype) {
		returnv_if_fail(pcoder->dec, -1);
		if (pcoder->dec->decoder)
			WebRtcIlbcfix_DecoderFree(pcoder->dec->decoder);
		delete pcoder->dec;
	}
	
	delete pcoder;
	pcoder = NULL;
	return 0;
}