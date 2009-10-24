/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include <stdio.h>
#include <time.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define SIMPLELOGGER_FATAL(target,msg)	(target).fatal( msg, __WFILE__, __LINE__ ); (target).flush()
#define SIMPLELOGGER_ERROR(target,msg)	(target).error( msg, __WFILE__, __LINE__ ); (target).flush()
#define SIMPLELOGGER_INFO(target,msg)	(target).info ( msg, __WFILE__, __LINE__ )
#define SIMPLELOGGER_DEBUG(target,msg)	(target).debug( msg, __WFILE__, __LINE__ )
#define SIMPLELOGGER_TRACE(target,msg)	(target).trace( msg, __WFILE__, __LINE__ )

/**
 * ���O�o�̓N���X
 */
class CSimpleLogger
{
public:
	/// �J�e�S���萔
	enum CATEGORY {
		CATEGORY_FATAL = 0,		///< FATAL
		CATEGORY_ERROR = 1,		///< ERROR
		CATEGORY_INFO  = 2,		///< INFO
		CATEGORY_DEBUG = 3,		///< DEBUG
		CATEGORY_TRACE = 4		///< TRACE
	};

private:
	CString		m_strLogfilePath;	///< ���O�t�@�C���̃p�X
	FILE*		m_fp;				///< FILE
	CATEGORY	m_level;			///< ���O���x��

public:
	CSimpleLogger(void) 
		: m_fp(NULL)
		, m_level(CATEGORY_DEBUG)
	{
	}

	~CSimpleLogger(void) {
		finish();
	}

	/**
	 * ������
	 */
	bool init( LPCTSTR strLogfilePath ) {

		if( m_fp != NULL ) {
			SIMPLELOGGER_FATAL( *this, L"���ɏ������ς݂ł�" );
			return false;
		}

		// ���[�e�[�g
		rotate( strLogfilePath );

		// �I�[�v��
		m_fp = _wfopen( strLogfilePath, L"a+t" );
		if( m_fp == NULL ) {
			return false;
		}

		return true;
	}

	/**
	 * �����̃��[�e�[�g���s��
	 */
	void rotate( LPCTSTR strLogfilePath ) {
		// �t�@�C���̍쐬���t���V�X�e�����t�ƈقȂ�΍폜����

		// �t�@�C���̏����擾����
		HANDLE hFile = CreateFile( strLogfilePath, 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			NULL, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == NULL ) {
			// file not found.
			return;
		}

		FILETIME ft;
		if( GetFileTime( hFile, &ft, NULL, NULL ) == FALSE ) {
			return;
		}
		CloseHandle( hFile );

		// �����ϊ�
		SYSTEMTIME stFile, stSystem;
		if( FileTimeToSystemTime( &ft, &stFile ) == FALSE ) {
			return;
		}

		// �V�X�e�������擾
		GetLocalTime( &stSystem );

		// ��r
		if( stFile.wYear  != stSystem.wYear  ||
			stFile.wMonth != stSystem.wMonth ||
			stFile.wDay   != stSystem.wDay   )
		{
			// ���t���قȂ�̂ō폜
			DeleteFile( strLogfilePath );
		}		
	}

	/**
	 * ���O���x���̐ݒ�
	 */
	bool setLogLevel( CATEGORY level ) {
		m_level = level;
		return true;
	}

	/**
	 * ���O���x���̎擾
	 */
	CATEGORY getLogLevel() {
		return m_level;
	}

	/**
	 * ���O�o�͏I��
	 */
	bool finish() {
		if( m_fp == NULL ) {
			return false;
		}
		fclose( m_fp );
		m_fp = NULL;
		return true;
	}

	/**
	 * flush
	 */
	bool flush() {
		if( m_fp == NULL ) {
			return false;
		}
		fflush( m_fp );
		return true;
	}

	/**
	 * FATAL ���b�Z�[�W�o��
	 */
	bool fatal( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		return outputMessage( CATEGORY_FATAL, message, strSourceFilename, line );
	}

	/**
	 * ERROR ���b�Z�[�W�o��
	 */
	bool error( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_ERROR ) 
			return false;
		return outputMessage( CATEGORY_ERROR, message, strSourceFilename, line );
	}

	/**
	 * INFO ���b�Z�[�W�o��
	 */
	bool info( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_INFO ) 
			return false;
		return outputMessage( CATEGORY_INFO, message, strSourceFilename, line );
	}

	/**
	 * DEBUG ���b�Z�[�W�o��
	 */
	bool debug( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_DEBUG ) 
			return false;
		return outputMessage( CATEGORY_DEBUG, message, strSourceFilename, line );
	}

	/**
	 * TRACE ���b�Z�[�W�o��
	 */
	bool trace( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_TRACE ) 
			return false;
		return outputMessage( CATEGORY_TRACE, message, strSourceFilename, line );
	}

	/**
	 * DEBUG �L���H
	 */
	bool isDebugEnabled() {
		return m_level >= CATEGORY_DEBUG;
	}

private:

	/**
	 * ���b�Z�[�W�o��
	 */
	bool outputMessage( CATEGORY category, LPCTSTR message, LPCTSTR strSourceFilename, int line ) {
		bool rval = outputMessage( m_fp, category, message, strSourceFilename, line );
#if defined(DEBUG) || defined(CONSOLE_DEBUG)
		rval = outputMessage( stdout, category, message, strSourceFilename, line );
#endif
		return rval;
	}

	static bool outputMessage( FILE* fp, CATEGORY category, LPCTSTR message, LPCTSTR strSourceFilename, int line ) {

		if( fp == NULL ) {
			return false;
		}

		// �J�e�S���萔�𕶎���(5����)�ɕϊ�����B
		LPCTSTR categoryText = category2text( category );

		// �����o��
		__time64_t now = _time64(NULL);
		struct tm newtime;
		_localtime64_s( &newtime, &now );
		fwprintf( fp, L"[%04d/%02d/%02d %02d:%02d:%02d] ",
			newtime.tm_year + 1900,
			newtime.tm_mon + 1,
			newtime.tm_mday,
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec );

		if( strSourceFilename != NULL ) {
			if( line >= 0 ) {
				// �s�ԍ��܂Ŏw�肳��Ă���̂ŁA�\�[�X�t�@�C�����ƍs�ԍ����o�͂���
				fwprintf( fp, L"%5s %s [%s:%d]\n", categoryText, message, strSourceFilename, line );
			}else{
				// �\�[�X�t�@�C�����̂ݎw�肳��Ă���̂ŁA�\�[�X�t�@�C�������o�͂���
				fwprintf( fp, L"%5s %s [%s]\n", categoryText, message, strSourceFilename );
			}
		}else{
			// �\�[�X�t�@�C�������w�肳��Ă��Ȃ��̂ŁA���b�Z�[�W���o�͂���
			fwprintf( fp, L"%5s %s\n", categoryText, message );
		}

		return true;
	}

	/**
	 * �J�e�S���萔�𕶎���ɕϊ�����B������͏�� 5�����B
	 */
	static LPCTSTR category2text( CATEGORY category ) {
		switch( category ) {
		case CATEGORY_FATAL:	return L"FATAL";
		case CATEGORY_ERROR:	return L"ERROR";
		case CATEGORY_INFO:		return L"INFO ";
		case CATEGORY_DEBUG:	return L"DEBUG";
		case CATEGORY_TRACE:	return L"TRACE";
		default:				return L"?????";
		}
	}
};
