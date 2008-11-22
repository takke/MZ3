/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"
#include "MZ3Doc.h"
#include "MZ3View.h"
#include "ReportView.h"
#include "WriteView.h"
#include "Ran2View.h"
#include "DownloadView.h"
#include "AboutDlg.h"
#include "util.h"
#include "util_gui.h"
#include "DebugDlg.h"
#include "url_encoder.h"
#include "mz3_revision.h"
#include "MZ3Parser.h"
#include "MouseGestureManager.h"
#include "MZ3FileCacheManager.h"
#include "ChooseClientTypeDlg.h"

/* lua support exp.
extern "C" {
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3App

BEGIN_MESSAGE_MAP(CMZ3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMZ3App::OnAppAbout)
END_MESSAGE_MAP()

/* lua support exp.
// the Lua object
lua_State* g_luaState = NULL;

int lua_mz3_trace(lua_State *lua)
{
	CString s( lua_tostring(lua, -1) );

	MZ3_TRACE(s);

	return 0;
}
*/

#define SZ_REG_ROOT		TEXT("System\\GWE\\Display")
#define SZ_REG_DBVOL	TEXT("LogicalPixelsY")

// -----------------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------------
CMZ3App::CMZ3App()
	: CWinApp()
	, m_dpi(96)
	, m_bgImageReportListCtrl(L"report.jpg")
	, m_bgImageMainBodyCtrl(L"body.jpg")
	, m_bgImageMainCategoryCtrl(L"header.jpg")
	// プラットフォーム用のフラグ
	, m_bPocketPC(FALSE)
	, m_bSmartphone(FALSE)
	, m_bWinMoFiveOh(FALSE)
	, m_bWinMo2003(FALSE)
	, m_bWinMo2003_SE(FALSE)
	, m_pMouseGestureManager(NULL)
{
}

// -----------------------------------------------------------------------------
// デストラクタ
// -----------------------------------------------------------------------------
CMZ3App::~CMZ3App()
{
#ifndef WINCE
	return;
#endif
	if( m_pReportView != NULL ) {
		delete m_pReportView;
		m_pReportView = NULL;
	}
//	if( m_pWriteView != NULL ) {
//		delete m_pWriteView;
//		m_pWriteView = NULL;
//	}
	if (m_pMouseGestureManager != NULL) {
		delete m_pMouseGestureManager;
		m_pMouseGestureManager = NULL;
	}
}

// 唯一の CMZ3App オブジェクトです。
CMZ3App theApp;

// CMZ3App 初期化

BOOL CMZ3App::InitInstance()
{
#ifndef WINCE 
	::GdiplusStartup(&gdiToken, &gdiSI, NULL);
#endif
	// プラットフォーム判定
	InitPlatformFlags();

	// 解像度/DPI判定
	InitResolutionFlags();

	// アクセス種別毎の振る舞い定義を初期化
	m_accessTypeInfo.init();

	// CAPEDIT および SIPPREF のような Windows Mobile 特有のコントロールを初期化するには、アプリケーションの
	// 初期化中に SHInitExtraControls を一度呼び出す必要があります。
#ifdef	WINCE
	SHInitExtraControls();
#endif

#ifndef	WINCE
	// Win32 用にロケールを設定
	setlocale( LC_ALL, "Japanese" ); 
#endif

	// ファイルパス群を初期化
	m_filepath.init();

	// MZ3 ロガーの初期化
	m_logger.init( m_filepath.mz3logfile );

	// ログレベルの設定
#ifdef	MZ3_DEBUG
	// Release_MZ3 なので debug 有効
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
#else
#ifdef	_DEBUG
	// Debug 以外なので debug 有効
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
#else
	// リリースなので info まで有効
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_INFO );
#endif
#endif

	MZ3LOGGER_INFO( MZ3_APP_NAME L" 起動開始 " + util::GetSourceRevision() );
	MZ3_TRACE(L" sizeof MZ3Data : %d bytes\n", sizeof(MZ3Data));

/* lua support exp.
	// Lua の初期化
	g_luaState = lua_open();

	lua_register(g_luaState, "mz3_trace", lua_mz3_trace);
	luaL_dostring(g_luaState, "mz3_trace('xxx\\n');");

	lua_close(g_luaState);
*/
	// オプション読み込み
	m_optionMng.Load();

	// デバッグモードならログレベル変更
	if( m_optionMng.IsDebugMode() ) {
		m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
	}

	// オプションによってファイルパス群を再生成
	m_filepath.init_logpath();

	// ログイン情報の読み込み
	m_loginMng.Read();

	// ログ用INIファイルの読み込み
	MZ3LOGGER_DEBUG( L"ini ファイル読み込み開始" );
	m_readlog.Load( m_filepath.logfile );
	MZ3LOGGER_DEBUG( L"ini ファイル読み込み完了" );

	// スキン関連の初期化
	LoadSkinSetting();

	/*
	// MSXML 利用のためのコード、せっかくだから残しておくお
	{
		CMixiData rss_data;
		rss_data.SetAccessType(ACCESS_RSS_READER_FEED);
		rss_data.SetURL(L"http://b.hatena.ne.jp/hotentry?mode=rss");
#ifdef WINCE
		CString rss_data_path = L"\\hatena.rss.utf8";
#else
		CString rss_data_path = L"M:\\hatena.rss.utf8";
#endif
		MZ3_TRACE( L"rss_file_path : /%s/\n", rss_data_path );

		util::StopWatch sw;
		{
			CoInitializeEx(NULL, COINIT_MULTITHREADED);

			CComPtr<IXMLDOMDocument> iXMLDoc;
			CComPtr<IXMLDOMNodeList> iXMLList;

			VARIANT_BOOL bSuccess=false;

			if(S_OK == iXMLDoc.CoCreateInstance( __uuidof(DOMDocument) ) ){

				sw.start();

				for (int loop=0; loop<10; loop++) {
					// 同期読込
					iXMLDoc->put_async(VARIANT_FALSE);

					// load XML
					CComVariant         varFile = rss_data_path;
					iXMLDoc->load(varFile, &bSuccess);
					if (bSuccess) {
						iXMLDoc->selectNodes(_bstr_t("rdf:RDF/item/title"), &iXMLList);

						if (iXMLList != NULL) {

							_bstr_t _bstGet;
							for (int i=0;; i++) {
								CComPtr<IXMLDOMElement> iXMLInfo;

								iXMLList->get_item(i, (IXMLDOMNode**)&iXMLInfo);

								if (NULL != iXMLInfo) {
									iXMLInfo->get_text( &_bstGet.GetBSTR() );

									MZ3_TRACE( L" %d : /%s/\n", i, _bstGet.GetBSTR() );
								} else {
									break;
								}
							}

						}
					} else {
						MessageBox( NULL, L"load failed", L"", MB_OK );
					}
				}

				CString msg;
				msg.Format( L"elapsed time : %.2f [sec]\n", sw.stop() / 1000.0 );
				MessageBox(NULL, msg, L"", MB_OK);

			} else {
				MessageBox( NULL, L"object generate failed", L"", MB_OK );
			}
		}

		{
			CString rss_data_path = util::MakeLogfilePath(rss_data);

			sw.start();

			CHtmlArray html_;
			html_.Load( rss_data_path );

			// html_ の文字列化
			std::vector<TCHAR> text;
			html_.TranslateToVectorBuffer( text );

			for (int loop=0; loop<10; loop++) {

				// XML 解析
				xml2stl::Container root;
				if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
					MZ3LOGGER_ERROR( L"XML 解析失敗" );
				} else {
					// rdf:RDF/item に対する処理
					try {
						const xml2stl::Node& rdf = root.getNode(L"rdf:RDF");
						
						int counter = 0;
						for (unsigned int i=0; i<rdf.getChildrenCount(); i++) {
							const xml2stl::Node& item = rdf.getNode(i);
							if (item.getName() != L"item") {
								continue;
							}

							// description, title の取得
							CString title = item.getNode(L"title").getTextAll().c_str();

							MZ3_TRACE( L" %d : /%s/\n", counter, title );
							counter++;
						}

					} catch (xml2stl::NodeNotFoundException& e) {
						MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
					}
				}
			}

			CString msg;
			msg.Format( L"elapsed time : %.2f [sec]\n", sw.stop() / 1000.0 );
			MessageBox(NULL, msg, L"", MB_OK);
		}
	}
	*/

	// 詳細画面のクラス登録
#ifdef USE_RAN2
	Ran2View::RegisterWndClass(AfxGetInstanceHandle());
#endif

	// 画像キャッシュの初期化
	m_imageCache.Create();

	// トップページ用の初期化
	MZ3LOGGER_INFO( L"タブ設定ファイル読み込み開始" );
	bool bExistGroupFile = util::ExistFile( m_filepath.groupfile );
	if( Mz3GroupDataReader::load( m_accessTypeInfo, m_root, m_filepath.groupfile ) ) {
		MZ3LOGGER_INFO( L"タブ設定ファイル読み込み成功" );
	}else{
		MZ3LOGGER_INFO( L"タブ設定ファイル読み込み失敗" );
		// ロード失敗
		// ファイルが存在するのにロード失敗なら初期化確認。
		// ファイルが存在しないなら初期状態なので確認なしで初期化。
		if( bExistGroupFile ) {
			if( MessageBox( NULL, 
					L"グループ定義ファイルの読み込みに失敗しました。\n"
					L"デフォルトのグループ定義で起動しますか？\n\n"
					L"「いいえ」：" MZ3_APP_NAME L"を終了します。",
					MZ3_APP_NAME, MB_YESNO ) != IDYES ) 
			{
				MZ3LOGGER_INFO( L"ユーザによるキャンセルのため終了" );
				return FALSE;
			}
		}

		// 初期化
		MZ3LOGGER_INFO( L"タブ初期化" );
		// クライアント選択画面を表示
		CChooseClientTypeDlg dlg;
		if (dlg.DoModal()==IDOK) {
			// 設定された内容で初期化
			m_root.initForTopPage(m_accessTypeInfo, dlg.m_initType);
		}

		theApp.SaveGroupData();
	}

	// 移行処理
	switch (m_optionMng.m_StartupTransitionDoneType) {
	case option::Option::STARTUP_TRANSITION_DONE_TYPE_NONE:
		{
			// クライアント選択画面を表示
			CChooseClientTypeDlg dlg;
			if (dlg.DoModal()==IDOK) {
				// 設定された内容で初期化
				m_root.initForTopPage(m_accessTypeInfo, dlg.m_initType);
			}

			// 次回以降は表示しない
			m_optionMng.m_StartupTransitionDoneType = option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED;
			theApp.SaveGroupData();
		}
		break;

	case option::Option::STARTUP_TRANSITION_DONE_TYPE_TWITTER_MODE_ADDED:
		// 実行済み
		{
			// フォントサイズの移行処理
			MZ3LOGGER_DEBUG(util::FormatString(L"フォントサイズの移行処理：m_fontHeight : %d", m_optionMng.m_fontHeight));

			// px 換算値を pt 換算値に変換する
			int iDPI = theApp.GetDPI();
			m_optionMng.m_fontHeight       = ::MulDiv(m_optionMng.m_fontHeight       , 72, iDPI);
			m_optionMng.m_fontHeightBig    = ::MulDiv(m_optionMng.m_fontHeightBig    , 72, iDPI);
			m_optionMng.m_fontHeightMedium = ::MulDiv(m_optionMng.m_fontHeightMedium , 72, iDPI);
			m_optionMng.m_fontHeightSmall  = ::MulDiv(m_optionMng.m_fontHeightSmall  , 72, iDPI);

			MZ3LOGGER_DEBUG(util::FormatString(L"移行後：m_fontHeight : %d", m_optionMng.m_fontHeight));

			// 次回以降は処理しない
			m_optionMng.m_StartupTransitionDoneType = option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED;
			theApp.SaveGroupData();
		}
		break;

	case option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED:
	default:
		// 最新のため移行処理なし
		break;
	}

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
#ifdef WINCE
		IDR_MAINFRAME,
#else
		IDR_MAINFRAME_WIN32,
#endif
		RUNTIME_CLASS(CMZ3Doc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CMZ3View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// コマンド ラインで指定されたディスパッチ コマンドです。アプリケーションが
	// /RegServer、/Register、/Unregserver または /Unregister で起動された場合、FALSE を返します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_nCmdShow = SW_SHOWMAXIMIZED;

	CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

	CCreateContext newContext;
	newContext.m_pNewViewClass = NULL;
	newContext.m_pNewDocTemplate = NULL;
	newContext.m_pLastView = NULL;
	newContext.m_pCurrentFrame = NULL;
	newContext.m_pCurrentDoc = pCurrentDoc;

	m_pMouseGestureManager = new MouseGestureManager;

	m_pMainView		= (CMZ3View*)((CFrameWnd*)m_pMainWnd)->GetActiveView();
	m_pReportView	= new CReportView;
	m_pWriteView	= new CWriteView;
	m_pDownloadView	= new CDownloadView;

	// ビューの初期化
	INT viewID = AFX_IDW_PANE_FIRST + 1;
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	// レポートビューの初期化
	((CView*)m_pReportView)->Create(NULL, MZ3_APP_NAME _T(" ReportView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pReportView->OnInitialUpdate();

	// 書き込みビューの初期化
	((CView*)m_pWriteView)->Create(NULL, MZ3_APP_NAME _T(" WriteView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pWriteView->OnInitialUpdate();

	// ダウンロードビューの初期化
	((CView*)m_pDownloadView)->Create(NULL, MZ3_APP_NAME _T(" DownloadView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pDownloadView->OnInitialUpdate();

	// メイン ウィンドウが初期化されたので、表示と更新を行う
	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow();

#ifdef WINCE
	if( m_bSmartphone ) {
		// 全画面表示
		::SHFullScreen(m_pMainWnd->m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON );
		::ShowWindow(((CMainFrame*)m_pMainWnd)->m_hCommandBar, SW_HIDE);

		RECT rc;

		//get window size
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, FALSE);

		m_pMainWnd->MoveWindow( 
			rc.left, 
			rc.top, 
			rc.right, 
			rc.bottom + MZ3_TOOLBAR_HEIGHT, 
			TRUE);
	}
#endif

	// 初回起動時（ユーザID、パスワード未設定時）は
	// ユーザ設定画面を表示する。
	if ((wcslen(m_loginMng.GetEmail()) == 0 && wcslen(m_loginMng.GetPassword()) == 0) &&
		(wcslen(m_loginMng.GetTwitterId()) == 0 && wcslen(m_loginMng.GetTwitterPassword()) == 0)
		)
	{
		CString msg;
		msg += MZ3_APP_NAME;
		msg += L" をダウンロードしていただきありがとうございます！\n\n";
		msg += L"次の画面で初期設定（ログイン設定）を行ってください。";
		MessageBox( NULL, msg, MZ3_APP_NAME, MB_OK );

		// ユーザ設定画面を表示する
		((CMainFrame*)m_pMainWnd)->OnSettingLogin();
	}

	// フォーカスをメインビューに変更する
	m_pMainView->SetFocus();

//	if( m_bSmartphone ) {
#ifdef WINCE
	// 下記の理由からここで再描画
	// - Smartphone/Standard だと初期描画されないっぽい
	// - 統合カラムモードのサイズ調整が行われない
	((CMainFrame*)m_pMainWnd)->ChangeAllViewFont();
#endif

	// 絵文字定義ファイルのロード
	{
		inifile::IniFile emojifile;
		if (emojifile.Load( m_filepath.emojifile )) {
			LPCSTR szSection = "mixi";
			int start = atoi(emojifile.GetValue( "start", szSection ).c_str());
			int end   = atoi(emojifile.GetValue( "end", szSection ).c_str());
			for (int i=start; i<=end; i++) {
				std::string s = emojifile.GetValue( (LPCSTR)util::int2str_a(i), szSection );
				if (!s.empty()) {
					std::vector<std::string> values;
					if (!util::split_by_comma( values, s ) || values.size() < 3) {
						// ロードエラー
						CString msg;
						msg.Format( L"load emojifile error, id at %d", i );
						MZ3LOGGER_ERROR( msg );
					}
					m_emoji.push_back( 
						EmojiMap(util::my_mbstowcs(values[0]).c_str(), 
								 util::my_mbstowcs(values[1]).c_str(), 
								 util::my_mbstowcs(values[2]).c_str()) );
				}
			}
		}
	}
	// 未ダウンロードファイルがあればダウンロードマネージャに登録、起動
	if (theApp.m_optionMng.m_bAutoDownloadEmojiImageFiles) {
		bool bHasNoDownloadedEmojiFile = false;
		for (size_t i=0; i<m_emoji.size(); i++) {
			EmojiMap& emoji = m_emoji[i];
			CString path = util::MakeImageLogfilePathFromUrl( emoji.url );
			if (!util::ExistFile(path)) {
				DownloadItem item( emoji.url, emoji.text, path, true );
				m_pDownloadView->AppendDownloadItem( item );
				bHasNoDownloadedEmojiFile = true;
			}
		}
		if (bHasNoDownloadedEmojiFile) {
			CString msg;
			msg += "絵文字ファイルをダウンロードしますか？";
			if (MessageBox( m_pMainView->GetSafeHwnd(), msg, MZ3_APP_NAME, MB_YESNO ) == IDYES) {
				ChangeView( m_pDownloadView );
			} else {
				msg = L"次回起動時に絵文字ファイルをダウンロードしますか？";
				if (MessageBox( m_pMainView->GetSafeHwnd(), msg, MZ3_APP_NAME, MB_YESNO ) != IDYES) {
					theApp.m_optionMng.m_bAutoDownloadEmojiImageFiles = false;
					theApp.m_optionMng.Save();
				}
			}
		}
	}

	//--- テスト用コード
#ifdef DEBUG
	// 
	// encode_euc TEST
	//
	if (false) {
		CString src_text = L"";
		CString valid_text = L"";
		CString url_encoded_text = L"";

		// アルファベットTEST
		url_encoded_text = URLEncoder::encode_euc(L"a");
		ASSERT(url_encoded_text==L"a");

		// 2バイト文字TEST
		url_encoded_text = URLEncoder::encode_euc(L"あ");
		ASSERT(url_encoded_text==L"%A4%A2");

		// 記号TEST
		url_encoded_text = URLEncoder::encode_euc(L"!\"#$%&'()-=^~\\|@[]{}+*};:],./<>?_");
		ASSERT(url_encoded_text==L"%21%22%23%24%25%26%27%28%29-%3D%5E%7E%5C%7C%40%5B%5D%7B%7D%2B%2A%7D%3B%3A%5D%2C.%2F%3C%3E%3F_");

		// 200000文字TEST
		src_text = L"";
		valid_text = L"";
		for (int i=0; i<20000; i++) {
			src_text += L"あいうえおかきくけこ";
			valid_text += L"%A4%A2%A4%A4%A4%A6%A4%A8%A4%AA%A4%AB%A4%AD%A4%AF%A4%B1%A4%B3";
		}
		url_encoded_text = URLEncoder::encode_euc(src_text);
		ASSERT(url_encoded_text==valid_text);
	}

	// 
	// encode_utf8 TEST
	//
	if (0) {
		CString src_text = L"";
		CStringA valid_text = "";
		CStringA url_encoded_text = "";

		// アルファベットTEST
		url_encoded_text = URLEncoder::encode_utf8(L"a");
		ASSERT(url_encoded_text=="a");

		// 2バイト文字TEST
		url_encoded_text = URLEncoder::encode_utf8(L"あ");
		ASSERT(url_encoded_text=="%E3%81%82");

		// 記号TEST
		url_encoded_text = URLEncoder::encode_utf8(L"!\"#$%&'()-=^~\\|@[]{}+*};:],./<>?_");
		ASSERT(url_encoded_text=="%21%22%23%24%25%26%27%28%29-%3D%5E%7E%5C%7C%40%5B%5D%7B%7D%2B%2A%7D%3B%3A%5D%2C.%2F%3C%3E%3F_");

		// 200000文字TEST
		src_text = L"";
		valid_text = L"";
		for (int i=0; i<20000; i++) {
			src_text += L"あいうえおかきくけこ";
			valid_text += "%E3%81%82%E3%81%84%E3%81%86%E3%81%88%E3%81%8A%E3%81%8B%E3%81%8D%E3%81%8F%E3%81%91%E3%81%93";
		}
		url_encoded_text = URLEncoder::encode_utf8(src_text);
		ASSERT(url_encoded_text==valid_text);
	}

#endif

	MZ3LOGGER_INFO( MZ3_APP_NAME L" 初期化完了" );

	return TRUE;
}

/// プラットフォーム用のフラグの設定
void CMZ3App::InitPlatformFlags()
{
#ifdef WINCE
	// プラットフォーム名の問い合わせ
	TCHAR atchPlat[64];
	SystemParametersInfo(SPI_GETPLATFORMTYPE, 64, (PVOID)atchPlat, 0);

	if (_tcsncmp(atchPlat, L"PocketPC", 64) == 0)
	{
		m_bPocketPC = TRUE;
	}

	if (_tcsncmp(atchPlat, L"SmartPhone", 64) == 0)
	{
		m_bSmartphone = TRUE;
	}

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 01)) 
		m_bWinMoFiveOh = TRUE;
	if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 20)) 
		m_bWinMo2003 = TRUE;
	if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 21)) 
		m_bWinMo2003_SE = TRUE;
#endif
}

/**
 * 解像度/DPIの判定
 */
void CMZ3App::InitResolutionFlags()
{
	// 解像度の判定
#ifdef WINCE
	int horizontal = DRA::GetScreenCaps(HORZRES);
	int vertical   = DRA::GetScreenCaps(VERTRES);

	if( (horizontal == 240 && vertical == 320) || (horizontal == 320 && vertical == 240) ) {
		m_currentDisplayMode = SR_QVGA;
	}else if( (horizontal == 480 && vertical == 640) || (horizontal == 640 && vertical == 480) ) {
		m_currentDisplayMode = SR_VGA;
	}else if( (horizontal == 480 && vertical == 800) || (horizontal == 800 && vertical == 480) ) {
		// EM･ONE対応
		m_currentDisplayMode = SR_VGA;
	}else if( (horizontal == 240 && vertical == 240) ) {
		m_currentDisplayMode = SR_SQUARE240;
	}else{
		// デフォルト値
		m_currentDisplayMode = SR_QVGA;
	}
#endif

	// DPI 値の取得
	{
		HKEY  hKey = NULL;
		DWORD dwDpi = m_dpi;	// "Dpi"のデータを受け取る
		DWORD dwType;			// 値の種類を受け取る
		DWORD dwSize;			// データのサイズを受け取る

		// レジストリをオープン
		
		// エラーなければ値を取得
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, SZ_REG_ROOT, 0, KEY_READ, &hKey) == 0) 
		{ 
			RegQueryValueEx(hKey, SZ_REG_DBVOL, NULL, &dwType, NULL, &dwSize);
			RegQueryValueEx(hKey, SZ_REG_DBVOL, NULL, &dwType, (LPBYTE)&dwDpi, &dwSize);

			// エラーがなければ取得した値を保存。
			m_dpi = dwDpi;

			//レジストリのクローズ
			RegCloseKey(hKey);
		}
	}
}


int CMZ3App::ExitInstance()
{
#ifndef WINCE 
	::GdiplusShutdown(gdiToken);
#endif
//	CString msg;
//	msg.Format( L"ログファイルを\n%s\nに保存します", logfile );
//	MessageBox( NULL, msg, 0, MB_OK );

	// 詳細画面のクラス登録解除
#ifdef USE_RAN2
	Ran2View::UnregisterWndClass(AfxGetInstanceHandle());
#endif

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" 終了処理開始" );

	// ログファイルの保存
	if( m_readlog.Save( m_filepath.logfile ) == false ) {
		MessageBox( NULL, L"ログファイル保存失敗", m_filepath.logfile, MB_OK );
	}

	// オプションの保存
	m_optionMng.Save();

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" 終了処理完了" );

	MZ3LOGGER_INFO( MZ3_APP_NAME L" 終了" );

	return CWinApp::ExitInstance();
}

/// バージョン情報
void CMZ3App::OnAppAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

/**
 * ビュー切り替え
 */
void CMZ3App::ChangeView( CView* pNewView )
{
	// 現在のビューのウィンドウIDの変更
	CView* pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
	UINT tempId = ::GetWindowLong( pActiveView->m_hWnd, GWL_ID );
	::SetWindowLong( pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID) );
	::SetWindowLong( pNewView->m_hWnd,    GWL_ID, tempId );

	//--- 現在のビュー
	// 非表示化
	pActiveView->ShowWindow( SW_HIDE );

	// MZ3 非表示完了通知
	::SendMessage( pActiveView->m_hWnd, WM_MZ3_HIDE_VIEW, NULL, NULL );

	//--- 切り替え先ビュー
	pNewView->ShowWindow( SW_SHOW );
	((CFrameWnd*)m_pMainWnd)->SetActiveView( pNewView );
	((CFrameWnd*)m_pMainWnd)->RecalcLayout( TRUE );
	m_pMainWnd->UpdateWindow();

	// MZ3 切り替え完了通知
	::SendMessage( pNewView->m_hWnd, WM_MZ3_FIT, NULL, NULL );
}

/**
 * アプリケーション実行ディレクトリパスの取得
 */
CString CMZ3App::GetAppDirPath()
{
	CString appPath;

	{
		TCHAR path[256];

		memset(path, 0x00, sizeof(TCHAR) * 256);
		GetModuleFileName(NULL, path, 256);

		appPath = path;
	}

	// 最後の\を取得してそこから左を返す
	return appPath.Left(appPath.ReverseFind('\\'));
}

/**
 * ユーザ設定（E-Mailアドレス、パスワード）を用いて、ログインページの実行後のURLを生成する
 */
/*
CString CMZ3App::MakeLoginUrl( LPCTSTR nextUrl )
{
	// アドレス、パスワード、next_url を EUC エンコードする
	CString mail = URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() );
	CString pass = URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() );
	CString encodedNextUrl = URLEncoder::encode_euc( nextUrl );

	CString uri;
	uri.Format(_T("http://mixi.jp/login.pl?email=%s&password=%s&next_url=%s"),
		mail,
		pass,
		encodedNextUrl
		);

//	MessageBox( NULL, uri, MZ3_APP_NAME, MB_OK );
	return uri;
}
*/
/**
 * ユーザ設定（E-Mailアドレス、パスワード）を用いて、自動ログインURLを生成する
 */
/*
CString CMZ3App::MakeLoginUrlForMixiMobile( LPCTSTR nextUrl )
{
	// アドレス、パスワード、next_url を EUC エンコードする
	CString mail = URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() );
	CString pass = URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() );
	CString encodedNextUrl = URLEncoder::encode_euc( nextUrl );

	CString url;
	url.Format( 
		L"http://m.mixi.jp/login.pl?submit=main&type=full&mode=password&key=&next_url=%s&email=%s&password=%s",
		encodedNextUrl,
		mail,
		pass);

	return url;
}
*/

/**
 * mixi のログイン用通信を開始する
 */
void CMZ3App::StartMixiLoginAccess(HWND hwnd, CMixiData* data)
{
	//--- ここからログイン対応
	static CPostData post_data;

	CString url = L"http://mixi.jp/login.pl";
	LPCTSTR nextUrl = L"/home.pl";

	post_data.ClearPostBody();
	post_data.AppendPostBody(L"email=");
	post_data.AppendPostBody(URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() ));
	post_data.AppendPostBody(L"&");
	post_data.AppendPostBody(L"password=");
	post_data.AppendPostBody(URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() ));
	post_data.AppendPostBody(L"&");
	post_data.AppendPostBody(L"next_url=");
	post_data.AppendPostBody(URLEncoder::encode_euc( nextUrl ));

	// Content-Type: application/x-www-form-urlencoded
	post_data.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

	// 成功時のメッセージを設定する
	post_data.SetSuccessMessage( WM_MZ3_GET_END );

	data->SetURL( url );
	data->SetAccessType( ACCESS_LOGIN );
	theApp.m_accessType = ACCESS_LOGIN;

	// 通信開始
	CString refUrl = L"";
	theApp.m_inet.Initialize( hwnd, data );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		&post_data);
}

/// コマンドバーのボタンの有効・無効制御
BOOL CMZ3App::EnableCommandBarButton( int nID, BOOL bEnable )
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		return ((CMainFrame*)m_pMainWnd)->m_wndCommandBar.GetToolBarCtrl().EnableButton( nID, bEnable);
	} else {
		return TRUE;
	}
#else
	return ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().EnableButton( nID, bEnable);
#endif
}

/**
 * 共通フォント作成
 *
 * pBaseFont をベースに、高さを fontHeight, フォント名を fontFace に変更したフォントを作成し、
 * m_font とする。
 */
bool CMZ3App::MakeNewFont( CFont* pBaseFont, int fontHeight, LPCTSTR fontFace )
{
	CFont *pFont = pBaseFont;
	if (pFont)
	{
#ifdef WINCE
		int iDPI = ::GetDeviceCaps(NULL, LOGPIXELSY);
		BYTE fontQuality = NONANTIALIASED_QUALITY;
		if (iDPI==192) {
			// VGA(非RealVGA) ならClearType指定
			fontQuality = CLEARTYPE_QUALITY;
		}
#else
		// DPI 値の取得
		int iDPI = theApp.GetDPI();

		BYTE fontQuality = DEFAULT_QUALITY;
#endif
		int newHeight = - theApp.pt2px(fontHeight);	// 負数とすることで pixel 値指定とする

		MZ3LOGGER_DEBUG( 
			util::FormatString(L"CMZ3App::MakeNewFont(), dpi[%d], fontHeight[%d], newHeight[%d]",
				iDPI, fontHeight, newHeight) );

		theApp.m_font.Detach();
		theApp.m_font.CreateFont( 
			newHeight,					// nHeight
			0,							// nWidth
			0,							// nEscapement
			0,							// nOrientation
			FW_MEDIUM,					// nWeight
			FALSE,						// bItalic
			FALSE,						// bUnderline
			0,							// cStrikeOut
			DEFAULT_CHARSET,			// nCharSet
			OUT_DEFAULT_PRECIS,			// nOutPrecision
			CLIP_DEFAULT_PRECIS,		// nClipPrecision
			fontQuality,				// nQuality
			DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
			fontFace);					// lpszFacename
	}
	return true;
}

/**
 * CMixiData を表示する（デバッグ用）
 */
void CMZ3App::ShowMixiData( CMixiData* data )
{
	CString msg;

	if( data==NULL ) {
		return;
	}

	CDebugDlg dlg;
	dlg.m_data = data;
	dlg.DoModal();
}

/// ファイルパスの初期化。
/// ログ関連は除く。
void CMZ3App::FilePath::init()
{
	// MZ3 本体のログファイル
	mz3logfile.Format( L"%s\\mz3log.txt", theApp.GetAppDirPath() );

	// HTML 用一時ファイル(SJIS)
//	TCHAR temppath[256];
//	::GetTempPath(256, temppath);
//	temphtml.Format( _T("%s\\mz3s.dat"), temppath );
	temphtml.Format( _T("%s\\mz3s.dat"), theApp.GetAppDirPath() );

	// オプション用INIファイルのパス
	inifile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("mz3.ini"));

	// ログファイル（INIファイル）のパス
	logfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("log.ini"));

	// グループ定義ファイル（INIファイル）のパス
	groupfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("toppage_group.ini"));

	// Readme.txt のパス
	helpfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("Readme.txt"));

	// ChangeLog.txt のパス
	historyfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("ChangeLog.txt"));

	// temp_draftfile.txt のパス
	tempdraftfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("temp_draftfile.txt"));

	// 絵文字定義ファイル のパス
	emojifile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("emoji.ini"));

	// スキンフォルダのパス
	skinFolder.Format( L"%s\\skin", theApp.GetAppDirPath() );
}

/// ログ関連ファイルパスの初期化
void CMZ3App::FilePath::init_logpath()
{
	//--- フォルダ
	deleteTargetFolders.RemoveAll();

	// ログ用フォルダ
	// オプションで指定されていればそれを使う。
	// 指定されていなければ、デフォルト出力先を生成する。
	logFolder = theApp.m_optionMng.GetLogFolder();
	if( logFolder.IsEmpty() ) {
		logFolder.Format(_T("%s\\log"), theApp.GetAppDirPath());
	}
	CreateDirectory( logFolder, NULL/*always null*/ );

	// データ用フォルダの作成
	downloadFolder.Format(_T("%s\\download"), logFolder);
	CreateDirectory( downloadFolder, NULL/*always null*/ );
	// クリーンアップ対象フォルダに追加
	deleteTargetFolders.Add(downloadFolder);

	// ダウンロード済み画像ファイル用フォルダ
	imageFolder.Format(_T("%s\\image"), logFolder );
	CreateDirectory( imageFolder, NULL/*always null*/ );

	// 日記のログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\diary");

	// コミュニティ書き込みのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\bbs");

	// アンケートのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\enquete");

	// イベントのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\event");

	// プロフィールのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\profile");

	// メッセージのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\message");

	// ニュースのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\news");

	// Twitterのログ用フォルダのパス
	deleteTargetFolders.Add(logFolder + L"\\twitter");
}

/**
 * 情報領域などの高さをフォントの高さをベースに計算する
 */
int CMZ3App::GetInfoRegionHeight( int fontHeight )
{
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		if( theApp.GetDPI() > 96 ) {
			// VGA かつ非RealVGA環境
			return fontHeight +6*2;
		}else{
			// VGA かつRealVGA環境
			return fontHeight +6;
		}
		break;

	case SR_QVGA:
	default:
		// VGA 以外
		if (m_bSmartphone) {
			// Smartphone/Standard Edition 環境
			// X02HT での検証結果を受けて下記の値を設定。
			return fontHeight +6;
		} else {
			return fontHeight +6;
//			return fontHeight -4;
		}
	}
#else
	return fontHeight +6*2;
#endif
}

/**
 * タブの高さをフォントの高さをベースに計算する
 */
int CMZ3App::GetTabHeight( int fontHeight )
{
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		if( theApp.GetDPI() > 96 ) {
			// VGA かつ非RealVGA環境
			return fontHeight + 6*2;
		}else{
			// VGA かつRealVGA環境
			return fontHeight + 8;
		}
		break;

	case SR_QVGA:
	default:
		// VGA 以外
		if( theApp.m_bSmartphone ) {
			// Smartphone/Standard Edition 環境
			return fontHeight +8;
		} else {
			return fontHeight +8;
//			return fontHeight -2;
		}
	}
#else
	// for win32
	return fontHeight + 6*2;
#endif
}

bool CMZ3App::LoadSkinSetting()
{
	// 設定ファイルのロード
	m_skininfo.loadSkinInifile( theApp.m_optionMng.m_strSkinname );

	// 各画像ファイル名の初期化
	theApp.m_bgImageMainBodyCtrl.setImageFileName( m_skininfo.strMainBodyCtrlImageFileName );
	theApp.m_bgImageMainCategoryCtrl.setImageFileName( m_skininfo.strMainCategoryCtrlImageFileName );
	theApp.m_bgImageReportListCtrl.setImageFileName( m_skininfo.strReportListCtrlImageFileName );

	return true;
}

/**
 * グループ定義ファイルの保存
 */
bool CMZ3App::SaveGroupData(void)
{
	return Mz3GroupDataWriter::save( m_accessTypeInfo, m_root, m_filepath.groupfile );
}

/**
 * ログアウト状態かどうかを判定する。データ取得直後に呼び出すこと。
 */
bool CMZ3App::IsMixiLogout( ACCESS_TYPE aType )
{
	if (util::IsMixiAccessType(aType)) {
		if (mixi::MZ3ParserBase::IsLogout(theApp.m_filepath.temphtml) ) {
			return true;
		} else if (aType != ACCESS_MAIN && wcslen(theApp.m_loginMng.GetOwnerID())==0) {
			// オーナーID未取得の場合もログアウトとみなす。
			return true;
		}
	}
	return false;
}

/**
 * 古いキャッシュファイルの削除
 */
bool CMZ3App::DeleteOldCacheFiles(void)
{
	MZ3LOGGER_INFO( L"古いキャッシュファイルの削除開始" );
	int nDeleted = 0;	// 削除済みファイル数
	int nTargetFileLastWriteDaysBack = 30*2;	// 2ヶ月以上前のファイルを削除対象とする
	MZ3FileCacheManager cacheManager;
	cacheManager.DeleteFiles( m_filepath.deleteTargetFolders, &nDeleted, nTargetFileLastWriteDaysBack );
	MZ3LOGGER_INFO( util::FormatString(L"古いキャッシュファイルを削除しました：%d個", nDeleted) );

	return true;
}

/**
 * pt 単位の値を現在の DPI にあわせて px 単位に変換する
 */
int CMZ3App::pt2px(int pt)
{
	return ::MulDiv(pt, m_dpi, 72);
}

/**
 * image をリサイズし、画像キャッシュに追加する。
 */
int CMZ3App::AddImageToImageCache(CWnd* pWnd, CMZ3BackgroundImage& image, const CString& strImagePath)
{
	// 16x16, 32x32, 48x48 にリサイズする。
	CMZ3BackgroundImage image16(L""), image32(L""), image48(L"");
	util::MakeResizedImage( pWnd, image16, image, 16, 16 );
	util::MakeResizedImage( pWnd, image32, image, 32, 32 );
	util::MakeResizedImage( pWnd, image48, image, 48, 48 );

	// ビットマップの追加
	CBitmap bm16, bm32, bm48;
	bm16.Attach( image16.getHandle() );
	bm32.Attach( image32.getHandle() );
	bm48.Attach( image48.getHandle() );

	return theApp.m_imageCache.Add( &bm16, &bm32, &bm48, (CBitmap*)NULL, strImagePath );
}
