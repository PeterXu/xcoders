#ifndef _XACODERS_H_
#define _XACODERS_H_

#ifdef WIN32
#ifdef XACODERS_EXPORTS
#define XACODERS_API extern "C" __declspec(dllexport)
#else
#define XACODERS_API extern "C" __declspec(dllimport)
#endif
#else
#define XACODERS_API
#endif

/**
 * The handle for audio coder (encoder/decoder) 定义音频编/解码器（codec）的句柄类型
 */
typedef void* xacoder_t;

typedef unsigned short uword16_t;

/**
 * audio type for ilbc	ilbc类型(编码/解码)
 */
enum xacoder_type_t {
	XA_TUnkonwn = -1,
	XA_ILBC_ENCDOER = 0,
	XA_ILBC_DECODER = 1,
};

/**
 * speech type for ilbc	ilbc的speech类型
 */
enum xacoder_speech_t {
	XA_SUnknown = 0,
	XA_ILBC_SPEECH = 1,
	XA_ILBC_CNG = 2,
};

/**
 * Create the audio coder	创建音频编/解码器
 *
 * @param ppcoder	[in] The address of one null handle, [out] the handle of a new coder
 *					[in] 输入空句柄的地址，[out] 返回创建的codec句柄		
 * @param ctype		[in] XA_ILBC_ENCDOER for encoder or XA_ILBC_DECODER for decoder
 *					[in] XA_ILBC_ENCDOER: 创建编码器, XA_ILBC_DECODER: 创建解码器
 * @return			Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XACODERS_API int xacoder_create(xacoder_t *ppcoder, int ctype);

/**
 * Init the audio coder
 *
 * @param coder 	[in] The coder handle								编/解码器（codec）句柄
 * @param framelen	[in] The frame length of the codec 20/30 (ms)		帧长度(20/30ms)
 * @return			Return 0 if success, or non-0 if a error occurs		成功返回0，否则非0值
 */
XACODERS_API int xacoder_init(xacoder_t coder, int framelen);

/**
 * Encodes one iLBC frame. Input speech length has to be a multiple of the frame length.
 *
 * @param coder			[in] The coder handle												编/解码器（codec）句柄
 * @param speechin		[in] Input speech vector											输入音频数据
 * @param len			[in] Samples in speechin (160, 240, 320 or 480)						输入数据的长度
 * @param encoded		[out] The encoded data vector										输出编码后的数据
 * @return				Return Length (in bytes) of coded data, or -1 if a error occurs		成功返回编码后数据长度，否则-1
 */
XACODERS_API int xacoder_encode(xacoder_t coder, const uword16_t *speechin, uword16_t len, uword16_t *encoded);

/**
 * Decodes a packet with iLBC frame(s). Output speech length
 * will be a multiple of 160 or 240 samples ((160 or 240)*frames/packet).
 *
 * @param coder			[in] The coder handle									编/解码器（codec）句柄
 * @param encoded		[in] Encoded iLBC frame(s)								需要解码的ilbc帧数据
 * @param len			[in] Bytes in encoded vector							ilbc帧数据长度
 * @param decoded		[out] The decoded vector								解码后的数据
 * @param speechtype	[out] XA_ILBC_SPEECH - normal, XA_ILBC_CNG - CNG		设置speech类型
 * @return				Return Samples in decoded vector, or -1 if a error occurs	成功返回解码后数据长度，否则-1
 */
XACODERS_API int xacoder_decode(xacoder_t coder, const uword16_t *encoded, uword16_t len, uword16_t *decoded, uword16_t *speechtype);

/**
 * This function conducts PLC for iLBC frame(s). Output speech length
 * will be a multiple of 160 or 240 samples.
 *
 * @param coder				[in] The coder handle											编/解码器（codec）句柄
 * @param decoded			[out] The "decoded" vector										解码后的数据
 * @param noOfLostFrames	[in] Number of PLC frames to produce							将生成PLC frames的数目
 * @return					Return Samples in decoded PLC vector, or -1 if a error occurs	成功返回解码后PLC数据长度，否则-1
 */
XACODERS_API int xacoder_decodeplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames);

/**
 * Updates the decoder when a packet loss has occured, but it does not produce any PLC data. 
 * Function can be used if another PLC method is used (i.e NetEq).
 *
 * @param coder				[in] The coder handle											编/解码器（codec）句柄
 * @param decoded			[out] The "decoded" vector (nothing in this case)				不使用
 * @param noOfLostFrames	[in] Number of lost frames										丢掉的帧数目
 * @return					Return Samples in decoded PLC vector, or -1 if a error occurs	成功返回PLC数据长度，否则-1
 */
XACODERS_API int xacoder_neteqplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames);

/**
 * Destroy one coder handle
 *
 * @param ppcoder	[in] The address of coder handle, [out] set it to NULL
 *					[in] 输入编/解码器（codec）句柄，[out] 设置为NULL
 * @return			Return 0 if success, or non-0 if a error occurs	成功返回0，否则非0值
 */
XACODERS_API int xacoder_destroy(xacoder_t *ppcoder);

#endif
