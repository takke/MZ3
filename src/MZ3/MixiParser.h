/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MZ3Parser.h"

#ifdef BT_MZ3

/// mixi用パーサ
namespace mixi {

/// 各種mixiパーサの基本クラス
class MixiParserBase : public parser::MZ3ParserBase
{
public:
	/**
	 * ログアウトしたかをチェックする
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename );
};

//■■■共通■■■

/**
 * 画像ダウンロードCGI 用パーサ
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MixiParserBase
{
public:

	/**
	 * 画像URL取得
	 */
	static CString GetImageURL( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		/* 
		 * 解析対象文字列：
		 * <img SRC="http://ic76.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// 画像へのリンクを抽出
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<img src=\"", L"\"" ) ) {
				// " から " までを取得する。
				if( util::GetBetweenSubString( line, L"\"", L"\"", uri ) > 0 ) {
					MZ3LOGGER_DEBUG( L"画像へのリンク抽出OK, url[" + uri + L"]" );
					break;
				}
			}
		}
		return uri;
	}
};


}//namespace mixi

#endif	// BT_MZ3
