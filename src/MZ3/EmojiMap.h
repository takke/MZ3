/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
