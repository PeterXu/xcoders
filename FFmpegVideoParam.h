#ifndef _FFMPEGVIDEOPARAM_H_
#define _FFMPEGVIDEOPARAM_H_

#include <string>

extern "C"
{
#include "libavformat/avformat.h"
}

#ifdef WIN32_NULL
#ifdef DLL_FILE
#   define FFMPEG_EXPORT _declspec(dllexport)
#else
#   define FFMPEG_EXPORT _declspec(dllimport)
#endif
#else
#   define FFMPEG_EXPORT
#endif

///
/// @brief  The video parameter class for FFmpegEncoder initializing
///
class FFMPEG_EXPORT FFmpegVideoParam
{
public:
	///
	/// @brief  Constructor for initializing an object of FFmpegVideoParam
	///
	///	@param  [in] width          The width of the video frame, must be greater than 0
	///	@param  [in] height         The height of the video frame, must be greater than 0
	///	@param  [in] pixelFormat    PixelFormat enum representing the pixel format of the source video frame
	///	@param  [in] bitRate        The target bit rate of the target video stream, must be greater than 0
	///	@param  [in] frameRate      The frame rate of the target video, must be greater than 0
	///	@param  [in] videoCodecName The name of the video codec which is going to be used in encoding/decoding
	///
	FFmpegVideoParam(int width, int height, PixelFormat pixelFormat, int bitRate, int frameRate, std::string videoCodecName = "");

	///
	/// @brief  Constructor for initializing an empty FFmpegVideoParam object
	///
	FFmpegVideoParam();

	void setVideoParam(int width, int height, PixelFormat pixelFormat, int bitRate, int frameRate, std::string videoCodecName = "");
	///
	/// @brief  Destructor
	///
	virtual ~FFmpegVideoParam();

	///
	///	@brief  Judge whether a FFmpegVideoParam object is empty
	///	
	bool empty();

	///
	///	@brief  Judge whether a FFmpegVideoParam object's parameters are right
	///	
	bool isValid();

public:
	int width;                  ///< The width of the video
	int height;                 ///< The height of the video
	PixelFormat pixelFormat;    ///< The pixel format of the video
	int bitRate;                ///< The bit rate of the video
	int frameRate;              ///< The frame rate of the video
	std::string videoCodecName; ///< The name of the video codec
};

#endif

