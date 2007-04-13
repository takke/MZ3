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

	int id_toolbar = 0;
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		id_toolbar = IDR_TOOLBAR;
		break;
	case SR_QVGA:
	default:
		id_toolbar = IDR_TOOLBAR_QVGA;
		break;
	}
	if (!m_wndCommandBar.Create(this) ||
		!m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
		!m_wndCommandBar.AddAdornments(dwAdornmentFlags) ||
		!m_wndCommandBar.LoadToolBar(id_toolbar)) {
			TRACE0("CommandBar の作成に失敗しました\n");
			return -1;      // 作成できませんでした。
	}

	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);

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
	COptionSheet cPropSht( _T("オプション"));
	cPropSht.SetPage();
	cPropSht.SetActivePage(0);
	if (cPropSht.DoModal() == IDOK) {
		theApp.m_optionMng.Save();

		// フォントのリセット
		ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
	}
}

// -----------------------------------------------------------------------------
// ストップボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateStopButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));  
}

// -----------------------------------------------------------------------------
// 戻るボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBackButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON));  
}

// -----------------------------------------------------------------------------
// 進むボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateForwardButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON));  
}

// -----------------------------------------------------------------------------
// 画像ボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateImageButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));  
}

// -----------------------------------------------------------------------------
// 書き込みボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON) );
}

// -----------------------------------------------------------------------------
// ブラウザボタンの制御
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBrowserButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));  
}

// -----------------------------------------------------------------------------
// 設定
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingGeneral()
{
	COptionSheet cPropSht( _T("オプション"));
	cPropSht.SetPage();
	cPropSht.SetActivePage(1);
	if (cPropSht.DoModal() == IDOK) {
		theApp.m_optionMng.Save();

		// フォントのリセット
		ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
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
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
}

/// 「文字サイズ｜中」メニュー
void CMainFrame::OnChangeFontMedium()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightMedium;
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
}

/// 「文字サイズ｜小」メニュー
void CMainFrame::OnChangeFontSmall()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightSmall;
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
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
		SetWindowPos( NULL, 0, 0, w, h,
			SWP_NOMOVE | SWP_NOZORDER );

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
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON) );
}

/// 画面｜次の画面メニューの制御
void CMainFrame::OnUpdateMenuNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON) );
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
 * その他｜巡回
 */
void CMainFrame::OnStartCruise()
{
	theApp.m_pMainView->StartCruise();
}
