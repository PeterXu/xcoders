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
 * The handle for audio coder (encoder/decoder) ������Ƶ��/��������codec���ľ������
 */
typedef void* xacoder_t;

typedef unsigned short uword16_t;

/**
 * audio type for ilbc	ilbc����(����/����)
 */
enum xacoder_type_t {
	XA_TUnkonwn = -1,
	XA_ILBC_ENCDOER = 0,
	XA_ILBC_DECODER = 1,
};

/**
 * speech type for ilbc	ilbc��speech����
 */
enum xacoder_speech_t {
	XA_SUnknown = 0,
	XA_ILBC_SPEECH = 1,
	XA_ILBC_CNG = 2,
};

/**
 * Create the audio coder	������Ƶ��/������
 *
 * @param ppcoder	[in] The address of one null handle, [out] the handle of a new coder
 *					[in] ����վ���ĵ�ַ��[out] ���ش�����codec���		
 * @param ctype		[in] XA_ILBC_ENCDOER for encoder or XA_ILBC_DECODER for decoder
 *					[in] XA_ILBC_ENCDOER: ����������, XA_ILBC_DECODER: ����������
 * @return			Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XACODERS_API int xacoder_create(xacoder_t *ppcoder, int ctype);

/**
 * Init the audio coder
 *
 * @param coder 	[in] The coder handle								��/��������codec�����
 * @param framelen	[in] The frame length of the codec 20/30 (ms)		֡����(20/30ms)
 * @return			Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XACODERS_API int xacoder_init(xacoder_t coder, int framelen);

/**
 * Encodes one iLBC frame. Input speech length has to be a multiple of the frame length.
 *
 * @param coder			[in] The coder handle												��/��������codec�����
 * @param speechin		[in] Input speech vector											������Ƶ����
 * @param len			[in] Samples in speechin (160, 240, 320 or 480)						�������ݵĳ���
 * @param encoded		[out] The encoded data vector										�������������
 * @return				Return Length (in bytes) of coded data, or -1 if a error occurs		�ɹ����ر�������ݳ��ȣ�����-1
 */
XACODERS_API int xacoder_encode(xacoder_t coder, const uword16_t *speechin, uword16_t len, uword16_t *encoded);

/**
 * Decodes a packet with iLBC frame(s). Output speech length
 * will be a multiple of 160 or 240 samples ((160 or 240)*frames/packet).
 *
 * @param coder			[in] The coder handle									��/��������codec�����
 * @param encoded		[in] Encoded iLBC frame(s)								��Ҫ�����ilbc֡����
 * @param len			[in] Bytes in encoded vector							ilbc֡���ݳ���
 * @param decoded		[out] The decoded vector								����������
 * @param speechtype	[out] XA_ILBC_SPEECH - normal, XA_ILBC_CNG - CNG		����speech����
 * @return				Return Samples in decoded vector, or -1 if a error occurs	�ɹ����ؽ�������ݳ��ȣ�����-1
 */
XACODERS_API int xacoder_decode(xacoder_t coder, const uword16_t *encoded, uword16_t len, uword16_t *decoded, uword16_t *speechtype);

/**
 * This function conducts PLC for iLBC frame(s). Output speech length
 * will be a multiple of 160 or 240 samples.
 *
 * @param coder				[in] The coder handle											��/��������codec�����
 * @param decoded			[out] The "decoded" vector										����������
 * @param noOfLostFrames	[in] Number of PLC frames to produce							������PLC frames����Ŀ
 * @return					Return Samples in decoded PLC vector, or -1 if a error occurs	�ɹ����ؽ����PLC���ݳ��ȣ�����-1
 */
XACODERS_API int xacoder_decodeplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames);

/**
 * Updates the decoder when a packet loss has occured, but it does not produce any PLC data. 
 * Function can be used if another PLC method is used (i.e NetEq).
 *
 * @param coder				[in] The coder handle											��/��������codec�����
 * @param decoded			[out] The "decoded" vector (nothing in this case)				��ʹ��
 * @param noOfLostFrames	[in] Number of lost frames										������֡��Ŀ
 * @return					Return Samples in decoded PLC vector, or -1 if a error occurs	�ɹ�����PLC���ݳ��ȣ�����-1
 */
XACODERS_API int xacoder_neteqplc(xacoder_t coder, uword16_t *decoded, uword16_t noOfLostFrames);

/**
 * Destroy one coder handle
 *
 * @param ppcoder	[in] The address of coder handle, [out] set it to NULL
 *					[in] �����/��������codec�������[out] ����ΪNULL
 * @return			Return 0 if success, or non-0 if a error occurs	�ɹ�����0�������0ֵ
 */
XACODERS_API int xacoder_destroy(xacoder_t *ppcoder);

#endif
