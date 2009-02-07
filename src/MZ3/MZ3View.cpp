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

// UI関連
#include "MZ3View.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "CommonEditDlg.h"
#include "MouseGestureManager.h"
#include "OpenUrlDlg.h"
#include "ChooseAccessTypeDlg.h"

// ユーティリティ関連
#include "HtmlArray.h"
#include "MixiData.h"
#include "CategoryItem.h"
#include "util.h"
#include "util_gui.h"
#include "util_goo.h"
#include "url_encoder.h"
#include "twitter_util.h"
#include "MixiParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID_INTERVAL_CHECK	101
#define SPLITTER_HEIGHT			10

/// アクセス種別と表示種別から、ボディーリストのヘッダー文字列（１カラム目）を取得する
LPCTSTR MyGetBodyHeaderColName1( ACCESS_TYPE accessType )
{
	return theApp.m_accessTypeInfo.getBodyHeaderCol1Name(accessType);
}

/// アクセス種別と表示種別から、ボディーリストのヘッダー文字列（２カラム目）を取得する
LPCTSTR MyGetBodyHeaderColName2( ACCESS_TYPE accessType, AccessTypeInfo::BODY_INDICATE_TYPE bodyIndicateType )
{
	AccessTypeInfo::BODY_INDICATE_TYPE type2 = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(accessType);
	AccessTypeInfo::BODY_INDICATE_TYPE type3 = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(accessType);

	if (bodyIndicateType==type2) {
		return theApp.m_accessTypeInfo.getBodyHeaderCol2Name(accessType);
	} else if (bodyIndicateType==type3) {
		return theApp.m_accessTypeInfo.getBodyHeaderCol3Name(accessType);
	} else {
		// 未定義
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
	ON_COMMAND(ID_TABMENU_DELETE, &CMZ3View::OnTabmenuDelete)
	ON_COMMAND_RANGE(ID_APPEND_MENU_BEGIN, ID_APPEND_MENU_END, &CMZ3View::OnAppendCategoryMenu)
	ON_COMMAND(ID_REMOVE_CATEGORY_ITEM, &CMZ3View::OnRemoveCategoryItem)
	ON_COMMAND(ID_EDIT_CATEGORY_ITEM, &CMZ3View::OnEditCategoryItem)
	ON_COMMAND(ID_TABMENU_EDIT, &CMZ3View::OnTabmenuEdit)
	ON_COMMAND(ID_TABMENU_ADD, &CMZ3View::OnTabmenuAdd)
	ON_COMMAND(ID_MENU_TWITTER_NEW_DM, &CMZ3View::OnMenuTwitterNewDm)
	ON_COMMAND(ID_MENU_TWITTER_CREATE_FAVOURINGS, &CMZ3View::OnMenuTwitterCreateFavourings)
	ON_COMMAND(ID_MENU_TWITTER_DESTROY_FAVOURINGS, &CMZ3View::OnMenuTwitterDestroyFavourings)
	ON_COMMAND(ID_MENU_TWITTER_CREATE_FRIENDSHIPS, &CMZ3View::OnMenuTwitterCreateFriendships)
	ON_COMMAND(ID_MENU_TWITTER_DESTROY_FRIENDSHIPS, &CMZ3View::OnMenuTwitterDestroyFriendships)
	ON_COMMAND(ID_MENU_RSS_READ, &CMZ3View::OnMenuRssRead)
	ON_COMMAND(IDM_CATEGORY_OPEN, &CMZ3View::OnCategoryOpen)
	ON_COMMAND(ID_ADD_RSS_FEED_MENU, &CMZ3View::OnAddRssFeedMenu)
	ON_COMMAND(ID_MENU_MIXI_ECHO_READ, &CMZ3View::OnMenuMixiEchoRead)
	ON_COMMAND(ID_MENU_MIXI_ECHO_UPDATE, &CMZ3View::OnMenuMixiEchoUpdate)
	ON_COMMAND(ID_MENU_MIXI_ECHO_SHOW_PROFILE, &CMZ3View::OnMenuMixiEchoShowProfile)
	ON_COMMAND(ID_ACCELERATOR_TOGGLE_INTEGRATED_MODE, &CMZ3View::OnAcceleratorToggleIntegratedMode)
	ON_COMMAND(ID_MENU_WASSR_READ, &CMZ3View::OnMenuWassrRead)
	ON_COMMAND(ID_MENU_WASSR_UPDATE, &CMZ3View::OnMenuWassrUpdate)
	ON_COMMAND(ID_MENU_MIXI_ECHO_REPLY, &CMZ3View::OnMenuMixiEchoReply)
	ON_COMMAND(ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST, &CMZ3View::OnMenuMixiEchoAddRefUserEchoList)
	ON_COMMAND(ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST, &CMZ3View::OnMenuMixiEchoAddUserEchoList)
	ON_COMMAND_RANGE(ID_REPORT_COPY_URL_BASE+1, ID_REPORT_COPY_URL_BASE+50, OnCopyClipboardUrl)
	ON_COMMAND(ID_MENU_GOOHOME_READ, &CMZ3View::OnMenuGoohomeRead)
	ON_COMMAND(ID_MENU_GOOHOME_UPDATE, &CMZ3View::OnMenuGoohomeUpdate)
	ON_COMMAND(ID_MENU_GOOHOME_READ_COMMENTS, &CMZ3View::OnMenuGoohomeReadComments)
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
	, m_bReloadingGroupTabByThread(false)
	, m_bRetryReloadGroupTabByThread(false)
	, m_pCategorySubMenuList(NULL)
	, m_bImeCompositioning(false)
	, m_bDragging(false)
	, m_preCategory(0)
	, m_selGroup(NULL)
	, m_hotList(NULL)
	, m_bModifyingBodyList(false)
	, m_abort(FALSE)
{
}

/**
 * デストラクタ
 */
CMZ3View::~CMZ3View()
{
	// サブメニューの解放
	if (m_pCategorySubMenuList != NULL) {
		delete[] m_pCategorySubMenuList;
	}
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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

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
		m_categoryList.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
		m_categoryList.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

		// スタイルの更新
		m_bodyList.ModifyStyle(0, dwStyle);

		// アイコンリストの作成
		m_iconImageListSmall.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 6);
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_EVENT_JOIN_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_BIRTHDAY_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_SCHEDULE_ICON) );
		m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);

		m_iconImageListLarge.Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 6);
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_EVENT_JOIN_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_BIRTHDAY_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_SCHEDULE_ICON) );

		// カラム作成
		// いずれも初期化時に再設定するので仮の幅を指定しておく。
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_bodyList.InsertColumn(0, _T(""), LVCFMT_LEFT, 120*2, -1);
			m_bodyList.InsertColumn(1, _T(""), LVCFMT_LEFT, 105*2, -1);
			m_bodyList.InsertColumn(2, _T(""), LVCFMT_LEFT, 0, -1);
			break;
		case SR_QVGA:
		default:
			m_bodyList.InsertColumn(0, _T(""), LVCFMT_LEFT, 120, -1);
			m_bodyList.InsertColumn(1, _T(""), LVCFMT_LEFT, 105, -1);
			m_bodyList.InsertColumn(2, _T(""), LVCFMT_LEFT, 0, -1);
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

	// 初期化スレッド開始
	AfxBeginThread( Initialize_Thread, this );

	// キャッシュ削除スレッド開始
	AfxBeginThread( CacheCleanup_Thread, this );

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
	case ACCESS_EVENT_JOIN:	iconIndex = 3;	break;
	case ACCESS_BIRTHDAY:	iconIndex = 4;	break;
	case ACCESS_SCHEDULE:	iconIndex = 5;  break;
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
 * キャッシュ削除用スレッド
 */
unsigned int CMZ3View::CacheCleanup_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	::Sleep( 300L );

	// 古いキャッシュファイルの削除
	theApp.DeleteOldCacheFiles();

	return 0;
}

/**
 * 遅延初期化メソッド（初期化用スレッドから起動される）
 */
bool CMZ3View::DoInitialize()
{
	// 初期データ設定
	util::MySetInformationText( m_hWnd, L"画面を作成しています..." );
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
	if (theApp.m_optionMng.IsBootCheckMnC() != false) {
		// 新着メッセージ確認
		DoNewCommentCheck();
	}

	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	// Twitterスタイルであればカテゴリに応じて送信タイプを初期化
	if (m_viewStyle==VIEW_STYLE_TWITTER) {
		MyResetTwitterStylePostMode();

		// コントロール状態の変更
		MyUpdateControlStatus();
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
	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	if( fontHeight == 0 ) {
		fontHeight = 12;
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
		hInfo = (int)(hInfoBase * (1+0.8*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#else
		hInfo = (int)(hInfoBase * (1+0.5*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
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
		hInfo = (int)(hInfoBase * (1+0.8*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#else
		hInfo = (int)(hInfoBase * (1+0.5*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
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

	util::MoveDlgItemWindow( this, IDC_HEADER_LIST, 0, y, cx, hCategory+1 );
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

	// 統合カラムモード
	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// カラムヘッダは不要
		util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);

	} else {
		// ラベルぬっ殺しモードの場合はスタイルを変更すっぺよ
		if( theApp.m_optionMng.m_killPaneLabel ) {
			// カラムヘッダ除去
			util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);
		} else {
			// カラムヘッダ設定
			util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, LVS_NOCOLUMNHEADER, NULL);
		}
	}

	// 画像再描画
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

	if (lpnmlv->iItem<0) {
		return;
	}

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
	} else {
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
		// データがNULLの場合
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
			CString path = util::MakeImageLogfilePathFromUrlMD5( theApp.m_inet.GetURL() );
			CopyFile( theApp.m_filepath.temphtml, path, FALSE/*bFailIfExists, 上書き*/ );

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

	case ACCESS_DOWNLOAD:
		{
			// パス生成
			CString strFilepath;
			{
				CString url = theApp.m_inet.GetURL();

				strFilepath.Format(_T("%s\\%s"), 
					theApp.m_filepath.downloadFolder, 
					util::ExtractFilenameFromUrl( url, L"_mz3_noname.dat" ) );

				// コピー
				CopyFile( theApp.m_filepath.temphtml, strFilepath, FALSE/*bFailIfExists, 上書き*/ );
			}

			// ダウンロードの場合は、実行を確認する。
			if (theApp.m_optionMng.m_bUseRunConfirmDlg) {
				CString msg;
				msg.Format( 
					L"ダウンロードが完了しました！\n\n"
					L"ファイル %s を開きますか？", strFilepath );

				if (MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) == IDYES) {
					util::OpenByShellExecute(strFilepath);
				}
			} else {
				util::OpenByShellExecute(strFilepath);
			}
		}
		break;
	}

	// 通信完了（フラグを下げる）
	m_access = FALSE;

	// カテゴリに応じてTwitter送信モードを初期化
	MyResetTwitterStylePostMode();

	// コントロール状態の変更
	MyUpdateControlStatus();

	MZ3LOGGER_DEBUG(_T("OnGetEndBinary end"));

	return TRUE;
}

/**
 * アクセス終了通知受信
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("解析中") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	if (lParam == NULL) {
		// データがNULLの場合
		LPCTSTR msg = L"内部エラーが発生しました(LPARAM=NULL)";
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

			// ログイン実行
			m_access = TRUE;
			m_abort = FALSE;

			// コントロール状態の変更
			MyUpdateControlStatus();

			theApp.StartMixiLoginAccess(m_hWnd, data);

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

				// コントロール状態の変更
				MyUpdateControlStatus();

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
				CString msgTemp;

				if( theApp.m_newMessageCount > 0 ) {
					msgTemp.Format(_T(" 新着メッセージ:%d件 "),theApp.m_newMessageCount);
					msg = msgTemp;
				}

				if( theApp.m_newCommentCount > 0 ) {
					msgTemp.Format(_T(" 新着コメント:%d件 "),theApp.m_newCommentCount);
					msg = msg + msgTemp;
				}

				if( theApp.m_newApplyCount > 0 ) {
					msgTemp.Format(_T(" 承認待ち:%d人"),theApp.m_newApplyCount);
					msg = msg + msgTemp;
				}

				if( msg == "" ) {
					msg = _T("新着メッセージ、コメントはありません");
				}

				util::MySetInformationText( m_hWnd, msg );

				m_checkNewComment = false;
			} else {
				// 新着メッセージ以外なので、ログインのための取得だった。

				// データを待避データに戻す
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return TRUE;
			}
		}
		break;

	case ACCESS_RSS_READER_AUTO_DISCOVERY:
		// RSS AutoDiscovery を試みる
		DoAccessEndProcForRssAutoDiscovery();
		break;

	case ACCESS_SOFTWARE_UPDATE_CHECK:
		// バージョンチェック
		if (!DoAccessEndProcForSoftwareUpdateCheck()) {
			MessageBox(L"バージョンチェックに失敗しました。しばらくしてから再度お試し下さい。");
		}
		util::MySetInformationText( m_hWnd, L"完了" );
		break;

	default:
		switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
		case AccessTypeInfo::INFO_TYPE_CATEGORY:
			// --------------------------------------------------
			// カテゴリ項目の取得
			// --------------------------------------------------
			if (DoAccessEndProcForBody(aType)) {
				return TRUE;
			}
			break;

		case AccessTypeInfo::INFO_TYPE_BODY:
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
	}

	// 通信完了（フラグを下げる）
	m_access = FALSE;

	// カテゴリに応じてTwitter送信モードを初期化
	MyResetTwitterStylePostMode();

	// コントロール状態の変更
	MyUpdateControlStatus();

	// フォーカスの変更
	MyUpdateFocus();

	return TRUE;
}

/**
* アクセスエラー通知受信
*/
LRESULT CMZ3View::OnGetError(WPARAM wParam, LPARAM lParam)
{
	CMixiData* pMixi = (CMixiData*)lParam;

	ACCESS_TYPE aType = pMixi->GetAccessType();
	if (aType == ACCESS_LOGIN) {
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

	// カテゴリに応じてTwitter送信モードを初期化
	MyResetTwitterStylePostMode();

	// コントロール状態の変更
	MyUpdateControlStatus();

	// フォーカスの変更
	MyUpdateFocus();

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

	m_access = FALSE;

	// カテゴリに応じてTwitter送信モードを初期化
	MyResetTwitterStylePostMode();

	// コントロール状態の変更
	MyUpdateControlStatus();

	LPCTSTR msg = _T("中断しました");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// フォーカスの変更
	MyUpdateFocus();

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

	util::StopWatch sw_generate_icon, sw_set_icon;

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
	DWORD dwLoadAndResizeMS = 0;
	sw_generate_icon.start();
	if (theApp.m_optionMng.m_bShowMainViewMiniImage && !bUseDefaultIcon) {
		// デフォルトアイコンがなかったので、ユーザ・コミュニティアイコン等を作成する
		for (int i=0; i<count; i++) {
			// タブ切り替えが行われればキャンセル
			if (m_bReloadingGroupTabByThread && m_bRetryReloadGroupTabByThread) {
				return;
			}

			CString miniImagePath = util::MakeImageLogfilePath( body[i] );

			const CMixiData& mixi = body[i];
			if (theApp.m_imageCache.GetImageIndex(miniImagePath) >= 0) {
				// ロード済みなのでロード不要
				bUseExtendedIcon = true;
			} else {
				util::StopWatch sw_load_and_resize;
				sw_load_and_resize.start();
				// 未ロードなのでロード
				CMZ3BackgroundImage image(L"");
				image.load( miniImagePath );
				if (image.isEnableImage()) {
					// リサイズして画像キャッシュに追加する。
					theApp.AddImageToImageCache(this, image, miniImagePath);

					bUseExtendedIcon = true;
				} else {
					// ロードエラー => ダウンロードマネージャに登録する
					if (mixi.GetImageCount()>0 && !miniImagePath.IsEmpty()) {
						CString url = mixi.GetImage(0);
						DownloadItem item( url, L"絵文字", miniImagePath, true );
						theApp.m_pDownloadView->AppendDownloadItem( item );
					}
				}
				dwLoadAndResizeMS += sw_load_and_resize.getElapsedMilliSecUntilNow();
			}
		}
	}
	sw_generate_icon.stop();

	// 行の高さが乱れた状態で描画されるのを回避するため描画停止
	m_bodyList.m_bStopDraw = true;

	// アイコン表示・非表示設定
	CBodyListCtrl::ICON_MODE iconMode = CBodyListCtrl::ICON_MODE_NONE;
	if (bUseDefaultIcon) {
		// デフォルトアイコン
		// 32px のデフォルトアイコンができたら下記のコメントを外すこと。
//		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// 統合カラムモード
//			CRect rect;
//			m_bodyList.GetItemRect(0, rect, LVIR_BOUNDS);
//			if (rect.Height()>=32) {
//				m_bodyList.SetImageList(&m_iconImageListLarge, LVSIL_SMALL);
//				iconMode = CBodyListCtrl::ICON_MODE_32;
//			} else {
//				m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);
//				iconMode = CBodyListCtrl::ICON_MODE_16;
//			}
//		} else {
			m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);
			iconMode = CBodyListCtrl::ICON_MODE_16;
//		}
	} else if (bUseExtendedIcon) {
		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// 統合カラムモード
			MEASUREITEMSTRUCT measureItemStruct;
			m_bodyList.MeasureItem(&measureItemStruct);
			MZ3_TRACE(L"アイテムの高さ : %d\n", measureItemStruct.itemHeight);
			if (measureItemStruct.itemHeight>=48) {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList48(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_48;
			} else if (measureItemStruct.itemHeight>=32) {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList32(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_32;
			} else {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList16(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_16;
			}
		} else {
			m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList16(), LVSIL_SMALL);
			iconMode = CBodyListCtrl::ICON_MODE_16;
		}
	}

	// アイコン描画モードの設定
	m_bodyList.MySetIconMode( iconMode );

	// アイコンの設定
	sw_set_icon.start();
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
			CString miniImagePath = util::MakeImageLogfilePath( body[i] );

			// インデックス探索
			iconIndex = theApp.m_imageCache.GetImageIndex( miniImagePath );
		}

		// アイコンのインデックスを設定
		util::MySetListCtrlItemImageIndex( m_bodyList, i, 0, iconIndex );
	}
	sw_set_icon.stop();

	m_bodyList.m_bStopDraw = false;

	// アイコンを設定することでボディリストのアイテムの高さがシステム標準に戻っている場合がある。
	// これを回避するため、再度フォントを設定することで、
	// OnSetFont() => MeasureItem() が呼び出されるようにし、
	// ボディリストのアイテムの高さを再設定する。
	m_bodyList.SetFont( &theApp.m_font );

	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	util::MySetInformationText( m_hWnd, L"アイコンの作成完了" );
	MZ3LOGGER_DEBUG(
		util::FormatString(L"アイコン生成完了, generate[%dms](load/resize[%dms]), set[%dms]", 
			sw_generate_icon.getElapsedMilliSecUntilStoped(),
			dwLoadAndResizeMS,
			sw_set_icon.getElapsedMilliSecUntilStoped()));
}

/**
 * ボディにデータを設定
 */
void CMZ3View::SetBodyList( CMixiDataList& body )
{
	m_bModifyingBodyList = true;

	// リストのアイテムを削除
	m_bodyList.m_bStopDraw = true;
	m_bodyList.SetRedraw(FALSE);
	m_bodyList.DeleteAllItems();

	// ヘッダの文字を変更
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory != NULL) {
		LPCTSTR szHeaderTitle1 = MyGetBodyHeaderColName1( pCategory->m_mixi.GetAccessType() );
		LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi.GetAccessType(), pCategory->m_bodyColType2 );
		LPCTSTR szHeaderTitle3 = MyGetBodyHeaderColName2( pCategory->m_mixi.GetAccessType(), pCategory->m_bodyColType3 );
		m_bodyList.SetHeader( szHeaderTitle1, szHeaderTitle2, szHeaderTitle3 );
	}

	// アイテムの追加
	m_bodyList.MySetIconMode( CBodyListCtrl::ICON_MODE_NONE );	// まずアイコンはオフにして生成
	util::StopWatch sw;
	sw.start();
	int count = body.size();
	for (int i=0; i<count; i++) {
		// タブ切り替えが行われればキャンセル
		if (m_bReloadingGroupTabByThread && m_bRetryReloadGroupTabByThread) {
			m_bModifyingBodyList = false;
			return;
		}

		// 文字列は表示時に取得する
		m_bodyList.InsertItem( i, L"", -1 );
	}
	MZ3LOGGER_DEBUG(
		util::FormatString(L"ボディリスト設定完了, elapsed[%dms], count[%d]", 
		sw.getElapsedMilliSecUntilNow(), m_bodyList.GetItemCount()));

	// 統合カラムモード用のフォーマッタを設定する
	if (pCategory!=NULL) {
		m_bodyList.m_strIntegratedLinePattern1 = 
			theApp.m_accessTypeInfo.getBodyIntegratedLinePattern1(pCategory->m_mixi.GetAccessType());
		m_bodyList.m_strIntegratedLinePattern2 = 
			theApp.m_accessTypeInfo.getBodyIntegratedLinePattern2(pCategory->m_mixi.GetAccessType());
	}

	m_bModifyingBodyList = false;
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	// アイコン用ImageListの設定
	SetBodyImageList( body );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;

	// バックバッファ経由で再描画
	m_bodyList.DrawDetail();
	m_bodyList.UpdateWindow();

	if (m_bodyList.GetItemCount()==0) {
		// アイテムが0件
		util::MySetInformationText( m_hWnd, L"完了" );
	} else {
		// 第1カラムに表示している内容を表示する。
		if (pCategory!=NULL) {
			CString strInfo = util::MyGetItemByBodyColType(&GetSelectedBodyItem(), pCategory->m_bodyColType1, false);
			// 絵文字を文字列に変換する
			if( LINE_HAS_EMOJI_LINK(strInfo) ) {
				mixi::ParserUtil::ReplaceEmojiImageToText( strInfo );
			}
			m_infoEdit.SetWindowText( strInfo );
		}
	}
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

	if( lpnmlv->iItem <0 ){
		return;
	}

	CMixiData& data = GetSelectedBodyItem();

	TRACE(_T("http://mixi.jp/%s\n"), data.GetURL());

	switch (data.GetAccessType()) {
	case ACCESS_LIST_FOOTSTEP:
		return;

	case ACCESS_COMMUNITY:
		// コミュニティの場合は、トピック一覧を表示する。
		// （暫定対応）
		OnViewBbsList();
		return;

	case ACCESS_TWITTER_USER:
		// 全文表示
		OnMenuTwitterRead();
		return;

	case ACCESS_MIXI_ECHO_USER:
		// 全文表示
		OnMenuMixiEchoRead();
		return;

	case ACCESS_WASSR_USER:
		// 全文表示
		OnMenuWassrRead();
		break;

	case ACCESS_GOOHOME_USER:
		// 全文表示
		OnMenuGoohomeRead();
		break;

	case ACCESS_RSS_READER_ITEM:
		// 詳細表示
		OnMenuRssRead();
		return;

	default:
		// 特殊な要素以外なので、通信処理開始。
		AccessProc(&data, util::CreateMixiUrl(data.GetURL()));
	}
}

/**
 * アイテム変更
 */
void CMZ3View::OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_bModifyingBodyList) {
		// アイテム追加中なのでイベントハンドラ応答なし
		return;
	}

	// 選択を変更したら、そのアイテムをアクティブにする
	if (pNMLV->uNewState != 3) {
		return;
	}

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	pCategory->selectedBody = pNMLV->iItem;

	// 第1カラムに表示している内容を表示する。
	CString strInfo = util::MyGetItemByBodyColType(&GetSelectedBodyItem(), pCategory->m_bodyColType1, false);
	// 絵文字を文字列に変換する
	if( LINE_HAS_EMOJI_LINK(strInfo) ) {
		mixi::ParserUtil::ReplaceEmojiImageToText( strInfo );
	}

	m_infoEdit.SetWindowText( strInfo );

	// 画像位置変更
	MoveMiniImageDlg();

	// Twitter であれば再表示(同一IDや引用者の色変更を伴うため)
	if (util::IsTwitterAccessType(pCategory->m_mixi.GetAccessType())) {
		static int s_lastSelected = 0;
		int selected = pCategory->selectedBody;

		if (!m_bodyList.m_bStopDraw) {
			std::set<int> redrawItems;

			// selected, s_lastSelected は再描画
			if (selected < m_bodyList.GetItemCount()) {
				redrawItems.insert( selected );
			}
			if (s_lastSelected < m_bodyList.GetItemCount()) {
				redrawItems.insert( s_lastSelected );
			}

			const CMixiData& sel_data      = pCategory->m_body[selected];
			const CMixiData& last_sel_data = pCategory->m_body[s_lastSelected];

			// selected, s_lastSelected と同一IDの項目は再描画
			int idxStart = m_bodyList.GetTopIndex();
			int idxEnd   = idxStart + m_bodyList.GetCountPerPage();
			for (int idx=idxStart; idx<=idxEnd; idx++) {
				if (redrawItems.count(idx)==0 && 0 <= idx && idx < (int)pCategory->m_body.size()) {
					int id = pCategory->m_body[idx].GetOwnerID();
					if (id == sel_data.GetOwnerID()) {
						redrawItems.insert(idx);
					} else if (selected!=s_lastSelected && id == last_sel_data.GetOwnerID()) {
						redrawItems.insert(idx);
					}
				}
			}

			// selected, s_lastSelected の言及ユーザは再描画
			static MyRegex reg;
			util::CompileRegex(reg, L"@([0-9a-zA-Z_]+)");
			for (int i=0; i<2; i++) {
				CString target = (i==0) ? sel_data.GetBody() : last_sel_data.GetBody();
				if (target.Find(L"@")!=-1) {
					for (int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++) {
						std::vector<MyRegex::Result>* pResults = NULL;
						if (reg.exec(target) == false || reg.results.size() != 2) {
							break;
						}

						// 一致すれば強調表示
						for (int idx=idxStart; idx<=idxEnd; idx++) {
							if (redrawItems.count(idx)==0 && 0 <= idx && idx < (int)pCategory->m_body.size()) {
								if (pCategory->m_body[idx].GetName()==reg.results[1].str.c_str()) {
									redrawItems.insert(idx);
								}
							}
						}

						// ターゲットを更新。
						target.Delete(0, reg.results[0].end);
					}
				}
			}

			for (std::set<int>::iterator it=redrawItems.begin(); it!=redrawItems.end(); it++) {
				int idx = (*it);
				m_bodyList.DrawItemWithBackSurface(idx);
			}
		}

		s_lastSelected = selected;
	} 

	// アイコン再描画
	InvalidateRect( m_rectIcon, FALSE );

	*pResult = 0;
}

BOOL CMZ3View::OnKeyUp(MSG* pMsg)
{
	// 共通処理
	switch (pMsg->wParam) {
#ifndef WINCE
	case VK_F1:
		// ヘルプ表示
		util::OpenByShellExecute( MZ4_MANUAL_URL );
		break;
#endif

#ifndef WINCE
	case VK_APPS:
		OnAcceleratorContextMenu();
		return TRUE;
#endif

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
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			pMainFrame->OnMenuOpenLocalFile();
		}
		break;

	case 'U':
		// Ctrl+Alt+U
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			pMainFrame->OnMenuOpenUrl();
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
	}else if (pMsg->hwnd == m_statusEdit.m_hWnd) {
		switch (pMsg->wParam) {
		case VK_UP:
			if (m_bImeCompositioning) {
				// 漢字入力中はデフォルト動作
				return FALSE;
			} else {
				// ボディリストに移動
				CommandSetFocusBodyList();
				return TRUE;
			}
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
		case VK_UP:
			return TRUE;

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
		case VK_RETURN:
			// Enter 押下で投稿・再取得
			OnBnClickedUpdateButton();
			break;
		}
	}

	return FALSE;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
#ifdef DEBUG
/*	switch (pMsg->message) {
	case WM_MOUSEMOVE:		wprintf( L"PTM: WM_MOUSEMOVE\n" );		break;
	case WM_MOUSEWHEEL:		wprintf( L"PTM: WM_MOUSEWHEEL\n" );		break;
	case WM_RBUTTONDOWN:	wprintf( L"PTM: WM_RBUTTONDOWN\n" );	break;
	case WM_RBUTTONUP:		wprintf( L"PTM: WM_RBUTTONUP\n" );		break;
	}
*/
//	wprintf( L"pretranslatemessage : %d(0x%X)\n", pMsg->message, pMsg->message );
#endif

	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// メッセージに応じて、定期取得のキャンセル処理を行う
		switch (pMsg->message) {
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
			ResetIntervalTimer();
			break;
		default:
			break;
		}
	}

	switch (pMsg->message) {
	case WM_KEYUP:
		{
			BOOL r = OnKeyUp( pMsg );

			// KEYDOWN リピート回数を初期化
			m_nKeydownRepeatCount = 0;

			if (r) {
				return r;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			// KEYDOWN リピート回数をインクリメント
			m_nKeydownRepeatCount ++;

			if (OnKeyDown( pMsg )) {
				return TRUE;
			}
		}
		break;

	case WM_IME_STARTCOMPOSITION:	// IME 変換開始
		m_bImeCompositioning = true;
		break;

	case WM_IME_ENDCOMPOSITION:		// IME 変換終了
		m_bImeCompositioning = false;
		break;

	case WM_RBUTTONDOWN:
		OnRButtonDown( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_RBUTTONUP:
		OnRButtonUp( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_LBUTTONDOWN:
		OnLButtonDown( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_LBUTTONUP:
		OnLButtonUp( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_MOUSEWHEEL:
		OnMouseWheel( LOWORD(pMsg->wParam), HIWORD(pMsg->wParam), CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_MOUSEMOVE:
		OnMouseMove( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;
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
	if( m_access ) return TRUE;	// アクセス中は無視

	// 横スクロールアニメーションを起動する
	m_categoryList.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_RIGHT );

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
	if( m_access ) return TRUE;	// アクセス中は無視

	// 横スクロールアニメーションを起動する
	m_categoryList.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_LEFT );

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
		CString strInfo = util::MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_bodyColType1, false);
		// 絵文字を文字列に変換する
		if( LINE_HAS_EMOJI_LINK(strInfo) ) {
			mixi::ParserUtil::ReplaceEmojiImageToText( strInfo );
		}
		m_infoEdit.SetWindowText( strInfo );

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
		// Xcrawl 拡張モード
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
		// Xcrawl 拡張モード以外
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
				if (m_bImeCompositioning) {
					// 漢字入力中はデフォルト動作
					return FALSE;
				} else {
					return CommandMoveUpBodyList();
				}
			}
			break;

		case VK_DOWN:
			if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
				// 一番下なので無視。
				if( m_access ) return TRUE;	// アクセス中は禁止

				// Twitter モードならつぶやくモードへ。
				if (m_viewStyle==VIEW_STYLE_TWITTER) {
					// モード判定
					CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
					if (pCategory!=NULL) {
						std::string strServiceType = theApp.m_accessTypeInfo.getServiceType(pCategory->m_mixi.GetAccessType());
						if (strServiceType == "Twitter") {
							OnMenuTwitterUpdate();
							return TRUE;
						} else if (strServiceType == "Wassr") {
							OnMenuWassrUpdate();
							return TRUE;
						} else if (strServiceType == "gooHome") {
							OnMenuGoohomeUpdate();
							return TRUE;
						} else if (pCategory->m_mixi.GetAccessType()==ACCESS_MIXI_RECENT_ECHO) {
							OnMenuMixiEchoUpdate();
							return TRUE;
						}
					}
				}
				// デフォルト動作
				return FALSE;

			}else{
				if (m_bImeCompositioning) {
					// 漢字入力中はデフォルト動作
					return FALSE;
				} else {
					return CommandMoveDownBodyList();
				}
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
			// メニュー表示
			PopupBodyMenu();
			break;

		case ACCESS_TWITTER_USER:
			// 全文表示
			OnMenuTwitterRead();
			break;

		case ACCESS_MIXI_ECHO_USER:
			// 全文表示
			OnMenuMixiEchoRead();
			break;

		case ACCESS_WASSR_USER:
			// 全文表示
			OnMenuWassrRead();
			break;

		case ACCESS_GOOHOME_USER:
			// 全文表示
			OnMenuGoohomeRead();
			break;

		case ACCESS_RSS_READER_ITEM:
			// 詳細表示
			OnMenuRssRead();
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
			if( ( m_selGroup->focusedCategory < m_categoryList.GetTopIndex() ) &&
				( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() - m_categoryList.GetCountPerPage() ) ) {
				// 移動先が画面より上、1画面以内にある時は1画面スクロール
				m_categoryList.Scroll( CSize(0, -m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
			} else {
				// 移動先が画面より下か、上で1画面以上離れている時はEnsureVisible()
				m_categoryList.EnsureVisible( m_selGroup->focusedCategory , TRUE );
			}

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
			if( ( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() + m_categoryList.GetCountPerPage() ) &&
				( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() + m_categoryList.GetCountPerPage() * 2) ) {
				// 移動先が画面より下、1画面以内にある時は1画面スクロール
				m_categoryList.Scroll( CSize(0, m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
			} else {
				// 移動先が画面より上か、下で1画面以上離れている時はEnsureVisible()
				m_categoryList.EnsureVisible( m_selGroup->focusedCategory , TRUE );
			}

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
			// 選択項目が無ければ下端へ移動
			pCategory->selectedBody = ( pCategory->selectedBody < 0 ? m_bodyList.GetItemCount() - 1 : pCategory->selectedBody );
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// 移動先が非表示なら上方向にスクロール
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				int topIdx = m_bodyList.GetTopIndex();
				int countPerPage = m_bodyList.GetCountPerPage();
				int selectedBody = pCategory->selectedBody;
				if (topIdx - countPerPage <= selectedBody && selectedBody < topIdx) {
					// 移動先が画面より上で、上側の1画面内にある時は1画面スクロール
					//            +-[TopIndex - CountPerPage]
					//            +
					// [selected]-+
					//            --[TopIndex]
					MEASUREITEMSTRUCT measureItemStruct;
					m_bodyList.MeasureItem(&measureItemStruct);
					int y = -countPerPage * measureItemStruct.itemHeight;
					m_bodyList.Scroll( CSize(0, y) );
				} else {
					// 移動先が画面より下か、上で1画面以上離れている時はEnsureVisible()
					m_bodyList.EnsureVisible( selectedBody, TRUE );
				}

				// 再描画
				if (theApp.m_optionMng.IsUseBgImage()) {
					// 再描画
					int topIdx = m_bodyList.GetTopIndex();
					m_bodyList.RedrawItems(topIdx, topIdx + m_bodyList.GetCountPerPage());
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
				int topIdx = m_bodyList.GetTopIndex();
				int countPerPage = m_bodyList.GetCountPerPage();
				int selectedBody = pCategory->selectedBody;
				if (topIdx + countPerPage <= selectedBody && selectedBody < topIdx + countPerPage * 2) {
					// 移動先が画面より下で、下側の1画面内にある時は1画面スクロール
					//            +-[TopIndex + CountPerPage]
					//            +
					// [selected]-+
					//            --[TopIndex + CountPerPage*2]
					MEASUREITEMSTRUCT measureItemStruct;
					m_bodyList.MeasureItem(&measureItemStruct);
					int y = countPerPage * measureItemStruct.itemHeight;
					m_bodyList.Scroll( CSize(0, y) );
				} else {
					// 移動先が画面より上か、下で1画面以上離れている時はEnsureVisible()
					m_bodyList.EnsureVisible( selectedBody, TRUE );
				}

				// 再描画
				if (theApp.m_optionMng.IsUseBgImage()) {
					// 再描画
					int topIdx = m_bodyList.GetTopIndex();
					m_bodyList.RedrawItems(topIdx, topIdx + m_bodyList.GetCountPerPage());
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
	case ACCESS_BIRTHDAY:			// 誕生日プロフィール
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

	// アクセス種別を設定
	theApp.m_accessType = data->GetAccessType();

	// URL 整形
	CString uri = a_url;
	switch (data->GetAccessType()) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// コミュニティ、アンケートなら、取得件数を変更
		switch( theApp.m_optionMng.GetPageType() ) {
		case GETPAGE_LATEST20:
			// 最新２０件取得
			uri = a_url;
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

	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// Twitterタイムライン：
		// pageパラメータがなければ(つまりpage=1であれば)since_idパラメータ追加。
//		if (util::GetParamFromURL(uri, L"page").IsEmpty()) {
//			CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
//			if (pCategory!=NULL && pCategory->m_body.size()>0) {
//				int last_id = pCategory->m_body[0].GetID();
//				uri.AppendFormat(L"%ssince_id=%d", (uri.Find('?')<0 ? L"?" : L"&"), last_id);
//			}
//		}
		break;
	}

	// 【API 用】
	// URL 内のID置換
	uri.Replace( L"{owner_id}", theApp.m_loginMng.GetOwnerID() );
	uri.Replace( L"{wassr:id}", theApp.m_loginMng.GetWassrId() );

	data->SetBrowseUri(uri);

	// リファラ
	CString referer;
	switch (data->GetAccessType()) {
	case ACCESS_ENQUETE:
		// アンケート
		referer = _T("http://mixi.jp/") + data->GetURL();
		referer.Replace(_T("view_enquete"), _T("reply_enquete"));
		break;

	case ACCESS_LIST_FRIEND:
		// マイミク一覧
		referer = L"http://mixi.jp/list_friend.pl";
		break;
	}

	// encoding 指定
	switch (theApp.m_accessTypeInfo.getRequestEncoding(data->GetAccessType())) {
	case AccessTypeInfo::ENCODING_SJIS:
		encoding = CInetAccess::ENCODING_SJIS;
		break;
	case AccessTypeInfo::ENCODING_UTF8:
		encoding = CInetAccess::ENCODING_UTF8;
		break;
	case AccessTypeInfo::ENCODING_NOCONVERSION:
		encoding = CInetAccess::ENCODING_NOCONVERSION;
		break;
	case AccessTypeInfo::ENCODING_EUC:
	default:
		encoding = CInetAccess::ENCODING_EUC;
		break;
	}

	// 認証情報の設定
	CString strUser = NULL;
	CString strPassword = NULL;
	if (theApp.m_accessTypeInfo.getServiceType(data->GetAccessType())=="Twitter") {
		// Twitter API => Basic 認証
		strUser     = theApp.m_loginMng.GetTwitterId();
		strPassword = theApp.m_loginMng.GetTwitterPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
			return;
		}
	}
	if (theApp.m_accessTypeInfo.getServiceType(data->GetAccessType())=="Wassr") {
		// Wassr API => Basic 認証
		strUser     = theApp.m_loginMng.GetWassrId();
		strPassword = theApp.m_loginMng.GetWassrPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
			return;
		}
	}
	if (theApp.m_accessTypeInfo.getServiceType(data->GetAccessType())=="gooHome") {
		// gooHome API => Basic 認証
		strUser     = theApp.m_loginMng.GetGooId();
		strPassword = gooutil::GetAPIKeyFromQuoteMailAddress( theApp.m_loginMng.GetGoohomeQuoteMailAddress() );

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でgooIDとひとこと投稿アドレスを設定してください" );
			return;
		}
	}

	// アクセス開始
	m_access = TRUE;
	m_abort = FALSE;

	// コントロール状態の変更
	MyUpdateControlStatus();

	// GET/POST 判定
	bool bPost = false;	// デフォルトはGET
	switch (theApp.m_accessTypeInfo.getRequestMethod(data->GetAccessType())) {
	case AccessTypeInfo::REQUEST_METHOD_POST:
		bPost = true;
		break;
	case AccessTypeInfo::REQUEST_METHOD_GET:
	case AccessTypeInfo::REQUEST_METHOD_INVALID:
	default:
		break;
	}

	static CPostData post;
	if (bPost) {
		post.SetSuccessMessage( WM_MZ3_POST_END );
		post.AppendAdditionalHeader(L"");
	}

	// [MZ3-API] GET/POST 直前のフック処理(の予定)
	switch (data->GetAccessType()) {
	case ACCESS_LIST_FRIEND:
		// マイミク一覧
		// マイミク一覧はブラウザでは下記の手順で取得される。
		//  1. list_friend.pl で post_key を取得し、
		//  2. そのキーを含めて ajax_friend_setting.pl から JSON 形式を取得する
		// 従って、MZ3 では下記の手順で取得する。
		//  1. data に post_key がなければ list_friend.pl から取得する。
		//  2. 成功すれば ajax_friend_setting.pl から取得する。
		CString post_key;

		CMixiDataList& body = m_selGroup->getSelectedCategory()->m_body;
		if (body.size()>0) {
			post_key = body[0].GetTextValue(L"post_key");
		}

//		MZ3_TRACE(L"★post_key[%s]\n", post_key);
		if (post_key.IsEmpty()) {
			// list_friend.pl から取得する
			uri = L"http://mixi.jp/list_friend.pl";
			bPost = false;
		} else {
			post.AppendAdditionalHeader(L"X-Requested-With: XMLHttpRequest");
			post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
			post.AppendPostBody(L"post_key=");
			post.AppendPostBody(post_key);
			post.AppendPostBody(L"&_=");
		}
		break;
	}

	// UserAgent設定
	// Wassr はブラウザ風UAだとAPIが叩けない事象の回避
	CString strUserAgent = L"";	// "" で初期値
	if (theApp.m_accessTypeInfo.getServiceType(data->GetAccessType())=="Wassr") {
		strUserAgent = MZ3_APP_NAME;
	}

	// アップデート時のUA設定
	switch (data->GetAccessType()) {
	case ACCESS_SOFTWARE_UPDATE_CHECK:
		strUserAgent.Format(L"%s(%s)", MZ3_APP_NAME, MZ3_VERSION_TEXT_SHORT);
		break;
	}

	// GET/POST 開始
	theApp.m_inet.Initialize( m_hWnd, data, encoding );
	if (bPost) {
		theApp.m_inet.DoPost(uri, referer, CInetAccess::FILE_HTML, &post, strUser, strPassword, strUserAgent );
	} else {
		theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML, strUser, strPassword, strUserAgent );
	}
}

/// 右ソフトキーメニュー｜全部読む
void CMZ3View::OnGetAll()
{
	// チェック
	//if( m_hotList != &m_bodyList ) {
	//	return;
	//}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
	case ACCESS_NEIGHBORDIARY:
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
	//if( m_hotList != &m_bodyList ) {
	//	return;
	//}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
	case ACCESS_NEIGHBORDIARY:
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
	if (m_selGroup==NULL) {
		return;
	}

	// スタイル変更
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	// Twitterスタイルであればカテゴリに応じて送信タイプを初期化
	if (m_viewStyle==VIEW_STYLE_TWITTER) {
		// カテゴリに応じてTwitter送信モードを初期化
		MyResetTwitterStylePostMode();

		// コントロール状態の変更
		MyUpdateControlStatus();
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
		msgHead.Format( L"%s : ", theApp.m_accessTypeInfo.getShortText(category.m_mixi.GetAccessType()));

		// HTML の取得
		util::MySetInformationText( m_hWnd, msgHead + _T("解析中 : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML 解析
		util::MySetInformationText( m_hWnd, msgHead + _T("解析中 : 2/3") );
		parser::MyDoParseMixiListHtml( category.m_mixi.GetAccessType(), category.m_mixi, body, html );

		// 取得したデータに from_log_flag を設定する
		// （「Twitterの新着件数カウントに含める」等のため）
		for (size_t i=0; i<body.size(); i++) {
			body[i].SetIntValue(L"from_log_flag", 1);
		}

		// ボディ一覧の設定
		util::MySetInformationText( m_hWnd, msgHead + _T("解析中 : 3/3") );

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
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		// サポートしている
		break;
	default:
		// 未サポートなので終了する
		{
			CString msg = L"この形式のログはサポートしていません : ";
			msg += theApp.m_accessTypeInfo.getShortText(mixi.GetAccessType());
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
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE );
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
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory == NULL) {
		return false;
	}

	/**
	 * 統合カラムモード → 1行モード(A)
	 * 1行モード(A) → [Bモードあり] 1行モード(B)
	 *              → [Bモードなし] 統合カラムモード
	 * 1行モード(B) → 統合カラムモード
	 */

	ACCESS_TYPE categoryAccessType = pCategory->m_mixi.GetAccessType();
	AccessTypeInfo::BODY_INDICATE_TYPE colType2 = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(categoryAccessType);
	AccessTypeInfo::BODY_INDICATE_TYPE colType3 = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(categoryAccessType);

	// 統合カラムモード → 1行モード(タイプA)
	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// タイプAに変更
		pCategory->m_bodyColType2 = colType2;
		pCategory->m_bodyColType3 = colType3;

		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// 状態に応じて、「表示項目」の変更を行う
	// いわゆるトグル動作。
	if (colType3==AccessTypeInfo::BODY_INDICATE_TYPE_NONE) {
		// 変更先タイプがない
		// → 統合カラムモード
		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// トグル動作
	if (pCategory->m_bodyColType2==colType2) {
		// 2 to 3
		pCategory->m_bodyColType2 = colType3;
		pCategory->m_bodyColType3 = colType2;
	} else {
		// 3 to 2
		// ? to 2
//		pCategory->m_bodyColType2 = colType2;
//		pCategory->m_bodyColType3 = colType3;

		// 3 to 統合カラムモード
		// ? to 統合カラムモード
		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// ヘッダー文字列の変更（第2, 3カラムのみ）
	m_bodyList.SetHeader( NULL, 
		MyGetBodyHeaderColName2(categoryAccessType, pCategory->m_bodyColType2), 
		MyGetBodyHeaderColName2(categoryAccessType, pCategory->m_bodyColType3));

	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// 統合カラムモードでは背景以外再描画
		Invalidate(FALSE);
	} else {
		// 再描画
		// バックバッファ経由で再描画
		m_bodyList.DrawDetail();
		m_bodyList.UpdateWindow();
	}

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
	// オプションの設定
	m_categoryList.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
	m_categoryList.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;

	// 表示を更新
	if( m_categoryList.WaitForPanScroll( 0L ) ){
		// 横スクロールが終わっていたら
		// 背景画像のみが表示されている状態なので最新状態に更新する
#ifdef WINCE
		m_categoryList.Invalidate( FALSE );
#else
		m_categoryList.Invalidate( TRUE );
#endif
		m_categoryList.Update( 0 );
	}

	// 選択変更時の処理を実行する（ログの読み込み）
	m_categoryList.m_bStopDraw = true;
	OnMySelchangedCategoryList();
	m_categoryList.m_bStopDraw = false;

	// 横スクロールの終了を待つ
	m_categoryList.WaitForPanScroll( 1000L );

	// 背景画像のみが表示されている状態なので最新状態に更新する
#ifdef WINCE
	m_categoryList.Invalidate( FALSE );
#else
	m_categoryList.Invalidate( TRUE );
#endif
	m_categoryList.Update( 0 );

	// アイコン再描画
	InvalidateRect( m_rectIcon, FALSE );
}

/**
 * グループタブ遅延読み込み用スレッド
 */
unsigned int CMZ3View::ReloadGroupTab_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	if( pView->m_bReloadingGroupTabByThread ) {
		pView->m_bRetryReloadGroupTabByThread = true;
		return 0;
	}

	pView->m_bReloadingGroupTabByThread = true;
	::Sleep( 10L );

	// 処理前後で選択グループが変化していれば、再読込を行う
	int selectedGroup = pView->m_groupTab.GetCurSel();
	for(;;) {
		// 選択変更時の処理を実行する
		pView->m_bRetryReloadGroupTabByThread = false;
		pView->OnSelchangedGroupTab();

		if( selectedGroup == pView->m_groupTab.GetCurSel() ) {
			// 処理前と選択グループが同じなので処理完了とする。
			break;
		}else{
			// 処理前と選択グループが違うので、再読込を行う。
			selectedGroup = pView->m_groupTab.GetCurSel();
		}
	}

	pView->m_bReloadingGroupTabByThread = false;
	pView->m_bRetryReloadGroupTabByThread = false;

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
	MessageBox( GetSelectedBodyItem().GetBody() );
}

/// 未読にする
void CMZ3View::OnSetNoRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ログINIファイルの項目を初期化
	CString logId = util::GetLogIdString( mixi );
	theApp.m_readlog.DeleteRecord( util::my_wcstombs((LPCTSTR)logId), "Log" );

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
	theApp.m_readlog.SetValue(util::my_wcstombs((LPCTSTR)logId), (const char*)util::int2str_a(mixi.GetCommentCount()), "Log");

	// ビューを更新
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// ボディリストでの右クリックメニュー
bool CMZ3View::PopupBodyMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	CMixiData& bodyItem = GetSelectedBodyItem();
	switch( bodyItem.GetAccessType() ) {
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
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
			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
			case ACCESS_NEIGHBORDIARY:
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
	case ACCESS_BIRTHDAY:
		// プロフィールなら、カテゴリ項目に応じて処理を変更する。（暫定）
		switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_INTRO:				// 紹介文
		case ACCESS_LIST_FAVORITE_USER:		// お気に入り
		case ACCESS_LIST_FOOTSTEP:			// 足あと
		case ACCESS_LIST_FRIEND:			// マイミク一覧
		case ACCESS_LIST_CALENDAR:			// カレンダー
			// 操作をメニューで選択
			{
				CMenu menu;
				menu.LoadMenu( IDR_PROFILE_ITEM_MENU );
				CMenu* pSubMenu = menu.GetSubMenu(0);

				ACCESS_TYPE categoryType = m_selGroup->getSelectedCategory()->m_mixi.GetAccessType();

				// お気に入り以外では「自己紹介」を削除。
				if( categoryType != ACCESS_LIST_FAVORITE_USER ) {
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

	case ACCESS_MIXI_ECHO_USER:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(2);	// echo用メニューはidx=2

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

			// ユーザ、引用ユーザのエコー一覧
			CString s;
			s.Format(L"%s さんのエコー", bodyItem.GetName());
			pSubMenu->ModifyMenu(ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST, MF_BYCOMMAND, 
								 ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST, s);

			// 引用ユーザのエコー一覧
			CString ref_user_name = bodyItem.GetTextValue(L"ref_user_name");
			if (!ref_user_name.IsEmpty()) {
				s.Format(L"%s さんのエコー", ref_user_name);
				pSubMenu->ModifyMenu(ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST, MF_BYCOMMAND, 
									 ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST, s);
			} else {
				pSubMenu->RemoveMenu(ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST, MF_BYCOMMAND);
			}

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_WASSR_USER:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(3);	// Wassr用メニューはidx=3

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

			ACCESS_TYPE categoryType = m_selGroup->getSelectedCategory()->m_mixi.GetAccessType();

			// 「お気に入り」であれば「外す」メニューを有効化
			if (categoryType == ACCESS_TWITTER_FAVORITES) {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_DESTROY_FAVOURINGS, MF_ENABLED | MF_BYCOMMAND );
			} else {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_DESTROY_FAVOURINGS, MF_GRAYED | MF_BYCOMMAND );
			}
			// 「ステータス」であれば「ふぁぼる」メニューを無効化
			if (categoryType == ACCESS_TWITTER_FRIENDS_TIMELINE) {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_CREATE_FAVOURINGS, MF_ENABLED | MF_BYCOMMAND );
			} else {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_CREATE_FAVOURINGS, MF_GRAYED | MF_BYCOMMAND );
			}
			// URL が空であれば「友達のサイト」を無効化
			if (GetSelectedBodyItem().GetURL().IsEmpty()) {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_SITE, MF_GRAYED | MF_BYCOMMAND );
			} else {
				pSubMenu->EnableMenuItem( ID_MENU_TWITTER_SITE, MF_ENABLED | MF_BYCOMMAND );
			}

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_RSS_READER_ITEM:
		{
			CMenu menu;
			menu.LoadMenu( IDR_RSS_MENU );
			CMenu editmenu;
			editmenu.CreatePopupMenu();

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
					// URLをコピーメニューの生成
					editmenu.AppendMenu( MF_STRING , ID_REPORT_COPY_URL_BASE+(i+1), s);
				}
			}
			// リンクの下に「URLをコピー」メニューを追加する
			pSubMenu->AppendMenu( MF_SEPARATOR , ID_REPORT_URL_BASE, _T("-"));
			pSubMenu->AppendMenu( MF_POPUP , (UINT_PTR)editmenu.GetSafeHmenu() , L"URLをコピー" ); 

			// メニューを開く
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_GOOHOME_USER:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(4);	// gooHome用メニューはidx=4

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
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_LIST_BBS),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_LIST_BBS),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_LIST_BBS),
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
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
			case ACCESS_EVENT_JOIN:
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
	if (theApp.m_pMouseGestureManager->IsProcessed()) {
		// マウスジェスチャ処理が行われたためキャンセル
		return;
	}

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
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// 巡回対象以外のカテゴリであれば巡回メニューを無効化する
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	if (pCategory != NULL) {
		if (theApp.m_accessTypeInfo.isCruiseTarget(pCategory->m_mixi.GetAccessType())) {
			// 巡回対象なので巡回メニューを無効化しない
		} else {
			// 巡回メニューを無効化する
			pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		}

		// 巡回予約済みであればチェックを付ける。
		if( pCategory->m_bCruise ) {
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
		}else{
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
		}
	} else {
		// 巡回メニューを無効化する
		pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
	}

	// 項目を追加
	const int CATEGORY_MENU_APPEND_SUB_MENU_IDX = 7;
	if (pSubMenu->GetMenuItemCount()<=CATEGORY_MENU_APPEND_SUB_MENU_IDX) {
		MZ3LOGGER_FATAL(L"カテゴリメニューの項目数が不正です");
	} else {
		CMenu* pAppendMenu = pSubMenu->GetSubMenu(CATEGORY_MENU_APPEND_SUB_MENU_IDX);
		if (pAppendMenu) {
			// ダミーを削除
			pAppendMenu->RemoveMenu( ID_APPEND_MENU_BEGIN, MF_BYCOMMAND );

			// フルテンプレート生成
			Mz3GroupData template_data;
			template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

			// サブメニューの初期化
			if (m_pCategorySubMenuList != NULL) {
				delete[] m_pCategorySubMenuList;
			}
			m_pCategorySubMenuList = new CMenu[template_data.groups.size()];

			int menuId = ID_APPEND_MENU_BEGIN;
			for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
				m_pCategorySubMenuList[groupIdx].CreatePopupMenu();

				CGroupItem& group = template_data.groups[groupIdx];

				// m_pCategorySubMenuList[groupIdx] にカテゴリ名を追加
				for (unsigned int ic=0; ic<group.categories.size(); ic++) {
					m_pCategorySubMenuList[groupIdx].AppendMenuW( MF_STRING, menuId, group.categories[ic].m_name );
					menuId ++;
				}

				pAppendMenu->AppendMenuW( MF_POPUP, (UINT)m_pCategorySubMenuList[groupIdx].m_hMenu, group.name );
				m_pCategorySubMenuList[groupIdx].Detach();
			}
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
	if (theApp.m_pMouseGestureManager->IsProcessed()) {
		// マウスジェスチャ処理が行われたためキャンセル
		return;
	}

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
	switch (item->m_mixi.GetAccessType()) {
	case ACCESS_LIST_BOOKMARK:
		// ブックマーク：アクセスなし（ローカルストレージ）
		SetBodyList( item->GetBodyList() );
		break;

	default:
		// インターネットにアクセス
		m_hotList = &m_bodyList;
		AccessProc( &item->m_mixi, util::CreateMixiUrl(item->m_mixi.GetURL()));
		break;
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
 * ボディリスト選択項目の画像がなければ取得する
 */
void CMZ3View::MoveMiniImageDlg(int idxBody/*=-1*/, int pointx/*=-1*/, int pointy/*=-1*/)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return;
	}

	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			// 対象(idxBodyパラメータ)が未指定であれば「選択項目」を対象とする。
			int target = idxBody;
			if (idxBody<0 || idxBody>=(int)pCategory->m_body.size()) {
				target = pCategory->selectedBody;
			}

			// mini画像が未ロードであれば取得する
			if (!pCategory->m_body.empty() && 0 <= target && target < (int)pCategory->m_body.size() ) {
				CMixiData& data = pCategory->m_body[ target ];
				MyLoadMiniImage( data );
			}
		}
	}
}

LRESULT CMZ3View::OnHideView(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

bool CMZ3View::MyLoadMiniImage(CMixiData& mixi)
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
			if (!m_access) {
				// アクセス中は禁止
				// 取得
				static CMixiData s_data;
				CMixiData dummy;
				s_data = dummy;
				s_data.SetAccessType( ACCESS_IMAGE );

				CString url = mixi.GetImage(0);

				// アクセス種別を設定
				theApp.m_accessType = s_data.GetAccessType();

				// アクセス開始
				m_access = TRUE;
				m_abort = FALSE;

				// コントロール状態の変更
				MyUpdateControlStatus();

				theApp.m_inet.Initialize( m_hWnd, &s_data );
				theApp.m_inet.DoGet(url, L"", CInetAccess::FILE_BINARY );
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
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnMenuFontMagnify();
}

/**
 * フォント縮小
 */
void CMZ3View::OnAcceleratorFontShrink()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnMenuFontShrink();
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
	if (theApp.m_pMouseGestureManager->IsGestureMode()) {
		// Ctrl+ホイールで拡大・縮小
		// 方向が逆になったらキャンセル
		int& s_delta = theApp.m_pMouseGestureManager->m_delta;
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>=WHEEL_DELTA) {
			// 前のタブ
			CommandSelectGroupTabBeforeItem();

			// 右クリック処理キャンセルのため、処理済みフラグを設定する
			theApp.m_pMouseGestureManager->SetProcessed();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<=-WHEEL_DELTA) {
			// 次のタブ
			CommandSelectGroupTabNextItem();

			// 右クリック処理キャンセルのため、処理済みフラグを設定する
			theApp.m_pMouseGestureManager->SetProcessed();
			s_delta += WHEEL_DELTA;
		}
		return TRUE;
	}

	if (nFlags & MK_CONTROL) {
		// Ctrl+ホイールで拡大・縮小
		// 高解像度ホイール対応のため、delta 値を累積する。
		static int s_delta = 0;
		// 方向が逆になったらキャンセル
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>=WHEEL_DELTA) {
			OnAcceleratorFontMagnify();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<=-WHEEL_DELTA) {
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
	CString item = data.GetBody();
	while( item.Replace( L"\r\n", L"" ) );

	item.Append( L"\r\n" );
	item.Append( L"----\r\n" );
	item.AppendFormat( L"name : %s\r\n", data.GetAuthor() );
	item.AppendFormat( L"description : %s\r\n", data.GetTitle() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );
	item.AppendFormat( L"id : %d\r\n", data.GetID() );
	item.AppendFormat( L"owner-id : %d\r\n", data.GetOwnerID() );

	if (data.GetChildrenSize()>=1) {
		// その他の情報を追加
		for (size_t i=0; i<data.GetChildrenSize(); i++) {
			CString s = data.GetChild(i).GetBodyItem(0);
			mixi::ParserUtil::StripAllTags( s );
			item.Append( s );
		}
	}

	MessageBox( item, data.GetName() );
}

/**
 * Twitter | 言い返す
 */
void CMZ3View::OnMenuTwitterReply()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_UPDATE;

	// ボタン名称変更
	MyUpdateControlStatus();

	// 入力領域にユーザのスクリーン名を追加。
	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// すでに含まれていれば追加しない
	CMixiData& data = GetSelectedBodyItem();
	if (strStatus.Find( util::FormatString(L"@%s", (LPCTSTR)data.GetName() ))!=-1) {
		return;
	}

	strStatus.AppendFormat( L"@%s ", (LPCTSTR)data.GetName() );

	SetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// End へ移動
	keybd_event( VK_END, 0, 0, 0 );
	keybd_event( VK_END, 0, KEYEVENTF_KEYUP, 0 );
}

/**
 * Twitter | メッセージ送信
 */
void CMZ3View::OnMenuTwitterNewDm()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_DM;

	// ボタン名称変更
	MyUpdateControlStatus();

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

/**
 * Twitter | つぶやく
 */
void CMZ3View::OnMenuTwitterUpdate()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_UPDATE;

	// ボタン名称変更
	MyUpdateControlStatus();

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

/**
 * カテゴリの選択状態に応じたビュースタイルの判定
 */
CMZ3View::VIEW_STYLE CMZ3View::MyGetViewStyleForSelectedCategory(void)
{
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			std::string strServiceType = theApp.m_accessTypeInfo.getServiceType(pCategory->m_mixi.GetAccessType());
			if (strServiceType == "Twitter") {
				// Twitter系であれば Twitter スタイル
				return VIEW_STYLE_TWITTER;
			} else if (strServiceType == "Wassr") {
				// Wassr系であれば Twitter スタイル
				return VIEW_STYLE_TWITTER;
			} else if (strServiceType == "gooHome") {
				// gooHome系であれば Twitter スタイル
				return VIEW_STYLE_TWITTER;
			} else if (strServiceType == "RSS") {
				// RSS は「イメージ付き」とする
				return VIEW_STYLE_IMAGE;
			} else {

				if (pCategory->m_mixi.GetAccessType()==ACCESS_MIXI_RECENT_ECHO) {
					// エコーは Twitter スタイル
					return VIEW_STYLE_TWITTER;
				}

				if (m_bodyList.IsEnableIcon()) {
					// 画像があれば「イメージ付き」とする
					CImageList* pImageList = m_bodyList.GetImageList(LVSIL_SMALL);
					if (pImageList != NULL &&
						pImageList->m_hImageList == theApp.m_imageCache.GetImageList16().m_hImageList)
					{
						return VIEW_STYLE_IMAGE;
					}
				}
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

	// mixi エコー用モード確認
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		MZ3LOGGER_ERROR( L"カテゴリが未選択" );
		return;
	}
/*	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_MIXI_RECENT_ECHO:
		// エコー表示中なので強制的にエコーモードに変更する
		m_twitterPostMode = TWITTER_STYLE_POST_MODE_MIXI_ECHO;
		break;
	}
*/
	// 入力文字列を取得
	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// 未入力時の処理
	if (strStatus.IsEmpty()) {
		switch (m_twitterPostMode) {
		case TWITTER_STYLE_POST_MODE_DM:
		case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
			// 未入力はNG
			return;

		case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
		case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		case TWITTER_STYLE_POST_MODE_UPDATE:
		default:
			// 未入力なので最新取得
			RetrieveCategoryItem();
			return;
		}
	}

	// DMモードであれば、送信先確認
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
		{
			CString msg;
			msg.Format( 
				L"mixi エコーで発言します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
		{
			CMixiData& data = pCategory->GetSelectedBody();
			CString msg;
			msg.Format( 
				L"mixi エコーで %s さんに返信します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				(LPCTSTR)data.GetName(), strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	case TWITTER_STYLE_POST_MODE_DM:
		{
			CMixiData& data = pCategory->GetSelectedBody();
			CString msg;
			msg.Format( 
				L"%s さんに以下のメッセージを送信します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				data.GetName(), strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	case TWITTER_STYLE_POST_MODE_UPDATE:
		{
			CMixiData& data = pCategory->GetSelectedBody();
			CString msg;
			msg.Format( 
				L"Twitterで発言します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		{
			CMixiData& data = pCategory->GetSelectedBody();
			CString msg;
			msg.Format( 
				L"Wassrで発言します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		{
			CString msg;
			msg.Format( 
				L"gooホームひとことで発言します。\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"よろしいですか？", 
				strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	default:
		break;
	}

	// フォーカスを入力領域に移動
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// 入力領域を無効化
	GetDlgItem( IDC_STATUS_EDIT )->EnableWindow( FALSE );

	static CPostData post;
	post.ClearPostBody();

	// ヘッダーを設定
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_DM:
	case TWITTER_STYLE_POST_MODE_UPDATE:
		post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client: %s", MZ3_APP_NAME ) );
		post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-URL: %s", L"http://mz3.jp/" ) );
		post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-Version: %s", MZ3_VERSION_TEXT_SHORT ) );
		break;
	default:
		break;
	}

	// POST パラメータを設定
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
		{
			// body=test+from+firefox&x=28&y=20&post_key=74b630af81dfaae59bfb6352728844a7&redirect=recent_echo
			CString post_key = GetSelectedBodyItem().GetTextValue(L"post_key");
			if (post_key.IsEmpty()) {
				MessageBox(L"送信用のキーが見つかりません。エコー一覧をリロードして下さい。");
				return;
			}
			post.AppendPostBody( "body=" );
			post.AppendPostBody( URLEncoder::encode_euc(strStatus) );
			post.AppendPostBody( "&x=28&y=20" );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post_key );
			post.AppendPostBody( "&redirect=recent_echo" );
		}
		break;

	case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
		{
			// body=test&x=13&y=13&parent_member_id=xxx&parent_post_time=xxx&redirect=list_echo&post_key=xxx
			CString post_key = GetSelectedBodyItem().GetTextValue(L"post_key");
			if (post_key.IsEmpty()) {
				MessageBox(L"送信用のキーが見つかりません。エコー一覧をリロードして下さい。");
				return;
			}

			CMixiData& data = pCategory->GetSelectedBody();
			post.AppendPostBody( "body=" );
			post.AppendPostBody( URLEncoder::encode_euc(strStatus) );
			post.AppendPostBody( "&x=28&y=20" );
			post.AppendPostBody( "&parent_member_id=" );
			post.AppendPostBody( util::int2str(data.GetAuthorID()) );
			post.AppendPostBody( "&parent_post_time=" );
			post.AppendPostBody( data.GetTextValue(L"echo_post_time") );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post_key );
			post.AppendPostBody( "&redirect=recent_echo" );
		}
		break;

	case TWITTER_STYLE_POST_MODE_DM:
		{
			CMixiData& data = pCategory->GetSelectedBody();
			int	twitterDirectMessageRecipientId = data.GetOwnerID();
			post.AppendPostBody( "text=" );
			post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
			post.AppendPostBody( "&user=" );
			post.AppendPostBody( util::int2str(twitterDirectMessageRecipientId) );
		}
		break;

	case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		post.AppendPostBody( "status=" );
		post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
		post.AppendPostBody( "&source=" );
		post.AppendPostBody( MZ3_APP_NAME );
		// TODO reply 時の処理
//		if (0) {
//			post.AppendPostBody( "&reply_status_rid=" );
//			post.AppendPostBody( reply_status_rid );
//		}
		break;

	case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		{
			// text=***&privacy=***
			// privacy 1:非公開
			//         2:公開
			//         4:友達まで公開
			//         8:友達の友達まで公開
			post.AppendPostBody( "text=" );
			post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
			post.AppendPostBody( "&privacy=2" );
		}
		break;

	case TWITTER_STYLE_POST_MODE_UPDATE:
	default:
		post.AppendPostBody( "status=" );
		post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
		if (theApp.m_optionMng.m_bAddSourceTextOnTwitterPost) {
			post.AppendPostBody( URLEncoder::encode_utf8(theApp.m_optionMng.m_strTwitterPostFotterText) );
		}
		post.AppendPostBody( "&source=" );
		post.AppendPostBody( MZ3_APP_NAME );
		break;
	}
	post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );
	post.SetSuccessMessage( WM_MZ3_POST_END );

	// POST先URL設定
	CString url;
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
		url = L"http://mixi.jp/add_echo.pl";
		break;

	case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
		url = L"http://mixi.jp/add_echo.pl";
		break;

	case TWITTER_STYLE_POST_MODE_DM:
		url = L"http://twitter.com/direct_messages/new.xml";
		break;

	case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		url = L"http://api.wassr.jp/statuses/update.json";
		break;

	case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		url = L"http://home.goo.ne.jp/api/quote/quotes/post/json";
		break;

	case TWITTER_STYLE_POST_MODE_UPDATE:
	default:
		url = L"http://twitter.com/statuses/update.xml";
		break;
	}

	// BASIC 認証設定
	CString strUser = NULL;
	CString strPassword = NULL;
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
		break;

	case TWITTER_STYLE_POST_MODE_DM:
	case TWITTER_STYLE_POST_MODE_UPDATE:
		// Twitter API => Basic 認証
		strUser     = theApp.m_loginMng.GetTwitterId();
		strPassword = theApp.m_loginMng.GetTwitterPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
			return;
		}
		break;

	case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		// Wassr API => Basic 認証
		strUser     = theApp.m_loginMng.GetWassrId();
		strPassword = theApp.m_loginMng.GetWassrPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でユーザIDとパスワードを設定してください" );
			return;
		}
		break;

	case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		// gooHome API => Basic 認証
		strUser     = theApp.m_loginMng.GetGooId();
		strPassword = gooutil::GetAPIKeyFromQuoteMailAddress( theApp.m_loginMng.GetGoohomeQuoteMailAddress() );

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox( L"ログイン設定画面でgooIDとひとこと投稿アドレスを設定してください" );
			return;
		}
	}

	// アクセス種別を設定
	switch (m_twitterPostMode) {
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
	case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
		theApp.m_accessType = ACCESS_MIXI_ADD_ECHO;
		break;

	case TWITTER_STYLE_POST_MODE_DM:
		theApp.m_accessType = ACCESS_TWITTER_NEW_DM;
		break;

	case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
		theApp.m_accessType = ACCESS_WASSR_UPDATE;
		break;

	case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
		theApp.m_accessType = ACCESS_GOOHOME_QUOTE_UPDATE;
		break;

	case TWITTER_STYLE_POST_MODE_UPDATE:
	default:
		theApp.m_accessType = ACCESS_TWITTER_UPDATE;
		break;
	}

	// アクセス開始
	m_access = TRUE;
	m_abort = FALSE;

	// コントロール状態の変更
	MyUpdateControlStatus();

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		L"", 
		CInetAccess::FILE_HTML, 
		&post, strUser, strPassword );

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

	ACCESS_TYPE aType = theApp.m_accessType;
	switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
	case AccessTypeInfo::INFO_TYPE_CATEGORY:
		// --------------------------------------------------
		// カテゴリ項目の取得
		// --------------------------------------------------

		// ログ保存
		{
			CString strLogfilePath = util::MakeLogfilePath( *(CMixiData*)lParam );
			if(! strLogfilePath.IsEmpty() ) {
				// 保存ファイルにコピー
				CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, 上書き*/ );
			}
		}

		DoAccessEndProcForBody(aType);

		break;

	case AccessTypeInfo::INFO_TYPE_POST:
		// --------------------------------------------------
		// 投稿処理
		// --------------------------------------------------
		MZ3_TRACE(L"POST Status[%d]\n", theApp.m_inet.m_dwHttpStatus);
		if (theApp.m_accessTypeInfo.getServiceType(aType)=="Twitter") {
			// Twitter投稿処理
			// HTTPステータスチェックを行う。
			LPCTSTR szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
			if (szStatusErrorMessage!=NULL) {
				CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
				util::MySetInformationText( m_hWnd, msg );
				MZ3LOGGER_ERROR( msg );
			} else {
				switch (aType) {
				case ACCESS_TWITTER_NEW_DM:
					util::MySetInformationText( m_hWnd, L"メッセージ送信終了" );
					break;
				case ACCESS_TWITTER_FAVOURINGS_CREATE:
					util::MySetInformationText( m_hWnd, L"ふぁぼった！" );
					break;
				case ACCESS_TWITTER_FAVOURINGS_DESTROY:
					util::MySetInformationText( m_hWnd, L"ふぁぼるのやめた！" );
					break;
				case ACCESS_TWITTER_FRIENDSHIPS_CREATE:
					util::MySetInformationText( m_hWnd, L"フォローした！" );
					break;
				case ACCESS_TWITTER_FRIENDSHIPS_DESTROY:
					util::MySetInformationText( m_hWnd, L"フォローやめた！" );
					break;
				case ACCESS_TWITTER_UPDATE:
				default:
					util::MySetInformationText( m_hWnd, L"ステータス送信終了" );
					break;
				}

				// 入力値を消去
				SetDlgItemText( IDC_STATUS_EDIT, L"" );
			}
		} else if (theApp.m_accessTypeInfo.getServiceType(aType) == "Wassr") {
			// HTTPステータスチェックを行う。
			if (theApp.m_inet.m_dwHttpStatus==200) {
				// OK

				// 入力値を消去
				SetDlgItemText( IDC_STATUS_EDIT, L"" );
			} else {
				LPCTSTR szStatusErrorMessage = L"?";
				CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
				util::MySetInformationText( m_hWnd, msg );
				MZ3LOGGER_ERROR( msg );
			}
		} else if (theApp.m_accessTypeInfo.getServiceType(aType) == "gooHome") {
			// HTTPステータスチェックを行う。
			if (theApp.m_inet.m_dwHttpStatus==200) {
				// OK

				// 入力値を消去
				SetDlgItemText( IDC_STATUS_EDIT, L"" );
			} else {
				LPCTSTR szStatusErrorMessage = L"?";
				CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
				util::MySetInformationText( m_hWnd, msg );
				MZ3LOGGER_ERROR( msg );
			}
		} else {
			// Twitter 以外
			switch (aType) {
			case ACCESS_MIXI_ADD_ECHO:
				// ログアウトチェック
				if (theApp.IsMixiLogout(aType)) {
					// ログアウト状態になっている
					MessageBox(L"未ログインです。エコー一覧をリロードし、mixiにログインして下さい。");
				} else {
					util::MySetInformationText( m_hWnd, L"エコー書き込み完了" );
					// 入力値を消去
					SetDlgItemText( IDC_STATUS_EDIT, L"" );
				}
				break;

			default:
				// アクセス種別不明
				break;
			}
		}
		break;

	default:
		// アクセス種別不明
		break;
	}

	// カテゴリに応じてTwitter送信モードを初期化
	MyResetTwitterStylePostMode();

	// コントロール状態の変更
	MyUpdateControlStatus();

	// フォーカスの変更
	MyUpdateFocus();

	// [MZ3-API] 通信終了(UI初期化完了)後のフック処理
	switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
	case AccessTypeInfo::INFO_TYPE_POST:
		switch (aType) {
		case ACCESS_TWITTER_UPDATE:
			// 投稿後にタイムラインを取得する
			if (theApp.m_optionMng.m_bTwitterReloadTLAfterPost) {
				RetrieveCategoryItem();
			}
			break;
		}
	}

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

			CMixiData& data = GetSelectedBodyItem();
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
		util::FormatString( L"@%sのタイムライン", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/user_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

/**
 * mixiエコー｜引用ユーザのエコー一覧
 */
void CMZ3View::OnMenuMixiEchoAddRefUserEchoList()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// タイムライン項目の追加
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	CString name = bodyItem.GetTextValue(L"ref_user_name");
	int author_id = _wtoi(bodyItem.GetTextValue(L"ref_user_id"));
	categoryItem.init( 
		// 名前
		util::FormatString( L"%sさんのエコー", name ),
		util::FormatString( L"http://mixi.jp/list_echo.pl?id=%d", author_id ), 
		ACCESS_MIXI_RECENT_ECHO, 
		m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_MIXI_RECENT_ECHO),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_MIXI_RECENT_ECHO),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_MIXI_RECENT_ECHO),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

/**
 * mixiエコー｜ユーザのエコー一覧
 */
void CMZ3View::OnMenuMixiEchoAddUserEchoList()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	// タイムライン項目の追加
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	CString name = bodyItem.GetName();
	int author_id = bodyItem.GetAuthorID();
	categoryItem.init( 
		// 名前
		util::FormatString( L"%sさんのエコー", name ),
		util::FormatString( L"http://mixi.jp/list_echo.pl?id=%d", author_id ), 
		ACCESS_MIXI_RECENT_ECHO, 
		m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_MIXI_RECENT_ECHO),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_MIXI_RECENT_ECHO),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_MIXI_RECENT_ECHO),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

/* Twitter 仕様変更により利用できないためコメントアウト
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
		util::FormatString( L"@%sのタイムライン", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/friends_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_TWITTER_FRIENDS_TIMELINE),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// 取得開始
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}
*/

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
	// バックバッファ経由で再描画
	m_bodyList.DrawDetail();
	m_bodyList.UpdateWindow();

	return true;
}

/// お気に入り追加
void CMZ3View::OnMenuTwitterCreateFavourings()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	static CMixiData s_data;
	s_data.SetAccessType( ACCESS_TWITTER_FAVOURINGS_CREATE );

	// URL 設定
	CString url;
	int id = GetSelectedBodyItem().GetID();
	url.Format( L"http://twitter.com/favourings/create/%d.xml", id );
	s_data.SetURL( url );
	s_data.SetBrowseUri( url );

	// 通信開始
	AccessProc( &s_data, s_data.GetURL() );
}

/// お気に入り削除
void CMZ3View::OnMenuTwitterDestroyFavourings()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	static CMixiData s_data;
	s_data.SetAccessType( ACCESS_TWITTER_FAVOURINGS_DESTROY );

	// URL 設定
	CString url;
	int id = GetSelectedBodyItem().GetID();
	url.Format( L"http://twitter.com/favourings/destroy/%d.xml", id );
	s_data.SetURL( url );
	s_data.SetBrowseUri( url );

	// 通信開始
	AccessProc( &s_data, s_data.GetURL() );
}

/// フォローする
void CMZ3View::OnMenuTwitterCreateFriendships()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	static CMixiData s_data;
	s_data.SetAccessType( ACCESS_TWITTER_FRIENDSHIPS_CREATE );

	CMixiData& item = GetSelectedBodyItem();
	CString msg = util::FormatString(L"%s さんをフォローします。よろしいですか？", (LPCTSTR)item.GetName());
	if (MessageBox(msg, NULL, MB_YESNO)!=IDYES) {
		return;
	}

	// URL 設定
	CString url;
	int id = item.GetOwnerID();
	url.Format( L"http://twitter.com/friendships/create/%d.xml", id );
	s_data.SetURL( url );
	s_data.SetBrowseUri( url );

	// 通信開始
	AccessProc( &s_data, s_data.GetURL() );
}

/// フォローやめる
void CMZ3View::OnMenuTwitterDestroyFriendships()
{
	if( m_access ) {
		// アクセス中は禁止
		return;
	}

	static CMixiData s_data;
	s_data.SetAccessType( ACCESS_TWITTER_FRIENDSHIPS_DESTROY );

	CMixiData& item = GetSelectedBodyItem();
	CString msg = util::FormatString(L"%s さんのフォローを解除します。よろしいですか？", (LPCTSTR)item.GetName());
	if (MessageBox(msg, NULL, MB_YESNO)!=IDYES) {
		return;
	}

	// URL 設定
	CString url;
	int id = item.GetOwnerID();
	url.Format( L"http://twitter.com/friendships/destroy/%d.xml", id );
	s_data.SetURL( url );
	s_data.SetBrowseUri( url );

	// 通信開始
	AccessProc( &s_data, s_data.GetURL() );
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

	// フルテンプレート生成
	Mz3GroupData template_data;
	template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

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
	dlg.SetMessage( L"カテゴリのタイトル/URLを入力してください" );
	dlg.mc_strEdit  = pCategoryItem->m_name;
	dlg.mc_strEdit2 = pCategoryItem->m_mixi.GetURL();
	if (dlg.DoModal()==IDOK) {
		pCategoryItem->m_name = dlg.mc_strEdit;
		pCategoryItem->m_mixi.SetURL( dlg.mc_strEdit2 );
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

/**
 * 状態に応じたコントロール状態の変更
 */
void CMZ3View::MyUpdateControlStatus(void)
{
	// 中止ボタン
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, m_access ? TRUE : FALSE );

	// プログレスバー
	if (m_access) {
		// 通信中は受信時に自動表示
	} else {
		mc_progressBar.ShowWindow( SW_HIDE );
	}

	// Twitter の更新ボタン
	CWnd* pUpdateButton = GetDlgItem( IDC_UPDATE_BUTTON );
	if (pUpdateButton!=NULL) {
		pUpdateButton->EnableWindow( m_access ? FALSE : TRUE );
	}

	// Twitter の入力領域
	CWnd* pStatusEdit = GetDlgItem( IDC_STATUS_EDIT );
	if (pStatusEdit!=NULL) {
		pStatusEdit->EnableWindow( m_access ? FALSE : TRUE );
	}

	// Twitterスタイルならボタンの名称をモードにより変更する
	switch (m_viewStyle) {
	case VIEW_STYLE_TWITTER:
		if (pUpdateButton!=NULL) {
			switch (m_twitterPostMode) {
			case TWITTER_STYLE_POST_MODE_DM:
				pUpdateButton->SetWindowTextW( L"DM" );
				break;
			case TWITTER_STYLE_POST_MODE_MIXI_ECHO:
				pUpdateButton->SetWindowTextW( L"echo" );
				break;
			case TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY:
				pUpdateButton->SetWindowTextW( L"返信" );
				break;
			case TWITTER_STYLE_POST_MODE_WASSR_UPDATE:
				pUpdateButton->SetWindowTextW( L"Wassr" );
				break;
			case TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE:
				pUpdateButton->SetWindowTextW( L"ひとこと" );
				break;
			case TWITTER_STYLE_POST_MODE_UPDATE:
			default:
				pUpdateButton->SetWindowTextW( L"更新" );
				break;
			}
		}
		break;
	}
}

/**
 * 右ボタン押下
 */
void CMZ3View::OnRButtonDown(UINT nFlags, CPoint point)
{
//	MZ3_TRACE( L"CMZ3View::OnRButtonDown\n" );

	// ジェスチャ開始
	theApp.m_pMouseGestureManager->StartGestureMode(point);

	// マウスキャプチャ開始
//	SetCapture();

	CFormView::OnRButtonDown(nFlags, point);
}

/**
 * 右ボタンリリース
 */
void CMZ3View::OnRButtonUp(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"CMZ3View::OnRButtonUp\n" );

	// マウスキャプチャ終了
//	ReleaseCapture();

#ifndef WINCE
	// MZ4 Only

	// 認識済みのジェスチャと比較
	CString strCmdList = CStringW(theApp.m_pMouseGestureManager->GetCmdList().c_str());
	MZ3_TRACE(L" gesture : %s\n", (LPCTSTR)strCmdList);

	if (strCmdList==L"R") {
		// 右：次のタブ
		OnAcceleratorNextTab();
	} else if (strCmdList==L"L") {
		// 左：前のタブ
		OnAcceleratorPrevTab();
	} else if (strCmdList==L"D") {
		// 下：カラム変更(暫定)
		MyChangeBodyHeader();
	} else if (strCmdList==L"U") {
		// 上：リロード
		OnAcceleratorReload();
	} else {
		// メッセージクリア
		m_infoEdit.SetWindowText( L"" );
//		util::MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_bodyColType1, false)
	}
#endif

	// ジェスチャ終了
	theApp.m_pMouseGestureManager->StopGestureMode();

	CFormView::OnRButtonUp(nFlags, point);
}

/**
 * 左ボタン押下
 */
void CMZ3View::OnLButtonDown(UINT nFlags, CPoint point)
{
	{
		CRect r;
		m_categoryList.GetWindowRect(&r);

		CPoint pt;
		GetCursorPos(&pt);

		int dy = pt.y-r.bottom;
		if (abs(dy)<SPLITTER_HEIGHT) {
			MZ3LOGGER_INFO(L"ドラッグ開始");

			// カーソル変更
			SetCursor(LoadCursor(NULL, IDC_SIZENS));

			// ドラッグ開始
			m_bDragging = true;
			m_ptDragStart = pt;

			// 子ウィンドウのドラッグ操作停止
			m_categoryList.m_bStopDragging = true;
			m_bodyList.m_bStopDragging = true;

			// キャプチャ開始
			SetCapture();

			return;
		}
	}
	CFormView::OnLButtonDown(nFlags, point);
}

/**
 * 左ボタンリリース
 */
void CMZ3View::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging) {
		{
			// スプリッター変更
			int& h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
			int& h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

			// オプション値を % に補正
			int sum = h1 + h2;
			if (sum>0) {
				h1 = (int)(h1 * 100.0 / sum);
				h2 = (int)(h2 * 100.0 / sum);
				sum = h1 + h2;
			}

			CRect rw;
			GetWindowRect(&rw);
			int cy = rw.Height();

			CPoint pt;
			GetCursorPos(&pt);

			int dy = pt.y - m_ptDragStart.y +3;

			int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
			int hGroup    = theApp.GetTabHeight(fontHeight);
			int hCategory = (cy * h1 / sum) - (hGroup -1);
			int mh1 = (int)(sum/(double)cy * (hCategory + dy +hGroup -1) - h1);
			MZ3LOGGER_DEBUG(util::FormatString(L"mh1 : %d  dy : %d", mh1, dy));

			h1 += mh1;
			h2 -= mh1;

			// 正規化
			if (h1 < N_HC_MIN || h2 > N_HB_MAX) {
				// 最小値に設定
				h1 = N_HC_MIN;
				h2 = N_HB_MAX;
			}
			if (h1 > N_HC_MAX || h2 < N_HB_MIN) {
				// 最小値に設定
				h1 = N_HC_MAX;
				h2 = N_HB_MIN;
			}

			// 再描画
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			pMainFrame->ChangeAllViewFont();
		}

		// ドラッグ終了処理
		MZ3LOGGER_INFO(L"ドラッグ終了");

		// キャプチャ終了
		ReleaseCapture();

		// マウスカーソルを元に戻す
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		// フラグクリア
		m_bDragging = false;

		// 子ウィンドウのドラッグ操作再開
		m_categoryList.m_bStopDragging = false;
		m_bodyList.m_bStopDragging = false;
	}
	CFormView::OnLButtonUp(nFlags, point);
}

/**
 * マウス移動
 */
void CMZ3View::OnMouseMove(UINT nFlags, CPoint point)
{
//	MZ3_TRACE( L"CMZ3View::OnMouseMove\n" );

#ifndef WINCE
	// MZ4 Only

	// スプリッタ
	{
		CRect r;
		m_categoryList.GetWindowRect(&r);

		CPoint pt;
		GetCursorPos(&pt);

		int dy = pt.y-r.bottom;
		if (m_bDragging || (dy < 0 && dy>-SPLITTER_HEIGHT)) {
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		} else {
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}

	// ジェスチャ
	if (theApp.m_pMouseGestureManager->IsGestureMode()) {

		// 閾値以上移動していれば処理実行
		const CPoint ptLastCmd = theApp.m_pMouseGestureManager->m_posLastCmd;

		int dx = point.x - ptLastCmd.x;
		int dy = point.y - ptLastCmd.y;

//		MZ3_TRACE( L" dx[%3d], dy[%3d]\n", dx, dy );

		// 閾値
		const int GESTURE_LIMIT_X = 20;
		const int GESTURE_LIMIT_Y = 20;

		MouseGestureManager::CMD cmd = MouseGestureManager::CMD_NONE;
		MouseGestureManager::CMD lastCmd = theApp.m_pMouseGestureManager->GetLastCmd();

		if (dx >= GESTURE_LIMIT_X) {
			// 右移動 : 1つ前と同じでなければ認識
			if (lastCmd != MouseGestureManager::CMD_RIGHT) {
				cmd = MouseGestureManager::CMD_RIGHT;
			}
		} else if (dx <= -GESTURE_LIMIT_X) {
			// 左移動 : 1つ前と同じでなければ認識
			if (lastCmd != MouseGestureManager::CMD_LEFT) {
				cmd = MouseGestureManager::CMD_LEFT;
			}
		}
		
		if (dy >= GESTURE_LIMIT_Y) {
			// 下移動 : 1つ前と同じでなければ認識
			if (lastCmd != MouseGestureManager::CMD_DOWN) {
				cmd = MouseGestureManager::CMD_DOWN;
			}
		} else if (dy <= -GESTURE_LIMIT_Y) {
			// 上移動 : 1つ前と同じでなければ認識
			if (lastCmd != MouseGestureManager::CMD_UP) {
				cmd = MouseGestureManager::CMD_UP;
			}
		}

		// 1つ前と同じでなければ登録
		if (cmd != MouseGestureManager::CMD_NONE &&	lastCmd!=cmd) {

			theApp.m_pMouseGestureManager->m_cmdList.push_back(cmd);
			theApp.m_pMouseGestureManager->m_posLastCmd = point;

			// ステータスバーに表示
			CString strCmdList = CStringW(theApp.m_pMouseGestureManager->GetCmdList().c_str());
			strCmdList.Replace(L"U", L"↑");
			strCmdList.Replace(L"D", L"↓");
			strCmdList.Replace(L"L", L"←");
			strCmdList.Replace(L"R", L"→");
			util::MySetInformationText( m_hWnd, strCmdList );
		}
	}
#endif
}

/**
 * ボディ系項目の取得完了処理
 *
 * @return bool 後続の処理を行わない場合に true を返す。
 */
bool CMZ3View::DoAccessEndProcForBody(ACCESS_TYPE aType)
{
	// ステータスコードチェック
	MZ3_TRACE(L"DoAccessEndProcForBody(), HTTP Status[%d], [%s]\n", 
		theApp.m_inet.m_dwHttpStatus,
		(LPCTSTR)CString(theApp.m_accessTypeInfo.getServiceType(aType).c_str()));
	LPCTSTR szStatusErrorMessage = NULL;	// 非NULLの場合はエラー発生
	if (theApp.m_accessTypeInfo.getServiceType(aType)=="Twitter") {
		// Twitter
		szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
	} else if (theApp.m_accessTypeInfo.getServiceType(aType)=="Wassr") {
		// Wassr
		if (theApp.m_inet.m_dwHttpStatus==200) {
			// OK
		} else {
			szStatusErrorMessage = L"不明なエラー";
		}
	}
	if (szStatusErrorMessage!=NULL) {
		CString msg = util::FormatString(L"サーバエラー(%d)：%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
		util::MySetInformationText( m_hWnd, msg );
		MZ3LOGGER_ERROR( msg );
		// 以降の処理を行わない。
		return false;
	}

	util::MySetInformationText( m_hWnd, _T("解析中 : 1/3") );

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
		return false;
	}

	// HTML の取得
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// 保存先 body の取得。
	CMixiData& parent_data = m_selGroup->getSelectedCategory()->m_mixi;
	CMixiDataList& body = m_selGroup->getSelectedCategory()->GetBodyList();

	// HTML 解析
	util::MySetInformationText( m_hWnd,  _T("解析中 : 2/3") );
	if (parser::MyDoParseMixiListHtml( aType, parent_data, body, html )) {

		// [MZ3-API] : パース後のフック処理(の予定)
		switch (aType) {
		case ACCESS_LIST_FRIEND:
//			MZ3_TRACE(L"★ACCESS_LIST_FRIEND\n");
			// マイミク一覧
			// list_friend.pl であれば、ajax_friend_setting.pl に変更して再リクエスト
			if (wcsstr(theApp.m_inet.GetURL(), L"list_friend.pl")!=NULL) {
				// 再リクエスト
				CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
				AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
				return false;
			} else {
				// 取得完了したので post_key を初期化しておく。
				CMixiDataList& body = m_selGroup->getSelectedCategory()->m_body;
				if (body.size()>0) {
//					MZ3_TRACE(L"  ★post_key, reset\n");
					body[0].SetTextValue(L"post_key", L"");
				}
			}
			break;

		case ACCESS_TWITTER_FRIENDS_TIMELINE:
			MZ3_TRACE(L"★ACCESS_TWITTER_FRIENDS_TIMELINE\n");
			// Twitter タイムライン、ページ変更(複数ページ取得)処理
			{
				CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
				int page = pCategoryItem->m_mixi.GetIntValue(L"request_page", 1);
				MZ3_TRACE(L"　★page=%d\n", page);

				// 新着件数を取得
				int new_count = parent_data.GetIntValue(L"new_count", 0);
				// ログファイルから取得していたデータは新着とみなす(ログより前のデータを取得するため)
				if (page==1) {
					size_t body_size = body.size();
					for (size_t i=0; i<body_size; i++) {
						if (body[i].GetIntValue(L"from_log_flag", 0)) {
							new_count ++;
							// 次回はカウントしないためフラグを下げとく
							body[i].SetIntValue(L"from_log_flag", 0);
						}
					}
				}

				MZ3_TRACE(L"　★新着件数=%d\n", new_count);

				// 最大ページ数未満で、かつ、新着件数が閾値よりも多い場合
				if (page<theApp.m_optionMng.m_nTwitterGetPageCount && new_count >= 20/2) {
//				if (page<50 && new_count >= 20/2) {
					// 次ページリクエスト
					page ++;
					pCategoryItem->m_mixi.SetIntValue(L"request_page", page);
					CString url = util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL());
					url.AppendFormat(L"%spage=%d", (url.Find('?')<0 ? L"?" : L"&"), page);

					static MZ3Data s_data;
					s_data = pCategoryItem->m_mixi;
					s_data.SetURL(url);
					s_data.SetBrowseUri(url);
					AccessProc(&s_data, url);
					return false;
				} else {
					// リクエストページ変数を初期化して終了
					pCategoryItem->m_mixi.SetIntValue(L"request_page", 1);
				}
			}
			break;
		}
	}

	// ボディ一覧の設定
	util::MySetInformationText( m_hWnd,  _T("解析中 : 3/3") );

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
			return true;
		}
	}

	return false;
}

/**
 * ビューの状態に応じてフォーカスを標準位置に戻す
 *
 * 主に通信完了時の復帰処理に用いる
 */
void CMZ3View::MyUpdateFocus(void)
{
	switch (m_viewStyle) {
	case VIEW_STYLE_TWITTER:
		{
			// Twitter関連のPOSTが完了したので、フォーカスを入力領域に移動する。
			// 但し、フォーカスがリストにある場合は移動しない。
			CWnd* pFocus = GetFocus();
			if (pFocus == NULL ||
				(pFocus->m_hWnd != m_categoryList.m_hWnd &&
				 pFocus->m_hWnd != m_bodyList.m_hWnd))
			{
				GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
			}
		}
		break;

	default:
		break;
	}
}

/**
 * 任意のURLを開く
 */
void CMZ3View::MyOpenUrl(void)
{
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

/**
 * 任意のローカルファイルを開く
 */
void CMZ3View::MyOpenLocalFile(void)
{
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

/**
 * RSS | 全文を読む
 */
void CMZ3View::OnMenuRssRead()
{
	CMixiData& data = GetSelectedBodyItem();

	CString item;

	// タイトル
	item = data.GetTitle();
	item.Append( L"\r\n" );

	// 本文
	CString body = data.GetBody();
	while( body.Replace( L"\r\n", L"" ) );
	item += body;
	item.Append( L"\r\n" );

	// 日付があれば追加
	if (!data.GetDate().IsEmpty()) {
		item.Append( L"----\r\n" );
		item.AppendFormat( L"%s\r\n", data.GetDate() );
	}

	// URL追加
	int n = (int)data.m_linkList.size();
	if( n > 0 ) {
		for( int i=0; i<n; i++ ) {
			// 追加
			item.AppendFormat( L"URL : %s", data.m_linkList[i].url );
		}
	}

	MessageBox( item, data.GetName() );
}

/*
 * OnCategoryOpen()
 * WM_COMMAND IDM_CATEBORY_OPEN
 * カテゴリのコンテキストメニュー「最新の一覧を取得する」
*/
void CMZ3View::OnCategoryOpen()
{
	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	int iItem = util::MyGetListCtrlSelectedItemIndex( m_categoryList );
	m_hotList = &m_categoryList;

	if (iItem<0) {
		return;
	}

	// カレントデータを取得
	int idx = (int)m_categoryList.GetItemData(iItem);
	m_selGroup->selectedCategory = idx;
	m_selGroup->focusedCategory  = idx;
	if (m_preCategory != iItem) {
		m_categoryList.SetActiveItem(iItem);
		m_categoryList.Update(m_preCategory);
		m_categoryList.Update(iItem);
	}

	// アクセス開始
	if (!RetrieveCategoryItem()) {
		return;
	}
}

/**
 * RSSフィードの追加
 */
void CMZ3View::OnAddRssFeedMenu()
{
	/*
	 * 1. ユーザに「RSS的なURL」を入力させる。
	 * 2. 1. のURLを取得する。
	 * 3. 取得したURLがRSSフィードであればそのまま追加して終了。タイトルを自動付与すること。
	 * 4. HTMLページであれば、RSS AutoDiscovery を実施する。
	 * 5. 4. が成功すればフィードを追加して終了。title タグからタイトルを自動付与すること。
	 * 6. 4. が失敗すればエラー出力して終了。
	 */
	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	CCommonEditDlg dlg;
	dlg.SetTitle( L"RSSフィードの追加" );
	dlg.SetMessage( L"RSSのURLを入力してください" );
	dlg.mc_strEdit  = L"http://";
	if (dlg.DoModal()==IDOK) {
		CString url = dlg.mc_strEdit;

		static MZ3Data s_data;
		s_data.SetAccessType( ACCESS_RSS_READER_AUTO_DISCOVERY );

		// URL 設定
		s_data.SetURL( url );
		s_data.SetBrowseUri( url );

		// 通信開始
		AccessProc( &s_data, s_data.GetURL(), CInetAccess::ENCODING_NOCONVERSION );
	}

}

void CMZ3View::OnMenuMixiEchoRead()
{
	CMixiData& data = GetSelectedBodyItem();

	// 本文を1行に変換して割り当て。
	CString item;

	CString v = data.GetBody();;
	while( v.Replace( L"\r\n", L"" ) );
	item.Append(v);
	item.Append(L"\r\n");
	item.Append(L"----\r\n");
	
	item.AppendFormat( L"name : %s\r\n", data.GetName() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );

	MessageBox( item, data.GetName() );
}

/**
 * mixiエコー | つぶやく
 */
void CMZ3View::OnMenuMixiEchoUpdate()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_MIXI_ECHO;

	// ボタン名称変更
	MyUpdateControlStatus();

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}


/**
 * mixiエコー | 返信
 */
void CMZ3View::OnMenuMixiEchoReply()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY;

	// ボタン名称変更
	MyUpdateControlStatus();

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

void CMZ3View::OnMenuMixiEchoShowProfile()
{
	static CMixiData s_data;

	s_data = GetSelectedBodyItem();
	s_data.SetAccessType(ACCESS_PROFILE);
	AccessProc( &s_data, util::CreateMixiUrl(s_data.GetURL()));
}

/**
 * 表示内容の再設定
 *
 * フォントやフォントサイズの変更時に呼び出される。
 */
void CMZ3View::ResetViewContent(void)
{
	// とりあえずカテゴリ変更時と同じ処理を実施。
	// これによりアイコンサイズの再設定が行われる
	OnMySelchangedCategoryList();
}

/**
 * 統合カラムモードの変更(トグル)
 */
void CMZ3View::OnAcceleratorToggleIntegratedMode()
{
	// 統合カラムモードの変更(トグル)
	theApp.m_optionMng.m_bBodyListIntegratedColumnMode = !theApp.m_optionMng.m_bBodyListIntegratedColumnMode;

	// レイアウト反映
	MySetLayout(0, 0);

	// ボディリスト選択位置の保存
	int selectedBody = 0;
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory) {
		selectedBody = pCategory->selectedBody;
	}

	// ボディリストの再構築
	OnMySelchangedCategoryList();

	// ボディリスト選択位置の復帰
	if (pCategory) {
		pCategory->selectedBody = selectedBody;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );
		m_bodyList.EnsureVisible(pCategory->selectedBody, FALSE);
	}
}

/**
 * Twitterスタイルの送信モードを、カテゴリのアクセス種別に応じて初期化する
 */
void CMZ3View::MyResetTwitterStylePostMode()
{
	MZ3_TRACE(L"MyResetTwitterStylePostMode()\n");

	// カテゴリの種別を優先して初期化
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory) {
		std::string strServiceType = theApp.m_accessTypeInfo.getServiceType(pCategory->m_mixi.GetAccessType());
		MZ3_TRACE(L" service_type : %s\n", (LPCTSTR)CString(strServiceType.c_str()));

		if (strServiceType == "Twitter") {
			// Twitter系
			m_twitterPostMode = TWITTER_STYLE_POST_MODE_UPDATE;
		} else if (strServiceType == "Wassr") {
			// Wassr系
			m_twitterPostMode = TWITTER_STYLE_POST_MODE_WASSR_UPDATE;
		} else if (strServiceType == "gooHome") {
			// Wassr系
			m_twitterPostMode = TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE;
		} else if (strServiceType == "RSS") {
			// RSS => 無視
		} else {
			// mixi系 => mixiエコー
			m_twitterPostMode = TWITTER_STYLE_POST_MODE_MIXI_ECHO;
		}
	} else {
		// デフォルトは Twitter
		m_twitterPostMode = TWITTER_STYLE_POST_MODE_UPDATE;
	}

	// 指定されたアクセス種別に応じて初期化
/*	switch (aType) {
	case ACCESS_MIXI_RECENT_ECHO:
	case ACCESS_MIXI_ADD_ECHO:
		// mixiエコーモード
		m_twitterPostMode = TWITTER_STYLE_POST_MODE_MIXI_ECHO;
		break;

	case ACCESS_WASSR_UPDATE:
		// Wassr
		m_twitterPostMode = TWITTER_STYLE_POST_MODE_WASSR_UPDATE;
		break;

	default:
		// Twitter「更新」に戻す
		m_twitterPostMode = TWITTER_STYLE_POST_MODE_UPDATE;
		break;
	}
*/
}

/**
 * Wassr | 読む
 */
void CMZ3View::OnMenuWassrRead()
{
	CMixiData& data = GetSelectedBodyItem();

	// 本文を1行に変換して割り当て。
	CString item;

	CString v = data.GetBody();;
	while( v.Replace( L"\r\n", L"" ) );
	item.Append(v);
	item.Append(L"\r\n");
	item.Append(L"----\r\n");
	
	item.AppendFormat( L"name : %s\r\n", data.GetName() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );

	MessageBox( item, data.GetName() );
}

/**
 * Wassr | つぶやく
 */
void CMZ3View::OnMenuWassrUpdate()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_WASSR_UPDATE;

	// ボタン名称変更
	MyUpdateControlStatus();

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

/**
 * URLをクリップボードにコピー
 */
void CMZ3View::OnCopyClipboardUrl(UINT nID)
{
	const CMixiData& data = GetSelectedBodyItem();

	UINT idx = nID - (ID_REPORT_COPY_URL_BASE+1);
	if( idx > data.m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = data.m_linkList[idx].url;

	// クリップボードにコピー
	util::SetClipboardDataTextW( url );

}

/**
 * RSS AutoDiscovery URL 取得後の処理
 */
bool CMZ3View::DoAccessEndProcForRssAutoDiscovery(void)
{
	// まずは RSSフィードであるか確認する。

	// HTML の取得
	std::vector<unsigned char> text;
	util::LoadDownloadedFile(text, theApp.m_filepath.temphtml);

	// 文字コード変換
	// とりあえず UTF-8 固定とする
	/*
	std::vector<unsigned char> out_text;
	out_text.reserve( text.size() );
	kfm::kfm k( text, out_text );
	k.tosjis();
	*/
	std::vector<TCHAR> out_text;
	kfm::utf8_to_ucs2( text, out_text );


	CMixiDataList dummy_list;
	CString title;

	int nAppendedFeed = 0;
	if (parser::RssFeedParser::parse( dummy_list, out_text, &title )) {
		MZ3LOGGER_INFO( util::FormatString(L"RSS だったので追加するよ。url[%s], title[%s]", 
			theApp.m_inet.GetURL(), title) );

		// 項目の追加
		CCategoryItem categoryItem;
		categoryItem.init( 
			// 名前
			title,
			theApp.m_inet.GetURL(), 
			ACCESS_RSS_READER_FEED, 
			m_selGroup->categories.size()+1,
			theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_RSS_READER_FEED),
			theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_RSS_READER_FEED),
			theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_RSS_READER_FEED));
		AppendCategoryList(categoryItem);

		nAppendedFeed = 1;
	} else {
		MZ3LOGGER_INFO( L"RSS じゃないので、RSS AutoDiscovery してみるよ" );

		CMixiDataList items;
		if (parser::RssAutoDiscoveryParser::parse( items, out_text ) &&
			items.size()>0)
		{
			// items の登録確認
			for (u_int i=0; i<items.size(); i++) {
				// 項目の追加
				CCategoryItem categoryItem;
				categoryItem.init( 
					// 名前
					items[i].GetTitle(),
					items[i].GetURL(), 
					ACCESS_RSS_READER_FEED, 
					m_selGroup->categories.size()+1,
					theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_RSS_READER_FEED),
					theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_RSS_READER_FEED),
					theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_RSS_READER_FEED));
				AppendCategoryList(categoryItem);
			}

			nAppendedFeed = items.size();
		}
	}
	
	switch (nAppendedFeed) {
	case 0:
		MessageBox( L"RSS が見つかりませんでした" );
		break;
	case 1:
		MessageBox( L"RSS を追加しました" );
		break;
	default:
		MessageBox( util::FormatString(L"%d 個の RSS を追加しました", nAppendedFeed) );
		break;
	}

	return true;
}

/**
 * バージョンチェック開始
 */
bool CMZ3View::DoCheckSoftwareUpdate(void)
{
	if (m_access) {
		// アクセス中は再アクセス不可
		return false;
	}

	static CMixiData s_data;
	s_data = CMixiData();
	s_data.SetAccessType(ACCESS_SOFTWARE_UPDATE_CHECK);
	AccessProc( &s_data, theApp.m_accessTypeInfo.getDefaultCategoryURL(ACCESS_SOFTWARE_UPDATE_CHECK));

	return true;
}

/**
 * バージョンチェック URL 取得後の処理
 */
bool CMZ3View::DoAccessEndProcForSoftwareUpdateCheck(void)
{
	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromFile(root, theApp.m_filepath.temphtml)) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	// バージョン、URL、タイトルの取得
	CString strLatestVersion, strUrl, strTitle;
	try {
#ifdef WINCE
		const xml2stl::Node& target = root.getNode(L"latest_version").getNode(L"mz3");
#else
		const xml2stl::Node& target = root.getNode(L"latest_version").getNode(L"mz4");
#endif
		strLatestVersion = target.getProperty(L"version").c_str();
		strUrl           = target.getProperty(L"url").c_str();
		strTitle         = target.getProperty(L"title").c_str();
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		return false;
	}

	// バージョンチェック
	MZ3LOGGER_DEBUG(
		util::FormatString(L"バージョンチェック結果：current[%s], latest_version[%s], url[%s], title[%s]",
			MZ3_VERSION_TEXT_SHORT,
			strLatestVersion,
			strUrl,
			strTitle));

	// バージョン番号の正規化
	// 0.9.3.7       => 0.9310700
	CString strCurrentVersionR = theApp.MakeMZ3RegularVersion(MZ3_VERSION_TEXT_SHORT);
	CString strLatestVersionR  = theApp.MakeMZ3RegularVersion(strLatestVersion);

	MZ3LOGGER_DEBUG(util::FormatString(L"正規化バージョン番号：current[%s]", strCurrentVersionR));
	MZ3LOGGER_DEBUG(util::FormatString(L"正規化バージョン番号：latest [%s]", strLatestVersionR));

	if (strLatestVersionR > strCurrentVersionR) {
		// 新バージョンあり
		CString msg;
		msg.Format(L"新しいバージョン(%s)が利用できます。\n今すぐダウンロードしてもよろしいですか？", strTitle);
		if (MessageBox(msg, NULL, MB_YESNO | MB_ICONQUESTION)==IDYES) {
			// MZ4はダウンロード
			static CMixiData s_data;
			s_data = CMixiData();
			s_data.SetAccessType(ACCESS_DOWNLOAD);

			// アクセス開始
			m_access = TRUE;
			m_abort = FALSE;

			// コントロール状態の変更
			MyUpdateControlStatus();

			// ダウンロードファイルパス
			theApp.m_inet.Initialize( m_hWnd, &s_data );
			theApp.m_accessType = s_data.GetAccessType();
			theApp.m_inet.DoGet(strUrl, _T(""), CInetAccess::FILE_BINARY);
		}
	} else {
		// 最新バージョン
		MessageBox(L"新しいバージョンはありませんでした。", NULL, MB_ICONINFORMATION);
	}

	return true;
}

/**
 * gooHome | 読む
 */
void CMZ3View::OnMenuGoohomeRead()
{
	CMixiData& data = GetSelectedBodyItem();

	// 本文を1行に変換して割り当て。
	CString item;

	CString v = data.GetBody();;
	while( v.Replace( L"\r\n", L"" ) );
	item.Append(v);
	item.Append(L"\r\n");
	item.Append(L"----\r\n");
	
	item.AppendFormat( L"name : %s\r\n", data.GetName() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );

	MessageBox( item, data.GetName() );
}

/**
 * gooHome | つぶやく
 */
void CMZ3View::OnMenuGoohomeUpdate()
{
	// モード変更
	m_twitterPostMode = TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE;

	// ボタン名称変更
	MyUpdateControlStatus();

	// フォーカス移動。
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

/**
 * gooHome | コメントを読む
 */
void CMZ3View::OnMenuGoohomeReadComments()
{
	CMixiData& data = GetSelectedBodyItem();

	util::OpenBrowserForUrl(data.GetURL());
}

/// 「カテゴリログのリロード」メニュー
void CMZ3View::ReloadCategoryListLog()
{
	// ログのロード
	MyLoadCategoryLogfile( *m_selGroup->getSelectedCategory() );

	// ボディリストに設定
	SetBodyList( m_selGroup->getSelectedCategory()->GetBodyList() );
}
