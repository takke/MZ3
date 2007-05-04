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
 * ログ出力クラス
 */
class CSimpleLogger
{
public:
	/// カテゴリ定数
	enum CATEGORY {
		CATEGORY_FATAL = 0,		///< FATAL
		CATEGORY_ERROR = 1,		///< ERROR
		CATEGORY_INFO  = 2,		///< INFO
		CATEGORY_DEBUG = 3,		///< DEBUG
		CATEGORY_TRACE = 4		///< TRACE
	};

private:
	CString		m_strLogfilePath;	///< ログファイルのパス
	FILE*		m_fp;				///< FILE
	CATEGORY	m_level;			///< ログレベル

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
	 * 初期化
	 */
	bool init( LPCTSTR strLogfilePath ) {

		if( m_fp != NULL ) {
			SIMPLELOGGER_FATAL( *this, L"既に初期化済みです" );
			return false;
		}

		m_fp = _wfopen( strLogfilePath, L"a+t" );
		if( m_fp == NULL ) {
			return false;
		}

		return true;
	}

	/**
	 * ログレベルの設定
	 */
	bool setLogLevel( CATEGORY level ) {
		m_level = level;
		return true;
	}

	/**
	 * ログ出力終了
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
	 * FATAL メッセージ出力
	 */
	bool fatal( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		return outputMessage( CATEGORY_FATAL, message, strSourceFilename, line );
	}

	/**
	 * ERROR メッセージ出力
	 */
	bool error( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_ERROR ) 
			return false;
		return outputMessage( CATEGORY_ERROR, message, strSourceFilename, line );
	}

	/**
	 * INFO メッセージ出力
	 */
	bool info( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_INFO ) 
			return false;
		return outputMessage( CATEGORY_INFO, message, strSourceFilename, line );
	}

	/**
	 * DEBUG メッセージ出力
	 */
	bool debug( LPCTSTR message, LPCTSTR strSourceFilename=NULL, int line=-1 ) {
		if( m_level < CATEGORY_DEBUG ) 
			return false;
		return outputMessage( CATEGORY_DEBUG, message, strSourceFilename, line );
	}

	/**
	 * TRACE メッセージ出力
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

		// カテゴリ定数を文字列(5文字)に変換する。
		LPCTSTR categoryText = category2text( category );

		// 時刻出力
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
				// 行番号まで指定されているので、ソースファイル名と行番号を出力する
				fwprintf( m_fp, L"%5s %s [%s:%d]\n", categoryText, message, strSourceFilename, line );
			}else{
				// ソースファイル名のみ指定されているので、ソースファイル名を出力する
				fwprintf( m_fp, L"%5s %s [%s]\n", categoryText, message, strSourceFilename );
			}
		}else{
			// ソースファイル名が指定されていないので、メッセージを出力する
			fwprintf( m_fp, L"%5s %s\n", categoryText, message );
		}

		return true;
	}

	/**
	 * カテゴリ定数を文字列に変換する。文字列は常に 5文字。
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
