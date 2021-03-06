/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "locale.h"
#include "AccessTypeInfo.h"

/// 初期化
bool AccessTypeInfo::init()
{
	ACCESS_TYPE type;

	m_map.clear();

	//------------------------------------------------------------------
	//--- グループ系
	//------------------------------------------------------------------
	type = ACCESS_GROUP_GENERAL;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_GROUP
		, ""
		, L"その他G"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "OTHERS";

	//------------------------------------------------------------------
	//--- mixi,リスト系
	//------------------------------------------------------------------
#ifdef BT_MZ3
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"最終ログイン>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"追加日>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1\t(%2)";
	m_map[type].bodyIntegratedLinePattern2 = L"%3";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"コミュニティ");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"人数>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"説明>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1\t(%2)";
	m_map[type].bodyIntegratedLinePattern2 = L"%3";

	type = ACCESS_LIST_FRIEND;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"マイミク一覧"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/list_friend_simple.pl
	m_map[type].cacheFilePattern = L"list_friend_simple{urlparam:page}.html";
	m_map[type].serializeKey = "FRIEND";
	m_map[type].defaultCategoryURL = L"http://mixi.jp/list_friend_simple.pl";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"ログイン時刻");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日時>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2\t(%3)";
	m_map[type].bodyIntegratedLinePattern2 = L"%1";

/*
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"時刻");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";
*/
	type = ACCESS_LIST_BOOKMARK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"ブックマーク"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].serializeKey = "BOOKMARK";
	m_map[type].defaultCategoryURL = L"";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"トピック");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME,  L"コミュニティ");
	m_map[type].bodyIntegratedLinePattern1 = L"%1";
	m_map[type].bodyIntegratedLinePattern2 = L"%2";
#endif

	//------------------------------------------------------------------
	//--- mixi,一般
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_LOGIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"ログイン"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"login.html";
	m_map[type].serializeKey = "MIXI_LOGIN";

	// トップページ
	type = ACCESS_MAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"メイン"
		, REQUEST_METHOD_GET
		);
	m_map[type].cacheFilePattern = L"home.html";
	m_map[type].serializeKey = "MIXI_HOME";

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
	m_map[type].serializeKey = "MIXI_DIARY";

	type = ACCESS_NEIGHBORDIARY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"日記内容"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/neighbor_diary.pl?id=xxx&owner_id=yyy&direction=prev&new_id=zzz
	// url(2) : http://mixi.jp/neighbor_diary.pl?id=xxx&owner_id=yyy&direction=next&new_id=zzz
	m_map[type].cacheFilePattern = L"diary\\{urlparam:owner_id}\\{urlparam:new_id}.html";
	m_map[type].serializeKey = "MIXI_NEIGHBORDIARY";

	type = ACCESS_NEWS;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"ニュース"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
	m_map[type].cacheFilePattern = L"news\\{urlparam:media_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_NEWS";

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
	m_map[type].serializeKey = "MIXI_BBS";

	type = ACCESS_ENQUETE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"アンケート"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
	m_map[type].cacheFilePattern = L"enquete\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_ENQUETE";

	type = ACCESS_EVENT;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_EVENT";

	type = ACCESS_EVENT_JOIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"イベント"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
	m_map[type].cacheFilePattern = L"event\\{urlparam:comm_id}\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_EVENT_JOIN";

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
	m_map[type].serializeKey = "MIXI_MYDIARY";

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
	m_map[type].serializeKey = "MIXI_MESSAGE";

	type = ACCESS_MOVIE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"動画"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_MOVIE";

	type = ACCESS_PROFILE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"プロフィール"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_PROFILE";

	type = ACCESS_BIRTHDAY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"誕生日"
		, REQUEST_METHOD_GET
		);
	// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
	m_map[type].cacheFilePattern = L"profile\\{urlparam:id}.html";
	m_map[type].serializeKey = "MIXI_BIRTHDAY";

	type = ACCESS_COMMUNITY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY		// 本来はコミュニティトップページ。v0.9.1 時点ではCMZ3View内でトピック一覧に切り替えている
		, "mixi"
		, L"コミュニティ"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_COMMUNITY";

	type = ACCESS_SCHEDULE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"追加した予定"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_SCHEDULE";
#endif

	type = ACCESS_IMAGE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"画像"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_IMAGE";

	type = ACCESS_DOWNLOAD;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "mixi"
		, L"ダウンロード"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "DOWNLOAD";


	//------------------------------------------------------------------
	//--- mixi,POST 系
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_POST_COMMENT_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"コメント投稿（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_COMMENT_CONFIRM";

	type = ACCESS_POST_COMMENT_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"コメント投稿（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_COMMENT_REGIST";

	type = ACCESS_POST_REPLYMESSAGE_ENTRY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"メッセージ返信（入力）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_ENTRY";

	type = ACCESS_POST_REPLYMESSAGE_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"メッセージ返信（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_CONFIRM";

	type = ACCESS_POST_REPLYMESSAGE_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"メッセージ返信（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_REPLYMESSAGE_REGIST";

	type = ACCESS_POST_NEWMESSAGE_ENTRY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"新規メッセージ（入力）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_ENTRY";

	type = ACCESS_POST_NEWMESSAGE_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"新規メッセージ（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_CONFIRM";

	type = ACCESS_POST_NEWMESSAGE_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"新規メッセージ（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWMESSAGE_REGIST";

	type = ACCESS_POST_NEWDIARY_CONFIRM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"日記投稿（確認）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWDIARY_CONFIRM";

	type = ACCESS_POST_NEWDIARY_REGIST;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"日記投稿（書込）"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "MIXI_POST_NEWDIARY_REGIST";
#endif

	//------------------------------------------------------------------
	//--- mixi echo 関連
	//------------------------------------------------------------------
#ifdef BT_MZ3
	type = ACCESS_MIXI_RECENT_VOICE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_CATEGORY
		, "mixi"
		, L"みんなのボイス"
		, REQUEST_METHOD_GET
		);
	// http://mixi.jp/recent_voice.pl
	// => recent_voice.html
	m_map[type].cacheFilePattern = L"mixi_voice\\{urlafter:mixi.jp/:recent_voice}.html";
	m_map[type].serializeKey = "MIXI_RECENT_ECHO";
	m_map[type].defaultCategoryURL = L"recent_voice.pl";
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY, L"発言");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_NAME, L"名前>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE, L"日付>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%2 \t%3";	// "名前  (日付)"
	m_map[type].bodyIntegratedLinePattern2 = L"%1";		// "発言"

	// add_voice.pl
	type = ACCESS_MIXI_ADD_VOICE;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"書き込む"
		, REQUEST_METHOD_POST
		);
	m_map[type].serializeKey = "MIXI_ADD_VOICE";

	// add_voice.pl(返信)
	type = ACCESS_MIXI_ADD_VOICE_REPLY;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_POST
		, "mixi"
		, L"書き込む(返信)"
		, REQUEST_METHOD_POST
		);
	m_map[type].serializeKey = "MIXI_ADD_VOICE_REPLY";
#endif

	//------------------------------------------------------------------
	//--- RSS Reader
	//------------------------------------------------------------------
#ifdef BT_MZ3
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
	m_map[type].bodyHeaderCol1 = BodyHeaderColumn(BODY_INDICATE_TYPE_TITLE, L"タイトル");
	m_map[type].bodyHeaderCol2 = BodyHeaderColumn(BODY_INDICATE_TYPE_BODY,  L"内容>>");
	m_map[type].bodyHeaderCol3 = BodyHeaderColumn(BODY_INDICATE_TYPE_DATE,  L"日付>>");
	m_map[type].bodyIntegratedLinePattern1 = L"%1 \t(%3)";	// "タイトル  (日付)"
	m_map[type].bodyIntegratedLinePattern2 = L"%2";			// "内容"

	type = ACCESS_RSS_READER_ITEM;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "RSS"
		, L"RSS Item"
		, REQUEST_METHOD_INVALID
		);
	m_map[type].requestEncoding = ENCODING_UTF8;
	m_map[type].serializeKey = "RSS_ITEM";

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
#endif

	//------------------------------------------------------------------
	//--- その他
	//------------------------------------------------------------------
	type = ACCESS_INVALID;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_INVALID
		, ""
		, L"<invalid>"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "INVALID";

	// バージョンチェック
	type = ACCESS_SOFTWARE_UPDATE_CHECK;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_OTHER
		, "general"
		, L"バージョンチェック"
		, REQUEST_METHOD_GET
		);
	// http://mz3.jp/latest_version.xml
	m_map[type].cacheFilePattern = L"mz3.jp\\latest_version.xml";
	m_map[type].defaultCategoryURL = L"http://mz3.jp/latest_version.xml";
	m_map[type].serializeKey = "MZ3_VERSION_CHECK";

	type = ACCESS_PLAIN;
	m_map[type] = AccessTypeInfo::Data(
		INFO_TYPE_BODY
		, "mixi"
		, L"汎用URL"
		, REQUEST_METHOD_GET
		);
	m_map[type].serializeKey = "PLAIN";

	//------------------------------------------------------------------
	// シリアライズキー → アクセス種別マップ の構築
	//------------------------------------------------------------------
	m_serializeKeyToAccessKeyMap.clear();
	for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		std::string serializeKey = it->second.serializeKey;
		if (!serializeKey.empty()) {
			m_serializeKeyToAccessKeyMap[ serializeKey ] = accessType;
		}
	}

	//------------------------------------------------------------------
	// TODO 必須項目のテスト
	//------------------------------------------------------------------

	//------------------------------------------------------------------
	// Debug モードのとき CSV ダンプする
	//------------------------------------------------------------------
#if defined(_DEBUG) && !defined(WINCE)
	CString tsv_dump_filepath;
	{
		TCHAR path[256];
		memset(path, 0x00, sizeof(TCHAR) * 256);
		GetModuleFileName(NULL, path, 256);
		tsv_dump_filepath = path;	// "...\\MZ3.exe"
		tsv_dump_filepath = tsv_dump_filepath.Left(tsv_dump_filepath.ReverseFind('\\'));
		tsv_dump_filepath += L"\\_access_type_list.csv";
	}

	FILE* fp_tsv = _wfopen(tsv_dump_filepath, L"wt");
	if (fp_tsv!=NULL) {
		fwprintf(fp_tsv, L"ACCESS_TYPE,infoType,serviceType,shortText,requestType,requestEncoding,cacheFilePattern,"
						 L"serializeKey,bCruiseTarget,defaultCategoryURL,bodyHeaderCol1,,bodyHeaderCol2,,bodyHeaderCol3,,"
						 L"bodyIntegratedLinePattern1,bodyIntegratedLinePattern2\n");
		for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			const ACCESS_TYPE& accessType = it->first;
			const Data& data = it->second;

			fwprintf(fp_tsv, L"\"%d\",", accessType);
			fwprintf(fp_tsv, L"\"%s\",", info_type_to_text(data.infoType));
			fwprintf(fp_tsv, L"\"%s\",", CString(data.serviceType.c_str()));
			fwprintf(fp_tsv, L"\"%s\",", data.shortText.c_str());
			fwprintf(fp_tsv, L"\"%s\",", request_method_to_text(data.requestMethod));
			fwprintf(fp_tsv, L"\"%s\",", encoding_to_text(data.requestEncoding));
			fwprintf(fp_tsv, L"\"%s\",", data.cacheFilePattern.c_str());

			//--- グループ系、カテゴリ系のみが持つ項目
			fwprintf(fp_tsv, L"\"%s\",", CString(data.serializeKey.c_str()));

			//--- カテゴリ系のみが持つ項目
			fwprintf(fp_tsv, L"\"%d\",", data.bCruiseTarget ? 1 : 0);
			fwprintf(fp_tsv, L"\"%s\",", data.defaultCategoryURL.c_str());

			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol1.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol1.type));
			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol2.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol2.type));
			fwprintf(fp_tsv, L"\"%s\",", data.bodyHeaderCol3.title.c_str());
			fwprintf(fp_tsv, L"\"%s\",", body_indicate_type_to_text(data.bodyHeaderCol3.type));

			// Excel hack.
			fwprintf(fp_tsv, L"=\"%s\",", data.bodyIntegratedLinePattern1.c_str());
			fwprintf(fp_tsv, L"=\"%s\",", data.bodyIntegratedLinePattern2.c_str());

			fwprintf(fp_tsv, L"\n");
		}
		fclose(fp_tsv);
	}
#endif

	return true;
}