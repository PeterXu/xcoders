#include "stdafx.h"
#include "FFmpegAudioParam.h"

FFmpegAudioParam::FFmpegAudioParam(int sampleRate, int channels, int bitRate, std::string audioCodecName) : 
    sampleRate(sampleRate), channels(channels), bitRate(bitRate), audioCodecName(audioCodecName)
{
}

FFmpegAudioParam::FFmpegAudioParam() :
    sampleRate(0), channels(0), bitRate(0), audioCodecName("")
{
}

FFmpegAudioParam::~FFmpegAudioParam()
{
}

bool FFmpegAudioParam::empty()
{
    return bitRate < 1 && sampleRate < 1 && channels < 1 && audioCodecName == "";
}

bool FFmpegAudioParam::isValid()
{
    // valid the arguments
    if (bitRate < 1 || sampleRate < 1 || channels < 1)
    {
        return false;
    }

    return true;
}

