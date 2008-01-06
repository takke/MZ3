/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/// MZ3View.cpp : CMZ3View クラスの実装

#include "stdafx.h"
#include "version.h"
#include "MZ3.h"

#include "MZ3Doc.h"
#include "MZ3View.h"

#include "MixiData.h"
#include "CategoryItem.h"
#include "HtmlArray.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "util.h"
#include "util_gui.h"
#include "MixiParser.h"
#include "ChooseAccessTypeDlg.h"
#include "OpenUrlDlg.h"
#include "CommonEditDlg.h"
#include "MiniImageDialog.h"
#include "url_encoder.h"
#include "twitter_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID_INTERVAL_CHECK	101

inline CString MyGetItemByBodyColType( CMixiData* data, CCategoryItem::BODY_INDICATE_TYPE bodyColType, bool bLimitForList=true )
{
	CString item;

	switch( bodyColType ) {
	case CCategoryItem::BODY_INDICATE_TYPE_DATE:
		item = data->GetDate();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_NAME:
		item = data->GetName();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_TITLE:
		item = data->GetTitle();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_BODY:
		// 本文を1行に変換して割り当て。
		for( u_int i=0; i<data->GetBodySize(); i++ ) {
			CString line = data->GetBody(i);
			while( line.Replace( L"\r\n", L"" ) );
			item.Append( line );
		}
		break;
	default:
		return L"";
	}

	// 上限設定
	if (bLimitForList) {
#ifdef WINCE
		// WindowsMobile の場合は、30文字くらいで切らないと落ちるので制限する。
		return item.Left( 30 );
#else
		// Windows の場合は、とりあえず100文字で切っておく。
		return item.Left( 100 );
#endif
	} else {
		return item;
	}
}

/// アクセス種別と表示種別から、ボディーリストのヘッダー文字列（２カラム目）を取得する
LPCTSTR MyGetBodyHeaderColName2( const CMixiData& mixi, CCategoryItem::BODY_INDICATE_TYPE bodyIndicateType )
{
	ACCESS_TYPE accessType = mixi.GetAccessType();

	switch (accessType) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("名前>>");
		default:
			return _T("日時>>");
		}
		break;
	case ACCESS_LIST_NEWS:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("配給元>>");
		default:
			return _T("配信時刻>>");
		}
		break;
	case ACCESS_LIST_FAVORITE:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_DATE:
			// お気に入りの種別（ユーザーorコミュニティ）に応じて変更
			if( mixi.GetURL().Find( L"kind=community" ) != -1 ) {
				return _T("人数>>");
			}else{
				return _T("最終ログイン>>");
			}
		default:
			// お気に入りの種別（ユーザーorコミュニティ）に応じて変更
			if( mixi.GetURL().Find( L"kind=community" ) != -1 ) {
				return _T("説明>>");
			}else{
				return _T("自己紹介>>");
			}
		}
		break;
	case ACCESS_LIST_FRIEND:
		return _T("ログイン時刻");
	case ACCESS_LIST_COMMUNITY:
		return _T("人数");
	case ACCESS_LIST_INTRO:
		return L"紹介文";
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_NEW_BBS_COMMENT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("コミュニティ>>");
		default:
			return _T("日時>>");
		}
		break;
	case ACCESS_LIST_CALENDAR:
	case ACCESS_LIST_BBS:
		return L"日付";
	case ACCESS_LIST_BOOKMARK:
		return _T("コミュニティ");
	case ACCESS_LIST_MYDIARY:
		return _T("日時");
	case ACCESS_LIST_MESSAGE_IN:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("差出人>>");
		default:
			return _T("日付>>");
		}
	case ACCESS_LIST_MESSAGE_OUT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("宛先>>");
		default:
			return _T("日付>>");
		}

	case ACCESS_LIST_FOOTSTEP:
		return _T("時刻");

	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("名前>>");
		default:
			return _T("日付>>");
		}

	default:
		return L"";
	}
}


// CMZ3View

IMPLEMENT_DYNCREATE(CMZ3View, CFormView)

BEGIN_MESSAGE_MAP(CMZ3View, CFormView)
	ON_WM_SIZE()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_HEADER_LIST, &CMZ3View::OnNMClickCategoryList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HEADER_LIST, &CMZ3View::OnLvnItemchangedCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_HEADER_LIST, &CMZ3View::OnNMDblclkCategoryList)
	ON_NOTIFY(NM_SETFOCUS, IDC_HEADER_LIST, &CMZ3View::OnNMSetfocusHeaderList)
	ON_NOTIFY(NM_RCLICK, IDC_HEADER_LIST, &CMZ3View::OnNMRclickHeaderList)

	ON_NOTIFY(NM_DBLCLK, IDC_BODY_LIST, &CMZ3View::OnNMDblclkBodyList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BODY_LIST, &CMZ3View::OnLvnItemchangedBodyList)
	ON_NOTIFY(NM_SETFOCUS, IDC_BODY_LIST, &CMZ3View::OnNMSetfocusBodyList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CMZ3View::OnHdnItemclickBodyList)
	ON_NOTIFY(NM_RCLICK, IDC_BODY_LIST, &CMZ3View::OnNMRclickBodyList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GROUP_TAB, &CMZ3View::OnTcnSelchangeGroupTab)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CMZ3View::OnHdnEndtrackHeaderList)
	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
	ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
	ON_MESSAGE(WM_MZ3_HIDE_VIEW, OnHideView)
	ON_MESSAGE(WM_MZ3_POST_END, OnPostEnd)
	ON_COMMAND(ID_WRITE_DIARY, &CMZ3View::OnWriteDiary)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_COMMAND(ID_OPEN_BROWSER, &CMZ3View::OnOpenBrowser)
	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CMZ3View::OnShowDebugInfo)
	ON_COMMAND(ID_GET_ALL, &CMZ3View::OnGetAll)
	ON_COMMAND(ID_GET_LAST10, &CMZ3View::OnGetLast10)
	ON_COMMAND(ID_VIEW_LOG, &CMZ3View::OnViewLog)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CMZ3View::OnOpenBrowserUser)
	ON_COMMAND(ID_OPEN_INTRO, &CMZ3View::OnOpenIntro)
	ON_COMMAND(ID_OPEN_SELFINTRO, &CMZ3View::OnOpenSelfintro)
	ON_COMMAND(IDM_SET_NO_READ, &CMZ3View::OnSetNoRead)
	ON_COMMAND(IDM_VIEW_BBS_LIST, &CMZ3View::OnViewBbsList)
	ON_COMMAND(IDM_VIEW_BBS_LIST_LOG, &CMZ3View::OnViewBbsListLog)
	ON_COMMAND(IDM_CRUISE, &CMZ3View::OnCruise)
	ON_COMMAND(IDM_CHECK_CRUISE, &CMZ3View::OnCheckCruise)
	ON_COMMAND(ID_SEND_NEW_MESSAGE, &CMZ3View::OnSendNewMessage)
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_NARROW, &CMZ3View::OnLayoutCategoryMakeNarrow)
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_WIDE, &CMZ3View::OnLayoutCategoryMakeWide)
	ON_EN_SETFOCUS(IDC_INFO_EDIT, &CMZ3View::OnEnSetfocusInfoEdit)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
	ON_NOTIFY(NM_CLICK, IDC_GROUP_TAB, &CMZ3View::OnNMClickGroupTab)
	ON_NOTIFY(NM_RCLICK, IDC_GROUP_TAB, &CMZ3View::OnNMRclickGroupTab)
	ON_COMMAND(ID_ACCELERATOR_FONT_MAGNIFY, &CMZ3View::OnAcceleratorFontMagnify)
	ON_COMMAND(ID_ACCELERATOR_FONT_SHRINK, &CMZ3View::OnAcceleratorFontShrink)
	ON_COMMAND(ID_ACCELERATOR_CONTEXT_MENU, &CMZ3View::OnAcceleratorContextMenu)
	ON_COMMAND(ID_ACCELERATOR_NEXT_TAB, &CMZ3View::OnAcceleratorNextTab)
	ON_COMMAND(ID_ACCELERATOR_PREV_TAB, &CMZ3View::OnAcceleratorPrevTab)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_SET_READ, &CMZ3View::OnSetRead)
	ON_COMMAND(ID_ACCELERATOR_RELOAD, &CMZ3View::OnAcceleratorReload)
	ON_COMMAND(ID_MENU_TWITTER_READ, &CMZ3View::OnMenuTwitterRead)
	ON_COMMAND(ID_MENU_TWITTER_REPLY, &CMZ3View::OnMenuTwitterReply)
	ON_COMMAND(ID_MENU_TWITTER_UPDATE, &CMZ3View::OnMenuTwitterUpdate)
	ON_COMMAND(ID_MENU_TWITTER_HOME, &CMZ3View::OnMenuTwitterHome)
	ON_COMMAND(ID_MENU_TWITTER_FAVORITES, &CMZ3View::OnMenuTwitterFavorites)
	ON_COMMAND(ID_MENU_TWITTER_SITE, &CMZ3View::OnMenuTwitterSite)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, &CMZ3View::OnBnClickedUpdateButton)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
	ON_WM_PAINT()
	ON_COMMAND(ID_MENU_TWITTER_FRIEND_TIMELINE, &CMZ3View::OnMenuTwitterFriendTimeline)
	ON_COMMAND(ID_MENU_TWITTER_FRIEND_TIMELINE_WITH_OTHERS, &CMZ3View::OnMenuTwitterFriendTimelineWithOthers)
	ON_COMMAND(ID_TABMENU_DELETE, &CMZ3View::OnTabmenuDelete)
	ON_COMMAND_RANGE(ID_APPEND_MENU_BEGIN, ID_APPEND_MENU_END, &CMZ3View::OnAppendCategoryMenu)
	ON_COMMAND(ID_REMOVE_CATEGORY_ITEM, &CMZ3View::OnRemoveCategoryItem)
	ON_COMMAND(ID_EDIT_CATEGORY_ITEM, &CMZ3View::OnEditCategoryItem)
	ON_COMMAND(ID_TABMENU_EDIT, &CMZ3View::OnTabmenuEdit)
	ON_COMMAND(ID_TABMENU_ADD, &CMZ3View::OnTabmenuAdd)
END_MESSAGE_MAP()

// CMZ3View コンストラクション/デストラクション

/**
 * コンストラクタ
 */
CMZ3View::CMZ3View()
	: CFormView(CMZ3View::IDD)
	, m_dwLastReturn( 0 )
	, m_nKeydownRepeatCount( 0 )
	, m_checkNewComment( false )
	, m_viewStyle(VIEW_STYLE_DEFAULT)
	, m_rectIcon(0,0,0,0)
{
	m_preCategory = 0;
	m_selGroup = NULL;
	m_pMiniImageDlg = NULL;

	m_hotList = NULL;

	m_nochange = FALSE;
	m_abort = FALSE;
}

/**
 * デストラクタ
 */
CMZ3View::~CMZ3View()
{
}

void CMZ3View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_TAB, m_groupTab);
	DDX_Control(pDX, IDC_HEADER_LIST, m_categoryList);
	DDX_Control(pDX, IDC_BODY_LIST, m_bodyList);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_STATUS_EDIT, m_statusEdit);
}

BOOL CMZ3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CFormView::PreCreateWindow(cs);
}

/**
 * 初期化処理
 */
void CMZ3View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	TRACE(_T("Rect = (%d, %d) - (%d, %d)\n"), rect.left, rect.top, rect.right, rect.bottom);
	OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));

	// インターネット回線を開く
//	theApp.m_inet.Open();

	m_access = FALSE;

	// プログレスバー初期化
	mc_progressBar.SetRange( 0, 1000 );

	// フォントの作成
	theApp.MakeNewFont( GetFont(), theApp.m_optionMng.GetFontHeight(), theApp.m_optionMng.GetFontFace() );

	// --------------------------------------------------
	// グループタブの設定
	// --------------------------------------------------
	{
		// フォント変更
		m_groupTab.SetFont( &theApp.m_font );
	}

	// --------------------------------------------------
	// カテゴリリストの設定
	// --------------------------------------------------
	{
		// フォント変更
		m_categoryList.SetFont( &theApp.m_font );

		// スタイル変更
		DWORD dwStyle = m_categoryList.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		// デフォルトをレポート表示でオーナー固定描画にする
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		// スタイルの更新
		m_categoryList.ModifyStyle(0, dwStyle);

		// 一行選択モードの設定
		ListView_SetExtendedListViewStyle((HWND)m_categoryList.m_hWnd, LVS_EX_FULLROWSELECT);

		// カラム作成
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_categoryList.InsertColumn(0, _T(""), LVCFMT_LEFT, 125*2, -1);
			m_categoryList.InsertColumn(1, _T(""), LVCFMT_LEFT, 100*2, -1);
			break;
		case SR_QVGA:
		default:
			m_categoryList.InsertColumn(0, _T(""), LVCFMT_LEFT, 125, -1);
			m_categoryList.InsertColumn(1, _T(""), LVCFMT_LEFT, 100, -1);
			break;
		}
	}

	// --------------------------------------------------
	// ボディリストの設定
	// --------------------------------------------------
	{
		// フォント変更
		m_bodyList.SetFont( &theApp.m_font );

		// グリッドライン表示
		m_bodyList.SetExtendedStyle(m_bodyList.GetExtendedStyle() | LVS_EX_GRIDLINES);

		// 一行選択モードの設定
		ListView_SetExtendedListViewStyle((HWND)m_bodyList.m_hWnd, LVS_EX_FULLROWSELECT);

		// スタイル変更
		DWORD dwStyle = m_bodyList.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		// デフォルトをレポート表示でオーナー固定描画にする
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		// スタイルの更新
		m_bodyList.ModifyStyle(0, dwStyle);

		// アイコンリストの作成
		m_iconImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_bodyList.SetImageList(&m_iconImageList, LVSIL_SMALL);

		// カラム作成
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_bodyList.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 120*2, -1);
			m_bodyList.InsertColumn(1, _T("名前"), LVCFMT_LEFT, 105*2, -1);
			break;
		case SR_QVGA:
		default:
			m_bodyList.InsertColumn(0, _T("タイトル"), LVCFMT_LEFT, 120, -1);
			m_bodyList.InsertColumn(1, _T("名前"), LVCFMT_LEFT, 105, -1);
			break;
		}
	}

	// リストカラム幅の変更
	ResetColumnWidth();

	// インフォメーションエディットの設定
	{
		// フォント変更
		m_infoEdit.SetFont( &theApp.m_font );
	}

	// ボタン制御
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_categoryList.SetFocus();

	// 子画面
#ifndef WINCE
	m_pMiniImageDlg = new CMiniImageDialog( this );
	m_pMiniImageDlg->ShowWindow( SW_HIDE );

	// 半透明処理
	::SetWindowLong( m_pMiniImageDlg->m_hWnd, GWL_EXSTYLE, GetWindowLong(m_pMiniImageDlg->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	
	typedef BOOL (WINAPI *PROCSETLAYEREDWINDOW)(HWND, COLORREF, BYTE, DWORD);
	PROCSETLAYEREDWINDOW pProcSetLayeredWindowAttributes;
	pProcSetLayeredWindowAttributes = 
		(PROCSETLAYEREDWINDOW)GetProcAddress(GetModuleHandleA("USER32.DLL"), "SetLayeredWindowAttributes");

	int n = 100-10;
	if (pProcSetLayeredWindowAttributes!=NULL) {
		pProcSetLayeredWindowAttributes(m_pMiniImageDlg->m_hWnd, 0, 255*n/100, LWA_ALPHA);
	}
#endif

	// 初期化スレッド開始
	AfxBeginThread( Initialize_Thread, this );

	// インターバルタイマー生成
	UINT result = SetTimer( TIMERID_INTERVAL_CHECK, 1000, NULL );
//	DWORD e = ::GetLastError();
}

/**
 * ボディリストのアイコンのインデックスを取得する
 */
inline int MyGetBodyListDefaultIconIndex( const CMixiData& mixi )
{
	int iconIndex = -1;
	switch (mixi.GetAccessType()) {
	case ACCESS_BBS:		iconIndex = 0;	break;
	case ACCESS_EVENT:		iconIndex = 1;	break;
	case ACCESS_ENQUETE:	iconIndex = 2;	break;
	default:				iconIndex = -1;	break;	// アイコンなし
	}
	return iconIndex;
}

/**
 * 初期化用スレッド
 */
unsigned int CMZ3View::Initialize_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	::Sleep( 10L );

	pView->DoInitialize();

	return 0;
}

/**
 * 遅延初期化メソッド（初期化用スレッドから起動される）
 */
bool CMZ3View::DoInitialize()
{
	// 初期データ設定
	InsertInitialData();

	m_hotList = &m_categoryList;

	// ログロード中に移動できないように、アクセスフラグを立てておく
	m_access = TRUE;

	// ログのロード
	MyLoadCategoryLogfile( *m_selGroup->getSelectedCategory() );

	// ボディリストに設定
	SetBodyList( m_selGroup->getSelectedCategory()->GetBodyList() );

	// アクセスフラグをおろす
	m_access = FALSE;

	// ブックマークのロード
	try{
		theApp.m_bookmarkMng.Load( theApp.m_root.GetBookmarkList() );
	}catch(...){
	}

	// 新着メッセージ確認
	if (theApp.m_optionMng.IsBootCheckMnC() != FALSE) {
		// 新着メッセージ確認
		DoNewCommentCheck();
	}

	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	return true;
}

/**
 * 初期データ設定
 */
void CMZ3View::InsertInitialData()
{
	// グループリストデータ構造を反映する。
	for( size_t i=0; i<theApp.m_root.groups.size(); i++ ) {
		CGroupItem& group = theApp.m_root.groups[i];

		// グループタブに追加する
		m_groupTab.InsertItem( i, group.name );
	}

	// 初期選択項目設定
	int tabIndex = 0;
	if (0 <= theApp.m_optionMng.m_lastTopPageTabIndex && theApp.m_optionMng.m_lastTopPageTabIndex < m_groupTab.GetItemCount()) {
		// 前回終了時のタブを復帰。
		tabIndex = theApp.m_optionMng.m_lastTopPageTabIndex;
	}
	m_groupTab.SetCurSel( tabIndex );

	// 選択中のグループ項目の設定
	m_selGroup = &theApp.m_root.groups[tabIndex];

	// カテゴリの選択状態を復帰
	int nCategory = m_selGroup->categories.size();
	if (0 <= theApp.m_optionMng.m_lastTopPageCategoryIndex && theApp.m_optionMng.m_lastTopPageCategoryIndex < nCategory) {
		m_selGroup->focusedCategory = m_selGroup->selectedCategory = theApp.m_optionMng.m_lastTopPageCategoryIndex;
	}

	// カテゴリーリストを初期化する
	MyUpdateCategoryListByGroupItem();
}

// CMZ3View 診断

#ifdef _DEBUG
void CMZ3View::AssertValid() const
{
	CFormView::AssertValid();
}

CMZ3Doc* CMZ3View::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMZ3Doc)));
	return (CMZ3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMZ3View メッセージ ハンドラ

/**
 * サイズ変更時の処理
 */
void CMZ3View::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	MZ3LOGGER_DEBUG( L"OnSize( " + util::int2str(nType) + L", " + util::int2str(cx) + L", " + util::int2str(cy) + L" )" );

	MySetLayout( cx, cy );
}

void CMZ3View::MySetLayout(int cx, int cy)
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
/*	{
		CRect rect;
		CString msg;

		GetWindowRect( &rect );
		msg.Format( L" wrect-cx,cy : %d, %d", rect.Width(), rect.Height() );
		MZ3LOGGER_DEBUG( msg );

		GetWindowRect( &rect );
		msg.Format( L" crect-cx,cy : %d, %d", rect.Width(), rect.Height() );
		MZ3LOGGER_DEBUG( msg );
	}
*/
	int fontHeight = theApp.m_optionMng.m_fontHeight;
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	// 画面下部の情報領域
	int hInfoBase = theApp.GetInfoRegionHeight(fontHeight);
	int hInfo     = hInfoBase;

	// グループタブ
	int hGroup    = theApp.GetTabHeight(fontHeight);

	// 投稿領域
	int hPost     = 0;
	CWnd* pStatusEdit   = GetDlgItem( IDC_STATUS_EDIT );
	CWnd* pUpdateButton = GetDlgItem( IDC_UPDATE_BUTTON );
	CRect rectUpdateButton;
	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( ES_MULTILINE, 0 );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_HIDE);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_HIDE);
		}
		break;
	case VIEW_STYLE_IMAGE:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( 0, ES_MULTILINE );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_HIDE);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_HIDE);
		}
#ifdef WINCE
		hInfo = (int)(hInfoBase * 1.8);
#else
		hInfo = (int)(hInfoBase * 1.5);
#endif
		break;
	case VIEW_STYLE_TWITTER:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( 0, ES_MULTILINE );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_SHOW);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_SHOW);
			pUpdateButton->GetClientRect(&rectUpdateButton);
		}
		hPost = rectUpdateButton.Height();
#ifdef WINCE
		hInfo = (int)(hInfoBase * 1.8);
#else
		hInfo = (int)(hInfoBase * 1.5);
#endif
		break;
	}

	// カテゴリ、ボディリストの領域を % で指定
	// （但し、カテゴリリストはグループタブを、ボディリストは情報領域を含む）
	const int h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	int hCategory = (cy * h1 / (h1+h2)) - (hGroup -1);
	int hBody     = (cy * h2 / (h1+h2)) - (hInfo + hPost -1);

	int y = 0;
	util::MoveDlgItemWindow( this, IDC_GROUP_TAB,   0, y, cx, hGroup    );
	y += hGroup;

	util::MoveDlgItemWindow( this, IDC_HEADER_LIST, 0, y, cx, hCategory );
	y += hCategory;

	util::MoveDlgItemWindow( this, IDC_BODY_LIST,   0, y, cx, hBody     );
	y += hBody;

	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		m_rectIcon.SetRect( 0, 0, 0, 0 );
		util::MoveDlgItemWindow( this, IDC_INFO_EDIT, 0, y, cx, hInfo     );
		y += hInfo;
		break;
	case VIEW_STYLE_IMAGE:
	case VIEW_STYLE_TWITTER:
		m_rectIcon.SetRect( 0, y, hInfo, y+hInfo );
		util::MoveDlgItemWindow( this, IDC_INFO_EDIT, hInfo, y, cx-hInfo, hInfo     );
		y += hInfo;
		break;
	}

	if (pUpdateButton!=NULL) {
		int w = rectUpdateButton.Width();
		if (hPost>0) {
			util::MoveDlgItemWindow( this, IDC_STATUS_EDIT,   0,    y, cx - w, hPost );
			util::MoveDlgItemWindow( this, IDC_UPDATE_BUTTON, cx-w, y, w,      hPost );
		}
	}

	// ラベルぬっ殺しモードの場合はスタイルを変更すっぺよ
	if( theApp.m_optionMng.m_killPaneLabel ) {
		util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);
	}
	MoveMiniImageDlg();

	// プログレスバーは別途配置
	// サイズは hInfoBase の 2/3 とする
	int hProgress = hInfoBase * 2 / 3;
	y = cy -hInfo -hPost -hProgress;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR, 0, y, cx, hProgress );

	// リストカラム幅の変更
	ResetColumnWidth();
}

/**
 * カテゴリリストクリック時の処理
 */
void CMZ3View::OnNMClickCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	if (m_access) {
		// アクセス中は中止
		return;
	}

	m_hotList = &m_categoryList;
	if (lpnmlv->iItem<0) {
		return;
	}

	// カテゴリリスト中の「現在選択されている項目」を更新
	m_categoryList.Update( m_selGroup->selectedCategory );
	int idx = (int)m_categoryList.GetItemData(lpnmlv->iItem);
	if( idx < 0 || idx >= (int)m_selGroup->categories.size() ) {
		idx = 0;
	}
	m_selGroup->selectedCategory = idx;

//	m_categoryList.Update( m_selGroup->getSelectedCategory()->selectedBody );

	OnMySelchangedCategoryList();

	*pResult = 0;
}

/**
 * カテゴリリストダブルクリック時の処理
 */
void CMZ3View::OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;
	m_hotList = &m_categoryList;

	// カレントデータを取得
	int idx = (int)m_categoryList.GetItemData(lpnmlv->iItem);
	m_selGroup->selectedCategory = idx;
	m_selGroup->focusedCategory  = idx;
	if (m_preCategory != lpnmlv->iItem) {
		m_categoryList.SetActiveItem(lpnmlv->iItem);
		m_categoryList.Update(m_preCategory);
		m_categoryList.Update(lpnmlv->iItem);
	}

	// アクセス開始
	if (!RetrieveCategoryItem()) {
		return;
	}

	*pResult = 0;
}

/**
 * カテゴリリスト選択アイテム変更時の処理
 */
void CMZ3View::OnLvnItemchangedCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState != 3) {
		return;
	}

	int idx = (int)m_categoryList.GetItemData( pNMLV->iItem );
	m_selGroup->focusedCategory = idx;

	if (m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY) {
		// 日記ボタンをアクティブにする
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	}
	else {
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	}
	m_infoEdit.SetWindowText(_T(""));

	*pResult = 0;
}

/**
 * アクセス終了通知受信(binary)
 */
LRESULT CMZ3View::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	MZ3LOGGER_DEBUG(_T("OnGetEndBinary start"));

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	if (lParam == NULL) {
		// データがＮＵＬＬの場合
		LPCTSTR msg = L"内部エラーが発生しました(戻り値＝NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return TRUE;
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	switch (aType) {
	case ACCESS_IMAGE:
		{
			// コピー
			CString path = util::MakeImageLogfilePathFromUrl( theApp.m_inet.GetURL() );
			CopyFile( theApp.m_filepath.temphtml, path, FALSE/*bFailIfExists, 上書き*/ );

			// 描画
			if (m_pMiniImageDlg!=NULL) {
				m_pMiniImageDlg->DrawImageFile( path );
			}

			// アイコン差し替え
			CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
			if (pCategory!=NULL) {
				int idx = pCategory->selectedBody;

				// CImageList::Replace が効かないので、リロードしてしまう。
				SetBodyImageList( pCategory->GetBodyList() );
			}

			// アイコン再描画
			InvalidateRect( m_rectIcon, FALSE );
		}
		break;
	}

	// 通信完了（フラグを下げる）
	m_access = FALSE;

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	MZ3LOGGER_DEBUG(_T("OnGetEndBinary end"));

	return TRUE;
}

/**
 * アクセス終了通知受信
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("HTML解析中") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	if (lParam == NULL) {
		// データがＮＵＬＬの場合
		LPCTSTR msg = L"内部エラーが発生しました(戻り値＝NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return TRUE;
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	// ログインページ以外であれば、最初にログアウトチェックを行っておく
	if (aType != ACCESS_LOGIN) {
		// HTML の取得

		// ログアウトチェック
		if (theApp.IsMixiLogout(aType)) {
			// ログアウト状態になっている
			MZ3LOGGER_INFO(_T("再度ログインしてからデータを取得します。"));
			util::MySetInformationText( m_hWnd, L"再度ログインしてからデータを取得します" );

			// mixi データを保存（待避）
			theApp.m_mixiBeforeRelogin = *data;
			data->SetURL( theApp.MakeLoginUrl() );

			// ログイン実行
			data->SetAccessType( ACCESS_LOGIN );
			AccessProc( data, data->GetURL() );

			return TRUE;
		}
	}

	// ログアウトしていなかったのでファイルコピー
	if( theApp.m_optionMng.m_bSaveLog ) {
		// 保存ファイルパスの生成
		CString strLogfilePath = util::MakeLogfilePath( *data );
		if(! strLogfilePath.IsEmpty() ) {
			// 保存ファイルにコピー
			CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, 上書き*/ );
		}
	}

	switch (aType) {
	case ACCESS_LOGIN:
		// --------------------------------------------------
		// ログイン処理
		// --------------------------------------------------
		{
			// HTML の取得
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ログイン成功
				if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
					// オーナーＩＤを取得する
					data->SetAccessType(ACCESS_MAIN); // アクセス種別を設定
					AccessProc(data, _T("http://mixi.jp/check.pl?n=%2Fhome.pl"));
				}
				else {
					// データを待避データに戻す
					*data = theApp.m_mixiBeforeRelogin;
					AccessProc(data, util::CreateMixiUrl(data->GetURL()));
				}
				return TRUE;
			} else {
				// ログイン失敗
				LPCTSTR emsg = _T("ログイン出来ませんでした");
				::MessageBox(m_hWnd, emsg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
				MZ3LOGGER_ERROR( emsg );

				// データを待避データに戻す
				*data = theApp.m_mixiBeforeRelogin;

				m_access = FALSE;

				// プログレスバーを非表示
				mc_progressBar.ShowWindow( SW_HIDE );
				util::MySetInformationText( m_hWnd, emsg );
				return TRUE;
			}
			break;
		}

	case ACCESS_MAIN:
		// --------------------------------------------------
		// メイン画面
		// --------------------------------------------------
		{
			// HTML の取得
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			mixi::HomeParser::parse( html );

			if (m_checkNewComment) {
				// コメント・メッセージ数チェックモード

				// 新着メッセージ、新着コメントの通知
				CString msg;
				if( theApp.m_newMessageCount > 0 ) {
					if( theApp.m_newCommentCount > 0 ) {
						msg.Format(_T("新着メッセージ %d件、新着コメント %d 件"), 
							theApp.m_newMessageCount, theApp.m_newCommentCount );
					}else{
						msg.Format(_T("新着メッセージ %d件"), 
							theApp.m_newMessageCount);
					}
				}
				else {
					if( theApp.m_newCommentCount > 0 ) {
						msg.Format(_T("新着コメント %d件"), 
							theApp.m_newCommentCount);
					}else{
						msg = _T("新着メッセージ、コメントはありません");
					}
				}
				util::MySetInformationText( m_hWnd, msg );

				m_checkNewComment = false;

				theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

				break;
			} else {
				// 新着メッセージ以外なので、ログインのための取得だった。

				// データを待避データに戻す
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return TRUE;
			}
		}

	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
	case ACCESS_LIST_NEWS:
	case ACCESS_LIST_FAVORITE:
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_NEW_BBS_COMMENT:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
	case ACCESS_LIST_INTRO:
	case ACCESS_LIST_BBS:
	case ACCESS_LIST_CALENDAR:
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// --------------------------------------------------
		// カテゴリ項目の取得
		// --------------------------------------------------
		{
			// ステータスコードチェック
			LPCTSTR szStatusErrorMessage = NULL;	// 非NULLの場合はエラー発生
			switch (aType) {
			case ACCESS_TWITTER_FRIENDS_TIMELINE:
				szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
				break;
			}
			if (szStatusErrorMessage!=NULL) {
				CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
				util::MySetInformationText( m_hWnd, msg );
				MZ3LOGGER_ERROR( msg );
				// 以降の処理を行わない。
				break;
			}

			util::MySetInformationText( m_hWnd, _T("HTML解析中 : 1/3") );

			// 巡回モード（リストモード）の場合は、巡回モードを終了する。
			if( m_cruise.enable() && !m_cruise.isFetchListMode() ) {
				// リストモード以外なので通信を終了する
				// 例えばボディモードの最後の要素が「次へ」のような場合にはここに来る。
				if( m_cruise.autoCruise ) {
					// 予約巡回なので次に進む
					m_cruise.targetCategoryIndex++;
					CruiseToNextCategory();
				}else{
					// 一時巡回なのでここで終了。
					m_cruise.finish();
					MessageBox( L"巡回完了" );
				}
				break;
			}

			// HTML の取得
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

			// 保存先 body の取得。
			CMixiDataList& body = m_selGroup->getSelectedCategory()->GetBodyList();

			// HTML 解析
			util::MySetInformationText( m_hWnd,  _T("HTML解析中 : 2/3") );
			mixi::MyDoParseMixiListHtml( aType, body, html );

			// ボディ一覧の設定
			util::MySetInformationText( m_hWnd,  _T("HTML解析中 : 3/3") );

			// 取得時刻文字列の作成
			SYSTEMTIME localTime;
			GetLocalTime(&localTime);
			m_selGroup->getSelectedCategory()->m_bFromLog = false;
			m_selGroup->getSelectedCategory()->SetAccessTime( localTime );
			CString timeStr = m_selGroup->getSelectedCategory()->GetAccessTimeString();
			m_categoryList.SetItemText( m_selGroup->selectedCategory, 1, timeStr );
			SetBodyList( body );		// ボディ一覧に表示

			if( aType == ACCESS_LIST_BBS ) {
				// コミュニティリストの場合は自動的にボディ一覧にフォーカスする
				m_bodyList.SetFocus();
			}

			// 巡回モードなら、ボディ要素の取得を開始する
			if( m_cruise.enable() ) {
				// 巡回モード

				// リストモードなのでボディモードに変更し、
				// ボディの取得を開始する。
				m_cruise.startBodyCruise();

				// ボディの最初の要素を取得する

				// ボディリストにフォーカスを移動する
				CommandSetFocusBodyList();

				// 選択解除
				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody,  false );

				// 次の巡回項目へ。
				if( DoNextBodyItemCruise() ) {
					// 通信継続のためここで return する
					return TRUE;
				}
			}

			util::MySetInformationText( m_hWnd, L"完了" );
		}
		break;

	case ACCESS_DIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
	case ACCESS_PLAIN:
	case ACCESS_PROFILE:
		// --------------------------------------------------
		// ボディ項目の取得
		// --------------------------------------------------

		// 巡回モードなら（解析せずに）次の巡回対象に移動する
		if( m_cruise.enable() ) {
			// 次の巡回対象があれば取得する
			// なければ終了する
			m_cruise.targetBodyItem++;

			// 選択解除
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody,  false );

			if( DoNextBodyItemCruise() ) {
				// 通信継続のためここで return する
				return TRUE;
			}
		}else{
			// 巡回モードでないので、解析してレポート画面を開く
			static CMixiData mixi;
			mixi = *data;
			MyParseMixiHtml( theApp.m_filepath.temphtml, mixi );
			MyShowReportView( mixi );
		}

		break;

	default:
		util::MySetInformationText( m_hWnd, L"完了（アクセス種別不明）" );
		break;
	}

	// 通信完了（フラグを下げる）
	m_access = FALSE;

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	return TRUE;
}

/**
* アクセスエラー通知受信
*/
LRESULT CMZ3View::OnGetError(WPARAM wParam, LPARAM lParam)
{
	// 中止ボタンを使用不可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	CMixiData* pMixi = (CMixiData*)lParam;

	if (pMixi->GetAccessType() == ACCESS_LOGIN) {
		// ログインの場合は待避させているから元に戻す
		*pMixi = theApp.m_mixiBeforeRelogin;
	}

	// 通信エラーが発生した場合の処理
	LPCTSTR smsg = L"エラーが発生しました";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"原因：%s", smsg, theApp.m_inet.GetErrorMessage() );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	m_access = FALSE;

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	return TRUE;
}

/**
 * アクセス中断通知受信
 * 
 * ユーザ指示による中断
 * 
 */
LRESULT CMZ3View::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	m_abort = FALSE;
	m_cruise.stop();

	return TRUE;
}

/**
 * 中断ボタン押下時の処理
 */
LRESULT CMZ3View::OnAbort(WPARAM wParam, LPARAM lParam)
{
	if( theApp.m_inet.IsConnecting() ) {
		// 通信中なら Abort を呼び出す
		theApp.m_inet.Abort();
	}

	// 中断を送信
	m_abort = TRUE;
	m_cruise.stop();

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	m_access = FALSE;

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	LPCTSTR msg = _T("中断しました");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	return TRUE;
}

/**
 * アクセス情報通知
 */
LRESULT CMZ3View::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText( *(CString*)lParam );
	return TRUE;
}

/**
 * 受信サイズ通知
 */
LRESULT CMZ3View::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
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

/**
 * ボディのイメージリストを作成
 *
 * ボディリストは作成済みであること。
 */
void CMZ3View::SetBodyImageList( CMixiDataList& body )
{
	util::MySetInformationText( m_hWnd, L"アイコン作成中..." );

	bool bUseDefaultIcon = false;
	bool bUseExtendedIcon = false;

	// アイコンの表示・非表示の制御
	// 方針：(1) オプション値により非表示になっていればアイコン非表示。
	//       (2) 全アイテムを走査し、アイコンが必要な項目があればアイコン表示、なければ非表示。
	INT_PTR count = body.size();
	if (theApp.m_optionMng.m_bShowMainViewIcon) {
		for (int i=0; i<count; i++) {
			int iconIndex = MyGetBodyListDefaultIconIndex(body[i]);
			if (iconIndex >= 0) {
				// アイコンありなので表示
				bUseDefaultIcon = true;
				break;
			}
		}
	}

	// ユーザやコミュニティの画像をアイコン化して表示する
	if (theApp.m_optionMng.m_bShowMainViewMiniImage && !bUseDefaultIcon) {
		// デフォルトアイコンがなかったので、ユーザ・コミュニティアイコン等を作成する
		for (int i=0; i<count; i++) {
			const CMixiData& mixi = body[i];
			// icon
			CMZ3BackgroundImage image(L"");
			CString miniImagePath = util::MakeImageLogfilePath( mixi );

			// ロード済みか判定
			bool bLoaded = theApp.m_imageCache.GetImageIndex(miniImagePath) >= 0 ? true : false;
			if (bLoaded) {
				// ロード済みなのでロード不要
				bUseExtendedIcon = true;
				continue;
			}

			// 未ロードなのでロード
			image.load( miniImagePath );
			if (image.isEnableImage()) {
				// 16x16 にリサイズする。
				CMZ3BackgroundImage resizedImage(L"");
				util::MakeResizedImage( this, resizedImage, image );

				// ビットマップの追加
				CBitmap bm;
				bm.Attach( resizedImage.getHandle() );

				theApp.m_imageCache.Add( &bm, (CBitmap*)NULL, miniImagePath );

				bUseExtendedIcon = true;
			} else {
				// ロードエラー
				// ダウンロードマネージャに登録する
				if (mixi.GetImageCount()>0 && !miniImagePath.IsEmpty()) {
					CString url = mixi.GetImage(0);
					DownloadItem item( url, L"絵文字", miniImagePath, true );
					theApp.m_pDownloadView->AppendDownloadItem( item );
				}
			}
		}
	}

	// アイコン表示・非表示設定
	m_bodyList.MyEnableIcon( bUseDefaultIcon || bUseExtendedIcon );
	if (bUseDefaultIcon) {
		m_bodyList.SetImageList(&m_iconImageList, LVSIL_SMALL);
	} else if (bUseExtendedIcon) {
		m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList(), LVSIL_SMALL);
	}

	// アイコンの設定
	int itemCount = m_bodyList.GetItemCount();
	for (int i=0; i<itemCount; i++) {
		const CMixiData& mixi = body[i];

		// アイコンのインデックスを種別により設定する
		int iconIndex = -1;
		if (bUseDefaultIcon ) {
			iconIndex = MyGetBodyListDefaultIconIndex(mixi);
		}
		if (bUseExtendedIcon) {
			// ファイルパスからインデックスを解決する
			CString miniImagePath = util::MakeImageLogfilePath( mixi );

			// インデックス探索
			iconIndex = theApp.m_imageCache.GetImageIndex( miniImagePath );
		}

		// アイコンのインデックスを設定
		util::MySetListCtrlItemImageIndex( m_bodyList, i, 0, iconIndex );
	}

	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	util::MySetInformationText( m_hWnd, L"アイコンの作成完了" );
}

/**
 * ボディにデータを設定
 */
void CMZ3View::SetBodyList( CMixiDataList& body )
{
	m_nochange = TRUE;

	// リストのアイテムを削除
	m_bodyList.m_bStopDraw = true;
	m_bodyList.SetRedraw(FALSE);
	m_bodyList.DeleteAllItems();

	// ヘッダの文字を変更
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory != NULL) {
		LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi, pCategory->m_secondBodyColType );
		switch (pCategory->m_mixi.GetAccessType()) {
		case ACCESS_LIST_DIARY:
		case ACCESS_LIST_NEW_COMMENT:
		case ACCESS_LIST_COMMENT:
		case ACCESS_LIST_CALENDAR:
			m_bodyList.SetHeader( _T("タイトル"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_NEWS:
			m_bodyList.SetHeader( _T("見出し"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FAVORITE:
			m_bodyList.SetHeader( _T("名前"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FRIEND:
			m_bodyList.SetHeader( _T("名前"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_COMMUNITY:
			m_bodyList.SetHeader( _T("コミュニティ"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_MESSAGE_IN:
		case ACCESS_LIST_MESSAGE_OUT:
			m_bodyList.SetHeader( _T("件名"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_MYDIARY:
			m_bodyList.SetHeader( _T("タイトル"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_NEW_BBS:
		case ACCESS_LIST_NEW_BBS_COMMENT:
		case ACCESS_LIST_BBS:
		case ACCESS_LIST_BOOKMARK:
			m_bodyList.SetHeader( _T("トピック"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FOOTSTEP:
			m_bodyList.SetHeader( _T("名前"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_INTRO:
			m_bodyList.SetHeader( _T("名前"), szHeaderTitle2 );
			break;
		case ACCESS_TWITTER_FRIENDS_TIMELINE:
			m_bodyList.SetHeader( _T("発言"), szHeaderTitle2 );
			break;
		}
	}

	// アイテムの追加
	m_bodyList.MyEnableIcon( false );	// まずアイコンはオフにして生成
	INT_PTR count = body.size();
	for (int i=0; i<count; i++) {
		CMixiData* data = &body[i];

		// １カラム目
		// どの項目を与えるかは、カテゴリ項目データ内の種別で決める
		int index = m_bodyList.InsertItem( i, MyGetItemByBodyColType(data,pCategory->m_firstBodyColType), -1 );

		// ２カラム目
		m_bodyList.SetItemText( index, 1, MyGetItemByBodyColType(data,pCategory->m_secondBodyColType) );

		// ボディの項目の ItemData に index を割り当てる。
		m_bodyList.SetItemData( index, index );
	}

	m_nochange = FALSE;
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;
	m_bodyList.Invalidate( FALSE );

	// アイコン用ImageListの設定
	SetBodyImageList( body );

	// アイテムが0件の場合は、mini画像画面を非表示にする
	if (m_bodyList.GetItemCount()==0) {
		MoveMiniImageDlg();
	}

	util::MySetInformationText( m_hWnd, L"完了" );
}

/**
 * 情報行のフォーカス設定
 */
void CMZ3View::OnEnSetfocusInfoEdit()
{
	// この行にはフォーカスを移さない
	// カテゴリリストにフォーカスを移す
	m_categoryList.SetFocus();
}

/**
 * ボディリストクリック
 */
void CMZ3View::OnNMDblclkBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	MZ3LOGGER_DEBUG( L"OnNMDblclkBodyList start" );
	*pResult = 0;

	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	// カレントデータを取得
	m_hotList = &m_bodyList;
	m_selGroup->getSelectedCategory()->selectedBody = lpnmlv->iItem;

	CMixiData& data = m_selGroup->getSelectedCategory()->GetSelectedBody();

	TRACE(_T("http://mixi.jp/%s\n"), data.GetURL());

	if (data.GetAccessType() == ACCESS_LIST_FOOTSTEP) {
		return;
	}

	// コミュニティの場合は、トピック一覧を表示する。
	// （暫定対応）
	switch (data.GetAccessType()) {
	case ACCESS_COMMUNITY:
		OnViewBbsList();
		return;

	case ACCESS_TWITTER_USER:
		// ダブルクリックの場合は全文表示
		OnMenuTwitterRead();
		return;
	}

	AccessProc(&data, util::CreateMixiUrl(data.GetURL()));
}

/**
 * アイテム変更
 */
void CMZ3View::OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_nochange != FALSE) {
		return;
	}

	// 選択を変更したら、そのアイテムをアクティブにする
	if (pNMLV->uNewState != 3) {
		return;
	}
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	pCategory->selectedBody = pNMLV->iItem;

	// 第1カラムに表示している内容を表示する。
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), pCategory->m_firstBodyColType, false) );

	// 画像位置変更
	MoveMiniImageDlg();

	// Twitter であれば同一オーナーIDの項目を再表示
	if (pCategory->m_mixi.GetAccessType()==ACCESS_TWITTER_FRIENDS_TIMELINE) {
		// 全件再描画する。
		// 背景の再描画をやめれば少しはマシになるかも。
		m_bodyList.Invalidate(FALSE);
	}

	// アイコン再描画
	InvalidateRect( m_rectIcon, FALSE );

	*pResult = 0;
}

BOOL CMZ3View::OnKeyUp(MSG* pMsg)
{
	// 共通処理
	switch (pMsg->wParam) {
	case VK_F1:
#ifdef WINCE
		if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
			// メインメニューのポップアップ
			RECT rect;
			int flags = TPM_CENTERALIGN | TPM_VCENTERALIGN;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

			CMenu menu;
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			if( theApp.m_bPocketPC ) {
				menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );
			}else{
				menu.LoadMenu(IDR_MAINFRAME);
			}
			menu.GetSubMenu(0)->TrackPopupMenu(flags,
				rect.left,
				rect.bottom,
				pMainFrame );
			menu.Detach();
			return TRUE;
		}
#else
		// ヘルプ表示
		util::OpenByShellExecute( MZ3_CHM_HELPFILENAME );
#endif
		break;
#ifdef WINCE
	case VK_F2:
#endif
#ifndef WINCE
	case VK_APPS:
#endif
		OnAcceleratorContextMenu();
		return TRUE;
	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		// 中断
		if (m_access) {
			::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
		}
		break;

	case 'D':
		// Ctrl+Alt+D
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			WCHAR szFile[MAX_PATH] = L"\0";

			OPENFILENAME ofn;
			memset( &(ofn), 0, sizeof(ofn) );
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = m_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH; 
			ofn.lpstrTitle = L"HTMLﾌｧｲﾙを開く...";
			ofn.lpstrFilter = L"HTMLﾌｧｲﾙ (*.htm;*.html)\0*.htm;*.html\0\0";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
			ofn.lpstrInitialDir = L"";

			if (GetOpenFileName(&ofn) == IDOK) {
				CString strLogfilePath = szFile;

				// アクセス種別の選択
				CChooseAccessTypeDlg dlg;
				if (dlg.DoModal() == IDOK) {
					// 解析
					static CMixiData s_mixi;
					s_mixi.SetAccessType( dlg.m_selectedAccessType );
					MyParseMixiHtml( strLogfilePath, s_mixi );

					// URL 設定
					s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

					// 表示
					MyShowReportView( s_mixi );
				}
			}
		}
		break;

	case 'U':
		// Ctrl+Alt+U
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			COpenUrlDlg dlg;
			if (dlg.DoModal() == IDOK) {

				CInetAccess::ENCODING encoding = (CInetAccess::ENCODING)dlg.m_encoding;

				// アクセス種別の選択
				CChooseAccessTypeDlg dlg1;
				if (dlg1.DoModal() == IDOK) {
					static CMixiData s_mixi;
					s_mixi.SetAccessType( dlg1.m_selectedAccessType );

					// URL 設定
					s_mixi.SetURL( dlg.mc_strUrl );
					s_mixi.SetBrowseUri( dlg.mc_strUrl );

					// 通信開始
					AccessProc( &s_mixi, s_mixi.GetURL(), encoding );
				}
			}
		}
		break;

	default:
		break;
	}

	// Xcrawl Canceler
#ifdef WINCE
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeyup( pMsg->wParam ) ) {
			// キャンセルされたので上下キーを無効にする。
	//		util::MySetInformationText( GetSafeHwnd(), L"Xcrawl canceled..." );
			return TRUE;
		}
	}
#endif

	// 各ペイン毎の処理
	if (pMsg->hwnd == m_categoryList.m_hWnd) {
		if( OnKeyupCategoryList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_bodyList.m_hWnd) {
		if( OnKeyupBodyList( pMsg->wParam ) ) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMZ3View::OnKeyDown(MSG* pMsg)
{
	// Xcrawl Canceler
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeydown(pMsg->wParam) ) {
			return TRUE;
		}
	}

	// 各ペイン毎の処理
	if (pMsg->hwnd == m_groupTab.m_hWnd) {
		if( OnKeydownGroupTab( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_categoryList.m_hWnd) {
		if( OnKeydownCategoryList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_bodyList.m_hWnd) {
		if( OnKeydownBodyList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_statusEdit.m_hWnd) {
		switch (pMsg->wParam) {
		case 'V':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Paste();
				return TRUE;
			}
			break;
		case 'C':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Copy();
				return TRUE;
			}
			break;
		case 'X':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Cut();
				return TRUE;
			}
			break;
		case 'Z':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Undo();
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// メッセージに応じて、定期取得のキャンセル処理を行う
		switch (pMsg->message) {
		case WM_KEYUP:
		case WM_KEYDOWN:
			ResetIntervalTimer();
			break;
		default:
			break;
		}
	}

	if (pMsg->message == WM_KEYUP) {
		BOOL r = OnKeyUp( pMsg );

		// KEYDOWN リピート回数を初期化
		m_nKeydownRepeatCount = 0;

		if (r) {
			return r;
		}
	}
	else if (pMsg->message == WM_KEYDOWN) {
		// KEYDOWN リピート回数をインクリメント
		m_nKeydownRepeatCount ++;

		if (OnKeyDown( pMsg )) {
			return TRUE;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * 他ビューからの復帰処理
 */
LRESULT CMZ3View::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	m_hotList->SetFocus();
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, true );
	theApp.ChangeView(theApp.m_pMainView);

	// アイテムの制御
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);

	// 書き込みボタン
	// 自分の日記の場合は有効に。
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON,
		(m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY));

	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	// mini画像ウィンドウの復帰
	MoveMiniImageDlg();

	return TRUE;
}

/**
 * 日記投稿メニュー
 */
void CMZ3View::OnWriteDiary()
{
	if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
		// オーナーＩＤを取得する
		// 日記一覧から取得

		// 全グループの全カテゴリを走査し、
		// 「日記」カテゴリを検索し、
		// 日記の CMixiData の URL からオーナーIDを取得する。
		bool bFound = false;
		for( size_t i=0; i<m_selGroup->categories.size(); i++ ) {
			CCategoryItem& category = m_selGroup->categories[i];
			if( category.m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY ) {
				// 日記発見。
				// 第1要素を探索する。
				if( category.GetBodyList().size() > 0 ) {
					CMixiData& mixi = category.GetBodyList()[0];
					CString& url = mixi.GetURL();
					theApp.m_loginMng.SetOwnerID(
						mixi::MixiUrlParser::GetOwnerID( url ) );

					theApp.m_loginMng.Write();

					bFound = true;
					break;
				}
			}
		}
	}

	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWDIARY, NULL );
}

/**
 * グループタブの選択変更イベント
 */
void CMZ3View::OnTcnSelchangeGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnSelchangedGroupTab();

	*pResult = 0;
}

/**
 * コマンド：グループタブの左項目への選択変更
 */
BOOL CMZ3View::CommandSelectGroupTabBeforeItem()
{
	// 左項目へ選択変更。
	if( m_groupTab.GetCurSel() == 0 ) {
		// 左端なので右端へ。
		m_groupTab.SetCurSel( m_groupTab.GetItemCount()-1 );
	}else{
		// 左項目へ選択変更。
		m_groupTab.SetCurSel( m_groupTab.GetCurSel()-1 );
	}

	// 遅延ロードスレッド開始
	AfxBeginThread( ReloadGroupTab_Thread, this );

	return TRUE;
}

/**
 * コマンド：グループタブの右項目への選択変更
 */
BOOL CMZ3View::CommandSelectGroupTabNextItem()
{
	// 右項目へ選択変更。
	if( m_groupTab.GetCurSel() == m_groupTab.GetItemCount()-1 ) {
		// 右端なので左端へ。
		m_groupTab.SetCurSel( 0 );
	}else{
		// 右項目へ選択変更。
		m_groupTab.SetCurSel( m_groupTab.GetCurSel()+1 );
	}

	// 遅延ロードスレッド開始
	AfxBeginThread( ReloadGroupTab_Thread, this );

	return TRUE;
}

/**
 * コマンド：カテゴリリストへのフォーカス移動
 */
BOOL CMZ3View::CommandSetFocusCategoryList()
{
	m_categoryList.SetFocus();
	m_hotList = &m_categoryList;
	m_infoEdit.SetWindowText( _T("") );

	m_categoryList.EnsureVisible( m_selGroup->focusedCategory, FALSE);

	return TRUE;
}

/**
 * コマンド：グループタブへのフォーカス移動
 */
BOOL CMZ3View::CommandSetFocusGroupTab()
{
	m_groupTab.SetFocus();
	m_infoEdit.SetWindowText( L"" );

	return TRUE;
}

/**
 * コマンド：ボディリストへのフォーカス移動
 */
BOOL CMZ3View::CommandSetFocusBodyList()
{
	if (m_bodyList.GetItemCount() != 0) {
		m_bodyList.SetFocus();
		m_hotList = &m_bodyList;

		// 第1カラムに表示している内容を表示する。
		m_infoEdit.SetWindowText( 
			MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType, false) );

		// 選択状態を更新
		int idx = m_selGroup->getSelectedCategory()->selectedBody;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idx, true );
		m_bodyList.EnsureVisible( idx, FALSE);
	}
	return TRUE;
}

/**
 * グループタブのキーダウンイベント
 */
BOOL CMZ3View::OnKeydownGroupTab( WORD vKey )
{
	switch( vKey ) {
	case VK_UP:
		// 上キー。
		if( m_access ) return TRUE;	// アクセス中は無視
		// 無視
		return TRUE;
	case VK_DOWN:
		// 下キー。
		// カテゴリーリストへのフォーカス移動
		return CommandSetFocusCategoryList();
	case VK_LEFT:
		// 左キー。
		if( m_access ) return TRUE;	// アクセス中は無視
		// 選択変更
		return CommandSelectGroupTabBeforeItem();
	case VK_RIGHT:
		// 右キー。
		if( m_access ) return TRUE;	// アクセス中は無視
		// 選択変更
		return CommandSelectGroupTabNextItem();
	case VK_RETURN:
		// Enterキー。
		// カテゴリーリストへのフォーカス移動
		return CommandSetFocusCategoryList();
	}
	return FALSE;
}

/**
 * カテゴリリストのキーダウンイベント
 */
BOOL CMZ3View::OnKeydownCategoryList( WORD vKey )
{
	// VK_UP, VK_DOWN
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// VK_KEYDOWN では無視。
			// VK_KEYUP で処理する。
			// これは、アドエスの Xcrawl 対応のため。

			// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveUpCategoryList();
			}

			return TRUE;

		case VK_DOWN:
			// VK_KEYDOWN では無視。
			// VK_KEYUP で処理する。
			// これは、アドエスの Xcrawl 対応のため。

			// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveDownCategoryList();
			}

			return TRUE;
		}
	} else {
		switch( vKey ) {
		case VK_UP:
			if( m_categoryList.GetItemState(0, LVIS_FOCUSED) != FALSE ) {
				// 一番上の項目なら無視
				return TRUE;
			} else {
#ifdef WINCE
				// デフォルト動作
				return FALSE;
#else
				return CommandMoveUpCategoryList();
#endif
			}
			break;

		case VK_DOWN:
			if( m_categoryList.GetItemState(m_categoryList.GetItemCount()-1, LVIS_FOCUSED) != FALSE ) {
				// 一番下の項目選択中なら、ボディリストの先頭へ。
				if (m_bodyList.GetItemCount() != 0) {
					// 選択状態を先頭に。以前の選択状態をOffに。
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
					m_selGroup->getSelectedCategory()->selectedBody = 0;
				}
				return CommandSetFocusBodyList();
			} else {
#ifdef WINCE
				// デフォルト動作
				return FALSE;
#else
				return CommandMoveDownCategoryList();
#endif
			}
			break;
		}
	}

	switch( vKey ) {
	case VK_LEFT:
		if( m_access ) return TRUE;	// アクセス中は無視

		// グループタブの選択変更
		return CommandSelectGroupTabBeforeItem();

		// グループタブに移動
//		return CommandSetFocusGroupTab();
		return TRUE;
	case VK_RIGHT:
		if( m_access ) return TRUE;	// アクセス中は無視

		// グループタブの選択変更
		return CommandSelectGroupTabNextItem();

		// ボディリストに移動
//		return CommandSetFocusBodyList();
	case VK_RETURN:
		if (m_selGroup->selectedCategory == m_selGroup->focusedCategory) {
			RetrieveCategoryItem();
		}
		else {
			// 非選択項目なので、取得時刻とボディの変更。
			// 非取得で、ログがあるならログから取得。

			// アクセス中は選択不可
			if (m_access) {
				return TRUE;
			}
			m_selGroup->selectedCategory = m_selGroup->focusedCategory;

			OnMySelchangedCategoryList();
		}
		return TRUE;
	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		if( m_access ) {
			// アクセス中は無視
			return TRUE;
		}
		// 非アクセス中は、ボディリストに移動する
		return CommandSetFocusBodyList();
	}
	return FALSE;
}

/**
 * カテゴリリストのキーUPイベント
 */
BOOL CMZ3View::OnKeyupCategoryList( WORD vKey )
{
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// キー長押しによる連続移動中なら、キーUPで移動しない。
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}
			return CommandMoveUpCategoryList();

		case VK_DOWN:
			// キー長押しによる連続移動中なら、キーUPで移動しない。
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}
			return CommandMoveDownCategoryList();
		}
	}
	return FALSE;
}

/**
 * ボディリストのキーダウンイベント
 */
BOOL CMZ3View::OnKeydownBodyList( WORD vKey )
{
	// ボディーリストでのキー押下

	// VK_UP, VK_DOWN
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// VK_KEYDOWN では無視。
			// VK_KEYUP で処理する。
			// これは、アドエスの Xcrawl 対応のため。

			// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveUpBodyList();
			}
			return TRUE;

		case VK_DOWN:
			// VK_KEYDOWN では無視。
			// VK_KEYUP で処理する。
			// これは、アドエスの Xcrawl 対応のため。

			// ただし、２回目以降のキー押下であれば、長押しとみなし、移動する
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveDownBodyList();
			}
			return TRUE;
		}
	} else {
		switch( vKey ) {
		case VK_UP:
			if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
				// 一番上。
				// カテゴリに移動

		//		if( m_access ) return TRUE;	// アクセス中は禁止

				// 選択状態を末尾に。以前の選択状態をOffに。
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
				m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
				
				return CommandSetFocusCategoryList();
			}else{
#ifdef WINCE
				// デフォルト動作
				return FALSE;
#else
				return CommandMoveUpBodyList();
#endif
			}
			break;

		case VK_DOWN:
			if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
				// 一番下なので無視。
				if( m_access ) return TRUE;	// アクセス中は禁止
				return TRUE;
			}else{
#ifdef WINCE
				// デフォルト動作
				return FALSE;
#else
				return CommandMoveDownBodyList();
#endif
			}
			break;
		}
	}

	switch( vKey ) {
	case VK_RETURN:

		if( m_dwLastReturn != 0 ) {
			if( GetTickCount() < m_dwLastReturn + theApp.m_optionMng.m_longReturnRangeMSec ) {
				// 長押し判定時間内に離されたので、通常押下とみなす。

				// キー押下時刻をクリアしておく。
				m_dwLastReturn = 0;
			}else{
				// 長押し判定時間以上経っているので、ログを開く

				util::MySetInformationText( m_hWnd, L"ログを開いています..." );

				// ログを開く
				OnViewLog();

				// キー押下時刻をクリアしておく。
				m_dwLastReturn = 0;

				// ここでは終了。
				return TRUE;
			}
		}

		// アクセス中は再アクセス不可
		if( m_access ) return TRUE;

		switch( GetSelectedBodyItem().GetAccessType() ) {
		case ACCESS_COMMUNITY:
		case ACCESS_TWITTER_USER:
			// メニュー表示
			PopupBodyMenu();
			break;

		default:
			// 特殊な要素以外なので、通信処理開始。
			AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
			break;
		}
		return TRUE;

	case VK_PROCESSKEY:	// 0xE5
	case VK_F23:		// 0x86
		// W-ZERO3 で RETURN キー押下時に飛んでくるキー。
		if( m_dwLastReturn == 0 ) {
			// m_dwLastReturn を更新しておく。
			m_dwLastReturn = GetTickCount();

			// 長押し判定用スレッドを開始
			AfxBeginThread( LongReturnKey_Thread, this );
		}
		break;

	case VK_LEFT:
		// 左ボタン。
		// ショートカット移動。
		{
			CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
			if (pCategory != NULL) {
				int idxSel = pCategory->selectedBody;
				int nItem = m_bodyList.GetItemCount();

				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idxSel, false );
				if( idxSel == 0 ) {
					// 一番上 → 一番下
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
					m_bodyList.EnsureVisible( nItem-1, FALSE);
					return TRUE;
				}else if( idxSel == nItem-1 ) {
					// 一番下     → 一番上
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
					m_bodyList.EnsureVisible( 0, FALSE );
					return TRUE;
				}else if( idxSel < nItem/2 ) {
					// 半分より上 → 一番上
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
					m_bodyList.EnsureVisible( 0, FALSE );
					return TRUE;
				}else{
					// 半分より下 → 一番下
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
					m_bodyList.EnsureVisible( nItem-1, FALSE);
					return TRUE;
				}
			}
		}
		// カテゴリに移動
//		return CommandSetFocusCategoryList();
	case VK_RIGHT:
		// 右ボタンで、２つ目の項目を変化させる
		MyChangeBodyHeader();
		return TRUE;

	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		if( m_access ) {
			// アクセス中は無視
			return TRUE;
		}
		// 非アクセス中は、カテゴリリストに移動する
		return CommandSetFocusCategoryList();

		// 非アクセス中は、カテゴリリストまたは入力領域に移動する
/*		switch (m_viewStyle) {
		case VIEW_STYLE_TWITTER:
			GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
			return TRUE;
		case VIEW_STYLE_DEFAULT:
		default:
			return CommandSetFocusCategoryList();
		}
*/
		return CommandSetFocusCategoryList();

	default:
//		if( MZ3LOGGER_IS_DEBUG_ENABLED() ) {
//			CString msg;
//			msg.Format( L"WM_KEYDOWN, 0x%X", vKey );
//			MZ3LOGGER_DEBUG( msg );
//		}
		break;
	}
	return FALSE;
}

/**
 * ボディリストのキーUPイベント
 */
BOOL CMZ3View::OnKeyupBodyList( WORD vKey )
{
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// キー長押しによる連続移動中なら、キーUPで移動しない。
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}

			return CommandMoveUpBodyList();

		case VK_DOWN:
			// キー長押しによる連続移動中なら、キーUPで移動しない。
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}

			return CommandMoveDownBodyList();
		}
	}

	return FALSE;
}

BOOL CMZ3View::CommandMoveUpCategoryList()
{
//	if( m_access ) return TRUE;	// アクセス中は無視

	if( m_categoryList.GetItemState(0, LVIS_FOCUSED) != FALSE ) {
		// 一番上の項目なら無視
		return TRUE;
	} else {
		// 一番上ではないので、上に移動
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory --;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );

		// 移動先が非表示なら上方向にスクロール
		if( !util::IsVisibleOnListBox( m_categoryList, m_selGroup->focusedCategory ) ) {
			m_categoryList.Scroll( CSize(0, -m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

			// 再描画
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_categoryList.RedrawItems(0, m_categoryList.GetItemCount());
				m_categoryList.UpdateWindow();
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveDownCategoryList()
{
//	if( m_access ) return TRUE;	// アクセス中は無視

	if( m_categoryList.GetItemState(m_categoryList.GetItemCount()-1, LVIS_FOCUSED) != FALSE ) {
		// 一番下の項目選択中なら、ボディリストの先頭へ。
		if (m_bodyList.GetItemCount() != 0) {
			// 選択状態を先頭に。以前の選択状態をOffに。
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
			m_selGroup->getSelectedCategory()->selectedBody = 0;
		}
		return CommandSetFocusBodyList();
	} else {
		// 一番下ではないので、下に移動
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory ++;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );

		// 移動先が非表示なら下方向にスクロール
		if( !util::IsVisibleOnListBox( m_categoryList, m_selGroup->focusedCategory ) ) {
			m_categoryList.Scroll( CSize(0, m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

			// 再描画
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_categoryList.RedrawItems(0, m_categoryList.GetItemCount());
				m_categoryList.UpdateWindow();
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveUpBodyList()
{
	if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
		// 一番上。
		// カテゴリに移動

//		if( m_access ) return TRUE;	// アクセス中は禁止

		// 選択状態を末尾に。以前の選択状態をOffに。
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
		
		return CommandSetFocusCategoryList();
	}else{
		// 一番上ではない。
		// 上に移動
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, false );
			pCategory->selectedBody --;
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// 移動先が非表示なら上方向にスクロール
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				m_bodyList.Scroll( CSize(0, -m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

				// 再描画
				if (theApp.m_optionMng.IsUseBgImage()) {
					m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
					m_bodyList.UpdateWindow();
					MoveMiniImageDlg();
				}
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveDownBodyList()
{
	if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
		// 一番下なので無視。
		if( m_access ) return TRUE;	// アクセス中は禁止
		return TRUE;
	}else{
		// 一番下ではない。
		// 下に移動
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, false );
			pCategory->selectedBody ++;
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// 移動先が非表示なら下方向にスクロール
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				m_bodyList.Scroll( CSize(0, m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

				// 再描画
				if (theApp.m_optionMng.IsUseBgImage()) {
					m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
					m_bodyList.UpdateWindow();
					MoveMiniImageDlg();
				}
			}
		}
		return TRUE;
	}
}


/**
 * 長押し判定用スレッド
 */
unsigned int CMZ3View::LongReturnKey_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	// 長押し判定時間分、Sleep する。
	Sleep( theApp.m_optionMng.m_longReturnRangeMSec );

	if( pView->m_dwLastReturn != 0 ) {
		// まだ VK_RETURN が来ていないので、ログを開く
		// のはボタンリリース時で、ここではメッセージのみ変更しておく。
		util::MySetInformationText( pView->GetSafeHwnd(), L"ログを開きます..." );

		// W-ZERO3 の場合は、VK_PROCESSKEY 押下からの経過時間で長押し判定
	}else{
		// 既に VK_RETURN が来ているので、何もしない。
	}

	return 0;
}

/**
 * 書き込みボタンの制御
 */
void CMZ3View::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
	if( theApp.m_bPocketPC ) {
#ifdef WINCE
		CMainFrame* pFrame;
		pFrame = (CMainFrame*)(theApp.GetMainWnd());
		pCmdUI->Enable(pFrame->m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));  
#endif
	}
}

/**
 * 書き込みボタン
 */
void CMZ3View::OnWriteButton()
{
	OnWriteDiary();
}

/**
 * ボディリストのフォーカス取得イベント
 *
 * ボタン制御など。
 */
void CMZ3View::OnNMSetfocusBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ブラウザボタン
	// 足あとの場合は有効に。
	if (m_selGroup!=NULL && m_selGroup->getSelectedCategory()!= NULL) {
		switch (m_selGroup->getSelectedCategory()->m_mixi.GetAccessType()) {
			case ACCESS_LIST_FOOTSTEP:
			case ACCESS_LIST_FRIEND:
				theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );
				break;

			default:
				theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );
				break;
		}
	} else {
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );
	}

	*pResult = 0;
}

/**
 * カテゴリリストのフォーカス取得イベント
 */
void CMZ3View::OnNMSetfocusHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ブラウザボタン
	// 常に無効
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	*pResult = 0;
}

/**
 * ブラウザで開く
 */
void CMZ3View::OnOpenBrowser()
{
	LPCTSTR url = GetSelectedBodyItem().GetBrowseUri();
	if( wcslen(url) == 0 ) {
		// url が空文字なので終了。
		return;
	}

	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_PROFILE:			// プロフィール
		{
			CString name = GetSelectedBodyItem().GetName();
			switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
			case ACCESS_LIST_FRIEND:		// マイミク一覧
				// 名前が「XXX さん(N)」となっているので、
				// 「XXX」に整形。
				int idx = name.ReverseFind( '(' );	// まず '(' 以降を削除
				if( idx > 0 ) {
					name = name.Left( idx );
					// 末尾の「さん」を削除
					if( name.Right(2) == L"さん" && name.GetLength() > 2 ) {
						name = name.Left( name.GetLength()-2 );
					}
				}
			}
			util::OpenBrowserForUser( url, name );
		}
		break;
	default:
		util::OpenBrowserForUrl( url );
		break;
	}
}

/**
 * デバッグ情報表示
 *
 * 現在の選択項目に関する情報を表示する
 */
void CMZ3View::OnShowDebugInfo()
{
	CWnd* wnd = GetFocus();
	if( wnd->m_hWnd == m_categoryList.m_hWnd ) {
		// カテゴリの項目に関する情報を表示する
		theApp.ShowMixiData( &m_selGroup->getFocusedCategory()->m_mixi );
	}else if( wnd->m_hWnd == m_groupTab.m_hWnd ) {
		// グループの項目に関する情報を表示する
		theApp.ShowMixiData( &m_selGroup->mixi );
	}else{
		// ボディの項目に関する情報を表示する
		theApp.ShowMixiData( &GetSelectedBodyItem() );
	}
}

/**
 * 新着メッセージのチェック
 */
bool CMZ3View::DoNewCommentCheck(void)
{
	if( m_access ) {
		// アクセス中は禁止
		return false;
	}

	static CMixiData mixi;
	mixi.SetAccessType( ACCESS_MAIN );

	m_checkNewComment = true;

	AccessProc( &mixi, _T("http://mixi.jp/home.pl") );

	return true;
}

/**
 * ネットアクセス
 */
void CMZ3View::AccessProc(CMixiData* data, LPCTSTR a_url, CInetAccess::ENCODING encoding)
{
	if( data != NULL ) {
		theApp.m_mixi4recv = *data;
	}

	CString uri;

	// アクセス種別を設定
	theApp.m_accessType = data->GetAccessType();

	switch (data->GetAccessType()) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// コミュニティ、アンケートなら、取得件数を変更
		switch( theApp.m_optionMng.GetPageType() ) {
		case GETPAGE_LATEST20:
			// 最新２０件取得
			{
				uri = a_url;
			}
			break;

		case GETPAGE_ALL:
			// 全件取得
			{
				CString buf = a_url;
				if (buf.Find(_T("comment_count")) != -1) {
					int pos = 0;
					CString tmp = buf.Tokenize(_T("&"), pos);
					uri = tmp;
					tmp = buf.Tokenize(_T("&"), pos); // ここは捨てる
					tmp = buf.Tokenize(_T("&"), pos);
					uri += _T("&");
					uri += tmp;
					uri += _T("&page=all");
				}
				else {
					uri = a_url;
					uri += _T("&page=all");
				}
			}
			break;
		}
		break;
	default:
		uri = a_url;
		break;
	}

	// リファラ
	CString referer;
	if (data->GetAccessType() == ACCESS_ENQUETE) {
		// アンケートの場合はリファラーを設定
		referer = _T("http://mixi.jp/") + data->GetURL();
		referer.Replace(_T("view_enquete"), _T("reply_enquete"));
	}

	// 【API 用】
	// URL 内のID置換
	uri.Replace( L"{owner_id}", theApp.m_loginMng.GetOwnerID() );

	data->SetBrowseUri(uri);

	// encoding 指定
	switch (data->GetAccessType()) {
	case ACCESS_LIST_FOOTSTEP:
		// mixi API => UTF-8
		encoding = CInetAccess::ENCODING_UTF8;
		break;

	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// Twitter API => UTF-8
		encoding = CInetAccess::ENCODING_UTF8;
		break;
	}

	// 認証情報の設定
	LPCTSTR szUser = NULL;
	LPCTSTR szPassword = NULL;
	switch (data->GetAccessType()) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// Twitter API => Basic 認証
		szUser     = theApp.m_loginMng.GetTwitterId();
		szPassword = theApp.m_loginMng.GetTwitterPassword();

		// 未指定の場合はエラー出力
		if (wcslen(szUser)==0 || wcslen(szPassword)==0) {
			MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
			return;
		}
		break;
	}

	// 中止ボタンを使用可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// アクセス開始
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, data, encoding );
	theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML, szUser, szPassword );
}

/// 右ソフトキーメニュー｜全部読む
void CMZ3View::OnGetAll()
{
	// チェック
	if( m_hotList != &m_bodyList ) {
		return;
	}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// ok.
		break;
	default:
		// NG.
		return;
	}

	// 全件に設定し、アクセス開始
	theApp.m_optionMng.SetPageType( GETPAGE_ALL );
	AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
}

/// 右ソフトキーメニュー｜最新の20件を読む
void CMZ3View::OnGetLast10()
{
	// チェック
	if( m_hotList != &m_bodyList ) {
		return;
	}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// ok.
		break;
	default:
		// NG.
		return;
	}

	// 20件に設定し、アクセス開始
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST20 );
	AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
}

/**
 * カテゴリリストの選択項目の変更時の処理
 */
void CMZ3View::OnMySelchangedCategoryList(void)
{
	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	// 選択状態（赤）の変更
	m_categoryList.SetActiveItem( m_selGroup->selectedCategory );

	// 選択項目の再描画
	m_categoryList.Update( m_selGroup->selectedCategory );

	// フォーカス項目の再描画
	if( m_selGroup->focusedCategory != m_selGroup->selectedCategory ) {
		m_categoryList.Update( m_selGroup->focusedCategory );
	}

	// 前回選択項目の再描画
	if (m_preCategory != m_selGroup->selectedCategory &&
		m_preCategory != m_selGroup->focusedCategory ) 
	{
		m_categoryList.Update( m_preCategory );
		m_preCategory = m_selGroup->selectedCategory;
	}

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		CMixiDataList body;
		SetBodyList( body );
	} else {
		// 選択項目が「未取得」なら、とりあえずファイルから取得する
		if( wcscmp( pCategory->GetAccessTimeString(), L"" ) == 0 ) {
			MyLoadCategoryLogfile( *pCategory );
		}

		// ボディリストに設定
		SetBodyList( pCategory->GetBodyList() );
	}
}

/**
 * カテゴリ項目をログファイルから読み込む
 */
bool CMZ3View::MyLoadCategoryLogfile( CCategoryItem& category )
{
	CString logfile = util::MakeLogfilePath( category.m_mixi );
	if( logfile.IsEmpty() ) {
		return false;
	}

	// ファイルの存在確認
	CFileStatus status;
	if( !CFile::GetStatus(logfile, status) ) {
		return false;
	}

	// 更新時刻の取得
//	CString time = status.m_mtime.Format( L"%m/%d %H:%M:%S log" );

	{
		CMixiDataList& body = category.GetBodyList();

		CString msgHead;
		msgHead.Format( L"%s : ", util::AccessType2Message(category.m_mixi.GetAccessType()) );

		// HTML の取得
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML 解析
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 2/3") );
		mixi::MyDoParseMixiListHtml( category.m_mixi.GetAccessType(), body, html );

		// ボディ一覧の設定
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 3/3") );

		// 取得時刻文字列の設定
		SYSTEMTIME st;
		status.m_mtime.GetAsSystemTime( st );
		category.SetAccessTime( st );
		category.m_bFromLog = true;
		m_categoryList.SetItemText( category.GetIndexOnList(), 1, category.GetAccessTimeString() );

		util::MySetInformationText( m_hWnd, msgHead + _T("完了") );
	}

	return true;
}

/**
 * 「ログを開く」メニュー
 */
void CMZ3View::OnViewLog()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// レポート画面で開けるタイプのみサポートする
	switch (mixi.GetAccessType()) {	
	case ACCESS_DIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_PROFILE:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		// サポートしている
		break;
	default:
		// 未サポートなので終了する
		{
			CString msg = L"この形式のログはサポートしていません : ";
			msg += util::AccessType2Message(mixi.GetAccessType());
			MZ3LOGGER_INFO( msg );

			util::MySetInformationText( m_hWnd, msg );
		}
		return;
	}

	CString strLogfilePath = util::MakeLogfilePath( mixi );

	// ファイル存在確認
	if(! util::ExistFile( strLogfilePath ) ) {
		// FILE NOT FOUND.
		CString msg = L"ログファイルがありません : " + strLogfilePath;
		MZ3LOGGER_INFO( msg );

		util::MySetInformationText( m_hWnd, msg );
		
		return;
	}

	// 解析
	static CMixiData s_mixi;
	s_mixi = mixi;
	MyParseMixiHtml( strLogfilePath, s_mixi );

	// URL 設定
	s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

	// 表示
	MyShowReportView( s_mixi );
}

/**
 * szHtmlfile を解析し、mixi を構築する
 */
void CMZ3View::MyParseMixiHtml(LPCTSTR szHtmlfile, CMixiData& mixi)
{
	// HTML の取得
	CHtmlArray html;
	html.Load( szHtmlfile );

	// HTML 解析
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"完了" );
}

/**
 * レポートビューを開き、mixi データを参照する
 */
void CMZ3View::MyShowReportView(CMixiData& mixi)
{
	// レポートビューに遷移
	util::MySetInformationText( m_hWnd, L"wait..." );
	theApp.m_pReportView->SetData( mixi );

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
	theApp.ChangeView( theApp.m_pReportView );
}

void CMZ3View::OnHdnItemclickBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	if( phdr->iItem == 1 ) {
		// 表示項目の変更（トグル）
		MyChangeBodyHeader();
	}

	*pResult = 0;
}

bool CMZ3View::MyChangeBodyHeader(void)
{
	// 状態に応じて、「表示項目」の変更を行う
	// いわゆるトグル動作。
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory == NULL) {
		return false;
	}
	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
	case ACCESS_LIST_NEWS:
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
	case ACCESS_LIST_NEW_BBS_COMMENT:
		// 「名前」と「時刻」
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_NAME ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		}
		break;
	case ACCESS_LIST_FAVORITE:
		// 「日付」と「本文」
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_BODY ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_BODY;
		}
		break;
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// 「日付」と「名前」
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_DATE ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}
		break;
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_BOOKMARK:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_INTRO:
	case ACCESS_LIST_CALENDAR:
	default:
		// それ以外では変更しないので終了。
		return false;
	}

	// ヘッダー文字列の変更（第２カラムのみ）
	m_bodyList.SetHeader( NULL, 
		MyGetBodyHeaderColName2(pCategory->m_mixi, pCategory->m_secondBodyColType) );

	// アイテムの更新
	INT_PTR count = pCategory->GetBodyList().size();
	for (int i=0; i<count; i++) {
		CMixiData& data = pCategory->GetBodyList()[i];
		// ２カラム目
		m_bodyList.SetItem( i, 1, LVIF_TEXT, 
			MyGetItemByBodyColType(&data,pCategory->m_secondBodyColType), 0, 0, 0, 0 );
	}

	// 第1カラムに表示している内容を表示する。
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType, false) );

	// アイコン再描画
	InvalidateRect( m_rectIcon, FALSE );

	return true;
}

/**
 * m_selGroup のグループで、カテゴリリストを初期化する。
 */
void CMZ3View::MyUpdateCategoryListByGroupItem(void)
{
	m_categoryList.SetRedraw( FALSE );
	m_categoryList.m_bStopDraw = true;	// 表示停止

	CCategoryItemList& list = m_selGroup->categories;
	size_t size = list.size();

	for( size_t i=0; i<size; i++ ) {
		CCategoryItem& category = list[i];

		// カテゴリーリストに追加する
		// カテゴリーリストの現サイズより i が
		// 同じか大きければ追加。
		// 小さければ更新。
		if( i>=(size_t)m_categoryList.GetItemCount() ) {
			m_categoryList.InsertItem( category.GetIndexOnList(), category.m_name, 0 );
		}else{
			m_categoryList.SetItemText( i, 0, category.m_name );
			util::MySetListCtrlItemFocusedAndSelected( m_categoryList, i, i==m_selGroup->focusedCategory );
		}

		// 取得時刻文字列の設定
		m_categoryList.SetItemText( i, 1, category.GetAccessTimeString() );

		// ItemData にインデックスを与える
		m_categoryList.SetItemData( i, (DWORD_PTR)i );
	}
	// あふれた分を削除
	int n = m_categoryList.GetItemCount();
	if( (size_t)n > size ) {
		for( int i=size; i<n; i++ ) {
			// あふれた分の最初のインデックス(=size)をn-size回指定して、削除する
			m_categoryList.DeleteItem( size );
		}
	}

	// フォーカス、選択状態の復帰
	if( size > 0 ) {
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
		m_categoryList.EnsureVisible( m_selGroup->focusedCategory, FALSE );

		// 選択状態（赤）の復帰
		m_categoryList.SetActiveItem( m_selGroup->selectedCategory );
	}

	m_categoryList.SetRedraw( TRUE );
	m_categoryList.m_bStopDraw = false;	// 表示再開
	m_categoryList.Invalidate( FALSE );
}

/**
 * グループタブの選択変更時の共通処理
 */
void CMZ3View::OnSelchangedGroupTab(void)
{
	// 選択中のグループ項目の設定
	m_selGroup = &theApp.m_root.groups[ m_groupTab.GetCurSel() ];

	// カテゴリーリストを初期化する
	MyUpdateCategoryListByGroupItem();
	m_categoryList.Update( 0 );

	// 選択変更時の処理を実行する（ログの読み込み）
	m_categoryList.m_bStopDraw = true;
	OnMySelchangedCategoryList();
	m_categoryList.m_bStopDraw = false;
#ifdef WINCE
	m_categoryList.Invalidate( FALSE );
#else
	m_categoryList.Invalidate( TRUE );
#endif

	// アイコン再描画
	InvalidateRect( m_rectIcon, FALSE );
}

/**
 * グループタブ遅延読み込み用スレッド
 */
unsigned int CMZ3View::ReloadGroupTab_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	static bool s_reloading = false;
	if( s_reloading ) {
		return 0;
	}

	s_reloading = true;
	::Sleep( 10L );

	// 処理前後で選択グループが変化していれば、再読込を行う
	int selectedGroup = pView->m_groupTab.GetCurSel();
	for(;;) {
		// 選択変更時の処理を実行する
		pView->OnSelchangedGroupTab();

		if( selectedGroup == pView->m_groupTab.GetCurSel() ) {
			// 処理前と選択グループが同じなので処理完了とする。
			break;
		}else{
			// 処理前と選択グループが違うので、再読込を行う。
			selectedGroup = pView->m_groupTab.GetCurSel();
		}
	}

	s_reloading = false;

	return 0;
}

/// 「プロフィールページをブラウザで開く」メニューイベント
void CMZ3View::OnOpenBrowserUser()
{
	OnOpenBrowser();
}

/// 紹介文を簡易表示メニューイベント
void CMZ3View::OnOpenIntro()
{
	// 本文（紹介文）を表示
	// 同じ処理なので OnOpenSelfintro に委譲。
	OnOpenSelfintro();
}

/// 自己紹介を簡易表示メニューイベント
void CMZ3View::OnOpenSelfintro()
{
	// 本文（自己紹介）を表示
	CString body = L"";
	CMixiData& mixi = GetSelectedBodyItem();
	for( u_int i=0; i<mixi.GetBodySize(); i++ ) {
		body.Append( mixi.GetBody(i) );
	}
	MessageBox( body );
}

/// 未読にする
void CMZ3View::OnSetNoRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ログINIファイルの項目を初期化
	CString logId = util::GetLogIdString( mixi );
	theApp.m_logfile.DeleteRecord( util::my_wcstombs((LPCTSTR)logId), "Log" );

	// ログファイル削除
	DeleteFile( util::MakeLogfilePath(mixi) );

	// ビューを更新
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}


/// 既読にする
void CMZ3View::OnSetRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ログINIファイルの項目を変更
	CString logId = util::GetLogIdString( mixi );
	theApp.m_logfile.SetValue(util::my_wcstombs((LPCTSTR)logId), (const char*)util::int2str_a(mixi.GetCommentCount()), "Log");

	// ビューを更新
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// ボディリストでの右クリックメニュー
bool CMZ3View::PopupBodyMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMixiData& bodyItem = GetSelectedBodyItem();
	switch( bodyItem.GetAccessType() ) {
	case ACCESS_DIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(0);	// 一般メニューはidx=0

			switch( bodyItem.GetAccessType() ) {
			case ACCESS_ENQUETE:
			case ACCESS_BBS:
				// BBS, アンケート
				// 20件or全件の切り替えメニューを出す
				if( theApp.m_optionMng.GetPageType() == GETPAGE_ALL ) {
					// 「全件」にチェック
					pSubMenu->CheckMenuItem( ID_GET_ALL, MF_CHECKED | MF_BYCOMMAND );
				}
				if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST20 ) {
					// 「最新の20件」にチェック
					pSubMenu->CheckMenuItem( ID_GET_LAST10, MF_CHECKED | MF_BYCOMMAND );
				}
				break;
			default:
				// それ以外では、「全件」と「最新の20件」を削除。
				pSubMenu->EnableMenuItem( ID_GET_ALL, MF_GRAYED | MF_BYCOMMAND );
				pSubMenu->EnableMenuItem( ID_GET_LAST10, MF_GRAYED | MF_BYCOMMAND );
			}

			// ログの有無チェック
			if( util::ExistFile( util::MakeLogfilePath(bodyItem) ) ) {
				pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_ENABLED | MF_BYCOMMAND );
			}else{
				pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_GRAYED | MF_BYCOMMAND );
			}

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_COMMUNITY:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(1);	// コミュニティ用メニューはidx=1

			// ログの有無チェック
			// コミュニティはトピック一覧のURLを用いる
			CMixiData mixi = bodyItem;
			CString url;
			url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
			mixi.SetURL(url);
			mixi.SetAccessType( ACCESS_LIST_BBS );
			if( util::ExistFile( util::MakeLogfilePath(mixi) ) ) {
				pSubMenu->EnableMenuItem( IDM_VIEW_BBS_LIST_LOG, MF_ENABLED | MF_BYCOMMAND );
			}else{
				pSubMenu->EnableMenuItem( IDM_VIEW_BBS_LIST_LOG, MF_GRAYED | MF_BYCOMMAND );
			}

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_PROFILE:
		// プロフィールなら、カテゴリ項目に応じて処理を変更する。（暫定）
		switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_INTRO:			// 紹介文
		case ACCESS_LIST_FAVORITE:		// お気に入り
		case ACCESS_LIST_FOOTSTEP:		// 足あと
		case ACCESS_LIST_FRIEND:		// マイミク一覧
		case ACCESS_LIST_CALENDAR:		// カレンダー
			// 操作をメニューで選択
			{
				CMenu menu;
				menu.LoadMenu( IDR_PROFILE_ITEM_MENU );
				CMenu* pSubMenu = menu.GetSubMenu(0);

				ACCESS_TYPE categoryType = m_selGroup->getSelectedCategory()->m_mixi.GetAccessType();

				// お気に入り以外では「自己紹介」を削除。
				if( categoryType != ACCESS_LIST_FAVORITE ) {
					pSubMenu->DeleteMenu( ID_OPEN_SELFINTRO, MF_BYCOMMAND );
				}

				// 紹介文以外では「紹介文」を削除
				if( categoryType != ACCESS_LIST_INTRO ) {
					pSubMenu->DeleteMenu( ID_OPEN_INTRO, MF_BYCOMMAND );
				}

				// ログの有無チェック
				if( util::ExistFile( util::MakeLogfilePath(bodyItem) ) ) {
					pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_ENABLED | MF_BYCOMMAND );
				}else{
					pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_GRAYED | MF_BYCOMMAND );
				}

				// メニューを開く
				pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
			}

			break;
		}
		break;

	case ACCESS_TWITTER_USER:
		{
			CMenu menu;
			menu.LoadMenu( IDR_TWITTER_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(0);	// メニューはidx=0

			// リンク
			int n = (int)bodyItem.m_linkList.size();
			if( n > 0 ) {
				pSubMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
				for( int i=0; i<n; i++ ) {
					// 追加
					CString s;
					s.Format( L"link : %s", bodyItem.m_linkList[i].text );
					pSubMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
				}
			}

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	}
	return true;
}

/// トピック一覧の閲覧準備
bool CMZ3View::PrepareViewBbsList(void)
{
	CMixiData& bodyItem = GetSelectedBodyItem();
	if (bodyItem.GetAccessType() == ACCESS_INVALID) {
		return false;
	}

	// URL はボディのアイテムからidを引き継ぐ。
	CString url;
	url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
	// 名前は分かるようにしておく
	CString name;
	name.Format( L"└%s", bodyItem.GetName() );
	CCategoryItem categoryItem;
	categoryItem.init( name, url, ACCESS_LIST_BBS, m_selGroup->categories.size(),
		CCategoryItem::BODY_INDICATE_TYPE_TITLE,
		CCategoryItem::BODY_INDICATE_TYPE_DATE,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );

	return AppendCategoryList(categoryItem);
}

/// コミュニティの右ソフトキーメニュー｜トピック一覧
void CMZ3View::OnViewBbsList()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// トピック一覧の閲覧準備
	PrepareViewBbsList();

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

/// コミュニティの右ソフトキーメニュー｜トピック一覧（ログ）
void CMZ3View::OnViewBbsListLog()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// トピック一覧の閲覧準備
	PrepareViewBbsList();

	// ログから取得する
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	MyLoadCategoryLogfile( *pCategory );

	// ボディリストに設定
	SetBodyList( pCategory->GetBodyList() );

	// フォーカスをボディリストに。
	m_bodyList.SetFocus();
}

/**
 * 巡回
 */
void CMZ3View::OnCruise()
{
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	CMixiData& mixi = pCategory->m_mixi;

	CString msg;
	msg.Format( 
		L"巡回を開始します。よろしいですか？\n"
		L"\n"
		L"巡回対象：%s", pCategory->m_name );
	if( MessageBox( msg, 0, MB_YESNO ) != IDYES ) {
		return;
	}

	// 巡回開始
	m_cruise.start();
	m_cruise.autoCruise = false;
	m_cruise.unreadOnly = false;

	// 「選択中のカテゴリ」を（フォーカスカテゴリで）変更しておく。
	m_selGroup->selectedCategory = m_selGroup->focusedCategory;
	OnMySelchangedCategoryList();

	AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()));
}

/**
 * Readme.txt を解析し、レポート画面で表示する。
 */
void CMZ3View::MyShowHelp(void)
{
	// *** Readme.txt を解析する ***

	// HTML の取得
	CHtmlArray html;
	html.Load( theApp.m_filepath.helpfile );

	// HTML 解析
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_HELP );
	mixi.SetTitle(MZ3_APP_NAME L" ヘルプ");
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"完了" );

	// *** 解析結果を表示する ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * ChangeLog.txt を解析し、レポート画面で表示する。
 */
void CMZ3View::MyShowHistory(void)
{
	// *** ChangeLog.txt を解析する ***

	// HTML の取得
	CHtmlArray html;
	html.Load( theApp.m_filepath.historyfile );

	// HTML 解析
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_HELP );
	mixi.SetTitle(MZ3_APP_NAME L" 改版履歴");
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"完了" );

	// *** 解析結果を表示する ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * mz3log.txt を解析し、レポート画面で表示する。
 */
void CMZ3View::MyShowErrorlog(void)
{
	// ログを閉じる
	theApp.m_logger.finish();

	// HTML の取得
	CHtmlArray html;
	html.Load( theApp.m_filepath.mz3logfile );

	// ログを開く
	theApp.m_logger.init( theApp.m_filepath.mz3logfile );

	// HTML 解析
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_ERRORLOG );
	mixi.SetTitle(MZ3_APP_NAME L" エラーログ");
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"完了" );

	// *** 解析結果を表示する ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * 巡回予約
 */
void CMZ3View::OnCheckCruise()
{
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	CMixiData& mixi = pCategory->m_mixi;

	CString msg;
	if( pCategory->m_bCruise ) {
		// 巡回対象になっているので巡回予約の解除。
		msg.Format( 
			L"この項目を巡回対象から解除します。よろしいですか？\n"
			L"\n"
			L"項目名：%s"
			, pCategory->m_name );
	}else{
		// 巡回対象ではないので巡回予約。
		msg.Format( 
			L"この項目を巡回対象にします。よろしいですか？\n"
			L"\n"
			L"項目名：%s\n"
			L"\n"
			L"★巡回はメニュー｜その他｜巡回から開始できます"
			, pCategory->m_name );
	}
	if( MessageBox( msg, 0, MB_YESNO ) != IDYES ) {
		return;
	}

	// YES なので予約設定/解除を実行する
	pCategory->m_bCruise = !pCategory->m_bCruise;

	// グループ定義ファイルの保存
	theApp.SaveGroupData();
}

bool CMZ3View::CruiseToNextCategory(void)
{
	// 次の巡回カテゴリを探索する
	if( !MoveToNextCruiseCategory() ) {
		// 巡回完了
		m_cruise.finish();
		util::MySetInformationText( m_hWnd, L"巡回完了（予約巡回）" );

		// カテゴリリストにフォーカスを移動する
		CommandSetFocusCategoryList();
		return false;
	}

	// リスト巡回モード
	m_cruise.backToListCruise();

	// 巡回する
	if( m_groupTab.GetCurSel() != m_cruise.targetGroupIndex ) {
		// グループが変化したので、「選択中のグループ」を変更する。
		m_selGroup = &theApp.m_root.groups[ m_cruise.targetGroupIndex ];
		m_groupTab.SetCurSel( m_cruise.targetGroupIndex );
		OnSelchangedGroupTab();
	}

	// 「選択中のカテゴリ」を変更する。
	util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
	m_selGroup->selectedCategory = m_selGroup->focusedCategory = m_cruise.targetCategoryIndex;
	util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
	CommandSetFocusCategoryList();
	OnMySelchangedCategoryList();

	// フォーカスを上ペインに。

	// 巡回対象カテゴリの解決
	CCategoryItem& targetCategory = theApp.m_root.groups[ m_cruise.targetGroupIndex ].categories[ m_cruise.targetCategoryIndex ];
	CMixiData& mixi = targetCategory.m_mixi;

	AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

	return true;
}

/**
 * 巡回開始
 */
void CMZ3View::StartCruise( bool unreadOnly )
{
	m_cruise.unreadOnly = unreadOnly;

	if( MessageBox( L"巡回を開始します。よろしいですか？", NULL, MB_YESNO ) != IDYES ) {
		return;
	}

	// 巡回対象カテゴリ探索
	// targetGroupIndex, targetCategoryIndex から巡回対象項目を探索する
	m_cruise.start();
	m_cruise.autoCruise = true;

	CruiseToNextCategory();
}

/**
 * 次の巡回項目を探索する
 *
 * @return true：巡回項目発見<br>
 *         false：巡回項目なし
 */
bool CMZ3View::MoveToNextCruiseCategory(void)
{
	std::vector<CGroupItem>& groups = theApp.m_root.groups;
	for( ; m_cruise.targetGroupIndex<(int)groups.size(); m_cruise.targetGroupIndex++ ) {

		CGroupItem& group = groups[m_cruise.targetGroupIndex];
		CCategoryItemList& categories = group.categories;
		for( ; m_cruise.targetCategoryIndex<(int)categories.size(); m_cruise.targetCategoryIndex++ ) {
			if( categories[ m_cruise.targetCategoryIndex ].m_bCruise ) {
				// 発見
				return true;
			}
		}
		m_cruise.targetCategoryIndex = 0;
	}
	MessageBox( L"巡回終了" );
	return false;
}

/**
 * メッセージを送る
 */
void CMZ3View::OnSendNewMessage()
{
	static CMixiData mixi;
	mixi = GetSelectedBodyItem();

	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWMESSAGE, &mixi );
}

int CMZ3View::GetListWidth(void)
{
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();

	// ピクセル数の微調整（スクリーン幅より少し小さくする）
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 35;
		break;
	case SR_QVGA:
	default:
		w -= 35/2;
		break;
	}
#else
	w -= 35;
#endif
	return w;
}

/**
 * カラムサイズ（幅）を再設定する。
 */
void CMZ3View::ResetColumnWidth()
{
	// 要素種別が「ヘルプ」なら日時を表示しない。

	// 幅の定義
	int w = GetListWidth();

	// カテゴリリストは 25:20 の比率で分割する
	if( m_categoryList.m_hWnd != NULL ) {
		const int W_COL1 = theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio;
		const int W_COL2 = theApp.m_optionMng.m_nMainViewCategoryListCol2Ratio;
		m_categoryList.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2) );
		m_categoryList.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2) );
	}

	// ボディリストは 24:21 の比率で分割する
	if( m_bodyList.m_hWnd != NULL ) {
		const int W_COL1 = theApp.m_optionMng.m_nMainViewBodyListCol1Ratio;
		const int W_COL2 = theApp.m_optionMng.m_nMainViewBodyListCol2Ratio;
		m_bodyList.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2) );
		m_bodyList.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2) );
	}
}

/**
 * ヘッダのドラッグ終了
 *
 * カラム幅の再構築を行う。
 */
void CMZ3View::OnHdnEndtrackHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MZ3LOGGER_DEBUG( L"OnHdnEndtrackHeaderList" );

	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// カラム幅の反映
	m_bodyList.SetColumnWidth( phdr->iItem, phdr->pitem->cxy );

	CRect rect;
	CHeaderCtrl* pHeader = NULL;

	// ボディリストヘッダの取得
	if( (pHeader = m_bodyList.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"ボディリストのヘッダを取得できないので終了" );
		return;
	}

	// カラム１
	if(! pHeader->GetItemRect( 0, rect ) ) {
		MZ3LOGGER_ERROR( L"ボディリストのヘッダ、第1カラムの幅を取得できないので終了" );
		return;
	}
	theApp.m_optionMng.m_nMainViewBodyListCol1Ratio = rect.Width();

	// カラム２
	// 最終カラムなので、リスト幅-他のカラムサイズとする。
	theApp.m_optionMng.m_nMainViewBodyListCol2Ratio
		= GetListWidth() - theApp.m_optionMng.m_nMainViewBodyListCol1Ratio;

	// カテゴリリスト

	// カテゴリリストの幅はドラッグで変更できないので、保存しない。
/*	if( (pHeader = m_categoryList.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"カテゴリリストのヘッダ が NULL なので終了" );
		return;
	}

	// カラム１
	if(! pHeader->GetItemRect( 0, rect ) ) return;
	theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio = rect.Width();

	// カラム２
	// 最終カラムなので、リスト幅-他のカラムサイズとする。
	theApp.m_optionMng.m_nMainViewCategoryListCol2Ratio
		= GetListWidth() - theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio;
*/

	*pResult = 0;
}

/**
 *
 * @return 巡回継続の場合は true、巡回終了の場合は false を返す
 */
bool CMZ3View::DoNextBodyItemCruise()
{
	CMixiDataList& bodyList = m_selGroup->getSelectedCategory()->GetBodyList();
	if( m_cruise.targetBodyItem >= (int)bodyList.size() ) {
		// 巡回終了
		if( m_cruise.autoCruise ) {
			// 予約巡回なので次に進む
			m_cruise.targetCategoryIndex++;
			bool rval = CruiseToNextCategory();
			if( rval ) {
				// 通信終了

				// 項目を選択/表示状態にする
				int idx = m_selGroup->getSelectedCategory()->selectedBody;
				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idx, true );
				m_bodyList.EnsureVisible( idx, FALSE );
			}
			return rval;
		}else{
			// 一時巡回なのでここで終了。
			m_cruise.finish();
			MessageBox( L"クイック巡回完了" );

			// 通信終了
			return false;
		}
	}else{
		// 次の巡回対象を取得する

		// 次のボディ要素に移動する

		m_selGroup->getSelectedCategory()->selectedBody = m_cruise.targetBodyItem;

		CMixiData& mixi = GetSelectedBodyItem();

		// 未読巡回モードなら、既読要素をスキップする
		if( m_cruise.unreadOnly ) {

			// 未読巡回モードなので、次の未読要素を探索する。
			// 全て既読なら次のカテゴリへ。
			bool unread = false;	// 未読フラグ
			switch( mixi.GetAccessType() ) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_ENQUETE:
				// コミュニティ、イベント、アンケートなので、
				// 該当トピックのコメントを全て既読なら既読と判定する。
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(mixi);
					if (lastIndex == -1) {
						// 全くの未読
						unread = true;
					} else if (lastIndex >= mixi.GetCommentCount()) {
						// 更新なし
						unread = false;
					} else {
						// 未読あり
						unread = true;
					}
				}
				break;
			default:
				// コミュニティ以外なので、ログの有無で既読・未読を判定する
				if(! util::ExistFile(util::MakeLogfilePath(mixi)) ) {
					unread = true;
				}
				break;
			}
			if(! unread ) {
				// 既読なので次のボディ要素へ。
				m_cruise.targetBodyItem ++;

				// 再帰呼び出しにより、次の巡回項目を探す。
				return DoNextBodyItemCruise();
			}
		}

		// 項目を選択/表示状態にする
		int idxNext = m_cruise.targetBodyItem;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idxNext, true );
		m_bodyList.EnsureVisible( idxNext, FALSE );

		// 取得する
		AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

		// 通信継続
		return true;
	}
}

void CMZ3View::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFormView::OnSettingChange(uFlags, lpszSection);

/*	MZ3LOGGER_DEBUG( L"OnSettingChange( " + util::int2str(uFlags) + L", " + lpszSection + L" )" );

	if( uFlags & SETTINGCHANGE_RESET ) {
	}
*/
}

static const int N_HC_MIN = 10;		///< カテゴリリストの最小値 [%]
static const int N_HC_MAX = 90;		///< カテゴリリストの最大値 [%]
static const int N_HB_MIN = 10;		///< ボディリストの最小値 [%]
static const int N_HB_MAX = 90;		///< ボディリストの最大値 [%]

/**
 * カテゴリリストを狭くする
 */
void CMZ3View::OnLayoutCategoryMakeNarrow()
{
	int& hc = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	int& hb = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

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
 * カテゴリリストを広くする
 */
void CMZ3View::OnLayoutCategoryMakeWide()
{
	int& hc = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	int& hb = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

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
 * カテゴリリストの右クリックイベント
 */
void CMZ3View::OnNMRclickHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// カテゴリリストでの右クリック
	PopupCategoryMenu();

	*pResult = 0;
}

/**
 * カテゴリリストのポップアップメニュー
 */
void CMZ3View::PopupCategoryMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// 巡回対象以外のカテゴリであれば巡回メニューを無効化する
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	if (pCategory != NULL) {
		switch( pCategory->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_NEW_BBS:
		case ACCESS_LIST_NEWS:
		case ACCESS_LIST_MESSAGE_IN:
		case ACCESS_LIST_MESSAGE_OUT:
		case ACCESS_LIST_DIARY:
		case ACCESS_LIST_MYDIARY:
		case ACCESS_LIST_BBS:
	//	case ACCESS_TWITTER_FRIENDS_TIMELINE:
			// 巡回対象なので巡回メニューを無効化しない
			break;
		default:
			// 巡回メニューを無効化する
			pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			break;
		}

		// 巡回予約済みであればチェックを付ける。
		if( pCategory->m_bCruise ) {
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
		}else{
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
		}
	} else {
		pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
	}

	// 項目を追加
	CMenu* pAppendMenu = pSubMenu->GetSubMenu(5);
	if (pAppendMenu) {
		// ダミーを削除
		pAppendMenu->RemoveMenu( ID_APPEND_MENU_BEGIN, MF_BYCOMMAND );

		Mz3GroupData template_data;
		template_data.initForTopPage();
		static CArray<CMenu, CMenu> subMenu;
		subMenu.RemoveAll();
		subMenu.SetSize( template_data.groups.size() );
		int menuId = ID_APPEND_MENU_BEGIN;
		for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
			subMenu[groupIdx].CreatePopupMenu();

			CGroupItem& group = template_data.groups[groupIdx];

			// subMenu にカテゴリ名を追加
			for (unsigned int ic=0; ic<group.categories.size(); ic++) {
				subMenu[groupIdx].AppendMenuW( MF_STRING, menuId, group.categories[ic].m_name );
				menuId ++;
			}

			pAppendMenu->AppendMenuW( MF_POPUP, (UINT)subMenu[groupIdx].m_hMenu, group.name );
		}
	}

	// 「項目を削除」の有効・無効
	if (pCategory != NULL) {
		pSubMenu->CheckMenuItem( ID_REMOVE_CATEGORY_ITEM, MF_BYCOMMAND | (pCategory->bSaveToGroupFile ? MF_UNCHECKED : MF_CHECKED) );
	} else {
		pSubMenu->RemoveMenu( ID_REMOVE_CATEGORY_ITEM, MF_BYCOMMAND );
	}

	// 「項目を変更」
	if (pCategory == NULL) {
		pSubMenu->RemoveMenu( ID_EDIT_CATEGORY_ITEM, MF_BYCOMMAND );
	}

	// メニュー表示
	pSubMenu->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * ボディリストの右クリックイベント
 */
void CMZ3View::OnNMRclickBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	PopupBodyMenu();
	*pResult = 0;
}

void CMZ3View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMERID_INTERVAL_CHECK) {
		// 定期取得機能
		if (theApp.m_optionMng.m_bEnableIntervalCheck) {
			// フォーカスチェック
			// 現在のアプリが MZ3：
			//  フォーカスがカテゴリリストにある場合
			// 現在のアプリが MZ3 以外：
			//  View が MZ3View である場合
			// というチェックをしたいけど、とりあえずViewのチェックのみ。
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			if (pMainFrame->GetActiveView() != theApp.m_pMainView) {
				// 現在のViewがMZ3View以外なので定期取得を行わない。
				
				// タイマーを更新（さらにN秒経つまで待つ）
				ResetIntervalTimer();
				return;
			}
/*
			if( GetFocus() != &m_categoryList ) {
				// フォーカスが違うので、タイマーを更新（さらにN秒経つまで待つ）
				ResetIntervalTimer();
				return;
			}
*/
			// タイマー開始から N 秒経過したか？
			int nElapsedSec = (GetTickCount() - m_dwIntervalTimerStartMsec)/1000;
			if( nElapsedSec >= theApp.m_optionMng.m_nIntervalCheckSec ) {
				util::MySetInformationText( m_hWnd, _T("☆定期取得を開始します") );

				// 経過。取得開始。
				RetrieveCategoryItem();

				// タイマーを更新
				ResetIntervalTimer();
			} else {
				// カウントダウン
				int restSec = theApp.m_optionMng.m_nIntervalCheckSec - nElapsedSec;
				switch( restSec ) {
				case 1:
					util::MySetInformationText( m_hWnd, _T("☆定期取得1秒前") );
					break;
				case 2:
					util::MySetInformationText( m_hWnd, _T("☆定期取得2秒前") );
					break;
				case 3:
					util::MySetInformationText( m_hWnd, _T("☆定期取得3秒前") );
					break;
				}
			}
		}
		return;
	}

	CFormView::OnTimer(nIDEvent);
}

bool CMZ3View::RetrieveCategoryItem(void)
{
	// アクセス中は再アクセス不可
	if (m_access) {
		return false;
	}
	if (m_selGroup==NULL) {
		return false;
	}
	CCategoryItem* item = m_selGroup->getSelectedCategory();
	if (item==NULL) {
		return false;
	}
	if (item->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
		// ブックマークはアクセスなし
		SetBodyList( item->GetBodyList() );
	} else {
		// インターネットにアクセス
		m_hotList = &m_bodyList;
		AccessProc( &item->m_mixi, util::CreateMixiUrl(item->m_mixi.GetURL()));
	}

	return true;
}

/**
 * タイマーを更新（さらにN秒経つまで待つ）
 */
void CMZ3View::ResetIntervalTimer(void)
{
	m_dwIntervalTimerStartMsec = GetTickCount();
}

BOOL CMZ3View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// メッセージに応じて、定期取得のキャンセル処理を行う
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		int id = wParam;
		switch( id ) {
		case IDC_HEADER_LIST:
		case IDC_BODY_LIST:
			switch( pnmhdr->code ) {
			case NM_RCLICK:
			case NM_CLICK:
			case NM_DBLCLK:
//			case LVN_ITEMCHANGED:
				ResetIntervalTimer();
				break;
			}
			break;
		case IDC_GROUP_TAB:
			switch(pnmhdr->code ) {
			case TCN_SELCHANGE:
				ResetIntervalTimer();
				break;
			}
			break;
		}
	}

	return CFormView::OnNotify(wParam, lParam, pResult);
}

/**
 * タブのクリック
 */
void CMZ3View::OnNMClickGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 仮想的にダブルクリックを判定する。
	static int s_idxLast = -1;
	static DWORD s_dwLastClickedTickCount = GetTickCount();

	int idx = m_groupTab.GetCurSel();
	if (s_idxLast != idx) {
		s_idxLast = idx;
		s_dwLastClickedTickCount = GetTickCount();
	} else {

		// しきい値をシステムから取得し、ダブルクリック判定
		if (GetTickCount() - s_dwLastClickedTickCount < GetDoubleClickTime()) {
			s_idxLast = -1;

			// ダブルクリック時の処理を実行：
			// 選択カテゴリのリロード
			RetrieveCategoryItem();
		} else {
			s_dwLastClickedTickCount = GetTickCount();
		}
	}

	*pResult = 0;
}

/**
 * mini画像ウィンドウの移動（および消去）
 */
void CMZ3View::MoveMiniImageDlg(int idxBody/*=-1*/, int pointx/*=-1*/, int pointy/*=-1*/)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return;
	}

	// mini画像画面制御
	bool bDrawMiniImage = false;
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			// mini画像が未ロードであれば取得する
			if (idxBody<0 || idxBody>=(int)pCategory->m_body.size()) {
				idxBody = pCategory->selectedBody;
			}
			if (!pCategory->m_body.empty()) {
				const CMixiData& data = pCategory->m_body[ idxBody ];
				MyLoadMiniImage( data );

				// プロフィール or コミュニティで、
				// かつ画像があれば表示

				CString path = util::MakeImageLogfilePath( data );
				if (!path.IsEmpty() ) {
					bDrawMiniImage = true;
				}
			}
		}
	}

	if (m_pMiniImageDlg != NULL) {
		if (!theApp.m_optionMng.m_bShowMainViewMiniImageDlg) {
			// オプションがOffなので、常に非表示
			m_pMiniImageDlg->ShowWindow( SW_HIDE );
		} else {
			m_pMiniImageDlg->ShowWindow( bDrawMiniImage ? SW_SHOWNOACTIVATE : SW_HIDE );

			if (bDrawMiniImage) {
				CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
				int idx = idxBody;
			
				CRect rectBodyList; 
				m_bodyList.GetWindowRect( &rectBodyList );

				CRect rect;
				m_bodyList.GetItemRect( idx, &rect, LVIR_BOUNDS );

				rect.OffsetRect( rectBodyList.left, rectBodyList.top );

				// オプションで指定
				const int w = theApp.m_optionMng.m_nMainViewMiniImageSize;
				const int h = theApp.m_optionMng.m_nMainViewMiniImageSize;

				// とりあえず行の直下に描画。
				int delta = 5;
				int x = 0;
				if (pointx>=0) {
					x = rectBodyList.left +pointx +delta;
					if (x+w > rectBodyList.right) {
						// ボディリストからはみ出すので左側に描画。
						x = x -w -delta -delta;
					}
				} else {
					x = rect.left+32;
				}

				int y = 0;
				if (pointy>=0) {
					y = rectBodyList.top +pointy +delta;
					if (y+h > rectBodyList.bottom) {
						// ボディリストからはみ出すので上側に描画。
						y = rectBodyList.top +pointy -h -delta;
					}
				} else {
					y = rect.bottom;
					if (y+h > rectBodyList.bottom) {
						// ボディリストからはみ出すので上側に描画。
						y = rect.top -h;
					}
				}

				// それでもはみだす場合（スクロール時など）は非表示
				if (y+h > rectBodyList.bottom || y<rectBodyList.top) {
					m_pMiniImageDlg->ShowWindow( SW_HIDE );
				} else {
					m_pMiniImageDlg->MoveWindow( x, y, w, h );
				}
			}
		}
	}
}

LRESULT CMZ3View::OnHideView(WPARAM wParam, LPARAM lParam)
{
	// 画像ウィンドウの消去
	if (m_pMiniImageDlg != NULL) {
		m_pMiniImageDlg->ShowWindow( SW_HIDE );
	}

	return TRUE;
}

bool CMZ3View::MyLoadMiniImage(const CMixiData& mixi)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return false;
	}

	if (!theApp.m_optionMng.m_bAutoLoadMiniImage) {
		return false;
	}

	CString miniImagePath = util::MakeImageLogfilePath( mixi );
	if (!miniImagePath.IsEmpty()) {
		if (!util::ExistFile(miniImagePath)) {
			if(! m_access ) {
				// アクセス中は禁止
				// 取得
				static CMixiData s_data;
				CMixiData dummy;
				s_data = dummy;
				s_data.SetAccessType( ACCESS_IMAGE );

				CString url = mixi.GetImage(0);

				// 中止ボタンを使用可にする
				theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

				// アクセス種別を設定
				theApp.m_accessType = s_data.GetAccessType();

				// アクセス開始
				m_access = TRUE;
				m_abort = FALSE;

				theApp.m_inet.Initialize( m_hWnd, &s_data );
				theApp.m_inet.DoGet(url, L"", CInetAccess::FILE_BINARY );
			}
		} else {
			// すでに存在するので描画
			if (m_pMiniImageDlg!=NULL) {
				m_pMiniImageDlg->DrawImageFile( miniImagePath );
			}
		}
	}

	return true;
}

/**
 * フォント拡大
 */
void CMZ3View::OnAcceleratorFontMagnify()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight+1 );

	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * フォント縮小
 */
void CMZ3View::OnAcceleratorFontShrink()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight-1 );

	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * コンテキストメニュー表示
 */
void CMZ3View::OnAcceleratorContextMenu()
{
	if( GetFocus() == &m_bodyList ) {
		// ボディリストでの右クリックメニュー
		PopupBodyMenu();
	}else{
		// カテゴリリストでの右クリック
		PopupCategoryMenu();
	}
}

/**
 * 次のタブ
 */
void CMZ3View::OnAcceleratorNextTab()
{
	CommandSelectGroupTabNextItem();
}

/**
 * 前のタブ
 */
void CMZ3View::OnAcceleratorPrevTab()
{
	CommandSelectGroupTabBeforeItem();
}

/**
 * ホイール
 */
BOOL CMZ3View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags & MK_CONTROL) {
		// Ctrl+ホイールで拡大・縮小
		// 高解像度ホイール対応のため、delta 値を累積する。
		static int s_delta = 0;
		// 方向が逆になったらキャンセル
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>WHEEL_DELTA) {
			OnAcceleratorFontMagnify();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<-WHEEL_DELTA) {
			OnAcceleratorFontShrink();
			s_delta += WHEEL_DELTA;
		}
		return TRUE;
	}

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * リロード (F5)
 */
void CMZ3View::OnAcceleratorReload()
{
	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	// アクセス開始
	if (!RetrieveCategoryItem()) {
		return;
	}
}

/**
 * Twitter | 全文を読む
 */
void CMZ3View::OnMenuTwitterRead()
{
	CMixiData& data = GetSelectedBodyItem();

	// 本文を1行に変換して割り当て。
	CString item;
	for( u_int i=0; i<data.GetBodySize(); i++ ) {
		CString line = data.GetBody(i);
		while( line.Replace( L"\r\n", L"" ) );
		item.Append( line );
	}

	item.Append( L"\r\n" );
	item.Append( L"----\r\n" );
	item.AppendFormat( L"name : %s\r\n", data.GetAuthor() );
	item.AppendFormat( L"description : %s\r\n", data.GetTitle() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );
	item.AppendFormat( L"id : %d\r\n", data.GetID() );
	item.AppendFormat( L"owner-id : %d\r\n", data.GetOwnerID() );

	if (data.GetChildrenSize()>=1) {
		CString source = data.GetChild(0).GetBody(0);
		mixi::ParserUtil::StripAllTags( source );
		item.AppendFormat( L"source : %s", source );
	}

	MessageBox( item, data.GetName() );
}

/**
 * Twitter | 言い返す
 */
void CMZ3View::OnMenuTwitterReply()
{
	// 入力領域にユーザのスクリーン名を追加。
	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	CMixiData& data = GetSelectedBodyItem();
	strStatus.AppendFormat( L"@%s ", (LPCTSTR)data.GetName() );

	SetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// End
	keybd_event( VK_END, 0, 0, 0 );
	keybd_event( VK_END, 0, KEYEVENTF_KEYUP, 0 );
}

/**
 * Twitter | つぶやく
 */
void CMZ3View::OnMenuTwitterUpdate()
{
	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

/**
 * Twitter | ホーム
 */
void CMZ3View::OnMenuTwitterHome()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( util::FormatString(L"http://twitter.com/%s", data.GetName()) );
}

/**
 * Twitter | Favorites
 */
void CMZ3View::OnMenuTwitterFavorites()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( util::FormatString(L"http://twitter.com/%s/favorites", data.GetName()) );
}

/**
 * Twitter | サイト
 */
void CMZ3View::OnMenuTwitterSite()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( data.GetURL() );
}

CMZ3View::VIEW_STYLE CMZ3View::MyGetViewStyleForSelectedCategory(void)
{
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			switch (pCategory->m_mixi.GetAccessType()) {
			case ACCESS_TWITTER_FRIENDS_TIMELINE:
				return VIEW_STYLE_TWITTER;
			default:
				if (m_bodyList.IsEnableIcon()) {
					CImageList* pImageList = m_bodyList.GetImageList(LVSIL_SMALL);
					if (pImageList != NULL &&
						pImageList->m_hImageList == theApp.m_imageCache.GetImageList().m_hImageList)
					{
						return VIEW_STYLE_IMAGE;
					}
				}
				break;
			}
		}
	}
	return VIEW_STYLE_DEFAULT;
}

/**
 * Twitter, 更新
 */
void CMZ3View::OnBnClickedUpdateButton()
{
	if (m_access) {
		return;
	}

	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	if (strStatus.IsEmpty()) {
		return;
	}

	static CPostData post;
	post.ClearPostBody();

	// ヘッダーを設定
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client: %s", MZ3_APP_NAME ) );
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-URL: %s", L"http://mz3.jp/" ) );
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-Version: %s", MZ3_VERSION_TEXT_SHORT ) );

	// POST パラメータを設定
	post.AppendPostBody( "status=" );
	post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
	if (theApp.m_optionMng.m_bAddSourceTextOnTwitterPost) {
		post.AppendPostBody( L" *" MZ3_APP_NAME L"*" );
	}
	post.AppendPostBody( "&source=" );
	post.AppendPostBody( MZ3_APP_NAME );

	post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

	post.SetSuccessMessage( WM_MZ3_POST_END );

	CString url = L"http://twitter.com/statuses/update.xml";

	// Twitter API => Basic 認証
	LPCTSTR szUser = NULL;
	LPCTSTR szPassword = NULL;
	szUser     = theApp.m_loginMng.GetTwitterId();
	szPassword = theApp.m_loginMng.GetTwitterPassword();

	// 未指定の場合はエラー出力
	if (wcslen(szUser)==0 || wcslen(szPassword)==0) {
		MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
		return;
	}

	// 中止ボタンを使用可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// アクセス種別を設定
	theApp.m_accessType = ACCESS_TWITTER_UPDATE;

	// アクセス開始
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		L"", 
		CInetAccess::FILE_HTML, 
		&post, szUser, szPassword );

//	CPostData::post_array& buf = post.GetPostBody();
//	MessageBox( CStringW(&buf[0], buf.size()) );
}

/**
 * アクセス終了通知受信 (POST)
 */
LRESULT CMZ3View::OnPostEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	// 通信完了（フラグを下げる）
	m_access = FALSE;

	// 現状はTwitterのみ対応。
	// HTTPステータスチェックを行う。
	LPCTSTR szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
	if (szStatusErrorMessage!=NULL) {
		CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
		util::MySetInformationText( m_hWnd, msg );
		MZ3LOGGER_ERROR( msg );
	} else {
		util::MySetInformationText( m_hWnd, L"ステータス送信終了" );

		// 入力値を消去
		SetDlgItemText( IDC_STATUS_EDIT, L"" );
	}

	// フォーカスを入力領域に移動
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	return TRUE;
}

/**
 * URL
 */
void CMZ3View::OnLoadUrl(UINT nID)
{
	const CMixiData& data = GetSelectedBodyItem();

	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( idx > data.m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = data.m_linkList[idx].url;
	LPCTSTR text = data.m_linkList[idx].text;

	// 確認画面
	util::OpenBrowserForUrl( url );
}

void CMZ3View::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// 描画メッセージで CFormView::OnPaint() を呼び出さないでください。

	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		break;
	case VIEW_STYLE_IMAGE:
	case VIEW_STYLE_TWITTER:
		// アイコン描画
		if (m_rectIcon.Width()>0 && m_rectIcon.Height()>0) {

			bool bDrawFinished = false;

			const CMixiData& data = GetSelectedBodyItem();
			CString path = util::MakeImageLogfilePath( data );
			if (!path.IsEmpty() ) {
				// 情報領域の左側に描画する。
				const CRect& rectIcon = m_rectIcon;

				CMZ3BackgroundImage image(L"");
				image.load( path );
				if (image.isEnableImage()) {
					// リサイズする。
					CMZ3BackgroundImage resizedImage(L"");
					util::MakeResizedImage( this, resizedImage, image, rectIcon.Width(), rectIcon.Height() );

					util::DrawBitmap( dc.GetSafeHdc(), resizedImage.getHandle(), 
						rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height(), 0, 0 );

					bDrawFinished = true;
				}
			}

			if (!bDrawFinished) {
				// 塗りつぶす
				dc.FillSolidRect( m_rectIcon, RGB(255,255,255) );
			}
		}
		break;
	}
}

void CMZ3View::OnMenuTwitterFriendTimeline()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// タイムライン項目の追加
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	categoryItem.init( 
		// 名前
		util::FormatString( L"+%sのタイムライン", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/user_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		CCategoryItem::BODY_INDICATE_TYPE_BODY,
		CCategoryItem::BODY_INDICATE_TYPE_NAME,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

void CMZ3View::OnMenuTwitterFriendTimelineWithOthers()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// タイムライン項目の追加
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	categoryItem.init( 
		// 名前
		util::FormatString( L"+%sのタイムライン", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/friends_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		CCategoryItem::BODY_INDICATE_TYPE_BODY,
		CCategoryItem::BODY_INDICATE_TYPE_NAME,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

bool CMZ3View::AppendCategoryList(const CCategoryItem& categoryItem)
{
	CString url = categoryItem.m_mixi.GetURL();

	// グループ(タブ)にタイムライン専用項目を追加する。既にあれば取得する。
	CCategoryItem* pCategoryItem = NULL;
	for( u_int i=0; i<m_selGroup->categories.size(); i++ ) {
		CCategoryItem& category = m_selGroup->categories[i];
		if( category.m_mixi.GetAccessType() == ACCESS_TWITTER_FRIENDS_TIMELINE &&
			category.m_mixi.GetURL()==url)
		{
			// 該当項目発見。
			pCategoryItem = &category;
			break;
		}
	}
	// 未発見なら追加
	if( pCategoryItem == NULL ) {
		m_selGroup->categories.push_back( categoryItem );
		pCategoryItem = &m_selGroup->categories[ m_selGroup->categories.size()-1 ];
		pCategoryItem->SetIndexOnList( m_selGroup->categories.size()-1 );
	}

//	*pCategoryItem = categoryItem;

	// タブの初期化
	MyUpdateCategoryListByGroupItem();

	// カテゴリの選択項目を再表示。
	{
		int idxLast = m_selGroup->focusedCategory;
		int idxNew  = pCategoryItem->GetIndexOnList();

		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxLast, false );
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxNew, true );
		m_selGroup->focusedCategory  = idxNew;
		m_selGroup->selectedCategory = idxNew;
		m_categoryList.SetActiveItem( idxNew );
		m_categoryList.Update( idxLast );
		m_categoryList.Update( idxNew );

		m_categoryList.EnsureVisible( idxNew, FALSE );
	}

	// フォーカスをカテゴリリストに。
	m_categoryList.SetFocus();

	// ボディリストは消去しておく。
	m_bodyList.DeleteAllItems();
	m_bodyList.SetRedraw(TRUE);
	m_bodyList.Invalidate( FALSE );

	return true;
}

/**
 * タブの右クリックメニュー
 */
void CMZ3View::OnNMRclickGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	PopupTabMenu();

	*pResult = 0;
}

/**
 * タブメニュー｜削除
 */
void CMZ3View::OnTabmenuDelete()
{
	if (m_selGroup->bSaveToGroupFile) {
		m_selGroup->bSaveToGroupFile = false;

		MessageBox( util::FormatString( L"[%s] タブは次回起動時に削除されます", (LPCTSTR)m_selGroup->name ) );
	} else {
		m_selGroup->bSaveToGroupFile = true;
	}

	// グループ定義ファイルの保存
	theApp.SaveGroupData();
}

/**
 * タブリストのポップアップメニュー
 */
bool CMZ3View::PopupTabMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMenu menu;
	menu.LoadMenu(IDR_TAB_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// 削除の有効・無効
	pSubMenu->CheckMenuItem( ID_TABMENU_DELETE, MF_BYCOMMAND | (m_selGroup->bSaveToGroupFile ? MF_UNCHECKED : MF_CHECKED) );

	// メニュー表示
	pSubMenu->TrackPopupMenu(flags, pt.x, pt.y, this);

	return true;
}

void CMZ3View::OnAppendCategoryMenu(UINT nID)
{
	int idx = nID - ID_APPEND_MENU_BEGIN;

	Mz3GroupData template_data;
	template_data.initForTopPage();

	int idxCounter = 0;
	for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
		CGroupItem& group = template_data.groups[groupIdx];

		for (unsigned int ic=0; ic<group.categories.size(); ic++) {
			if (idxCounter==idx) {
				// この項目を追加する
				CCategoryItem item = group.categories[ic];
				AppendCategoryList( item );

				// カテゴリリスト中の「現在選択されている項目」を更新
				m_hotList = &m_categoryList;
				m_selGroup->selectedCategory = m_selGroup->categories.size()-1;
				OnMySelchangedCategoryList();

				// グループ定義ファイルの保存
				theApp.SaveGroupData();
				return;
			}

			idxCounter ++;
		}
	}
}

/**
 * カテゴリメニュー｜項目を削除
 */
void CMZ3View::OnRemoveCategoryItem()
{
	CCategoryItem* pCategoryItem = m_selGroup->getFocusedCategory();

	if (pCategoryItem->bSaveToGroupFile) {
		pCategoryItem->bSaveToGroupFile = false;

		MessageBox( util::FormatString( L"[%s] は次回起動時に削除されます", (LPCTSTR)pCategoryItem->m_name ) );
	} else {
		pCategoryItem->bSaveToGroupFile = true;
	}

	// グループ定義ファイルの保存
	theApp.SaveGroupData();
}

/**
 * カテゴリメニュー｜変更
 */
void CMZ3View::OnEditCategoryItem()
{
	CCategoryItem* pCategoryItem = m_selGroup->getFocusedCategory();
	
	CCommonEditDlg dlg;
	dlg.SetTitle( L"カテゴリのタイトル変更" );
	dlg.SetMessage( L"カテゴリのタイトルを入力してください" );
	dlg.mc_strEdit = pCategoryItem->m_name;
	if (dlg.DoModal()==IDOK) {
		pCategoryItem->m_name = dlg.mc_strEdit;
		m_categoryList.SetItemText( m_selGroup->focusedCategory, 0, pCategoryItem->m_name );

		// グループ定義ファイルの保存
		theApp.SaveGroupData();
	}
}

/**
 * タブメニュー｜変更
 */
void CMZ3View::OnTabmenuEdit()
{
	CGroupItem* pGroupItem = m_selGroup;
	
	CCommonEditDlg dlg;
	dlg.SetTitle( L"タブのタイトル変更" );
	dlg.SetMessage( L"タブのタイトルを入力してください" );
	dlg.mc_strEdit = pGroupItem->name;
	if (dlg.DoModal()==IDOK) {
		pGroupItem->name = dlg.mc_strEdit;

		TCITEM tcItem;
		TCHAR buffer[256] = {0};
		tcItem.pszText = buffer;
		tcItem.cchTextMax = 256;
		tcItem.mask = TCIF_TEXT;
		wcsncpy( tcItem.pszText, pGroupItem->name, 255 );
		m_groupTab.SetItem(m_groupTab.GetCurSel(), &tcItem);

		// グループ定義ファイルの保存
		theApp.SaveGroupData();
	}
}

/**
 * タブメニュー｜追加
 */
void CMZ3View::OnTabmenuAdd()
{
	CCommonEditDlg dlg;
	dlg.SetTitle( L"タブの追加" );
	dlg.SetMessage( L"タブのタイトルを入力してください" );
	dlg.mc_strEdit = L"新しいタブ";
	if (dlg.DoModal()==IDOK) {
		// データ構造追加
		CGroupItem group;
		group.init( dlg.mc_strEdit, L"", ACCESS_GROUP_OTHERS );
		theApp.m_root.groups.push_back( group );

		// グループタブに追加
		int tabIndex = m_groupTab.GetItemCount();
		m_groupTab.InsertItem( tabIndex, dlg.mc_strEdit);

		// 選択変更
		m_groupTab.SetCurSel( tabIndex );

		// 選択中のグループ項目の設定
		m_selGroup = &theApp.m_root.groups[tabIndex];

		// カテゴリーリストを初期化する
		OnSelchangedGroupTab();

		// グループ定義ファイルの保存
		theApp.SaveGroupData();
	}
}
