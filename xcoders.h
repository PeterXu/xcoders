// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� XCODERS_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// XCODERS_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef XCODERS_EXPORTS
#define XCODERS_API extern "C" __declspec(dllexport)
#else
#define XCODERS_API extern "C" __declspec(dllimport)
#endif


////////////////////////////////////////////////////////////
///=========================================================


/**
 * The handle for coder	�����/��������codec���ľ��
 */
typedef void* xcoder_t;

/**
 * The type for coder ��/������
 */
enum xcoder_type_t {
	XCODER_ENCODER,
	XCODER_DECODER,
};

/**
 * Create one coder and return its handle	����һ����/��������codec������������
 *
 * @param pcoder	[in] The address of one null handle, [out] the handle of a new coder
 *					[in] ����վ���ĵ�ַ��[out] ���ش�����codec���
 * @param ctype		[in] coder type, @refer xcoder_type_t			���ñ�/������
 * @return 		Return 0 if success, or non-0 if a error occurs		�ɹ�����0, �����0ֵ
 */
XCODERS_API int xcoder_create(xcoder_t *ppcoder, int ctype);

/** 
 * Format types	֧�ֵ�����������ɫ�ռ�
 */
enum xcoder_colorspace_t {
    XCODER_FMT_I420,
    XCODER_FMT_RGBA,
};

/** 
 * FEC types ֧�ֵ�FEC����
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
 * Set option for coder output	����codec���������
 *
 * @param coder 	[in] The coder handle								��/��������codec�����
 * @param format 	[in] the format of encoded/decoded frame			���������Ƶ���ݵĸ�ʽ(����Ϊxcoder_format_t)
 * @param fps		[in] the fps of encoded/decoded frames				���������Ƶ���ݵ�fps
 * @param bitrate 	[in] the bitrate of encoded/decoded frames(bit/s)	���������Ƶ���ݵ�bitrate��bit/s��
 * @param fec 		[in] whether to use fec, @refer xcoder_fec_t		����fec(xcoder_fec_t)
 * @return 		Return 0 if success, or non-0 if a error occurs			�ɹ�����0�������0ֵ
 */
XCODERS_API int xcoder_set_options(xcoder_t coder, xcoder_format_t format, int fps, int bitrate, int fec);

/** 
 * Callback types	�ص�����
 */
enum xcoder_cb_t {
    XCODER_CB_ENCODED_FRAME,
    XCODER_CB_DECODED_FRAME,
    XCODER_CB_ERROR,
};

/** 
 * Callback for coder results	�ص�����ָ��
 *
 * @param cbtype 	[in] The type of this callback, @refer xcoder_cb_t	�ص�����(ֵΪxcoder_cb_t)
 * @param data 		[in] The output data								��/��������codec�����������
 * @param size  	[in] The size of output data						��/��������codec��������ݵĴ�С
 * @param priv  	[in] The private data								�û��Զ�������
 */
typedef void (*xcoder_callback_t) (int cbtype, const unsigned char *data, int size, void *priv);

/**
 * Open one coder	�򿪱�/��������codec��
 *
 * @param coder 	[in] The coder handle							��/��������codec�����
 * @param cb 		[in] The callback function for coder results	��/��������codec��������ص�����
 * @param priv  	[in] The private data, which will be returned in xcoder_callback_t	�û��Զ�������, ����xcoder_callback_t�е����һ����������
 * @return 		Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XCODERS_API int xcoder_open(xcoder_t coder, xcoder_callback_t cb, void *priv);


/**
 * Encode/Decode one frame	��/����һ֡����
 *
 * @param coder 	[in] The coder handle									��/��������codec�����
 * @param data 		[in] The input buffer									�������ݴ�ŵ�buffer
 * @param size 		[in] The size of input frame							�������ݵĴ�С
 * @param format 	[in] The format of input frame, @refer xcoder_format_t	�������ݵĸ�ʽ(ֵ����Ϊxcoder_format_t)
 * @return 		Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XCODERS_API int xcoder_code_frame(xcoder_t xcoder, unsigned char *data, int size, xcoder_format_t format);

/**
 * Close one coder	�رձ�/��������codec��
 *
 * @param coder 	[in] The coder handle							��/��������codec�����
 * @return 		Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XCODERS_API int xcoder_close(xcoder_t coder);

/**
 * Destroy one coder handle	����һ����/��������codec��
 *
 * @param pcoder	[in] The address of coder handle, [out] set it to NULL
 *					[in] �����/��������codec�������[out] ����ΪNULL
 * @return 		Return 0 if success, or non-0 if a error occurs		�ɹ�����0�������0ֵ
 */
XCODERS_API int xcoder_destroy(xcoder_t *ppcoder);


