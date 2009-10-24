/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "util_base.h"

/**
 * MZ3非依存ユーティリティ
 */

/// MZ3 用ユーティリティ
namespace util
{

/**
 * ファイルを開く
 */
bool OpenByShellExecute( LPCTSTR target, LPCTSTR param )
{
	// ファイルを開く
	SHELLEXECUTEINFO sei;
	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = target;
	sei.lpParameters = param;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;

	return ShellExecuteEx( &sei ) == TRUE;
}

/**
 * 指定された URL を既定のブラウザで開く
 */
void OpenUrlByBrowser( LPCTSTR url, LPCTSTR param )
{
	OpenByShellExecute(url, param);
}

/**
 * ファイルの存在チェック
 *
 * ファイルが存在すれば true、存在しなければ false を返す
 */
bool ExistFile( LPCTSTR szFilepath )
{
	CFileStatus rStatus;

	if( CFile::GetStatus(szFilepath, rStatus) != FALSE ) {
		return true;
	}else{
		return false;
	}
}

/**
 * ファイルが存在すれば削除する
 */
bool RemoveWhenExist( LPCTSTR szFilepath )
{
	if( util::ExistFile( szFilepath ) ) {
		CFile::Remove( szFilepath );
	}else{
		return false;
	}
	return true;
}

/// int 型数値を文字列に変換する
CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int 型数値を文字列に変換する
CStringA int2str_a( int n )
{
	CStringA s;
	s.Format( "%d", n );
	return s;
}

/**
 * 文字列 szLeft と文字列 szRight で囲まれた部分文字列を取得し、szRight の次の文字のインデックスを返す。
 *
 * @return szRight の次の文字の位置を返す。文字列が見つからないときは -1 を返す。
 */
int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
{
	int pos1 = str.Find( szLeft );
	if( pos1 == -1 )
		return -1;

	// 部分文字列の開始位置=szLeft開始位置+szLeftの長さ
	int start = pos1+wcslen(szLeft);

	int pos2 = str.Find( szRight, start );
	if( pos2 == -1 ) 
		return -1;

	// 部分文字列の長さ
	int len = pos2-start;
	if( len <= 0 ) {
		return -1;
	}

	// 部分文字列の取得
	result = str.Mid( start, len );

	// szRight の次の文字の位置を返す
	return pos2+wcslen(szRight);
}

/**
 * 文字列 szKey（の次の文字）以降の部分文字列を取得し、szKey のインデックスを返す。
 *
 * @return szKey の位置を返す。文字列が見つからないときは -1 を返す。
 */
int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos1 = str.Find( szKey );
	if( pos1 == -1 )
		return -1;

	// 部分文字列の開始位置 = szKey開始位置 + szKeyの長さ
	int start = pos1+wcslen(szKey);

	// 部分文字列の取得
	result = str.Mid( start );

	// szKey の位置を返す
	return pos1;
}

/**
 * 文字列 szKey より前の部分文字列を取得し、szKey のインデックスを返す。
 *
 * @return szKey の位置を返す。文字列が見つからないときは -1 を返す。
 */
int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos = str.Find( szKey );
	if( pos == -1 )
		return -1;

	// 部分文字列の取得
	result = str.Left( pos );

	// szKey の位置を返す
	return pos;
}

/**
 * URL から GET パラメータを取得する。
 *
 * 取得できない場合は空文字列を返す。
 */
CString GetParamFromURL( const CString& url, LPCTSTR paramName )
{
	// ? 以降を抽出する
	int idxQuestion = url.Find( '?' );
	if (idxQuestion == -1) {
		return L"";
	}
	
	// "{paramName}=" を探索する
	CString pattern = paramName;
	pattern += L"=";
	int idxStart = url.Find( pattern, idxQuestion+1 );
	if (idxStart == -1) {
		return L"";
	}
	// '&' があれば、そこまでを抽出。なければ全てを抽出。
	int idxEnd = url.Find( '&', idxStart + pattern.GetLength() );
	if (idxEnd == -1) {
		// '&' なし
		return url.Mid( idxStart + pattern.GetLength() );
	} else {
		// '&' あり
		return url.Mid( idxStart + pattern.GetLength(), idxEnd - (idxStart + pattern.GetLength()) );
	}
}

/**
 * 数値を3桁区切り文字列に変換する
 */
CString int2comma_str(int n)
{
	if( n == 0 ) {
		return L"0";
	}

	CString s;

	int figure = 0;		// 桁数（1桁なら0）

	while( n > 0 ) {
		if( figure>0 && (figure % 3) == 0 ) {
			s = util::int2str( n % 10 ) + L"," + s;
		}else{
			s = util::int2str( n % 10 ) + s;
		}
		figure ++;
		n = n / 10;
	}

	return s;
}

/// UNICODE -> ANSI
std::string my_wcstombs( const std::wstring& wide_string )
{
	static std::vector<char> ansi_string(1024);
	memset( &ansi_string[0], 0x00, sizeof(char) * 1024 );
	wcstombs( &ansi_string[0], wide_string.c_str(), 1023 );
	return &ansi_string[0];
}

/// ANSI -> UNICODE
std::wstring my_mbstowcs( const std::string& ansi_string )
{
	std::vector<wchar_t> wide_string(1024);
	memset( &wide_string[0], 0x00, sizeof(wchar_t) * 1024 );
	mbstowcs( &wide_string[0], ansi_string.c_str(), 1023 );
	return &wide_string[0];
}

/// カンマ区切りで文字列リスト化する。
bool split_by_comma( std::vector<std::string>& values, const std::string& value )
{
	values.clear();

	size_t idxFrom = 0;
	while( idxFrom < value.length() ) {
		size_t at = value.find( ',', idxFrom );
		if( at == std::string::npos ) {
			// not found.
			// idxFrom 以降を追加して終了。
			values.push_back( value.substr(idxFrom) );
			return true;
		}

		// カンマ発見。
		// idxFrom からカンマの前まで（[idxFrom,at-1]）を追加。
		values.push_back( value.substr(idxFrom,at-idxFrom) );

		// 検索開始位置更新
		idxFrom = at+1;
	}

	return true;
}

/**
 * line に、
 * 指定された全ての文字列が順に存在すれば true を返す。
 * 大文字小文字は区別しない。
 */
bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, 
				LPCTSTR str2/*=NULL*/, 
				LPCTSTR str3/*=NULL*/, 
				LPCTSTR str4/*=NULL*/, 
				LPCTSTR str5/*=NULL*/ )
{
	// 大文字に変換した文字列を対象とする
	CString target( line );
	target.MakeUpper();

	// 検索文字列群
	LPCTSTR findStrings[] = { str1, str2, str3, str4, str5 };

	int idx = 0;
	for( int i=0; i<5; i++ ) {
		if( findStrings[i] != NULL ) {
			CString str = findStrings[i];
			str.MakeUpper();

			idx = target.Find( str, idx );
			if( idx < 0 ) {
				return false;
			}
			idx += str.GetLength();
		}
	}

	return true;
}

CString FormatString(LPCTSTR szFormat, ...)
{
	CString str;

	va_list args;
	va_start(args, szFormat);
	str.FormatV(szFormat, args);
	va_end(args);

	return str;
}

/**
 * クリップボードにテキストを設定 (UNICODE)
 *
 * @return エラー時 false、成功時 true を返す。
 */
bool SetClipboardDataTextW( const wchar_t* szText_ )
{
	HGLOBAL hMem = GlobalAlloc( GHND | GMEM_SHARE, (wcslen(szText_)+1)*sizeof(TCHAR) );
	wchar_t* pPtr = (wchar_t*)GlobalLock( hMem );
	wcscpy( pPtr, szText_ );
	GlobalUnlock( hMem );
	
	if( !OpenClipboard(NULL) )
		return false;
	EmptyClipboard();
	HANDLE hRet = SetClipboardData( CF_UNICODETEXT, hMem );
	CloseClipboard();
	
	if( hRet == NULL ) {
		GlobalFree( hMem );
		return false;
	}
	// GlobalFree は OS が行う
	return true;
}

/**
 * ダウンロード済みファイルをバッファに読み込む
 */
bool LoadDownloadedFile(std::vector<unsigned char>& buffer, LPCTSTR filename)
{
	FILE* fp = _wfopen(filename, _T("rb"));
	if (fp==NULL) {
		return false;
	}

	CHAR buf[4096];
	while (fgets(buf, 4096, fp) != NULL) {
		size_t old_size = buffer.size();
		int new_size = strlen(buf);
		if (new_size>0) {
			buffer.resize( old_size+new_size );
			strncpy( (char*)&buffer[old_size], buf, new_size );
		}
	}
	fclose(fp);

	return true;
}

}
