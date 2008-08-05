/**
 * 各種定数
 */
#pragma once

/// アプリケーション名
#ifdef WINCE
#define	MZ3_APP_NAME				L"MZ3"
#else
#define	MZ3_APP_NAME				L"MZ4"
#endif

// CHM ヘルプファイルパス
#ifndef WINCE
#define MZ3_CHM_HELPFILENAME		L"MZ4.chm"
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
/// データ送信完了（確認用送信処理）
#define WM_MZ3_POST_CONFIRM			WM_APP + 12
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

/**
 * アクセスタイプ
 */
enum ACCESS_TYPE {
	ACCESS_INVALID = -1,		///< 不正なアクセス種別（初期値=アクセス種別未設定）
	ACCESS_LOGIN = 0,			///< ログイン
	ACCESS_MAIN,				///< メイン(home.pl)
	ACCESS_DIARY,				///< 日記内容(view_diary.pl)
	ACCESS_BBS,					///< コミュニティ内容(view_bbs.pl)
	ACCESS_ENQUETE,				///< アンケート(view_enquete.pl)
	ACCESS_EVENT,				///< イベント(view_event.pl)
	ACCESS_EVENT_JOIN,			///< 参加イベント(view_event.pl)  //MOriyama追加
	ACCESS_EVENT_MEMBER,		///< イベント参加者一覧(list_event_member.pl)
	ACCESS_MYDIARY,				///< 自分の日記内容(view_diary.pl)
	ACCESS_MESSAGE,				///< メッセージ(view_message.pl)
	ACCESS_NEWS,				///< ニュース内容(view_news.pl)
	ACCESS_PROFILE,				///< 個人ページ(show_friend.pl)
	ACCESS_BIRTHDAY,			///< 誕生日個人ページ(show_friend.pl)  //MOriyama追加
	ACCESS_COMMUNITY,			///< コミュニティページ(view_community.pl)
	ACCESS_IMAGE,				///< 画像
	ACCESS_MOVIE,				///< 動画
	ACCESS_SCHEDULE,			///< 追加した予定  //MOriyama追加
	ACCESS_DOWNLOAD,			///< 汎用ダウンロード
	ACCESS_PLAIN,				///< 汎用テキストデータ

	//--- GROUP 系：グループタブの項目。
	ACCESS_GROUP_MYDIARY,		///< 日記 グループ
	ACCESS_GROUP_COMMUNITY,		///< コミュニティ グループ
	ACCESS_GROUP_MESSAGE,		///< メッセージ グループ
	ACCESS_GROUP_NEWS,			///< ニュースグループ
	ACCESS_GROUP_OTHERS,		///< その他 グループ
	ACCESS_GROUP_TWITTER,		///< Twitter グループ

	//--- LIST 系：カテゴリリストの項目。
	ACCESS_LIST_MYDIARY,			///< 自分の日記一覧
	ACCESS_LIST_DIARY,				///< マイミク最新日記一覧
	ACCESS_LIST_NEW_COMMENT,		///< 最新コメント一覧
	ACCESS_LIST_NEW_BBS,			///< コミュニティ書き込み一覧(new_bbs.pl)
	ACCESS_LIST_MESSAGE_IN,			///< メッセージ一覧（受信箱）
	ACCESS_LIST_MESSAGE_OUT,		///< メッセージ一覧（受信箱）
	ACCESS_LIST_FOOTSTEP,			///< 足跡
	ACCESS_LIST_COMMENT,			///< コメント一覧
	ACCESS_LIST_NEWS,				///< ニュース一覧
	ACCESS_LIST_BOOKMARK,			///< ブックマーク
	ACCESS_LIST_FAVORITE_USER,		///< お気に入りユーザ
	ACCESS_LIST_FAVORITE_COMMUNITY,	///< お気に入りコミュ
	ACCESS_LIST_FRIEND,				///< マイミク一覧
	ACCESS_LIST_COMMUNITY,			///< コミュニティ一覧
	ACCESS_LIST_INTRO,				///< 紹介文(show_intro.pl)
	ACCESS_LIST_BBS,				///< トピック一覧(list_bbs.pl)
	ACCESS_LIST_NEW_BBS_COMMENT,	///< コミュニティコメント記入履歴
	ACCESS_LIST_CALENDAR,			///< カレンダー  //icchu追加

	//--- mixi echo 関連
	ACCESS_MIXI_RECENT_ECHO,			///< みんなのエコー：recent_echo.pl


	//--- POST 系：POST 時の進捗メッセージ用。CMixiData として保持することはない。
	ACCESS_POST_CONFIRM_COMMENT,		///< コメント投稿（確認画面）
	ACCESS_POST_ENTRY_COMMENT,			///< コメント投稿（書き込み画面）
	ACCESS_POST_CONFIRM_REPLYMESSAGE,	///< メッセージ返信（確認画面）
	ACCESS_POST_ENTRY_REPLYMESSAGE,		///< メッセージ返信（書き込み画面）
	ACCESS_POST_CONFIRM_NEWMESSAGE,		///< 新規メッセージ（確認画面）
	ACCESS_POST_ENTRY_NEWMESSAGE,		///< 新規メッセージ（書き込み画面）
	ACCESS_POST_CONFIRM_NEWDIARY,		///< 日記投稿（確認画面）
	ACCESS_POST_ENTRY_NEWDIARY,			///< コメント投稿（書き込み画面）

	//--- Twitter Timeline 系
	ACCESS_TWITTER_FRIENDS_TIMELINE,	///< [GET/POST] friends_timeline.xml
	ACCESS_TWITTER_UPDATE,				///< [POST] ステータス更新
	ACCESS_TWITTER_USER,				///< 各発言要素

	//--- Twitter DM 系
	ACCESS_TWITTER_DIRECT_MESSAGES,		///< [GET/POST] direct_messages.xml
	ACCESS_TWITTER_NEW_DM,				///< [POST] DM送信

	//--- Twitter Favorites 系
	ACCESS_TWITTER_FAVORITES,			///< [GET/POST] お気に入り取得
	ACCESS_TWITTER_FAVOURINGS_CREATE,	///< お気に入り登録
	ACCESS_TWITTER_FAVOURINGS_DESTROY,	///< お気に入りから外す

	//--- Twitter Friend 系
	ACCESS_TWITTER_FRIENDSHIPS_CREATE,	///< add
	ACCESS_TWITTER_FRIENDSHIPS_DESTROY,	///< remove

	//--- RSS Reader
	ACCESS_RSS_READER_FEED,				///< RSS Reader Feed
	ACCESS_RSS_READER_ITEM,				///< RSS Reader Item
	ACCESS_RSS_READER_AUTO_DISCOVERY,	///< RSS Reader; RSS AutoDiscovery 用Item

	//--- その他
	ACCESS_HELP,						///< Readme.txt 専用の特殊タイプ
	ACCESS_ERRORLOG,					///< mz3log.txt 専用の特殊タイプ
};

/**
 * ページの取得タイプ
 */
enum GETPAGE_TYPE
{
	GETPAGE_ALL = 0,		///< 全件取得
	GETPAGE_LATEST20 = 1,	///< 最新の２０件取得
};

/**
 * WriteView の処理種別
 */
enum WRITEVIEW_TYPE {
	WRITEVIEW_TYPE_INVALID  = -1,		///< 初期値
	WRITEVIEW_TYPE_COMMENT  =  0,		///< コメント投稿処理（日記、コミュニティ共用？）
	WRITEVIEW_TYPE_NEWDIARY =  1,		///< 日記投稿処理
	WRITEVIEW_TYPE_REPLYMESSAGE  =  2,	///< メッセージ返信処理
	WRITEVIEW_TYPE_NEWMESSAGE  =  3,	///< 新規メッセージの送信処理
};

/// 送信する Content-Type の種別
enum CONTENT_TYPE {
	CONTENT_TYPE_INVALID = -1,			///< 初期値
	CONTENT_TYPE_MULTIPART = 0,			///< Content-Type: multipart/form-data
	CONTENT_TYPE_FORM_URLENCODED = 1,	///< Content-Type: application/x-www-form-urlencoded
};

/// コンパイル失敗メッセージ
#define FAILED_TO_COMPILE_REGEX_MSG		L"コンパイル失敗"

/// Smartphone/Standard Edition の場合のツールバー調整分
#define MZ3_TOOLBAR_HEIGHT	26

/// 無限ループ防止用ループ最大値
#define MZ3_INFINITE_LOOP_MAX_COUNT	100000