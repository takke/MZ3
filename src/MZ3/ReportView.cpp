/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// ReportView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "MainFrm.h"
#include "IniFile.h"
#include "WriteView.h"
#include "HtmlArray.h"
#include "util.h"
#include "util_gui.h"
#include "QuoteDlg.h"
#include "MixiParser.h"
#include "ViewFilter.h"
#include "CommonSelectDlg.h"
#include "Ran2View.h"

#define MASK_COLOR RGB(255,0,255);

// CReportView

IMPLEMENT_DYNCREATE(CReportView, CFormView)

/**
 * コンストラクタ
 */
CReportView::CReportView()
	: CFormView(CReportView::IDD)
	, m_nKeydownRepeatCount(0)
	, m_currentData(NULL)
{
	m_imageState = FALSE;
	m_detailView = NULL;
}

/**
 * デストラクタ
 */
CReportView::~CReportView()
{
	delete m_detailView;
}

void CReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_LIST, m_list);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_VSCROLLBAR, m_vScrollbar);
}

BEGIN_MESSAGE_MAP(CReportView, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_VSCROLL()
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
    ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
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
	ON_NOTIFY(NM_RCLICK, IDC_REPORT_LIST, &CReportView::OnNMRclickReportList)
	ON_COMMAND(ID_OPEN_PROFILE, &CReportView::OnOpenProfile)
	ON_COMMAND(ID_OPEN_PROFILE_LOG, &CReportView::OnOpenProfileLog)
	ON_COMMAND(ID_SEND_MESSAGE, &CReportView::OnSendMessage)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()

	ON_MESSAGE(WM_MZ3_MOVE_DOWN_LIST, OnMoveDownList)
	ON_MESSAGE(WM_MZ3_MOVE_UP_LIST, OnMoveUpList)
	ON_MESSAGE(WM_MZ3_GET_LIST_ITEM_COUNT, OnGetListItemCount)

	ON_COMMAND_RANGE(ID_REPORT_COPY_URL_BASE+1, ID_REPORT_COPY_URL_BASE+50, OnCopyClipboardUrl)
	ON_COMMAND_RANGE(ID_REPORT_COPY_IMAGE+1, ID_REPORT_COPY_IMAGE+50, OnCopyClipboardImage)
	ON_COMMAND_RANGE(ID_REPORT_COPY_MOVIE+1, ID_REPORT_COPY_MOVIE+50, OnCopyClipboardMovie)
	ON_COMMAND(ID_MENU_NEXT_DIARY, &CReportView::OnMenuNextDiary)
	ON_COMMAND(ID_MENU_PREV_DIARY, &CReportView::OnMenuPrevDiary)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT_DIARY, &CReportView::OnUpdateMenuNextDiary)
	ON_UPDATE_COMMAND_UI(ID_MENU_PREV_DIARY, &CReportView::OnUpdateMenuPrevDiary)
	ON_COMMAND(IDM_LOAD_FULL_DIARY, &CReportView::OnLoadFullDiary)
	ON_UPDATE_COMMAND_UI(IDM_LOAD_FULL_DIARY, &CReportView::OnUpdateLoadFullDiary)
	ON_COMMAND_RANGE(ID_LUA_MENU_BASE, ID_LUA_MENU_BASE+1000, OnLuaMenu)
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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

		// スタイルの更新
		m_list.ModifyStyle(0, dwStyle);

		// アイコンリストの作成
		m_pimgList.Create(16, 16, ILC_COLOR4 | ILC_MASK, 2, 0);
		m_pimgList.Add( AfxGetApp()->LoadIcon(IDI_NO_PHOTO_ICON) );
		m_pimgList.Add( AfxGetApp()->LoadIcon(IDI_PHOTO_ICON) );
		m_list.SetImageList(&m_pimgList, LVSIL_SMALL);

		// カラムの追加
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("名前"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("日時"), LVCFMT_LEFT, 20, -1);

		// オプションの設定
		m_list.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
		m_list.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
	}

	//--- 通知領域の変更
	{
		// フォント変更
		m_infoEdit.SetFont( &theApp.m_font );
	}

	m_nochange = FALSE;

	// スクロール量の初期値設定
	m_scrollLine = theApp.m_optionMng.m_reportScrollLine;

	const int DETAIL_VIEWID = 1000;	// 暫定なのでてきとー
	if( m_detailView != NULL ){
		delete m_detailView;
	}

	// 画面サイズで構築する（超富豪的・・・）
	CRect viewRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &viewRect, 0);
	m_detailView = new Ran2View();
//	TRACE(TEXT("sy=%d,viewWidth=%d,viewHeight=%d\r\n"),sy,viewWidth,viewHeight);

	MZ3LOGGER_INFO(L"らんらんビュー初期化開始");
	m_detailView->Create(TEXT("RAN2WND"),TEXT(""),CS_GLOBALCLASS,viewRect,(CWnd*)this,DETAIL_VIEWID);
	MZ3LOGGER_INFO(L"らんらんビュー初期化完了(1/2)");

	// 超暫定
	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 9;
	}
	m_detailView->ChangeViewFont( fontHeight, theApp.m_optionMng.GetFontFace() );
	m_detailView->ShowWindow(SW_SHOW);
	MZ3LOGGER_INFO(L"らんらんビュー初期化完了(2/2)");
}

/**
 * サイズ変更時の処理
 */
void CReportView::OnSize(UINT nType, int cx, int cy)
{
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

	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	if( fontHeight == 0 ) {
		fontHeight = 12;
	}

	int hTitle  = theApp.GetInfoRegionHeight(fontHeight);	// タイトル領域はフォントサイズ依存

	const int h1 = theApp.m_optionMng.m_nReportViewListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nReportViewBodyHeightRatio;
	int hList   = (cy * h1 / (h1+h2))-hTitle;	// (全体のN%-タイトル領域) をリスト領域とする
	int hReport = (cy * h2 / (h1+h2));			// 全体のN%をレポート領域とする

	// 情報領域は必要に応じて表示されるため、上記の比率とは関係なくサイズを設定する
	int hInfo   = theApp.GetInfoRegionHeight(fontHeight);	// 情報領域もフォントサイズ依存

	// スクロールバーの幅
	int barWidth = ::GetSystemMetrics(SM_CXVSCROLL);

	// 各コントロールの移動
	util::MoveDlgItemWindow( this, IDC_TITLE_EDIT,  0, 0,            cx, hTitle  );
	util::MoveDlgItemWindow( this, IDC_REPORT_LIST, 0, hTitle,       cx, hList   );

	// RAN2 の移動
	if (m_detailView && ::IsWindow(m_detailView->GetSafeHwnd())) {
		int wRan2 = cx - barWidth;
		m_detailView->MoveWindow( 0, hTitle+hList, wRan2, hReport );
		ShowCommentData( m_currentData );
	}

	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,   0, cy - hInfo,   cx, hInfo   );

	// スクロールバー調整
//	util::MoveDlgItemWindow(this, IDC_VSCROLLBAR, cx-barWidth, cy - hInfo, cx, hInfo);
	if( m_vScrollbar ){
		int barSX = cx - barWidth;
		util::MoveDlgItemWindow(this, IDC_VSCROLLBAR, cx-barWidth, hTitle+hList, barWidth, hReport);
		m_vScrollbar.SetWindowPos( m_detailView, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		// スクロールバーが不要な時は隠す
		m_vScrollbar.ShowWindow(SW_HIDE);
		int viewLineCount = m_detailView->GetViewLineMax();
		int allLineCount  = m_detailView->GetAllLineCount();
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		si.nMin  = 0;
		si.nMax  = allLineCount-1;
		si.nPage = viewLineCount;
		si.nPos  = 0;
		m_vScrollbar.SetScrollInfo(&si, TRUE);
		if( allLineCount-viewLineCount > 0 ) {
			m_vScrollbar.ShowWindow(SW_SHOW);
		}
	}

	// ラベルぬっ殺しモードの場合はスタイルを変更すっぺよ
	if( theApp.m_optionMng.m_killPaneLabel ){
		util::ModifyStyleDlgItemWindow(this,IDC_REPORT_LIST,NULL,LVS_NOCOLUMNHEADER);
	}

	// スクロールタイプが「ページ単位」なら再計算
	if( theApp.m_optionMng.m_reportScrollType == option::Option::REPORT_SCROLL_TYPE_PAGE ) {
		if (m_detailView != NULL) {
			int charHeightOffset = m_detailView->GetCharHeightOffset();
			if ((fontHeight+charHeightOffset)>0) {
				m_scrollLine = (hReport / (fontHeight+charHeightOffset)) - 2;
			}
		}
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
	ResetColumnWidth( m_data );
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
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
		break;

	default:
		pDC->SetBkMode(TRANSPARENT);
		break;
	}

	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

/**
 * データ設定
 */
void CReportView::SetData(const CMixiData& data)
{
	// 初期化処理
	m_data = data;
	m_nochange = TRUE;
	m_lastIndex = 0;

	m_list.DeleteAllItems();
	m_list.SetRedraw(FALSE);

	// カラムサイズを変更する
	ResetColumnWidth( m_data );

	// どこまでデータを取得したかを設定する
	TRACE(_T("Address = %s\n"), m_data.GetURL());

	// ID を設定しておく
	if (m_data.GetID() == -1) {
		// ここでＩＤを作る
		m_data.SetID( mixi::MixiUrlParser::GetID(m_data.GetURL()) );
	}

	// 既読位置の変更
	m_lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(m_data);
	if (m_lastIndex == -1) {
		m_lastIndex = 0;
	} else {
		m_lastIndex ++;
	}

	INT_PTR count = m_data.GetChildrenSize();
	if (count > 0 && m_lastIndex > 0) {
		CMixiData& subItem = m_data.GetChild( count-1 );
		if (m_lastIndex > subItem.GetCommentIndex()) {
			m_lastIndex = subItem.GetCommentIndex();
		}
	}

	// タイトルの設定
	CString title = m_data.GetTitle();
	switch (m_data.GetAccessType()) {
#ifdef BT_MZ3
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
		break;
#endif
	default:
		if( !m_data.GetName().IsEmpty() ) {
			title.AppendFormat( _T("(%s)"), (LPCTSTR)m_data.GetName() );
		}
		break;
	}
	m_titleEdit.SetWindowText( title );


	// ----------------------------------------
	// コメントの追加
	// ----------------------------------------
	TRACE(_T("コメント数 = [%d]\n"), count);

	int focusItem = 0;
	for (int i=0; i<count; i++) {
		CMixiData& subItem = m_data.GetChild(i);

		// 画像の有無でアイコンのインデックスを変更する
		int imageIndex = (subItem.GetImageCount() == 0) ? 0 : 1;
		CString strIndex;
		if (subItem.GetCommentIndex()>=0) {
			// 未指定の場合は空白とする。
			strIndex = util::int2str(subItem.GetCommentIndex());
		}
		int idx = m_list.InsertItem(i+1, strIndex, imageIndex);

		if (subItem.GetCommentIndex() == m_lastIndex) {
			// この次の項目を選択項目とする
			focusItem = idx + 1;
		}

		// Author 列
		m_list.SetItem(idx, 1, LVIF_TEXT | LVIF_IMAGE, subItem.GetAuthor(), 0, 0, 0, 0);
		// Date 列
		m_list.SetItem(idx, 2, LVIF_TEXT, subItem.GetDate(), 0, 0, 0, 0);
		// ItemData に CMixiData* を与える
		m_list.SetItemData(idx, (DWORD_PTR)&subItem);
	}

	// 親をリストに表示
	{
		// 画像の有無でアイコンのインデックスを変更する
		int imageIndex = (m_data.GetImageCount() == 0) ? 0 : 1;
		int idx = m_list.InsertItem(0, _T("-"), imageIndex);
		// Author 列
		m_list.SetItem(idx, 1, LVIF_TEXT, m_data.GetAuthor(), 0, 0, 0, 0);
		// Date 列 : 先頭項目には初期版から表示していない。必要であれば、下記をコメントアウトすること。
//		m_list.SetItem(idx, 2, LVIF_TEXT, m_data.GetDate(), 0, 0, 0, 0);
		if( !m_data.GetOpeningRange().IsEmpty() ) {
			m_list.SetItem(idx, 2, LVIF_TEXT, m_data.GetOpeningRange() , 0, 0, 0, 0);
		}

		// ItemData に CMixiData* を与える
		m_list.SetItemData(0, (DWORD_PTR)&m_data);
	}

	m_nochange = FALSE;

	if (count == 0) {
		// コメントがないため、親要素を既読位置とする。
		m_lastIndex = 0;
	} else {
		// 選択状態の正規化
		CMixiData& firstCommentItem = m_data.GetChild(0);
		if (m_lastIndex < firstCommentItem.GetCommentIndex() && m_lastIndex > 0) {
			// 先頭項目のインデックスよりも既読位置が小さい場合、
			// 以前に見ていた既読項目が「流れている」と判断し、
			// 既読位置を先頭項目のインデックスを与える。
			m_lastIndex = firstCommentItem.GetCommentIndex();
			focusItem = 1;
		}
	}

	// プロフィール表示の場合は常に先頭項目を選択
#ifdef BT_MZ3
	if (m_data.GetAccessType() == ACCESS_PROFILE ||
		m_data.GetAccessType() == ACCESS_BIRTHDAY ||
		m_data.GetAccessType() == ACCESS_NEIGHBORDIARY ) {
		focusItem = 0;
	}
#endif

	//--- UI 関連
	m_list.SetRedraw(TRUE);
	m_list.SetFocus();
	m_list.SetItemState( focusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	m_list.EnsureVisible( focusItem, FALSE );

	MyUpdateControlStatus();
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
	if (data==NULL) {
		return;
	}

	CStringArray* bodyStrArray = new CStringArray();

	// 書体を変更して1行目を描画
	bodyStrArray->Add(L"<blue>");
	bodyStrArray->Add(data->GetAuthor() + L"　" + data->GetDate() );
	if( !data->GetOpeningRange().IsEmpty() ) {
		bodyStrArray->Add( L"　【" + data->GetOpeningRange() + L"】" );
	}
	bodyStrArray->Add(L"</blue>");
	bodyStrArray->Add(L"<br>");

	// 2行目を描画
	if( !data->GetPrevDiary().IsEmpty() || !data->GetNextDiary().IsEmpty() ){
		if( !data->GetPrevDiary().IsEmpty() ){
			//CString PrevLink = data->GetPrevDiary();
			//ViewFilter::ReplaceHTMLTagToRan2Tags( PrevLink, *bodyStrArray, theApp.m_emoji, this );
			bodyStrArray->Add(L"<prevdiary>");
			bodyStrArray->Add(L"<<前の日記へ");
			bodyStrArray->Add(L"</prevdiary>");
		}
		bodyStrArray->Add(L"　");
		if( !data->GetNextDiary().IsEmpty() ){
			//CString NextLink = data->GetNextDiary();
			//ViewFilter::ReplaceHTMLTagToRan2Tags( NextLink, *bodyStrArray, theApp.m_emoji, this );
			bodyStrArray->Add(L"<nextdiary>");
			bodyStrArray->Add(L"次の日記へ>>");
			bodyStrArray->Add(L"</nextdiary>");
		}
		bodyStrArray->Add(L"<br>");
	}

//	TRACE( L"■---xdump start---\r\n" );
//	for (int i=0; i<data->GetBodySize(); i++) {
//		TRACE( L"{%d}%s|\r\n", CString(data->GetBody(i)).GetLength(), data->GetBody(i) );
//	}
//	TRACE( L"■---xdump end---\r\n" );

	// 先頭に改行があったりと、色々フォーマットがおかしいので単一の文字列に連結する
	CString str = data->GetBody();

	// 改行分割して追加する
	for (;;) {
		int idxCrlf = str.Find( L"\r\n" );

		CString target;
		if (idxCrlf == -1) {
			target = str;
			if (target.IsEmpty()) {
				break;
			}
		} else {
			target.SetString( str, idxCrlf );
		}

		if( target.GetLength() == 0 ) {
			if (idxCrlf != -1) {
				LPCTSTR brLine = TEXT("<br>");
				bodyStrArray->Add(brLine);
			}
		} else {
			// 絵文字用フィルタ
			//ViewFilter::ReplaceEmojiCodeToRan2ImageTags( target, *bodyStrArray, theApp.m_emoji, this );
			ViewFilter::ReplaceHTMLTagToRan2Tags( target, *bodyStrArray, theApp.m_emoji, this );
			LPCTSTR brLine = TEXT("<br>");
			bodyStrArray->Add(brLine);
		}

		if (idxCrlf == -1) {
			break;
		}
		str.Delete(0, idxCrlf+2);
	}

	// 最後に１行入れて見やすくする
	bodyStrArray->Add(_T("<br>"));

//	TRACE( L"■---dump start---\r\n" );
//	for (int i=0; i<bodyStrArray->GetCount(); i++) {
//		TRACE( L"{%d}%s|\r\n", bodyStrArray->GetAt(i).GetLength(), bodyStrArray->GetAt(i) );
//	}
//	TRACE( L"■---dump end---\r\n" );

	// blockquoteの前に改行を入れる
	ViewFilter::InsertBRTagToBeforeblockquoteTag( bodyStrArray );

	// 描画開始
	m_scrollBarHeight = m_detailView->LoadDetail(bodyStrArray, &theApp.m_imageCache.GetImageList16());
	TRACE(TEXT("LoadDetailで%d行をパースしました\r\n"), m_scrollBarHeight);
	m_detailView->ResetDragOffset();
	// OnSize 時にこのルートを2回通るため、即時描画を行うとMZ3の場合に一瞬だけ画面が乱れる。
	// これを回避するため、即時描画を行わず、Invalidate により WM_PAINT 通知でまとめて描画する。
	m_detailView->DrawDetail(0, false);
	m_detailView->Invalidate(FALSE);
	bodyStrArray->RemoveAll();
	delete bodyStrArray;

	// らんらんビューオプション設定
	m_detailView->m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
	m_detailView->m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
	m_detailView->m_bUseDoubleClickMove = theApp.m_optionMng.m_bUseRan2DoubleClickMove;

	// スクロールバーが不要な時は隠す
	m_vScrollbar.ShowWindow(SW_HIDE);
	int viewLineCount = m_detailView->GetViewLineMax();
	int allLineCount  = m_detailView->GetAllLineCount();
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	si.nMin  = 0;
	si.nMax  = allLineCount-1;
	si.nPage = viewLineCount;
	si.nPos  = 0;
	m_vScrollbar.SetScrollInfo(&si, TRUE);
	if( allLineCount-viewLineCount > 0 ) {
		m_vScrollbar.ShowWindow(SW_SHOW);
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

	CMixiData* data = (CMixiData*)m_list.GetItemData(pNMLV->iItem);
	m_currentData = data;
	if (m_lastIndex < data->GetCommentIndex()) {
		// 既読位置が小さいため、より新しい項目を見たと判断し、
		// 既読位置を更新する。
		m_lastIndex = data->GetCommentIndex();
	}
	if ((pNMLV->iItem+1)==m_list.GetItemCount()) {
		// 最終項目を見ている場合、
		// （コメント削除等の可能性があるため）
		// URL 内に comment_count があれば、その値を既読位置とする。
		CString comment_count = util::GetParamFromURL( m_data.GetURL(), L"comment_count" );
		if (!comment_count.IsEmpty() && _wtoi(comment_count)>0) {
			m_lastIndex = _wtoi(comment_count);
		}
	}
		

	m_list.SetRedraw(FALSE);
	if (pNMLV->iItem == 0) {
		ShowParentData(data);
	}
	else {
		ShowCommentData(data);
	}

	m_imageState = (BOOL)(data->GetImageCount() > 0);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, ((!theApp.m_access) & m_imageState));
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, !theApp.m_access);

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

/// 一番上の項目に移動
BOOL CReportView::CommandMoveToFirstList()
{
	util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, false );
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );
	m_list.EnsureVisible( 0, FALSE );

	return TRUE;
}

/// 一番下の項目に移動
BOOL CReportView::CommandMoveToLastList()
{
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, false );
	util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, true );
	m_list.EnsureVisible( m_list.GetItemCount()-1, FALSE );

	return TRUE;
}

BOOL CReportView::CommandMoveUpList()
{
	// アイテム数が1以下であれば変更しない
	if (m_list.GetItemCount()<=1) {
		return TRUE;
	}

	if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE) {
		// 一番上の項目選択中なので、一番下に移動
		CommandMoveToLastList();
	} else {
		// 一番上ではないので、上に移動
		int idx = m_list.GetSelectedItem();
		if (idx<0) {
			// 選択なしなので最初の項目を選択
			idx = 0;
			util::MySetListCtrlItemFocusedAndSelected( m_list, idx, true );
			// 非表示ならスクロール
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				m_list.EnsureVisible( idx, FALSE );
			}
		} else {
			util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
			util::MySetListCtrlItemFocusedAndSelected( m_list, --idx, true );

			// 移動先が非表示なら上方向にスクロール
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				if( ( idx < m_list.GetTopIndex() ) &&
					( idx >= m_list.GetTopIndex() - m_list.GetCountPerPage() ) ) {
					// 移動先が画面より上、1画面以内にある時は1画面スクロール
					m_list.Scroll( CSize(0, -m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
				} else {
					// 移動先が画面より下か、上で1画面以上離れている時はEnsureVisible()
					m_list.EnsureVisible( idx , TRUE );
				}
			}
		}
	}

	if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
		m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_RIGHT);
	}

	return TRUE;
}

BOOL CReportView::CommandMoveDownList()
{
	// アイテム数が1以下であれば変更しない
	if (m_list.GetItemCount()<=1) {
		return TRUE;
	}

	if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
		// 一番下の項目選択中なので、一番上に移動
		CommandMoveToFirstList();
	} else {
		// 一番下ではないので、下に移動
		int idx = m_list.GetSelectedItem();
		if (idx<0) {
			// 選択なしなので最初の項目を選択
			idx = 0;
			util::MySetListCtrlItemFocusedAndSelected( m_list, idx, true );
			// 非表示ならスクロール
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				m_list.EnsureVisible( idx, FALSE );
			}
		} else {
			util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
			util::MySetListCtrlItemFocusedAndSelected( m_list, ++idx, true );

			// 移動先が非表示なら下方向にスクロール
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				if( ( idx >= m_list.GetTopIndex() + m_list.GetCountPerPage() ) &&
					( idx < m_list.GetTopIndex() + m_list.GetCountPerPage() * 2 ) ) {
					// 移動先が画面より下、1画面以内にある時は1画面スクロール
					m_list.Scroll( CSize(0, m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
				} else {
					// 移動先が画面より上か、下で1画面以上離れている時はEnsureVisible()
					m_list.EnsureVisible( idx , TRUE );
				}
			}
		}
	}

	if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
		m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_LEFT);
	}

	return TRUE;
}

BOOL CReportView::CommandScrollUpEdit()
{
	int pos = m_vScrollbar.GetScrollPos();
	if( pos <= 0 ) {
		return FALSE;
	}
	pos -= m_scrollLine;
	if (pos<0) {
		// 下限値補正
		pos = 0;
	}

	m_vScrollbar.SetScrollPos(pos);
	m_detailView->ResetDragOffset();
	m_detailView->DrawDetail(pos);

	return TRUE;
}

BOOL CReportView::CommandScrollDownEdit()
{
	int pos = m_vScrollbar.GetScrollPos();
	if( pos >= m_scrollBarHeight ){
		return FALSE;
	}
	pos += m_scrollLine;
	if (pos > m_scrollBarHeight) {
		// 上限値補正
		pos = m_scrollBarHeight;
	}

	m_vScrollbar.SetScrollPos(pos);
	m_detailView->ResetDragOffset();
	m_detailView->DrawDetail(pos);
	return TRUE;
}


BOOL CReportView::OnKeyUp(MSG* pMsg)
{
	switch (pMsg->wParam) {
#ifndef WINCE
	case VK_F1:
		// ヘルプ表示
		util::OpenByShellExecute( MZ4_MANUAL_URL );
		break;
#endif

#ifndef WINCE
	case VK_APPS:
		// レポートメニューの表示
		MyPopupReportMenu();
		return TRUE;
#endif

	case VK_BACK:				// クリアボタン
	case VK_ESCAPE:
		// リストの場合は前画面に戻る
		if (pMsg->hwnd == m_list.m_hWnd) {
			if (theApp.m_access) {
				// アクセス中は中断処理
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
			} else {
				OnMenuBack();
			}
			return TRUE;
		}

		// TODO: エディットコントロールにフォーカスがある場合に
		//       リストにフォーカスを移動したいが、ここではできない。
		//       エディットコントロールのサブクラス化が必要か？
		break;

/*	case VK_RETURN:
		if (pMsg->hwnd == m_list.m_hWnd) {
			// レポートメニューの表示
			MyPopupReportMenu();
			return TRUE;
		}
		break;
*/	}

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
					return CommandScrollUpEdit();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// キー長押しによる連続移動中なら、キーUPで移動しない。
						return TRUE;
					}

					// スクロール位置判定（スクロール余地があればスクロールする）
					if (CommandScrollUpEdit()) {
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
					return CommandScrollDownEdit();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// キー長押しによる連続移動中なら、キーUPで移動しない。
						return TRUE;
					}

					// スクロール位置判定（スクロール余地があればスクロールする）
					if (CommandScrollDownEdit()) {
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
					// スクロール位置判定（スクロール余地があればスクロールする）
					if (CommandScrollUpEdit()) {
						return TRUE;
					}
//					MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveUpList();
				}

				return TRUE;

			case VK_DOWN:
				// VK_KEYDOWN では無視。
				// VK_KEYUP で処理する。
				// これは、アドエスの Xcrawl 対応のため。

				// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
				if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
					// スクロール位置判定（スクロール余地があればスクロールする）
					if (CommandScrollDownEdit()) {
						return TRUE;
					}
//					MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveDownList();
				}

				return TRUE;
			}
		}
	} else {
		// Xcrawl オプション無効時の処理
		if (pMsg->hwnd == m_list.m_hWnd) {
			// リストでのキー押下イベント
			switch(pMsg->wParam) {
			case VK_UP:
				// スクロール位置判定（スクロール余地があればスクロールする）
				if (CommandScrollUpEdit()) {
					return TRUE;
				}

				// 項目変更
				if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE &&
					// アイテム数が1以下であれば変更しない
					m_list.GetItemCount()>1)
				{
					// 一番上の項目選択中なので、一番下に移動
					CommandMoveToLastList();

					if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
						m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_RIGHT);
					}

					return true;
				} else {
#ifdef WINCE
					// デフォルト動作
					return FALSE;
#else
					return CommandMoveUpList();
#endif
				}

			case VK_DOWN:
				// スクロール位置判定（スクロール余地があればスクロールする）
				if (CommandScrollDownEdit()) {
					return TRUE;
				}

				// 項目変更
				if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE &&
					// アイテム数が1以下であれば変更しない
					m_list.GetItemCount()>1)
				{
					// 一番下の項目選択中なので、一番上に移動
					CommandMoveToFirstList();

					if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
						m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_LEFT);
					}

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
			// 次のコメントに移動
			return CommandMoveDownList();

		case VK_LEFT:
			// 前のコメントに移動
			return CommandMoveUpList();

		case VK_SPACE:
			if (GetAsyncKeyState( VK_CONTROL ) & 0x8000) {
				// Ctrl+Space
				OnAcceleratorNextComment();
			} else {
				// Space
				OnAcceleratorScrollOrNextComment();
			}
			return TRUE;

		}
	}

	return FALSE;
}

BOOL CReportView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_list.m_hWnd) {
		switch (pMsg->message) {
		case WM_KEYUP:
			{
				BOOL r = OnKeyUp(pMsg);

				// KEYDOWN リピート回数を初期化
				m_nKeydownRepeatCount = 0;

				if( r ) {
					return TRUE;
				}
			}
			break;

		case WM_KEYDOWN:
			{
				// KEYDOWN リピート回数をインクリメント
				m_nKeydownRepeatCount ++;

				if( OnKeyDown(pMsg) ) {
					return TRUE;
				}
			}
			break;

		case WM_MOUSEWHEEL:
			// らんらんビュー上のホイールもフォーカスが違うのでこちらに来る。
			// マウスの座標で判定する。
			{
				CPoint pt;
				GetCursorPos( &pt );

				CRect rect;
				m_detailView->GetWindowRect( &rect );
				if (rect.PtInRect( pt )) {
					OnMouseWheel( LOWORD(pMsg->wParam), HIWORD(pMsg->wParam), CPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) );
					return TRUE;
				}
			}
			break;
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
 * m_lastIndex を保存
 */
void CReportView::SaveIndex()
{
	theApp.m_readlog.SetValue(util::GetLogIdString( m_data ), (LPCTSTR)util::int2str(m_lastIndex), "Log");
}

/**
 * ブックマークに追加
 */
void CReportView::OnAddBookmark()
{
#ifdef BT_MZ3
	if (m_data.GetAccessType() != ACCESS_BBS &&
		m_data.GetAccessType() != ACCESS_EVENT &&
		m_data.GetAccessType() != ACCESS_EVENT_JOIN &&
		m_data.GetAccessType() != ACCESS_ENQUETE) {
		::MessageBox(m_hWnd, _T("コミュニティ以外は\n登録出来ません"), MZ3_APP_NAME, NULL);
		return;
	}

	if( theApp.m_bookmarkMng.Add( &m_data, theApp.m_root.GetBookmarkList() ) != FALSE ) {
		::MessageBox(m_hWnd, _T("登録しました"), MZ3_APP_NAME, NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("既に登録されています"), MZ3_APP_NAME, NULL);
	}
#else
	// TODO 暫定
	::MessageBox(m_hWnd, _T("コミュニティ以外は\n登録出来ません"), MZ3_APP_NAME, NULL);
#endif
}

/**
 * ブックマークから削除
 */
void CReportView::OnDelBookmark()
{
	if (theApp.m_bookmarkMng.Delete(&m_data,theApp.m_root.GetBookmarkList()) != FALSE) {
		::MessageBox(m_hWnd, _T("削除しました"), MZ3_APP_NAME, NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("登録されていません"), MZ3_APP_NAME, NULL);
	}
}

/**
 * 画像ＤＬ
 */
void CReportView::OnLoadImage(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	CString url = m_currentData->GetImage(nID - ID_REPORT_IMAGE-1);
	MZ3LOGGER_DEBUG( L"画像ダウンロード開始 url[" + url + L"]" );

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// イメージURLをCGIから取得
	theApp.m_accessType = ACCESS_IMAGE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();
}

/**
 * 動画ＤＬ
 */
void CReportView::OnLoadMovie(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

#ifdef BT_MZ3
	CString url = m_currentData->GetMovie(nID - ID_REPORT_MOVIE-1);
	MZ3LOGGER_DEBUG( L"動画ダウンロード開始 url[" + url + L"]" );

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// 動画URLをCGIから取得
	theApp.m_accessType = ACCESS_MOVIE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );

	MyUpdateControlStatus();
#endif
}


/**
 * ページ変更
 */
void CReportView::OnLoadPageLink(UINT nID)
{
	int idx = nID - ID_REPORT_PAGE_LINK_BASE-1;
	if( 0 <= idx && idx <(int)m_data.m_linkPage.size() ) {
		// ok.
	}else{
		return;
	}

	MyLoadMixiViewPage( m_data.m_linkPage[idx] );
}

/**
 * mixi 内リンクページを開く
 */
bool CReportView::MyLoadMixiViewPage( const CMixiData::Link link )
{
	if (theApp.m_access) {
		return false;
	}

	ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
	if (theApp.m_accessTypeInfo.getInfoType(estimatedAccessType)==AccessTypeInfo::INFO_TYPE_BODY) {
		// 既読位置を保存
		SaveIndex();

		theApp.m_access = true;
		m_abort = FALSE;

		MyUpdateControlStatus();

		// m_data の書き換え
		{
			// 初期化
			CMixiData mixi;

			// データ構築
			mixi.SetURL( link.url );
			mixi.SetTitle( link.text );

			// 名前は引き継ぐ
			mixi.SetName( m_data.GetName() );

			// アクセス種別を設定
			mixi.SetAccessType( estimatedAccessType );
			
			m_data = mixi;
			m_data.SetBrowseUri(util::CreateMixiUrl(link.url));
		}
		theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );

		theApp.m_accessType = m_data.GetAccessType();
		theApp.m_inet.DoGet( util::CreateMixiUrl(link.url), _T(""), CInetAccess::FILE_HTML );
		return true;
	} else {

		m_infoEdit.ShowWindow(SW_SHOW);
		util::MySetInformationText( m_hWnd, L"未サポートのURLです：" + link.url );
		return false;
	}

	return true;
}

/**
 * 再読込
 */
void CReportView::OnReloadPage()
{
	// アクセス種別が「ヘルプ」なら何もしない
	switch( m_data.GetAccessType() ) {
	case ACCESS_HELP:
		return;
	}

	// 既読位置を保存
	SaveIndex();

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );

	theApp.m_accessType = m_data.GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data.GetURL()), _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();
}

/**
 * URL
 */
void CReportView::OnLoadUrl(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( m_currentData->m_linkList.size() == 0 || idx > m_currentData->m_linkList.size() ) {
		return;
	}

	// mixi 内リンクであればここでロードする。
	if (MyLoadMixiViewPage( m_currentData->m_linkList[idx] )) {
		return;
	} else {
		// mixi内リンクでなければエラーが表示されているので隠す
		m_infoEdit.ShowWindow(SW_HIDE);
	}

	LPCTSTR url  = m_currentData->m_linkList[idx].url;
	LPCTSTR text = m_currentData->m_linkList[idx].text;

	// mixi 内リンクでなかったたので、ブラウザで開く等の処理を行う

	// 確認画面
	CString msg;
	msg.Format( 
		L"下記のURLを開きます。\n"
		L"どの方法で開きますか？\n\n"
		L"%s", url );

	CCommonSelectDlg dlg;
	dlg.SetTitle( L"オープン方法の選択" );
	dlg.SetMessage( msg );
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT1, L"ブラウザで開く" );
#ifndef WINCE
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, MZ3_APP_NAME L"でダウンロード" );
#else
	if( _tcsstr( url , L"maps.google.co.jp" ) == NULL ){
		dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, MZ3_APP_NAME L"でダウンロード" );
	} else {
		dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, L"Google Mapsで開く" );
	}
#endif
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

			// WindowsMobile 版のみ自動ログインURL変換を行う
/*
#ifdef WINCE
			if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
				// mixi モバイル用自動ログインURL変換
				requestUrl = util::ConvertToMixiMobileAutoLoginUrl( url );
			} else {
				// mixi 用自動ログインURL変換
				requestUrl = util::ConvertToMixiAutoLoginUrl( url );
			}
#endif
*/
			// ブラウザで開く
			util::OpenUrlByBrowser( requestUrl );
		}
		break;
	case CCommonSelectDlg::BUTTONCODE_SELECT2:
#ifdef WINCE
		if( _tcsstr( url , L"maps.google.co.jp" ) != NULL && _tcsstr( url , L"ll=" ) != NULL ) {
			// Google Mapsで開く
			static MyRegex reg;
			if( util::CompileRegex( reg, L"[&|\\?]ll=(\\-?[0-9\\.]*,\\-?[0-9\\.]*)" ) ) {
				if( reg.exec(url) && reg.results.size() == 2 ) {
					CString commandline = L"/Program Files/GoogleMaps/GoogleMaps.exe";
					CString commandparam =  L"-URL \"?action=locn";
					if( _tcsstr( url , L"t=h" ) ){
						commandparam += "&view=satv";
					} else {
						commandparam += "&view=mapv";
					}
					commandparam += "&a=@latlon:";
					commandparam += reg.results[1].str.c_str();
					commandparam += L"\"";

					// Google Mapsを起動
					util::OpenUrlByBrowser(commandline, commandparam);
					break;
				}
			}
		}
#endif
		// MZ3でダウンロード
		{
			theApp.m_access = true;
			m_abort = FALSE;

			// ダウンロードファイルパス
			theApp.m_inet.Initialize( m_hWnd, NULL );
			theApp.m_accessType = ACCESS_DOWNLOAD;

			MyUpdateControlStatus();

			theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
		}
		break;
	default:
		break;
	}
}

/**
 * アクセス終了通知(HTML)
 */
LRESULT CReportView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));
	util::MySetInformationText( m_hWnd, _T("解析中") );

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	theApp.m_access = false;

	// ログインページ以外であれば、最初にログアウトチェックを行っておく
	if (theApp.m_accessType != ACCESS_LOGIN && theApp.IsMixiLogout(theApp.m_accessType)) {
		// ログアウト状態になっている
		MZ3LOGGER_INFO(_T("再度ログインしてからデータを取得します。"));
		util::MySetInformationText( m_hWnd, L"再度ログインしてからデータを取得します" );

		// mixi データを保存（待避）
		theApp.m_mixiBeforeRelogin = m_data;
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);

		// ログイン実行
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);

		return TRUE;
	}

	bool bRetry = false;
	switch( theApp.m_accessType ) {
#ifdef BT_MZ3
	case ACCESS_IMAGE:
	case ACCESS_MOVIE:
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			//イメージのURLを取得
			CString url;
			switch( theApp.m_accessType ) {
			case ACCESS_IMAGE:		url = mixi::ShowPictureParser::GetImageURL( html );	break;
			case ACCESS_MOVIE:		url = theApp.m_inet.GetURL();							break;
			default:
				break;
			}

			util::MySetInformationText( m_hWnd, _T("完了") );

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
				dlg.SetTitle( L"オープン方法の選択" );
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

	case ACCESS_LOGIN:
		// ログインしたかどうかの確認
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );
			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ログイン成功
				if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) != 0) {
					MZ3LOGGER_DEBUG( L"OwnerID 取得済み" );
				} else {
					MZ3LOGGER_INFO( L"OwnerIDが未取得なので、ログインし、取得する (2)" );

					((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
					theApp.m_accessType = ACCESS_MAIN;
					theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML );
					return TRUE;
				}
			} else {
				// ログイン失敗
				LPCTSTR msg = L"ログインに失敗しました";
				util::MySetInformationText( m_hWnd, msg );
				::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONERROR);

				::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
				return TRUE;
			}
		}
		break;

	case ACCESS_MAIN:
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// パース
			MZ3Data data;
			theApp.DoParseMixiHomeHtml(&data, &html);

			if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
				LPCTSTR msg = L"ログインに失敗しました(2)";
				util::MySetInformationText( m_hWnd, msg );

				MZ3LOGGER_ERROR( msg );

				::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
				return TRUE;
			}

			// データを待避データに戻す
			m_data = theApp.m_mixiBeforeRelogin;
			theApp.m_accessType = m_data.GetAccessType();
			theApp.m_inet.DoGet(util::CreateMixiUrl(m_data.GetURL()), L"", CInetAccess::FILE_HTML );
		}
		break;
#endif

	default:
		if( theApp.m_accessType == m_data.GetAccessType() ) {
			// リロード or ページ変更

			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// HTML 解析
			parser::MyDoParseMixiHtml( m_data.GetAccessType(), m_data, html );
			util::MySetInformationText( m_hWnd, _T("wait...") );

			theApp.m_pReportView->SetData( m_data );
			util::MySetInformationText( m_hWnd, L"完了" );

			// レポートビューに遷移
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
#ifdef BT_MZ3
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE);
#else
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
#endif
			theApp.ChangeView( theApp.m_pReportView );

			// ログファイルに保存
			if( theApp.m_optionMng.m_bSaveLog ) {
				// 保存ファイルパスの生成
				CString strLogfilePath = util::MakeLogfilePath( m_data );
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
		MyUpdateControlStatus();
	}

	return TRUE;
}

/**
 * アクセス終了通知受信(Binary)
 */
LRESULT CReportView::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, NULL);
		return TRUE;
	}

	theApp.m_access = false;

	MyUpdateControlStatus();

	// MZ3 API : イベントハンドラ関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(theApp.m_accessType));
	if (util::CallMZ3ScriptHookFunctions2("get_end_binary_report_view", &rvals, 
			util::MyLuaData(serializeKey),
			util::MyLuaData(theApp.m_inet.m_dwHttpStatus),
			util::MyLuaData(CStringA(theApp.m_inet.GetURL())),
			util::MyLuaData(CStringA(theApp.m_filepath.temphtml))
			))
			
	{
		// イベントハンドラ完了
		return TRUE;
	}


	// 保存ファイルにコピー
	// パス生成
	CString strFilepath;
	{
		CString url = theApp.m_inet.GetURL();

		switch( theApp.m_accessType ) {
#ifdef BT_MZ3
		case ACCESS_IMAGE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.imageFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.png" ) );
			break;

		case ACCESS_MOVIE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.flv" ) );
			break;
#endif

		default:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.dat" ) );
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
			return TRUE;
		}
	}

	// ファイルを開く
	util::OpenByShellExecute(strFilepath);

	return TRUE;
}

/**
 * アクセスエラー通知受信
 */
LRESULT CReportView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR smsg = L"エラーが発生しました";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"原因：%s", smsg, theApp.m_inet.GetErrorMessage() );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
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

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * 中断ボタン押下時の処理
 */
LRESULT CReportView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// 通信中でないならすぐに終了する
	if( !theApp.m_inet.IsConnecting() ) {
		return TRUE;
	}
	theApp.m_inet.Abort();
	m_abort = TRUE;

	LPCTSTR msg = _T("中断しました");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * アクセス情報通知
 */
LRESULT CReportView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

/**
 * イメージボタン押下時の処理
 */
void CReportView::OnImageButton()
{
	if (m_currentData==NULL) {
		return;
	}

	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();

	CMenu menu;
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
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * 書き込み開始（本文入力ビューの表示）
 */
void CReportView::OnWriteComment()
{
#ifdef BT_MZ3
	// 引用方法の確認
	quote::QuoteType quoteType = quote::QUOTETYPE_INVALID;
	int idx = m_list.GetSelectedItem();
	CMixiData* data = NULL;
	if( idx >= 0 ){
		data = (CMixiData*)m_list.GetItemData(idx);
	}

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

	// 書き込み種別の取得
	WRITEVIEW_TYPE writeViewType = ACCESS_INVALID;

	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData((int)ACCESS_INVALID));
	if (util::CallMZ3ScriptHookFunctions2("get_write_view_type_by_report_item_access_type", &rvals, 
			util::MyLuaData(&m_data))) {
		writeViewType = (WRITEVIEW_TYPE) rvals[0].m_number;
		MZ3LOGGER_DEBUG(util::FormatString(L"estimated write view type by lua : %d", writeViewType));
	} else {
		MZ3LOGGER_ERROR(L"Lua 側で処理がないので中止");
		MessageBox(L"サポートされていない操作です");
		return;
	}

	// 書き込みビューを表示
	CWriteView* pWriteView = theApp.m_pWriteView;
	pWriteView->StartWriteView( writeViewType, &m_data );

	// 引用する
	if( quoteType != quote::QUOTETYPE_INVALID && data != NULL ) {
		CString str = quote::MakeQuoteString( *data, quoteType );

		// らんらんビュータグを消去
		ViewFilter::RemoveRan2ViewTag( str );

		((CEdit*)pWriteView->GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(str);
	}
#endif
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
	if (theApp.m_Platforms.PocketPC) {
		OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	}

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );

	return TRUE;
}

/**
 * 他ビューからの復帰処理
 */
LRESULT CReportView::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	theApp.ChangeView(theApp.m_pReportView);

	// Write ビューが有効ならONに。
#ifdef BT_MZ3
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE);
#else
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, FALSE);
#endif

	theApp.EnableCommandBarButton(ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton(ID_OPEN_BROWSER, TRUE);

	return TRUE;
}

/**
 * 再読込（投稿後など）
 */
LRESULT CReportView::OnReload(WPARAM wParam, LPARAM lParam)
{
	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );
	theApp.m_accessType = m_data.GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data.GetURL()), _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * ブラウザで開く
 */
void CReportView::OnOpenBrowser()
{
	util::OpenUrlByBrowserWithConfirm( m_data.GetBrowseUri() );
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
	util::OpenUrlByBrowserWithConfirmForUser( url, strUserName );
}

/**
 * プロフィールページを開く
 */
void CReportView::OnOpenProfile()
{
	// 選択アイテムの取得
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URLの生成
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// 開く
	MyLoadMixiViewPage( CMixiData::Link( url, data->GetAuthor() ) );
}

/**
 * プロフィールページを開く（ログ）
 */
void CReportView::OnOpenProfileLog()
{
#ifdef BT_MZ3
	// 選択アイテムの取得
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URLの生成
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// 開く
	static CMixiData s_mixi;
	CMixiData dummy;
	s_mixi = dummy;
	s_mixi.SetAccessType(ACCESS_PROFILE);
	s_mixi.SetURL(url);
	s_mixi.SetBrowseUri(url);

	CString strLogfilePath = util::MakeLogfilePath( s_mixi );

	// ファイル存在確認
	if(! util::ExistFile( strLogfilePath ) ) {
		// FILE NOT FOUND.
		CString msg = L"ログファイルがありません : " + strLogfilePath;
		MZ3LOGGER_ERROR( msg );

		util::MySetInformationText( m_hWnd, msg );
		
		return;
	}

	// HTML の取得
	CHtmlArray html;
	html.Load( strLogfilePath );

	// HTML 解析
	parser::MyDoParseMixiHtml( s_mixi.GetAccessType(), s_mixi, html );
	util::MySetInformationText( m_hWnd, L"完了" );

	// URL 設定
	s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

	// 表示
	SetData( s_mixi );
#endif
}


/**
 * メッセージ送信
 */
void CReportView::OnSendMessage()
{
#ifdef BT_MZ3
	// 選択アイテムの取得
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URLの生成
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// mixi アイテムの生成
	static CMixiData s_mixi;
	CMixiData dummy;
	s_mixi = dummy;
	s_mixi.SetAccessType(ACCESS_PROFILE);
	s_mixi.SetURL(url);
	s_mixi.SetBrowseUri(url);

	// hack : OwnerIDがなければAuthorIDを送信先ユーザIDとする
	s_mixi.SetOwnerID(data->GetOwnerID()>0 ? data->GetOwnerID() : data->GetAuthorID());

	// 書き込み画面生成
	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWMESSAGE, &s_mixi );
#endif
}

/**
 * メニュー生成
 */
void CReportView::MyPopupReportMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType()));
	if (util::CallMZ3ScriptHookFunctions2("popup_report_menu", &rvals, 
			util::MyLuaData(serializeKey), 
			util::MyLuaData(&m_data), 
			util::MyLuaData(m_list.GetSelectedItem()),
			util::MyLuaData(this)))
	{
		return;
	}

	// TODO 下記のビルトインコードをLuaスクリプト化すること
	CMenu menu;
	menu.LoadMenu(IDR_REPORT_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// 「進む」削除
	int idxPage = 7;		// 「ページ」メニュー（次へが有効の場合は-1となる）
	int idxDiarySeparator = 5;
#ifdef BT_MZ3
	if( theApp.m_pWriteView->IsWriteCompleted() ) {
#else
	if( false ) {
#endif
		pcThisMenu->RemoveMenu(ID_NEXT_MENU, MF_BYCOMMAND);
		idxPage --;
		idxDiarySeparator--;
	}

	// 「書き込み」に関する処理
	switch( m_data.GetAccessType() ) {
#ifdef BT_MZ3
	case ACCESS_MESSAGE:
		// メッセージなら、送信箱か受信箱かによって処理が異なる
		if( m_data.GetURL().Find( L"&box=outbox" ) != -1 ) {
			// 送信箱なので、書き込み無効
			pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		}else{
			// 受信箱なので、「メッセージ返信」に変更
			pcThisMenu->ModifyMenu( ID_WRITE_COMMENT,
				MF_BYCOMMAND,
				ID_WRITE_COMMENT,
				_T("メッセージ返信"));
		}
		break;

	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_MYDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
		// 日記、トピ等であれば有効
		break;

	case ACCESS_NEWS:
#endif
	default:
		// ニュース等なら書き込み無効
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		idxPage --;
		idxDiarySeparator --;
		break;

	}

	// 外部ブログなら、書き込み無効
//	if (m_data.IsOtherDiary() != FALSE) {
//		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
//	}

	// 前の日記、次の日記の処理
	switch( m_data.GetAccessType() ) {
#ifdef BT_MZ3
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_MYDIARY:
		if( m_data.GetPrevDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(ID_MENU_PREV_DIARY, MF_BYCOMMAND);
			idxPage--;
			idxDiarySeparator--;
		}
		if( m_data.GetNextDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(ID_MENU_NEXT_DIARY, MF_BYCOMMAND);
			idxPage--;
			idxDiarySeparator--;
		}
		if( m_data.GetPrevDiary().IsEmpty() &&  m_data.GetNextDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(idxDiarySeparator, MF_BYPOSITION);
			idxPage--;
		}
		// 「全てを表示」メニューの処理
		{
			CString fulldiarylink = m_data.GetFullDiary();
			if( fulldiarylink.IsEmpty() ){
				pcThisMenu->RemoveMenu(IDM_LOAD_FULL_DIARY, MF_BYCOMMAND);
			} else {
				// リンク文字列をメニューに設定する
				std::vector<CMixiData::Link> list_;
				mixi::ParserUtil::ExtractURI( fulldiarylink , list_ );

				pcThisMenu->ModifyMenu( IDM_LOAD_FULL_DIARY ,  MF_BYCOMMAND | MF_STRING , IDM_LOAD_FULL_DIARY , list_[0].text );
			}
		}
		break;
#endif

	default:
		// 前の日記へ
		pcThisMenu->RemoveMenu(ID_MENU_PREV_DIARY, MF_BYCOMMAND);
		idxPage--;
		idxDiarySeparator--;

		// 次の日記へ
		pcThisMenu->RemoveMenu(ID_MENU_NEXT_DIARY, MF_BYCOMMAND);
		idxPage--;
		idxDiarySeparator--;

		// 次の日記へ、の後のセパレータ
		pcThisMenu->RemoveMenu(idxDiarySeparator, MF_BYPOSITION);
		idxPage--;

		// ページ｜全てを表示
		pcThisMenu->RemoveMenu(IDM_LOAD_FULL_DIARY, MF_BYCOMMAND);
	}

	// 「ページ」および「URLをコピー」の追加

	// URLコピーサブメニュー
	CMenu* pcEditSubItem = NULL;
	CMenu cEditCopyURLSubItem;

	cEditCopyURLSubItem.CreatePopupMenu();

	bool bSubLinkAppended = false;

	// ページリンクがあれば追加。
	if( !m_data.m_linkPage.empty() ) {
		// リンクがあるので追加。
		CMenu* pSubMenu = pcThisMenu->GetSubMenu( idxPage );
		if( pSubMenu != NULL ) {
			// 追加
			for (int i=0; i<(int)m_data.m_linkPage.size(); i++) {
				const CMixiData::Link& link = m_data.m_linkPage[i];
				pSubMenu->AppendMenu(MF_STRING, ID_REPORT_PAGE_LINK_BASE+i+1, link.text);
			}
		}
	}

	// 画像
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			// セパレータを追加
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				CString imageName;
				imageName.Format(_T("画像%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
				// サブメニューに画像リンクコピーメニューを追加
				cEditCopyURLSubItem.AppendMenu(MF_STRING, ID_REPORT_COPY_IMAGE+i+1, imageName);
				bSubLinkAppended = true;
			}
		}
	}

	// 動画
	if (m_currentData != NULL) {
		if (m_currentData->GetMovieCount() > 0) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_MOVIE, _T("-"));
			if( bSubLinkAppended ){
				// サブメニューにセパレータを追加
				cEditCopyURLSubItem.AppendMenu(MF_SEPARATOR, ID_REPORT_COPY_MOVIE, _T("-"));
			}
			for (int i=0; i<m_currentData->GetMovieCount(); i++) {
				CString MovieName;
				MovieName.Format(_T("動画%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_MOVIE+i+1, MovieName);
				// サブメニューに動画リンクコピーメニューを追加
				cEditCopyURLSubItem.AppendMenu(MF_STRING, ID_REPORT_COPY_MOVIE+i+1, MovieName);
				bSubLinkAppended = true;
			}
		}
	}

	// リンク
	if( m_currentData != NULL ) {
		int n = (int)m_currentData->m_linkList.size();
		if( n > 0 ) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
			if( bSubLinkAppended ){
				// サブメニューにセパレータを追加
				cEditCopyURLSubItem.AppendMenu(MF_SEPARATOR, ID_REPORT_COPY_URL_BASE, _T("-"));
			}
			for( int i=0; i<n; i++ ) {
				// 追加
				CString s;
				s.Format( L"link : %s", m_currentData->m_linkList[i].text );
				pcThisMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
				// サブメニューにリンクコピーメニューを追加
				cEditCopyURLSubItem.AppendMenu( MF_STRING, ID_REPORT_COPY_URL_BASE+(i+1), s);
				bSubLinkAppended = true;
			}
		}
	}

	// リンクサブメニューをメニューに追加
	if( bSubLinkAppended ){
		// 「編集」-「コピー」サブメニューを探す
		for( UINT idxMenu = 0 ; idxMenu < pcThisMenu->GetMenuItemCount() ; idxMenu++ ){
			pcEditSubItem = pcThisMenu->GetSubMenu( idxMenu );
			if( pcEditSubItem ) {
				if( pcEditSubItem->GetMenuItemID( 0 ) == ID_EDIT_COPY ) {
					break;
				}
			}
			pcEditSubItem = NULL;
		}
		// 「コピー」の下に「URLをコピー」サブメニューを追加する
		if( pcEditSubItem ) {
			pcEditSubItem->AppendMenu( MF_SEPARATOR, ID_REPORT_COPY_IMAGE, _T("-"));
			pcEditSubItem->AppendMenu( MF_POPUP , (UINT_PTR)cEditCopyURLSubItem.m_hMenu , _T("URLをコピー"));
		}
	}

	// ブラウザで開く(このページ)：URLがなければ無効
	if (m_currentData->GetURL().IsEmpty()) {
		pcThisMenu->RemoveMenu(ID_OPEN_BROWSER, MF_BYCOMMAND);
	}

	// ブラウザで開く(ユーザページ), 
	// プロフィールページを開く, メッセージを送信する：IDがなければ無効
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*m_currentData);
	if (nUserId<=0) {
		pcThisMenu->RemoveMenu(ID_OPEN_BROWSER_USER, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_OPEN_PROFILE, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_OPEN_PROFILE_LOG, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_SEND_MESSAGE, MF_BYCOMMAND);
	} else {
		// プロフィールページを開く（ログ）はログがなければ無効化
#ifdef BT_MZ3
		CMixiData mixi;
		mixi.SetAccessType( ACCESS_PROFILE );
		mixi.SetURL( util::FormatString( L"http://mixi.jp/show_friend.pl?id=%d", nUserId ) );
		CString path = util::MakeLogfilePath( mixi );
		if (!util::ExistFile(path)) {
			pcThisMenu->EnableMenuItem( ID_OPEN_PROFILE_LOG, MF_GRAYED | MF_BYCOMMAND );
		}
#endif
	}

	// メニューのポップアップ
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);	  
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
		break;
	}

	return CFormView::WindowProc(message, wParam, lParam);
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

/**
 * リストの右クリックイベント
 */
void CReportView::OnNMRclickReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MyPopupReportMenu();

	*pResult = 0;
}


void CReportView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString logStr;
	int newPos = pScrollBar->GetScrollPos();
	int pageOffset = (m_detailView->GetViewLineMax()-1);

	// スクルールバーの位置が行数を越えるなら無条件で処理を中断
	if( newPos > m_scrollBarHeight ){
		return;
	}

	switch( nSBCode ){

		case SB_THUMBTRACK:	// ノブによる移動は禁止
			if( (int)nPos < m_scrollBarHeight )
				newPos = nPos;
			break;

		case SB_PAGEUP:
			if( newPos - pageOffset >= 0 )
				newPos -= pageOffset;
			else
				newPos = 0;
			break;

		case SB_PAGEDOWN:
			if( newPos + pageOffset <= m_scrollBarHeight )
				newPos += pageOffset;
			else
				newPos = m_scrollBarHeight;
			break;

		case SB_LINEUP:
			if( newPos - 1 >= 0 )
				newPos -= 1;
			break;

		case SB_LINEDOWN:
			if( newPos + 1 <= m_scrollBarHeight )
				newPos += 1;
			break;
	}

	// 終了通知以外は位置を再設定する。
	if( nSBCode != SB_ENDSCROLL ){ 
		// 暫定でLINEUP/DOWN,PAGEUP/DOWNのみだけ透過
		if( nSBCode == SB_LINEUP || nSBCode == SB_LINEDOWN ||
			nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN ||
			nSBCode == SB_THUMBTRACK ){
/*			logStr.Format(TEXT("バー位置:%d/%d\r\n"),newPos,scrollBarHeight);
			OutputDebugString(logStr);
*/
			pScrollBar->SetScrollPos(newPos);
			m_detailView->ResetDragOffset();
			m_detailView->DrawDetail(newPos);
		}
	}else if( nSBCode == SB_ENDSCROLL ){
	}
}


void CReportView::OnDestroy()
{
	CFormView::OnDestroy();

	m_detailView->DestroyWindow();
}

void CReportView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// スクロール確定。
	// スクロールバーの位置を変更。
	m_vScrollbar.SetScrollPos( m_detailView->MyGetScrollPos() );

	CFormView::OnLButtonUp(nFlags, point);
}

void CReportView::OnMouseMove(UINT nFlags, CPoint point)
{
	// スクロールバーの位置を変更。
	m_vScrollbar.SetScrollPos( m_detailView->MyGetScrollPos() );

	CFormView::OnMouseMove(nFlags, point);
}

void CReportView::OnAcceleratorScrollOrNextComment()
{
	// スクロール位置判定（スクロール余地があればスクロールする）
	if (CommandScrollDownEdit()) {
		return;
	}

	// 次の項目に移動
	CommandMoveDownList();
}

void CReportView::OnAcceleratorNextComment()
{
	// 次の項目に移動
	CommandMoveDownList();
}

void CReportView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_detailView == NULL) {
		return;
	}

	// 上下N%以内でのダブルクリックであれば項目変更
	CRect rect;
	m_detailView->GetClientRect(&rect);
	int y = point.y - rect.top;
	double y_pos_in_percent = y / (double)rect.Height() * 100.0;

#define N_DOUBLE_CLICK_MOVE_ITEM_LIMIT	40.0
	if (y_pos_in_percent < N_DOUBLE_CLICK_MOVE_ITEM_LIMIT) {
		// 前の項目に移動
		CommandMoveUpList();
	} else if (y_pos_in_percent > 100.0-N_DOUBLE_CLICK_MOVE_ITEM_LIMIT) {
		// 次の項目に移動
		CommandMoveDownList();
	}
}

void CReportView::OnRButtonUp(UINT nFlags, CPoint point)
{
	MyPopupReportMenu();

//	CFormView::OnRButtonUp(nFlags, point);
}

afx_msg void CReportView::OnEditCopy()
{
	if (m_currentData==NULL) {
		return;
	}

	CString str;

	str += m_currentData->GetAuthor();
	str += _T("　");
	str += m_currentData->GetDate();
	str += _T("\r\n");

	str += m_currentData->GetBody();

	// らんらんビュータグを消去
	ViewFilter::RemoveRan2ViewTag( str );

	util::SetClipboardDataTextW( str );
}

BOOL CReportView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 高解像度ホイール対応のため、delta 値を累積する。
	static int s_delta = 0;
	// 方向が逆になったらキャンセル
	if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
		s_delta = 0;
	}
	s_delta += zDelta;

	if (s_delta>WHEEL_DELTA) {
		CommandScrollUpEdit();
		s_delta -= WHEEL_DELTA;
	} else if (s_delta<-WHEEL_DELTA) {
		CommandScrollDownEdit();
		s_delta += WHEEL_DELTA;
	}
	return TRUE;

//	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * 次のコメントへ移動
 */
LRESULT CReportView::OnMoveDownList(WPARAM dwLoaded, LPARAM dwLength)
{
	return CommandMoveDownList();
}

/**
 * 前のコメントへ移動
 */
LRESULT CReportView::OnMoveUpList(WPARAM dwLoaded, LPARAM dwLength)
{
	return CommandMoveUpList();
}

/**
 * コメントリストのアイテム数を取得
 */
LRESULT CReportView::OnGetListItemCount(WPARAM dwLoaded, LPARAM dwLength)
{
	return (LRESULT)m_list.GetItemCount();
}
/**
 * リンクのURLをクリップボードにコピー
 */
void CReportView::OnCopyClipboardUrl(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_URL_BASE+1);
	if( idx > m_currentData->m_linkList.size() ) {
		return;
	}

	CMixiData::Link link = m_currentData->m_linkList[idx];
	CString url = link.url;

#ifdef BT_MZ3
	ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
	switch (estimatedAccessType) {
	case ACCESS_MYDIARY:
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
	case ACCESS_EVENT_MEMBER:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		url = util::CreateMixiUrl(link.url);
		break;
	}
#endif

	util::SetClipboardDataTextW( url );
}

/**
 * 画像のURLをクリップボードにコピー
 */
void CReportView::OnCopyClipboardImage(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_IMAGE+1);
	if( (int)idx > m_currentData->GetImageCount() ) {
		return;
	}

	util::SetClipboardDataTextW( m_currentData->GetImage(idx) );
}

/**
 * 動画のURLをクリップボードにコピー
 */
void CReportView::OnCopyClipboardMovie(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_MOVIE+1);
	if( (int)idx > m_currentData->GetMovieCount() ) {
		return;
	}

	util::SetClipboardDataTextW( m_currentData->GetMovie(idx) );
}

/**
 * 「次の日記へ>>」メニュー処理
 */
void CReportView::OnMenuNextDiary()
{
	//MZ3_TRACE(L"CReportView::OnMenuNextDiary()\n");

	CString link = m_data.GetNextDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi 内リンクのはずなのでロードする。
			if ( MyLoadMixiViewPage( list_[0] )) {
				// 横スクロールアニメーションを起動する
				m_list.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_LEFT );
				return;
			} else {
				// mixi内リンクでなければエラーが表示されているので隠す
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}

	}
}

/**
 * 「<<前の日記へ」メニュー処理
 */
void CReportView::OnMenuPrevDiary()
{
	//MZ3_TRACE(L"CReportView::OnMenuPrevDiary()\n");

	CString link = m_data.GetPrevDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi 内リンクのはずなのでロードする。
			if ( MyLoadMixiViewPage( list_[0] )) {
				// 横スクロールアニメーションを起動する
				m_list.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_RIGHT );
				return;
			} else {
				// mixi内リンクでなければエラーが表示されているので隠す
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}
	}
}

/**
 * 「次の日記へ>>」メニュー活性化処理
 */
void CReportView::OnUpdateMenuNextDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetNextDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

/**
 * 「<<前の日記へ」メニュー活性化処理
 */
void CReportView::OnUpdateMenuPrevDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetPrevDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

/**
 * 「全てを表示」メニュー処理
 */
void CReportView::OnLoadFullDiary()
{
	//MZ3_TRACE(L"CReportView::OnLoadFullDiary()\n");

	CString link = m_data.GetFullDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi 内リンクのはずなのでロードする。
			if ( MyLoadMixiViewPage( list_[0] )) {
				return;
			} else {
				// mixi内リンクでなければエラーが表示されているので隠す
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}
	}
}

/**
 * 「全てを表示」メニュー活性化処理
 */
void CReportView::OnUpdateLoadFullDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetFullDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

void CReportView::MyUpdateControlStatus(void)
{
	if (theApp.m_access) {
		MZ3LOGGER_DEBUG(L"****** CReportView::MyUpdateControlStatus(1)");
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

		// プログレスバーを表示
		mc_progressBar.ShowWindow( SW_SHOW );

		m_infoEdit.ShowWindow(SW_SHOW);

	} else {
		MZ3LOGGER_DEBUG(L"****** CReportView::MyUpdateControlStatus(0)");
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

		// プログレスバーを非表示
		mc_progressBar.ShowWindow( SW_HIDE );

		m_infoEdit.ShowWindow(SW_HIDE);
	}
}

/**
 * MZ3 API で登録されたメニューのイベント
 */
void CReportView::OnLuaMenu(UINT nID)
{
	UINT idx = nID - ID_LUA_MENU_BASE;
	if (idx >= theApp.m_luaMenus.size()) {
		MZ3LOGGER_ERROR(util::FormatString(L"不正なメニューIDです [%d]", nID));
		return;
	}

	// Lua関数名取得＆呼び出し
	const std::string& strFuncName = theApp.m_luaMenus[idx];
	util::CallMZ3ScriptHookFunction("", "select_menu", strFuncName.c_str(), &m_data);
}
