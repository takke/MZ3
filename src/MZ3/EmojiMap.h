#pragma once

/// �G�����}�b�v
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
