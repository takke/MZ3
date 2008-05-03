/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "AccessTypeInfo.h"

/// 初期化
bool AccessTypeInfo::init()
{
	//------------------------------------------------------------------
	//--- グループ系
	m_map[ACCESS_GROUP_COMMUNITY] = AccessTypeInfo::Data(
		""
		, L"コミュニティG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_MESSAGE] = AccessTypeInfo::Data(
		""
		, L"メッセージG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_MYDIARY] = AccessTypeInfo::Data(
		""
		, L"日記G"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_NEWS] = AccessTypeInfo::Data(
		""
		, L"ニュースG"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_OTHERS] = AccessTypeInfo::Data(
		""
		, L"その他G"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_GROUP_TWITTER] = AccessTypeInfo::Data(
		""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,リスト系
	ACCESS_TYPE type;
	
	type = ACCESS_LIST_INTRO;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"紹介文"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"show_intro.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"名前";
	m_map[type].bodyHeaderCol2NameA = L"紹介文";

	type = ACCESS_LIST_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_friend_diary.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"タイトル";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日時>>";

	type = ACCESS_LIST_NEW_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"新着コメント一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"タイトル";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日時>>";

	type = ACCESS_LIST_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"タイトル";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日時>>";

	type = ACCESS_LIST_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"ニュース一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol1Name  = L"見出し";
	m_map[type].bodyHeaderCol2NameA = L"配信時刻>>";
	m_map[type].bodyHeaderCol2NameB = L"配給元>>";

	type = ACCESS_LIST_FAVORITE_USER;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"お気に入りユーザ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_bookmark.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"名前";
	m_map[type].bodyHeaderCol2NameA = L"最終ログイン>>";
	m_map[type].bodyHeaderCol2NameB = L"自己紹介>>";

	type = ACCESS_LIST_FAVORITE_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"お気に入りコミュ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_bookmark.pl?kind=community";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol1Name  = L"コミュニティ";
	m_map[type].bodyHeaderCol2NameA = L"人数>>";
	m_map[type].bodyHeaderCol2NameB = L"説明>>";

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"マイミク一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_friend.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"名前";
	m_map[type].bodyHeaderCol2NameA = L"ログイン時刻";

	type = ACCESS_LIST_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュニティ一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"list_community.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"コミュニティ";
	m_map[type].bodyHeaderCol2NameA = L"人数";

	type = ACCESS_LIST_NEW_BBS_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュコメント記入履歴"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs_comment.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"トピック";
	m_map[type].bodyHeaderCol2NameA = L"コミュニティ>>";
	m_map[type].bodyHeaderCol2NameB = L"日時>>";

	type = ACCESS_LIST_NEW_BBS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュ書込一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"トピック";
	m_map[type].bodyHeaderCol2NameA = L"コミュニティ>>";
	m_map[type].bodyHeaderCol2NameB = L"日時>>";

	type = ACCESS_LIST_BBS;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"トピック一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	// 動的に生成されるためデフォルトURLは不要
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"トピック";
	m_map[type].bodyHeaderCol2NameA = L"日付";

	type = ACCESS_LIST_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_diary.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"タイトル";
	m_map[type].bodyHeaderCol2NameA = L"日時";

	type = ACCESS_LIST_FOOTSTEP;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"足あと"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://mixi.jp/atom/tracks/r=2/member_id={owner_id}";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"名前";
	m_map[type].bodyHeaderCol2NameA = L"時刻";

	type = ACCESS_LIST_MESSAGE_IN;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ(受信箱)"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"件名";
	m_map[type].bodyHeaderCol2NameA = L"差出人>>";
	m_map[type].bodyHeaderCol2NameB = L"日付>>";

	type = ACCESS_LIST_MESSAGE_OUT;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ(送信箱)"
		, REQUEST_METHOD_GET
		);
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl?box=outbox";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"件名";
	m_map[type].bodyHeaderCol2NameA = L"宛先>>";
	m_map[type].bodyHeaderCol2NameB = L"日付>>";

	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"ブックマーク"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol1Name  = L"トピック";
	m_map[type].bodyHeaderCol2NameA = L"コミュニティ";

	type = ACCESS_LIST_CALENDAR;
	m_map[type] = AccessTypeInfo::Data(
		"mixi"
		, L"カレンダー"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"show_calendar.pl";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_TITLE;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"タイトル";
	m_map[type].bodyHeaderCol2NameA = L"日付";

	//------------------------------------------------------------------
	//--- mixi,一般
	m_map[ACCESS_LOGIN] = AccessTypeInfo::Data(
		"mixi"
		, L"ログイン"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"メイン"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記内容"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"ニュース"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュ書込"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_ENQUETE] = AccessTypeInfo::Data(
		"mixi"
		, L"アンケート"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT] = AccessTypeInfo::Data(
		"mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_EVENT_MEMBER] = AccessTypeInfo::Data(
		"mixi"
		, L"イベント参加者一覧"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_ADDDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記投稿"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_IMAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"画像"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_MOVIE] = AccessTypeInfo::Data(
		"mixi"
		, L"動画"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_DOWNLOAD] = AccessTypeInfo::Data(
		"mixi"
		, L"ダウンロード"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PROFILE] = AccessTypeInfo::Data(
		"mixi"
		, L"プロフィール"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュニティ"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_PLAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"汎用URL"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,POST 系
	m_map[ACCESS_POST_CONFIRM_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ返信（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ返信（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"新規メッセージ（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"新規メッセージ（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_CONFIRM_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記投稿（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[ACCESS_POST_ENTRY_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- Twitter 系
	type = ACCESS_TWITTER_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"タイムライン"
		, REQUEST_METHOD_POST// タイムライン取得をPOSTにしてみる
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"発言";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日付>>";

	type = ACCESS_TWITTER_FAVORITES;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/favorites.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"発言";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日付>>";

	type = ACCESS_TWITTER_DIRECT_MESSAGES;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		);
	m_map[type].defaultCategoryURL = L"http://twitter.com/direct_messages.xml";
	m_map[type].bodyHeaderCol1Type  = BODY_INDICATE_TYPE_BODY;
	m_map[type].bodyHeaderCol2TypeA = BODY_INDICATE_TYPE_NAME;
	m_map[type].bodyHeaderCol2TypeB = BODY_INDICATE_TYPE_DATE;
	m_map[type].bodyHeaderCol1Name  = L"メッセージ";
	m_map[type].bodyHeaderCol2NameA = L"名前>>";
	m_map[type].bodyHeaderCol2NameB = L"日付>>";

	type = ACCESS_TWITTER_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"更新"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_USER;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"Twitter発言"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_NEW_DM;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"メッセージ送信"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_TWITTER_FAVOURINGS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り登録"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FAVOURINGS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り削除"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FRIENDSHIPS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"フォロー登録"
		, REQUEST_METHOD_POST
		);

	type = ACCESS_TWITTER_FRIENDSHIPS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		"Twitter"
		, L"フォロー解除"
		, REQUEST_METHOD_POST
		);

	//------------------------------------------------------------------
	//--- その他
	m_map[ACCESS_INVALID] = AccessTypeInfo::Data(
		""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);

	return true;
}