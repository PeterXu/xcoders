#ifndef _FFMEPGAUDIOPARAM_H_
#define _FFMEPGAUDIOPARAM_H_

#include <string>

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
/// @brief  The audio parameter class for FFmpegEncoder initializing
///
class FFMPEG_EXPORT FFmpegAudioParam
{
public:
	///
	/// @brief  Constructor for initializing an object of FFmpegAudioParam
	///
	/// @param  [in] sampleRate     The sample rate of the audio, must be greater than 0
	/// @param  [in] channels       The number of channels in the audio, must be greater than 0
	/// @param  [in] bitRate        The target bit rate of the target audio stream, must be greater than 0
	/// @param  [in] audioCodecName The name of the audio codec which is going to be used in encoding/decoding
	///
	FFmpegAudioParam(int sampleRate, int channels, int bitRate, std::string audioCodecName = "");

	///
	/// @brief  Constructor for initializing an empty FFmpegAudioParam object
	///
	FFmpegAudioParam();

	///
	/// @brief  Destructor
	///
	virtual ~FFmpegAudioParam();

	///
	///	@brief  Judge whether a FFmpegAudioParam object is empty
	///	
	bool empty();

	///
	///	@brief  Judge whether a FFmpegAudioParam object's parameters are right
	///	
	bool isValid();

public:
	int sampleRate;             ///< The sample rate of the audio
	int channels;               ///< The number of audio channels
	int bitRate;                ///< The bit rate of the audio
	std::string audioCodecName;	///< The name of the audio codec
};

#endif
