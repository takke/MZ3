/**
 * 各種定数
 */
#pragma once

/// バージョン文字列
#define MZ3_VERSION_TEXT			L"MZ3.i Version 0.8.0.0 Beta7"

/// リビジョン番号
#define MZ3_SVN_REVISION			L"$Rev$" 

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

/// 中断
#define WM_MZ3_ABORT				WM_APP + 26
/// 画面再構成
#define WM_MZ3_FIT					WM_APP + 40

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
	ACCESS_MYDIARY,				///< 自分の日記内容(view_diary.pl)
	ACCESS_MESSAGE,				///< メッセージ(view_message.pl)
	ACCESS_NEWS,				///< ニュース内容(view_news.pl)
	ACCESS_PROFILE,				///< 個人ページ(show_friend.pl) （v0.6.2 現在、ただのURLリンク）
	ACCESS_COMMUNITY,			///< コミュニティページ(view_community.pl)
	ACCESS_ADDDIARY,			///< 日記投稿
	ACCESS_IMAGE,				///< 画像
	ACCESS_DOWNLOAD,			///< ダウンロード

	//--- GROUP 系：グループタブの項目。
	ACCESS_GROUP_MYDIARY,		///< 日記 グループ
	ACCESS_GROUP_COMMUNITY,		///< コミュニティ グループ
	ACCESS_GROUP_MESSAGE,		///< メッセージ グループ
	ACCESS_GROUP_NEWS,			///< ニュースグループ
	ACCESS_GROUP_OTHERS,		///< その他 グループ

	//--- LIST 系：カテゴリリストの項目。
	ACCESS_LIST_MYDIARY,		///< 自分の日記一覧
	ACCESS_LIST_DIARY,			///< マイミク最新日記一覧
	ACCESS_LIST_NEW_COMMENT,	///< 最新コメント一覧
	ACCESS_LIST_NEW_BBS,		///< コミュニティ書き込み一覧(new_bbs.pl)
	ACCESS_LIST_MESSAGE_IN,		///< メッセージ一覧（受信箱）
	ACCESS_LIST_MESSAGE_OUT,	///< メッセージ一覧（受信箱）
	ACCESS_LIST_FOOTSTEP,		///< 足跡
	ACCESS_LIST_COMMENT,		///< コメント一覧
	ACCESS_LIST_NEWS,			///< ニュース一覧
	ACCESS_LIST_BOOKMARK,		///< ブックマーク
	ACCESS_LIST_FAVORITE,		///< お気に入り
	ACCESS_LIST_FRIEND,			///< マイミク一覧
	ACCESS_LIST_COMMUNITY,		///< コミュニティ一覧
	ACCESS_LIST_INTRO,			///< 紹介文(show_intro.pl)
	ACCESS_LIST_BBS,			///< トピック一覧(list_bbs.pl)

	//--- POST 系：POST 時の進捗メッセージ用。CMixiData として保持することはない。
	ACCESS_POST_CONFIRM_COMMENT,		///< コメント投稿（確認画面）
	ACCESS_POST_ENTRY_COMMENT,			///< コメント投稿（書き込み画面）
	ACCESS_POST_CONFIRM_REPLYMESSAGE,	///< メッセージ返信（確認画面）
	ACCESS_POST_ENTRY_REPLYMESSAGE,		///< メッセージ返信（書き込み画面）
	ACCESS_POST_CONFIRM_NEWMESSAGE,		///< 新規メッセージ（確認画面）
	ACCESS_POST_ENTRY_NEWMESSAGE,		///< 新規メッセージ（書き込み画面）
	ACCESS_POST_CONFIRM_NEWDIARY,		///< 日記投稿（確認画面）
	ACCESS_POST_ENTRY_NEWDIARY,			///< コメント投稿（書き込み画面）

	//--- その他
	ACCESS_HELP,				///< Readme.txt 専用の特殊タイプ
};

/**
 * ページの取得タイプ
 */
enum GETPAGE_TYPE
{
	GETPAGE_ALL = 0,		///< 全件取得
	GETPAGE_LATEST10 = 1,	///< 最新の１０件取得
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
