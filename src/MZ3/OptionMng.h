/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "constants.h"

/// オプションデータ
namespace option {

/**
 * オプションデータ管理クラス
 */
class Option
{
private:
	bool			m_bDebugMode;			///< デバッグモード
	bool			m_bUseAutoConnection;	///< 自動接続を使う？
	bool			m_bUseProxy;			///< プロキシを使う？
	bool			m_bUseGlobalProxy;		///< グローバルプロキシを使う？
											///< （自動接続ONの場合のみ有効）
public:
	CString			m_strUserAgent;			///< User-Agent

private:
	CString			m_proxyServer;			///< プロキシのサーバ
	int				m_proxyPort;			///< プロキシのポート
	CString			m_proxyUser;			///< プロキシのユーザ
	CString			m_proxyPassword;		///< プロキシのパスワード

	GETPAGE_TYPE	m_GetPageType;			///< ページ取得種別

	BOOL			m_bBootCheckMnC;		///< 起動時のメッセージ確認
	CString			m_quoteMark;			///< 引用符号

	BOOL			m_bUseBgImage;			///< 背景に画像を表示するかどうか
	int				m_totalRecvBytes;		///< 総データ受信量

public:
	int				m_nIntervalCheckSec;	///< 定期取得間隔[sec]
	bool			m_bEnableIntervalCheck;	///< 定期取得機能が有効か？

#ifndef WINCE
	CString			m_strWindowPos;			///< ウィンドウ位置・サイズ
#endif

	bool			m_bConvertUrlForMixiMobile;	///< mixi モバイルのURLに変換するかどうか

	int				m_fontHeight;			///< フォント（中）のサイズ（0 の場合は設定しない）
	CString			m_fontFace;				///< フォント名（"" の場合は設定しない）

	int				m_fontHeightBig;		///< フォント（大）のサイズ（0 の場合は設定しない）
	int				m_fontHeightMedium;		///< フォント（中）のサイズ（0 の場合は設定しない）
	int				m_fontHeightSmall;		///< フォント（小）のサイズ（0 の場合は設定しない）

	int				m_longReturnRangeMSec;	///< 長押し判定時間（ミリ秒）

	CString			m_logFolder;			///< ログ出力先フォルダ。"" の場合はデフォルト出力先。
	bool			m_bSaveLog;				///< ログを保存するかどうか。

	CString			m_strSkinname;			///< 現在のスキン名（=フォルダ名）

	bool			m_bUseLeftSoftKey;		///< 左ソフトキーによるメニュー表示を利用する？
	bool			m_bUseXcrawlExtension;	///< Xcrawl の誤動作防止機能
	bool			m_bUseRunConfirmDlg;	///< ダウンロード後の実行確認画面を表示する？

	int				m_nMainViewCategoryListHeightRatio;	///< メイン画面のカテゴリリストの高さ（比率）[1,1000]
	int				m_nMainViewBodyListHeightRatio;		///< メイン画面のボディリストの高さ（比率）[1,1000]
	int				m_nReportViewListHeightRatio;		///< レポート画面のリストの高さ（比率）[1,1000]
	int				m_nReportViewBodyHeightRatio;		///< レポート画面のボディの高さ（比率）[1,1000]

	int				m_nMainViewCategoryListCol1Ratio;	///< メイン画面のカテゴリリストのカラム１の幅（比率）[1,1000]
	int				m_nMainViewCategoryListCol2Ratio;	///< メイン画面のカテゴリリストのカラム２の幅（比率）[1,1000]
	int				m_nMainViewBodyListCol1Ratio;		///< メイン画面のボディリストのカラム１の幅（比率）[1,1000]
	int				m_nMainViewBodyListCol2Ratio;		///< メイン画面のボディリストのカラム２の幅（比率）[1,1000]

	int				m_nReportViewListCol1Ratio;			///< レポート画面のリストのカラム１の幅（比率）[1,1000]
	int				m_nReportViewListCol2Ratio;			///< レポート画面のリストのカラム２の幅（比率）[1,1000]
	int				m_nReportViewListCol3Ratio;			///< レポート画面のリストのカラム３の幅（比率）[1,1000]

	bool			m_killPaneLabel;					///< ペインのラベルを表示しない？

	enum REPORT_SCROLL_TYPE {
		REPORT_SCROLL_TYPE_LINE = 0,	///< 行単位
		REPORT_SCROLL_TYPE_PAGE = 1,	///< ページ単位
	};
	REPORT_SCROLL_TYPE	m_reportScrollType;		///< レポート画面のスクロールタイプ
	int				m_reportScrollLine;			///< レポート画面のスクロール行数

	int				m_lastTopPageTabIndex;		///< 前回終了時のタブのインデックス
	int				m_lastTopPageCategoryIndex;	///< 前回終了時のカテゴリのインデックス

	//--- メイン画面
	bool			m_bShowMainViewIcon;		///< トピック等にアイコンを表示する？
	bool			m_bShowMainViewMiniImage;	///< ユーザやコミュニティの画像を表示する？
	bool			m_bShowMainViewMiniImageDlg;			///< 別画面で表示する？
	bool			m_bShowMainViewMiniImageOnMouseOver;	///< マウスオーバーで表示する？
	int				m_nMainViewMiniImageSize;	///< mini画面サイズ

private:
	int				m_recvBufSize;				///< 受信バッファサイズ

public:
	Option()
		: m_GetPageType( GETPAGE_LATEST20 )
		, m_bUseAutoConnection( true )
		, m_bUseProxy( false )
		, m_bUseGlobalProxy( true )
		, m_bBootCheckMnC( FALSE )
		, m_bUseBgImage( TRUE )
		, m_fontHeight( 24 )				// 初期値は Load() 内で設定される
		, m_fontHeightBig( 28 )				// 初期値は Load() 内で設定される
		, m_fontHeightMedium( 24 )			// 初期値は Load() 内で設定される
		, m_fontHeightSmall( 18 )			// 初期値は Load() 内で設定される
#ifdef WINCE
		, m_recvBufSize( 2048 )
#else
		, m_recvBufSize( 8192 )
#endif
		, m_bSaveLog( true )
		, m_totalRecvBytes( 0 )
		, m_bUseLeftSoftKey( true )
		, m_bUseXcrawlExtension( false )
		, m_bUseRunConfirmDlg( true )
		, m_quoteMark( L"> " )
		, m_nMainViewCategoryListHeightRatio( 30 )	// カテゴリリストは 30%
		, m_nMainViewBodyListHeightRatio( 70 )		// ボディリストは 70%
		, m_nReportViewListHeightRatio( 40 )		// リストは 40%
		, m_nReportViewBodyHeightRatio( 60 )		// ボディは 60%
		, m_nReportViewListCol1Ratio( 7 )			// カラム１は 7/N
		, m_nReportViewListCol2Ratio( 17 )			// カラム２は 17/N
		, m_nReportViewListCol3Ratio( 21 )			// カラム３は 21/N
		, m_nMainViewCategoryListCol1Ratio( 25 )	// カテゴリリスト、カラム１は 25/N
		, m_nMainViewCategoryListCol2Ratio( 20 )	// カテゴリリスト、カラム２は 20/N
		, m_nMainViewBodyListCol1Ratio( 24 )		// ボディリスト、カラム１は 24/N
		, m_nMainViewBodyListCol2Ratio( 21 )		// ボディリスト、カラム２は 21/N
		, m_reportScrollType( REPORT_SCROLL_TYPE_LINE )
		, m_reportScrollLine( 7 )
#ifdef WINCE
		, m_bConvertUrlForMixiMobile( true )
#else
		, m_bConvertUrlForMixiMobile( false )
#endif
		, m_bDebugMode( false )
		, m_longReturnRangeMSec( 300 )
		, m_strUserAgent( L"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" )
		, m_strSkinname( L"aluminium" )
		, m_nIntervalCheckSec( 60 )
		, m_bEnableIntervalCheck( false )
		, m_lastTopPageTabIndex( 0 )
		, m_lastTopPageCategoryIndex( 0 )
		, m_bShowMainViewIcon( true )
#ifdef WINCE
		, m_bShowMainViewMiniImage( true )
		, m_bShowMainViewMiniImageDlg( false )
		, m_bShowMainViewMiniImageOnMouseOver( false )
#else
		, m_bShowMainViewMiniImage( true )
		, m_bShowMainViewMiniImageDlg( true )
		, m_bShowMainViewMiniImageOnMouseOver( true )
#endif
		, m_nMainViewMiniImageSize( 50 )
	{
	}

	virtual ~Option()
	{
	}

	/// 正規化
	static int normalizeRange( int value, int minv, int maxv ) 
	{
		// 下限値調整
		value = max( value, minv );

		// 上限値調整
		value = min( value, maxv );

		return value;
	}

	void Load();
	void Save();

	void SetUseProxy(bool value) { m_bUseProxy = value; }
	bool IsUseProxy() { return m_bUseProxy; }
	
	void SetDebugMode(bool value) { m_bDebugMode = value; }
	bool IsDebugMode() { return m_bDebugMode; }
	
	void SetUseGlobalProxy(bool value) { m_bUseGlobalProxy = value; }
	bool IsUseGlobalProxy() { return m_bUseGlobalProxy; }
	
	void SetUseAutoConnection(bool value) { m_bUseAutoConnection = value; }
	bool IsUseAutoConnection() { return m_bUseAutoConnection; }
	
	void SetProxyServer(LPCTSTR value) { m_proxyServer = value; }
	LPCTSTR GetProxyServer() { return m_proxyServer; }
	
	void SetProxyPort(int value) { m_proxyPort = value; }
	int GetProxyPort() { return m_proxyPort; }

	void SetProxyUser(LPCTSTR value) { m_proxyUser = value; }
	LPCTSTR GetProxyUser() { return m_proxyUser; }

	void SetProxyPassword(LPCTSTR value) { m_proxyPassword = value; }
	LPCTSTR GetProxyPassword() { return m_proxyPassword; }

	GETPAGE_TYPE GetPageType() { return m_GetPageType; }
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; }

	BOOL IsBootCheckMnC()			{ return m_bBootCheckMnC; }			///< 起動時のチェック
	void SetBootCheckMnC(BOOL flag) { m_bBootCheckMnC = flag; }			///< 起動時のチェックの設定

	LPCTSTR GetQuoteMark()				{ return m_quoteMark; }			///< 引用符号の取得
	void SetQuoteMark( LPCTSTR mark )	{ m_quoteMark = mark; }			///< 引用符号の設定

	BOOL IsUseBgImage()					{ return m_bUseBgImage; }		///< 背景画像を使う？
	void SetUseBgImage(BOOL flag)		{ m_bUseBgImage = flag; }		///< 背景画像を使うかどうかの設定

	int  GetFontHeight()				{ return m_fontHeight; }		///< フォントサイズの取得
	void SetFontHeight( int fontHeight ){ m_fontHeight = fontHeight; }	///< フォントサイズの設定

	LPCTSTR GetFontFace()				{ return m_fontFace; }			///< フォント名の取得
	void SetFontFace( LPCTSTR fontFace ){ m_fontFace = fontFace; }		///< フォント名の設定

	LPCTSTR GetLogFolder()					{ return m_logFolder; }		///< ログフォルダの取得
	void SetLogFolder( LPCTSTR logFolder )	{ m_logFolder = logFolder; }///< ログフォルダの設定

	int	 GetTotalRecvBytes()			{ return m_totalRecvBytes; }	///< 総データ受信量の取得
	void AddTotalRecvBytes( int bytes )	{ m_totalRecvBytes += bytes; }	///< 総データ受信量の追加
	void ResetTotalRecvBytes()			{ m_totalRecvBytes = 0; }		///< 総データ受信量のリセット

	/// 指定された値を受信バッファサイズとして有効な値に変換する
	int NormalizeRecvBufSize( int bufSize ) {

		const int RECVBUFSIZE_MIN = 128;
		const int RECVBUFSIZE_MAX = 32768;

		return normalizeRange( bufSize, RECVBUFSIZE_MIN, RECVBUFSIZE_MAX );
	}

	int GetRecvBufSize() {
		// 正規化した値を返す
		return NormalizeRecvBufSize( m_recvBufSize );
	}

	void SetRecvBufSize( int recvBufSize ) {
		m_recvBufSize = NormalizeRecvBufSize( recvBufSize );
	}

	/// フォントサイズの正規化
	static int normalizeFontSize( int n ) {
		return normalizeRange( n, 8, 50 );
	}

	/// 長押し判定時間の正規化
	static int normalizeLongReturnRangeMSec( int msec ) {
		return normalizeRange( msec, 100, 1000 );
	}

	/// 定期取得間隔の正規化
	static int normalizeIntervalCheckSec( int sec ) {
		return normalizeRange( sec, 10, 600 );
	}
};

}