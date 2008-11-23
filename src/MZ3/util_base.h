/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * MZ3非依存ユーティリティ
 */

/// MZ3 用ユーティリティ
namespace util
{
/**
 * ファイルを開く
 */
inline bool OpenByShellExecute( LPCTSTR target )
{
	// ファイルを開く
	SHELLEXECUTEINFO sei;
	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = target;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;
	return ShellExecuteEx( &sei ) == TRUE;
}

/**
 * ファイルの存在チェック
 *
 * ファイルが存在すれば true、存在しなければ false を返す
 */
inline bool ExistFile( LPCTSTR szFilepath )
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
inline bool RemoveWhenExist( LPCTSTR szFilepath )
{
	if( util::ExistFile( szFilepath ) ) {
		CFile::Remove( szFilepath );
	}else{
		return false;
	}
	return true;
}

/**
 * 指定された URL を既定のブラウザで開く
 */
inline void OpenUrlByBrowser( LPCTSTR url )
{
	SHELLEXECUTEINFO sei;

	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = url;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;

	ShellExecuteEx(&sei);
}

/// int 型数値を文字列に変換する
inline CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int 型数値を文字列に変換する
inline CStringA int2str_a( int n )
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
inline int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
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
inline int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
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
inline int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
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
inline CString GetParamFromURL( const CString& url, LPCTSTR paramName )
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
inline CString int2comma_str(int n)
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
inline std::string my_wcstombs( const std::wstring& wide_string ) {
	static std::vector<char> ansi_string(1024);
	memset( &ansi_string[0], 0x00, sizeof(char) * 1024 );
	wcstombs( &ansi_string[0], wide_string.c_str(), 1023 );
	return &ansi_string[0];
}

/// ANSI -> UNICODE
inline std::wstring my_mbstowcs( const std::string& ansi_string ) {
	std::vector<wchar_t> wide_string(1024);
	memset( &wide_string[0], 0x00, sizeof(wchar_t) * 1024 );
	mbstowcs( &wide_string[0], ansi_string.c_str(), 1023 );
	return &wide_string[0];
}

/// カンマ区切りで文字列リスト化する。
inline bool split_by_comma( std::vector<std::string>& values, const std::string& value )
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
inline bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, LPCTSTR str2=NULL, LPCTSTR str3=NULL, LPCTSTR str4=NULL, LPCTSTR str5=NULL )
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

inline CString FormatString(LPCTSTR szFormat, ...)
{
	CString str;

	va_list args;
	va_start(args, szFormat);
	str.FormatV(szFormat, args);
	va_end(args);

	return str;
}

/**
 * ストップウォッチ
 *
 * 処理時間計測用
 */
class StopWatch
{
	DWORD dwStart;		///< 計測開始時刻
	DWORD dwStop;		///< 計測停止時刻
public:
	// コンストラクタ
	StopWatch() {
		start();
	}

	/// 停止、経過時間を取得する
	DWORD stop() {
		dwStop = ::GetTickCount();
		return getElapsedMilliSecUntilStoped();
	}

	/// 計測開始
	void start() {
		dwStop = dwStart = ::GetTickCount();
	}

	/// 停止時までの経過時間を msec 単位で取得する
	DWORD getElapsedMilliSecUntilStoped()
	{
		return dwStop - dwStart;
	}

	/// 現在の経過時間を msec 単位で取得する
	DWORD getElapsedMilliSecUntilNow()
	{
		return ::GetTickCount() - dwStart;
	}
};

/**
 * code from http://techtips.belution.com/ja/vc/0083/
 */
template< class T>
int FindFileCallback(const TCHAR* szDirectory,
                    const TCHAR* szFile,
                    int (*pFindCallback)(const TCHAR* szDirectory,
                                         const WIN32_FIND_DATA* Data,
                                         T    pCallbackProbe),
                    T    pCallbackProbe = NULL,
                    int  nDepth = -1)
{
    int            nResult = TRUE;
    HANDLE          hFile  = INVALID_HANDLE_VALUE;
    TCHAR          szPath[ MAX_PATH ];
    WIN32_FIND_DATA data;

    //_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    //  探索深度が尽きたら次のディレクトリは探さない．
    //
    if( nDepth == 0 || nResult == FALSE) return nResult;

    //_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    //  ディレクトリ潜航
    //
    _stprintf( szPath, _T("%s*.*"), szDirectory);
    hFile = FindFirstFile( szPath, &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
            // ディレクトリでないならやりなおし．
            if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                != FILE_ATTRIBUTE_DIRECTORY ) continue;

            // カレント及び親ディレクトリならやりなおし．
            if( _tcscmp(data.cFileName, _T(".")) == 0
                || _tcscmp(data.cFileName, _T("..")) == 0 ) continue;

            // 発見ディレクトリの潜航準備．
            _stprintf( szPath, _T("%s%s\\"), szDirectory, data.cFileName);

            //再帰呼び出し．ただし Depth -1 で渡す．ブクブク．
            nResult = FindFileCallback( szPath, szFile, pFindCallback, pCallbackProbe, nDepth -1);
        }
        while( FindNextFile( hFile, &data) && nResult);

        FindClose( hFile );
    }

    //_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    //  ファイル探索
    //
    _stprintf( szPath, _T("%s%s"), szDirectory, szFile);
    hFile = FindFirstFile( szPath, &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
            nResult = (pFindCallback)( szDirectory, &data, pCallbackProbe);
        }
        while( FindNextFile( hFile, &data) && nResult);

        FindClose( hFile );
    }

    return nResult;
}

/**
 * クリップボードにテキストを設定 (UNICODE)
 *
 * @return エラー時 false、成功時 true を返す。
 */
inline bool SetClipboardDataTextW( const wchar_t* szText_ )
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
inline bool LoadDownloadedFile(std::vector<unsigned char>& buffer, LPCTSTR filename)
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