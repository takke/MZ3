/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
public:
	/// 起動時の移行処理の完了フラグ
	enum STARTUP_TRANSITION_DONE_TYPE
	{
		STARTUP_TRANSITION_DONE_TYPE_NONE = 0,					///< 初期版
		STARTUP_TRANSITION_DONE_TYPE_TWITTER_MODE_ADDED = 1,	///< Twitterモード追加時のメッセージ表示済
		STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED = 2,		///< フォントサイズの調整済
	};
	STARTUP_TRANSITION_DONE_TYPE	m_StartupTransitionDoneType;	///< 起動時のメッセージの表示済みフラグ

private:
	bool			m_bDebugMode;			///< デバッグモード

public:
	bool			m_bUseDevVerCheck;		///< バージョンチェックに開発版も含める
	bool			m_bShowBalloonOnNewTL;	///< [MZ4] バルーン表示有無

private:
	CString			m_proxyServer;			///< プロキシのサーバ
	int				m_proxyPort;			///< プロキシのポート
	CString			m_proxyUser;			///< プロキシのユーザ
	CString			m_proxyPassword;		///< プロキシのパスワード

#ifdef BT_MZ3
	GETPAGE_TYPE	m_GetPageType;			///< ページ取得種別
#endif

	//--- BOOT
	bool			m_bBootCheckMnC;		///< 起動時のメッセージ確認

	CString			m_quoteMark;			///< 引用符号
	BOOL			m_bUseBgImage;			///< 背景に画像を表示するかどうか
	int				m_totalRecvBytes;		///< 総データ受信量

public:
	int				m_nIntervalCheckSec;	///< 定期取得間隔[sec]
	bool			m_bEnableIntervalCheck;	///< 定期取得機能が有効か？

#ifndef WINCE
	CString			m_strWindowPos;			///< ウィンドウ位置・サイズ
#endif

	bool			m_bConfirmOpenURL;		///< URLを開くときに確認する

	int				m_fontHeight;			///< フォント（中）のサイズ（0 の場合は設定しない）
	CString			m_fontFace;				///< フォント名（"" の場合は設定しない）
	bool			m_bUseClearTypeFont;	///< Clear Type Font

	int				m_longReturnRangeMSec;	///< 長押し判定時間（ミリ秒）

	CString			m_logFolder;			///< ログ出力先フォルダ。"" の場合はデフォルト出力先。
	bool			m_bSaveLog;				///< ログを保存するかどうか。

	CString			m_strSkinname;			///< 現在のスキン名（=フォルダ名）

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

	int				m_lastTopPageTabIndex;				///< 前回終了時のタブのインデックス
	int				m_lastTopPageCategoryIndex;			///< 前回終了時のカテゴリのインデックス

	bool			m_bAutoDownloadEmojiImageFiles;		///< 起動時に絵文字ファイルを自動取得する

	//--- レポート画面
	enum REPORT_SCROLL_TYPE {
		REPORT_SCROLL_TYPE_LINE = 0,	///< 行単位
		REPORT_SCROLL_TYPE_PAGE = 1,	///< ページ単位
	};
	REPORT_SCROLL_TYPE	m_reportScrollType;				///< レポート画面のスクロールタイプ
	int				m_reportScrollLine;					///< レポート画面のスクロール行数
	
	bool			m_bUseRan2PanScrollAnimation;		///< らんらんビューのパンスクロールアニメーション
	bool			m_bUseRan2HorizontalDragMove;		///< 横ドラッグでの項目移動
	bool			m_bUseRan2DoubleClickMove;			///< ダブルクリックでの項目移動

	bool			m_bListScrollByPixelOnMZ3;			///< MZ3(WM)でピクセル単位スクロールを実行するか？
														///< T-01A(おそらくWM6.5も)ではスクロールがもたつくので
														///< デフォルトオフとする

	//--- メイン画面
	bool			m_bShowMainViewIcon;				///< トピック等にアイコンを表示する？
	bool			m_bShowMainViewMiniImage;			///< ユーザやコミュニティの画像を表示する？
	bool			m_bAutoLoadMiniImage;				///< 画像の自動取得

	bool			m_bBodyListIntegratedColumnMode;	///< ボディリストのカラムの統合モード
	int				m_nBodyListIntegratedColumnModeLine;///< ボディリストのカラムの統合モード
	bool			m_bOneClickCategoryFetchMode;		///< 上ペインのリストクリック時に取得する
	bool			m_bMagnifyModeCategoryAtStart;		///< 起動時に上ペインを最大化する

	enum BODYLIST_ICONSIZE {
		BODYLIST_ICONSIZE_AUTO = 0,		///< 自動
		BODYLIST_ICONSIZE_64 = 64,
		BODYLIST_ICONSIZE_48 = 48,
		BODYLIST_ICONSIZE_32 = 32,
		BODYLIST_ICONSIZE_16 = 16,
	};
	BODYLIST_ICONSIZE m_bodyListIconSize;			///< ボディリストのアイコンサイズ

	//--- 通信
	int				m_recvBufSize;					///< 受信バッファサイズ
	bool			m_bUseAutoConnection;			///< 自動接続を使う？
	CString			m_strUserAgent;					///< User-Agent

	bool			m_bShowNetProgressBar;			///< 通信中のプログレスバー表示

	bool			m_bUseProxy;					///< プロキシを使う？
	bool			m_bUseGlobalProxy;				///< グローバルプロキシを使う？
													///< （自動接続ONの場合のみ有効）
public:
	//--- Twitter
	bool			m_bAddSourceTextOnTwitterPost;	///< 文末に *MZ3* マークをつける
	CString			m_strTwitterPostFotterText;		///< 文末に付けるマーク
	int				m_nTwitterStatusLineCount;		///< Twitterモード時のステータス行数
	int				m_nTwitterGetPageCount;			///< Twitterモード時の取得ページ数
	bool			m_bTwitterReloadTLAfterPost;	///< 投稿後にタイムラインを取得する
	bool			m_bTwitterCursorRestoreAfterTLFetch;	///< TL取得時にカーソル位置を維持する
	int				m_nTwitterRTStyle;				///< ReTweet 形式
													///< 0:"RT @takke: 発言"
													///< 1:"公式RT"
													///< 2:"QT @takke: 発言"
													///< 3:"公式RT/RT"

public:
	Option()
		: m_bUseAutoConnection( true )
#ifdef BT_MZ3
		, m_GetPageType( GETPAGE_LATEST20 )
#endif
		, m_bUseProxy( false )
		, m_bUseGlobalProxy( true )
		, m_bBootCheckMnC( false )
		, m_bUseBgImage( TRUE )
		, m_fontHeight( 8 )					// 初期値は Load() 内で設定される
#ifdef WINCE
		, m_recvBufSize( 2048 )
#else
		, m_recvBufSize( 8192 )
#endif
		, m_bSaveLog( true )
		, m_totalRecvBytes( 0 )
		, m_bUseXcrawlExtension( false )
		, m_bUseRunConfirmDlg( true )
		, m_quoteMark( L"> " )
		, m_nMainViewCategoryListHeightRatio( 20 )	// カテゴリリストは 20%
		, m_nMainViewBodyListHeightRatio( 80 )		// ボディリストは 80%
		, m_nReportViewListHeightRatio( 30 )		// リストは 30%
		, m_nReportViewBodyHeightRatio( 70 )		// ボディは 70%
		, m_nReportViewListCol1Ratio( 7 )			// カラム１は 7/N
		, m_nReportViewListCol2Ratio( 17 )			// カラム２は 17/N
		, m_nReportViewListCol3Ratio( 21 )			// カラム３は 21/N
		, m_nMainViewCategoryListCol1Ratio( 25 )	// カテゴリリスト、カラム１は 25/N
		, m_nMainViewCategoryListCol2Ratio( 20 )	// カテゴリリスト、カラム２は 20/N
		, m_nMainViewBodyListCol1Ratio( 24 )		// ボディリスト、カラム１は 24/N
		, m_nMainViewBodyListCol2Ratio( 21 )		// ボディリスト、カラム２は 21/N
		, m_reportScrollType( REPORT_SCROLL_TYPE_LINE )
		, m_reportScrollLine( 7 )
		, m_bConfirmOpenURL( true )
/*
#ifdef WINCE
		, m_bConvertUrlForMixiMobile( true )
#else
		, m_bConvertUrlForMixiMobile( false )
#endif
*/
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
		, m_bUseRan2DoubleClickMove( false )
#else
		, m_bShowMainViewMiniImage( true )
		, m_bUseRan2DoubleClickMove( true )
#endif
		, m_bAutoLoadMiniImage( true )
		, m_bListScrollByPixelOnMZ3( false )
		, m_bBodyListIntegratedColumnMode( true )
		, m_nBodyListIntegratedColumnModeLine( 3 )
		, m_bOneClickCategoryFetchMode( true )
#ifdef BT_TKTW
		, m_bMagnifyModeCategoryAtStart( true )
#else
		, m_bMagnifyModeCategoryAtStart( false )
#endif
		, m_bAddSourceTextOnTwitterPost( true )
		, m_StartupTransitionDoneType( STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED )
		, m_nTwitterStatusLineCount(3)
		, m_bUseRan2PanScrollAnimation( true )
		, m_bUseRan2HorizontalDragMove( true )
		, m_bAutoDownloadEmojiImageFiles( true )
#ifdef WINCE
		, m_nTwitterGetPageCount(1)
#else
		, m_nTwitterGetPageCount(3)
#endif
#ifdef BT_TKTW
		, m_strTwitterPostFotterText(L" *TkTw*")
#else
		, m_strTwitterPostFotterText(L" *" MZ3_APP_NAME L"*")
#endif
		, m_bTwitterReloadTLAfterPost(false)
		, m_bTwitterCursorRestoreAfterTLFetch(false)
		, m_nTwitterRTStyle(0)
		, m_bUseDevVerCheck(false)
		, m_bShowNetProgressBar(false)
		, m_bShowBalloonOnNewTL(true)
		, m_bodyListIconSize(BODYLIST_ICONSIZE_AUTO)
		, m_bUseClearTypeFont(true)
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

private:
	void Sync(bool bSave);

public:
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

#ifdef BT_MZ3
	GETPAGE_TYPE GetPageType() { return m_GetPageType; }
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; }
#endif

	bool IsBootCheckMnC()			{ return m_bBootCheckMnC; }			///< 起動時のチェック
	void SetBootCheckMnC(bool flag) { m_bBootCheckMnC = flag; }			///< 起動時のチェックの設定

	LPCTSTR GetQuoteMark()				{ return m_quoteMark; }			///< 引用符号の取得
	void SetQuoteMark( LPCTSTR mark )	{ m_quoteMark = mark; }			///< 引用符号の設定

	BOOL IsUseBgImage()					{ return m_bUseBgImage; }		///< 背景画像を使う？
	void SetUseBgImage(BOOL flag)		{ m_bUseBgImage = flag; }		///< 背景画像を使うかどうかの設定

	int  GetFontHeight()				{ return m_fontHeight; }		///< フォントサイズの取得
	
	/// フォントサイズ(pt換算)をpixel換算値(local DPI換算)で返却する
	int  GetFontHeightByPixel(int iDPI)	{
		return ::MulDiv(m_fontHeight, iDPI, 72);
	}
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
		return normalizeRange( n, 5, 50 );
	}

	/// 長押し判定時間の正規化
	static int normalizeLongReturnRangeMSec( int msec ) {
		return normalizeRange( msec, 100, 1000 );
	}

	/// 定期取得間隔の正規化
	static int normalizeIntervalCheckSec( int sec ) {
		return normalizeRange( sec, 10, 600 );
	}

	/// Twitterモード時のステータス行数
	static int normalizeTwitterStatusLineCount( int n ) {
		return normalizeRange( n, 2, 3 );
	}

	/// Twitterモード時の最大取得ページ数
	static int normalizeTwitterGetPageCount( int n ) {
		return normalizeRange( n, 1, 5 );
	}
};

}