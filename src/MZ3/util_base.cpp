/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "util_base.h"

/**
 * MZ3��ˑ����[�e�B���e�B
 */

/// MZ3 �p���[�e�B���e�B
namespace util
{

/**
 * �t�@�C�����J��
 */
bool OpenByShellExecute( LPCTSTR target, LPCTSTR param )
{
	// �t�@�C�����J��
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
 * �w�肳�ꂽ URL ������̃u���E�U�ŊJ��
 */
void OpenUrlByBrowser( LPCTSTR url, LPCTSTR param )
{
	OpenByShellExecute(url, param);
}

/**
 * �t�@�C���̑��݃`�F�b�N
 *
 * �t�@�C�������݂���� true�A���݂��Ȃ���� false ��Ԃ�
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
 * �t�@�C�������݂���΍폜����
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

/// int �^���l�𕶎���ɕϊ�����
CStringW int2str( int n )
{
	CString s;
	s.Format( L"%d", n );
	return s;
}

/// int �^���l�𕶎���ɕϊ�����
CStringA int2str_a( int n )
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
int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result )
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
int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result )
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
int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result )
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
CString GetParamFromURL( const CString& url, LPCTSTR paramName )
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
CString int2comma_str(int n)
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

/// �J���}��؂�ŕ����񃊃X�g������B
bool split_by_comma( std::vector<std::string>& values, const std::string& value )
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
bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, 
				LPCTSTR str2/*=NULL*/, 
				LPCTSTR str3/*=NULL*/, 
				LPCTSTR str4/*=NULL*/, 
				LPCTSTR str5/*=NULL*/ )
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
 * �N���b�v�{�[�h�Ƀe�L�X�g��ݒ� (UNICODE)
 *
 * @return �G���[�� false�A������ true ��Ԃ��B
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
	// GlobalFree �� OS ���s��
	return true;
}

/**
 * �_�E�����[�h�ς݃t�@�C�����o�b�t�@�ɓǂݍ���
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
