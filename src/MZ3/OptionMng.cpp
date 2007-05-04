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

void Option::Load()
{
	MZ3LOGGER_DEBUG( L"オプション設定読み込み開始" );

	const CString& fileName = theApp.m_filepath.inifile;

	inifile::IniFile inifile;

	CFileStatus rStatus;
	if (CFile::GetStatus(fileName, rStatus) == FALSE) {
		inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
	}

	if(! inifile.Load( theApp.m_filepath.inifile ) ) {
		return;
	}

	if (inifile.SectionExists("Proxy") != FALSE) {
		// プロキシセクションがあった場合
		m_proxyUse = atoi( inifile.GetValue("Use", "Proxy").c_str() );

		m_proxyServer   = util::my_mbstowcs( inifile.GetValue("Server", "Proxy") ).c_str();
		m_proxyPort     = atoi( inifile.GetValue("Port", "Proxy").c_str() );
		m_proxyUser     = util::my_mbstowcs( inifile.GetValue("User", "Proxy") ).c_str();
		m_proxyPassword = util::my_mbstowcs( inifile.GetValue("Password", "Proxy") ).c_str();

	}
	TRACE(_T("Proxy[%d] %s:%d\n"), m_proxyUse, m_proxyServer, m_proxyPort);

	if (inifile.SectionExists("Page") != FALSE) {
		m_GetPageType = (GETPAGE_TYPE)atoi( inifile.GetValue("GetType", "Page").c_str() );
	}

	if (inifile.SectionExists("Boot") != FALSE) {
		m_bBootCheckMnC = (BOOL)atoi(inifile.GetValue("CheckMnC", "Boot").c_str());
	}

	if (inifile.SectionExists("UI") != FALSE) {
		// UI セクション

		// 背景画像の利用
		std::string s;
		s = inifile.GetValue("UseBgImage", "UI");
		if( s.empty() ) {
			// 初期値
			m_bUseBgImage = TRUE;
		}else{
			m_bUseBgImage = (BOOL)atoi(s.c_str());
		}

		// フォントサイズ
		s = inifile.GetValue("FontHeight", "UI");
		if( s.empty() ) {
			// 初期値
			m_fontHeight = 0;
		}else{
			m_fontHeight = (BOOL)atoi(s.c_str());
		}
		// フォントサイズ（大）
		s = inifile.GetValue("FontHeight_Big", "UI");
		if( s.empty() ) {
			// 初期値
//			m_fontHeightBig = 28;
		}else{
			m_fontHeightBig = (BOOL)atoi(s.c_str());
		}

		// フォントサイズ（中）
		s = inifile.GetValue("FontHeight_Medium", "UI");
		if( s.empty() ) {
			// 初期値
			m_fontHeightMedium = m_fontHeight;
		}else{
			m_fontHeightMedium = (BOOL)atoi(s.c_str());
		}

		// フォントサイズ（小）
		s = inifile.GetValue("FontHeight_Small", "UI");
		if( s.empty() ) {
			// 初期値
//			m_fontHeightSmall = 18;
		}else{
			m_fontHeightSmall = (BOOL)atoi(s.c_str());
		}

		// フォント名
		m_fontFace = inifile.GetValue("FontFace", "UI").c_str();

		// 左ソフトキー有効？
		s = inifile.GetValue( "UseLeftSoftKey", "UI" );
		if( s.empty() ) {
			// 初期値をそのまま使う
		}else{
			m_bUseLeftSoftKey = (s == "1");
		}

		// 引用符号
		s = inifile.GetValue( "QuoteMark", "UI" );
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

		// リストの高さ（比率）
		const int RATIO_MAX = 1000;
		s = inifile.GetValue( "MainViewCategoryListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewBodyListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewBodyHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nReportViewBodyHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}

		// リストのカラム幅（比率）
		s = inifile.GetValue( "MainViewBodyListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewBodyListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewCategoryListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewCategoryListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol3Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol3Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}

		// レポート画面のスクロールタイプ
		s = inifile.GetValue( "ReportScrollType", "UI" );
		if(! s.empty() ) {
			int type = atoi(s.c_str());
			switch( type ) {
			case option::Option::REPORT_SCROLL_TYPE_LINE:
			case option::Option::REPORT_SCROLL_TYPE_PAGE:
				m_reportScrollType = (option::Option::REPORT_SCROLL_TYPE) type;
				break;
			default:
				MZ3LOGGER_ERROR( L"ReportScrollType が規定値以外です" );
				break;
			}
		}

		// レポート画面のスクロール行数
		s = inifile.GetValue( "ReportScrollLine", "UI" );
		if(! s.empty() ) {
			m_reportScrollLine = normalizeRange( atoi(s.c_str()), 1, 100 );
		}
	}

	if (inifile.SectionExists("Log") != FALSE) {
		// Log セクション

		// 保存フラグ
		std::string s = inifile.GetValue( "SaveLog", "Log" );
		if( s.empty() ) {
			// 初期値をそのまま使う
		}else{
			m_bSaveLog = (s == "1");
		}

		// ログフォルダ
		m_logFolder = inifile.GetValue("LogFolder", "Log").c_str();
	}

	if (inifile.SectionExists("Net") != FALSE) {
		// 受信バッファサイズ
		m_recvBufSize = atoi( inifile.GetValue("RecvBufferSize", "Net").c_str() );

		// 総データ受信量
		m_totalRecvBytes = atoi( inifile.GetValue("TotalRecvBytes", "Net").c_str() );
	}

	MZ3LOGGER_DEBUG( L"オプション設定読み込み完了" );
}

void Option::Save()
{
	MZ3LOGGER_DEBUG( L"オプション設定保存開始" );

	inifile::IniFile inifile;

	inifile.SetValue("Use", (LPCSTR)util::int2str_a(m_proxyUse), "Proxy");
	inifile.SetValue(L"Server", m_proxyServer, "Proxy");
	inifile.SetValue("Port", (LPCSTR)util::int2str_a(m_proxyPort), "Proxy");

	inifile.SetValue(L"User", m_proxyUser, "Proxy");
	inifile.SetValue(L"Password", m_proxyPassword, "Proxy");
	inifile.SetValue("GetType", (LPCSTR)util::int2str_a(m_GetPageType), "Page");
	inifile.SetValue("CheckMnC", (LPCSTR)util::int2str_a(m_bBootCheckMnC), "Boot");

	//--- UI 系
	CStringA s;
	// 背景画像の利用
	inifile.SetValue( "UseBgImage", (LPCSTR)util::int2str_a(m_bUseBgImage), "UI");

	// フォントサイズ
	inifile.SetValue( "FontHeight", (LPCSTR)util::int2str_a(m_fontHeight), "UI");

	// フォントサイズ（大）
	inifile.SetValue( "FontHeight_Big", (LPCSTR)util::int2str_a(m_fontHeightBig), "UI");
	// フォントサイズ（中）
	inifile.SetValue( "FontHeight_Medium", (LPCSTR)util::int2str_a(m_fontHeightMedium), "UI");
	// フォントサイズ（小）
	inifile.SetValue( "FontHeight_Small", (LPCSTR)util::int2str_a(m_fontHeightSmall), "UI");

	// フォント名
	inifile.SetValue( L"FontFace", m_fontFace, "UI");
	// 左ソフトキー有効？
	inifile.SetValue( "UseLeftSoftKey", m_bUseLeftSoftKey ? "1" : "0", "UI" );

	// 引用符号
	// 末尾の半角スペースを保存するため、
	// "/" で囲む形で保存する
	inifile.SetValue( L"QuoteMark", L"/" + m_quoteMark + L"/", "UI" );

	// リストの高さ（比率）
	inifile.SetValue( "MainViewCategoryListHeightRatio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListHeightRatio), "UI" );
	inifile.SetValue( "MainViewBodyListHeightRatio",     (LPCSTR)util::int2str_a(m_nMainViewBodyListHeightRatio), "UI" );
	inifile.SetValue( "ReportViewListHeightRatio", (LPCSTR)util::int2str_a(m_nReportViewListHeightRatio), "UI" );
	inifile.SetValue( "ReportViewBodyHeightRatio", (LPCSTR)util::int2str_a(m_nReportViewBodyHeightRatio), "UI" );

	// リストのカラム幅（比率）
	inifile.SetValue( "MainViewBodyListCol1Ratio", (LPCSTR)util::int2str_a(m_nMainViewBodyListCol1Ratio), "UI" );
	inifile.SetValue( "MainViewBodyListCol2Ratio", (LPCSTR)util::int2str_a(m_nMainViewBodyListCol2Ratio), "UI" );
	inifile.SetValue( "MainViewCategoryListCol1Ratio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListCol1Ratio), "UI" );
	inifile.SetValue( "MainViewCategoryListCol2Ratio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListCol2Ratio), "UI" );

	inifile.SetValue( "ReportViewListCol1Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol1Ratio), "UI" );
	inifile.SetValue( "ReportViewListCol2Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol2Ratio), "UI" );
	inifile.SetValue( "ReportViewListCol3Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol3Ratio), "UI" );

	// レポート画面のスクロールタイプ
	inifile.SetValue( "ReportScrollType", (LPCSTR)util::int2str_a(m_reportScrollType), "UI" );

	// レポート画面のスクロール行数
	inifile.SetValue( "ReportScrollLine", (LPCSTR)util::int2str_a(m_reportScrollLine), "UI" );

	//--- Log 関連
	// 保存フラグ
	inifile.SetValue( "SaveLog", m_bSaveLog ? "1" : "0", "Log" );

	// 保存先フォルダ
	inifile.SetValue( L"LogFolder", m_logFolder, "Log");

	// 受信バッファサイズ
	inifile.SetValue( "RecvBufferSize", (LPCSTR)util::int2str_a(m_recvBufSize), "Net" );

	// 総データ受信量
	inifile.SetValue( "TotalRecvBytes", (LPCSTR)util::int2str_a(m_totalRecvBytes), "Net" );

	// Save
	inifile.Save( theApp.m_filepath.inifile, false );

	MZ3LOGGER_DEBUG( L"オプション設定保存完了" );
}

}// namespace option
