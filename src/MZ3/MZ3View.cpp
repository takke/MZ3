/// MZ3View.cpp : CMZ3View クラスの実装

#include "stdafx.h"
#include "MZ3.h"

#include "MZ3Doc.h"
#include "MZ3View.h"

#include "MixiData.h"
#include "CategoryItem.h"
#include "HtmlArray.h"
#include "ReportView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "util.h"
#include "MixiParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

inline CString MyGetItemByBodyColType( CMixiData* data, CCategoryItem::BODY_INDICATE_TYPE bodyColType )
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
	return item.Left( 30 );
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
		return L"";
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
	default:
		return L"";
	}
}


// CMZ3View

IMPLEMENT_DYNCREATE(CMZ3View, CFormView)

BEGIN_MESSAGE_MAP(CMZ3View, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_HEADER_LIST, &CMZ3View::OnNMClickCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_HEADER_LIST, &CMZ3View::OnNMDblclkCategoryList)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HEADER_LIST, &CMZ3View::OnLvnItemchangedCategoryList)
    
	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
	ON_EN_SETFOCUS(IDC_INFO_EDIT, &CMZ3View::OnEnSetfocusInfoEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_BODY_LIST, &CMZ3View::OnNMDblclkBodyList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BODY_LIST, &CMZ3View::OnLvnItemchangedBodyList)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
	ON_COMMAND(ID_WRITE_DIARY, &CMZ3View::OnWriteDiary)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_NOTIFY(NM_SETFOCUS, IDC_BODY_LIST, &CMZ3View::OnNMSetfocusBodyList)
	ON_COMMAND(ID_OPEN_BROWSER, &CMZ3View::OnOpenBrowser)
	ON_NOTIFY(NM_SETFOCUS, IDC_HEADER_LIST, &CMZ3View::OnNMSetfocusHeaderList)
	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CMZ3View::OnShowDebugInfo)
	ON_COMMAND(ID_GET_ALL, &CMZ3View::OnGetAll)
	ON_COMMAND(ID_GET_LAST10, &CMZ3View::OnGetLast10)
	ON_COMMAND(ID_VIEW_LOG, &CMZ3View::OnViewLog)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CMZ3View::OnHdnItemclickBodyList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GROUP_TAB, &CMZ3View::OnTcnSelchangeGroupTab)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CMZ3View::OnOpenBrowserUser)
	ON_COMMAND(ID_OPEN_INTRO, &CMZ3View::OnOpenIntro)
	ON_COMMAND(ID_OPEN_SELFINTRO, &CMZ3View::OnOpenSelfintro)
	ON_COMMAND(IDM_SET_NO_READ, &CMZ3View::OnSetNoRead)
	ON_COMMAND(IDM_VIEW_BBS_LIST, &CMZ3View::OnViewBbsList)
	ON_COMMAND(IDM_VIEW_BBS_LIST_LOG, &CMZ3View::OnViewBbsListLog)
	ON_COMMAND(IDM_CRUISE, &CMZ3View::OnCruise)
	ON_COMMAND(IDM_CHECK_CRUISE, &CMZ3View::OnCheckCruise)
	ON_COMMAND(ID_SEND_NEW_MESSAGE, &CMZ3View::OnSendNewMessage)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CMZ3View::OnHdnEndtrackHeaderList)
	ON_WM_SETTINGCHANGE()
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_NARROW, &CMZ3View::OnLayoutCategoryMakeNarrow)
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_WIDE, &CMZ3View::OnLayoutCategoryMakeWide)
END_MESSAGE_MAP()

// CMZ3View コンストラクション/デストラクション

/**
 * コンストラクタ
 */
CMZ3View::CMZ3View()
	: CFormView(CMZ3View::IDD)
	, m_dwLastReturn( 0 )
	, m_nKeydownRepeatCount( 0 )
{
	m_preCategory = 0;

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

	m_login = FALSE;
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

	// 初期化スレッド開始
	AfxBeginThread( Initialize_Thread, this );
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
	m_groupTab.SetCurSel( 0 );

	// 選択中のグループ項目の設定
	m_selGroup = &theApp.m_root.groups[0];

	// カテゴリーリストを初期化する
	MyUpdateCategoryListByGroupItem();

//	m_categoryList.SetFocus();
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
	int hInfo     = theApp.GetInfoRegionHeight(fontHeight);

	// グループタブ
	int hGroup    = fontHeight -2;				// デフォルト値
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		if( theApp.GetDPI() > 96 ) {
			// VGA かつ非RealVGA環境
			hGroup    = fontHeight +12;
		}else{
			// VGA かつRealVGA環境
			hGroup    = fontHeight + 8;
		}
		break;

	case SR_QVGA:
		if( theApp.m_bSmartphone ) {
			// Smartphone/Standard Edition 環境
			hGroup    = fontHeight + 8;
		}
		break;
	}
#else
	// for win32
	hGroup = fontHeight + 12;
#endif

	// カテゴリ、ボディリストの領域を % で指定
	// （但し、カテゴリリストはグループタブを、ボディリストは情報領域を含む）
	const int h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	int hCategory = (cy * h1 / (h1+h2)) - (hGroup - 1);
	int hBody     = (cy * h2 / (h1+h2)) - (hInfo - 1);

	int y = 0;
	util::MoveDlgItemWindow( this, IDC_GROUP_TAB,   0, y, cx, hGroup    );
	y += hGroup;
	util::MoveDlgItemWindow( this, IDC_HEADER_LIST, 0, y, cx, hCategory );
	y += hCategory;
	util::MoveDlgItemWindow( this, IDC_BODY_LIST,   0, y, cx, hBody     );
	y += hBody;
	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,   0, y, cx, hInfo     );

	// プログレスバーは別途配置
	// サイズは hInfo の 2/3 とする
	int hProgress = hInfo * 2 / 3;
	y = cy - hInfo - hProgress;
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

	m_hotList = &m_categoryList;
	if (lpnmlv->iItem<0) {
		return;
	}

	// カテゴリリスト中の「現在選択されている項目」を更新
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

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	TRACE(_T("http://mixi.jp/%s\n"), pCategory->m_mixi.GetURL());

	if (pCategory->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
		theApp.m_root.GetBookmarkList().clear();
		theApp.m_bookmarkMng.Load( theApp.m_root.GetBookmarkList() );
		SetBodyList( pCategory->GetBodyList() );
		return;
	}else{
		// インターネットにアクセス
		AccessProc( &pCategory->m_mixi, util::CreateMixiUrl(pCategory->m_mixi.GetURL()));
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
 * アクセス終了通知受信
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("HTML解析中") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return LRESULT();
	}

	if (lParam == NULL) {
		// データがＮＵＬＬの場合
		LPCTSTR msg = L"内部エラーが発生しました(戻り値＝NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return LRESULT();
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	// ログインページ以外であれば、最初にログアウトチェックを行っておく
	if (aType != ACCESS_LOGIN ) {
		// HTML の取得

		// ログアウトチェック
		if (mixi::LoginPageParser::isLogout(theApp.m_filepath.temphtml) ) {

			// ログアウト状態になっている
//			MessageBox(_T("ログアウトしてます\n"));
			MZ3LOGGER_DEBUG(_T("再度ログインしてからデータを取得します。\n"));

			util::MySetInformationText( m_hWnd, L"再度ログインしてからデータを取得します" );

			// mixi データを保存（待避）
			theApp.m_mixiBeforeRelogin = *data;
			data->SetURL( theApp.MakeLoginUrl() );

			// ログイン実行
			data->SetAccessType( ACCESS_LOGIN );
			AccessProc( data, data->GetURL() );

			return LRESULT();
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
				m_login = TRUE;
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
				return LRESULT();
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
				return LRESULT();
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

				m_checkNewComment = FALSE;

				theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

				break;
			}else{
				// 新着メッセージ以外なので、ログインのための取得だった。

				// データを待避データに戻す
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return LRESULT();
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
		// --------------------------------------------------
		// カテゴリ項目の取得
		// --------------------------------------------------
		{
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
			body.clear();

			// HTML 解析
			util::MySetInformationText( m_hWnd,  _T("HTML解析中 : 2/3") );
			mixi::MyDoParseMixiHtml( aType, body, html );

			// ボディ一覧の設定
			util::MySetInformationText( m_hWnd,  _T("HTML解析中 : 3/3") );

			// 取得時刻文字列の作成
			CString timeStr;
			{
				SYSTEMTIME localTime;
				GetLocalTime(&localTime);
				timeStr.Format( _T("%02d/%02d %02d:%02d:%02d"),
					localTime.wMonth,
					localTime.wDay,
					localTime.wHour,
					localTime.wMinute,
					localTime.wSecond);
			}
			m_selGroup->getSelectedCategory()->SetAccessTime( timeStr );
			m_categoryList.SetItemText( m_selGroup->selectedCategory, 1, m_selGroup->getSelectedCategory()->GetAccessTime() );
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
			MyParseMixiHtml( theApp.m_filepath.temphtml, *data );
			MyShowReportView( *data );
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

	return LRESULT();
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
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	MZ3LOGGER_ERROR( msg );

	m_access = FALSE;

	// プログレスバーを非表示
	mc_progressBar.ShowWindow( SW_HIDE );

	return LRESULT();
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

	return LRESULT();
}

/**
 * 中断処理
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

	return LRESULT();
}

/**
 * アクセス情報通知
 */
LRESULT CMZ3View::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText( *(CString*)lParam );
	return LRESULT();
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
	LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi, pCategory->m_secondBodyColType );
	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
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
	}

	// アイテムの追加
	INT_PTR count = body.size();
	for (int i=0; i<count; i++) {
		CMixiData* data = &body[i];

		// １カラム目
		// どの項目を与えるかは、カテゴリ項目データ内の種別で決める
		int index = m_bodyList.InsertItem( i, MyGetItemByBodyColType(data,pCategory->m_firstBodyColType) );
		// ２カラム目
		m_bodyList.SetItemText( index, 1, MyGetItemByBodyColType(data,pCategory->m_secondBodyColType) );

		// ボディの項目の ItemData にも CMixiData を割り当てる。
		m_bodyList.SetItemData( index, (DWORD_PTR)data );
	}

	m_nochange = FALSE;
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;
	m_bodyList.Invalidate( FALSE );
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
	*pResult = 0;

	if (m_access) {
		// アクセス中は再アクセス不可
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	// カレントデータを取得
	m_hotList = &m_bodyList;
	m_selGroup->getSelectedCategory()->selectedBody = lpnmlv->iItem;

	CMixiData* data = (CMixiData*)m_bodyList.GetItemData(lpnmlv->iItem);
	TRACE(_T("http://mixi.jp/%s\n"), data->GetURL());

	if (data->GetAccessType() == ACCESS_LIST_FOOTSTEP) {
		return;
	}

	AccessProc(data, util::CreateMixiUrl(data->GetURL()));
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
	m_selGroup->getSelectedCategory()->selectedBody = pNMLV->iItem;

	// 第1カラムに表示している内容を表示する。
//	m_infoEdit.SetWindowText( GetSelectedBodyItem().GetTitle() );
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType) );

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
		// TODO ヘルプ表示
#endif
		break;
	case VK_F2:
#ifndef WINCE
	case VK_APPS:
#endif
		if( GetFocus() == &m_bodyList ) {
			// ボディリストでの右クリックメニュー
			PopupBodyMenu();
		}else{
			// カテゴリリストでの右クリック
			RECT rect;
#ifdef WINCE
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
#else
			GetWindowRect(&rect);
#endif

			POINT pt;
			pt.x = rect.left + (rect.right-rect.left) / 2;
			pt.y = rect.top  + (rect.bottom-rect.top) / 2;
			CMenu menu;
			menu.LoadMenu(IDR_CATEGORY_MENU);
			CMenu* pSubMenu = menu.GetSubMenu(0);

			// 巡回対象以外のカテゴリであれば巡回メニューを無効化する
			switch( m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() ) {
			case ACCESS_LIST_NEW_BBS:
			case ACCESS_LIST_NEWS:
			case ACCESS_LIST_MESSAGE_IN:
			case ACCESS_LIST_MESSAGE_OUT:
			case ACCESS_LIST_DIARY:
			case ACCESS_LIST_MYDIARY:
			case ACCESS_LIST_BBS:
				// 巡回対象なので巡回メニューを無効化しない
				break;
			default:
				// 巡回メニューを無効化する
				pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
				pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
				break;
			}

			// 巡回予約済みであればチェックを付ける。
			if( m_selGroup->getFocusedCategory()->m_bCruise ) {
				pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
			}else{
				pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
			}

			// メニュー表示
			pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);
		}
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

	default:
		break;
	}

	// Xcrawl Canceler
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeyup( pMsg->wParam ) ) {
			// キャンセルされたので上下キーを無効にする。
	//		util::MySetInformationText( GetSafeHwnd(), L"Xcrawl canceled..." );
			return TRUE;
		}
	}

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
	}

	return FALSE;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP) {
		BOOL r = OnKeyUp( pMsg );

		// KEYDOWN リピート回数を初期化
		m_nKeydownRepeatCount = 0;

		return r;
	}
	else if (pMsg->message == WM_KEYDOWN) {
		// KEYDOWN リピート回数をインクリメント
		m_nKeydownRepeatCount ++;

		return OnKeyDown( pMsg );
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * 他ビューからの復帰処理
 */
LRESULT CMZ3View::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	m_hotList->SetFocus();
	util::MySetListCtrlItemFocusedAndSelected( *m_hotList, m_selGroup->getSelectedCategory()->selectedBody, true );
	theApp.ChangeView(theApp.m_pMainView);

	// アイテムの制御
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);

	// 書き込みボタン
	// 自分の日記の場合は有効に。
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON,
		(m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY));

	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	return LRESULT();
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
		m_infoEdit.SetWindowText( GetSelectedBodyItem().GetTitle() );

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
			// アクセス中は再アクセス不可
			if (m_access) {
				return TRUE;
			}
			if (m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
				SetBodyList( m_selGroup->getFocusedCategory()->GetBodyList() );
				return TRUE;
			}
			// インターネットにアクセス
			m_hotList = &m_bodyList;
			AccessProc( 
				&m_selGroup->getFocusedCategory()->m_mixi, 
				util::CreateMixiUrl(m_selGroup->getFocusedCategory()->m_mixi.GetURL()));
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
			// メニュー表示
			PopupBodyMenu();
			break;
		case ACCESS_PROFILE:
			// プロフィールなら、カテゴリ項目に応じて処理を変更する。（暫定）
			switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
			case ACCESS_LIST_INTRO:			// 紹介文
			case ACCESS_LIST_FAVORITE:		// お気に入り
			case ACCESS_LIST_FOOTSTEP:		// 足あと
			case ACCESS_LIST_FRIEND:		// マイミク一覧
				// 操作をメニューで選択
				{
					RECT rect;
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

					POINT pt;
					pt.x = (rect.right-rect.left) / 2;
					pt.y = (rect.bottom-rect.top) / 2;
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

					// メニューを開く
					pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN,
						pt.x,
						pt.y,
						this);
				}

				break;
			}
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
			int idxSel = m_selGroup->getSelectedCategory()->selectedBody;
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
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
		m_selGroup->getSelectedCategory()->selectedBody --;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, true );

		// 移動先が非表示なら上方向にスクロール
		if( !util::IsVisibleOnListBox( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody ) ) {
			m_bodyList.Scroll( CSize(0, -m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

			// 再描画
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
				m_bodyList.UpdateWindow();
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
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
		m_selGroup->getSelectedCategory()->selectedBody ++;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, true );

		// 移動先が非表示なら下方向にスクロール
		if( !util::IsVisibleOnListBox( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody ) ) {
			m_bodyList.Scroll( CSize(0, m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

			// 再描画
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
				m_bodyList.UpdateWindow();
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
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, 
		(GetSelectedBodyItem().GetAccessType() == ACCESS_LIST_FOOTSTEP) );

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
void CMZ3View::AccessProc(CMixiData* data, LPCTSTR a_url)
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

	data->SetBrowseUri(uri);

	// 中止ボタンを使用可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// アクセス開始
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, data );
	theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML );
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

	// 選択項目が「未取得」なら、とりあえずファイルから取得する
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if( wcscmp( pCategory->GetAccessTime(), L"" ) == 0 ) {
		MyLoadCategoryLogfile( *pCategory );
	}

	// ボディリストに設定
	SetBodyList( pCategory->GetBodyList() );
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
	CString time = status.m_mtime.Format( L"%m/%d %H:%M:%S log" );

	{
		CMixiDataList& body = category.GetBodyList();
		body.clear();

		CString msgHead;
		msgHead.Format( L"%s : ", util::AccessType2Message(category.m_mixi.GetAccessType()) );

		// HTML の取得
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML 解析
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 2/3") );
		mixi::MyDoParseMixiHtml( category.m_mixi.GetAccessType(), body, html );

		// ボディ一覧の設定
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML解析中 : 3/3") );

		// 取得時刻文字列の設定
		category.SetAccessTime( time );
		m_categoryList.SetItemText( category.GetIndexOnList(), 1, category.GetAccessTime() );

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
	MyParseMixiHtml( strLogfilePath, mixi );

	// URL 設定
	mixi.SetBrowseUri( util::CreateMixiUrl(GetSelectedBodyItem().GetURL()) );

	// 表示
	MyShowReportView( mixi );
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
	theApp.m_pReportView->SetData( &mixi );

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
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_BOOKMARK:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_INTRO:
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
		m_categoryList.SetItemText( i, 1, category.GetAccessTime() );

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
	m_categoryList.Invalidate( FALSE );
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

	// メモリ上の lastIndex を初期化
	mixi.SetLastIndex( -1 );

	// ログINIファイルの項目を初期化
	// （本当は項目ごと削除すべき）
	theApp.m_logfile.SetValue( (LPCSTR)util::int2str_a(mixi.GetID()), "-1", "Log" );

	// ログファイル削除
	DeleteFile( util::MakeLogfilePath(mixi) );

	// ビューを更新
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// ボディリストでの右クリックメニュー
bool CMZ3View::PopupBodyMenu(void)
{
	// 右クリックメニュー表示位置
	RECT rect;
#ifdef WINCE
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
#else
	GetWindowRect(&rect);
#endif

	POINT pt;
	pt.x = rect.left + (rect.right-rect.left) / 2;
	pt.y = rect.top  + (rect.bottom-rect.top) / 2;

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
			pSubMenu->TrackPopupMenu( TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this );
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
			pSubMenu->TrackPopupMenu( TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this );
		}
		break;
	}
	return true;
}

/// トピック一覧の閲覧準備
bool CMZ3View::PrepareViewBbsList(void)
{
	CMixiData& bodyItem = GetSelectedBodyItem();

	// グループにコミュニティ専用項目を追加する。既にあれば取得する。
	CCategoryItem* pCategoryItem = NULL;
	for( u_int i=0; i<m_selGroup->categories.size(); i++ ) {
		CCategoryItem& category = m_selGroup->categories[i];
		if( category.m_mixi.GetAccessType() == ACCESS_LIST_BBS ) {
			// 該当項目発見。
			pCategoryItem = &category;
			break;
		}
	}
	// 未発見なら追加
	if( pCategoryItem == NULL ) {
		CCategoryItem category;
		m_selGroup->categories.push_back( category );
		pCategoryItem = &m_selGroup->categories[ m_selGroup->categories.size()-1 ];
	}

	// 初期化
	
	// URL はボディのアイテムからidを引き継ぐ。
	CString url;
	url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
	// 名前は分かるようにしておく
	CString name;
	name.Format( L"└%s", bodyItem.GetName() );
	pCategoryItem->init( name, url, ACCESS_LIST_BBS, m_selGroup->categories.size(),
		CCategoryItem::BODY_INDICATE_TYPE_TITLE,
		CCategoryItem::BODY_INDICATE_TYPE_DATE );

	// タブの初期化
	MyUpdateCategoryListByGroupItem();

	// カテゴリの選択項目を再表示。
	{
		int idxLast = m_selGroup->focusedCategory;
		int idxNew  = m_categoryList.GetItemCount()-1;

		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxLast, false );
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxNew, true );
		m_selGroup->focusedCategory  = idxNew;
		m_selGroup->selectedCategory = idxNew;
		m_categoryList.SetActiveItem( idxNew );
		m_categoryList.Update( idxLast );
		m_categoryList.Update( idxNew );
	}

	// フォーカスをカテゴリリストに。
	m_categoryList.SetFocus();

	// ボディリストは消去しておく。
	m_bodyList.DeleteAllItems();
	m_bodyList.SetRedraw(TRUE);
	m_bodyList.Invalidate( FALSE );

	return true;
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
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 35;
		break;
	case SR_QVGA:
	default:
		w -= 35/2;
		break;
	}
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
				if (mixi.GetLastIndex() == -1) {
					// 全くの未読
					unread = true;
				} else if (mixi.GetLastIndex() >= mixi.GetCommentCount()) {
					// 更新なし
					unread = false;
				} else {
					// 未読あり
					unread = true;
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
