#pragma once

#include "wininet.h"
#include "constants.h"
#include "PostData.h"

// CInetAccess コマンド ターゲット

class CInetAccess
{
public:
	/**
	 * 受信ファイル種別
	 *
	 * この種別によって、受信後の処理が変化する。
	 */
	enum FILE_TYPE {
		FILE_HTML,		///< HTML ファイル。受信後に SJIS への漢字コード変換を実施する。
		FILE_BINARY		///< バイナリファイル。受信後の漢字コード変換を実施しない。
	};

	/**
	 * 通信処理の種別
	 */
	enum EXEC_SENDRECV_TYPE {
		EXEC_SENDRECV_TYPE_GET,		///< GET メソッド
		EXEC_SENDRECV_TYPE_POST,	///< POST メソッド
	};
private:
	TCHAR				m_lpszHostName[256];
	TCHAR				m_lpszUserName[256];
	TCHAR				m_lpszPassword[256];
	URL_COMPONENTS		m_sComponents;
	CString				m_strPath;				///< URL のパス部分（/以降、/を含む）

	CString				m_uri;					///< URI
	CString				m_ref;					///< リファラー

	HWND				m_hwnd;					///< 終了通知を送るウィンドウのハンドル



	void*				m_object;				///< 終了メッセージで LPARAM に渡すポインタ

	BOOL				m_abort;				///< 中断フラグ

	CPostData*			m_postData;				///< 送信データ
	CWinThread*			m_pThreadMain;			///< スレッドのポインタ

	HINTERNET			m_hConnection;
	HINTERNET			m_hRequest;
	HINTERNET			m_hInternet;			///< セッション(InternetOpenで取得)

	bool				m_bAccessing;			///< 通信中フラグ



	FILE_TYPE			m_fileType;				///< 受信ファイル種別

	CString				m_strErrorMsg;			///< エラー時のメッセージ。
												///< WM_MZ3_GET_ERROR 時にのみ有効。

	int					m_nRedirect;			///< リダイレクト回数

public:
	/**
	 * コンストラクタ
	 */
	CInetAccess()
		: m_hInternet(NULL)
	{
		Initialize( NULL, NULL );
	}

	virtual ~CInetAccess();

	bool Open();
	void CloseInternetHandles();

	LPCTSTR	GetURL()			{ return m_uri; }

	BOOL DoGet( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type );
	BOOL DoPost( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData );

	void Initialize( HWND hwnd, void* object );

	bool	IsConnecting()		{ return m_bAccessing; }		///< 通信中？
	LPCTSTR GetErrorMessage()	{ return m_strErrorMsg; }		///< エラーメッセージの取得
	int		GetRedirectCount()	{ return m_nRedirect; }			///< リダイレクト回数の取得

	/// 中断
	inline void Abort() 
	{ 
		m_abort = TRUE;
		::Sleep(100);

		CloseInternetHandles();

		// Abort 時は Internet ハンドルも閉じる
		if( m_hInternet != NULL ) {
			InternetCloseHandle( m_hInternet );
			m_hInternet = NULL;
		}
	};

private:
	static unsigned int __stdcall ExecGet_Thread(LPVOID);
	static unsigned int __stdcall ExecPost_Thread(LPVOID);

	void ParseURI();

	int ExecSendRecv( EXEC_SENDRECV_TYPE execType );

	HRESULT WINAPI SP_EstablishInetConnProc( CString& proxy );
};


