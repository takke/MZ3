// RegexTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "../MZ3/MyRegex.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "Japanese" ); 
	wprintf( L"---start---\n" );

	std::wstring target 
		= L"<img src=\"http://img.mixi.jp/img/emoji/85.gif\" alt=\"喫煙\" width=\"16\" height=\"16\" class=\"emoji\" border=\"0\">a<img src=\"http://img.mixi.jp/img/emoji/85.gif\" alt=\"喫煙\" width=\"16\" height=\"16\" class=\"emoji\" border=\"0\">";

	std::wstring pattern
		= L"<img src=\"http://img.mixi.jp/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>";

	target = L"<table border=0 cellpadding=0 cellspacing=0 width=1 align=right><tr><td bgcolor=\"#F8A448\"><table border=0 cellpadding=2 cellspacing=1><tr align=left><td bgcolor=\"#eeeeee\"><img alt=\"(C)1989,2002,2007 SQUARE ENIX CO., LTD. All Rights Reserved.　写真：ITmedia Copyright(c) ITmedia, Inc. All Rights Reserved.\" src=\"http://news.mixi.jp/img/news/ITmedia_200705130060000thumb.jpg\" border=1 Vspace=\"2\" hspace=\"2\"><br><font size=\"-1\">(C)1989,2002,2007 SQUARE ENIX CO., LTD. All Rights Reserved.　写真：ITmedia Copyright(c) ITmedia, Inc. All Rights Reserved.</font></td></tr></table></td><td></td></tr></table>";
	pattern = L"<img[^>]*src=\"([^\"]+)\" [^>]*>";

	wprintf( L"target [%s]\n", target.c_str() );
	wprintf( L"pattern[%s]\n", pattern.c_str() );

	// 正規表現のコンパイル（一回のみ）
	static MyRegex reg;
	if( !reg.isCompiled() ) {
		if(! reg.compile( pattern.c_str() ) ) {
			// コンパイル失敗なのでそのまま終了。
			// つーかコンパイル失敗はNGだorz
			wprintf( L"cannot compile...\n" );
			return 0;
		}
	}

	// 探索
	if( reg.exec(target.c_str()) == false ) {
		// 未発見。
		wprintf( L"not found...\n" );
	}else{
		wprintf( L"found : %d\n", reg.results.size() );
		for( u_int i=0; i<reg.results.size(); i++ ) {
			MyRegex::Result& result = reg.results[i];
			wprintf( L"+ %2d\n", i );
			wprintf( L"  start[%d]\n", result.start );
			wprintf( L"  end  [%d]\n", result.end );
			wprintf( L"  str  [%s]\n", result.str.c_str() );
		}
	}

	// 置換テスト
	LPCTSTR replace = L"(({1}))";
	reg.replaceAll( target, replace );
	wprintf( L" replace         [%s]\n", replace );
	wprintf( L" replaced target [%s]\n", target.c_str() );

	wprintf( L"---end---\n" );

	return 0;
}

