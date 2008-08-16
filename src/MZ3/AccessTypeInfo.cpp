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
	ACCESS_TYPE type;

	//------------------------------------------------------------------
	//--- グループ系
	type = ACCESS_GROUP_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"コミュニティG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "COMMUNITY";

	type = ACCESS_GROUP_MESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"メッセージG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MESSAGE";

	type = ACCESS_GROUP_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"日記G"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MYDIARY";

	type = ACCESS_GROUP_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"ニュースG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "NEWS";

	type = ACCESS_GROUP_OTHERS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"その他G"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "OTHERS";

	type = ACCESS_GROUP_TWITTER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "TWITTER";

	//------------------------------------------------------------------
	//--- mixi,リスト系
	type = ACCESS_LIST_INTRO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"紹介文"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_intro.pl
	// url(2) : http://mixi.jp/show_intro.pl?page=2
	m_map[type].cacheFilePattern = L"show_intro{urlparam:page}.html";
	m_map[type].serializeKey = "INTRO";
	m_map[type].defaultCategoryURL = L"show_intro.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"紹介文");

	type = ACCESS_LIST_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_friend_diary.html";
	m_map[type].serializeKey = "DIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_friend_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");

	type = ACCESS_LIST_NEW_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"新着コメント一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_comment.html";
	m_map[type].serializeKey = "NEW_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");

	type = ACCESS_LIST_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"コメント一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_comment.html";
	m_map[type].serializeKey = "COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");

	type = ACCESS_LIST_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"ニュース一覧"
		, REQUEST_METHOD_GET
		);
	// カテゴリ、ページ番号毎に分割
	// url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
	// url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
	// url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
	// url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn
	m_map[type].cacheFilePattern = L"list_news_category_{urlparam:id}_{urlparam:page}.html";
	m_map[type].serializeKey = "NEWS";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"見出し");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"配信時刻>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"配給元>>");

	type = ACCESS_LIST_FAVORITE_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"お気に入りユーザ"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_bookmark.pl
	// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
	// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
	// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community
	m_map[type].cacheFilePattern = L"list_bookmark_{urlparam:kind}_{urlparam:page}.html";
	m_map[type].serializeKey = "FAVORITE";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"最終ログイン>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"自己紹介>>");

	type = ACCESS_LIST_FAVORITE_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"お気に入りコミュ"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_bookmark.pl
	// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
	// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
	// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community
	m_map[type].cacheFilePattern = L"list_bookmark_{urlparam:kind}_{urlparam:page}.html";
	m_map[type].serializeKey = "FAVORITE_COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_bookmark.pl?kind=community";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"コミュニティ");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"人数>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"説明>>");

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"マイミク一覧"
		, REQUEST_METHOD_POST
		);
	// url(1) : http://mixi.jp/ajax_friend_setting.pl?type=thumbnail&mode=get_friends&page=1&sort=nickname
	m_map[type].cacheFilePattern = L"ajax_friend_setting{urlparam:page}.html";
	m_map[type].serializeKey = "FRIEND";
	m_map[type].defaultCategoryURL = L"ajax_friend_setting.pl?type=thumbnail&mode=get_friends&page=1&sort=nickname";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"ログイン時刻");
	m_map[type].requestEncoding = ENCODING_UTF8;	// mixi API => UTF-8

	type = ACCESS_LIST_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"コミュニティ一覧"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_community.pl?id=xxx
	// url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2
	m_map[type].cacheFilePattern = L"list_community{urlparam:page}.html";
	m_map[type].serializeKey = "COMMUNITY";
	m_map[type].defaultCategoryURL = L"list_community.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"コミュニティ");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"人数");

	type = ACCESS_LIST_NEW_BBS_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"コミュコメント記入履歴"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/new_bbs_comment.pl?id=xxx
	// url(2) : http://mixi.jp/new_bbs_comment.pl?id=xxx&page=2
	m_map[type].cacheFilePattern = L"new_bbs_comment{urlparam:page}.html";
	m_map[type].serializeKey = "NEW_BBS_COMMENT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs_comment.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");

	type = ACCESS_LIST_NEW_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"コミュ書込一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"new_bbs_{urlparam:page}.html";
	m_map[type].serializeKey = "BBS";	// 本当は "NEW_BBS" だな
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"new_bbs.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");

	type = ACCESS_LIST_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"トピック一覧"
		, REQUEST_METHOD_GET
		);
	// url : list_bbs.pl?id=xxx
	m_map[type].cacheFilePattern = L"list_bbs_{urlparam:id}.html";
	m_map[type].serializeKey = "TOPIC";	// 本当は "BBS" だな
	m_map[type].bCruiseTarget = true;
	// 動的に生成されるためデフォルトURLは不要
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付");

	type = ACCESS_LIST_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_diary.html";
	m_map[type].serializeKey = "MYDIARY";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_diary.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時");

	type = ACCESS_LIST_FOOTSTEP;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"足あと"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"tracks.xml";
	m_map[type].requestEncoding = ENCODING_UTF8;	// mixi API => UTF-8
	m_map[type].serializeKey = "FOOTSTEP";
	m_map[type].defaultCategoryURL = L"http://mixi.jp/atom/tracks/r=2/member_id={owner_id}";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"時刻");

	type = ACCESS_LIST_MESSAGE_IN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"メッセージ(受信箱)"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_message_in.html";
	m_map[type].serializeKey = "MESSAGE_IN";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"件名");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"差出人>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付>>");

	type = ACCESS_LIST_MESSAGE_OUT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"メッセージ(送信箱)"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"list_message_out.html";
	m_map[type].serializeKey = "MESSAGE_OUT";
	m_map[type].bCruiseTarget = true;
	m_map[type].defaultCategoryURL = L"list_message.pl?box=outbox";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"件名");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"宛先>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付>>");

	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"ブックマーク"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].serializeKey = "BOOKMARK";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ");

	type = ACCESS_LIST_CALENDAR;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"カレンダー"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"show_calendar.html";
	m_map[type].serializeKey = "CALENDAR";
	m_map[type].defaultCategoryURL = L"show_calendar.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付");

	//------------------------------------------------------------------
	//--- mixi,一般
	type = ACCESS_LOGIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"ログイン"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"login.html";

	// トップページ
	type = ACCESS_MAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"メイン"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"home.html";

	type = ACCESS_DIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"日記内容"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
	// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:id}.html";

	type = ACCESS_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"ニュース"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
	m_map[type].cacheFilePattern = L"news\\{urlparam:media_id}\\{urlparam:id}.html";

	type = ACCESS_BBS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"コミュ書込"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_bbs.pl?id=9360823&comm_id=1198460
	// url(2) : http://mixi.jp/view_bbs.pl?page=4&comm_id=1198460&id=9360823
	m_map[type].cacheFilePattern = L"bbs\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_ENQUETE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"アンケート"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
	m_map[type].cacheFilePattern = L"enquete\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT_JOIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";

	type = ACCESS_EVENT_MEMBER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// v0.9.1 時点ではReportViewの[ページ]からの遷移のみサポート
		, "mixi"
		, L"イベント参加者一覧"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_MYDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"日記"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
	// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:id}.html";

	type = ACCESS_MESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_message.pl?id=xxx&box=inbox
	// url(2) : http://mixi.jp/view_message.pl?id=xxx&box=outbox
	m_map[type].cacheFilePattern = L"message\\{urlparam:box}\\{urlparam:id}.html";

	type = ACCESS_IMAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"画像"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_MOVIE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"動画"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_DOWNLOAD;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"ダウンロード"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_PROFILE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"プロフィール"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";

	type = ACCESS_BIRTHDAY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"プロフィール"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";

	type = ACCESS_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// 本来はコミュニティトップページ。v0.9.1 時点ではCMZ3View内でトピック一覧に切り替えている
		, "mixi"
		, L"コミュニティ"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_SCHEDULE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"追加した予定"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_PLAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"汎用URL"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- mixi,POST 系
	type = ACCESS_POST_CONFIRM_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"コメント投稿（確認）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_COMMENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_REPLYMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"メッセージ返信（確認）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_REPLYMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"メッセージ返信（書込）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_NEWMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"新規メッセージ（確認）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_NEWMESSAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"新規メッセージ（書込）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_CONFIRM_NEWDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"日記投稿（確認）"
		, REQUEST_METHOD_GET
		);

	type = ACCESS_POST_ENTRY_NEWDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		);

	//------------------------------------------------------------------
	//--- Twitter 系
	type = ACCESS_TWITTER_FRIENDS_TIMELINE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"タイムライン"
		, REQUEST_METHOD_POST// タイムライン取得をPOSTにしてみる
		);
	// http://twitter.com/statuses/friends_timeline.xml
	// http://twitter.com/statuses/friends_timeline/takke.xml
	// http://twitter.com/statuses/replies.xml
	// http://twitter.com/statuses/user_timeline.xml
	// http://twitter.com/statuses/user_timeline/takke.xml
	// => twitter/user_timeline_takke.xml
	m_map[type].cacheFilePattern = L"twitter\\{urlafter:statuses/:friends_timeline.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FRIENDS_TIMELINE";
	m_map[type].defaultCategoryURL = L"http://twitter.com/statuses/friends_timeline.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"発言");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"日付>>");

	type = ACCESS_TWITTER_FAVORITES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"お気に入り"
		, REQUEST_METHOD_GET
		);
	// http://twitter.com/favorites.xml
	// http://twitter.com/favorites/takke.xml
	// => twitter/favorites_takke.xml
	m_map[type].cacheFilePattern = L"twitter\\favorites{urlafter:favorites/:.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_FAVORITES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/favorites.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"発言");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"日付>>");

	type = ACCESS_TWITTER_DIRECT_MESSAGES;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "Twitter"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		);
	// http://twitter.com/direct_messages.xml
	// => twitter/recv.xml
	// http://twitter.com/direct_messages/sent.xml
	// => twitter/sent.xml
	m_map[type].cacheFilePattern = L"twitter\\{urlafter:direct_messages/:recv.xml}";
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8
	m_map[type].serializeKey = "TWITTER_DIRECT_MESSAGES";
	m_map[type].defaultCategoryURL = L"http://twitter.com/direct_messages.xml";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"メッセージ");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"日付>>");

	type = ACCESS_TWITTER_UPDATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"更新"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_USER;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "Twitter"
		, L"Twitter発言"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_NEW_DM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"メッセージ送信"
		, REQUEST_METHOD_GET
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FAVOURINGS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"お気に入り登録"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FAVOURINGS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"お気に入り削除"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FRIENDSHIPS_CREATE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"フォロー登録"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	type = ACCESS_TWITTER_FRIENDSHIPS_DESTROY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "Twitter"
		, L"フォロー解除"
		, REQUEST_METHOD_POST
		);
	m_map[type].requestEncoding = ENCODING_UTF8;	// Twitter API => UTF-8

	//------------------------------------------------------------------
	//--- mixi echo 関連
	type = ACCESS_MIXI_RECENT_ECHO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"みんなのエコー"
		, REQUEST_METHOD_GET
		);
	// http://mixi.jp/recent_echo.pl
	// => recent_echo.html
	m_map[type].cacheFilePattern = L"recent_echo.html";
	m_map[type].serializeKey = "MIXI_RECENT_ECHO";
	m_map[type].defaultCategoryURL = L"recent_echo.pl";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"発言");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"日付>>");

	// add_echo.pl
	type = ACCESS_MIXI_ADD_ECHO;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"書き込む"
		, REQUEST_METHOD_GET
		);


	//------------------------------------------------------------------
	//--- RSS Reader
	type = ACCESS_RSS_READER_FEED;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "RSS"
		, L"RSS Feed"
		, REQUEST_METHOD_GET
		);
	// http://hogehoge.mz3.jp/hogefuga/
	// => hogehoge.mz3.jp_hogefuga_.rss
	m_map[type].cacheFilePattern = L"rss\\{urlafter://}.rss";
	m_map[type].requestEncoding = ENCODING_UTF8;
	m_map[type].serializeKey = "RSS_FEED";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1  = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY,  L"内容>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付>>");

	type = ACCESS_RSS_READER_ITEM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "RSS"
		, L"RSS Item"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].requestEncoding = ENCODING_UTF8;

	type = ACCESS_RSS_READER_AUTO_DISCOVERY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "RSS"
		, L"RSS AutoDiscovery"
		, REQUEST_METHOD_GET
		);
	// no-cache
//	m_map[type].cacheFilePattern = L"rss\\{urlafter://}.rss";
	m_map[type].requestEncoding = ENCODING_NOCONVERSION;
	m_map[type].serializeKey = "RSS_AUTO_DISCOVERY";

	//------------------------------------------------------------------
	//--- その他
	type = ACCESS_INVALID;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_INVALID
		, ""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);

	// TODO 必須項目のテスト

	return true;
}