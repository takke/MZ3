// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "MZ3.h"

#include "MainFrm.h"
#include "OptionSheet.h"
#include "OptionTabUser.h"
#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "ReportView.h"
#include "WriteView.h"
#include "MZ3View.h"
#include "UserDlg.h"
#include "util.h"
#include "url_encoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD dwAdornmentFlags = 0; // [終了] ボタン

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_COMMAND(ID_BACK_BUTTON, OnBackButton)
    ON_COMMAND(ID_FORWARD_BUTTON, OnForwardButton)
    ON_COMMAND(ID_STOP_BUTTON, OnStopButton)
    ON_UPDATE_COMMAND_UI(ID_BACK_BUTTON, OnUpdateBackButton)
    ON_UPDATE_COMMAND_UI(ID_FORWARD_BUTTON, OnUpdateForwardButton)
    ON_UPDATE_COMMAND_UI(ID_STOP_BUTTON, OnUpdateStopButton)
    ON_UPDATE_COMMAND_UI(ID_IMAGE_BUTTON, OnUpdateImageButton)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_UPDATE_COMMAND_UI(ID_OPEN_BROWSER, OnUpdateBrowserButton)
	ON_COMMAND(ID_SETTING_LOGIN, &CMainFrame::OnSettingLogin)
	ON_COMMAND(ID_SETTING_GENERAL, &CMainFrame::OnSettingGeneral)
	ON_COMMAND(ID_MENU_CLOSE, &CMainFrame::OnMenuClose)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_ALL, &CMainFrame::OnUpdateGetpageAll)
	ON_COMMAND(IDM_GETPAGE_ALL, &CMainFrame::OnGetpageAll)
	ON_COMMAND(IDM_GETPAGE_LATEST10, &CMainFrame::OnGetpageLatest10)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_LATEST10, &CMainFrame::OnUpdateGetpageLatest10)
	ON_COMMAND(ID_CHANGE_FONT_BIG, &CMainFrame::OnChangeFontBig)
	ON_COMMAND(ID_CHANGE_FONT_MEDIUM, &CMainFrame::OnChangeFontMedium)
	ON_COMMAND(ID_CHANGE_FONT_SMALL, &CMainFrame::OnChangeFontSmall)
	ON_UPDATE_COMMAND_UI(ID_MENU_BACK, &CMainFrame::OnUpdateMenuBack)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT, &CMainFrame::OnUpdateMenuNext)
	ON_COMMAND(ID_MENU_BACK, &CMainFrame::OnMenuBack)
	ON_COMMAND(ID_MENU_NEXT, &CMainFrame::OnMenuNext)
	ON_WM_ACTIVATE()
	ON_COMMAND(IDM_CHECK_NEW, &CMainFrame::OnCheckNew)
	ON_COMMAND(ID_HELP_MENU, &CMainFrame::OnHelpMenu)
	ON_COMMAND(ID_HISTORY_MENU, &CMainFrame::OnHistoryMenu)
	ON_COMMAND(IDM_START_CRUISE, &CMainFrame::OnStartCruise)
	ON_COMMAND(IDM_START_CRUISE_UNREAD_ONLY, &CMainFrame::OnStartCruiseUnreadOnly)
	ON_COMMAND(IDM_OPEN_MIXI_MOBILE_BY_BROWSER, &CMainFrame::OnOpenMixiMobileByBrowser)
	ON_COMMAND(ID_ERRORLOG_MENU, &CMainFrame::OnErrorlogMenu)
	ON_COMMAND(ID_CHANGE_SKIN, &CMainFrame::OnChangeSkin)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnUpdateSkinMenuItem)
	ON_COMMAND_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnSkinMenuItem)
	ON_COMMAND(ID_MENU_ACTION, &CMainFrame::OnMenuAction)
END_MESSAGE_MAP()


// CMainFrame コンストラクション/デストラクション

// -----------------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------------
CMainFrame::CMainFrame()
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
			!m_wndCommandBar.AddAdornments(dwAdornmentFlags) ||
			!m_wndCommandBar.LoadToolBar(id_toolbar)) {
				TRACE0("CommandBar の作成に失敗しました\n");
				return -1;      // 作成できませんでした。
		}

		m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
	}

	if( theApp.m_bSmartphone ) {
		// Smartphone/Standard Edition の場合はメニューバーを作成する
		SHMENUBARINFO mbi;

		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize = sizeof(SHMENUBARINFO);
		mbi.hwndParent = m_hWnd;
		mbi.nToolBarId = IDR_MAINFRAME;
		mbi.hInstRes = AfxGetInstanceHandle();
		mbi.nBmpId = 0;
		mbi.cBmpImages = 0;

		if (SHCreateMenuBar(&mbi)==FALSE) {
			MZ3LOGGER_FATAL(L"Couldn't create menu bar");
		}
		HWND hwndMenuBar = mbi.hwndMB;
	}
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

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
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// レポートビュー → メインビュー

		// 終了処理
		((CReportView*)pActiveView)->EndProc();

		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, TRUE );
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE );

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
			if (theApp.m_pWriteView->IsSendEnd() == FALSE) {
				// 未送信データ有り
				int ret = ::MessageBox(m_hWnd, _T("未投稿のデータがあります\n破棄されますがよろしいですか？"),
					_T("MZ3"), MB_ICONQUESTION | MB_OKCANCEL);
				if (ret == IDCANCEL) {
					// 処理を中止
					return;
				}
				theApp.m_pWriteView->SetSendEnd(TRUE);
			}

			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE );

			theApp.ChangeView(theApp.m_pMainView);
		} else {
			// Write ビュー（日記以外） → レポートビュー
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, TRUE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

			// ReportViewに戻る
			theApp.ChangeView(theApp.m_pReportView);
		}
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
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// レポートビュー → 書き込みビュー
		// 但し、未送信の場合のみ。
		if( theApp.m_pWriteView->m_sendEnd == FALSE ) {
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

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
		// 送信完了フラグ(m_sendEnd)がONなら、「書き込みビューに行ける」と判断する
		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

		theApp.ChangeView(theApp.m_pReportView);
	}
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
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));
	}
#endif
}

// -----------------------------------------------------------------------------
// 戻るボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBackButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON));  
	}
#endif
}

// -----------------------------------------------------------------------------
// 進むボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateForwardButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON));  
	}
#endif
}

// -----------------------------------------------------------------------------
// 画像ボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateImageButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));  
	}
#endif
}

// -----------------------------------------------------------------------------
// 書き込みボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON) );
	}
#endif
}

// -----------------------------------------------------------------------------
// ブラウザボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBrowserButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));  
	}
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
			L"お手数ですが、MZ3.ini を直接編集してください" );
	} else {
		COptionSheet cPropSht( _T("オプション"));
		cPropSht.SetPage();
		cPropSht.SetActivePage(1);
		if (cPropSht.DoModal() == IDOK) {
			theApp.m_optionMng.Save();

			// フォントのリセット
			ChangeAllViewFont();
		}
	}
}

/// 「閉じる」メニューイベントハンドラ
void CMainFrame::OnMenuClose()
{
	int iRet;
	iRet = ::MessageBox(m_hWnd, _T("ＭＺ３を終了しますか？"), _T("ＭＺ３"), MB_ICONQUESTION | MB_OKCANCEL);
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
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST10 );
}

/// 「最新１０件取得」メニューのチェック状態制御
void CMainFrame::OnUpdateGetpageLatest10(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST10 ) {
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
//		MySetListViewFont( &pView->m_groupList );
		pView->m_groupTab.SetFont( &theApp.m_font );

		// カテゴリリスト
		MySetListViewFont( &pView->m_categoryList );

		// ボディリスト
		MySetListViewFont( &pView->m_bodyList );

		// インフォメーションエディット
		pView->m_infoEdit.SetFont( &theApp.m_font );
	}

	//--- レポートビュー
	{
		CReportView* pView = theApp.m_pReportView;

		// タイトル
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// リスト
		MySetListViewFont( &pView->m_list );

		// エディット
		pView->m_edit.SetFont( &theApp.m_font );

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
	}

	// サイズ変更
	{
		CRect rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int w = rect.Width();
		int h = rect.Height() - rect.top;

		// Smartphone/Standard Edition の場合はツールバーの分だけ微調整
		if( theApp.m_bSmartphone ) {
			h += MZ3_TOOLBAR_HEIGHT;
		}

		SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );

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
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON) );
	}
#endif
}

/// 画面｜次の画面メニューの制御
void CMainFrame::OnUpdateMenuNext(CCmdUI *pCmdUI)
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON) );
	}
#endif
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
	theApp.m_pMainView->MyShowHelp();
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
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	POINT pt;
	pt.x = (rect.right-rect.left) / 2;
	pt.y = (rect.bottom-rect.top) / 2;

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

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);
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
				skinfileList.push_back( data.cFileName );
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
void CMainFrame::OnOpenMixiMobileByBrowser()
{
	CString url = theApp.MakeLoginUrlForMixiMobile( L"home.pl" );
	util::OpenUrlByBrowser( url );
}

/**
 * 右ソフトキー押下イベント：ポップアップ
 */
void CMainFrame::OnMenuAction()
{
}
