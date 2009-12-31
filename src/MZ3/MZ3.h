/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
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
#include "MZ3BackgroundImage.h"
#include "MZ3SkinInfo.h"
#include "EmojiMap.h"
#include "ImageCacheManager.h"
#include "AccessTypeInfo.h"
#include <hash_map>
#include <hash_set>

#ifndef WINCE
	#include "gdiplus.h"
	using namespace Gdiplus;
#endif

// コンソール用デバッグログ出力用マクロ
#ifdef CONSOLE_DEBUG
# define MZ3_TRACE wprintf
#else
# define MZ3_TRACE __noop
#endif

// ログ出力用マクロ
#define MZ3LOGGER_FATAL(msg)			SIMPLELOGGER_FATAL(theApp.m_logger,msg)
#define MZ3LOGGER_ERROR(msg)			SIMPLELOGGER_ERROR(theApp.m_logger,msg)
#define MZ3LOGGER_INFO(msg)				SIMPLELOGGER_INFO (theApp.m_logger,msg)
#define MZ3LOGGER_DEBUG(msg)			SIMPLELOGGER_DEBUG(theApp.m_logger,msg)
#define MZ3LOGGER_TRACE(msg)			SIMPLELOGGER_TRACE(theApp.m_logger,msg)
#define MZ3LOGGER_IS_DEBUG_ENABLED()	theApp.m_logger.isDebugEnabled()

#define TOOLBAR_HEIGHT 24

/// 現行のWMデバイスで利用されている解像度の列挙。(Phone editionは除く)
enum ScreenResolution { 
	SR_QVGA,			///< 320x240
	SR_SQUARE240,		///< 240x240
	SR_VGA,				///< 640x480
};

class CMZ3View;
class CReportView;
class CWriteView;
class CDownloadView;
class CDetailView;
class CMZ3BackgroundImage;
class MouseGestureManager;
class CHtmlArray;

// CMZ3App:
// このクラスの実装については、MZ3.cpp を参照してください。
//

class CMZ3App : public CWinApp
{
public:
	AccessTypeInfo	m_accessTypeInfo;	///< アクセス種別毎のMZ3/4の振る舞いを定義する情報

	lua_State*		m_luaState;			///< the Lua object


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
		CString		tempdraftfile;	///< 書き込み失敗時の下書きファイル
		CString		emojifile;		///< 絵文字定義ファイル

		CString		skinFolder;		///< スキン用フォルダのパス

		// フォルダ
		CString		downloadFolder;	///< ダウンロード済みファイル用フォルダのパス
		CString		logFolder;		///< ログ用共通フォルダのパス

		CString		imageFolder;	///< 画像のダウンロード済みファイル用フォルダのパス

		CStringArray deleteTargetFolders;	///< 定期クリーンアップ対象フォルダリスト

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

public:
	DWORD				m_dpi;					///< DPI
	ScreenResolution	m_currentDisplayMode;	///< 解像度

	/// プラットフォーム用のフラグ
	struct Platforms {
		bool PocketPC;
		bool Smartphone;
		bool WM2003;		///< Windows Mobile 2003
		bool WM2003_SE;		///< Windows Mobile 2003 SE
		bool WM5_0;			///< Windows Mobile 5
		bool WM6_0;			///< Windows Mobile 6
		bool WM6_1;			///< Windows Mobile 6.1
		bool WM6_5;			///< Windows Mobile 6.5
		bool WM6_5_1;		///< Windows Mobile 6.5 SE (or 6.5.1)

		Platforms()
			: PocketPC(false)
			, Smartphone(false)
			, WM2003(false)
			, WM2003_SE(false)
			, WM5_0(false)
			, WM6_0(false)
			, WM6_1(false)
			, WM6_5(false)
			, WM6_5_1(false)
		{}
	};
	Platforms			m_Platforms;			///< Platform

	bool				m_bProMode;				///< ProMode

public:
	//--- ロガー
	CSimpleLogger		m_logger;				///< ログ出力オブジェクト

	//--- UI
	CFont				m_font;					///< 共通フォント
	CFont				m_fontSmall;			///< 共通フォント(小) : 共通フォントに対する相対指定
	CFont				m_fontBig;				///< 共通フォント(大) : 共通フォントに対する相対指定
	CMZ3View*			m_pMainView;			///< メインビュー
	CReportView*		m_pReportView;			///< レポートビュー
	CWriteView*			m_pWriteView;			///< 書き込みビュー
	CDownloadView*		m_pDownloadView;		///< ダウンロードビュー
	CDetailView*		m_pDetailView;			///< 詳細ビュー

	MouseGestureManager* m_pMouseGestureManager;///< マウスジェスチャ管理

	CMZ3SkinInfo		m_skininfo;				///< スキン情報

	CMZ3BackgroundImage	m_bgImageMainCategoryCtrl;	///< メインビュー、カテゴリコントロールの背景用ビットマップ
	CMZ3BackgroundImage	m_bgImageMainBodyCtrl;		///< メインビュー、ボディコントロールの背景用ビットマップ
	CMZ3BackgroundImage	m_bgImageReportListCtrl;	///< レポートビュー、リストコントロールの背景用ビットマップ

	CBrush				m_brushMainStatusBar;		///< メインビュー、ステータスバーの背景色ブラシ
	CBrush				m_brushMainEdit;			///< メインビュー、エディットコントロールの背景色ブラシ

	FilePath			m_filepath;				///< MZ3 で利用しているファイルパス群

	option::Option		m_optionMng;			///< オプションデータ
	option::Login		m_loginMng;				///< ログインデータ
	option::Bookmark	m_bookmarkMng;			///< ブックマークデータ

	inifile::IniFile	m_readlog;				///< 既読管理ログ用 INI ファイル

	//--- mixi データ
	Mz3GroupData		m_root;					///< mixi 用データのルート要素

	//--- 絵文字マップ
	EmojiMapList		m_emoji;				///< mixi 絵文字マップ

	//--- 画像
	ImageCacheManager	m_imageCache;			///< グローバル画像キャッシュ（16x16）

	stdext::hash_set<std::wstring> m_notFoundFileList;	///< 存在しないファイルの集合
												///< ファイルI/OがWMだとやけに遅いのでキャッシュ化

	//--- 通信系
	bool				m_access;				///< 通信状態
	CMixiData			m_mixiBeforeRelogin;	///< 再ログイン前の mixi オブジェクト
	ACCESS_TYPE			m_accessType;			///< 現在のアクセス種別

	CMixiData			m_mixi4recv;			///< 受信時に利用する mixi データ
	CInetAccess			m_inet;					///< ネット接続オブジェクト

#ifndef WINCE
	// GDI+初期化トークン
	GdiplusStartupInput gdiSI;
	ULONG_PTR			gdiToken;
#endif

	//--- MZ3 Script 関連
public:
	struct Service
	{
		std::string name;		///< サービス名
		bool        selected;	///< デフォルト状態で選択済かどうか

		Service() : selected(false) {}
		Service(const std::string& a_name, bool a_selected) : name(a_name), selected(a_selected) {}
	};
	struct AccountData
	{
		std::string service_name;		///< サービス名
		std::string id_name;			///< IDの表示名
		std::string password_name;		///< Passwordの表示名

		AccountData(const std::string& sn="", const std::string& idn="", const std::string& pwn="")
			: service_name(sn), id_name(idn), password_name(pwn) {}
	};
	stdext::hash_map<std::string, std::string> m_luaParsers;				///< シリアライズキー → パーサ名
	stdext::hash_map<std::string, std::vector<std::string>> m_luaHooks;		///< {イベント} → パーサ名
	std::vector<std::string>						m_luaMenus;		///< 登録済みメニュー項目
	std::vector<Service>							m_luaServices;	///< 登録済みサービス群
	std::vector<AccountData>						m_luaAccounts;	///< 登録済みアカウント情報(ログイン設定用)
	int								   m_luaLastRegistedAccessType;	///< 利用済みアクセス種別

public:
	void ChangeView(CView*);
	CString GetAppDirPath();

//	CString MakeLoginUrl( LPCTSTR nextUrl=L"/home.pl" );
//	CString MakeLoginUrlForMixiMobile( LPCTSTR nextUrl );
	void StartMixiLoginAccess(HWND hwnd, CMixiData* data);

	BOOL EnableCommandBarButton( int nID, BOOL bEnable );
public:
	bool MakeNewFont( CFont* pBaseFont, int fontHeight, LPCTSTR fontFace );
	void ShowMixiData( CMixiData* data );

	ScreenResolution GetDisplayMode() { return m_currentDisplayMode; }
	DWORD GetDPI() { return m_dpi; }
	int GetInfoRegionHeight( int fontHeight );
	int GetTabHeight( int fontHeight );

	bool LoadSkinSetting();
	void ReflectSkinSetting();

	void InitPlatformFlags();
	void InitResolutionFlags();
	bool SaveGroupData(void);

	bool IsMixiLogout( ACCESS_TYPE aType );
	bool DeleteOldCacheFiles(void);
	int pt2px(int pt);
	int AddImageToImageCache(CWnd* pWnd, CMZ3BackgroundImage& srcImage, const CString& strImagePath);
	CString MakeMZ3RegularVersion(CString strVersion);

	//--- MZ3 Script 関連
	bool MyLuaInit(void);
	bool MyLuaClose(void);
	bool MyLuaExecute(LPCTSTR szLuaStatement);
	bool MyLuaErrorReport(int status);

	void DoParseMixiHomeHtml(CMixiData* data, CHtmlArray* html);

	CInetAccess::ENCODING GetInetAccessEncodingByAccessType(ACCESS_TYPE aType);

};

extern CMZ3App theApp;
