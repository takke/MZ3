// DownloadView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "MainFrm.h"
#include "DownloadView.h"
#include "util.h"
#include "util_gui.h"


// CDownloadView

IMPLEMENT_DYNCREATE(CDownloadView, CFormView)

CDownloadView::CDownloadView()
	: CFormView(CDownloadView::IDD)
	, m_access(FALSE)
	, m_targetItemIndex( -1 )
{
}

CDownloadView::~CDownloadView()
{
}

void CDownloadView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_ITEM_LIST, m_list);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_CONTINUE_CHECK, mc_checkContinue);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
}

BEGIN_MESSAGE_MAP(CDownloadView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CONTINUE_CHECK, &CDownloadView::OnBnClickedContinueCheck)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
	ON_BN_CLICKED(IDC_START_STOP_BUTTON, &CDownloadView::OnBnClickedStartStopButton)
	ON_BN_CLICKED(IDC_EXIT_BUTTON, &CDownloadView::OnBnClickedExitButton)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
//	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
	ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
	ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
	ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
	ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
END_MESSAGE_MAP()


// CDownloadView 診断

#ifdef _DEBUG
void CDownloadView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDownloadView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDownloadView メッセージ ハンドラ

void CDownloadView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// プログレスバー初期化
	mc_progressBar.SetRange( 0, 1000 );

	//--- タイトルの変更
	{
		// フォント変更
		m_titleEdit.SetFont( &theApp.m_font );

		// タイトル変更
		m_titleEdit.SetWindowText( L"ダウンロードマネージャ" );
	}

	//--- リストの変更
	{
		// フォント変更
		m_list.SetFont( &theApp.m_font );

		// グリッドライン表示
		m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES);

		// 一行選択モードの設定
		ListView_SetExtendedListViewStyle((HWND)m_list.m_hWnd, LVS_EX_FULLROWSELECT);

		DWORD dwStyle = m_list.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		dwStyle |= LVS_REPORT;

		// スタイルの更新
		m_list.ModifyStyle(0, dwStyle);

		// アイコンリストの作成
		m_imageList.Create(16, 16, ILC_COLOR4, 2, 0);
		m_imageList.Add( AfxGetApp()->LoadIcon(IDI_DOWNLOADED_ICON) );
		m_list.SetImageList(&m_imageList, LVSIL_SMALL);

		// カラムの追加
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		m_list.InsertColumn(0, _T("ファイル名"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("備考"), LVCFMT_LEFT, 20, -1);
	}

	//--- チェック
	{
		mc_checkContinue.SetCheck( BST_CHECKED );
	}

	//--- 通知領域の変更
	{
		// フォント変更
		m_infoEdit.SetFont( &theApp.m_font );
	}
}

/**
 * サイズ変更時の処理
 */
void CDownloadView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// 前回の値を保存し、(0,0) の場合はその値を利用する
	static int s_cx = 0;
	static int s_cy = 0;
	if (cx==0 && cy==0) {
		cx = s_cx;
		cy = s_cy;
	} else {
		s_cx = cx;
		s_cy = cy;
	}

	CFormView::OnSize(nType, cx, cy);

	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	int hTitle    = theApp.GetInfoRegionHeight(fontHeight);	// タイトルはフォントサイズ依存
	int hButton   = theApp.GetInfoRegionHeight(fontHeight);	// ボタンサイズはフォントサイズ依存
	int hInfo     = theApp.GetInfoRegionHeight(fontHeight);	// 情報領域もフォントサイズ依存
	int hProgress = hInfo * 2 / 3;							// サイズは hInfo の 2/3 とする
	int hList     = cy-hTitle-hButton-hInfo-hProgress;		// (全体-他の領域) をリスト領域とする

	int y = 0;
	util::MoveDlgItemWindow( this, IDC_TITLE_EDIT,        0, y, cx, hTitle  );
	y += hTitle;
	util::MoveDlgItemWindow( this, IDC_ITEM_LIST,         0, y, cx, hList   );
	y += hList;

	int x = 0;
	int w = 50;
	util::MoveDlgItemWindow( this, IDC_START_STOP_BUTTON, x, y, w,  hButton );
	x += w;
	util::MoveDlgItemWindow( this, IDC_EXIT_BUTTON,       x, y, w,  hButton );
	x += w +10;
	w = cx - x;
	util::MoveDlgItemWindow( this, IDC_CONTINUE_CHECK,    x, y, w,  hButton );
	y += hButton;

	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,         0, y, cx, hInfo   );
	y += hInfo;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR,      0, y, cx, hProgress );
	y += hProgress;

	// 選択中の行が表示されるようにする
	if( m_list.m_hWnd != NULL ) {
		int idx = util::MyGetListCtrlSelectedItemIndex(m_list);
		m_list.EnsureVisible( idx, FALSE );
	}

	// リストカラム幅の変更
	ResetColumnWidth();
}

/**
 * カラムサイズ（幅）を再設定する。
 */
void CDownloadView::ResetColumnWidth(void)
{
	if( m_list.m_hWnd == NULL )
		return;

	// 幅の定義
	int w = GetListWidth();

	const int W_COL1 = 30;
	const int W_COL2 = 70;
	const int W_COL3 = 0;

	// ヘルプ以外
	m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
	m_list.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2+W_COL3) );
	m_list.SetColumnWidth(2, w * W_COL3/(W_COL1+W_COL2+W_COL3) );
}

int CDownloadView::GetListWidth(void)
{
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();

	// ピクセル数の微調整（スクリーン幅より少し小さくする）
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 30;
		break;
	case SR_QVGA:
	default:
		w -= 30/2;
		break;
	}
#else
	w -= 30;
#endif
	return w;
}

void CDownloadView::MyUpdateControls(void)
{
	// 開始・停止ボタンの制御
	if (m_access) {
		GetDlgItem( IDC_START_STOP_BUTTON )->SetWindowText( L"停止" );
		GetDlgItem( IDC_START_STOP_BUTTON )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_START_STOP_BUTTON )->SetWindowText( L"開始" );

		// 非アクセス中なので、未ダウンロードファイルがあれば開始可能。
		bool bHasNoFinishedItem = false;
		for (int i=0; i<m_items.size(); i++) {
			if (!m_items[i].bFinished) {
				bHasNoFinishedItem = true;
				break;
			}
		}
		GetDlgItem( IDC_START_STOP_BUTTON )->EnableWindow( bHasNoFinishedItem ? TRUE : FALSE );
	}

	// 戻る・停止アイコン
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, m_access ? FALSE : TRUE );
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, m_access ? TRUE : FALSE );

	// 終了ボタンはアクセス中は無効
	GetDlgItem( IDC_EXIT_BUTTON )->EnableWindow( m_access ? FALSE : TRUE );
}

/**
 * 表示イベント
 */
LRESULT CDownloadView::OnFit(WPARAM wParam, LPARAM lParam)
{
	// TODO ダウンロード済みの項目を削除

	// 項目の更新
	m_list.DeleteAllItems();
	for (size_t i=0; i<m_items.size(); i++) {
		DownloadItem& item = m_items[i];

		CString filename;
		int idxFind = item.localpath.ReverseFind( '\\' );
		if (idxFind>=0) {
			filename = item.localpath.Mid(idxFind+1);
		}

		int idx = m_list.InsertItem( i, filename, item.bFinished ? 0 : -1 );
		m_list.SetItem( idx, 1, LVIF_TEXT, item.url, 0, 0, 0, 0 );
	}
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );

	// コントロール状態の更新
	MyUpdateControls();

	if (!m_access) {
		// 準備完了
		util::MySetInformationText( m_hWnd, L"準備完了" );
	}

	return TRUE;
}

void CDownloadView::OnBnClickedContinueCheck()
{
	// ダウンロード完了時にチェック状態を参照するため特に処理は不要。
}

/**
 * 開始・停止ボタン
 */
void CDownloadView::OnBnClickedStartStopButton()
{
	if (m_access) {
		// 停止処理
		::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
	} else {
		// 開始処理

		// ターゲット選択
		int idx = util::MyGetListCtrlSelectedItemIndex( m_list );
		if (idx<0) {
			return;
		}
		m_targetItemIndex = idx;

		// ダウンロード開始
		DoDownloadSelectedItem();
	}
}

/**
 * 終了ボタン
 */
void CDownloadView::OnBnClickedExitButton()
{
	// 通信中は無効
	if (m_access) {
		return;
	}

	// 戻る
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/**
 * アクセス情報通知
 */
LRESULT CDownloadView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

/**
 * アクセス終了通知受信(Binary)
 */
LRESULT CDownloadView::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	if (m_abortRequested) {
		// WM_MZ3_GET_ABORT メッセージが来ない場合があるのでここで状態復帰
		util::MySetInformationText( m_hWnd, _T("中断しました") );

		m_access = FALSE;
		m_abortRequested = FALSE;
		MyUpdateControls();
		return FALSE;
	}

	// 保存ファイルにコピー
	CopyFile( theApp.m_filepath.temphtml, m_items[m_targetItemIndex].localpath, FALSE/*bFailIfExists, 上書き*/ );

	// フラグ変更
	m_items[ m_targetItemIndex ].bFinished = true;
	// フラグ変更に対するイメージ変更
	util::MySetListCtrlItemImageIndex( m_list, m_targetItemIndex, 0, 0 );
	// 更新
	m_list.Update( m_targetItemIndex );

	// 「連続」チェック済みで、次のアイテムがあれば継続。
	if (IsDlgButtonChecked(IDC_CONTINUE_CHECK) == BST_CHECKED) {
		// 次の未ダウンロードアイテムを探索
		for (int i=m_targetItemIndex+1; i<m_items.size(); i++) {
			if (!m_items[i].bFinished) {
				// 発見。選択状態を変更し、再ダウンロード開始。
				util::MySetListCtrlItemFocusedAndSelected( m_list, m_targetItemIndex, false );	// 非選択
				m_targetItemIndex = i;
				util::MySetListCtrlItemFocusedAndSelected( m_list, m_targetItemIndex, true );	// 選択

				// ダウンロード開始
				if (DoDownloadSelectedItem()) {
					return TRUE;
				}
			}
		}
	}

	// ダウンロード完了
	m_access = FALSE;
	MyUpdateControls();

	util::MySetInformationText( m_hWnd, L"ダウンロードが完了しました" );

	return TRUE;
}

/**
 * アクセスエラー通知受信
 */
LRESULT CDownloadView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	if (m_abortRequested) {
		// WM_MZ3_GET_ABORT メッセージが来ない場合があるのでここで状態復帰
		util::MySetInformationText( m_hWnd, _T("中断しました") );

		m_access = FALSE;
		m_abortRequested = FALSE;
		MyUpdateControls();
		return TRUE;
	}

	LPCTSTR smsg = L"エラーが発生しました";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"原因：%s", smsg, theApp.m_inet.GetErrorMessage() );
	MZ3LOGGER_ERROR( msg );

	m_access = FALSE;
	MyUpdateControls();

	return TRUE;
}

/**
 * アクセス中断通知受信
 * 
 * ユーザ指示による中断
 */
LRESULT CDownloadView::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	// ユーザからのアクセス中断指示をうけると、ここに通知が帰ってくる
	// ボタンを元に戻してメッセージをだして処理終了
	util::MySetInformationText( m_hWnd, _T("中断しました") );

	m_access = FALSE;
	m_abortRequested = FALSE;
	MyUpdateControls();

	return TRUE;
}

/**
 * 中断ボタン押下時の処理
 */
LRESULT CDownloadView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// 通信中でないならすぐに終了する
	if( !theApp.m_inet.IsConnecting() ) {
		return TRUE;
	}
	theApp.m_inet.Abort();

	LPCTSTR msg = _T("中断しました");
	util::MySetInformationText( m_hWnd, msg );
	MZ3LOGGER_DEBUG( msg );

	m_access = FALSE;
	m_abortRequested = TRUE;
	MyUpdateControls();

	return TRUE;
}

/**
 * 受信サイズ通知
 */
LRESULT CDownloadView::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
{
	if( dwLength == 0 ) {
		// 総サイズが不明なので、仮に N KB とみなす
		dwLength = 40 * 1024;
	}

	// 受信サイズと総サイズが同一なら、受信完了とみなす
	if( dwLoaded == dwLength ) {
	}

	// [0,1000] で受信サイズ通知
	int pos = (int)(dwLoaded * 1000.0 / dwLength);
	mc_progressBar.SetPos( pos );

	return TRUE;
}

/**
 * ダウンロード開始処理
 */
bool CDownloadView::DoDownloadSelectedItem(void)
{
	if (m_targetItemIndex<0 || m_targetItemIndex >= m_items.size()) {
		MZ3LOGGER_FATAL( L"インデックス不正" );
		return false;
	}

	if( m_list.m_hWnd != NULL ) {
		m_list.EnsureVisible( m_targetItemIndex, FALSE );
	}

	CString url = m_items[m_targetItemIndex].url;
	MZ3LOGGER_DEBUG( L"ダウンロード開始 url[" + url + L"]" );

	m_access = TRUE;
	MyUpdateControls();
	mc_progressBar.SetPos( 0 );

	// 取得開始
	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_accessType = ACCESS_DOWNLOAD;
	theApp.m_inet.DoGet( url, _T(""), CInetAccess::FILE_BINARY );

	return true;
}

