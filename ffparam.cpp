#include "ffparam.h"

//
// for audio parameters

FFAudioParam::FFAudioParam() : sampleRate(0), channels(0), bitRate(0), codecName("")
{}

FFAudioParam::FFAudioParam(int sampleRate, int channels, int bitRate, std::string codecName) :
    sampleRate(sampleRate), channels(channels), bitRate(bitRate), codecName(codecName) 
{}

bool FFAudioParam::empty()
{
    return bitRate < 1 && sampleRate < 1 && channels < 1 && codecName == "";
}

bool FFAudioParam::isValid()
{
    return !(bitRate < 1 || sampleRate < 1 || channels < 1);
}


//
// for video parameters

FFVideoParam::FFVideoParam() :
    width(0), height(0), pixelFmt(PIX_FMT_NONE), bitRate(0), frameRate(0), codecName("")
{
}

FFVideoParam::FFVideoParam(
    int width, int height, PixelFormat pixelFmt, 
    int bitRate, int frameRate, std::string codecName) :
    width(width), 
    height(height), 
    pixelFmt(pixelFmt), 
    bitRate(bitRate), 
    frameRate(frameRate), 
    codecName(codecName)
{
}

bool FFVideoParam::empty()
{
    return width < 1 && height < 1 && pixelFmt == PIX_FMT_NONE  && 
        bitRate < 1 && frameRate < 1 && codecName == "";
}

bool FFVideoParam::isValid()
{
    return !(width < 1 || height < 1 || pixelFmt == PIX_FMT_NONE || bitRate < 1 || frameRate < 1);
}

void FFVideoParam::updateParam(int w, int h, PixelFormat fmt, int br, int fr, std::string codec)
{
    width = w;
    height = h;
    pixelFmt = fmt;
    bitRate = br;
    frameRate = fr;
    codecName = codec;
}

