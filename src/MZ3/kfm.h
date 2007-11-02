#pragma once

#include <vector>

/// In Memory Kanji-Filter
namespace kfm {

#define DEFAULT_OCODE  JIS      /* �f�t�H�[���g�� JIS �o�� */
#define BUFFER_SIZE    4096

/* MAXMIMELEN �͋K�i�ł� 75 ��������Ă��Ȃ��̂ő��߂ɂƂ� */
#define MAXMIMELEN      512

typedef std::vector<unsigned char> kf_buf_type;

/**
 * getc �Ɠ�������� vector �ɑ΂��čs�����b�p�[
 */
class kf_buf_reader
{
private:
	const kf_buf_type& buf_;	///< vector �̃o�b�t�@
	size_t len_readed;			///< �ǂݍ��ݍς݃T�C�Y

public:
	/// Constructor
	kf_buf_reader( const kf_buf_type& buf )
		: buf_(buf), len_readed(0)
	{
	}
	
	/**
	 * get_char
	 *
	 * �����̏ꍇ�� EOF ��Ԃ�
	 */
	int get_char()
	{
		if( len_readed < buf_.size() ) {
			int c = buf_[len_readed];
			len_readed ++;
			return c;
		}else{
			return EOF;
		}
	}
};

/**
 * putc �Ɠ�������� vector �ɑ΂��čs�����b�p�[
 */
class kf_buf_writer
{
private:
	kf_buf_type& buf_;			///< vector �̃o�b�t�@

public:
	/// Constructor
	kf_buf_writer( kf_buf_type& buf )
		: buf_(buf)
	{
	}
	
	/**
	 * put_char
	 */
	int put_char( int c )
	{
		buf_.push_back( c );
		return c;
	}
};

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

	kf_buf_reader	inbuf;
	kf_buf_writer	outbuf;
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

}