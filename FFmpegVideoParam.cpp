#include "stdafx.h"
#include "FFmpegVideoParam.h"

FFmpegVideoParam::FFmpegVideoParam(int width, int height, PixelFormat pixelFormat, int bitRate, int frameRate, std::string videoCodecName) :
width(width), height(height), pixelFormat(pixelFormat), bitRate(bitRate), frameRate(frameRate), videoCodecName(videoCodecName)
{
}

FFmpegVideoParam::FFmpegVideoParam() :
width(0), height(0), pixelFormat(PIX_FMT_NONE), bitRate(0), frameRate(0), videoCodecName("")
{
}

void FFmpegVideoParam::setVideoParam(int w, int h, PixelFormat fmt, int br, int fr, std::string codec)
{
	width = w;
	height = h;
	pixelFormat = fmt;
	bitRate = br;
	frameRate = fr;
	videoCodecName = codec;
}

FFmpegVideoParam::~FFmpegVideoParam()
{
}

bool FFmpegVideoParam::empty()
{
	return width < 1 && height < 1 && pixelFormat == PIX_FMT_NONE  && 
		bitRate < 1 && frameRate < 1 && videoCodecName == "";
}

bool FFmpegVideoParam::isValid()
{
	// valid the arguments
	if (width < 1 || height < 1 || pixelFormat == PIX_FMT_NONE || bitRate < 1 || frameRate < 1)
	{
		return false;
	}

	return true;
}

