#ifndef _FFMEPGENCODER_H_
#define _FFMEPGENCODER_H_

extern "C"
{
#define INT64_C
#define __STDC_LIMIT_MACROS
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

#include "FFmpegVideoParam.h"
#include "FFmpegAudioParam.h"


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
/// @brief  The video/audio encoder class for wrapping FFmpeg encoding API.
///
class FFMPEG_EXPORT FFmpegEncoder
{
public:
	//////////////////////////////////////////////////////////////////////////
	//
	//  Initialization and finalization
	//
	//////////////////////////////////////////////////////////////////////////

	///
	/// @brief  Constructor for initializing an object of FFmpegEncoder.
	///
	/// If A/V stream is not needed, pass an empty FFmpegVideoParam/FFmpegAudioParam directly.
	///	For example, the following codes create a "flv" encoder with audio only:
	/// @verbatim
	///		FFmpegVideoParam videoParam;
	///		FFmpegAudioParam audioParam(64000, 44100, 2);
	///		FFmpegEncoder testVideo(videoParam, audioParam);
	///         testVideo.open("test.flv");
	///         //... codes go here
	///         testVideo.close();
	/// @endverbatim
	///
	/// If no output, A/V codec must be specified in the FFmpegVideoParam/FFmpegAudioParam.
	///	For example:
	/// @verbatim
	///		FFmpegVideoParam videoParam(352, 288, PIX_FMT_YUV420P, 400000, 25, "flv");
	///		FFmpegAudioParam audioParam(64000, 44100, 2, "libmp3lame");
	///		FFmpegEncoder testVideo(videoParam, audioParam);
	///         testVideo.open();
	///         //... codes go here
	///         testVideo.close();
	/// @endverbatim
	///
	/// @param  [in] videoParam   The video parameters to initialize the FFmpegEncoder.
	/// @param  [in] audioParam   The audio parameters to initialize the FFmpegEncoder.
	///
	FFmpegEncoder(const FFmpegVideoParam &videoParam, const FFmpegAudioParam &audioParam);

	// only video
	FFmpegEncoder(const FFmpegVideoParam &videoParam);

	// only audio
	FFmpegEncoder(const FFmpegAudioParam &audioParam);

	///
	/// @brief  Destructor
	///
	virtual ~FFmpegEncoder();


public:
	//////////////////////////////////////////////////////////////////////////
	//
	//  Public properties
	//
	//////////////////////////////////////////////////////////////////////////

	///
	/// @brief  Get the video buffer which contains the encoded frame data.
	///
	/// @return An uint8_t pointer to the encoded video frame data.
	/// @retval NULL Encoder is not opened or there is no video to be encoded.
	///
	const uint8_t *getVideoEncodedBuffer() const;

	///
	/// @brief  Get the presentation time stamp of the video stream being encoded.
	///
	/// This is usually used for synchronizing the video and audio streams.
	///
	/// @return A non-negative double representing the time stamp (in seconds).
	/// @retval 0 Encoder is not opened or there is no video to be encoded or output.
	///
	double getVideoTimeStamp() const;

	///
	/// @brief  Get the video parameters
	///
	const FFmpegVideoParam &getVideoParam() const;

	///
	/// @brief  Get the size of the raw video frame (unit: in bytes/uint8_t).
	///
	/// This is usually used for allocating memory for the raw video frame data.
	///
	/// @return A non-negative int representing the number of bytes (unit: in bytes/uint8_t).
	/// @retval 0 Encoder is not opened or there is no video to be encoded.
	///
	int getVideoFrameSize() const;

	///
	/// @brief  Get the audio buffer which contains the encoded frame data.
	///
	/// @return An uint8_t pointer to the encoded audio frame data.
	/// @retval NULL Encoder is not opened or there is no audio to be encoded.
	///
	const uint8_t *getAudioEncodedBuffer() const;

	///
	/// @brief  Get the presentation time stamp of the audio stream being encoded.
	///
	/// This is usually used for synchronizing the video and audio streams.
	///
	/// @return A non-negative double representing the time stamp (in seconds).
	/// @retval 0 Encoder is not opened or there is no audio to be encoded or output.
	///
	double getAudioTimeStamp() const;

	///
	/// @brief  Get the audio parameters
	///
	const FFmpegAudioParam &getAudioParam() const;

	///
	/// @brief  Get the size of raw audio frame including all channels (unit: in bytes/uint8_t).
	///
	/// This is usually used for allocating memory for the raw audio frame data.
	///
	/// @return A non-negative int representing the number of bytes including all audio channels (unit: in bytes/uint8_t).
	/// @retval 0 Encoder is not opened or this is no audio to be encoded.
	///
	int getAudioFrameSize() const;


public:
	//////////////////////////////////////////////////////////////////////////
	//
	//  Public Methods
	//
	//////////////////////////////////////////////////////////////////////////

	///
	/// @brief  Open the codec, output file and allocate the necessary internal structures.
	///
	/// Must be called before encoding process.
	///
	/// @param  [in/opt] fileName  The name of the file to which encoded results are written.
	///
	int open(const char *fileName = NULL);

	///
	/// @brief  Close the codec, output file and release the memories
	///
	/// Must be called after encoding process is finished.
	///
	void close();

	///
	/// @brief  Encode one video frame (just encode, won't write encoded data to output file).
	///
	/// The encoded frame data can be retrieved by calling getVideoEncodedBuffer().
	/// Use this method only when this is no output, otherwise use writeVideoFrame() instead.
	///
	/// @param  [in] frameData The image data of the frame to be encoded.
	///
	/// @return A non-negative int representing the size of the encoded buffer.
	///
	int encodeVideoFrame(const uint8_t *frameData);
	int encodeVideoFrame(const uint8_t *frameData, PixelFormat format, int width, int height);

	///
	/// @brief  Encode one audio frame (just encode, won't write encoded data to output file).
	///
	/// The encoded frame data can be retrieved by calling getAudioEncodedBuffer().
	/// Use this method only when this is no output, otherwise use writeAudioFrame() instead.
	///
	/// @param  [in]     frameData The audio data of the frame to be encoded.
	/// @param  [in/opt] dataSize  The size of audio frame data, required for PCM related codecs.
	///
	/// @return A non-negative int representing the size of the encoded buffer.
	///
	int encodeAudioFrame(const uint8_t *frameData, int dataSize = 0);

private:
	//////////////////////////////////////////////////////////////////////////
	//
	//  Private Definitions
	//
	//////////////////////////////////////////////////////////////////////////

	bool encodeVideo;   ///< Whether video encoding is needed
	bool encodeAudio;   ///< Whether audio encoding is needed
	bool opened;        ///< Whether the FFmpegEncoder is opened yet

	FFmpegVideoParam videoParam;    ///< The video parameters of the video to be encoded
	FFmpegAudioParam audioParam;    ///< The audio parameters of the audio to be encoded

	AVFormatContext *outputContext; ///< The output format context
	AVStream *videoStream;          ///< The video output stream
	AVStream *audioStream;          ///< The audio output stream

	AVPicture *videoFrame;          ///< The temporary video frame for pixel format conversion
	uint8_t *videoBuffer;       ///< The video output buffer
	int     videoBufferSize;    ///< The size of video output buffer

	uint8_t *audioBuffer;       ///< The audio output buffer
	int     audioBufferSize;    ///< The size of audio output buffer

private:
	//////////////////////////////////////////////////////////////////////////
	//
	//  Private Methods
	//
	//////////////////////////////////////////////////////////////////////////

	// init parameters for encode
	void init();

	///
	/// @brief  Encode a video frame to the internal encoded data buffer
	///
	/// @param  [in] picture    The video frame data to be encoded
	///
	/// @return A non-negative int represents the size of the encoded data
	///
	int encodeVideoData(AVPicture *picture, FFmpegVideoParam &inParam);

	///
	/// @brief  Convert the pixel format of the input image
	///
	/// @param  [in]  srcParam    The parameters of the source image picture
	/// @param  [in]  dstContext  The codec context of the output (destination) image picture
	/// @param  [in]  srcPic      The source image picture to be converted
	/// @param  [out] dstPic      Return the output image picture which has been converted
	///
	/// @return 0 when conversion is successful, otherwise a negative int
	///
	int convertPixFmt(AVPicture *srcPic, AVPicture *dstPic, const FFmpegVideoParam *srcParam, const AVCodecContext *dstContext);

	///
	/// @brief  Encode an audio frame to the internal encoded data buffer
	///
	/// @param  [in]          frameData  The audio frame data to be encoded (in short)
	/// @param  [in/optional] dataSize   The size of audio frame to be encoded (unit: in short)
	///
	/// @return A non-negative int represents the size of the encoded data
	///
	int encodeAudioData(short *frameData, int dataSize);
};

#endif

