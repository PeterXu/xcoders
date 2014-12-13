#ifndef _FFPARAM_H_
#define _FFPARAM_H_

#include <string>

class FF_EXPORT FFParam
{
pulic:
    virtual ~FFParam() {}

    /**
     * Judge whether one FFParam's parameters are empty or not.
     */
    virtual bool empty() = 0;

    /**
     * Judge whether one FFParam's parameters are right or not.
     */
    virtual bool isValid() = 0;
};

class FF_EXPORT FFAudioParam : public FFParam
{
public:
    /**
     * Constructor for initializing an empty FFAudioParam object
     */
    FFAudioParam();

    /**
     * Constructor for initializing an object of FFAudioParam
     *
     * @param  [in] sampleRate     The sample rate of the audio, must be greater than 0
     * @param  [in] channels       The number of channels in the audio, must be greater than 0
     * @param  [in] bitRate        The target bit rate of the target audio stream, must be greater than 0
     * @param  [in] codecName      The name of the audio codec which is going to be used in encoding/decoding
     */
    FFAudioParam(int sampleRate, int channels, int bitRate, std::string codecName="");

    virtual bool empty();
    virtual bool isValid();

public:
    int sampleRate;             ///< The sample rate of the audio
    int channels;               ///< The number of audio channels
    int bitRate;                ///< The bit rate of the audio
    std::string codecName;      ///< The name of the audio codec
};


class FF_EXPORT FFVideoParam : public FFParam
{
public:
    /**
     * Constructor for initializing an empty FFVideoParam object
     */
    FFVideoParam();

    /**
     * Constructor for initializing an object of FFVideoParam
     *
     * @param  [in] width          The width of the video frame, must be greater than 0
     * @param  [in] height         The height of the video frame, must be greater than 0
     * @param  [in] pixelFormat    PixelFormat enum representing the pixel format of the source video frame
     * @param  [in] bitRate        The target bit rate of the target video stream, must be greater than 0
     * @param  [in] frameRate      The frame rate of the target video, must be greater than 0
     * @param  [in] codecName      The name of the video codec which is going to be used in encoding/decoding
     */
    FFVideoParam(int width, int height, PixelFormat pixelFmt, int bitRate, int frameRate, std::string codecName="");

    virtual bool empty();
    virtual bool isValid();

    void updateParam(int width, int height, PixelFormat pixelFmt, int bitRate, int frameRate, std::string codecName="");

public:
    int width;                  ///< The width of the video
    int height;                 ///< The height of the video
    PixelFormat pixelFmt;       ///< The pixel format of the video
    int bitRate;                ///< The bit rate of the video
    int frameRate;              ///< The frame rate of the video
    std::string codecName;      ///< The name of the video codec
};

#endif // _FFPARAM_H_
