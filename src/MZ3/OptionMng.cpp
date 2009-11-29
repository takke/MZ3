/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionMng.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionMng.h"
#include "IniFile.h"
#include "util.h"

/// オプションデータ
namespace option {

// Option メンバ関数

/**
 * ini ファイルの同期(bool)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, bool& targetValue, const char* key, const char* section, bool defaultValue)
{
	if (bSave) {
		inifile.SetValue(key, (LPCSTR)util::int2str_a(targetValue ? 1 : 0), section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// 初期値
			targetValue = defaultValue;
		} else {
			targetValue = atoi( s.c_str() ) ? true : false;
		}
	}
}

/**
 * ini ファイルの同期(CString)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, CString& targetValue, const char* key, const char* section, const CString defaultValue)
{
	if (bSave) {
		inifile.SetValue(util::my_mbstowcs(key).c_str(), targetValue, section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// 初期値
			targetValue = defaultValue;
		} else {
			targetValue = util::my_mbstowcs( s.c_str() ).c_str();
		}
	}
}

/**
 * ini ファイルの同期(int)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, int& targetValue, const char* key, const char* section, int defaultValue)
{
	if (bSave) {
		inifile.SetValue(key, (LPCSTR)util::int2str_a(targetValue), section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// 初期値
			targetValue = defaultValue;
		} else {
			targetValue = atoi( s.c_str() );
		}
	}
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, bool& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, CString& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, int& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}

/**
 * 保存/読込処理
 */
void Option::Sync(bool bSave)
{
	inifile::IniFile inifile;

	// ProMode
	if (theApp.m_bProMode && !bSave) {
		// スキンのデフォルト値を変更
#ifdef WINCE
		m_strSkinname = L"dark";
#endif
	}

	//--- 初期処理
	if (bSave) {
		// 保存時の初期処理
	} else {
		// 読込時の初期処理
		const CString& fileName = theApp.m_filepath.inifile;

		CFileStatus rStatus;
		if (CFile::GetStatus(fileName, rStatus) == FALSE) {
			inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
		}

		if(! inifile.Load( theApp.m_filepath.inifile ) ) {
	//		return;
		}
	}

	//--- General
	SyncIniValue(inifile, bSave, (int&)m_StartupTransitionDoneType, "StartupMessageDoneType", "General" );
	SyncIniValue(inifile, bSave, m_bConfirmOpenURL, "ConfirmOpenURL", "General" );
//	SyncIniValue(inifile, bSave, m_bConvertUrlForMixiMobile, "ConvertUrlForMixiMobile", "General" );
	SyncIniValue(inifile, bSave, m_bUseDevVerCheck, "UseDevVerCheck", "General");

	// 起動時に絵文字ファイルを自動取得する
	SyncIniValue(inifile, bSave, m_bAutoDownloadEmojiImageFiles, "AutoDownloadEmojiImageFiles", "General" );

	//--- Proxy
	SyncIniValue(inifile, bSave, m_bUseProxy,		"Use",			  "Proxy");
	SyncIniValue(inifile, bSave, m_bUseGlobalProxy, "UseGlobalProxy", "Proxy");

	SyncIniValue(inifile, bSave, m_proxyServer,     "Server",		  "Proxy");
	SyncIniValue(inifile, bSave, m_proxyPort,       "Port",		      "Proxy");
	SyncIniValue(inifile, bSave, m_proxyUser,       "User",		      "Proxy");
	SyncIniValue(inifile, bSave, m_proxyPassword,   "Password",		  "Proxy");

	//--- Page
	SyncIniValue(inifile, bSave, (int&)m_GetPageType, "GetType", "Page");

	//--- Boot
	SyncIniValue(inifile, bSave, m_bBootCheckMnC, "CheckMnC", "Boot");

	//--- UI
	// 背景画像の有無
	SyncIniValue(inifile, bSave, m_bUseBgImage, "UseBgImage", "UI");

	// フォントサイズ
	if (!bSave) {
		// 初期値設定
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeight = 24; break;
//		case SR_QVGA:
//		default:			m_fontHeight = 12; break;
//		}
		m_fontHeight = 9;
#else
		m_fontHeight = 11;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeight, "FontHeight", "UI");

	// フォントサイズ（大）
	if (!bSave) {
		// 初期値設定
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeightBig = 28; break;
//		case SR_QVGA:
//		default:			m_fontHeightBig = 14; break;
//		}
		m_fontHeightBig = 11;
#else
		m_fontHeightBig = 13;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeightBig, "FontHeight_Big", "UI");

	// フォントサイズ（中）
	if (!bSave) {
		// 初期値設定
		m_fontHeightMedium = m_fontHeight;
	}
	SyncIniValue(inifile, bSave, m_fontHeightMedium, "FontHeight_Medium", "UI");

	// フォントサイズ（小）
	if (!bSave) {
		// 初期値設定
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeightSmall = 18; break;
//		case SR_QVGA:
//		default:			m_fontHeightSmall = 10; break;
//		}
		m_fontHeightSmall = 7;
#else
		m_fontHeightSmall = 9;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeightSmall, "FontHeight_Small", "UI");

	// フォント名
	SyncIniValue(inifile, bSave, m_fontFace, "FontFace", "UI");

	// Xcrawl 誤動作防止機能
	SyncIniValue(inifile, bSave, m_bUseXcrawlExtension, "UseXcrawlCanceler", "UI" );

	// ダウンロード後の実行確認画面を表示する？
	SyncIniValue(inifile, bSave, m_bUseRunConfirmDlg, "UseRunConfirmDlg", "UI" );

	// 長押し判定時間
	SyncIniValue(inifile, bSave, m_longReturnRangeMSec, "LongReturnRangeMSec", "UI" );

	// 引用符号
	if (bSave) {
		// 引用符号
		// 末尾の半角スペースを保存するため、
		// "/" で囲む形で保存する
		inifile.SetValue( L"QuoteMark", L"/" + m_quoteMark + L"/", "UI" );
	} else {
		// 引用符号
		std::string s = inifile.GetValue( "QuoteMark", "UI" );
		if( s.empty() ) {
			// 初期値をそのまま使う
		}else{
			m_quoteMark = s.c_str();
			// 先頭と末尾の "/" を削除する
			if( m_quoteMark.Left(1) == L"/" ) {
				m_quoteMark.Delete( 0 );
			}
			if( m_quoteMark.Right(1) == L"/" ) {
				m_quoteMark.Delete( m_quoteMark.GetLength()-1 );
			}
		}
	}

	//--- UI 系
	// リストの高さ（比率）
	const int RATIO_MAX = 1000;
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListHeightRatio, "MainViewCategoryListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListHeightRatio = normalizeRange(m_nMainViewCategoryListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewBodyListHeightRatio, "MainViewBodyListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListHeightRatio = normalizeRange( m_nMainViewBodyListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListHeightRatio, "ReportViewListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListHeightRatio = normalizeRange( m_nReportViewListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewBodyHeightRatio, "ReportViewBodyHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewBodyHeightRatio = normalizeRange( m_nReportViewBodyHeightRatio, 1, RATIO_MAX );
	}

	// リストのカラム幅（比率）
	SyncIniValue(inifile, bSave, m_nMainViewBodyListCol1Ratio, "MainViewBodyListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListCol1Ratio = normalizeRange( m_nMainViewBodyListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewBodyListCol2Ratio, "MainViewBodyListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListCol2Ratio = normalizeRange( m_nMainViewBodyListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListCol1Ratio, "MainViewCategoryListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListCol1Ratio = normalizeRange( m_nMainViewCategoryListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListCol2Ratio, "MainViewCategoryListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListCol2Ratio = normalizeRange( m_nMainViewCategoryListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol1Ratio, "ReportViewListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol1Ratio = normalizeRange( m_nReportViewListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol2Ratio, "ReportViewListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol2Ratio = normalizeRange( m_nReportViewListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol3Ratio, "ReportViewListCol3Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol3Ratio = normalizeRange( m_nReportViewListCol3Ratio, 1, RATIO_MAX );
	}

	// レポート画面のスクロールタイプ
	SyncIniValue(inifile, bSave, (int&)m_reportScrollType, "ReportScrollType", "UI" );
	if (!bSave) {
		// check
		switch (m_reportScrollType) {
		case option::Option::REPORT_SCROLL_TYPE_LINE:
		case option::Option::REPORT_SCROLL_TYPE_PAGE:
			break;
		default:
			MZ3LOGGER_ERROR( L"ReportScrollType が規定値以外です" );
			m_reportScrollType = option::Option::REPORT_SCROLL_TYPE_LINE;
			break;
		}
	}

	// レポート画面のスクロール行数
	SyncIniValue(inifile, bSave, m_reportScrollLine, "ReportScrollLine", "UI" );
	if (!bSave) {
		// normalize
		m_reportScrollLine = normalizeRange( m_reportScrollLine, 1, 100 );
	}

	// らんらんビューのパンスクロール
	SyncIniValue(inifile, bSave, m_bUseRan2PanScrollAnimation, "UseRan2PanScrollAnimation", "UI" );

	// 横ドラッグでの項目移動
	SyncIniValue(inifile, bSave, m_bUseRan2HorizontalDragMove, "UseRan2HorizontalDragMove", "UI" );

	// ダブルクリックでの項目移動
	SyncIniValue(inifile, bSave, m_bUseRan2DoubleClickMove, "UseRan2DoubleClickMove", "UI" );

	// MZ3(WM)でピクセル単位スクロールを実行するか？
	SyncIniValue(inifile, bSave, m_bListScrollByPixelOnMZ3, "ListScrollByPixelOnMZ3", "UI");

	// スキン名
	SyncIniValue(inifile, bSave, m_strSkinname, "SkinName", "UI" );

#ifndef WINCE
	// ウィンドウ位置・サイズ
	SyncIniValue(inifile, bSave, m_strWindowPos, "WindowPos", "UI" );
#endif

	// 定期取得間隔
	SyncIniValue(inifile, bSave, m_nIntervalCheckSec, "IntervalCheckSec", "UI" );
	if(!bSave) {
		// normalize
		m_nIntervalCheckSec = normalizeIntervalCheckSec(m_nIntervalCheckSec);
	}

	// 前回終了時のタブのインデックス
	SyncIniValue(inifile, bSave, m_lastTopPageTabIndex, "LastTopPageTabIndex", "UI" );

	// 前回終了時のカテゴリのインデックス
	SyncIniValue(inifile, bSave, m_lastTopPageCategoryIndex, "LastTopPageCategoryIndex", "UI" );

	// Treo用の画面節約モード::ペインのラベル非表示
	SyncIniValue(inifile, bSave, m_killPaneLabel, "KillPaneLabel", "UI" );

	//--- Log 関連
	// 保存フラグ
	SyncIniValue(inifile, bSave, m_bSaveLog, "SaveLog", "Log" );

	// ログフォルダ
	SyncIniValue(inifile, bSave, m_logFolder, "LogFolder", "Log" );

	// デバッグモード フラグ
	SyncIniValue(inifile, bSave, m_bDebugMode, "DebugMode", "Log" );

	//--- Net
	// 受信バッファサイズ
	SyncIniValue(inifile, bSave, m_recvBufSize, "RecvBufferSize", "Net");

	// 自動接続
	SyncIniValue(inifile, bSave, m_bUseAutoConnection, "AutoConnect", "Net");

	// 総データ受信量
	SyncIniValue(inifile, bSave, m_totalRecvBytes, "TotalRecvBytes", "Net");

	// User-Agent
	SyncIniValue(inifile, bSave, m_strUserAgent, "UserAgent", "Net" );

	// 通信中のプログレスバー表示
	SyncIniValue(inifile, bSave, m_bShowNetProgressBar, "ShowNetProgressBar", "Net");

	//--- メイン画面
	// トピック等にアイコンを表示する？
	SyncIniValue(inifile, bSave, m_bShowMainViewIcon, "ShowMainViewIcon", "MainView");

	// ユーザやコミュニティの画像を表示する？
	SyncIniValue(inifile, bSave, m_bShowMainViewMiniImage, "ShowMainViewMiniImage", "MainView");
	
	// 画像の自動取得
	SyncIniValue(inifile, bSave, m_bAutoLoadMiniImage, "AutoLoadMiniImage", "MainView");

	// ボディリストのカラムの統合モード
	SyncIniValue(inifile, bSave, m_bBodyListIntegratedColumnMode, "MainViewBodyListIntegratedColumnMode", "MainView");

	// 上ペインのリストクリック時に取得する
	SyncIniValue(inifile, bSave, m_bOneClickCategoryFetchMode, "MainViewOneClickCategoryFetchMode", "MainView");

	//--- Twitter
	SyncIniValue(inifile, bSave, m_bAddSourceTextOnTwitterPost, "AddSourceTextOnTwitterPost", "Twitter");
	SyncIniValue(inifile, bSave, m_strTwitterPostFotterText, "PostFotterText", "Twitter" );
	SyncIniValue(inifile, bSave, m_nTwitterStatusLineCount, "TwitterStatusLineCount", "Twitter");
	if (!bSave) {
		// normalize
		m_nTwitterStatusLineCount = normalizeTwitterStatusLineCount(m_nTwitterStatusLineCount);
	}
	SyncIniValue(inifile, bSave, m_nTwitterGetPageCount, "GetPageCount", "Twitter");
	if (!bSave) {
		// normalize
		m_nTwitterGetPageCount = normalizeTwitterGetPageCount(m_nTwitterGetPageCount);
	}
	SyncIniValue(inifile, bSave, m_bTwitterReloadTLAfterPost, "ReloadTLAfterPost", "Twitter");

	// 終了処理
	if (bSave) {
		// Save
		inifile.Save( theApp.m_filepath.inifile, false );
	}
}

void Option::Load()
{
	MZ3LOGGER_DEBUG( L"オプション設定読み込み開始" );

	Sync(false);

	MZ3LOGGER_DEBUG( L"オプション設定読み込み完了" );
}

void Option::Save()
{
	MZ3LOGGER_DEBUG( L"オプション設定保存開始" );

	inifile::IniFile inifile;

	Sync(true);

	MZ3LOGGER_DEBUG( L"オプション設定保存完了" );
}

}// namespace option
