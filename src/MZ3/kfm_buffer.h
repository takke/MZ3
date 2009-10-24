/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include <vector>

/// In Memory Kanji-Filter
namespace kfm {

/**
 * getc �Ɠ�������� vector �ɑ΂��čs�����b�p�[
 */
template <class T>
class kf_buf_reader
{
private:
	const std::vector<T>& buf_;	///< vector �̃o�b�t�@
	size_t len_readed;			///< �ǂݍ��ݍς݃T�C�Y
	size_t buf_size;			///< �T�C�Y

public:
	/// Constructor
	kf_buf_reader( const std::vector<T>& buf )
		: buf_(buf), len_readed(0), buf_size(buf.size())
	{
	}
	
	/**
	 * get_char
	 *
	 * �����̏ꍇ�� EOF ��Ԃ�
	 */
	inline int get_char() {
		if (len_readed < buf_size) {
			return buf_[len_readed++];
//			int c = buf_[len_readed];
//			len_readed ++;
//			return c;
		}else{
			return EOF;
		}
	}

	inline bool is_eof() const {
		return len_readed >= buf_size;
	}

	/**
	 * target �������܂œǂݔ�΂��A���̒��O�܂ł̕������ until_text �ɐݒ�(�ǉ�)����
	 */
	inline bool search_until_target( const wchar_t* target, std::wstring* until_text )
	{
		if (is_eof()) {
			return false;
		}
		const wchar_t* p_start = (const wchar_t*)&buf_[len_readed];

		const wchar_t* found_at = wcsstr(p_start, target);
		if (found_at==NULL) {
			len_readed = buf_size;
			return false;
		}

		int until_len = found_at - p_start;
		if (until_text!=NULL) {
			until_text->append(p_start, until_len);
		}

		len_readed += until_len + wcslen(target);

		return true;
	}

	/**
	 * target �������܂œǂݔ�΂��A���̒��O�܂ł̕������ until_text �ɐݒ�(�ǉ�)����
	 */
	inline bool search_until_target( const wchar_t target, std::wstring* until_text )
	{
		if (is_eof()) {
			return false;
		}
		const wchar_t* p_start = (const wchar_t*)&buf_[len_readed];

		const wchar_t* found_at = wcschr(p_start, target);
		if (found_at==NULL) {
			len_readed = buf_size;
			return false;
		}

		int until_len = found_at - p_start;
		if (until_text!=NULL) {
			until_text->append(p_start, until_len);
		}

		len_readed += until_len + 1;	// 1=length of target

		return true;
	}
};

/**
 * putc �Ɠ�������� vector �ɑ΂��čs�����b�p�[
 */
template <class T>
class kf_buf_writer
{
private:
	std::vector<T>& buf_;			///< vector �̃o�b�t�@

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