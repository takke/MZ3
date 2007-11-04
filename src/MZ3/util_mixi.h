#pragma once
/**
 * mixi関連ユーティリティ
 */

/// MZ3 用ユーティリティ
namespace util
{
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
	case ACCESS_MOVIE:				text = L"動画";					break;
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
	case ACCESS_LIST_NEW_BBS_COMMENT:text = L"コミュコメント記入履歴";		break;
	case ACCESS_LIST_NEW_BBS:		text = L"コミュ書込一覧";		break;
	case ACCESS_LIST_BBS:			text = L"トピック一覧";			break;
	case ACCESS_LIST_MYDIARY:		text = L"日記一覧";				break;
	case ACCESS_LIST_FOOTSTEP:		text = L"足あと";				break;
	case ACCESS_LIST_MESSAGE_IN:	text = L"メッセージ(受信箱)";	break;
	case ACCESS_LIST_MESSAGE_OUT:	text = L"メッセージ(送信箱)";	break;
	case ACCESS_LIST_BOOKMARK:		text = L"ブックマーク";			break;
	case ACCESS_LIST_CALENDAR:		text = L"カレンダー";			break;  //icchu追加

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

}