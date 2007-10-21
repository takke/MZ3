// MZ3.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"
#include "MZ3Doc.h"
#include "MZ3View.h"
#include "ReportView.h"
#include "WriteView.h"
#include "AboutDlg.h"
#include "util.h"
#include "DebugDlg.h"
#include "url_encoder.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3App

BEGIN_MESSAGE_MAP(CMZ3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMZ3App::OnAppAbout)
END_MESSAGE_MAP()


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
}

// 唯一の CMZ3App オブジェクトです。
CMZ3App theApp;

// CMZ3App 初期化

BOOL CMZ3App::InitInstance()
{
	// プラットフォーム判定
	InitPlatformFlags();

	// 解像度/DPI判定
	InitResolutionFlags();

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
	m_logfile.Load( m_filepath.logfile );
	MZ3LOGGER_DEBUG( L"ini ファイル読み込み完了" );

	// スキン関連の初期化
	LoadSkinSetting();

	// IE コンポーネントの初期化
#ifdef WINCE
	if (m_optionMng.m_bRenderByIE) {	// TODO この条件はIE正式対応前のための仮設定。正式対応後は常に実行すること。
		InitHTMLControl( AfxGetInstanceHandle() );
	}
#endif

	// トップページ用の初期化
	MZ3LOGGER_INFO( L"タブ設定ファイル読み込み開始" );
	bool bExistGroupFile = util::ExistFile( theApp.m_filepath.groupfile );
	if( Mz3GroupDataReader::load( m_root, theApp.m_filepath.groupfile ) ) {
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
		m_root.initForTopPage();
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

	m_pMainView		= (CMZ3View*)((CFrameWnd*)m_pMainWnd)->GetActiveView();
	m_pReportView	= new CReportView;
	m_pWriteView	= new CWriteView;

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
	if( wcslen(m_loginMng.GetEmail()) == 0 ||
		wcslen(m_loginMng.GetPassword()) == 0 )
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

	if( m_bSmartphone ) {
		// Smartphone/Standard だと初期描画されないっぽいので、ここで再描画しておく
		((CMainFrame*)m_pMainWnd)->ChangeAllViewFont();
	}

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
//	CString msg;
//	msg.Format( L"ログファイルを\n%s\nに保存します", logfile );
//	MessageBox( NULL, msg, 0, MB_OK );

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" 終了処理開始" );

	// ログファイルの保存
	if( m_logfile.Save( m_filepath.logfile ) == false ) {
		MessageBox( NULL, L"ログファイル保存失敗", m_filepath.logfile, MB_OK );
	}

	// オプションの保存
	m_optionMng.Save();

	// グループ定義ファイルの保存
	Mz3GroupDataWriter::save( m_root, m_filepath.groupfile );

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
	// 現在のビュー
	CView* pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
	UINT temp = ::GetWindowLong( pActiveView->m_hWnd, GWL_ID );
	::SetWindowLong( pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID) );
	::SetWindowLong( pNewView->m_hWnd, GWL_ID, temp );

	pActiveView->ShowWindow( SW_HIDE );
	pNewView->ShowWindow( SW_SHOW );
	((CFrameWnd*)m_pMainWnd)->SetActiveView( pNewView );
	((CFrameWnd*)m_pMainWnd)->RecalcLayout( TRUE );
	m_pMainWnd->UpdateWindow();

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

/**
 * ユーザ設定（E-Mailアドレス、パスワード）を用いて、自動ログインURLを生成する
 */
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
		LOGFONT lf;
		pFont->GetLogFont( &lf );

		if( fontHeight != 0 ) {
			lf.lfHeight = fontHeight;
		}
		if( wcslen(fontFace) > 0 ) {
			wcscpy_s( lf.lfFaceName, 31, fontFace );
		}

		theApp.m_font.Detach();
		theApp.m_font.CreateFontIndirect( &lf );
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
	TCHAR temppath[256];
	::GetTempPath(256, temppath);

	// MZ3 本体のログファイル
	mz3logfile.Format( L"%s\\mz3log.txt", theApp.GetAppDirPath() );

	// HTML 用一時ファイル(SJIS)
	temphtml.Format( _T("%s\\mz3s.dat"), temppath );

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

	// スキンフォルダのパス
	skinFolder.Format( L"%s\\skin", theApp.GetAppDirPath() );
}

/// ログ関連ファイルパスの初期化
void CMZ3App::FilePath::init_logpath()
{
	//--- フォルダ

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

	// ダウンロード済み画像ファイル用フォルダ
	imageFolder.Format(_T("%s\\image"), logFolder );
	CreateDirectory( imageFolder, NULL/*always null*/ );

	// 日記のログ用フォルダのパス
	diaryFolder.Format(_T("%s\\diary"), logFolder );
	CreateDirectory( diaryFolder, NULL/*always null*/ );

	// コミュニティ書き込みのログ用フォルダのパス
	bbsFolder.Format(_T("%s\\bbs"), logFolder );
	CreateDirectory( bbsFolder, NULL/*always null*/ );

	// アンケートのログ用フォルダのパス
	enqueteFolder.Format(_T("%s\\enquete"), logFolder );
	CreateDirectory( enqueteFolder, NULL/*always null*/ );

	// イベントのログ用フォルダのパス
	eventFolder.Format(_T("%s\\event"), logFolder );
	CreateDirectory( eventFolder, NULL/*always null*/ );

	// メッセージのログ用フォルダのパス
	messageFolder.Format(_T("%s\\message"), logFolder );
	CreateDirectory( messageFolder, NULL/*always null*/ );

	// ニュースのログ用フォルダのパス
	newsFolder.Format(_T("%s\\news"), logFolder );
	CreateDirectory( newsFolder, NULL/*always null*/ );

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
	default:
		// VGA 以外
		if (m_bSmartphone) {
			return fontHeight +6;
		} else {
			return fontHeight -4;
		}
	}
#else
	return fontHeight +6*2;
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
