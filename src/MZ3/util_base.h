/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * MZ3��ˑ����[�e�B���e�B
 */

/// MZ3 �p���[�e�B���e�B
namespace util
{
/**
 * �t�@�C�����J��
 */
inline bool OpenByShellExecute( LPCTSTR target )
{
	// �t�@�C�����J��
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
 * �t�@�C���̑��݃`�F�b�N
 *
 * �t�@�C�������݂���� true�A���݂��Ȃ���� false ��Ԃ�
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
 * �t�@�C�������݂���΍폜����
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
 * �w�肳�ꂽ URL ������̃u���E�U�ŊJ��
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

/// int �^���l�𕶎���ɕϊ�����
inline CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int �^���l�𕶎���ɕϊ�����
inline CStringA int2str_a( int n )
{
	CStringA s;
	s.Format( "%d", n );
	return s;
}

/**
 * ������ szLeft �ƕ����� szRight �ň͂܂ꂽ������������擾���AszRight �̎��̕����̃C���f�b�N�X��Ԃ��B
 *
 * @return szRight �̎��̕����̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
{
	int pos1 = str.Find( szLeft );
	if( pos1 == -1 )
		return -1;

	// ����������̊J�n�ʒu=szLeft�J�n�ʒu+szLeft�̒���
	int start = pos1+wcslen(szLeft);

	int pos2 = str.Find( szRight, start );
	if( pos2 == -1 ) 
		return -1;

	// ����������̒���
	int len = pos2-start;
	if( len <= 0 ) {
		return -1;
	}

	// ����������̎擾
	result = str.Mid( start, len );

	// szRight �̎��̕����̈ʒu��Ԃ�
	return pos2+wcslen(szRight);
}

/**
 * ������ szKey�i�̎��̕����j�ȍ~�̕�����������擾���AszKey �̃C���f�b�N�X��Ԃ��B
 *
 * @return szKey �̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos1 = str.Find( szKey );
	if( pos1 == -1 )
		return -1;

	// ����������̊J�n�ʒu = szKey�J�n�ʒu + szKey�̒���
	int start = pos1+wcslen(szKey);

	// ����������̎擾
	result = str.Mid( start );

	// szKey �̈ʒu��Ԃ�
	return pos1;
}

/**
 * ������ szKey ���O�̕�����������擾���AszKey �̃C���f�b�N�X��Ԃ��B
 *
 * @return szKey �̈ʒu��Ԃ��B�����񂪌�����Ȃ��Ƃ��� -1 ��Ԃ��B
 */
inline int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
{
	int pos = str.Find( szKey );
	if( pos == -1 )
		return -1;

	// ����������̎擾
	result = str.Left( pos );

	// szKey �̈ʒu��Ԃ�
	return pos;
}

/**
 * URL ���� GET �p�����[�^���擾����B
 *
 * �擾�ł��Ȃ��ꍇ�͋󕶎����Ԃ��B
 */
inline CString GetParamFromURL( const CString& url, LPCTSTR paramName )
{
	// ? �ȍ~�𒊏o����
	int idxQuestion = url.Find( '?' );
	if (idxQuestion == -1) {
		return L"";
	}
	
	// "{paramName}=" ��T������
	CString pattern = paramName;
	pattern += L"=";
	int idxStart = url.Find( pattern, idxQuestion+1 );
	if (idxStart == -1) {
		return L"";
	}
	// '&' ������΁A�����܂ł𒊏o�B�Ȃ���ΑS�Ă𒊏o�B
	int idxEnd = url.Find( '&', idxStart + pattern.GetLength() );
	if (idxEnd == -1) {
		// '&' �Ȃ�
		return url.Mid( idxStart + pattern.GetLength() );
	} else {
		// '&' ����
		return url.Mid( idxStart + pattern.GetLength(), idxEnd - (idxStart + pattern.GetLength()) );
	}
}

/**
 * ���l��3����؂蕶����ɕϊ�����
 */
inline CString int2comma_str(int n)
{
	if( n == 0 ) {
		return L"0";
	}

	CString s;

	int figure = 0;		// �����i1���Ȃ�0�j

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

/// �J���}��؂�ŕ����񃊃X�g������B
inline bool split_by_comma( std::vector<std::string>& values, const std::string& value )
{
	values.clear();

	size_t idxFrom = 0;
	while( idxFrom < value.length() ) {
		size_t at = value.find( ',', idxFrom );
		if( at == std::string::npos ) {
			// not found.
			// idxFrom �ȍ~��ǉ����ďI���B
			values.push_back( value.substr(idxFrom) );
			return true;
		}

		// �J���}�����B
		// idxFrom ����J���}�̑O�܂Łi[idxFrom,at-1]�j��ǉ��B
		values.push_back( value.substr(idxFrom,at-idxFrom) );

		// �����J�n�ʒu�X�V
		idxFrom = at+1;
	}

	return true;
}

/**
 * line �ɁA
 * �w�肳�ꂽ�S�Ă̕����񂪏��ɑ��݂���� true ��Ԃ��B
 * �啶���������͋�ʂ��Ȃ��B
 */
inline bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, LPCTSTR str2=NULL, LPCTSTR str3=NULL, LPCTSTR str4=NULL, LPCTSTR str5=NULL )
{
	// �啶���ɕϊ������������ΏۂƂ���
	CString target( line );
	target.MakeUpper();

	// ����������Q
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
 * �X�g�b�v�E�H�b�`
 *
 * �������Ԍv���p
 */
class StopWatch
{
	DWORD dwStart;		///< �v���J�n����
	DWORD dwStop;		///< �v����~����
public:
	// �R���X�g���N�^
	StopWatch() {
		start();
	}

	/// ��~�A�o�ߎ��Ԃ��擾����
	DWORD stop() {
		dwStop = ::GetTickCount();
		return getElapsedMilliSecUntilStoped();
	}

	/// �v���J�n
	void start() {
		dwStop = dwStart = ::GetTickCount();
	}

	/// ��~���܂ł̌o�ߎ��Ԃ� msec �P�ʂŎ擾����
	DWORD getElapsedMilliSecUntilStoped()
	{
		return dwStop - dwStart;
	}

	/// ���݂̌o�ߎ��Ԃ� msec �P�ʂŎ擾����
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
    //  �T���[�x���s�����玟�̃f�B���N�g���͒T���Ȃ��D
    //
    if( nDepth == 0 || nResult == FALSE) return nResult;

    //_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    //  �f�B���N�g�����q
    //
    _stprintf( szPath, _T("%s*.*"), szDirectory);
    hFile = FindFirstFile( szPath, &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
            // �f�B���N�g���łȂ��Ȃ���Ȃ����D
            if( (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                != FILE_ATTRIBUTE_DIRECTORY ) continue;

            // �J�����g�y�ѐe�f�B���N�g���Ȃ���Ȃ����D
            if( _tcscmp(data.cFileName, _T(".")) == 0
                || _tcscmp(data.cFileName, _T("..")) == 0 ) continue;

            // �����f�B���N�g���̐��q�����D
            _stprintf( szPath, _T("%s%s\\"), szDirectory, data.cFileName);

            //�ċA�Ăяo���D������ Depth -1 �œn���D�u�N�u�N�D
            nResult = FindFileCallback( szPath, szFile, pFindCallback, pCallbackProbe, nDepth -1);
        }
        while( FindNextFile( hFile, &data) && nResult);

        FindClose( hFile );
    }

    //_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    //  �t�@�C���T��
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
 * �N���b�v�{�[�h�Ƀe�L�X�g��ݒ� (UNICODE)
 *
 * @return �G���[�� false�A������ true ��Ԃ��B
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
	// GlobalFree �� OS ���s��
	return true;
}

/**
 * �_�E�����[�h�ς݃t�@�C�����o�b�t�@�ɓǂݍ���
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