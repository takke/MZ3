// RegexTest.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "../MZ3/MyRegex.h"
#include "../MZ3/xml2stl.h"

int test_date()
{
	LPCTSTR target_list[] = {
		L"<span class=\"date\">2007年7月05日&nbsp;1:55</span></dt>",
		L"<span class=\"date\">7月05日&nbsp;1:55</span></dt>",
		L"<td>10月08日</td></tr>",
		L"<td WIDTH=\"1%\" nowrap CLASS=\"f08\">10月14日 16:47</td></tr>",
		L"10月14日 16:47",
		L"<dd>2007年10月07日01:27</dd>",
		L"<font COLOR=#996600>日　付</font>&nbsp;:&nbsp;2007年10月08日 21時52分&nbsp;&nbsp;",
		NULL
	};

	std::wstring pattern = L"";
	pattern = L"([0-9]{2,4})?年?([0-9]{1,2}?)月([0-9]{1,2})日[^0-9]*([0-9]{1,2})?:?時?([0-9]{2})?";
//	pattern = L"<td.+?>(.+?)</";

	for (int iTarget=0; target_list[iTarget]!=NULL; iTarget++ ) {
		wprintf( L"---start---\n" );
		std::wstring target = target_list[iTarget];

		wprintf( L"target [%s]\n", target.c_str() );
		wprintf( L"pattern[%s]\n", pattern.c_str() );

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( pattern.c_str() ) ) {
				// コンパイル失敗なのでそのまま終了。
				wprintf( L"cannot compile...\n" );
				return 0;
			}
		}
		wprintf( L"compile, ok.\n" );

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
//				wprintf( L"  strd [%d]\n", _wtoi(result.str.c_str()) );
			}
		}

		// 置換テスト
	/*	LPCTSTR replace = L"(({1}))";
		reg.replaceAll( target, replace );
		wprintf( L" replace         [%s]\n", replace );
		wprintf( L" replaced target [%s]\n", target.c_str() );
	*/
	}
	wprintf( L"---end---\n" );

	return 1;
}

using namespace xml2stl;

int test_xml2stl()
{
	printf( "--start--\n" );
//	getchar();
	{
		Container root;

		xml2stl::SimpleXmlParser::loadFromFile( root, L"C:\\Users\\takke\\Desktop\\MZ4.0.9.0.0bXX\\log\\tracks.xml" );

		// dump
	//	dump_container( root, stdout );

		try {
			const Node& feed = root.getNode( _T("feed") );
			size_t n = feed.getChildrenCount();
			for( size_t i=0; i<n; i++ ) {
				wprintf( _T("  entry[%d] = %s\n"), 
					i, 
					feed.getNode( _T("entry"), i ).getNode( _T("title") ).getText().c_str() );
			}
		} catch( NodeNotFoundException& ) {
			wprintf( _T("node not found...\n") );
		}
//		getchar();
		wprintf( _T("--end--\n") );
	}
//	getchar();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "Japanese" ); 

	return test_xml2stl();

//	test_date();

	std::wstring target = L"2007-11-07T08:00:40Z";
	std::wstring pattern = L"";
	pattern = L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})Z";

	// 正規表現のコンパイル（一回のみ）
	static MyRegex reg;
	if( !reg.isCompiled() ) {
		if(! reg.compile( pattern.c_str() ) ) {
			// コンパイル失敗なのでそのまま終了。
			wprintf( L"cannot compile...\n" );
			return 0;
		}
	}
	wprintf( L"compile, ok.\n" );

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


	return 0;
}

