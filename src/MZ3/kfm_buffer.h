/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include <vector>

/// In Memory Kanji-Filter
namespace kfm {

/**
 * getc と同じ動作を vector に対して行うラッパー
 */
template <class T>
class kf_buf_reader
{
private:
	const std::vector<T>& buf_;	///< vector のバッファ
	size_t len_readed;			///< 読み込み済みサイズ

public:
	/// Constructor
	kf_buf_reader( const std::vector<T>& buf )
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

	bool is_eof() const {
		return len_readed >= buf_.size();
	}
};

/**
 * putc と同じ動作を vector に対して行うラッパー
 */
template <class T>
class kf_buf_writer
{
private:
	std::vector<T>& buf_;			///< vector のバッファ

public:
	/// Constructor
	kf_buf_writer( std::vector<T>& buf )
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

}