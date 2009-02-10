/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"
#include "OptionSheet.h"
#include "ReportView.h"
#include "WriteView.h"
#include "DownloadView.h"
#include "MZ3View.h"
#include "Ran2View.h"
#include "UserDlg.h"

#include "util.h"
#include "util_gui.h"
#include "url_encoder.h"
#include "version.h"

// MZ4 の初回起動時のウィンドウサイズ
#define MZ4_WINDOW_DEFAULT_SIZE_X	400
#define MZ4_WINDOW_DEFAULT_SIZE_Y	600

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_COMMAND(ID_BACK_BUTTON, OnBackButton)
    ON_COMMAND(ID_FORWARD_BUTTON, OnForwardButton)
    ON_COMMAND(ID_AUTO_RELOAD_BUTTON, OnAutoReloadButton)
    ON_COMMAND(ID_STOP_BUTTON, OnStopButton)
	ON_COMMAND(ID_SETTING_LOGIN, &CMainFrame::OnSettingLogin)
	ON_COMMAND(ID_SETTING_GENERAL, &CMainFrame::OnSettingGeneral)
	ON_COMMAND(ID_MENU_CLOSE, &CMainFrame::OnMenuClose)
	ON_COMMAND(IDM_GETPAGE_ALL, &CMainFrame::OnGetpageAll)
	ON_COMMAND(IDM_GETPAGE_LATEST10, &CMainFrame::OnGetpageLatest10)
	ON_COMMAND(ID_CHANGE_FONT_BIG, &CMainFrame::OnChangeFontBig)
	ON_COMMAND(ID_CHANGE_FONT_MEDIUM, &CMainFrame::OnChangeFontMedium)
	ON_COMMAND(ID_CHANGE_FONT_SMALL, &CMainFrame::OnChangeFontSmall)
	ON_COMMAND(ID_MENU_BACK, &CMainFrame::OnMenuBack)
	ON_COMMAND(ID_MENU_NEXT, &CMainFrame::OnMenuNext)
	ON_COMMAND(IDM_CHECK_NEW, &CMainFrame::OnCheckNew)
	ON_COMMAND(ID_HELP_MENU, &CMainFrame::OnHelpMenu)
	ON_COMMAND(ID_HISTORY_MENU, &CMainFrame::OnHistoryMenu)
	ON_COMMAND(IDM_START_CRUISE, &CMainFrame::OnStartCruise)
	ON_COMMAND(IDM_START_CRUISE_UNREAD_ONLY, &CMainFrame::OnStartCruiseUnreadOnly)
//	ON_COMMAND(IDM_OPEN_MIXI_MOBILE_BY_BROWSER, &CMainFrame::OnOpenMixiMobileByBrowser)
	ON_COMMAND(ID_ERRORLOG_MENU, &CMainFrame::OnErrorlogMenu)
	ON_COMMAND(ID_CHANGE_SKIN, &CMainFrame::OnChangeSkin)
	ON_COMMAND(ID_MENU_ACTION, &CMainFrame::OnMenuAction)
	ON_COMMAND(ID_ENABLE_INTERVAL_CHECK, &CMainFrame::OnEnableIntervalCheck)
	ON_COMMAND(ID_MENU_CHECK_UPDATE, &CMainFrame::OnCheckUpdate)
	ON_COMMAND_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnSkinMenuItem)
    ON_UPDATE_COMMAND_UI(ID_BACK_BUTTON, OnUpdateBackButton)
    ON_UPDATE_COMMAND_UI(ID_FORWARD_BUTTON, OnUpdateForwardButton)
    ON_UPDATE_COMMAND_UI(ID_STOP_BUTTON, OnUpdateStopButton)
    ON_UPDATE_COMMAND_UI(ID_IMAGE_BUTTON, OnUpdateImageButton)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_UPDATE_COMMAND_UI(ID_OPEN_BROWSER, OnUpdateBrowserButton)
	ON_UPDATE_COMMAND_UI(ID_AUTO_RELOAD_BUTTON, OnUpdateAutoReloadButton)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_ALL, &CMainFrame::OnUpdateGetpageAll)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_LATEST10, &CMainFrame::OnUpdateGetpageLatest10)
	ON_UPDATE_COMMAND_UI(ID_MENU_BACK, &CMainFrame::OnUpdateMenuBack)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT, &CMainFrame::OnUpdateMenuNext)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_INTERVAL_CHECK, &CMainFrame::OnUpdateEnableIntervalCheck)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnUpdateSkinMenuItem)
	ON_UPDATE_COMMAND_UI(ID_MENU_CHECK_UPDATE, &CMainFrame::OnUpdateCheckUpdate)
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_COMMAND(ID_DOWNLOAD_MANAGER_VIEW, &CMainFrame::OnDownloadManagerView)
	ON_COMMAND(ID_MENU_OPEN_URL, &CMainFrame::OnMenuOpenUrl)
	ON_COMMAND(ID_MENU_OPEN_LOCAL_FILE, &CMainFrame::OnMenuOpenLocalFile)
	ON_COMMAND(ID_MENU_FONT_MAGNIFY, &CMainFrame::OnMenuFontMagnify)
	ON_COMMAND(ID_MENU_FONT_SHRINK, &CMainFrame::OnMenuFontShrink)
	ON_COMMAND(ID_MENU_RELOAD_LUA_SCRIPTS, &CMainFrame::OnMenuReloadLuaScripts)
	ON_COMMAND(ID_MENU_RELOAD_CATEGORY_LIST_LOG, &CMainFrame::OnMenuReloadCategoryListLog)
END_MESSAGE_MAP()


// CMainFrame コンストラクション/デストラクション

// -----------------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------------
CMainFrame::CMainFrame()
	: m_bBackPageEnabled(FALSE)
	, m_bForwardPageEnabled(FALSE)
#ifdef WINCE
	, m_hMenu(NULL)
#endif
{

}

// -----------------------------------------------------------------------------
// デストラクタ
// -----------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}

/**
 * CREATE イベント
 */



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifdef WINCE
/*
	if( theApp.m_bPocketPC ) {
		// dpi値によってツールバーの画像を変更
		int id_toolbar = 0;

		// RealVGA化していたら(dpi値が96以下だったら)コマンドバーに小さい画像を使用。それより大きいDpiは大きいの。
		if (theApp.GetDPI() <= 96) {
			id_toolbar = IDR_TOOLBAR_QVGA;
		} else {
			id_toolbar = IDR_TOOLBAR;
		}

		if (!m_wndCommandBar.Create(this) ||
			!m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
			!m_wndCommandBar.AddAdornments(CMDBAR_OK) ||
			!m_wndCommandBar.LoadToolBar(id_toolbar)) {
				MZ3LOGGER_FATAL(L"CommandBar の作成に失敗しました\n");
				return -1;      // 作成できませんでした。
		}

		m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
	}
*/

	// メニューバーを作成する
	{
		m_hMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

		SHMENUBARINFO mbi;

		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize = sizeof(SHMENUBARINFO);
		mbi.hwndParent = m_hWnd;
		mbi.dwFlags = SHCMBF_HMENU;
		mbi.nToolBarId = (UINT)m_hMenu;
		mbi.hInstRes = AfxGetInstanceHandle();

		if (SHCreateMenuBar(&mbi)==FALSE) {
			MZ3LOGGER_FATAL(L"Couldn't create menu bar");
		}
		m_hwndMenuBar = mbi.hwndMB;
	}
#endif

#ifndef WINCE
	// ツールバーの生成
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		                       WS_CHILD | WS_VISIBLE | 
							   CBRS_TOP | 
							   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		MZ3LOGGER_FATAL(L"ツールバー の作成に失敗しました");
		return -1;      // fail to create
	}

    //ツールバーをロード
//	int idToolbar = IDR_TOOLBAR;
//	int nToolbarWidth  = 32;
//	int nToolbarHeight = 32;
	int idToolbar = IDR_TOOLBAR_WIN32;
	int nToolbarWidth  = 16;
	int nToolbarHeight = 16;

	m_wndToolBar.LoadToolBar(idToolbar);

	// 256 色ビットマップをロード
	HBITMAP hbm = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(idToolbar),
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	CBitmap bm;
	bm.Attach(hbm);

	// 256 色ビットマップを割り当て
	int nBtnCnt = 8;	//ボタンの数
	static CImageList m_ImageList;
	m_ImageList.Create( nToolbarWidth, nToolbarHeight, ILC_COLOR32 | ILC_MASK, nBtnCnt, 1 );
	m_ImageList.Add(&bm, (CBitmap*)NULL);
	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ImageList);


	//  スタイルを設定する
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );		//  ドッキングモードを設定
#endif

#ifdef WINCE
	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	// ドキュメントの名称を表示しない
	cs.style &= ~FWS_ADDTOTITLE;

#ifndef WINCE
	cs.cx = MZ4_WINDOW_DEFAULT_SIZE_X;
	cs.cy = MZ4_WINDOW_DEFAULT_SIZE_Y;

	// 前回のサイズを復帰する
	if (theApp.m_optionMng.m_strWindowPos.GetLength() > 16) {
		LPCTSTR strrc = theApp.m_optionMng.m_strWindowPos;
		CRect rc(_wtoi(strrc), _wtoi(strrc+5), _wtoi(strrc+10), _wtoi(strrc+15));
		// タスクバーの位置による調整を行う
		CRect desktoprct;
		SystemParametersInfo(SPI_GETWORKAREA,0,&desktoprct,0);
		rc.OffsetRect( desktoprct.left , desktoprct.top );

		cs.cx = rc.Width();
		cs.cy = rc.Height();

		cs.x  = rc.left;
		cs.y  = rc.top;
	}

	// タイトル変更
	MySetTitle();
#else
#endif

	return TRUE;
}

// CMainFrame 診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
#endif //_DEBUG

// CMainFrame メッセージ ハンドラ


/**
 * 戻るボタン
 */
void CMainFrame::OnBackButton()
{
#ifndef WINCE
	if (!m_bBackPageEnabled) {
		// ボタンが非活性なら処理しない（直接呼ばれる場合があるので）
		return;
	}
#endif

	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// レポートビュー → メインビュー

		// 終了処理
		((CReportView*)pActiveView)->EndProc();

		m_bForwardPageEnabled = TRUE;
		m_bBackPageEnabled = FALSE;

		// 戻る
		::SendMessage( theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL );
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// Write ビュー（日記, 新規メッセージ） → メインビュー
		// Write ビュー（日記以外）             → レポートビュー

		if( theApp.m_pWriteView->IsWriteFromMainView() ) {
			// Write ビュー（日記, 新規メッセージ） → メインビュー

			// 書きかけのデータがあるかどうかの判定
			if (!theApp.m_pWriteView->IsWriteCompleted()) {
				// 未送信データ有り
				int ret = ::MessageBox(m_hWnd, _T("未投稿のデータがあります\n破棄されますがよろしいですか？"),
					MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
				if (ret == IDCANCEL) {
					// 処理を中止
					return;
				}
				theApp.m_pWriteView->SetWriteCompleted(true);
			}

			m_bForwardPageEnabled = FALSE;
			m_bBackPageEnabled = FALSE;

			theApp.ChangeView(theApp.m_pMainView);
		} else {
			// Write ビュー（日記以外） → レポートビュー
			m_bForwardPageEnabled = TRUE;
			m_bBackPageEnabled = TRUE;

			// ReportViewに戻る
			theApp.ChangeView(theApp.m_pReportView);
		}
		return;
	}

	if (pActiveView == theApp.m_pDownloadView) {
		// ダウンロードビュー → メインビュー
		m_bForwardPageEnabled = FALSE;
		m_bBackPageEnabled = FALSE;

		// 戻る
		theApp.ChangeView(theApp.m_pMainView);

		// メイン画面のリロード(アイコンリロードの可能性があるため)
		theApp.m_pMainView->OnSelchangedGroupTab();

		return;
	}


	//--- メインビュー
	// 戻り先はないが、とりあえず変更イベントを送っておく
	::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);

}

/**
 * 進むボタン
 */
void CMainFrame::OnForwardButton()
{
#ifndef WINCE
	if (!m_bForwardPageEnabled) {
		// ボタンが非活性なら処理しない（直接呼ばれる場合があるので）
		return;
	}
#endif

	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// レポートビュー → 書き込みビュー
		// 但し、未送信の場合のみ。
		if (!theApp.m_pWriteView->IsWriteCompleted()) {
			m_bForwardPageEnabled = FALSE;
			m_bBackPageEnabled = TRUE;

			theApp.ChangeView( theApp.m_pWriteView );
		}
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// 書き込みビュー → （遷移先なし）
		return;
	}

	if (pActiveView == theApp.m_pMainView ) {
		// メインビュー → レポートビュー

		// 書き込みビューに行けるなら、NEXT ボタンを有効に。
		// 送信完了フラグ(IsWriteCompleted())がOFFなら、「書き込みビューに行ける」と判断する
		m_bForwardPageEnabled = theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE;
		m_bBackPageEnabled = TRUE;

		theApp.ChangeView(theApp.m_pReportView);
	}
}

/**
 * 定期取得ボタン
 */
void CMainFrame::OnAutoReloadButton()
{
	// メニューのハンドラに委譲
	OnEnableIntervalCheck();
}

// -----------------------------------------------------------------------------
// ログイン設定ボタン
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingLogin()
{
	CUserDlg dlg;
	dlg.DoModal();
}

// -----------------------------------------------------------------------------
// ストップボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateStopButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// 戻るボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBackButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bBackPageEnabled); 
}

// -----------------------------------------------------------------------------
// 進むボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateForwardButton(CCmdUI* pCmdUI)
{
#ifndef WINCE
	pCmdUI->Enable(m_bForwardPageEnabled);
#endif
}

// -----------------------------------------------------------------------------
// 画像ボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateImageButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));  
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// 書き込みボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON) );
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// ブラウザボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBrowserButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));  
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));
#endif
}

// -----------------------------------------------------------------------------
// 定期取得ボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateAutoReloadButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	// WinCE は定期取得ボタンなし
#else
	pCmdUI->Enable();
	pCmdUI->SetCheck( theApp.m_optionMng.m_bEnableIntervalCheck ? TRUE : FALSE );
#endif
}

// -----------------------------------------------------------------------------
// 設定
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingGeneral()
{
	if( theApp.m_bSmartphone ) {
		// Smartphone/Standard Edition では COptionSheet 等が利用できないため、警告メッセージを表示する。
		MessageBox( 
			L"Smartphone/Standard Edition ではオプション画面を表示できません。\n"
			L"お手数ですが、mz3.ini を直接編集してください" );
	} else {
#ifndef SMARTPHONE2003_UI_MODEL
		static int s_iLastActivePage = 1;

		COptionSheet cPropSht( _T("オプション"));
		
		// ページ生成
		cPropSht.SetPage();

		// アクティブページの設定
		if (s_iLastActivePage < 0 || s_iLastActivePage >= cPropSht.GetPageCount()) {
			s_iLastActivePage = 0;	// 初期化
		}
		cPropSht.SetActivePage(s_iLastActivePage);
		if (cPropSht.DoModal() == IDOK) {

			// iniファイルの保存
			theApp.m_optionMng.Save();

			// フォントのリセット
			ChangeAllViewFont();

			// メイン画面のリロード
			theApp.m_pMainView->OnSelchangedGroupTab();
		}

		// アクティブページのインデックスを保存しておく
		s_iLastActivePage = cPropSht.m_iLastPage;
#endif
	}
}

/// 「閉じる」メニューイベントハンドラ
void CMainFrame::OnMenuClose()
{
	int iRet;
	iRet = ::MessageBox(m_hWnd, MZ3_APP_NAME _T("を終了しますか？"), MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDOK) {
		theApp.m_pReportView->SaveIndex();
		ShowWindow(SW_HIDE);
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}
}

/// 「全件取得」メニュー押下イベント
void CMainFrame::OnGetpageAll()
{
	theApp.m_optionMng.SetPageType( GETPAGE_ALL );
}

/// 「全件取得」メニューのチェック状態制御
void CMainFrame::OnUpdateGetpageAll(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_ALL ) {
		pCmdUI->SetCheck( TRUE );
	}else{
		pCmdUI->SetCheck( FALSE );
	}
}

/// 「最新１０件取得」メニュー押下イベント
void CMainFrame::OnGetpageLatest10()
{
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST20 );
}

/// 「最新１０件取得」メニューのチェック状態制御
void CMainFrame::OnUpdateGetpageLatest10(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST20 ) {
		pCmdUI->SetCheck( TRUE );
	}else{
		pCmdUI->SetCheck( FALSE );
	}
}

/// 「文字サイズ｜大」メニュー
void CMainFrame::OnChangeFontBig()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightBig;
	ChangeAllViewFont();
}

/// 「文字サイズ｜中」メニュー
void CMainFrame::OnChangeFontMedium()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightMedium;
	ChangeAllViewFont();
}

/// 「文字サイズ｜小」メニュー
void CMainFrame::OnChangeFontSmall()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightSmall;
	ChangeAllViewFont();
}

inline void MySetListViewFont( CListCtrl* pListCtrl )
{
	// いったん、オーナー固定描画を解除する
	DWORD dwStyle = pListCtrl->GetStyle();
	pListCtrl->ModifyStyle(LVS_OWNERDRAWFIXED, 0);

	// フォント変更
	pListCtrl->SetFont( &theApp.m_font );

	// オーナー固定描画再開
	pListCtrl->ModifyStyle( 0, LVS_OWNERDRAWFIXED );
}

/**
 * 全てのビューのフォントを変更する
 */
bool CMainFrame::ChangeAllViewFont(int fontHeight)
{
	if (fontHeight < 0) {
		// デフォルト値補正
		fontHeight = theApp.m_optionMng.m_fontHeight;
	}

	// フォントの作成
	theApp.MakeNewFont( theApp.m_pMainView->GetFont(), fontHeight, theApp.m_optionMng.GetFontFace() );

	//--- メインビュー
	{
		CMZ3View* pView = (CMZ3View*)theApp.m_pMainView;

		// グループタブ
		pView->m_groupTab.SetFont( &theApp.m_font );

		// カテゴリリスト
		MySetListViewFont( &pView->m_categoryList );

		// ボディリスト
		MySetListViewFont( &pView->m_bodyList );

		// インフォメーションエディット
		pView->m_infoEdit.SetFont( &theApp.m_font );

		// ビュー内容の再設定
		pView->ResetViewContent();
	}

	//--- レポートビュー
	{
		CReportView* pView = theApp.m_pReportView;

		// タイトル
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// リスト
		MySetListViewFont( &pView->m_list );

		// RAN2エディット
		pView->m_detailView->ChangeViewFont( fontHeight, theApp.m_optionMng.GetFontFace() );
		pView->m_detailView->DrawDetail(0);

		// 通知領域
		pView->m_infoEdit.SetFont( &theApp.m_font );
	}

	//--- 書き込みビュー
	{
		CWriteView* pView = theApp.m_pWriteView;

		// タイトル
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// エディット
		pView->m_bodyEdit.SetFont( &theApp.m_font );

		// ボタン
		pView->m_sendButton.SetFont( &theApp.m_font );
		pView->m_cancelButton.SetFont( &theApp.m_font );

		// 通知領域
		pView->m_infoEdit.SetFont( &theApp.m_font );

		// 公開範囲コンボボックス
		pView->m_viewlimitCombo.SetFont( &theApp.m_font );
	}

	// サイズ変更
	{
#ifdef WINCE
		CRect rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int w = rect.Width();
		int h = rect.Height() - rect.top;

		// Smartphone/Standard Edition の場合はツールバーの分だけ微調整
		if( theApp.m_bSmartphone ) {
			h += MZ3_TOOLBAR_HEIGHT;
		}

		SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );
#else
		// Win32 の場合は現在の大きさを継続する
		// (0,0) を送ることで、前回のサイズ値を再利用する。
		int w = 0;
		int h = 0;
#endif

		// 各Viewに通知を送る
		theApp.m_pMainView->OnSize( 0, w, h );
		theApp.m_pReportView->OnSize( 0, w, h );
		theApp.m_pWriteView->OnSize( 0, w, h );
	}
	return false;
}

/// 画面｜前の画面メニューの制御
void CMainFrame::OnUpdateMenuBack(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bBackPageEnabled);
}

/// 画面｜次の画面メニューの制御
void CMainFrame::OnUpdateMenuNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bForwardPageEnabled);
}

/// 定期取得メニュー
void CMainFrame::OnEnableIntervalCheck()
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck == false) {
		// 有効になったので、メッセージを表示する
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
			(GetAsyncKeyState(VK_SHIFT) & 0x8000))
		{
			// Ctrl や Shift 押下時にはダイアログを出さない
		} else {
			CString msg;
			msg.Format( 
				L"定期取得機能は、カテゴリの項目を定期的に取得する機能です。\n"
				L"\n"
				L"・メイン画面を開いている場合のみ有効です。\n"
				L"・取得間隔はオプション画面で設定できます。\n"
				L"　（現在の取得間隔は【%d秒】です）"
				, theApp.m_optionMng.m_nIntervalCheckSec );
			MessageBox( msg );
		}

		// 設定する
		theApp.m_pMainView->ResetIntervalTimer();
	}

	// オプションのトグル
	theApp.m_optionMng.m_bEnableIntervalCheck = !theApp.m_optionMng.m_bEnableIntervalCheck;

	// タイトル変更
	MySetTitle();
}

/// 定期取得メニューの制御
void CMainFrame::OnUpdateEnableIntervalCheck(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( theApp.m_optionMng.m_bEnableIntervalCheck ? TRUE : FALSE );
}

/// バージョンチェック
void CMainFrame::OnCheckUpdate()
{
	// メイン画面であれば利用可能
	CView* pActiveView = GetActiveView();
	if (pActiveView == theApp.m_pMainView) {
		theApp.m_pMainView->DoCheckSoftwareUpdate();
	}
}

/// バージョンチェックメニューの制御
void CMainFrame::OnUpdateCheckUpdate(CCmdUI *pCmdUI)
{
	// メイン画面であれば利用可能
	CView* pActiveView = GetActiveView();
	if (pActiveView == theApp.m_pMainView) {
		pCmdUI->Enable();
	} else {
		pCmdUI->Enable(FALSE);
	}
}

/// 画面｜前の画面メニューのイベント
void CMainFrame::OnMenuBack()
{
	OnBackButton();
}

/// 画面｜次の画面メニューのイベント
void CMainFrame::OnMenuNext()
{
	OnForwardButton();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

void CMainFrame::OnCheckNew()
{
	theApp.m_pMainView->DoNewCommentCheck();
}

/**
 * ヘルプ｜MZ3 ヘルプ
 */
void CMainFrame::OnHelpMenu()
{
#ifdef WINCE
	// Readme.txt を解析して表示
	theApp.m_pMainView->MyShowHelp();
#else
	// Win32 の場合はオンラインマニュアル表示
	util::OpenByShellExecute( MZ4_MANUAL_URL);
#endif
}

/**
 * ヘルプ｜改版履歴
 */
void CMainFrame::OnHistoryMenu()
{
	theApp.m_pMainView->MyShowHistory();
}

/**
 * エラーログを開く
 */
void CMainFrame::OnErrorlogMenu()
{
	theApp.m_pMainView->MyShowErrorlog();
}

/**
 * スキン変更。
 *
 * さらにスキン一覧のメニューを表示する
 */
void CMainFrame::OnChangeSkin()
{
	CMenu menu;
	menu.LoadMenu(IDR_SKIN_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// スキンフォルダの一覧を生成する
	std::vector<std::wstring> skinfileList;
	GetSkinFolderNameList(skinfileList);

	// リスト化
	for( int i=0; i<(int)skinfileList.size(); i++ ) {
		UINT flag = MF_STRING | MF_ENABLED;

		LPCTSTR filename = skinfileList[i].c_str();

		// 現在のスキンであればチェック
		if (filename == theApp.m_optionMng.m_strSkinname) {
			flag |= MF_CHECKED;
		}

		// スキンタイトルを取得
		CString title = CMZ3SkinInfo::loadSkinTitle( filename );

		// スキン名も追加する
		if (title != filename) {
			title.AppendFormat( L" (%s)", filename );
		}

		pcThisMenu->AppendMenu( flag, ID_SKIN_BASE+i, title );
	}

	// ダミーを削除
	pcThisMenu->DeleteMenu( ID_SKIN_DUMMY, MF_BYCOMMAND );

	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * スキン格納用フォルダからすべてのスキン用フォルダ名を取得する
 */
void CMainFrame::GetSkinFolderNameList(std::vector<std::wstring>& skinfileList)
{
    int    nResult = TRUE;
    HANDLE hFile  = INVALID_HANDLE_VALUE;
    TCHAR  szPath[ MAX_PATH ];
    WIN32_FIND_DATA data;

    //  ファイル探索
	_stprintf( szPath, _T("%s\\*"), (LPCTSTR)theApp.m_filepath.skinFolder);
    hFile = FindFirstFile(szPath, &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// ディレクトリ名発見。追加する。

				// . から始まるディレクトリは除外
				if (data.cFileName[0] != '.') {
					skinfileList.push_back( data.cFileName );
				}
			}
        }
        while( FindNextFile( hFile, &data) && nResult);

        FindClose( hFile );
    }

}

void CMainFrame::OnUpdateSkinMenuItem(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

/**
 * スキン切り替え
 */
void CMainFrame::OnSkinMenuItem(UINT nID)
{
	CWnd* pWnd = GetActiveView();

	// スキンの有効・無効チェック
	if (!theApp.m_optionMng.IsUseBgImage()) {
		return;
	}

	int n = nID - ID_SKIN_BASE;

	// スキンフォルダの一覧を生成する
	std::vector<std::wstring> skinfileList;
	GetSkinFolderNameList(skinfileList);

	// 入力チェック
	if (n < 0 || n >= (int)skinfileList.size()) {
		return;
	}

	// スキン名の解決
	LPCTSTR szSkinName = skinfileList[n].c_str();
//	MessageBox( szSkinName );

	// 現在のスキン名の保存
	CString strOriginalSkinName = theApp.m_optionMng.m_strSkinname;

	// スキン切り替え
	theApp.m_optionMng.m_strSkinname = szSkinName;

	// スキン情報の更新
	theApp.LoadSkinSetting();

	// スキンファイルチェック
	if (!theApp.m_bgImageMainBodyCtrl.isValidSkinfile(szSkinName) ||
		!theApp.m_bgImageMainCategoryCtrl.isValidSkinfile(szSkinName) ||
		!theApp.m_bgImageReportListCtrl.isValidSkinfile(szSkinName))
	{
		// スキンファイルが見つからないため終了

		// スキン名を戻す
		theApp.m_optionMng.m_strSkinname = strOriginalSkinName;
		
		// スキン情報の更新
		theApp.LoadSkinSetting();

		if (pWnd) {
			util::MySetInformationText( pWnd->GetSafeHwnd(), L"スキン画像ファイルが見つかりません" );
		}
		return;
	}

	// 背景画像をリロードする
	theApp.m_bgImageMainBodyCtrl.load();
	theApp.m_bgImageMainCategoryCtrl.load();
	theApp.m_bgImageReportListCtrl.load();

	// リロード
	ChangeAllViewFont();

	// メッセージ
	CString msg;
	if (theApp.m_skininfo.strSkinName != szSkinName) {
		msg.Format( L"スキンを [%s (%s)] に変更しました", theApp.m_skininfo.strSkinTitle, szSkinName );
	} else {
		msg.Format( L"スキンを [%s] に変更しました", theApp.m_skininfo.strSkinTitle );
	}
	if (pWnd) {
		util::MySetInformationText( pWnd->GetSafeHwnd(), msg );
	}
}

/**
 * その他｜全てを巡回
 */
void CMainFrame::OnStartCruise()
{
	theApp.m_pMainView->StartCruise( false );
}

/**
 * その他｜未読を巡回
 */
void CMainFrame::OnStartCruiseUnreadOnly()
{
	theApp.m_pMainView->StartCruise( true );
}

/**
 * その他｜mixiモバイルを開く
 */
/*
void CMainFrame::OnOpenMixiMobileByBrowser()
{
	CString url = theApp.MakeLoginUrlForMixiMobile( L"home.pl" );
	util::OpenUrlByBrowser( url );
}
*/

/**
 * 右ソフトキー押下イベント：ポップアップ
 */
void CMainFrame::OnMenuAction()
{
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pMainView) {
		// メインビュー
		theApp.m_pMainView->OnAcceleratorContextMenu();
		return;
	}

	if (pActiveView == theApp.m_pReportView) {
		// レポートビュー
		theApp.m_pReportView->MyPopupReportMenu();
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// Write ビュー
		theApp.m_pWriteView->PopupWriteBodyMenu();
		return;
	}

	if (pActiveView == theApp.m_pDownloadView) {
		// ダウンロードビュー
		// 処理なし
		return;
	}

}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// 終了時のタブ・カテゴリ選択状態を保存
	CMZ3View* pView = (CMZ3View*)theApp.m_pMainView;
	theApp.m_optionMng.m_lastTopPageTabIndex      = pView->m_groupTab.GetCurSel();
	theApp.m_optionMng.m_lastTopPageCategoryIndex = pView->m_selGroup->selectedCategory;

#ifndef WINCE
	// 終了時の位置・サイズを保存
	WINDOWPLACEMENT    wp;
    if (GetWindowPlacement(&wp)) {
		CString    cb;
		cb.Format( L"%04d %04d %04d %04d", 
			wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
		theApp.m_optionMng.m_strWindowPos = cb;
	}
#endif

#ifdef WINCE
	// メニューの破棄
	if (m_hMenu!=NULL) {
		DestroyMenu(m_hMenu);
	}
#endif
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR nmhdr = (LPNMHDR)lParam;

#ifndef WINCE
	switch (nmhdr->code) {
	case TTN_NEEDTEXT:
		{
			static LPTOOLTIPTEXT lptip;
			lptip = (LPTOOLTIPTEXT)lParam;
			switch (lptip->hdr.idFrom) {
			case ID_BACK_BUTTON:	lptip->lpszText = L"戻る";				break;
			case ID_FORWARD_BUTTON:	lptip->lpszText = L"進む";				break;
			case ID_STOP_BUTTON:	lptip->lpszText = L"停止";				break;
			case ID_WRITE_BUTTON:
				// View によって違う
				if (GetActiveView() == theApp.m_pMainView) {
					lptip->lpszText = L"日記を書く";
				} else {
					lptip->lpszText = L"コメントを書く";
				}
				break;
			case ID_IMAGE_BUTTON:		lptip->lpszText = L"画像を開く";		break;
			case ID_OPEN_BROWSER:		lptip->lpszText = L"ブラウザで開く";	break;
			case ID_AUTO_RELOAD_BUTTON:	lptip->lpszText = L"定期取得";			break;
			case ID_APP_ABOUT:			lptip->lpszText = L"バージョン情報";	break;
			}
		}
		break;
	default:
		break;
	}
#endif

	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

/// タイトル変更
void CMainFrame::MySetTitle(void)
{
#ifdef WINCE
	// タイトル変更
	CString title = MZ3_APP_NAME;

	// 定期取得状態
	if( theApp.m_optionMng.m_bEnableIntervalCheck ) {
		title += L".i";
	}

	SetTitle(title);
	if (m_hWnd) {
		SetWindowText(title);
	}
#else
	// タイトル変更
	CString title = MZ3_APP_NAME L" " MZ3_VERSION_TEXT;
	title.Replace( L"Version ", L"v" );

	// 定期取得状態
	if( theApp.m_optionMng.m_bEnableIntervalCheck ) {
		title += L" 【定期取得】";
	}

	SetTitle(title);
	if (m_hWnd) {
		SetWindowText(title);
	}
#endif
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);

	if (theApp.m_pMainView) {
		theApp.m_pMainView->MoveMiniImageDlg();
	}
}

void CMainFrame::OnDownloadManagerView()
{
	m_bForwardPageEnabled = FALSE;
	m_bBackPageEnabled = TRUE;

	theApp.ChangeView(theApp.m_pDownloadView);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
#ifndef WINCE
	switch (pMsg->message) {
	case WM_XBUTTONUP:
		{
			// IntelliMouseのサイドキーボタン
			switch (GET_XBUTTON_WPARAM(pMsg->wParam)) {
			case XBUTTON1:
				// first X button
				OnBackButton();
				return TRUE;
			case XBUTTON2:
				// second X button
				OnForwardButton();
				return TRUE;
			}
		}
		break;
	}
#endif

	return CFrameWnd::PreTranslateMessage(pMsg);
}

/**
 * 任意のURLを開く
 */
void CMainFrame::OnMenuOpenUrl()
{
	// メインビュー表示
	theApp.ChangeView(theApp.m_pMainView);

	// 委譲
	theApp.m_pMainView->MyOpenUrl();
}

/**
 * 任意のローカルファイルを開く
 */
void CMainFrame::OnMenuOpenLocalFile()
{
	// メインビュー表示
	theApp.ChangeView(theApp.m_pMainView);

	// 委譲
	theApp.m_pMainView->MyOpenLocalFile();
}

/// 「文字サイズ｜大きくする」メニュー
void CMainFrame::OnMenuFontMagnify()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight+1 );
	ChangeAllViewFont();
}

/// 「文字サイズ｜小さくする」メニュー
void CMainFrame::OnMenuFontShrink()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight-1 );
	ChangeAllViewFont();
}

/// 「スクリプトのリロード」メニュー
void CMainFrame::OnMenuReloadLuaScripts()
{
	// クローズ
	theApp.MyLuaClose();

	// 初期化
	theApp.MyLuaInit();
}

/// 「カテゴリログのリロード」メニュー
void CMainFrame::OnMenuReloadCategoryListLog()
{
	theApp.m_pMainView->ReloadCategoryListLog();
}
