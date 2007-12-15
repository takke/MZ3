/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// ŠG•¶Žšƒ}ƒbƒv
class EmojiMap {
public:
	CString code;
	CString url;
	CString text;

	EmojiMap( LPCTSTR code_, LPCTSTR url_, LPCTSTR text_ )
		: code(code_), url(url_), text(text_)
	{
	}
};
typedef std::vector<EmojiMap> EmojiMapList;
