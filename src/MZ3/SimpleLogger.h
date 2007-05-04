#pragma once

#include <stdio.h>
#include <time.h>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define SIMPLELOGGER_FATAL(target,msg)	(target).fatal( msg, __WFILE__, __LINE__ )
#define SIMPLELOGGER_ERROR(target,msg)	(target).error( msg, __WFILE__, __LINE__ )
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

		m_fp = _wfopen( strLogfilePath, L"a+t" );
		if( m_fp == NULL ) {
			return false;
		}

		return true;
	}

	/**
	 * ���O���x���̐ݒ�
	 */
	bool setLogLevel( CATEGORY level ) {
		m_level = level;
		return true;
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

private:

	bool outputMessage( CATEGORY category, LPCTSTR message, LPCTSTR strSourceFilename, int line ) {

		if( m_fp == NULL ) {
			return false;
		}

		// �J�e�S���萔�𕶎���(5����)�ɕϊ�����B
		LPCTSTR categoryText = category2text( category );

		// �����o��
		__time64_t now = _time64(NULL);
		struct tm newtime;
		_localtime64_s( &newtime, &now );
		fwprintf( m_fp, L"[%04d/%02d/%02d %02d:%02d:%02d] ",
			newtime.tm_year + 1900,
			newtime.tm_mon + 1,
			newtime.tm_mday,
			newtime.tm_hour,
			newtime.tm_min,
			newtime.tm_sec );

		if( strSourceFilename != NULL ) {
			if( line >= 0 ) {
				// �s�ԍ��܂Ŏw�肳��Ă���̂ŁA�\�[�X�t�@�C�����ƍs�ԍ����o�͂���
				fwprintf( m_fp, L"%5s %s [%s:%d]\n", categoryText, message, strSourceFilename, line );
			}else{
				// �\�[�X�t�@�C�����̂ݎw�肳��Ă���̂ŁA�\�[�X�t�@�C�������o�͂���
				fwprintf( m_fp, L"%5s %s [%s]\n", categoryText, message, strSourceFilename );
			}
		}else{
			// �\�[�X�t�@�C�������w�肳��Ă��Ȃ��̂ŁA���b�Z�[�W���o�͂���
			fwprintf( m_fp, L"%5s %s\n", categoryText, message );
		}

		return true;
	}

	/**
	 * �J�e�S���萔�𕶎���ɕϊ�����B������͏�� 5�����B
	 */
	LPCTSTR category2text( CATEGORY category ) {
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
