// InetAccess.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "InetAccess.h"
#include "HtmlArray.h"

#include "MyRegex.h"
#include "wininet.h"
#include "kfm.h"
#include "util.h"

#pragma comment(lib, "wininet.lib")
#pragma comment( lib, "cellcore.lib" )

#include <initguid.h>
#include <connmgr.h>
#include <connmgr_proxy.h>
#define ARRAYSIZE( a ) (sizeof( a )/sizeof(a[0]) )

#define SZ_REG_CONNECTION_ROOT TEXT("System\\State\\Connections")
#define SZ_REG_CONNECTION_DBVOL TEXT("Count") 

// CInetAccess

/**
 * 画面下部の情報領域に通信量に関するメッセージを表示する
 *
 * @param dwLoaded      読み込み済みデータ量
 * @param dwContentLen  総データ量。負のときは参考値とみなす。0 のときは総データ量不明とみなす。
 * @param dwElapsedMsec 経過時間[msec]
 */
inline void MySetTrafficInfo( HWND hWnd, LONG dwLoaded, LONG dwContentLen, DWORD dwElapsedMsec )
{
	// メッセージ生成
	CString msg;
	DWORD dwLength = 0;
	if( dwContentLen > 0 ) {
		// 進捗率計算
		dwLength = dwContentLen;

		double rate = dwLoaded/(double)dwLength;
		rate = min(rate,1.0);	// [0,1] に正規化
		msg.Format( _T("(%.0f%%) %d.%d/%d.%dKB"), 
			rate*100.0, 
			dwLoaded / 1024,
			dwLoaded % 1024 / 103,
			dwLength / 1024,
			dwLength % 1024 / 103 );

	}else if( dwContentLen < 0 ) {
		// 前回受信時の値（参考値）で進捗率算出
		dwLength = -dwContentLen;

		double rate = dwLoaded/(double)dwLength;
		rate = min(rate,1.0);	// [0,1] に正規化
		msg.Format( _T("(%.0f%%) %d.%d/%d.%d+KB"), 
			rate*100.0, 
			dwLoaded / 1024,
			dwLoaded % 1024 / 103,
			dwLength / 1024,
			dwLength % 1024 / 103 );
	}else{
		// 受信サイズ、同参考値もないため、受信済みサイズのみ表示
		msg.Format( _T("%d.%dKB"), dwLoaded / 1024, dwLoaded % 1024 / 103 );

		dwLength = 0;
	}

	// 末尾に予想完了時間を表示する
	// - dwLength が 0 の場合はサイズ不明のため表示しない。
	// - dwLoaded が 0 の場合は 0 除算回避のため表示しない。
	// - 経過時間が N [msec] 未満の場合は、情報が信頼できないため表示しない。
	if( dwLength != 0 && dwLoaded != 0 && dwElapsedMsec > 200 ) {
		// bps = loaded / sec
		// rest = len - loaded
		// sec = msec / 1000
		// 残り時間[sec] = rest / bps
		//               = rest / (loaded/sec)
		//               = rest * sec / loaded
		DWORD rest     = dwLength - dwLoaded;
		if( rest < 0 ) {
			// 既に予想転送量を超えている。
			// 残り 0 とみなす。
			rest = 0;
		}
		// 心理的に 0 [sec] =終了と思えるため、「小数点以下切り上げ」しておく。
		DWORD restSec  = (DWORD)ceil( rest * (double)dwElapsedMsec / 1000 / dwLoaded );

		// 最大で 3600 sec までとする
		if( restSec > 3600 ) restSec = 3600;
		if( restSec < 0    ) restSec = 0;
		msg.AppendFormat( L" [%dsec]", restSec );
	}

	// メッセージ設定
	util::MySetInformationText( hWnd, msg );

	// プログレスバー設定も行う
	::SendMessage( hWnd, WM_MZ3_ACCESS_LOADED, (WPARAM)dwLoaded, (LPARAM)dwLength );
}

/**
 * デストラクタ
 */
CInetAccess::~CInetAccess()
{
	// m_hRequest, m_hConnection を閉じる。
	CloseInternetHandles();
}


// CInetAccess メンバ関数

/**
 * インターネットの接続を開く
 * 
 * @note ログインする前にかならず行うこと
 */
bool CInetAccess::Open()
{
	// プロキシ文字列
	CString proxy = L"";

	if( theApp.m_optionMng.IsUseAutoConnection() ) {
		// インターネット自動接続。
		// 接続が無かったらダイアルアップ処理を行う。
		// また、グローバルプロキシの接続先を取得する。
		SP_EstablishInetConnProc( proxy );
	}

	if (theApp.m_optionMng.IsUseProxy() ) {
		// プロキシ文字列を生成する

		if( theApp.m_optionMng.IsUseGlobalProxy() ) {
			// 「端末の設定を使用する」オプションが有効。
			// SP_EstablishInetConnProc にて取得済みであるため、ここでは何もしない。
		}else{
			// 「手動設定」オプションが有効。
			// ユーザ指定のサーバとポート番号からプロキシ文字列を生成する
			proxy.Format(_T("%s:%d"),
				theApp.m_optionMng.GetProxyServer(),
				theApp.m_optionMng.GetProxyPort());
		}
	}

	try {
		// 既に開かれていれば閉じる
		CloseInternetHandles();

		// 接続を開く
		m_hInternet = InternetOpen(_T("MZ3"), INTERNET_OPEN_TYPE_PROXY, proxy, NULL, 0);
	} catch (CException &) {
		m_hInternet = NULL;

		// エラー発生
		m_strErrorMsg = L"エラー発生";
		MessageBox( NULL, m_strErrorMsg, 0, MB_OK );
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return false;
	}
	if( m_hInternet == NULL ) {
		m_strErrorMsg.Format( L"エラー発生 (%d)", GetLastError() );
		MessageBox( NULL, m_strErrorMsg, 0, MB_OK );
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return false;
	}

/*
	util::MySetInformationText( m_hwnd, _T("初期処理完了") );
*/
	return true;
}

/**
 * 通信の初期化処理
 */
void CInetAccess::Initialize( HWND hwnd, void* object )
{
	m_bAccessing	= false;
	m_hConnection	= NULL;
	m_hRequest		= NULL;

	m_hwnd			= hwnd;
	m_object		= object;
	m_nRedirect		= 0;
}

/**
 * インターネットアクセス
 * 
 * 指定のURIにアクセスして、ファイルを取得し、SJISに変換してファイルに保存する。
 * ファイルは全て theApp.m_filepath.temphtml に保存する。
 * 
 * @param uri	[in] URI
 * @param ref	[in] リファラ
 *
 * @return 常に TRUE を返す。
 */
BOOL CInetAccess::DoGet( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type )
{
	// 中断フラグを初期化
	m_abort		= FALSE;

	m_uri		= uri;
	m_ref		= ref;
	m_fileType	= type;
	m_nRedirect = 0;

	// スレッド開始
	m_pThreadMain = AfxBeginThread(ExecGet_Thread, this);

	return TRUE;
}

/**
 * スレッド中継
 */
unsigned int CInetAccess::ExecGet_Thread(LPVOID This)
{
	// GET 処理
	CInetAccess* inet = (CInetAccess*)This;
	inet->m_bAccessing = true;
	int msg = inet->ExecSendRecv( EXEC_SENDRECV_TYPE_GET );
	::PostMessage( inet->m_hwnd, msg, NULL, (LPARAM)inet->m_object );
	inet->m_bAccessing = false;

	return 0;
}
/**
 * データをポストする
 */
BOOL CInetAccess::DoPost( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData )
{
	// 中断フラグを初期化
	m_abort		= FALSE;

	m_uri		= uri;
	m_ref		= ref;
	m_fileType	= type;
	m_postData	= postData;
	m_nRedirect = 0;

	// スレッド開始
	m_pThreadMain = AfxBeginThread(ExecPost_Thread, this);

	return TRUE;
}

/**
 * スレッド中継
 */
unsigned int CInetAccess::ExecPost_Thread(LPVOID This)
{
	// POST 処理
	CInetAccess* inet = (CInetAccess*)This;
	inet->m_bAccessing = true;
	int msg = inet->ExecSendRecv( EXEC_SENDRECV_TYPE_POST );
	::PostMessage( inet->m_hwnd, msg, NULL, (LPARAM)inet->m_object );
	inet->m_bAccessing = false;

	return 0;
}

/**
 * out_buf の末尾に pData を dwSize だけ追加する
 */
inline bool my_append_buf( kfm::kf_buf_type& out_buf, char* pData, DWORD dwSize )
{
	if( dwSize==0 ) {
		return true;
	}

	// 拡張前のサイズ
	size_t base_size = out_buf.size();

	// 拡張
	out_buf.resize( base_size+dwSize );

	// 格納
	memcpy( &out_buf[base_size], pData, dwSize );

	return true;
}

/**
 * 自動接続。
 *
 * 接続がされていなかったらダイアルアップ処理を行う。
 * また、コネクションマネージャからプロキシ情報を取得する。
 */
HRESULT WINAPI CInetAccess::SP_EstablishInetConnProc( CString& proxy )
{
	CONNMGR_CONNECTIONINFO ci = {0};
	PROXY_CONFIG pcProxy = {0};
	DWORD dwStatus = 0;
	DWORD dwIndex = 0;
	HRESULT hr = S_OK;
	HANDLE hConnection = NULL;
	DWORD dwTimeOut = 120000;	//タイムアウト値

	// コネクションマネージャの初期化
	ci.cbSize     = sizeof(CONNMGR_CONNECTIONINFO);
	ci.dwParams   = CONNMGR_PARAM_GUIDDESTNET;
	ci.dwFlags    = CONNMGR_FLAG_PROXY_HTTP;
	ci.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;

	// URLチェック
	hr = ConnMgrMapURL(L"http://mixi.jp", &(ci.guidDestNet), &dwIndex);

	// 接続処理
	hr = ConnMgrEstablishConnectionSync(&ci, &hConnection, dwTimeOut, &dwStatus);

	// コネクションマネージャからプロクシ情報を取得する
	hr = ConnMgrProviderMessage( hConnection, &IID_ConnPrv_IProxyExtension, NULL, 0, 0, (PBYTE)&pcProxy, sizeof(pcProxy)); 
	if (S_OK == hr)
	{
		// 取得OK。
		// proxy にプロキシ情報を設定する
		proxy = pcProxy.szProxyServer;
	}
	else if (E_NOINTERFACE == hr)
	{
		// プロクシ情報がない場合はダイレクト接続
		proxy = L"";
		hr = S_OK;
	}

	if (hConnection)
	{
		ConnMgrReleaseConnection(hConnection, TRUE);
	}

	return hr;
}

/**
 * ネットワークの接続状態を確認する。
 *
 * @return 接続：true、切断：false
 */
bool CInetAccess::IsNetworkConnected()
{
	// 切断されていればクローズ後、再オープン
	HKEY  hKey = NULL;
	DWORD dwConnect=0;		// Connectionのステータス
	DWORD dwType;			// 値の種類を受け取る
	DWORD dwSize;			// データのサイズを受け取る

	// レジストリをオープン
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,SZ_REG_CONNECTION_ROOT,0,KEY_READ,&hKey) == 0) 
	{ 
		RegQueryValueEx(hKey,SZ_REG_CONNECTION_DBVOL,NULL,&dwType,NULL,&dwSize);
		RegQueryValueEx(hKey,SZ_REG_CONNECTION_DBVOL,NULL,&dwType,(LPBYTE)&dwConnect,&dwSize);

		// レジストリのクローズ
		RegCloseKey(hKey);

		// dwConnect>0 であれば接続状態
		return (dwConnect > 0) ? true : false;
	}else{
		// レジストリエントリがなければ切断状態と判断する
		return false;
	}
}

/**
 * 送受信処理を行う。
 *
 * GET メソッドまたは POST メソッドで通信を行う。
 *
 * @return ウィンドウメッセージ。WM_MZ3_GET_ERROR 時には、エラーメッセージも設定される。
 */
int CInetAccess::ExecSendRecv( EXEC_SENDRECV_TYPE execType )
{
	// URL 分解
	ParseURI();

	// ----------------------------------------
	// 接続開始
	// ----------------------------------------
	if (m_hInternet == NULL) {
		// 接続処理を行う
		if( !Open() ) {
			m_strErrorMsg = L"接続エラー(InternetOpen,1)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
	}else{
		// 自動接続がONの場合、接続状態をチェックする。
		// 何らかの理由で切断されている場合、再接続を行う。
		if( theApp.m_optionMng.IsUseAutoConnection() ) {
			// 切断されていればクローズ後、再オープン
			if( !IsNetworkConnected() ) {
				// クローズ
				CloseInternetHandles();
				// オープン
				if( !Open() ) {
					m_strErrorMsg = L"接続エラー(InternetOpen,2)";
					MZ3LOGGER_ERROR( m_strErrorMsg );
					return WM_MZ3_GET_ERROR;
				}
			}
		}
	}

	try {
		// 接続
		m_hConnection = ::InternetConnect(
			m_hInternet,				// セッション
			m_lpszHostName,				// ホスト名
			m_sComponents.nPort,		// ポート番号
			NULL,						// ユーザ
			NULL,						// パスワード
			INTERNET_SERVICE_HTTP,
			0,							// 同期
			NULL);						// コンテキストなし

		// --------------------------------------------------
		// 中断確認
		// --------------------------------------------------
		if (m_abort != FALSE) {
			// m_hRequest, m_hConnection を閉じる。
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}

	} catch (CException &) {
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"例外発生(InternetConnect)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}else{
			return WM_MZ3_GET_ABORT;
		}
	}

	if( m_hConnection==NULL ) {
		// 接続ＮＧ
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		m_strErrorMsg = L"コネクションなし";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}

	util::MySetInformationText( m_hwnd, _T("サイトに接続しました") );

	// タイムアウト値を30秒に設定
	int timeout = 30000;
	::InternetSetOption(m_hConnection, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)timeout, sizeof(timeout));

	// プロクシ設定がONの場合、IDとパスワードをセット
	if (theApp.m_optionMng.IsUseProxy() ) {

		LPTSTR proxy = _T("INTERNET_OPTION_PROXY");
		LPTSTR user  = const_cast<LPTSTR>(theApp.m_optionMng.GetProxyUser());
		LPTSTR pass  = const_cast<LPTSTR>(theApp.m_optionMng.GetProxyPassword());

		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY, proxy, sizeof(proxy));
		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY_USERNAME, user, sizeof(user));
		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY_PASSWORD, pass, sizeof(pass));
	}

	// リクエスト送信
	try {
		DWORD dwFlags = INTERNET_FLAG_RELOAD |
						INTERNET_FLAG_NO_UI |
						INTERNET_FLAG_KEEP_CONNECTION |
						INTERNET_FLAG_NO_AUTO_REDIRECT;

		if (m_sComponents.nScheme == INTERNET_SCHEME_HTTPS) {
			// HTTPS で接続するため、
			// フラグに INTERNET_FLAG_SECURE を追加。
			dwFlags = dwFlags | INTERNET_FLAG_SECURE;
		}

		// メソッド名とHTTPバージョンを、メソッドに応じて変更する
		LPCTSTR szMethodName = L"";
		LPCTSTR szHttpVersion = NULL;
		if( execType == EXEC_SENDRECV_TYPE_GET ) {
			szMethodName  = L"GET";
			szHttpVersion = NULL;		// デフォルトのHTTPバージョン
		}else{
			szMethodName  = L"POST";
			szHttpVersion = L"HTTP/1.1";
		}

		// リクエスト送信
		m_hRequest = HttpOpenRequest( m_hConnection, 
									  szMethodName,	// メソッド
									  m_strPath,
									  szHttpVersion,// HTTP バージョン
									  m_ref,		// 履歴なし
									  NULL,
									  dwFlags,		// フラグ
									  NULL);		// コンテキストなし
	}
	catch (CException &) {
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"例外発生(HttpOpenRequest)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
		else {
			return WM_MZ3_GET_ABORT;
		}
	}
	if( m_hRequest == NULL ) {
		// リクエスト失敗（オープン）
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		m_strErrorMsg = L"リクエスト失敗（オープン）";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}


	// --------------------------------------------------
	// 中断確認
	// --------------------------------------------------
	if (m_abort != FALSE) {
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		return WM_MZ3_GET_ABORT;
	}

	//--- メソッドに応じて、「データ送信」or「リクエスト送信」を切り替える
	if( execType == EXEC_SENDRECV_TYPE_GET ) {
		//--- GET メソッドなので「リクエスト送信」を実行
		util::MySetInformationText( m_hwnd, _T("リクエスト送信中") );

		// logging
		if( theApp.m_logger.isDebugEnabled() ) {
			CString msg;
			msg.Format( L"url : %s", m_strPath );

			// パスワードを消す
			static MyRegex reg;
			if( !reg.isCompiled() ) {
				if(! reg.compile( L"password=[^&]+" ) ) {
					MZ3LOGGER_DEBUG( FAILED_TO_COMPILE_REGEX_MSG );
				}
			}
			if( reg.isCompiled() ) {
				reg.replaceAll( msg, L"password=xxxxxxxx" );
			}
			MZ3LOGGER_DEBUG( msg );

			msg.Format( L"ref : %s", m_ref );
			MZ3LOGGER_DEBUG( msg );
		}

		try {

			BOOL bRet = HttpSendRequest(m_hRequest,
				NULL,    // 追加ヘッダなし
				0,       // ヘッダ長
				NULL,    // ボディなし
				0);      // ボディ長

			if( bRet == FALSE ) {
				// リクエスト送信失敗
				// m_hRequest, m_hConnection を閉じる。
				CloseInternetHandles();
				m_strErrorMsg = L"リクエスト送信失敗";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
		}
		catch (CException &) {
			// m_hRequest, m_hConnection を閉じる。
			CloseInternetHandles();
			if (m_abort == FALSE) {
				m_strErrorMsg = L"例外発生(HttpSendRequest)";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			else {
				return WM_MZ3_GET_ABORT;
			}
		}
		//--- リクエスト送信成功
	}else{
		//--- POST メソッドなので「データ送信」を実行
		util::MySetInformationText( m_hwnd, _T("データ送信中") );

		// データ送信
		// POSTデータの設定
		CPostData::post_array& buf = m_postData->GetPostBody();

		// logging
		if( theApp.m_logger.isDebugEnabled() ) {
			CString msg;
			msg.Format( L"url : %s", m_strPath );
			MZ3LOGGER_DEBUG( msg );
			msg.Format( L"ref : %s", m_ref );
			MZ3LOGGER_DEBUG( msg );
			msg.Format( L"post-body : %s", CStringW(&buf[0], buf.size()) );
			MZ3LOGGER_DEBUG( msg );
		}

		try {
			BOOL bRet = FALSE;
			switch (m_postData->GetContentType() ) {
			case CONTENT_TYPE_MULTIPART:
				{
					// multiform
					TCHAR pszContentType_Multi[] = 
						_T("Content-Type: multipart/form-data; boundary=---------------------------7d62ee108071e\r\n");
					bRet = ::HttpSendRequest(m_hRequest,
						pszContentType_Multi,
						lstrlen(pszContentType_Multi),
						&buf[0],
						buf.size() );
					if( bRet == FALSE ) {
						// ヘッダ送信失敗
						// m_hRequest, m_hConnection を閉じる。
						CloseInternetHandles();
						m_strErrorMsg = L"ヘッダ送信失敗";
						MZ3LOGGER_ERROR( m_strErrorMsg );
						return WM_MZ3_GET_ERROR;
					}
				}
				break;

			case CONTENT_TYPE_FORM_URLENCODED:
				{
					// urlencode
					TCHAR pszContentType_UrlEncoded[] = 
						_T("Content-Type: application/x-www-form-urlencoded\r\n");
					bRet = ::HttpSendRequest(m_hRequest,
						pszContentType_UrlEncoded,
						lstrlen(pszContentType_UrlEncoded),
						&buf[0],
						buf.size() );
					if( bRet == FALSE ) {
						// POSTメッセージ送信失敗
						// m_hRequest, m_hConnection を閉じる。
						CloseInternetHandles();
						m_strErrorMsg = L"POSTメッセージ送信中にエラーが発生しました";
						MZ3LOGGER_ERROR( m_strErrorMsg );
						return WM_MZ3_GET_ERROR;
					}
				}
				break;

			default:
				{
					// 未サポートのContent-Typeなのでエラーとする。
					m_strErrorMsg.Format( 
						L"アプリケーションエラー\r\n"
						L"未サポートのContent-Type [%d]", m_postData->GetContentType() );
					MZ3LOGGER_ERROR( m_strErrorMsg );
					return WM_MZ3_GET_ERROR;
				}
				break;
			}
		}
		catch (CException &) {
			// m_hRequest, m_hConnection を閉じる。
			CloseInternetHandles();
			if (m_abort == FALSE) {
				m_strErrorMsg = L"例外発生(HttpSendRequest)";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			else {
				return WM_MZ3_GET_ABORT;
			}
		}
		//--- データ送信成功
	}

	// --------------------------------------------------
	// 中断確認
	// --------------------------------------------------
	if (m_abort != FALSE) {
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		return WM_MZ3_GET_ABORT;
	}

	// 受信バッファ（全体）
	kfm::kf_buf_type recv_buffer;
	recv_buffer.reserve( 1024*100 );	// バッファサイズを大きくしておく

	try {
		DWORD dwContentLen  = 0;		// コンテント長（0 なら受信サイズ不明）
		DWORD dwContentLenSample = 0;	// コンテント長の参考値（前回受信時の値）

		// コンテント長を確認する
		{
			util::MySetInformationText( m_hwnd, _T("受信サイズ確認中") );
			DWORD dwContentLenSize = sizeof(dwContentLen);
			if( HttpQueryInfo(m_hRequest, 
				HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
				(LPVOID)&dwContentLen,
				&dwContentLenSize,
				0) ) 
			{
				// コンテント長によって完了した割合 (%) を計算できる。
			}else{
				// コンテント長がない。
				// 前回の同ページの受信サイズがあれば、参考値として割合（%）を計算する。
				CFileStatus status;
				if( CFile::GetStatus(util::MakeLogfilePath(theApp.m_mixi4recv),status) &&
					status.m_size > 0 ) 
				{
					// 参考値を設定
					dwContentLenSample = (DWORD)status.m_size;
					dwContentLen = 0;
				}else{
					// 完了した割合 (%) を計算できない。
					// 完了するまで受信。
					dwContentLen = 0;
				}
			}
		}

		// ステータスコードの取得
		{
			util::MySetInformationText( m_hwnd, _T("ステータスコード確認中") );
			TCHAR szStatusCode[16] = L"";
			DWORD len = sizeof(szStatusCode);
			if( HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE, &szStatusCode, &len, 0 ) ) {
				// ステータスコード取得成功。
				// ステータスコードが 30x ならリダイレクト。
				if( lstrcmp( szStatusCode, L"301" ) == 0 ||
					lstrcmp( szStatusCode, L"302" ) == 0 ) 
				{
					// リダイレクト先を取得する
					TCHAR szLocation[1024] = L"";
					DWORD len = sizeof(szLocation);
					if( HttpQueryInfo(m_hRequest, HTTP_QUERY_LOCATION, &szLocation, &len, 0 ) ) {
						// リダイレクト先が取得できたので、リダイレクト実行。
						// とりあえず再帰で。
						m_uri = szLocation;
						m_nRedirect ++;

						if( theApp.m_logger.isDebugEnabled() ) {
							CString msg;
							msg.Format( L"リダイレクト：[%s]", m_uri );
							MZ3LOGGER_DEBUG( msg );
						}
						return ExecSendRecv( execType );
					}
				}
			}
		}

		DWORD dwOneRecvSize = theApp.m_optionMng.GetRecvBufSize();	// 一度に受信するサイズ
		{
			// バリデーション
			if( dwOneRecvSize < 128 ) {
				dwOneRecvSize = 128;
			}
			if( dwOneRecvSize > 1024*100 ) {
				dwOneRecvSize = 1024*100;
			}
		}
		// 受信処理
		util::StopWatch sw;
		sw.start();
		for(;;) {
			// リクエストサイズの計算
			// 基本的には dwOneRecvSize を用いる。
			// 総サイズが分かっているリクエスト（画像など、CGI以外）で、
			// 残りサイズが dwOneRecvSize より小さい場合は、残りサイズを指定する。
			DWORD dwRequestSize = dwOneRecvSize;
			if( dwContentLen > 0 && dwContentLen-recv_buffer.size() < dwRequestSize ) {
				dwRequestSize = dwContentLen - recv_buffer.size();
			}

			// 受信
			static std::vector<char> pData( 1024*100+1 );
			DWORD dwBytesRead = 0;
			if( InternetReadFile(m_hRequest, &pData[0], dwRequestSize, &dwBytesRead) == FALSE ) {
				break;
			}
			if( dwBytesRead <= 0 ) {
				break;
			}

			// サイズ報告付き
			CString msg;
			DWORD dwElapsedMsec = sw.getElapsedMilliSecUntilNow();
			if( dwContentLen > 0 ) {
				// 進捗率計算
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), dwContentLen, dwElapsedMsec );
			}else if( dwContentLenSample > 0 ) {
				// 前回受信時の値（参考値）で進捗率算出
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), -(LONG)dwContentLenSample, dwElapsedMsec );
			}else{
				// 受信サイズ、同参考値もないため、受信済みサイズのみ表示
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), 0, dwElapsedMsec );
			}

			pData[dwBytesRead] = 0;

			// recv_buffer に追加
			my_append_buf( recv_buffer, &pData[0], dwBytesRead );

			// 総データ受信量の加算
			theApp.m_optionMng.AddTotalRecvBytes( dwBytesRead );

			// 中断確認
			if (m_abort != FALSE) {
				// m_hRequest, m_hConnection を閉じる。
				CloseInternetHandles();
				return WM_MZ3_GET_ABORT;
			}
		}
		// 受信完了
		MySetTrafficInfo( m_hwnd, recv_buffer.size(), recv_buffer.size(), sw.getElapsedMilliSecUntilNow() );
	}
	catch (CException &) {
		// m_hRequest, m_hConnection を閉じる。
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"例外発生(受信処理中)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
		else {
			return WM_MZ3_GET_ABORT;
		}
	}

	// 受信サイズ０なら失敗、中断
	if( recv_buffer.empty() )
	{
		m_strErrorMsg = L"受信サイズが 0 byte";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}

	// HTML なら文字コード変換を実施する
	if (m_fileType == FILE_HTML) {

		util::MySetInformationText( m_hwnd, _T("データ変換中") );

		if (m_abort != FALSE) {
			// 中断
			// m_hRequest, m_hConnection を閉じる。
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}

//		util::StopWatch sw;
//		sw.start();

		// 文字コード変換(SJISに変換)
		kfm::kf_buf_type out_buf;
		{
			out_buf.reserve( recv_buffer.size() );
			kfm::kfm k( recv_buffer, out_buf );
			k.tosjis();
		}

//		sw.stop();
//		CString msg;
//		msg.Format( L"%d [msec]", sw.getElapsedMilliSec() );
//		MessageBox( m_hwnd, msg, L"", MB_OK );

//		sw.start();
		// ファイル出力
		{
			FILE* fp_out = _wfopen( theApp.m_filepath.temphtml, _T("wb"));
			if( fp_out == NULL ) {
				m_strErrorMsg = L"ファイルの出力に失敗しました";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			
			fwrite( &out_buf[0], out_buf.size(), 1, fp_out );
			fclose(fp_out);
		}
//		sw.stop();
//		msg.Format( L"%d [msec]", sw.getElapsedMilliSec() );
//		MessageBox( m_hwnd, msg, L"", MB_OK );

		if (m_abort != FALSE) {
			// 中断
			// m_hRequest, m_hConnection を閉じる。
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}
	}else{
		// HTML ではないので、バッファをファイルに出力する
		FILE* fp_out = _wfopen( theApp.m_filepath.temphtml, _T("wb"));
		if( fp_out == NULL ) {
			m_strErrorMsg = L"ファイルの出力に失敗しました";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}

		fwrite( &recv_buffer[0], recv_buffer.size(), 1, fp_out );
		fclose(fp_out);
	}

	util::MySetInformationText( m_hwnd, _T("アクセス終了") );

	// 終了通知
	if( execType == EXEC_SENDRECV_TYPE_GET ) {
		switch( m_fileType ) {
		case FILE_BINARY:
			return WM_MZ3_GET_END_BINARY;
		case FILE_HTML:
		default:
			return WM_MZ3_GET_END;
		}
	}else{
		return m_postData->GetSuccessMessage();
	}
}

/**
 * m_hRequest, m_hConnection を閉じる。
 */
void CInetAccess::CloseInternetHandles()
{
	// m_hRequest が NULL でなければ、閉じる
	if( m_hRequest != NULL ) {
		MZ3LOGGER_DEBUG( L"InternetCloseHandle( m_hRequest )" );
		InternetCloseHandle( m_hRequest );
		m_hRequest = NULL;
	}

	// m_hConnection が NULL でなければ、閉じる
	if( m_hConnection != NULL ) {
		MZ3LOGGER_DEBUG( L"InternetCloseHandle( m_hConnection )" );
		InternetCloseHandle( m_hConnection );
		m_hConnection = NULL;
	}
}

/**
 * 接続のための前処理
 *
 * URI の解析、分解を行う
 */
void CInetAccess::ParseURI()
{
	TCHAR lpszScheme[256];
	TCHAR lpszUrlPath[1024];
	TCHAR lpszExtraInfo[1024];

	ZeroMemory(&m_sComponents,sizeof(URL_COMPONENTS));
	m_sComponents.dwStructSize		= sizeof(URL_COMPONENTS);
	m_sComponents.lpszHostName		= m_lpszHostName;
	m_sComponents.dwHostNameLength	= sizeof(m_lpszHostName) / sizeof(TCHAR);
	m_sComponents.lpszScheme		= lpszScheme;
	m_sComponents.dwSchemeLength	= sizeof(lpszScheme) / sizeof(TCHAR);
	m_sComponents.lpszUserName		= m_lpszUserName;
	m_sComponents.dwUserNameLength	= sizeof(m_lpszUserName) / sizeof(TCHAR);
	m_sComponents.lpszPassword		= m_lpszPassword;
	m_sComponents.dwPasswordLength	= sizeof(m_lpszPassword) / sizeof(TCHAR);
	m_sComponents.lpszUrlPath		= lpszUrlPath;
	m_sComponents.dwUrlPathLength	= sizeof(lpszUrlPath) / sizeof(TCHAR);
	m_sComponents.lpszExtraInfo		= lpszExtraInfo;
	m_sComponents.dwExtraInfoLength	= sizeof(lpszExtraInfo) / sizeof(TCHAR);

	::InternetCrackUrl(m_uri, lstrlen(m_uri), ICU_ESCAPE, &m_sComponents);

	// パス部分を生成
	m_strPath = m_uri;
	int nFind = m_strPath.Find(m_lpszHostName);
	if (nFind >= 0) {
		// ホスト名が見つかった
		// '/' で分割してみる
		nFind = m_strPath.Find(_T("/"),nFind);
		if (nFind >= 0) {
			// '/' も見つかったので、'/' 以降を抽出
			m_strPath = m_strPath.Right(m_strPath.GetLength() - nFind);
		}
	}
	if (nFind < 0) {
		// ホスト名が見つからなかった
		m_strPath = lpszUrlPath;
		m_strPath += lpszExtraInfo;
	}
}
