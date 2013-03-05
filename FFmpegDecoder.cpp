#include "stdafx.h"
#include "FFmpegDecoder.h"
#include "LogTrace.h"

FFmpegDecoder::FFmpegDecoder(const FFmpegVideoParam &vp, const FFmpegAudioParam &ap) : 
videoParam(vp), audioParam(ap)
{
	init();
}

FFmpegDecoder::FFmpegDecoder(const FFmpegVideoParam &vp) : videoParam(vp)
{
	init();
}

FFmpegDecoder::FFmpegDecoder(const FFmpegAudioParam &ap) : audioParam(ap)
{
	init();
}

void FFmpegDecoder::init()
{
	// initialize the private fields
	this->inputContext     = NULL;
	this->videoStream      = NULL;
	this->audioStream      = NULL;

	this->videoFrameBuffer = NULL;
	this->videoFrameSize   = 0;
	this->videoBufferSize  = 0;

	this->audioFrameBuffer = NULL;
	this->audioFrameSize   = 0;
	this->audioBufferSize  = 0;
	
	this->currentPacketPts = 0;
	this->currentPacketDts = 0;
	
	this->opened           = false;
	this->decodeVideo      = !this->videoParam.empty();
	this->decodeAudio      = !this->audioParam.empty();

	// register all codecs and demux
	av_register_all();
}

FFmpegDecoder::~FFmpegDecoder()
{
	this->close();
}


//////////////////////////////////////////////////////////////////////////
//
//  Public properties
//
//////////////////////////////////////////////////////////////////////////

const FFmpegVideoParam &FFmpegDecoder::getVideoParam() const
{
	return this->videoParam;
}

const FFmpegAudioParam &FFmpegDecoder::getAudioParam() const
{
	return this->audioParam;
}

const uint8_t *FFmpegDecoder::getVideoFrame() const
{
	return this->videoFrameBuffer;
}

int FFmpegDecoder::getVideoFrameSize() const
{
	return this->videoFrameSize;
}

const uint8_t *FFmpegDecoder::getAudioFrame() const
{
	return this->audioFrameBuffer;
}

int FFmpegDecoder::getAudioFrameSize() const
{
	return this->audioFrameSize;
}

double FFmpegDecoder::getPresentTimeStamp() const
{
	return this->currentPacketPts;
}

double FFmpegDecoder::getDecodeTimeStamp() const
{
	return this->currentPacketDts;
}


//////////////////////////////////////////////////////////////////////////
//
//  Public Methods
//
//////////////////////////////////////////////////////////////////////////

int FFmpegDecoder::open()
{
	LOGI("FFmpegDecoder.open, begin!");
	if (this->opened)
	{
		LOGW("FFmpegDecoder.open, try to reopen!");
		return -1;
	}

	if (this->videoParam.videoCodecName.empty() && 
		this->audioParam.audioCodecName.empty())
	{
		LOGE("FFmpegDecoder.open, no a/v codec name");
		return -1;
	}

	// allocate the output media context
	this->inputContext = avformat_alloc_context();
	if (!this->inputContext)
	{
		LOGE("FFmpegDecoder.open, failed to alloc context");
		return -1;
	}

	// video related initialization if necessary
	if (this->decodeVideo)
	{
		// find the video encoder
		AVCodec *videoCodec = NULL;

		// use the codec name preferentially if it is specified in the input param
		videoCodec = avcodec_find_decoder_by_name(this->videoParam.videoCodecName.c_str());
		if (!videoCodec)
		{
			LOGE("FFmpegDecoder.open, find no video codec!");
			return -1;
		}

		// add the video stream with stream id 0
		this->videoStream = av_new_stream(this->inputContext, 0);
		if (!this->videoStream)
		{
			LOGE("FFmpegDecoder.open, failed to new video stream!");
			return -1;
		}

		// get the video parameters
		AVCodecContext *videoCodecContext = this->videoStream->codec;
		videoCodecContext->codec_id     = videoCodec->id;
		videoCodecContext->codec_type   = CODEC_TYPE_VIDEO;
		videoCodecContext->width        = this->videoParam.width;
		videoCodecContext->height       = this->videoParam.height;
		videoCodecContext->pix_fmt      = this->videoParam.pixelFormat;
		videoCodecContext->bit_rate     = this->videoParam.bitRate;
		this->videoStream->r_frame_rate.den = this->videoParam.frameRate;
		this->videoStream->r_frame_rate.num = 1;


		// open the video codec
		if (avcodec_open(videoCodecContext, videoCodec))
		{
			LOGE("FFmpegDecoder.open, find but failed to open video codec!");
			return -1;
		}

		// allocate the video frame to be decoded
		this->videoBufferSize  = avpicture_get_size(this->videoParam.pixelFormat, this->videoParam.width, this->videoParam.height);
		this->videoFrameSize   = 0;
		this->videoFrameBuffer = (uint8_t *)av_malloc(this->videoBufferSize);
	}

	// audio related initialization if necessary
	if (this->decodeAudio)
	{
		// find the video encoder
		AVCodec *audioCodec = NULL;

		// use the codec name preferentially if it is specified in the input param
		audioCodec = avcodec_find_decoder_by_name(this->audioParam.audioCodecName.c_str());

		// add the audio stream with stream id 1
		this->audioStream = av_new_stream(this->inputContext, 1);
		if (!this->videoStream)
		{
			return -1;
		}

		if (!audioCodec)
		{
			return -1;
		}

		// get the audio parameters
		AVCodecContext *audioCodecContext = this->audioStream->codec;
		audioCodecContext->codec_id     = audioCodec->id;
		audioCodecContext->codec_type   = CODEC_TYPE_AUDIO;
		audioCodecContext->sample_rate  = this->audioParam.sampleRate;
		audioCodecContext->channels     = this->audioParam.channels;
		audioCodecContext->bit_rate     = this->audioParam.bitRate;

		// open the audio codec
		if (avcodec_open(audioCodecContext, audioCodec))
		{
			return -1;
		}

		// allocate output buffer
		this->audioBufferSize  = AVCODEC_MAX_AUDIO_FRAME_SIZE;
		this->audioFrameSize   = 0;
		this->audioFrameBuffer = (uint8_t *)av_malloc(this->audioBufferSize);
	}

	this->opened = true;
	LOGI("FFmpegDecoder.open, end!");
	return 0;
}

void FFmpegDecoder::close()
{
	if (!this->opened)
	{
		return;
	}

	this->currentPacketPts = 0;
	this->currentPacketDts = 0;

	if (this->decodeVideo)
	{
		avcodec_close(this->videoStream->codec);
		av_freep(&this->videoFrameBuffer);
		av_freep(&this->videoStream);
		this->videoFrameSize = 0;
		this->videoBufferSize = 0;
	}

	if (this->decodeAudio)
	{
		avcodec_close(this->audioStream->codec);
		av_freep(&this->audioFrameBuffer);
		av_freep(&this->audioStream);
		this->audioFrameSize  = 0;
		this->audioBufferSize = 0;
	}

	av_freep(&this->inputContext);

	this->opened      = false;
	this->decodeAudio = false;
	this->decodeVideo = false;
}

int FFmpegDecoder::decodeVideoFrame(const uint8_t *frameData, int dataSize, int64_t pts, int64_t dts)
{
	if (!this->opened)
	{
		LOGE("FFmpegDecoder::decodeVideoFrame, not open");
		return -1;
	}

	if (!this->decodeVideo)
	{
		LOGE("FFmpegDecoder::decodeVideoFrame, cannot decode video");
		return -1;
	}

	AVPacket pkt;
	pkt.data = (uint8_t *)frameData;
	pkt.size = dataSize;
	pkt.pts = pts;
	pkt.dts = dts;

	// decode video frame
	this->currentPacketPts = (double)pkt.pts * this->videoStream->time_base.num / this->videoStream->time_base.den;
	this->currentPacketDts = (double)pkt.dts * this->videoStream->time_base.num / this->videoStream->time_base.den;
	return this->decodeVideoFrame(pkt);
}

int FFmpegDecoder::decodeAudioFrame(const uint8_t *frameData, int dataSize, int64_t pts, int64_t dts)
{
	if (!this->opened)
	{
		LOGE("FFmpegDecoder::decodeAudioFrame, not open");
		return -1;
	}

	if (!this->decodeAudio)
	{
		LOGE("FFmpegDecoder::decodeAudioFrame, cannot decode audio");
		return -1;
	}

	AVPacket pkt;
	pkt.data = (uint8_t *)frameData;
	pkt.size = dataSize;
	pkt.pts = pts;
	pkt.dts = dts;

	this->currentPacketPts = (double)pkt.pts * this->audioStream->time_base.num / this->audioStream->time_base.den;
	this->currentPacketDts = (double)pkt.dts * this->audioStream->time_base.num / this->audioStream->time_base.den;
	return this->decodeAudioFrame(pkt);
}


//////////////////////////////////////////////////////////////////////////
//
//  Private Methods
//
//////////////////////////////////////////////////////////////////////////

int FFmpegDecoder::decodeVideoFrame(AVPacket &avpkt)
{
	int decodedSize, gotPicture = 0;
	AVFrame videoFrame;

	// set default value
	avcodec_get_frame_defaults(&videoFrame);

	// decode the video frame
	decodedSize = avcodec_decode_video2(this->videoStream->codec, &videoFrame, &gotPicture, &avpkt);

	this->videoFrameSize = 0;
	if (gotPicture != 0)
	{
		// read the data to the buffer
		avpicture_layout((AVPicture*)&videoFrame, this->videoParam.pixelFormat, this->videoParam.width, this->videoParam.height, this->videoFrameBuffer, this->videoBufferSize);
		this->videoFrameSize = this->videoBufferSize;
	}

	if (decodedSize < 0)
	{
		LOGI("FFmpegDecoder.decodeVideoFrame, error!");
		return -1;
	}

	return decodedSize;
}

int FFmpegDecoder::decodeAudioFrame(AVPacket &avpkt)
{
	int decodedSize, outputFrameSize = this->audioBufferSize;

	// decode one audio frame
	decodedSize = avcodec_decode_audio3(this->audioStream->codec, (int16_t *)this->audioFrameBuffer, &outputFrameSize, &avpkt);

	this->audioFrameSize = outputFrameSize;

	if (avpkt.size - decodedSize <= 0)
	{
		// all the audio frames in the packet have been decoded
	}

	if (decodedSize < 0)
	{
		return -1;
	}

	return decodedSize;
}

