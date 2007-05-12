// RegexTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "../MZ3/MyRegex.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "Japanese" ); 
	wprintf( L"---start---\n" );

	std::wstring target 
		= L"<img src=\"http://img.mixi.jp/img/emoji/85.gif\" alt=\"喫煙\" width=\"16\" height=\"16\" class=\"emoji\" border=\"0\">";

	std::wstring pattern
		= L"<img src=\"([^\"]+)\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\".*>";

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

	wprintf( L"---end---\n" );

	return 0;
}

