/**
 * その他のユーティリティ関数
 */
#pragma once

/// MZ3 用ユーティリティ
namespace util
{

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
	if( MessageBox( NULL, msg, L"MZ3", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// ブラウザで開く
	util::OpenUrlByBrowser( url );
}

/**
 * gsgetfile.dll を用いてファイル選択画面を表示する
 *
 * 成功時は IDOK が返る
 */
inline int GetOpenFileNameEx(OPENFILENAME* pofn)
{
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
}

/**
 * FDQ.DLL を用いてフォルダ選択画面を表示する
 *
 * @param hWnd			[in] 親画面
 * @param szTitle		[in] フォルダ選択画面のタイトル
 * @param strFolderPath	[in/out] 初期選択フォルダ＆選択済みフォルダパス
 * @return 成功時は true、失敗時は false を返す。
 */
inline bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& szFolderPath )
{
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
	wcsncpy( szFileName, theApp.m_filepath.logFolder, 255 );
	int rc = FolderTree (hWnd, _T("ログフォルダの変更"), szFileName);
	FreeLibrary (hInst);

	if( rc == IDOK && util::ExistFile(szFileName) ) {
		szFolderPath = szFileName;
		return true;
	}else{
		return false;
	}
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
	if( MessageBox( NULL, msg, L"MZ3", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// ブラウザで開く
	util::OpenUrlByBrowser( url );
}

/// アクセス種別を文字列に変換する
inline LPCTSTR AccessType2Message( ACCESS_TYPE type )
{
	LPCTSTR text = L"";
	switch( type ) {
	case ACCESS_LOGIN:				text = L"ログイン";				break;
	case ACCESS_MAIN:				text = L"メイン";				break;
	case ACCESS_DIARY:				text = L"日記内容";				break;
	case ACCESS_NEWS:				text = L"ニュース";				break;
	case ACCESS_BBS:				text = L"コミュ書込";			break;
	case ACCESS_ENQUETE:			text = L"アンケート";			break;
	case ACCESS_EVENT:				text = L"イベント";				break;
	case ACCESS_MYDIARY:			text = L"日記";					break;
	case ACCESS_ADDDIARY:			text = L"日記投稿";				break;
	case ACCESS_MESSAGE:			text = L"メッセージ";			break;
	case ACCESS_IMAGE:				text = L"画像";					break;
	case ACCESS_DOWNLOAD:			text = L"ダウンロード";			break;
	case ACCESS_PROFILE:			text = L"プロフィール";			break;
	case ACCESS_COMMUNITY:			text = L"コミュニティ";			break;
	case ACCESS_LIST_INTRO:			text = L"紹介文";				break;

	case ACCESS_LIST_DIARY:			text = L"日記一覧";				break;
	case ACCESS_LIST_NEW_COMMENT:	text = L"新着コメント一覧";		break;
	case ACCESS_LIST_COMMENT:		text = L"コメント一覧";			break;
	case ACCESS_LIST_NEWS:			text = L"ニュース一覧";			break;
	case ACCESS_LIST_FAVORITE:		text = L"お気に入り";			break;
	case ACCESS_LIST_FRIEND:		text = L"マイミク一覧";			break;
	case ACCESS_LIST_COMMUNITY:		text = L"コミュニティ一覧";		break;
	case ACCESS_LIST_NEW_BBS:		text = L"コミュ書込一覧";		break;
	case ACCESS_LIST_BBS:			text = L"トピック一覧";			break;
	case ACCESS_LIST_MYDIARY:		text = L"日記一覧";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"足あと";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"メッセージ(受信箱)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"メッセージ(送信箱)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"ブックマーク";			break;

	case ACCESS_GROUP_COMMUNITY:	text = L"コミュニティG";		break;
	case ACCESS_GROUP_MESSAGE:		text = L"メッセージG";			break;
	case ACCESS_GROUP_MYDIARY:		text = L"日記G";				break;
	case ACCESS_GROUP_NEWS:			text = L"ニュースG";			break;
	case ACCESS_GROUP_OTHERS:		text = L"その他G";				break;

	// POST 系
	case ACCESS_POST_CONFIRM_COMMENT:		text = L"コメント投稿（確認）";		break;
	case ACCESS_POST_ENTRY_COMMENT:			text = L"コメント投稿（書込）";		break;
	case ACCESS_POST_CONFIRM_REPLYMESSAGE:	text = L"メッセージ返信（確認）";	break;
	case ACCESS_POST_ENTRY_REPLYMESSAGE:	text = L"メッセージ返信（書込）";	break;
	case ACCESS_POST_CONFIRM_NEWMESSAGE:	text = L"新規メッセージ（確認）";	break;
	case ACCESS_POST_ENTRY_NEWMESSAGE:		text = L"新規メッセージ（書込）";	break;
	case ACCESS_POST_CONFIRM_NEWDIARY:		text = L"日記投稿（確認）";			break;
	case ACCESS_POST_ENTRY_NEWDIARY:		text = L"コメント投稿（書込）";		break;

	case ACCESS_INVALID:			text = L"<invalid>";			break;
	default:						text = L"<unknown>";			break;
	}
	return text;
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
		uri.Format(_T("http://mixi.jp/%s\n"), str);
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

}
