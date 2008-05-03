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
	// グループ系
	m_map[ACCESS_GROUP_COMMUNITY] = AccessTypeInfo::Data(
		""
		, L"コミュニティG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_MESSAGE] = AccessTypeInfo::Data(
		""
		, L"メッセージG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_MYDIARY] = AccessTypeInfo::Data(
		""
		, L"日記G"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_NEWS] = AccessTypeInfo::Data(
		""
		, L"ニュースG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_OTHERS] = AccessTypeInfo::Data(
		""
		, L"その他G"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_GROUP_TWITTER] = AccessTypeInfo::Data(
		""
		, L"TwitterG"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_LOGIN] = AccessTypeInfo::Data(
		"mixi"
		, L"ログイン"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"メイン"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記内容"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"ニュース"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュ書込"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_ENQUETE] = AccessTypeInfo::Data(
		"mixi"
		, L"アンケート"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_EVENT] = AccessTypeInfo::Data(
		"mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_EVENT_MEMBER] = AccessTypeInfo::Data(
		"mixi"
		, L"イベント参加者一覧"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_ADDDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記投稿"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_IMAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"画像"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_MOVIE] = AccessTypeInfo::Data(
		"mixi"
		, L"動画"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_DOWNLOAD] = AccessTypeInfo::Data(
		"mixi"
		, L"ダウンロード"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_PROFILE] = AccessTypeInfo::Data(
		"mixi"
		, L"プロフィール"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュニティ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_PLAIN] = AccessTypeInfo::Data(
		"mixi"
		, L"汎用URL"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_INTRO] = AccessTypeInfo::Data(
		"mixi"
		, L"紹介文"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_DIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"新着コメント一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEWS] = AccessTypeInfo::Data(
		"mixi"
		, L"ニュース一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FAVORITE] = AccessTypeInfo::Data(
		"mixi"
		, L"お気に入り"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FRIEND] = AccessTypeInfo::Data(
		"mixi"
		, L"マイミク一覧"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_COMMUNITY] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュニティ一覧"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_BBS_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュコメント記入履歴"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_NEW_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"コミュ書込一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_BBS] = AccessTypeInfo::Data(
		"mixi"
		, L"トピック一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MYDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記一覧"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_FOOTSTEP] = AccessTypeInfo::Data(
		"mixi"
		, L"足あと"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MESSAGE_IN] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ(受信箱)"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_MESSAGE_OUT] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ(送信箱)"
		, REQUEST_METHOD_GET
		, true	// bCruiseTarget
		);
	m_map[ACCESS_LIST_BOOKMARK] = AccessTypeInfo::Data(
		"mixi"
		, L"ブックマーク"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_LIST_CALENDAR] = AccessTypeInfo::Data(
		"mixi"
		, L"カレンダー"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	// POST 系
	m_map[ACCESS_POST_CONFIRM_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（確認）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_COMMENT] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ返信（確認）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_REPLYMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"メッセージ返信（書込）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"新規メッセージ（確認）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_NEWMESSAGE] = AccessTypeInfo::Data(
		"mixi"
		, L"新規メッセージ（書込）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_CONFIRM_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"日記投稿（確認）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_POST_ENTRY_NEWDIARY] = AccessTypeInfo::Data(
		"mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	// Twitter 系
	m_map[ACCESS_TWITTER_FRIENDS_TIMELINE] = AccessTypeInfo::Data(
		"Twitter"
		, L"タイムライン"
		, REQUEST_METHOD_POST// タイムライン取得をPOSTにしてみる
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FAVORITES] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_DIRECT_MESSAGES] = AccessTypeInfo::Data(
		"Twitter"
		, L"メッセージ"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_UPDATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"更新"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_USER] = AccessTypeInfo::Data(
		"Twitter"
		, L"Twitter発言"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_NEW_DM] = AccessTypeInfo::Data(
		"Twitter"
		, L"メッセージ送信"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_FAVOURINGS_CREATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り登録"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FAVOURINGS_DESTROY] = AccessTypeInfo::Data(
		"Twitter"
		, L"お気に入り削除"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);

	m_map[ACCESS_TWITTER_FRIENDSHIPS_CREATE] = AccessTypeInfo::Data(
		"Twitter"
		, L"フォロー登録"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);
	m_map[ACCESS_TWITTER_FRIENDSHIPS_DESTROY] = AccessTypeInfo::Data(
		"Twitter"
		, L"フォロー解除"
		, REQUEST_METHOD_POST
		, false	// bCruiseTarget
		);

	m_map[ACCESS_INVALID] = AccessTypeInfo::Data(
		""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		, false	// bCruiseTarget
		);

	return true;
}