/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include <string>
#include <vector>

/**
 * MZ3��ˑ����[�e�B���e�B
 */

/// MZ3 �p���[�e�B���e�B
namespace util
{

bool OpenByShellExecute( LPCTSTR target, LPCTSTR param=NULL );
void OpenUrlByBrowser( LPCTSTR url, LPCTSTR param=NULL );
bool ExistFile( LPCTSTR szFilepath );
bool RemoveWhenExist( LPCTSTR szFilepath );
CStringW int2str( int n );
CStringA int2str_a( int n );
int GetBetweenSubString( const CString& str, LPCTSTR szLeft, LPCTSTR szRight, CString& result );
int GetAfterSubString( const CString& str, LPCTSTR szKey, CString& result );
int GetBeforeSubString( const CString& str, LPCTSTR szKey, CString& result );
CString GetParamFromURL( const CString& url, LPCTSTR paramName );
CString int2comma_str(int n);
std::string my_wcstombs( const std::wstring& wide_string );
std::wstring my_mbstowcs( const std::string& ansi_string );
bool split_by_comma( std::vector<std::string>& values, const std::string& value );
bool LineHasStringsNoCase( 
				const CString& line, 
				LPCTSTR str1, LPCTSTR str2=NULL, LPCTSTR str3=NULL, LPCTSTR str4=NULL, LPCTSTR str5=NULL );
CString FormatString(LPCTSTR szFormat, ...);
bool SetClipboardDataTextW( const wchar_t* szText_ );
bool LoadDownloadedFile(std::vector<unsigned char>& buffer, LPCTSTR filename);

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


}
