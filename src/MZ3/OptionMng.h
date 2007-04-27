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
	BOOL			m_proxyUse;			///< プロキシを使う？
	CString			m_proxyServer;		///< プロキシのサーバ
	int				m_proxyPort;		///< プロキシのポート
	CString			m_proxyUser;		///< プロキシのユーザ
	CString			m_proxyPassword;	///< プロキシのパスワード

	GETPAGE_TYPE	m_GetPageType;		///< ページ取得種別

	BOOL			m_bBootCheckMnC;	///< 起動時のメッセージ確認
	CString			m_quoteMark;		///< 引用符号

	BOOL			m_bUseBgImage;		///< 背景に画像を表示するかどうか
	int				m_totalRecvBytes;	///< 総データ受信量

public:
	int				m_fontHeight;		///< フォント（中）のサイズ（0 の場合は設定しない）
	CString			m_fontFace;			///< フォント名（"" の場合は設定しない）

	int				m_fontHeightBig;	///< フォント（大）のサイズ（0 の場合は設定しない）
	int				m_fontHeightMedium;	///< フォント（中）のサイズ（0 の場合は設定しない）
	int				m_fontHeightSmall;	///< フォント（小）のサイズ（0 の場合は設定しない）

	CString			m_logFolder;		///< ログ出力先フォルダ。"" の場合はデフォルト出力先。
	bool			m_bSaveLog;			///< ログを保存するかどうか。

	bool			m_bUseLeftSoftKey;	///< 左ソフトキーによるメニュー表示を利用する？

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

private:
	int				m_recvBufSize;		///< 受信バッファサイズ

public:
	Option()
		: m_GetPageType( GETPAGE_ALL )
		, m_bBootCheckMnC( FALSE )
		, m_bUseBgImage( TRUE )
		, m_fontHeight( 24 )
		, m_fontHeightBig( 28 )
		, m_fontHeightMedium( 24 )
		, m_fontHeightSmall( 18 )
		, m_recvBufSize( 512 )
		, m_bSaveLog( true )
		, m_totalRecvBytes( 0 )
		, m_bUseLeftSoftKey( true )
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

	{
	}

	virtual ~Option()
	{
	}

	void Load();
	void Save();

	void SetProxyUse(BOOL value) { m_proxyUse = value; };
	BOOL IsProxyUse() { return m_proxyUse; };
	
	void SetProxyServer(LPCTSTR value) { m_proxyServer = value; };
	LPCTSTR GetProxyServer() { return m_proxyServer; };
	
	void SetProxyPort(int value) { m_proxyPort = value; };
	int GetProxyPort() { return m_proxyPort; };

	void SetProxyUser(LPCTSTR value) { m_proxyUser = value; };
	LPCTSTR GetProxyUser() { return m_proxyUser; };

	void SetProxyPassword(LPCTSTR value) { m_proxyPassword = value; };
	LPCTSTR GetProxyPassword() { return m_proxyPassword; };

	GETPAGE_TYPE GetPageType() { return m_GetPageType; };
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; };

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
		if( bufSize < RECVBUFSIZE_MIN ) {
			return RECVBUFSIZE_MIN;
		}
		const int RECVBUFSIZE_MAX = 32768;
		if( bufSize > RECVBUFSIZE_MAX ) {
			return RECVBUFSIZE_MAX;
		}

		return bufSize;
	}

	int GetRecvBufSize() {
		// 正規化した値を返す
		return NormalizeRecvBufSize( m_recvBufSize );
	}

	void SetRecvBufSize( int recvBufSize ) {
		m_recvBufSize = NormalizeRecvBufSize( recvBufSize );
	}

};

}