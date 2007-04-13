#pragma once

#include <vector>

/// In Memory Kanji-Filter
namespace kfm {

typedef std::vector<unsigned char> kf_buf_type;

/**
 * getc と同じ動作を vector に対して行うラッパー
 */
class kf_buf_reader
{
private:
	const kf_buf_type& buf_;	///< vector のバッファ
	size_t len_readed;			///< 読み込み済みサイズ

public:
	/// Constructor
	kf_buf_reader( const kf_buf_type& buf )
		: buf_(buf), len_readed(0)
	{
	}
	
	/**
	 * get_char
	 *
	 * 末尾の場合は EOF を返す
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
 * putc と同じ動作を vector に対して行うラッパー
 */
class kf_buf_writer
{
private:
	kf_buf_type& buf_;			///< vector のバッファ

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
private:
	int verbose;
	int mime;
	int text;
	int quote;

	kf_buf_reader	inbuf;
	kf_buf_writer	outbuf;

public:
//	kfm( FILE* fp_in, FILE* fp_out )
//		: infile(fp_in), outfile(fp_out)
	kfm( const kf_buf_type& buf_in, kf_buf_type& buf_out )
		: inbuf(buf_in), outbuf(buf_out)
	{
		verbose = 0;
		mime = 0;
		text = 0;
		quote = 0;
	}

	virtual ~kfm()
	{
	}

public:
	void error(char *format, ...);

//	void setInBuffer( kf_buf_type* buf ) {}

	void tosjis(void);
	void tojis(void);
	void toeuc(void);

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