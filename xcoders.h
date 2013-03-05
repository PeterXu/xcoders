// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 XCODERS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// XCODERS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef XCODERS_EXPORTS
#define XCODERS_API extern "C" __declspec(dllexport)
#else
#define XCODERS_API extern "C" __declspec(dllimport)
#endif


////////////////////////////////////////////////////////////
///=========================================================


/**
 * The handle for coder	定义编/解码器（codec）的句柄
 */
typedef void* xcoder_t;

/**
 * The type for coder 编/解码器
 */
enum xcoder_type_t {
	XCODER_ENCODER,
	XCODER_DECODER,
};

/**
 * Create one coder and return its handle	创建一个编/解码器（codec）并返回其句柄
 *
 * @param pcoder	[in] The address of one null handle, [out] the handle of a new coder
 *					[in] 输入空句柄的地址，[out] 返回创建的codec句柄
 * @param ctype		[in] coder type, @refer xcoder_type_t			设置编/解码器
 * @return 		Return 0 if success, or non-0 if a error occurs		成功返回0, 否则非0值
 */
XCODERS_API int xcoder_create(xcoder_t *ppcoder, int ctype);

/** 
 * Format types	支持的输入数据颜色空间
 */
enum xcoder_colorspace_t {
    XCODER_FMT_I420,
    XCODER_FMT_RGBA,
};

/** 
 * FEC types 支持的FEC类型
 */
enum xcoder_fec_t {
    XCODER_FEC_NONE,
    XCODER_FEC_DEFAULT,
};

typedef struct xcoder_format_t {
	int width;
	int height;
	int colorspace;	/* @refer xcoder_colorspace_t */
}xcoder_format_t;

/**
 * Set option for coder output	设置codec的输出参数
 *
 * @param coder 	[in] The coder handle								编/解码器（codec）句柄
 * @param format 	[in] the format of encoded/decoded frame			设置输出视频数据的格式(类型为xcoder_format_t)
 * @param fps		[in] the fps of encoded/decoded frames				设置输出视频数据的fps
 * @param bitrate 	[in] the bitrate of encoded/decoded frames(bit/s)	设置输出视频数据的bitrate（bit/s）
 * @param fec 		[in] whether to use fec, @refer xcoder_fec_t		设置fec(xcoder_fec_t)
 * @return 		Return 0 if success, or non-0 if a error occurs			成功返回0，否则非0值
 */
XCODERS_API int xcoder_set_options(xcoder_t coder, xcoder_format_t format, int fps, int bitrate, int fec);

/** 
 * Callback types	回调类型
 */
enum xcoder_cb_t {
    XCODER_CB_ENCODED_FRAME,
    XCODER_CB_DECODED_FRAME,
    XCODER_CB_ERROR,
};

/** 
 * Callback for coder results	回调函数指针
 *
 * @param cbtype 	[in] The type of this callback, @refer xcoder_cb_t	回调类型(值为xcoder_cb_t)
 * @param data 		[in] The output data								编/解码器（codec）的输出数据
 * @param size  	[in] The size of output data						编/解码器（codec）输出数据的大小
 * @param priv  	[in] The private data								用户自定义数据
 */
typedef void (*xcoder_callback_t) (int cbtype, const unsigned char *data, int size, void *priv);

/**
 * Open one coder	打开编/解码器（codec）
 *
 * @param coder 	[in] The coder handle							编/解码器（codec）句柄
 * @param cb 		[in] The callback function for coder results	编/解码器（codec）的输出回调函数
 * @param priv  	[in] The private data, which will be returned in xcoder_callback_t	用户自定义数据, 将在xcoder_callback_t中的随后一个参数返回
 * @return 		Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XCODERS_API int xcoder_open(xcoder_t coder, xcoder_callback_t cb, void *priv);


/**
 * Encode/Decode one frame	编/解码一帧数据
 *
 * @param coder 	[in] The coder handle									编/解码器（codec）句柄
 * @param data 		[in] The input buffer									输入数据存放的buffer
 * @param size 		[in] The size of input frame							输入数据的大小
 * @param format 	[in] The format of input frame, @refer xcoder_format_t	输入数据的格式(值类型为xcoder_format_t)
 * @return 		Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XCODERS_API int xcoder_code_frame(xcoder_t xcoder, unsigned char *data, int size, xcoder_format_t format);

/**
 * Close one coder	关闭编/解码器（codec）
 *
 * @param coder 	[in] The coder handle							编/解码器（codec）句柄
 * @return 		Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XCODERS_API int xcoder_close(xcoder_t coder);

/**
 * Destroy one coder handle	销毁一个编/解码器（codec）
 *
 * @param pcoder	[in] The address of coder handle, [out] set it to NULL
 *					[in] 输入编/解码器（codec）句柄，[out] 设置为NULL
 * @return 		Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XCODERS_API int xcoder_destroy(xcoder_t *ppcoder);


