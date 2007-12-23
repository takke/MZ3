/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * MZ3依存ユーティリティ関数群
 * 
 * 主に theApp に依存するもの。
 */

#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"

/// MZ3 用ユーティリティ
namespace util
{

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

inline CString ExtractFilenameFromUrl( const CString& url, const CString& strDefault )
{
	int idx = url.ReverseFind( '/' );
	if (idx >= 0) {
		return url.Mid( idx+1 );
	}
	return strDefault;
}

inline CString MakeImageLogfilePathFromUrl( const CString& url )
{
	CString filename = ExtractFilenameFromUrl( url, L"" );
	if (!filename.IsEmpty()) {
		return theApp.m_filepath.imageFolder + L"\\" + filename;
	}
	return L"";
}

/**
 * CMixiData に対応する画像ファイルのパスを生成する
 */
inline CString MakeImageLogfilePath( const CMixiData& data )
{
	// アクセス種別に応じてパスを生成する
	switch( data.GetAccessType() ) {
	case ACCESS_PROFILE:
	case ACCESS_COMMUNITY:
	case ACCESS_TWITTER_USER:
		if (data.GetImageCount()>0) {
			CString path = MakeImageLogfilePathFromUrl( data.GetImage(0) );
			if (!path.IsEmpty()) {
				return path;
			}
		}
		break;
	default:
		break;
	}
	return L"";
}

/**
 * mixi 用アクセス種別かどうかを返す
 */
inline bool IsMixiAccessType( ACCESS_TYPE type )
{
	switch (type) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		return false;
	default:
		return true;
	}
}

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
	case ACCESS_LIST_FOOTSTEP:		return theApp.m_filepath.logFolder + L"\\tracks.xml";
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

	case ACCESS_PROFILE:
		{
			// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
			int id = mixi::MixiUrlParser::GetID( data.GetURL() );
			return util::FormatString( L"%s\\%d.html", theApp.m_filepath.profileFolder, id );
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

	//--- Twitter 系
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		{
			// http://twitter.com/statuses/friends_timeline.xml
			// http://twitter.com/statuses/friends_timeline/takke.xml
			// http://twitter.com/statuses/replies.xml
			// http://twitter.com/statuses/user_timeline.xml
			// http://twitter.com/statuses/user_timeline/takke.xml
			// => twitter_user_timeline_takke.xml

			CString filename;
			CString after;
			if (util::GetAfterSubString( data.GetURL(), L"statuses/", after )<0) {
				// default
				filename = L"twitter_friends_timeline.xml";
			} else {
				after.Replace( L"/", L"_" );
				filename = L"twitter_" + after;
			}

			return theApp.m_filepath.logFolder + L"\\" + filename;
		}

	default:
		// 上記以外なら、ファイル名なし
		return L"";
	}
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
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// 自動ログイン変換
	CString requestUrl = url;

	// WindowsMobile 版のみ自動ログインURL変換を行う
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

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
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// 自動ログイン変換
	CString requestUrl = url;

	// WindowsMobile 版のみ自動ログインURL変換を行う
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

	// ブラウザで開く
	util::OpenUrlByBrowser( requestUrl );
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

}
