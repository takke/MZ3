// RegexTest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "../MZ3/MyRegex.h"

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "Japanese" ); 

	LPCTSTR target_list[] = {
		L"<span class=\"date\">2007�N7��05��&nbsp;1:55</span></dt>",
		L"<span class=\"date\">7��05��&nbsp;1:55</span></dt>",
		L"<td>10��08��</td></tr>",
		L"<td WIDTH=\"1%\" nowrap CLASS=\"f08\">10��14�� 16:47</td></tr>",
		L"10��14�� 16:47",
		L"<dd>2007�N10��07��01:27</dd>",
		L"<font COLOR=#996600>���@�t</font>&nbsp;:&nbsp;2007�N10��08�� 21��52��&nbsp;&nbsp;",
		NULL
	};

	std::wstring pattern = L"";
	pattern = L"([0-9]{2,4})?�N?([0-9]{1,2}?)��([0-9]{1,2})��[^0-9]*([0-9]{1,2})?:?��?([0-9]{2})?";
//	pattern = L"<td.+?>(.+?)</";

	for (int iTarget=0; target_list[iTarget]!=NULL; iTarget++ ) {
		wprintf( L"---start---\n" );
		std::wstring target = target_list[iTarget];

		wprintf( L"target [%s]\n", target.c_str() );
		wprintf( L"pattern[%s]\n", pattern.c_str() );

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( pattern.c_str() ) ) {
				// �R���p�C�����s�Ȃ̂ł��̂܂܏I���B
				wprintf( L"cannot compile...\n" );
				return 0;
			}
		}
		wprintf( L"compile, ok.\n" );

		// �T��
		if( reg.exec(target.c_str()) == false ) {
			// �������B
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

		// �u���e�X�g
	/*	LPCTSTR replace = L"(({1}))";
		reg.replaceAll( target, replace );
		wprintf( L" replace         [%s]\n", replace );
		wprintf( L" replaced target [%s]\n", target.c_str() );
	*/
	}
	wprintf( L"---end---\n" );

	return 0;
}

