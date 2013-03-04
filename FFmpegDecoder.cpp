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
    this->audioPacketData  = NULL;
    this->audioPacketSize  = 0;
    this->currentPacketPts = 0;
    this->currentPacketDts = 0;
    this->opened           = false;
    this->hasInput         = false;
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

int FFmpegDecoder::open(const char *fileName)
{
	LOGI("FFmpegDecoder.open, begin!");
    if (this->opened)
    {
		 LOGW("FFmpegDecoder.open, try to reopen!");
        return -1;
    }

    this->hasInput = (fileName != NULL) && (fileName[0] != 0);
    if (!this->hasInput && this->videoParam.videoCodecName.empty() && 
            this->audioParam.audioCodecName.empty())
    {
		 LOGE("FFmpegDecoder.open, no input or codec name");
        return -1;
    }

    if (this->hasInput) 
    {
        this->decodeVideo = false;
        this->decodeAudio = false;

        // open a media file as input.
        // The codecs are not opened. Only the file header (if present) is read
        if (av_open_input_file(&this->inputContext, fileName, NULL, 0, NULL))
        {
			 LOGE("FFmpegDecoder.open, fail to av_open_input_file");
            return -1;
        }

        // Read packets of a media file to get stream information.
        if (av_find_stream_info(this->inputContext) < 0)
        {
			 LOGE("FFmpegDecoder.open, fail to av_find_stream_info");
            return -1;
        }

        // find the video/audio stream
        for (size_t i = 0; i < this->inputContext->nb_streams; i++)
        {
            // TODO
            // there might be several audio or video streams,
            // however, only one audio/video stream is used here
            if (!this->videoStream && 
                    this->inputContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
            {
                this->videoStream = this->inputContext->streams[i];
                this->decodeVideo = true;
                continue;
            }

            if (!this->audioStream && 
                    this->inputContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
            {
                this->audioStream = this->inputContext->streams[i];
                this->decodeAudio = true;
                continue;
            }
        }
    }
    else
    {
        // allocate the output media context
        this->inputContext = avformat_alloc_context();
        if (!this->inputContext)
        {
			 LOGE("FFmpegDecoder.open, failed to alloc context");
            return -1;
        }
    }

    // video related initialization if necessary
    if (this->decodeVideo)
    {
        // find the video encoder
        AVCodec *videoCodec = NULL;
        if (this->hasInput)
        {
            // initialize the video codec
            AVCodecContext *videoCodecContext = this->videoStream->codec;
            videoCodec = avcodec_find_decoder(videoCodecContext->codec_id);
        }
        else
        {
            // use the codec name preferentially if it is specified in the input param
            videoCodec = avcodec_find_decoder_by_name(this->videoParam.videoCodecName.c_str());

            // add the video stream with stream id 0
            this->videoStream = av_new_stream(this->inputContext, 0);
            if (!this->videoStream)
            {
				  LOGE("FFmpegDecoder.open, failed to new video stream!");
                return -1;
            }
        }

        if (!videoCodec)
        {
			 LOGE("FFmpegDecoder.open, find no video codec!");
            return -1;
        }

        // get the video parameters
        AVCodecContext *videoCodecContext = this->videoStream->codec;
        if (this->hasInput)
        {
            this->videoParam.width       = videoCodecContext->width;
            this->videoParam.height      = videoCodecContext->height;
            this->videoParam.pixelFormat = videoCodecContext->pix_fmt;
            this->videoParam.bitRate     = videoCodecContext->bit_rate;
            this->videoParam.frameRate   = this->videoStream->r_frame_rate.num / this->videoStream->r_frame_rate.den;
        }
        else 
        {
            videoCodecContext->codec_id     = videoCodec->id;
            videoCodecContext->codec_type   = CODEC_TYPE_VIDEO;
            videoCodecContext->width        = this->videoParam.width;
            videoCodecContext->height       = this->videoParam.height;
            videoCodecContext->pix_fmt      = this->videoParam.pixelFormat;
            videoCodecContext->bit_rate     = this->videoParam.bitRate;
            this->videoStream->r_frame_rate.den = this->videoParam.frameRate;
            this->videoStream->r_frame_rate.num = 1;
        }

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
        if (this->hasInput)
        {
            // initialize the audio codec
            AVCodecContext *audioCodecContext = this->audioStream->codec;
            audioCodec = avcodec_find_decoder(audioCodecContext->codec_id);
        }
        else
        {
            // use the codec name preferentially if it is specified in the input param
            audioCodec = avcodec_find_decoder_by_name(this->audioParam.audioCodecName.c_str());

            // add the audio stream with stream id 1
            this->audioStream = av_new_stream(this->inputContext, 1);
            if (!this->videoStream)
            {
                return -1;
            }
        }

        if (!audioCodec)
        {
            return -1;
        }

        // get the audio parameters
        AVCodecContext *audioCodecContext = this->audioStream->codec;
        if (this->hasInput)
        {
            this->audioParam.sampleRate = audioCodecContext->sample_rate;
            this->audioParam.channels   = audioCodecContext->channels;
            this->audioParam.bitRate    = audioCodecContext->bit_rate;
        }
        else
        {
            audioCodecContext->codec_id     = audioCodec->id;
            audioCodecContext->codec_type   = CODEC_TYPE_AUDIO;
            audioCodecContext->sample_rate  = this->audioParam.sampleRate;
            audioCodecContext->channels     = this->audioParam.channels;
            audioCodecContext->bit_rate     = this->audioParam.bitRate;
        }

        // open the audio codec
        if (avcodec_open(audioCodecContext, audioCodec))
        {
            return -1;
        }

        // allocate output buffer
        this->audioBufferSize  = AVCODEC_MAX_AUDIO_FRAME_SIZE;
        this->audioFrameSize   = 0;
        this->audioFrameBuffer = (uint8_t *)av_malloc(this->audioBufferSize);
        this->audioPacketData  = NULL;
        this->audioPacketSize  = 0; // no data in the packet now, for initialization
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

	if (this->hasInput) {
		// close the input file
		av_close_input_file(this->inputContext);
	}

    this->currentPacketPts = 0;
    this->currentPacketDts = 0;

    if (this->decodeVideo)
    {
        avcodec_close(this->videoStream->codec);
        av_freep(&this->videoFrameBuffer);
        this->videoFrameSize = 0;
        this->videoBufferSize = 0;
    }

    if (this->decodeAudio)
    {
        avcodec_close(this->audioStream->codec);
        av_freep(&this->audioFrameBuffer);
        this->audioFrameSize  = 0;
        this->audioBufferSize = 0;
        this->audioPacketData = NULL;
        this->audioPacketSize = 0;
    }

    this->inputContext = NULL;
    this->audioStream  = NULL;
    this->videoStream  = NULL;

    this->opened      = false;
    this->decodeAudio = false;
    this->decodeVideo = false;
}

int FFmpegDecoder::decodeVideoFrame(const uint8_t *frameData, int dataSize)
{
	if (!this->opened)
	{
		LOGE("FFmpegDecoder::decodeVideoFrame, not open");
		return -1;
	}

	if (!this->decodeVideo)
	{
		LOGE("FFmpegDecoder::decodeVideoFrame, cannot decode video");
		return -2;
	}

	if (this->hasInput)
	{
		LOGE("FFmpegDecoder::decodeVideoFrame, input from file");
		return -3;
	}

	// decode video frame
	this->currentPacketPts = (double)this->currentPacket.pts * this->videoStream->time_base.num / this->videoStream->time_base.den;
	this->currentPacketDts = (double)this->currentPacket.dts * this->videoStream->time_base.num / this->videoStream->time_base.den;
	AVPacket pkt;
	pkt.data = (uint8_t *)frameData;
	pkt.size = dataSize;
	return this->decodeVideoFrame(pkt);
}

int FFmpegDecoder::decodeFrame()
{
    if (!this->opened)
    {
        return -1;
    }

    // one audio packet may contains several audio frame, so we need to
    // make sure all the audio frames in current packet were decoded.
    if (this->audioPacketSize <= 0)
    {
        // all data in the audio packet have been decoded,
        // read new frame now
        if (this->readFrame() < 0)
        {
            return -1;
        }
    }

    if (this->videoStream && this->currentPacket.stream_index == this->videoStream->index)
    {
        // decode video frame
        this->currentPacketPts = (double)this->currentPacket.pts * this->videoStream->time_base.num / this->videoStream->time_base.den;
        this->currentPacketDts = (double)this->currentPacket.dts * this->videoStream->time_base.num / this->videoStream->time_base.den;
        this->decodeVideoFrame();
        return 0;
    }
    else if (this->audioStream && this->currentPacket.stream_index == this->audioStream->index)
    {
        // decode audio frame
        this->currentPacketPts = (double)this->currentPacket.pts * this->audioStream->time_base.num / this->audioStream->time_base.den;
        this->currentPacketDts = (double)this->currentPacket.dts * this->audioStream->time_base.num / this->audioStream->time_base.den;
        this->decodeAudioFrame();
        return 1;
    }
    else
    {
        // error
        return -1;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Private Methods
//
//////////////////////////////////////////////////////////////////////////

int FFmpegDecoder::readFrame()
{
    av_init_packet(&this->currentPacket);

    // read frame from input
    if (av_read_frame(this->inputContext, &this->currentPacket) < 0)
    {
        return -1;
    }

    // For video, the returned packet contain exactly one frame.
    // For audio, it contains an integer number of frames if each frame has
    // a known fixed size (e.g. PCM or ADPCM) data.
    // It contains only one frame if the audio frames have a variable size (e.g. MPEG audio).

    if (this->currentPacket.stream_index == this->audioStream->index)
    {
        // set the audio packet data to be decoded
        // for dealing with one packet contains several audio frames
        this->audioPacketData = this->currentPacket.data;
        this->audioPacketSize = this->currentPacket.size;
    }

    return 0;
}

int FFmpegDecoder::decodeVideoFrame()
{
    int decodedSize, gotPicture = 0;
    AVFrame videoFrame;

    // set default value
    avcodec_get_frame_defaults(&videoFrame);

    // decode the video frame
    decodedSize = avcodec_decode_video2(this->videoStream->codec, &videoFrame, &gotPicture, &(this->currentPacket));

    this->videoFrameSize = 0;
    if (gotPicture != 0)
    {
        // read the data to the buffer
        avpicture_layout((AVPicture*)&videoFrame, this->videoParam.pixelFormat, this->videoParam.width, this->videoParam.height, this->videoFrameBuffer, this->videoBufferSize);
        this->videoFrameSize = this->videoBufferSize;
    }

    // video frame is decoded
    // free the packet now
    av_free_packet(&this->currentPacket);

    if (decodedSize < 0)
    {
        return -1;
    }

    return 0;
}

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

	return 0;
}

int FFmpegDecoder::decodeAudioFrame()
{
    int decodedSize, outputFrameSize = this->audioBufferSize;

    // decode one audio frame
    AVPacket pkt;
    pkt.data = this->audioPacketData; 
    pkt.size = this->audioPacketSize;
    decodedSize = avcodec_decode_audio3(this->audioStream->codec, (int16_t *)this->audioFrameBuffer, &outputFrameSize, &pkt);

    this->audioFrameSize = outputFrameSize;

    if (this->audioPacketSize - decodedSize <= 0)
    {
        // all the audio frames in the packet have been decoded
        // free the packet now
        av_free_packet(&this->currentPacket);
    }

    if (decodedSize< 0)
    {
        return -1;
    }
    else
    {
        this->audioPacketData += decodedSize;
        this->audioPacketSize -= decodedSize;
    }

    return 0;
}

