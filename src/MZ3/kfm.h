#pragma once

#include <vector>
#include "kfm_buffer.h"

/// In Memory Kanji-Filter
namespace kfm {

#define DEFAULT_OCODE  JIS      /* デフォールトは JIS 出力 */
#define BUFFER_SIZE    4096

/* MAXMIMELEN は規格では 75 だが守られていないので多めにとる */
#define MAXMIMELEN      512

typedef std::vector<unsigned char> kf_buf_type;

/**
 * In Memory Kanji-Filter
 */
class kfm
{
public:
	enum CODE_TYPE { UNKNOWN, SJIS, EUC, JIS };

private:
	int verbose;
	int mime;
	int text;
	int quote;
	CODE_TYPE icode;
	CODE_TYPE default_icode;
	CODE_TYPE ocode;

	kf_buf_reader<unsigned char>	inbuf;
	kf_buf_writer<unsigned char>	outbuf;
public:
	kfm( const kf_buf_type& buf_in, kf_buf_type& buf_out )
		: inbuf(buf_in)
		, outbuf(buf_out)
		, verbose(0)
		, mime(0)
		, text(0)
		, quote(0)
		, default_icode(UNKNOWN)
		, ocode(DEFAULT_OCODE)
	{
	}

	virtual ~kfm()
	{
	}

public:
	void error(char *format, ...);

	void tosjis(void);
	void tojis(void);
	void toeuc(void);

	void set_default_input_code( CODE_TYPE t )
	{
		default_icode = t;
	}

private:
	void sjis_to_jis(int *ph, int *pl);
	void jis_to_sjis(int *ph, int *pl);
	void guess(int imax, unsigned char buf[]);
	int dehex(char c);
	int decode_mime(unsigned char *buf);
	int quoted_getc(void);
	int mime_getword(unsigned char *buf, int c);
	int mime_getspaces(unsigned char *buf, int c);
	int mime_getc(void);
	int mygetc(void);
	void myputc(int c);
};

/// UTF-8からMBCSへの変換
int utf8_to_mbcs( const kf_buf_type& input, kf_buf_type& output );

/// UTF-8からUCS2(SJIS)への変換
int utf8_to_ucs2( const kf_buf_type& input, std::vector<TCHAR>& output );

/// UTF-8からUCS2(SJIS)への変換
int utf8_to_ucs2( const char* input, std::vector<TCHAR>& output, int input_length=-1 );

/// UCS2(SJIS) から UTF-8 への変換
bool ucs2_to_utf8(const CString& strText, CStringA& strResult);

}// namespace kfm

inline CStringA MyWCS2UTF8(const LPCTSTR wcs)
{
	if (wcs==NULL) {
		return NULL;
	}
	CStringA utf8;
	kfm::ucs2_to_utf8(wcs, utf8);
	return utf8;
}

inline CStringW MyUTF82WCS2(const char* utf8)
{
	if (utf8==NULL) {
		return NULL;
	}
	std::vector<TCHAR> wcs;
	kfm::utf8_to_ucs2(utf8, wcs);
	return &wcs[0];
}
