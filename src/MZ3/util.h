/**
 * その他のユーティリティ関数
 */
#pragma once

#include "MyRegex.h"
#include "util_base.h"

/// MZ3 用ユーティリティ
namespace util
{

/**
 * gsgetfile.dll を用いてファイル選択画面を表示する
 *
 * 成功時は IDOK が返る
 */
/*
inline int GetOpenFileNameEx(OPENFILENAME* pofn)
{
#ifdef WINCE
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		BOOL (*gsGetOpenFileName)(OPENFILENAME* pofn);
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
		if (gsGetOpenFileName) {
			BOOL ret = gsGetOpenFileName(pofn);
			FreeLibrary(hInst);
			return ret;
		}
		FreeLibrary(hInst);
	}
	return GetOpenFileName(pofn);
#else
	// for win32
	// TODO 実装すること
	return 0;
#endif
}
*/

/**
 * フォルダ選択画面を表示する
 *
 * @param hWnd			[in] 親画面
 * @param szTitle		[in] フォルダ選択画面のタイトル
 * @param strFolderPath	[in/out] 初期選択フォルダ＆選択済みフォルダパス
 * @return 成功時は true、失敗時は false を返す。
 */
inline bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& strFolderPath )
{
#ifdef WINCE
	// FDQ.DLL を用いてフォルダ選択画面を表示する

	// DLLを取得する（Windowsディレクトに無いときはフルパスで指定する）
	HINSTANCE hInst = LoadLibrary (_T("FDQ.DLL"));
	if( hInst == NULL ) {
		MessageBox( NULL, L"FDQ.DLL not found", L"", MB_OK );
		return false;
	}
	int (pascal *FolderTree) (HWND,TCHAR*,TCHAR*);

	FolderTree = (int (pascal *) (HWND,TCHAR*,TCHAR*)) GetProcAddress (hInst, TEXT("FolderTree"));
	if (FolderTree == 0) {
		FreeLibrary (hInst);
		return false;	//エラー
	}

	// 保存先変更画面の表示
	TCHAR szFileName[256] = L"";
	wcsncpy( szFileName, szFolderPath, 255 );
	TCHAR szTitleBuf[256] = L"";
	wcsncpy( szTitleBuf, szTitle, 255 );
	int rc = FolderTree (hWnd, szTitleBuf, szFileName);
	FreeLibrary (hInst);

	if( rc == IDOK && util::ExistFile(szFileName) ) {
		strFolderPath = szFileName;
		return true;
	}else{
		return false;
	}
#else
    BROWSEINFO bInfo;
    LPITEMIDLIST pIDList;
    TCHAR szDisplayName[MAX_PATH];
    
    // BROWSEINFO構造体に値を設定
    bInfo.hwndOwner             = hWnd;						// ダイアログの親ウインドウのハンドル
    bInfo.pidlRoot              = NULL;                     // ルートフォルダを示すITEMIDLISTのポインタ (NULLの場合デスクトップフォルダが使われます）
    bInfo.pszDisplayName        = szDisplayName;            // 選択されたフォルダ名を受け取るバッファのポインタ
    bInfo.lpszTitle             = szTitle;					// ツリービューの上部に表示される文字列 
    bInfo.ulFlags               = BIF_RETURNONLYFSDIRS;     // 表示されるフォルダの種類を示すフラグ
    bInfo.lpfn                  = NULL;                     // BrowseCallbackProc関数のポインタ
    bInfo.lParam                = (LPARAM)0;                // コールバック関数に渡す値

    // フォルダ選択ダイアログを表示
    pIDList = ::SHBrowseForFolder(&bInfo);
    if(pIDList == NULL){

        // フォルダが選択されずにダイアログが閉じられた
		return false;

    }else{

        // ItemIDListをパス名に変換します
        if(!::SHGetPathFromIDList(pIDList, szDisplayName)){
            // エラー処理
			return false;
        }

        // szDisplayNameに選択されたフォルダのパスが入っています
		strFolderPath = szDisplayName;

        // 最後にpIDListのポイントしているメモリを開放します
        ::CoTaskMemFree( pIDList );
    }
	return true;
#endif
}

/**
 * 画面下部の情報領域にメッセージを設定する
 */
inline void MySetInformationText( HWND hWnd, LPCTSTR szMessage )
{
	CString text;
	if( theApp.m_inet.IsConnecting() ) {
		// 通信中なので、アクセス種別を頭に付ける
		text = AccessType2Message(theApp.m_accessType);

		// リダイレクト回数が１回以上なら、それも表示しておく
		int nRedirect = theApp.m_inet.GetRedirectCount();
		if( nRedirect > 0 ) {
			CString s;
			s.Format( L"[%d]", nRedirect );
			text += s;
		}

		text += L"：";
		text += szMessage;
	}else{
		text = szMessage;
	}
	::SendMessage( hWnd, WM_MZ3_ACCESS_INFORMATION, NULL, (LPARAM)&text );
}

/**
 * 指定されたビットマップ hBitmap を hdc に描画する
 */
inline bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int tox, int toy )
{
	if( hBitmap == NULL ) {
		return false;
	}

	BITMAP	bmp;
	GetObject( hBitmap, sizeof(bmp), &bmp );

	HDC hdc1 = CreateCompatibleDC(NULL);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc1,hBitmap);

	// 転送
//	TRACE( L"DrawBitmap [%d,%d,%d,%d,%d,%d]\n", x, y, w, h, tox, toy );
	BitBlt( hdc, x, y, w, h, hdc1, tox, toy, SRCCOPY );

	SelectObject( hdc1, hBitmapOld );

	DeleteDC( hdc1 );
	return true;
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
 * CMixiData に対応するログファイルのパスを生成する
 */
inline CString MakeLogfilePath( const CMixiData& data )
{
	// アクセス種別に応じてパスを生成する
	switch( data.GetAccessType() ) {

	// ログインページ
	case ACCESS_LOGIN:				return theApp.m_filepath.logFolder + L"\\login.html";

	// トップページ
	case ACCESS_MAIN:				return theApp.m_filepath.logFolder + L"\\home.html";
	
	// リスト系
	case ACCESS_LIST_DIARY:			return theApp.m_filepath.logFolder + L"\\new_friend_diary.html";
	case ACCESS_LIST_NEW_COMMENT:	return theApp.m_filepath.logFolder + L"\\new_comment.html";
	case ACCESS_LIST_COMMENT:		return theApp.m_filepath.logFolder + L"\\list_comment.html";
	case ACCESS_LIST_NEW_BBS:
		{
			// ページ番号毎に分割
			CString page = L"1";
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\new_bbs_%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_MESSAGE_IN:	return theApp.m_filepath.logFolder + L"\\list_message_in.html";
	case ACCESS_LIST_MESSAGE_OUT:	return theApp.m_filepath.logFolder + L"\\list_message_out.html";
	case ACCESS_LIST_FOOTSTEP:		return theApp.m_filepath.logFolder + L"\\show_log.html";
	case ACCESS_LIST_MYDIARY:		return theApp.m_filepath.logFolder + L"\\list_diary.html";
	case ACCESS_LIST_CALENDAR:		return theApp.m_filepath.logFolder + L"\\show_calendar.html";  //icchu追加
	case ACCESS_LIST_NEWS:
		{
			// カテゴリ、ページ番号毎に分割
			// url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
			// url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn

			CString url = data.GetURL();
			url += L"&";

			// カテゴリ、ページ番号抽出
			CString category;
			util::GetBetweenSubString( url, L"id=", L"&", category );
			CString page;
			util::GetBetweenSubString( url, L"page=", L"&", page );

			CString path;
			path.Format( L"%s\\list_news_category_%s_%s.html", theApp.m_filepath.logFolder, category, page );
			return path;
		}

	case ACCESS_LIST_BBS:
		{
			// url : list_bbs.pl?id=xxx
			CString id;
			util::GetAfterSubString( data.GetURL(), L"id=", id );

			CString path;
			path.Format( L"%s\\list_bbs_%s.html", theApp.m_filepath.logFolder, id );
			return path;
		}

	case ACCESS_LIST_FAVORITE:
		{
			// url(1) : http://mixi.jp/list_bookmark.pl
			// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
			// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
			// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community

			// ページ分割
			CString page;
			util::GetAfterSubString( data.GetURL(), L"?", page );

			page.Replace( L"&", L"_" );
			page.Replace( L"=", L"_" );
			if( !page.IsEmpty() ) {
				page.Insert( 0, '_' );
			}

			CString path;
			path.Format( L"%s\\list_bookmark%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_FRIEND:
		{
			// url(1) : http://mixi.jp/list_friend.pl
			// url(2) : http://mixi.jp/list_friend.pl?page=2

			// ページ分割
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_friend%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_COMMUNITY:
		{
			// url(1) : http://mixi.jp/list_community.pl?id=xxx
			// url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2

			// ページ分割
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_community%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_NEW_BBS_COMMENT:
		{
			// url(1) : http://mixi.jp/new_bbs_comment.pl?id=xxx
			// url(2) : http://mixi.jp/new_bbs_comment.pl?id=xxx&page=2

			// ページ分割
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\new_bbs_comment%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_INTRO:
		{
			// url(1) : http://mixi.jp/show_intro.pl
			// url(2) : http://mixi.jp/show_intro.pl?page=2

			// ページ分割
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\show_intro%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	// アイテム系
	case ACCESS_MYDIARY:
	case ACCESS_DIARY:
		{
			CString owner_id, id;
			// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
			// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
			CString url = data.GetURL() + L"&";	// url(1) の場合に対処するため、後ろに & を付けておく
			util::GetBetweenSubString( url, L"?id=", L"&", id );
			util::GetBetweenSubString( url, L"owner_id=", L"&", owner_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.diaryFolder, owner_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_BBS:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_bbs.pl?id=9360823&comm_id=1198460
			// url(2) : http://mixi.jp/view_bbs.pl?page=4&comm_id=1198460&id=9360823
			CString url;	// ? 以降の URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// 後ろに & を付けておく
			url += L"&";
			// 頭に & を付けておく
			url.Insert( 0, L"&" );
			// url(1) : &id=9360823&comm_id=1198460&
			// url(2) : &page=4&comm_id=1198460&id=9360823&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.bbsFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_ENQUETE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
			CString url;	// ? 以降の URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// 後ろに & を付けておく
			url += L"&";
			// 頭に & を付けておく
			url.Insert( 0, L"&" );
			// url(1) : &id=13632077&comment_count=1&comm_id=1198460&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.enqueteFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_EVENT:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
			CString url;	// ? 以降の URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// 後ろに & を付けておく
			url += L"&";
			// 頭に & を付けておく
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&comment_count=20&comm_id=xxx&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.eventFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_MESSAGE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// url(2) : http://mixi.jp/view_message.pl?id=xxx&box=outbox
			CString url;	// ? 以降の URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// 後ろに & を付けておく
			url += L"&";
			// 頭に & を付けておく
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&box=inbox&
			// url(2) : &id=xxx&box=outbox&
			util::GetBetweenSubString( url, L"&id=", L"&", id );

			// url に inbox があれば inbox ディレクトリに。
			CString path;
			if( url.Find( L"box=inbox" ) != -1 ) {
				path.Format( L"%s\\inbox", theApp.m_filepath.messageFolder );
			}else{
				path.Format( L"%s\\outbox", theApp.m_filepath.messageFolder );
			}
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_NEWS:
		{
			CString media_id, id;
			// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
			CString url;	// ? 以降の URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// 後ろに & を付けておく
			url += L"&";
			// 頭に & を付けておく
			url.Insert( 0, L"&" );
			// url(1) : &id=132704&media_id=4&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"&media_id=", L"&", media_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.newsFolder, media_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	default:
		// 上記以外なら、ファイル名なし
		return L"";
	}
}

/**
 * URL からアクセス種別を推定する
 */
inline ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url ) 
{
	// とりあえず view 系のみ
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // メイン
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // 日記内容
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // コミュニティ内容
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // アンケート
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // イベント
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // 自分の日記内容
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // メッセージ
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // ニュース内容
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // 個人ページ
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // コミュニティページ

	// 不明なので INVALID とする
	return ACCESS_INVALID;
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
 * url 作成（http://mixi.jp/ の補完）
 */
inline CString CreateMixiUrl(LPCTSTR str)
{
	if( wcsstr( str, L"mixi.jp" ) == NULL ) {
		CString uri;
		uri.Format(_T("http://mixi.jp/%s"), str);
		return uri;
	}else{
		return str;
	}
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

/**
 * 指定されたURLが mixi の URL であれば、
 * mixi モバイルの自動ログイン用URLに変換する
 */
inline CString ConvertToMixiMobileAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ が含まれているので mixi の URL とみなす。
		// mixi モバイル用URLに変換したURLを返す。
		return theApp.MakeLoginUrlForMixiMobile( path );
	}

	// mixi 用URLではないため、変換せずに返す。
	return url;
}

/**
 * 指定されたURLが mixi の URL であれば、
 * mixi の自動ログイン用URLに変換する
 */
inline CString ConvertToMixiAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ が含まれているので mixi の URL とみなす。
		// mixi の自動ログイン用URLに変換したURLを返す。
		return theApp.MakeLoginUrl( path );
	}

	// mixi 用URLではないため、変換せずに返す。
	return url;
}

/**
 * 指定されたURLをブラウザで開く（確認画面付き）
 */
inline void OpenBrowserForUrl( LPCTSTR url )
{
	// 確認画面
	CString msg;
	msg.Format( 
		L"下記のURLをブラウザで開きます。\n\n"
		L"%s\n\n"
		L"よろしいですか？", url );
	if( MessageBox( NULL, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// 自動ログイン変換
	CString requestUrl = url;
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}

	// ブラウザで開く
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * 指定されたユーザのプロフィールページをブラウザで開く（確認画面付き）
 */
inline void OpenBrowserForUser( LPCTSTR url, LPCTSTR szUserName )
{
	// 確認画面
	CString msg;
	msg.Format( 
		L"%s さんのプロフィールページをブラウザで開きます。\n\n"
		L"%s\n\n"
		L"よろしいですか？", szUserName, url );
	if( MessageBox( NULL, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// 自動ログイン変換
	CString requestUrl = url;
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}

	// ブラウザで開く
	util::OpenUrlByBrowser( requestUrl );
}

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
 * 未コンパイルであればコンパイルする。
 *
 * コンパイル失敗時はエラーログを出力する
 */
inline bool CompileRegex( MyRegex& reg, LPCTSTR szPattern )
{
	if( reg.isCompiled() ) {
		return true;
	} else {
		if(! reg.compile( szPattern ) ) {
			CString msg = FAILED_TO_COMPILE_REGEX_MSG;
			msg += L", pattern[";
			msg += szPattern;
			msg += L"]";
			MZ3LOGGER_FATAL( msg );
			return false;
		}
		return true;
	}
}

/**
 * 未読・既読状態管理ログファイル用のIDを生成する。
 */
inline CString GetLogIdString( const CMixiData& mixi )
{
	CString logId;

	switch (mixi.GetAccessType()) {
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
		{
			CString id;
			if (util::GetBetweenSubString( mixi.GetURL(), L"id=", L"&", id ) >= 0) {
				logId.Format(_T("d%s"), id);
			}
		}
		break;
	case ACCESS_BBS:
		logId.Format(_T("b%d"), mixi.GetID());
		break;
	case ACCESS_EVENT:
		logId.Format(_T("v%d"), mixi.GetID());
		break;
	case ACCESS_ENQUETE:
		logId.Format(_T("e%d"), mixi.GetID());
		break;

	default:
		logId = L"";
		break;
	}

	return logId;
}

}
