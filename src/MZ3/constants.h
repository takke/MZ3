/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
/**
 * 各種定数
 */
#pragma once

/// アプリケーション名
#ifdef BT_MZ3
# ifdef WINCE
#  define	MZ3_APP_NAME				L"MZ3"
# else
#  define	MZ3_APP_NAME				L"MZ4"
# endif
#endif

#ifdef BT_TKTW
# define	MZ3_APP_NAME				L"TkTweets"
#endif

// ヘルプファイルパス
#ifndef WINCE
# define MZ4_MANUAL_URL				L"http://mz3.jp/manual_mz4/"
#endif

// Window メッセージ

/// データ取得完了（正常終了）
#define WM_MZ3_GET_END				WM_APP + 2 
/// データ取得完了（バイナリデータ取得完了）
#define WM_MZ3_GET_END_BINARY		WM_APP + 20
/// データ取得完了（エラー発生）
#define WM_MZ3_GET_ERROR			WM_APP + 4
/// データ取得完了（中断）
#define WM_MZ3_GET_ABORT			WM_APP + 6
/// 
#define WM_MZ3_ACCESS_INFORMATION	WM_APP + 8
/// ビュー変更
#define WM_MZ3_CHANGE_VIEW			WM_APP + 10
/// 再読込（投稿後など）
#define WM_MZ3_RELOAD				WM_APP + 11
/// データ送信完了（入力画面取得用送信処理）
#define WM_MZ3_POST_ENTRY_END		WM_APP + 12
/// データ送信完了（確認用送信処理）
#define WM_MZ3_POST_CONFIRM_END		WM_APP + 13
/// データ送信完了（正常終了）
#define	WM_MZ3_POST_END				WM_APP + 14
/// データ送信完了（中断）
#define WM_MZ3_POST_ABORT			WM_APP + 18
/// データ受信量
#define WM_MZ3_ACCESS_LOADED		WM_APP + 19

/// コメント移動
#define WM_MZ3_MOVE_DOWN_LIST		WM_APP + 21
#define WM_MZ3_MOVE_UP_LIST			WM_APP + 22

/// コメントリストのアイテム数を取得
#define WM_MZ3_GET_LIST_ITEM_COUNT	WM_APP + 23

/// 中断
#define WM_MZ3_ABORT				WM_APP + 26
/// 画面再構成
#define WM_MZ3_FIT					WM_APP + 40
/// ビュー非表示
#define WM_MZ3_HIDE_VIEW			WM_APP + 41

// タスクトレイ関連
#ifndef WINCE
#define WM_TRAYICONMESSAGE			WM_APP +100
#endif

/**
 * アクセスタイプ
 */
enum ACCESS_TYPE {
	ACCESS_INVALID = -1,		///< 不正なアクセス種別（初期値=アクセス種別未設定）

	//----------------------------------------------
	// mixi 系
	//----------------------------------------------
#ifdef BT_MZ3
	ACCESS_LOGIN=0,				///< ログイン
	ACCESS_MAIN,				///< メイン(home.pl)
	ACCESS_DIARY,				///< 日記内容(view_diary.pl)
	ACCESS_NEIGHBORDIARY,		///< 日記内容(neighbor_diary.pl)	//MOriyama追加 次の日記、前の日記
	ACCESS_BBS,					///< コミュニティ内容(view_bbs.pl)
	ACCESS_ENQUETE,				///< アンケート(view_enquete.pl)
	ACCESS_EVENT,				///< イベント(view_event.pl)
	ACCESS_EVENT_JOIN,			///< 参加イベント(view_event.pl)  //MOriyama追加
	ACCESS_MYDIARY,				///< 自分の日記内容(view_diary.pl)
	ACCESS_MESSAGE,				///< メッセージ(view_message.pl)
	ACCESS_NEWS,				///< ニュース内容(view_news.pl)
	ACCESS_PROFILE,				///< 個人ページ(show_friend.pl)
	ACCESS_BIRTHDAY,			///< 誕生日個人ページ(show_friend.pl)  //MOriyama追加
	ACCESS_COMMUNITY,			///< コミュニティページ(view_community.pl)
	ACCESS_MOVIE,				///< 動画
	ACCESS_SCHEDULE,			///< 追加した予定  //MOriyama追加
//	ACCESS_MIXI_VOICE_USER,		///< ボイスのオブジェクト
#endif
	ACCESS_IMAGE,				///< 画像

	//----------------------------------------------
	// 汎用
	//----------------------------------------------
	ACCESS_DOWNLOAD,			///< 汎用ダウンロード
	ACCESS_PLAIN,				///< 汎用テキストデータ

	//--- GROUP 系：グループタブの項目。
	ACCESS_GROUP_GENERAL,		///< グループ 一般

	//--- LIST 系：カテゴリリストの項目。
#ifdef BT_MZ3
//	ACCESS_LIST_MYDIARY,			///< 自分の日記一覧
	ACCESS_LIST_NEW_COMMENT,		///< 最新コメント一覧
	ACCESS_LIST_NEW_BBS,			///< コミュニティ書き込み一覧(new_bbs.pl)
//	ACCESS_LIST_MESSAGE_IN,			///< メッセージ一覧（受信箱）
//	ACCESS_LIST_MESSAGE_OUT,		///< メッセージ一覧（受信箱）
//	ACCESS_LIST_FOOTSTEP,			///< 足跡
//	ACCESS_LIST_COMMENT,			///< コメント一覧
//	ACCESS_LIST_NEWS,				///< ニュース一覧
	ACCESS_LIST_FAVORITE_USER,		///< お気に入りユーザ
	ACCESS_LIST_FAVORITE_COMMUNITY,	///< お気に入りコミュ
	ACCESS_LIST_FRIEND,				///< マイミク一覧
//	ACCESS_LIST_COMMUNITY,			///< コミュニティ一覧
//	ACCESS_LIST_INTRO,				///< 紹介文(show_intro.pl)
//	ACCESS_LIST_BBS,				///< トピック一覧(list_bbs.pl)
	ACCESS_LIST_NEW_BBS_COMMENT,	///< コミュニティコメント記入履歴
//	ACCESS_LIST_CALENDAR,			///< カレンダー  //icchu追加
#endif
	ACCESS_LIST_BOOKMARK,			///< ブックマーク

	//--- mixi ボイス 関連
#ifdef BT_MZ3
	ACCESS_MIXI_RECENT_VOICE,			///< みんなのエコー：recent_voice.pl
	ACCESS_MIXI_ADD_VOICE,				///< エコー書き込む：add_voice.pl
	ACCESS_MIXI_ADD_VOICE_REPLY,		///< エコー返信：add_voice.pl
#endif

	//--- POST 系：POST 時の進捗メッセージ用。CMixiData として保持することはない。
#ifdef BT_MZ3
	ACCESS_POST_COMMENT_CONFIRM,		///< コメント投稿（確認画面）
	ACCESS_POST_COMMENT_REGIST,			///< コメント投稿（書き込み画面）
	ACCESS_POST_NEWDIARY_CONFIRM,		///< 日記投稿（確認画面）
	ACCESS_POST_NEWDIARY_REGIST,		///< 日記投稿（書き込み画面）
	ACCESS_POST_REPLYMESSAGE_ENTRY,		///< メッセージ返信（入力画面）
	ACCESS_POST_REPLYMESSAGE_CONFIRM,	///< メッセージ返信（確認画面）
	ACCESS_POST_REPLYMESSAGE_REGIST,	///< メッセージ返信（書き込み画面）
	ACCESS_POST_NEWMESSAGE_ENTRY,		///< 新規メッセージ（入力画面）
	ACCESS_POST_NEWMESSAGE_CONFIRM,		///< 新規メッセージ（確認画面）
	ACCESS_POST_NEWMESSAGE_REGIST,		///< 新規メッセージ（書き込み画面）
#endif

	//----------------------------------------------
	// Twitter
	//----------------------------------------------
	ACCESS_TWITTER_UPDATE,				///< [POST] ステータス更新
	ACCESS_TWITTER_USER,				///< 各発言要素

	//----------------------------------------------
	// RSS
	//----------------------------------------------
#ifdef BT_MZ3
	//--- RSS Reader
	ACCESS_RSS_READER_FEED,				///< RSS Reader Feed
	ACCESS_RSS_READER_ITEM,				///< RSS Reader Item
	ACCESS_RSS_READER_AUTO_DISCOVERY,	///< RSS Reader; RSS AutoDiscovery 用Item
#endif

	//--- その他
	ACCESS_HELP,						///< Readme.txt 専用の特殊タイプ
	ACCESS_ERRORLOG,					///< mz3log.txt 専用の特殊タイプ
	ACCESS_SOFTWARE_UPDATE_CHECK,		///< バージョンチェック

	//--- API登録
	ACCESS_TYPE_MZ3_SCRIPT_BASE = 1000,	///< これ以降の番号は全て MZ3 Script API 登録によるアクセス種別。
										///< 内容は実行時に決定する。
};

/**
 * ページの取得タイプ
 */
#ifdef BT_MZ3
enum GETPAGE_TYPE
{
	GETPAGE_ALL = 0,		///< 全件取得
	GETPAGE_LATEST20 = 1,	///< 最新の２０件取得
};
#endif

/**
 * WriteView の処理種別
 *
 * → Lua 化のために ACCESS_TYPE に統一。Entery/Confirm のアクセス種別を用いる。
 */
#ifdef BT_MZ3
typedef ACCESS_TYPE WRITEVIEW_TYPE;
#define WRITEVIEW_TYPE_COMMENT		ACCESS_POST_COMMENT_CONFIRM		///< コメント投稿処理（日記、コミュニティ共用？）
#define WRITEVIEW_TYPE_NEWDIARY		ACCESS_POST_NEWDIARY_CONFIRM	///< 日記投稿処理
#define	WRITEVIEW_TYPE_REPLYMESSAGE	ACCESS_POST_REPLYMESSAGE_ENTRY	///< メッセージ返信処理
#define WRITEVIEW_TYPE_NEWMESSAGE	ACCESS_POST_NEWMESSAGE_ENTRY	///< 新規メッセージの送信処理
#endif

/// 送信する Content-Type の種別
typedef LPCTSTR CONTENT_TYPE;

///< 初期値
#define CONTENT_TYPE_INVALID L""

/// Content-Type: multipart/form-data
#define CONTENT_TYPE_MULTIPART L"multipart/form-data; boundary=---------------------------7d62ee108071e"
/// Content-Type: application/x-www-form-urlencoded
#define CONTENT_TYPE_FORM_URLENCODED L"application/x-www-form-urlencoded"

/// コンパイル失敗メッセージ
#define FAILED_TO_COMPILE_REGEX_MSG		L"コンパイル失敗"

/// Smartphone/Standard Edition の場合のツールバー調整分
#define MZ3_TOOLBAR_HEIGHT	26

/// 無限ループ防止用ループ最大値
#define MZ3_INFINITE_LOOP_MAX_COUNT	100000
