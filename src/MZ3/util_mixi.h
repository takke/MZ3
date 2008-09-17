/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * mixi関連ユーティリティ
 */
#include "util_base.h"
#include "MixiUrlParser.h"

/// MZ3 用ユーティリティ
namespace util
{

/**
 * URL からアクセス種別を推定する
 */
inline ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url ) 
{
	// view 系
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // メイン
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // 日記内容
	if( url.Find( L"neighbor_diary.pl" ) != -1 ){ return ACCESS_NEIGHBORDIARY;} // 日記内容(次の日記、前の日記)
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // コミュニティ内容
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // アンケート
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // イベント
	if( url.Find( L"list_event_member.pl" ) != -1 ) { return ACCESS_EVENT_MEMBER; } // イベント参加者一覧
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // 自分の日記内容
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // メッセージ
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // ニュース内容
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // 個人ページ
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // コミュニティページ

	// list 系
	if( url.Find( L"list_bookmark.pl?kind=community" ) != -1 ) { return ACCESS_LIST_FAVORITE_COMMUNITY; }
	if( url.Find( L"list_bookmark.pl" ) != -1 ) { return ACCESS_LIST_FAVORITE_USER; }
	
	// 不明なので INVALID とする
	return ACCESS_INVALID;
}

/**
 * ユーザIDを AuthorID, OwnerID から取得する
 */
inline int GetUserIdFromAuthorOrOwnerID( const CMixiData& mixi )
{
	int userId = mixi.GetAuthorID();
	if( userId < 0 ) {
		userId = mixi.GetOwnerID();
	}
	return userId;
}

/**
 * url 作成（http://mixi.jp/ の補完）
 */
inline CString CreateMixiUrl(LPCTSTR str)
{
	if( wcsstr( str, L"mixi.jp" ) == NULL &&
		wcsstr( str, L"http://" ) == NULL)
	{
		CString uri;
		uri.Format(_T("http://mixi.jp/%s"), str);
		return uri;
	}else{
		return str;
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
	case ACCESS_NEIGHBORDIARY:
		logId.Format(_T("d%d"), mixi::MixiUrlParser::GetID( mixi.GetURL() ) );
		break;
	case ACCESS_BBS:
		logId.Format(_T("b%d"), mixi.GetID());
		break;
	case ACCESS_EVENT:
		logId.Format(_T("v%d"), mixi.GetID());
		break;
	case ACCESS_EVENT_JOIN:
		logId.Format(_T("j%d"), mixi.GetID());
		break;
	case ACCESS_ENQUETE:
		logId.Format(_T("e%d"), mixi.GetID());
		break;
	case ACCESS_PROFILE:
		logId.Format(_T("p%d"), mixi.GetID());
		break;

	default:
		logId = L"";
		break;
	}

	return logId;
}

}