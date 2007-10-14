// ReportView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportView.h"
#include "MainFrm.h"
#include "IniFile.h"
#include "WriteView.h"
#include "HtmlArray.h"
#include "util.h"
#include "QuoteDlg.h"
#include "MixiParser.h"
#include "CommonSelectDlg.h"

#define MASK_COLOR RGB(255,0,255);

// CReportView

IMPLEMENT_DYNCREATE(CReportView, CFormView)

static HINSTANCE g_HtmlViewInstance = NULL;			///< htmlview.dll のインスタンス

/**
 * コンストラクタ
 */
CReportView::CReportView()
	: CFormView(CReportView::IDD)
	, m_nKeydownRepeatCount(0)
	, m_hwndHtml(NULL)
{
	m_data = NULL;
	m_whiteBr = CreateSolidBrush(RGB(255, 255, 255));
	m_imageState = FALSE;
	m_posHtmlScroll = 0;
	m_posHtmlScrollMax = 0;
}

/**
 * デストラクタ
 */
CReportView::~CReportView()
{
	m_pimgList->DeleteImageList();
	delete m_pimgList;
	m_pimgList = NULL;
}

void CReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_LIST, m_list);
	DDX_Control(pDX, IDC_REPORT_EDIT, m_edit);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
}

BEGIN_MESSAGE_MAP(CReportView, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REPORT_LIST, &CReportView::OnLvnItemchangedReportList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_REPORT_LIST, &CReportView::OnLvnKeydownReportList)

	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
	ON_COMMAND(ID_ADD_BOOKMARK, &CReportView::OnAddBookmark)
	ON_COMMAND(ID_DEL_BOOKMARK, &CReportView::OnDelBookmark)
    ON_COMMAND_RANGE(ID_REPORT_IMAGE+1, ID_REPORT_IMAGE+50, OnLoadImage)
    ON_COMMAND_RANGE(ID_REPORT_MOVIE+1, ID_REPORT_MOVIE+50, OnLoadMovie)
	ON_COMMAND_RANGE(ID_REPORT_PAGE_LINK_BASE+1, ID_REPORT_PAGE_LINK_BASE+50, OnLoadPageLink)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
    ON_COMMAND(ID_IMAGE_BUTTON, OnImageButton)
	ON_COMMAND(IDM_RELOAD_PAGE, OnReloadPage)
    ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetImageEnd)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
    ON_MESSAGE(WM_MZ3_RELOAD, OnReload)
	ON_COMMAND(ID_WRITE_COMMENT, &CReportView::OnWriteComment)
	ON_COMMAND(ID_WRITE_MESSAGE, &CReportView::OnWriteComment)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_COMMAND(ID_EDIT_COPY, &CReportView::OnEditCopy)
	ON_COMMAND(ID_SELECT_ALL, &CReportView::OnSelectAll)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
    ON_COMMAND(ID_OPEN_BROWSER, OnOpenBrowser)

	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CReportView::OnShowDebugInfo)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CReportView::OnOpenBrowserUser)
	ON_UPDATE_COMMAND_UI(ID_WRITE_COMMENT, &CReportView::OnUpdateWriteComment)
	ON_COMMAND(ID_MENU_BACK, &CReportView::OnMenuBack)
	ON_COMMAND(ID_BACK_MENU, &CReportView::OnBackMenu)
	ON_COMMAND(ID_NEXT_MENU, &CReportView::OnNextMenu)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CReportView::OnHdnEndtrackReportList)
	ON_COMMAND(IDM_LAYOUT_REPORTLIST_MAKE_NARROW, &CReportView::OnLayoutReportlistMakeNarrow)
	ON_COMMAND(IDM_LAYOUT_REPORTLIST_MAKE_WIDE, &CReportView::OnLayoutReportlistMakeWide)
END_MESSAGE_MAP()


// CReportView 診断

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CReportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CReportView メッセージ ハンドラ

/**
 * 初期化処理
 */
void CReportView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// プログレスバー初期化
	mc_progressBar.SetRange( 0, 1000 );

	//--- タイトルの変更
	{
		// フォント変更
		m_titleEdit.SetFont( &theApp.m_font );
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
		// デフォルトをレポート表示でオーナー固定描画にする
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		// スタイルの更新
		m_list.ModifyStyle(0, dwStyle);

		m_pimgList = new CImageList();
		m_pimgList->Create(16, 16, ILC_COLOR4, 2, 0);
		m_pimgList->Add( AfxGetApp()->LoadIcon(IDI_NO_PHOTO_ICON) );
		m_pimgList->Add( AfxGetApp()->LoadIcon(IDI_PHOTO_ICON) );
		m_list.SetImageList(m_pimgList, LVSIL_SMALL);

		// カラムの追加
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("名前"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("日時"), LVCFMT_LEFT, 20, -1);
	}

	//--- エディットの変更
	{
		// フォント変更
		m_edit.SetFont( &theApp.m_font );
	}

	//--- 通知領域の変更
	{
		// フォント変更
		m_infoEdit.SetFont( &theApp.m_font );
	}

	m_nochange = FALSE;

	// スクロール量の初期値設定
	m_scrollLine = theApp.m_optionMng.m_reportScrollLine;

	// PocketIE コントロールの初期化
#ifdef WINCE
	if (theApp.m_optionMng.m_bRenderByIE) {			// TODO 絵文字正式対応時は本条件を外し、UI からOn/Offを切り替え可能にすること。
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER;
		CString sInfo;
		if (!g_HtmlViewInstance) {
			g_HtmlViewInstance = ::LoadLibrary(L"htmlview.dll");
		}

		CRect rect( 0, 30, 40, 60);
		m_hwndHtml = ::CreateWindow(DISPLAYCLASS,//DISPLAYCLASS,
									NULL,
									dwStyle,
									rect.left,
									rect.top,
									rect.right,
									rect.bottom,
									m_hWnd,
									0,
									g_HtmlViewInstance,
									NULL);

		::SetWindowLong( m_hwndHtml, GWL_ID, 12321);
	}
#endif
}

/**
 * HTML コントロールに文字列を設定する
 */
void CReportView::SetHtmlText(LPCTSTR szHtmlText)
{
#ifdef WINCE
	::SendMessage( m_hwndHtml, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)_T(""));

	CString s;

	// TODO 正式対応時はスタイルシートで「動的に」設定すること。メッセージで。
	s.Format( L"<html><body style=\"font-size: %dpx; font-family: '%s';\"><pre>", 
		theApp.m_optionMng.GetFontHeight()*5/10, (LPCTSTR)theApp.m_optionMng.GetFontFace() );
	::SendMessage(m_hwndHtml, DTM_ADDTEXTW, FALSE, (LPARAM)(LPCTSTR)s);

	::SendMessage(m_hwndHtml, DTM_ADDTEXTW, FALSE, (LPARAM)(LPCTSTR)szHtmlText);

	::SendMessage(m_hwndHtml, DTM_ADDTEXTW, FALSE, (LPARAM)(LPCTSTR)L"</pre></body></html>");

	::SendMessage(m_hwndHtml, DTM_ENDOFSOURCE, 0, 0);
#endif
}

/**
 * サイズ変更時の処理
 */
void CReportView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	int hTitle  = theApp.GetInfoRegionHeight(fontHeight);	// タイトル領域はフォントサイズ依存

	const int h1 = theApp.m_optionMng.m_nReportViewListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nReportViewBodyHeightRatio;
	int hList   = (cy * h1 / (h1+h2))-hTitle;	// (全体のN%-タイトル領域) をリスト領域とする
	int hReport = (cy * h2 / (h1+h2));			// 全体のN%をレポート領域とする

	// 情報領域は必要に応じて表示されるため、上記の比率とは関係なくサイズを設定する
	int hInfo   = theApp.GetInfoRegionHeight(fontHeight);	// 情報領域もフォントサイズ依存

	util::MoveDlgItemWindow( this, IDC_TITLE_EDIT,  0, 0,            cx, hTitle  );
	util::MoveDlgItemWindow( this, IDC_REPORT_LIST, 0, hTitle,       cx, hList   );
	if (theApp.m_optionMng.m_bRenderByIE) {
		// HTML コントロールの移動
		::MoveWindow( m_hwndHtml, 0, hTitle+hList, cx, hReport, TRUE );
		// エディットコントロールの非表示化
		CWnd* pDlg = GetDlgItem( IDC_REPORT_EDIT );
		if (pDlg) pDlg->ShowWindow( SW_HIDE );
	} else {
		// HTML コントロールの非表示化
		if (m_hwndHtml) {
			::ShowWindow( m_hwndHtml, SW_HIDE );
		}

		// エディットコントロールの移動
		util::MoveDlgItemWindow( this, IDC_REPORT_EDIT, 0, hTitle+hList, cx, hReport );
	}
	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,   0, cy - hInfo,   cx, hInfo   );

	// スクロールタイプが「ページ単位」なら再計算
	if( theApp.m_optionMng.m_reportScrollType == option::Option::REPORT_SCROLL_TYPE_PAGE ) {
		m_scrollLine = (hReport / fontHeight) - 2;
		TRACE(_T("Scrol Line = %d\n"), m_scrollLine);
	}else{
		m_scrollLine = theApp.m_optionMng.m_reportScrollLine;
	}

	// 選択中の行が表示されるようにする
	if( m_list.m_hWnd != NULL ) {
		int idx = m_list.GetSelectedItem();
		m_list.EnsureVisible( idx, FALSE );
	}

	// プログレスバーは別途配置
	// サイズは hInfo の 2/3 とする
	int hProgress = hInfo * 2 / 3;
	int y = cy - hInfo - hProgress;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR, 0, y, cx, hProgress );

	// リストカラム幅の変更
	if( m_data != NULL ) {
		ResetColumnWidth( *m_data );
	}
}

HBRUSH CReportView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == GetDlgItem(IDC_INFO_EDIT)->m_hWnd) {
		return hbr;
	}

	switch (nCtlColor) {
  case CTLCOLOR_STATIC:
	  pDC->SetBkMode(TRANSPARENT);
	  return (HBRUSH)m_whiteBr;
	  break;

  default:
	  pDC->SetBkMode(TRANSPARENT);
	  break;
	}

	return m_whiteBr;
}

/**
 * データ設定
 */
void CReportView::SetData(CMixiData* data)
{
	m_access = FALSE;
	m_nochange = TRUE;
	m_lastIndex = 0;

	m_data = data;

	m_list.DeleteAllItems();
	m_list.SetRedraw(FALSE);

	// カラムサイズを変更する
	ResetColumnWidth( *data );

	// どこまでデータを取得したかを設定する
	TRACE(_T("Address = %s\n"), m_data->GetURL());

	// URIを分解
	CString tmp;

	if (m_data->GetID() == -1) {
		// ここでＩＤを作る
		tmp = m_data->GetURL();
		tmp = tmp.Mid(tmp.Find(_T("id=")) + wcslen(_T("id=")));
		tmp = tmp.Left(tmp.Find(_T("&")));
		data->SetID(_wtoi(tmp));
	}

	switch (m_data->GetAccessType()) {
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
		{
			CString id;
			if (util::GetBetweenSubString( m_data->GetURL(), L"id=", L"&", id ) >= 0) {
				tmp.Format(_T("d%s"), id);
			}
		}
		break;
	case ACCESS_BBS:
		tmp.Format(_T("b%d"), m_data->GetID());
		break;
	case ACCESS_EVENT:
		tmp.Format(_T("v%d"), m_data->GetID());
		break;
	case ACCESS_ENQUETE:
		tmp.Format(_T("e%d"), m_data->GetID());
		break;

	}

	memset(m_idKey, 0x00, sizeof(char) * 24);
	wcstombs(m_idKey, tmp, 24);

	// 既読位置の変更
	m_lastIndex = m_data->GetLastIndex();
	if (m_lastIndex == -1) {
		m_lastIndex = 0;
	} else {
		m_lastIndex++;
	}


	int focusItem = 0;
	INT_PTR count = m_data->GetChildrenSize();

	if (count != 0 && m_lastIndex != 0) {
		CMixiData& cmtData = m_data->GetChild( count-1 );
		if (cmtData.GetCommentIndex() <= m_lastIndex && m_lastIndex != 0) {
			m_lastIndex = cmtData.GetCommentIndex();
		}
	}

	// タイトルの設定
	int index;
	CString title = m_data->GetTitle();
	index = title.ReverseFind(_T('('));
	if (index != -1) {
		title = title.Left(index);
	}
	if (m_data->GetAccessType() != ACCESS_MYDIARY &&
		m_data->GetAccessType() != ACCESS_MESSAGE) 
	{
		if( !m_data->GetName().IsEmpty() ) {
			title.AppendFormat( _T("(%s)"), (LPCTSTR)m_data->GetName() );
		}
	}
	m_titleEdit.SetWindowText( title );


	int nItem;
	int imgNo = 0;

	// ----------------------------------------
	// コメントの追加
	// ----------------------------------------
	TRACE(_T("コメント数 = [%d]\n"), count);

	for (int i=0; i<count; i++) {
		CMixiData& cmtData = m_data->GetChild(i);

		if (cmtData.GetImageCount() == 0) {
			imgNo = 0;
		}
		else {
			imgNo = 1;
		}
		nItem = m_list.InsertItem(i+1, util::int2str(cmtData.GetCommentIndex()), imgNo);

		if (cmtData.GetCommentIndex() == m_lastIndex) {
			focusItem = nItem + 1;
		}

		m_list.SetItem(nItem, 1, LVIF_TEXT | LVIF_IMAGE, cmtData.GetAuthor(), 0, 0, 0, 0);
		m_list.SetItem(nItem, 2, LVIF_TEXT, cmtData.GetDate(), 0, 0, 0, 0);
		m_list.SetItemData(nItem, (DWORD_PTR)&cmtData);
	}

	// 親をリストに表示
	if (m_data->GetImageCount() == 0) {
		imgNo = 0;
	}
	else {
		imgNo = 1;
	}
	nItem = m_list.InsertItem(0, _T("-"), imgNo);
	m_list.SetItem(nItem, 1, LVIF_TEXT, m_data->GetAuthor(), 0, 0, 0, 0);
	m_list.SetItemData(0, (DWORD_PTR)m_data);

	m_nochange = FALSE;

	if (count == 0) {
		m_lastIndex = 0;
	}
	else {
		// 先頭のデータを取得
		CMixiData& cmtData = m_data->GetChild(0);
		if (cmtData.GetCommentIndex() > m_lastIndex && m_lastIndex != 0) {
			m_lastIndex = cmtData.GetCommentIndex();
			focusItem = 1;
		}
	}

	//--- UI 関連
	m_list.SetRedraw(TRUE);

	m_list.SetFocus();
	m_list.SetItemState( focusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	m_list.EnsureVisible( focusItem, FALSE );
}

/**
 * 親記事データ表示
 */
void CReportView::ShowParentData(CMixiData* data)
{
	return ShowCommentData(data);
}

/**
 * コメントデータ表示
 */
void CReportView::ShowCommentData(CMixiData* data)
{
	if (theApp.m_optionMng.m_bRenderByIE) {
		const int n = data->GetBodySize();

		// スクロール位置の初期化
		m_posHtmlScroll = 0;
		m_posHtmlScrollMax = n;

		CString str;

		str.AppendFormat( L"<a name='mz3line0'><span style='color: blue;'>%s %s</span></a><br />", 
				data->GetAuthor(), data->GetDate() );

		for( int i=0; i<n; i++ ){
			str.AppendFormat( L"<a name='mz3line%d'>", i+1 );
			str += data->GetBody(i);
			str += L"</a><br />\r\n";
		}

		str += _T("<br />");		// 最後に１行入れて見やすくする

		SetHtmlText(str);
	} else {
		CString str = _T("");

		str += data->GetAuthor();
		str += _T("　");
		str += data->GetDate();
		str += _T("\r\n");

		const int n = data->GetBodySize();
		for( int i=0; i<n; i++ ){
			str += data->GetBody(i);
		}

		str += _T("\r\n");			// 最後に１行入れて見やすくする
		m_edit.SetWindowText(str);

		// Win32 の場合は再描画
#ifndef WINCE
		m_edit.Invalidate();
#endif
	}
}

void CReportView::OnLvnItemchangedReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_nochange != FALSE) {
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState != 3) {
		return;
	}

	CMixiData* data;
	data = (CMixiData*)m_list.GetItemData(pNMLV->iItem);
	m_currentData = data;
	if (data->GetCommentIndex() > m_lastIndex) {
		m_lastIndex = data->GetCommentIndex();
	}

	m_list.SetRedraw(FALSE);
	if (pNMLV->iItem == 0) {
		ShowParentData(data);
	}
	else {
		ShowCommentData(data);
	}

	m_imageState = (BOOL)(data->GetImageCount() > 0);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, ((!m_access) & m_imageState));
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, !m_access);

	m_list.SetRedraw(TRUE);

	*pResult = 0;
}

void CReportView::OnLvnKeydownReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if (pLVKeyDow->wVKey == VK_RETURN) {
		// レポートメニューの表示
		MyPopupReportMenu();
	}
	*pResult = 0;
}

BOOL CReportView::CommandMoveUpList()
{
	if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE) {
		// 一番上の項目選択中なので、一番下に移動
		util::MySetListCtrlItemFocusedAndSelected( m_list, 0, false );
		util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, true );
		m_list.EnsureVisible( m_list.GetItemCount()-1, FALSE );
	} else {
		// 一番上ではないので、上に移動
		int idx = m_list.GetSelectedItem();
		util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
		util::MySetListCtrlItemFocusedAndSelected( m_list, --idx, true );

		// 移動先が非表示なら上方向にスクロール
		if( !util::IsVisibleOnListBox( m_list, idx ) ) {
			m_list.Scroll( CSize(0, -m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
		}
	}
	return TRUE;
}

BOOL CReportView::CommandMoveDownList()
{
	if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
		// 一番下の項目選択中なので、一番上に移動
		util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, false );
		util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );
		m_list.EnsureVisible( 0, FALSE );
	} else {
		// 一番下ではないので、下に移動
		int idx = m_list.GetSelectedItem();
		util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
		util::MySetListCtrlItemFocusedAndSelected( m_list, ++idx, true );

		// 移動先が非表示なら下方向にスクロール
		if( !util::IsVisibleOnListBox( m_list, idx ) ) {
			m_list.Scroll( CSize(0, m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
		}
	}
	return TRUE;
}

BOOL CReportView::CommandScrollUpList()
{
	if (theApp.m_optionMng.m_bRenderByIE) {
#ifdef WINCE
		// アンカーによりスクロールする
		m_posHtmlScroll = max(m_posHtmlScroll-1, 0);

		CString s;
		s.Format(L"mz3line%d", m_posHtmlScroll);
		::SendMessage( m_hwndHtml, DTM_ANCHORW, 0, (LPARAM)(LPCTSTR)s);
#endif
	} else {
		m_edit.LineScroll( -m_scrollLine );
	}
	return TRUE;
}

BOOL CReportView::CommandScrollDownList()
{
	if (theApp.m_optionMng.m_bRenderByIE) {
#ifdef WINCE
		// アンカーによりスクロールする
		m_posHtmlScroll = min(m_posHtmlScroll+1, m_posHtmlScrollMax);

		CString s;
		s.Format(L"mz3line%d", m_posHtmlScroll);
		::SendMessage( m_hwndHtml, DTM_ANCHORW, 0, (LPARAM)(LPCTSTR)s);
#endif
	} else {
		m_edit.LineScroll( m_scrollLine );

	}
	return TRUE;
}


BOOL CReportView::OnKeyUp(MSG* pMsg)
{
	switch (pMsg->wParam) {
	case VK_F1:
		if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
			CMenu menu;
			RECT rect;

			// メインメニューのポップアップ
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			if( theApp.m_bPocketPC ) {
#ifdef WINCE
				menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );
#endif
			} else {
				menu.LoadMenu(IDR_MAINFRAME);
			}
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
			menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN,
				rect.left,
				rect.bottom - TOOLBAR_HEIGHT,
				pMainFrame );
			menu.Detach();
			return TRUE;
		}
		break;

	case VK_F2:
		// レポートメニューの表示
		MyPopupReportMenu();
		return TRUE;

	case VK_BACK:				// クリアボタン
		if (m_access != FALSE) {
			// アクセス中は中断処理
			::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
		}
		else {
			OnMenuBack();
		}
		return TRUE;
	}

	// Xcrawl Canceler
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeyup( pMsg->wParam ) ) {
			// キャンセルされたので上下キーを無効にする。
	//		util::MySetInformationText( GetSafeHwnd(), L"Xcrawl canceled..." );
			return TRUE;
		}

		if (pMsg->hwnd == m_list.m_hWnd) {
			// リストでのキーUPイベント
			switch(pMsg->wParam) {
			case VK_UP:
				if( m_xcrawl.isXcrawlEnabled() ) {
					// Xcrawl ではスクロール
					return CommandScrollUpList();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// キー長押しによる連続移動中なら、キーUPで移動しない。
						return TRUE;
					}
					if( CommandMoveUpList() ) {
						return TRUE;
					}
				}
				break;

			case VK_DOWN:
				if( m_xcrawl.isXcrawlEnabled() ) {
					// Xcrawl ではスクロール
					return CommandScrollDownList();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// キー長押しによる連続移動中なら、キーUPで移動しない。
						return TRUE;
					}
					if( CommandMoveDownList() ) {
						return TRUE;
					}
				}
				break;
			}
		}
	}

	return FALSE;
}

BOOL CReportView::OnKeyDown(MSG* pMsg)
{
	switch (MapVirtualKey(pMsg->wParam, 2)) {
	case 48:
		TRACE(_T("0 Down\n"));
		return TRUE;
	}

	// VK_UP, VK_DOWN の処理
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		// Xcrawl Canceler
		if( m_xcrawl.procKeydown(pMsg->wParam) ) {
			return TRUE;
		}

		if (pMsg->hwnd == m_list.m_hWnd) {
			// リストでのキー押下イベント
			switch(pMsg->wParam) {
			case VK_UP:
				// VK_KEYDOWN では無視。
				// VK_KEYUP で処理する。
				// これは、アドエスの Xcrawl 対応のため。

				// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
				if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
	//				MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveUpList();
				}

				return TRUE;

			case VK_DOWN:
				// VK_KEYDOWN では無視。
				// VK_KEYUP で処理する。
				// これは、アドエスの Xcrawl 対応のため。

				// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
				if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
	//				MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveDownList();
				}

				return TRUE;
			}
		}
	} else {
		if (pMsg->hwnd == m_list.m_hWnd) {
			// リストでのキー押下イベント
			switch(pMsg->wParam) {
			case VK_UP:
				if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE) {
					// 一番上の項目選択中なので、一番下に移動
					util::MySetListCtrlItemFocusedAndSelected( m_list, 0, false );
					util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, true );
					m_list.EnsureVisible( m_list.GetItemCount()-1, FALSE );

					return TRUE;
				} else {
#ifdef WINCE
					// デフォルト動作
					return FALSE;
#else
					return CommandMoveUpList();
#endif
				}

			case VK_DOWN:
				if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
					// 一番下の項目選択中なので、一番上に移動
					util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, false );
					util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );
					m_list.EnsureVisible( 0, FALSE );

					return TRUE;
				} else {
#ifdef WINCE
					// デフォルト動作
					return FALSE;
#else
					return CommandMoveDownList();
#endif
				}
				break;
			}
		}
	}

	if (pMsg->hwnd == m_list.m_hWnd) {
		// リストでのキー押下イベント
		switch(pMsg->wParam) {
		case VK_RIGHT:
			return CommandScrollDownList();

		case VK_LEFT:
			return CommandScrollUpList();
		}
	}

	return FALSE;
}

BOOL CReportView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_list.m_hWnd) {
		if (pMsg->message == WM_KEYUP) {
			BOOL r = OnKeyUp(pMsg);

//			CString s;
//			s.Format( L"keyup, %0X", pMsg->wParam );
//			MZ3LOGGER_ERROR( s );

			// KEYDOWN リピート回数を初期化
			m_nKeydownRepeatCount = 0;

			if( r ) {
				return TRUE;
			}
		}
		else if (pMsg->message == WM_KEYDOWN) {
			// KEYDOWN リピート回数をインクリメント
			m_nKeydownRepeatCount ++;

//			CString s;
//			s.Format( L"keydown, %0X", pMsg->wParam );
//			MZ3LOGGER_ERROR( s );

			if( OnKeyDown(pMsg) ) {
				return TRUE;
			}
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * 終了処理
 */
void CReportView::EndProc()
{
	SaveIndex();
}

/**
 * インデックス保存
 *
 * m_lastIndex を m_idKey に保存
 */
void CReportView::SaveIndex()
{
	if (m_data != NULL) {
		theApp.m_logfile.SetValue(m_idKey, (const char*)util::int2str_a(m_lastIndex), "Log");

		m_data->SetLastIndex(m_lastIndex);
	}
}

/**
 * ブックマークに追加
 */
void CReportView::OnAddBookmark()
{

	if (m_data->GetAccessType() != ACCESS_BBS &&
		m_data->GetAccessType() != ACCESS_EVENT &&
		m_data->GetAccessType() != ACCESS_ENQUETE) {
			::MessageBox(m_hWnd, _T("コミュニティ以外は\n登録出来ません"), _T("MZ3"), NULL);
			return;
	}

	if( theApp.m_bookmarkMng.Add( m_data, theApp.m_root.GetBookmarkList() ) != FALSE ) {
		::MessageBox(m_hWnd, _T("登録しました"), _T("MZ3"), NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("既に登録されています"), _T("MZ3"), NULL);
	}


}

/**
 * ブックマークから削除
 */
void CReportView::OnDelBookmark()
{
	if (theApp.m_bookmarkMng.Delete(m_data,theApp.m_root.GetBookmarkList()) != FALSE) {
		::MessageBox(m_hWnd, _T("削除しました"), _T("MZ3"), NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("登録されていません"), _T("MZ3"), NULL);
	}
}

/**
 * 画像ＤＬ
 */
void CReportView::OnLoadImage(UINT nID)
{
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	CString url = m_currentData->GetImage(nID - ID_REPORT_IMAGE-1);
	MZ3LOGGER_DEBUG( L"画像ダウンロード開始 url[" + url + L"]" );

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// イメージURLをCGIから取得
	theApp.m_accessType = ACCESS_IMAGE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_HTML );
}

/**
 * 動画ＤＬ
 */
void CReportView::OnLoadMovie(UINT nID)
{
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	CString url = m_currentData->GetMovie(nID - ID_REPORT_MOVIE-1);
	MZ3LOGGER_DEBUG( L"動画ダウンロード開始 url[" + url + L"]" );

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// 動画URLをCGIから取得
	theApp.m_accessType = ACCESS_MOVIE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
}


/**
 * ページ変更
 */
void CReportView::OnLoadPageLink(UINT nID)
{
	int idx = nID - ID_REPORT_PAGE_LINK_BASE-1;
	if( 0 <= idx && idx <(int)m_data->m_linkPage.size() ) {
		// ok.
	}else{
		return;
	}

	// 既読位置を保存
	SaveIndex();

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	const CMixiData::Link& link = m_data->m_linkPage[idx];

	m_access = TRUE;
	m_abort = FALSE;

	// m_data の書き換え
	{
		// 実体はとりあえず static にしておく。
		// m_data 自身を書き換えると「ログを開く」が異なってしまうため。
		static CMixiData s_mixi;
		// 初期化
		CMixiData mixi;	// 初期化用

		// データ構築
		mixi.SetURL( link.url );
		mixi.SetTitle( link.text );

		// 名前は引き継ぐ
		mixi.SetName( m_data->GetName() );

		// アクセス種別を URL から推定
		ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
		if( estimatedAccessType != ACCESS_INVALID ) {
			mixi.SetAccessType( estimatedAccessType );
		}

		s_mixi = mixi;
		m_data = &s_mixi;
	}

	theApp.m_inet.Initialize( m_hWnd, NULL );

	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(link.url), _T(""), CInetAccess::FILE_HTML );
}

/**
 * 再読込
 */
void CReportView::OnReloadPage()
{
	// アクセス種別が「ヘルプ」なら何もしない
	switch( m_data->GetAccessType() ) {
	case ACCESS_HELP:
		return;
	}

	// 既読位置を保存
	SaveIndex();

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data->GetURL()), _T(""), CInetAccess::FILE_HTML );
}

/**
 * URL
 */
void CReportView::OnLoadUrl(UINT nID)
{
	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( idx > m_currentData->m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = m_currentData->m_linkList[idx].url;
	LPCTSTR text = m_currentData->m_linkList[idx].text;

	// 確認画面
	CString msg;
	msg.Format( 
		L"下記のURLを開きます。\n"
		L"どの方法で開きますか？\n\n"
		L"%s", url );

	CCommonSelectDlg dlg;
	dlg.SetMessage( msg );
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT1, L"ブラウザで開く" );
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, L"MZ3でダウンロード" );
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_CANCEL,  L"キャンセル" );
	if( dlg.DoModal() != IDOK ) {
		return;
	}

	int r = dlg.m_pressedButtonCode;
	switch( r ) {
	case CCommonSelectDlg::BUTTONCODE_SELECT1:
		// ブラウザで開く
		{
			// 自動ログイン変換
			CString requestUrl = url;
			if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
				// mixi モバイル用自動ログインURL変換
				requestUrl = util::ConvertToMixiMobileAutoLoginUrl( url );
			} else {
				// mixi 用自動ログインURL変換
				requestUrl = util::ConvertToMixiAutoLoginUrl( url );
			}

			// ブラウザで開く
			util::OpenUrlByBrowser( requestUrl );
		}
		break;
	case CCommonSelectDlg::BUTTONCODE_SELECT2:
		// MZ3でダウンロード
		{
			m_access = TRUE;
			m_abort = FALSE;

			// ダウンロードファイルパス
			m_infoEdit.ShowWindow(SW_SHOW);
			theApp.m_inet.Initialize( m_hWnd, NULL );
			theApp.m_accessType = ACCESS_DOWNLOAD;

			// ボタン状態の変更
			theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

			theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
		}
		break;
	default:
		break;
	}
}

/**
 * アクセス終了通知受信(HTML)
 */
LRESULT CReportView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{

	TRACE(_T("InetAccess End\n"));
	util::MySetInformationText( m_hWnd, _T("HTML解析中") );

	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return LRESULT();
	}

	bool bRetry = false;
	switch( theApp.m_accessType ) {
	case ACCESS_IMAGE:
	case ACCESS_MOVIE:
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			//イメージのURLを取得
			CString url;
			switch( theApp.m_accessType ) {
			case ACCESS_IMAGE:		url = mixi::ShowPictureParser::GetImageURL( html ); break;
			case ACCESS_MOVIE:		url = theApp.m_inet.GetURL();						break;
			default:
				break;
			}

			util::MySetInformationText( m_hWnd, _T("完了") );

			m_access = FALSE;

			// イメージのファイル名を生成
			CString strFilepath;
			CString strFilename;

			// url からファイル名を抽出する。
			// url : http://ic32.mixi.jp/p/ZZZ/ZZZ/album/ZZ/ZZ/XXXXX.jpg
			// url : http://ic46.mixi.jp/p/ZZZ/ZZZ/diary/ZZ/ZZ/XXXXX.jpg
			// url : http://ic32.mixi.jp/p/ZZZ/ZZZ/album/ZZ/ZZ/XXXXX.flv
			// とりあえず / 以降を使う
			int idx = url.ReverseFind( '/' );
			if( idx == -1 ) {
				// 見つからなかったのでファイル名不正エラー
				CString msg;
				msg.Format( 
					L"ファイル名が不明のため続行できません\n"
					L" url : [%s]", url );
				MZ3LOGGER_ERROR( msg );
				MessageBox( msg );
				return 0;
			}else{ 
				strFilename = url.Mid(idx+1);
				switch( theApp.m_accessType ) {
				case ACCESS_IMAGE:
					strFilepath.Format(_T("%s\\%s"), 
						theApp.m_filepath.imageFolder, 
						strFilename );
					break;
				case ACCESS_MOVIE:
					strFilepath.Format(_T("%s\\%s"), 
						theApp.m_filepath.downloadFolder, 
						strFilename );
					break;
				}
			}

			// 既にダウンロード済みなら再ダウンロードするか確認。
			if( util::ExistFile( strFilepath ) ) {
				CString msg;
				msg.Format( 
					L"同名のファイルがダウンロード済みです。\n"
					L"ファイル名：%s\n\n"
					L"再ダウンロードしますか？"
					, strFilename );

				CCommonSelectDlg dlg;
				dlg.SetMessage( msg );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT1, L"再ダウンロードする" );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, L"ダウンロード済みファイルを開く" );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_CANCEL,  L"やっぱりやめる" );
				if( dlg.DoModal() != IDOK ) {
					break;
				}

				int r = dlg.m_pressedButtonCode;
				switch( r ) {
				case CCommonSelectDlg::BUTTONCODE_SELECT1:
					// 再ダウンロード
					// ダウンロード実行。
					bRetry = true;
					break;
				case CCommonSelectDlg::BUTTONCODE_SELECT2:
					// ダウンロード済みファイルを開く
					util::OpenByShellExecute( strFilepath );
					break;
				default:
					break;
				}
			}else{
				bRetry = true;
			}

			if( bRetry ) {
				// イメージ・動画を再ダウンロード
				theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
			}
		}
		break;

	default:
		if( theApp.m_accessType == m_data->GetAccessType() ) {
			// リロード or ページ変更

			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// HTML 解析
			mixi::MyDoParseMixiHtml( m_data->GetAccessType(), *m_data, html );
			util::MySetInformationText( m_hWnd, _T("wait...") );

			theApp.m_pReportView->SetData( m_data );
			util::MySetInformationText( m_hWnd, L"完了" );

			// レポートビューに遷移
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
			theApp.ChangeView( theApp.m_pReportView );

			// ログファイルに保存
			if( theApp.m_optionMng.m_bSaveLog ) {
				// 保存ファイルパスの生成
				CString strLogfilePath = util::MakeLogfilePath( *m_data );
				if( !strLogfilePath.IsEmpty() ) {
					// 保存ファイルにコピー
					CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, 上書き*/ );
				}
			}
		}else{
			util::MySetInformationText( m_hWnd, _T("種別不明") );
		}
		break;
	}

	if( bRetry == false ) {
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

		// プログレスバーを非表示
		mc_progressBar.ShowWindow( SW_HIDE );

		m_infoEdit.ShowWindow(SW_HIDE);
	}

	return LRESULT();
}

/**
 * アクセス終了通知受信(Image)
 */
LRESULT CReportView::OnGetImageEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, NULL);
		return LRESULT();
	}

	m_access = FALSE;

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	m_infoEdit.ShowWindow(SW_HIDE);

	// 保存ファイルにコピー
	// パス生成
	CString strFilepath;
	{
		CString url = theApp.m_inet.GetURL();

		switch( theApp.m_accessType ) {
		case ACCESS_IMAGE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.imageFolder, 
				url.Mid( url.ReverseFind( '/' )+1 ) );
			break;

		case ACCESS_MOVIE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				url.Mid( url.ReverseFind( '/' )+1 ) );
			break;

		default:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				url.Mid( url.ReverseFind( '/' )+1 ) );
			break;
		}

		// コピー
		CopyFile( theApp.m_filepath.temphtml, strFilepath, FALSE/*bFailIfExists, 上書き*/ );
	}

	// ダウンロードの場合は、実行を確認する。
	if( theApp.m_optionMng.m_bUseRunConfirmDlg ) {
		CString msg;
		msg.Format( 
			L"ダウンロードが完了しました！\n\n"
			L"ファイル %s を開きますか？", strFilepath );

		if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) != IDYES ) {
			return LRESULT();
		}
	}

	// ファイルを開く
	SHELLEXECUTEINFO sei;
	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = strFilepath;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;
	ShellExecuteEx(&sei);

	return LRESULT();
}

/**
 * アクセスエラー通知受信
 */
LRESULT CReportView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);

	LPCTSTR smsg = L"エラーが発生しました";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"原因：%s", smsg, theApp.m_inet.GetErrorMessage() );
	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	m_access = FALSE;
	m_infoEdit.ShowWindow(SW_HIDE);

	return LRESULT();
}

/**
 * アクセス中断通知受信
 * 
 * ユーザ指示による中断
 * 
 */
LRESULT CReportView::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	// ユーザからのアクセス中断指示をうけると、ここに通知が帰ってくる
	// ボタンを元に戻してメッセージをだして処理終了
	util::MySetInformationText( m_hWnd, _T("中断しました") );

	// 中止ボタンを使用不可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;

	return LRESULT();
}

/**
 * 中断処理
 */
LRESULT CReportView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// 通信中でないならすぐに終了する
	if( !theApp.m_inet.IsConnecting() ) {
		return LRESULT();
	}
	theApp.m_inet.Abort();
	m_abort = TRUE;

	LPCTSTR msg = _T("中断しました");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	// 中止ボタンを使用不可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;
	return LRESULT();
}

/**
 * アクセス情報通知
 */
LRESULT CReportView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
  m_infoEdit.SetWindowText(*(CString*)lParam);
  return LRESULT();
}

/**
 * イメージボタン押下時の処理
 */
void CReportView::OnImageButton()
{
	POINT pt;
	RECT rect;
	CMenu menu;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	pt.x = (rect.right-rect.left) / 2;
	pt.y = (rect.bottom-rect.top) / 2;
	menu.LoadMenu(IDR_IMAGE_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	CString imageName;
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			// セパレータを付けておく
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));

			// 画像リンクを生成する
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				imageName.Format(_T("画像%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
			}
		}
	}

	pcThisMenu->DeleteMenu(0, MF_BYPOSITION);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);
}

/**
 * 書き込み開始（本文入力ビューの表示）
 */
void CReportView::OnWriteComment()
{
	// 引用方法の確認
	quote::QuoteType quoteType = quote::QUOTETYPE_INVALID;
	int idx = m_list.GetSelectedItem();
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	{
		CQuoteDlg dlg;
		dlg.m_pMixi = data;
		dlg.DoModal();
		quoteType = dlg.m_quoteType;

		// キャンセルなら終了
		if( quoteType == quote::QUOTETYPE_INVALID ) {
			return;
		}
	}

	// 書き込みビューを表示
	CWriteView* pWriteView = ((CWriteView*)theApp.m_pWriteView);
	if (m_data->GetAccessType() == ACCESS_MESSAGE) {
		pWriteView->StartWriteView( WRITEVIEW_TYPE_REPLYMESSAGE, m_data );
	} else {
		pWriteView->StartWriteView( WRITEVIEW_TYPE_COMMENT, m_data );
	}

	// 引用する
	if( quoteType != quote::QUOTETYPE_INVALID && data != NULL ) {
		CString str = quote::MakeQuoteString( *data, quoteType );

		((CEdit*)pWriteView->GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(str);
	}

}

/**
 * 書き込みボタン
 */
void CReportView::OnWriteButton()
{
	OnWriteComment();
}

/**
 * ＦＩＴ
 */
LRESULT CReportView::OnFit(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if( theApp.m_bPocketPC ) {
		OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	}

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, !m_data->IsOtherDiary());
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

	return LRESULT();
}

/**
 * 他ビューからの復帰処理
 */
LRESULT CReportView::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	theApp.ChangeView(theApp.m_pReportView);

	// Write ビューが有効ならONに。
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->m_sendEnd==FALSE ? TRUE : FALSE );

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );

	return LRESULT();
}

/**
 * 再読込（投稿後など）
 */
LRESULT CReportView::OnReload(WPARAM wParam, LPARAM lParam)
{
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data->GetURL()), _T(""), CInetAccess::FILE_HTML );

	return LRESULT();
}

/**
 * ブラウザで開く
 */
void CReportView::OnOpenBrowser()
{
	util::OpenBrowserForUrl( m_data->GetBrowseUri() );
}

/**
 * ブラウザでユーザのプロフィールページを開く
 */
void CReportView::OnOpenBrowserUser()
{
	// 選択アイテムの取得
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// ユーザ名とURLの生成
	CString strUserName = data->GetAuthor();
	int nUserId = data->GetAuthorID();
	if( nUserId < 0 ) {
		nUserId = data->GetOwnerID();
	}
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// 開く
	util::OpenBrowserForUser( url, strUserName );
}

void CReportView::MyPopupReportMenu(void)
{
	CMenu menu;
	POINT pt;
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	pt.x = (rect.right - rect.left) / 2;
	pt.y = (rect.bottom - rect.top) / 2;

	menu.LoadMenu(IDR_REPORT_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// 「書き込み」に関する処理
	switch( m_data->GetAccessType() ) {
	case ACCESS_MESSAGE:
		// メッセージなら、送信箱か受信箱かによって処理が異なる
		if( m_data->GetURL().Find( L"&box=outbox" ) != -1 ) {
			// 送信箱なので、書き込み無効
			pcThisMenu->RemoveMenu( ID_WRITE_COMMENT, MF_BYCOMMAND);
		}else{
			// 受信箱なので、「メッセージ返信」に変更
			pcThisMenu->ModifyMenu( ID_WRITE_COMMENT,
				MF_BYCOMMAND,
				ID_WRITE_COMMENT,
				_T("メッセージ返信"));
		}
		break;
	case ACCESS_NEWS:
		// ニュースなら書き込み無効
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		break;
	}

	// 外部ブログなら、書き込み無効
	if (m_data->IsOtherDiary() != FALSE) {
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
	}

	// 「次へ」無効化
	int idxPage = 4;		// 「ページ」メニュー（次へが有効の場合は-1となる）
	if( theApp.m_pWriteView->m_sendEnd ) {
		pcThisMenu->RemoveMenu(ID_NEXT_MENU, MF_BYCOMMAND);
		idxPage = idxPage-1;
	}

	// 「ページ」の追加
	// ページリンクがあれば追加。
	if( !m_data->m_linkPage.empty() ) {
		// リンクがあるので追加。
		CMenu* pSubMenu = pcThisMenu->GetSubMenu( idxPage );
		if( pSubMenu != NULL ) {
			// 追加
			for (int i=0; i<(int)m_data->m_linkPage.size(); i++) {
				const CMixiData::Link& link = m_data->m_linkPage[i];
				pSubMenu->AppendMenu(MF_STRING, ID_REPORT_PAGE_LINK_BASE+i+1, link.text);
			}
		}
	}

	// 画像
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				CString imageName;
				imageName.Format(_T("画像%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
			}
		}
	}

	// 動画
	if (m_currentData != NULL) {
		if (m_currentData->GetMovieCount() > 0) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_MOVIE, _T("-"));
			for (int i=0; i<m_currentData->GetMovieCount(); i++) {
				CString MovieName;
				MovieName.Format(_T("動画%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_MOVIE+i+1, MovieName);
			}
		}
	}

	// リンク
	if( m_currentData != NULL ) {
		int n = (int)m_currentData->m_linkList.size();
		if( n > 0 ) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
			for( int i=0; i<n; i++ ) {
				// 追加
				CString s;
				s.Format( L"link : %s", m_currentData->m_linkList[i].text );
				pcThisMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
			}
		}
	}

	// メニューのポップアップ
	menu.GetSubMenu(0)->
		TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);	  
}

/**
 * 現在の項目に関するデバッグ情報を表示する
 */
void CReportView::OnShowDebugInfo()
{
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}

	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);
	theApp.ShowMixiData( data );
}


void CReportView::OnUpdateWriteComment(CCmdUI *pCmdUI)
{
}

/// 前の画面に戻るボタン押下イベント
void CReportView::OnMenuBack()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/// 右ソフトキーメニュー｜前へ
void CReportView::OnBackMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/// 右ソフトキーメニュー｜次へ
void CReportView::OnNextMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnForwardButton();
}

/**
 * 受信サイズ通知
 */
LRESULT CReportView::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
{
	if( dwLength == 0 ) {
		// 総サイズが不明なので、仮に N KB とみなす
		dwLength = 40 * 1024;
	}

	// 受信サイズと総サイズが同一なら、受信完了とみなし、プログレスバーを非表示にする。
	if( dwLoaded == dwLength ) {
		mc_progressBar.ShowWindow( SW_HIDE );
		return TRUE;
	}

	// プログレスバーを表示
	mc_progressBar.ShowWindow( SW_SHOW );
	::SetWindowPos( mc_progressBar.m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION );

	// [0,1000] で受信サイズ通知
	int pos = (int)(dwLoaded * 1000.0 / dwLength);
	mc_progressBar.SetPos( pos );

	return TRUE;
}

int CReportView::GetListWidth(void)
{
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();
	// ピクセル数の微調整（スクリーン幅より少し小さくする）
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 30;
		break;
	case SR_QVGA:
	default:
		w -= 30/2;
		break;
	}
	return w;
}

/**
 * 表示する要素に応じて、カラムサイズ（幅）を再設定する。
 */
void CReportView::ResetColumnWidth(const CMixiData& mixi)
{
	if( m_list.m_hWnd == NULL )
		return;

	// 要素種別が「ヘルプ」なら日時を表示しない。

	// 幅の定義
	int w = GetListWidth();

	// ヘルプなら、W_COL1:(W_COL2+W_COL3):0 の比率で分割する
	// ヘルプ以外なら、W_COL1:W_COL2:W_COL3 の比率で分割する
	const int W_COL1 = theApp.m_optionMng.m_nReportViewListCol1Ratio;
	const int W_COL2 = theApp.m_optionMng.m_nReportViewListCol2Ratio;
	const int W_COL3 = theApp.m_optionMng.m_nReportViewListCol3Ratio;
	switch( mixi.GetAccessType() ) {
	case ACCESS_HELP:
		// ヘルプ
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * (W_COL2+W_COL3)/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, 0 );
		break;
	default:
		// ヘルプ以外
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, w * W_COL3/(W_COL1+W_COL2+W_COL3) );
		break;
	}
}

/**
 * ヘッダのドラッグ終了
 *
 * カラム幅の再構築を行う。
 */
void CReportView::OnHdnEndtrackReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MZ3LOGGER_DEBUG( L"OnHdnEndtrackReportList" );

	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// カラム幅の反映
	m_list.SetColumnWidth( phdr->iItem, phdr->pitem->cxy );

	CRect rect;
	CHeaderCtrl* pHeader = NULL;

	// リストの取得
	if( (pHeader = m_list.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"リストのヘッダを取得できないので終了" );
		return;
	}

	// カラム１
	if(! pHeader->GetItemRect( 0, rect ) ) {
		MZ3LOGGER_ERROR( L"リストのヘッダ、第1カラムの幅を取得できないので終了" );
		return;
	}
	theApp.m_optionMng.m_nReportViewListCol1Ratio = rect.Width();

	// カラム２
	if(! pHeader->GetItemRect( 1, rect ) ) {
		MZ3LOGGER_ERROR( L"リストのヘッダ、第2カラムの幅を取得できないので終了" );
		return;
	}
	theApp.m_optionMng.m_nReportViewListCol2Ratio = rect.Width();

	// カラム３
	// 最終カラムなので、リスト幅-他のカラムサイズとする。
	theApp.m_optionMng.m_nReportViewListCol3Ratio
		= GetListWidth() 
			- theApp.m_optionMng.m_nReportViewListCol1Ratio
			- theApp.m_optionMng.m_nReportViewListCol2Ratio;

	*pResult = 0;
}

LRESULT CReportView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_NOTIFY:
#ifdef WINCE
		NM_HTMLVIEW * pnmHTML = (NM_HTMLVIEW *) lParam;
		if (!pnmHTML)
			break;
		LPNMHDR pnmh = (LPNMHDR) &(pnmHTML->hdr);

		switch(pnmh->code) {
		case NM_HOTSPOT:
			break;

		case NM_INLINE_IMAGE:
			// HTML コントロールのインライン画像の要求

			// 画像をダウンロードする
			LoadHTMLImage(pnmHTML->szTarget, pnmHTML->dwCookie);

			// バグ回避のため親プロシージャへの転送を抑止する。
			return TRUE;
		}
#endif
		break;
	}

	return CFormView::WindowProc(message, wParam, lParam);
}

BOOL CReportView::LoadHTMLImage(LPCTSTR szTarget, DWORD dwCookie) 
{
#ifdef WINCE
	// TODO 絵文字のローカルキャッシュ化、またはキュー化＆HTML再ロードの仕組みを実装すること。
	CFile file;
	LPCTSTR szCacheFile = theApp.m_filepath.temphtml;

	if(file.Open(szCacheFile,CFile::modeCreate + CFile::modeWrite)) 
	{ 
		//Setup the internet connect for html file download 
		HINTERNET hInternetImage = NULL; 
		DWORD dwRead = 0; 
		char cBuffer[4096]="\0"; 
		//WINCE all transfers are binary 
		//It is transfer and save onto a file on pda 
		hInternetImage = InternetOpenUrl(theApp.m_inet.m_hInternet, szTarget, 
			NULL, 0, 
			INTERNET_FLAG_EXISTING_CONNECT, 
			0); 
		if (!hInternetImage) 
		{ 
			CString msg;
			msg.Format( 
				L"Can't Open URL! Check URL or connection.\n\n"
				L" URL : [%s]", szTarget );
			MZ3LOGGER_ERROR(msg); 
			InternetCloseHandle(hInternetImage); 
			return FALSE; 
		} 

		//Loop to read file 
		while (InternetReadFile(hInternetImage, &cBuffer, 4096, &dwRead)) 
		{ 
			file.Write(&cBuffer,4096); 
			if(dwRead ==0) 
				break; 
			memset(&cBuffer,0,4096); 
		} 
		InternetCloseHandle(hInternetImage); 
		file.Close(); 

	} //end of CFile:Open 
	else 
	{
		MZ3LOGGER_ERROR(_T("File error"));
		return FALSE;
	} 

	// this is to read the image file from the pda and send to the 
	// DTM_SETIMAGE for display 
	// Problems occured here, when it send to DTM_SETIMAGE, it display on 
	// the screen for a while and disappear 
	INLINEIMAGEINFO imgInfo; 
	HBITMAP hBitmap = SHLoadImageFile( szCacheFile );
	if( hBitmap ) {
		BITMAP bmp;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		imgInfo.dwCookie    = dwCookie; 
		imgInfo.iOrigHeight = bmp.bmHeight;
		imgInfo.iOrigWidth  = bmp.bmWidth;
		imgInfo.hbm         = hBitmap;
		imgInfo.bOwnBitmap  = TRUE;				// HTML コントロールにBITMAPを破棄させる
		::SendMessage( m_hwndHtml, DTM_SETIMAGE, 0, (LPARAM)&imgInfo ); 
	} 
	else 
	{ 
		MZ3LOGGER_ERROR(_T("Load Not Ok"));
		::SendMessage( m_hwndHtml, DTM_IMAGEFAIL, 0, dwCookie );
	} 
#endif
	return TRUE; 
}

static const int N_HC_MIN = 10;		///< リストの最小値 [%]
static const int N_HC_MAX = 90;		///< リストの最大値 [%]
static const int N_HB_MIN = 10;		///< エディットの最小値 [%]
static const int N_HB_MAX = 90;		///< エディットの最大値 [%]

/**
 * リストを狭くする
 */
void CReportView::OnLayoutReportlistMakeNarrow()
{
	int& hc = theApp.m_optionMng.m_nReportViewListHeightRatio;
	int& hb = theApp.m_optionMng.m_nReportViewBodyHeightRatio;

	// オプション値を % に補正
	int sum = hc + hb;
	if (sum>0) {
		hc = (int)(hc * 100.0 / sum);
		hb = (int)(hb * 100.0 / sum);
	}

	// 狭める
	const int STEP = 5;
	hc -= STEP;
	hb += STEP;

	if (sum<=0 || hc < N_HC_MIN || hb > N_HB_MAX) {
		// 最小値に設定
		hc = N_HC_MIN;
		hb = N_HB_MAX;
	}

	// 再描画
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * リストを広くする
 */
void CReportView::OnLayoutReportlistMakeWide()
{
	int& hc = theApp.m_optionMng.m_nReportViewListHeightRatio;
	int& hb = theApp.m_optionMng.m_nReportViewBodyHeightRatio;

	// オプション値を % に補正
	int sum = hc + hb;
	if (sum>0) {
		hc = (int)(hc * 100.0 / sum);
		hb = (int)(hb * 100.0 / sum);
	}

	// 広くする
	const int STEP = 5;
	hc += STEP;
	hb -= STEP;

	if (sum<=0 || hc > N_HC_MAX || hb < N_HB_MIN) {
		// 最小値に設定
		hc = N_HC_MAX;
		hb = N_HB_MIN;
	}

	// 再描画
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}
