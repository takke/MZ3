// MZ3.h : MZ3 アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH のこのファイルをインクルードする前に、'stdafx.h' をインクルードします"
#endif

#include "resourceppc.h"
#include "OptionMng.h"
#include "BookmarkMng.h"
#include "LoginMng.h"
#include "IniFile.h"
#include "InetAccess.h"
#include "GroupItem.h"
#include "Mz3GroupData.h"
#include "CategoryItem.h"
#include "SimpleLogger.h"

// ログ出力用マクロ
#define MZ3LOGGER_FATAL(msg)	SIMPLELOGGER_FATAL(theApp.m_logger,msg)
#define MZ3LOGGER_ERROR(msg)	SIMPLELOGGER_ERROR(theApp.m_logger,msg)
#define MZ3LOGGER_INFO(msg)		SIMPLELOGGER_INFO (theApp.m_logger,msg)
#define MZ3LOGGER_DEBUG(msg)	SIMPLELOGGER_DEBUG(theApp.m_logger,msg)
#define MZ3LOGGER_TRACE(msg)	SIMPLELOGGER_TRACE(theApp.m_logger,msg)

#define TOOLBAR_HEIGHT 24

/// 現行のWM5デバイスで利用されている解像度の列挙。(Phone editionは除く)
enum ScreenResolution { 
	SR_QVGA,			///< 320x240
	SR_SQUARE240,		///< 240x240
	SR_VGA,				///< 640x480
};

class CWriteView;
class CMZ3View;
class CReportView;

// CMZ3App:
// このクラスの実装については、MZ3.cpp を参照してください。
//

class CMZ3App : public CWinApp
{
public:
	/// MZ3 で利用しているファイルのパス
	class FilePath {
	public:
		CString		mz3logfile;		///< MZ3 のログファイル
		CString		temphtml;		///< HTML 用一時ファイル(SJIS)のパス
		CString		logfile;		///< ログファイル（INIファイル）のパス
		CString		inifile;		///< オプション用INIファイルのパス
		CString		groupfile;		///< グループ定義ファイル（INIファイル）のパス
		CString		helpfile;		///< Readme.txt のパス
		CString		historyfile;	///< ChangeLog.txt のパス

		CString		groupBgImage;	///< グループリスト背景画像のパス
		CString		categoryBgImage;///< カテゴリリスト背景画像のパス
		CString		bodyBgImage;	///< ボディリスト背景画像のパス

		CString		reportBgImage;	///< レポートリスト背景画像のパス

		// フォルダ
		CString		downloadFolder;	///< ダウンロード済みファイル用フォルダのパス
		CString		logFolder;		///< ログ用共通フォルダのパス

		CString		imageFolder;	///< 画像のダウンロード済みファイル用フォルダのパス
		CString		diaryFolder;	///< 日記のログ用フォルダのパス
		CString		bbsFolder;		///< コミュニティ書き込みのログ用フォルダのパス
		CString		enqueteFolder;	///< アンケートのログ用フォルダのパス
		CString		eventFolder;	///< イベントのログ用フォルダのパス
		CString		messageFolder;	///< メッセージのログ用フォルダのパス
		CString		newsFolder;		///< ニュースのログ用フォルダのパス

		void init();
		void init_logpath();
	};

public:
	CMZ3App();
	~CMZ3App();

	// オーバーライド
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// 実装
public:
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

private:
	DWORD				m_dpi;					///< DPI
	ScreenResolution	m_currentDisplayMode;	///< 解像度
public:

	//--- ロガー
	CSimpleLogger		m_logger;				///< ログ出力オブジェクト

	//--- UI
	CFont				m_font;					///< 共通フォント
	CMZ3View*			m_pMainView;			///< メインビュー
	CReportView*		m_pReportView;			///< レポートビュー
	CWriteView*			m_pWriteView;			///< 書き込みビュー

	int					m_newMessageCount;		///< 新着メッセージ数
	int					m_newCommentCount;		///< 新着コメント数

	FilePath			m_filepath;				///< MZ3 で利用しているファイルパス群

	option::Option		m_optionMng;			///< オプションデータ
	option::Login		m_loginMng;				///< ログインデータ
	option::Bookmark	m_bookmarkMng;			///< ブックマークデータ

	inifile::IniFile	m_logfile;				///< ログ用 INI ファイル

	//--- mixi データ

	Mz3GroupData		m_root;					///< mixi 用データのルート要素

	//--- 通信系
	CMixiData			m_mixiBeforeRelogin;	///< 再ログイン前の mixi オブジェクト
	ACCESS_TYPE			m_accessType;			///< 現在のアクセス種別

	CMixiData			m_mixi4recv;			///< 受信時に利用する mixi データ
	CInetAccess			m_inet;					///< ネット接続オブジェクト

private:
	BOOL				m_reConnect;			///< 再接続フラグ

public:
	void ChangeView(CView*);
	CString GetAppDirPath();

	void SetReConnect(BOOL value) { m_reConnect = value; };
	BOOL IsReConnect() { return m_reConnect; }

	CString MakeLoginUrl( LPCTSTR nextUrl=L"/home.pl" );
	CString MakeLoginUrlForMixiMobile( LPCTSTR nextUrl );

	BOOL EnableCommandBarButton( int nID, BOOL bEnable );
public:
	bool MakeNewFont( CFont* pBaseFont, int fontHeight, LPCTSTR fontFace );
	void ShowMixiData( CMixiData* data );

	ScreenResolution GetDisplayMode() { return m_currentDisplayMode; }
	DWORD GetDPI() { return m_dpi; }
	int GetInfoRegionHeight( int fontHeight );
};

extern CMZ3App theApp;
