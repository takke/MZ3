/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3GroupFileEditorDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3GroupFileEditor.h"
#include "MZ3GroupFileEditorDlg.h"
#include "../MZ3/util_base.h"
#include "../MZ3/util_gui.h"
#include "NameDlg.h"
#include "CategorySettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMZ3GroupFileEditorDlg ダイアログ




CMZ3GroupFileEditorDlg::CMZ3GroupFileEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMZ3GroupFileEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMZ3GroupFileEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CATEGORY_LIST, mc_listCategory);
	DDX_Control(pDX, IDC_TAB1, mc_tab);
}

BEGIN_MESSAGE_MAP(CMZ3GroupFileEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_EXIT_APP, &CMZ3GroupFileEditorDlg::OnExitApp)
	ON_COMMAND(ID_ABOUT, &CMZ3GroupFileEditorDlg::OnAbout)
	ON_COMMAND(ID_OPEN_FILE, &CMZ3GroupFileEditorDlg::OnOpenFile)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMZ3GroupFileEditorDlg::OnTcnSelchangeTab1)
	ON_NOTIFY(NM_RCLICK, IDC_TAB1, &CMZ3GroupFileEditorDlg::OnNMRclickTab1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CMZ3GroupFileEditorDlg::OnLvnItemchangedCategoryList)
	ON_NOTIFY(TCN_FOCUSCHANGE, IDC_TAB1, &CMZ3GroupFileEditorDlg::OnTcnFocusChangeTab1)
	ON_NOTIFY(NM_SETFOCUS, IDC_CATEGORY_LIST, &CMZ3GroupFileEditorDlg::OnNMSetfocusCategoryList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_CATEGORY_LIST, &CMZ3GroupFileEditorDlg::OnNMKillfocusCategoryList)
	ON_WM_KEYUP()
	ON_COMMAND(ID_ACCELERATOR_MOVE_DOWN, &CMZ3GroupFileEditorDlg::OnAcceleratorMoveDown)
	ON_COMMAND(ID_ACCELERATOR_MOVE_LEFT, &CMZ3GroupFileEditorDlg::OnAcceleratorMoveLeft)
	ON_COMMAND(ID_ACCELERATOR_MOVE_RIGHT, &CMZ3GroupFileEditorDlg::OnAcceleratorMoveRight)
	ON_COMMAND(ID_ACCELERATOR_MOVE_UP, &CMZ3GroupFileEditorDlg::OnAcceleratorMoveUp)
	ON_COMMAND(ID_ACCELERATOR_TAB_LEFT, &CMZ3GroupFileEditorDlg::OnAcceleratorTabLeft)
	ON_COMMAND(ID_ACCELERATOR_TAB_RIGHT, &CMZ3GroupFileEditorDlg::OnAcceleratorTabRight)
	ON_COMMAND(ID_ACCELERATOR_SAVE, &CMZ3GroupFileEditorDlg::OnAcceleratorSave)
	ON_COMMAND(ID_FILE_SAVE, &CMZ3GroupFileEditorDlg::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMZ3GroupFileEditorDlg::OnUpdateFileSave)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_COMMAND(ID_ADD_TAB, &CMZ3GroupFileEditorDlg::OnAddTab)
	ON_COMMAND(ID_DELETE_TAB, &CMZ3GroupFileEditorDlg::OnDeleteTab)
	ON_COMMAND(ID_TAB_MOVE_LEFT, &CMZ3GroupFileEditorDlg::OnTabMoveLeft)
	ON_COMMAND(ID_TAB_MOVE_RIGHT, &CMZ3GroupFileEditorDlg::OnTabMoveRight)
	ON_COMMAND(ID_TAB_RENAME, &CMZ3GroupFileEditorDlg::OnTabRename)
	ON_NOTIFY(NM_RCLICK, IDC_CATEGORY_LIST, &CMZ3GroupFileEditorDlg::OnNMRclickCategoryList)
	ON_COMMAND(ID_CATEGORY_SETTING, &CMZ3GroupFileEditorDlg::OnCategorySetting)
	ON_COMMAND(ID_CATEGORY_ADD, &CMZ3GroupFileEditorDlg::OnCategoryAdd)
	ON_COMMAND(ID_CATEGORY_DELETE, &CMZ3GroupFileEditorDlg::OnCategoryDelete)
	ON_COMMAND(ID_CATEGORY_MOVE_UP, &CMZ3GroupFileEditorDlg::OnCategoryMoveUp)
	ON_COMMAND(ID_CATEGORY_MOVE_DOWN, &CMZ3GroupFileEditorDlg::OnCategoryMoveDown)
	ON_COMMAND(ID_CATEGORY_MOVE_LEFT, &CMZ3GroupFileEditorDlg::OnCategoryMoveLeft)
	ON_COMMAND(ID_CATEGORY_MOVE_RIGHT, &CMZ3GroupFileEditorDlg::OnCategoryMoveRight)
	ON_NOTIFY(NM_DBLCLK, IDC_CATEGORY_LIST, &CMZ3GroupFileEditorDlg::OnNMDblclkCategoryList)
	ON_NOTIFY(NM_CLICK, IDC_TAB1, &CMZ3GroupFileEditorDlg::OnNMClickTab1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMZ3GroupFileEditorDlg メッセージ ハンドラ

BOOL CMZ3GroupFileEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	// アクセラレータテーブルをロード
    m_hAccelTable = ::LoadAccelerators(
        AfxGetInstanceHandle(),
        MAKEINTRESOURCE(IDR_ACCELERATOR1)
    );


	mc_listCategory.InsertColumn(0, L"名前", 0, 200);

	ListView_SetExtendedListViewStyle(mc_listCategory.m_hWnd, LVS_EX_FULLROWSELECT);

	MyUpdateControlsState();

	// ファイルがあればロード
	LPCTSTR inifilename = L"toppage_group.ini";
	if (util::ExistFile(inifilename)) {
		theApp.m_strGroupInifilePath = inifilename;
		Mz3GroupDataReader::load( theApp.m_accessTypeInfo, theApp.m_group_info, theApp.m_strGroupInifilePath);

		MyReloadTabs();
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

LRESULT CMZ3GroupFileEditorDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE); // 更新ハンドラを呼び出してUIの状態を更新する
	return FALSE;
}


void CMZ3GroupFileEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMZ3GroupFileEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMZ3GroupFileEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMZ3GroupFileEditorDlg::OnExitApp()
{
	SendMessage(WM_CLOSE);
}

void CMZ3GroupFileEditorDlg::OnAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMZ3GroupFileEditorDlg::OnOpenFile()
{
	CFileDialog dlg( TRUE, L"ini", L"toppage_group.ini", OFN_FILEMUSTEXIST,
		L"MZ3 group file (*.ini)|*.ini|全て (*.*)|*.*||");
	
	if (dlg.DoModal() == IDOK) {
		theApp.m_strGroupInifilePath = dlg.GetPathName();
//		MessageBox(filepath);

		Mz3GroupDataReader::load( theApp.m_accessTypeInfo, theApp.m_group_info, theApp.m_strGroupInifilePath);

		MyReloadTabs();
	}
}

/**
 * タブ選択状態変更イベント
 */
void CMZ3GroupFileEditorDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	MyReloadCategory();

	*pResult = 0;
}

/**
 * タブの右クリックメニュー
 */
void CMZ3GroupFileEditorDlg::OnNMRclickTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDR_TAB_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, this);

	*pResult = 0;
}

/**
 * カテゴリの右クリックメニュー
 */
void CMZ3GroupFileEditorDlg::OnNMRclickCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, this);

	*pResult = 0;
}

bool CMZ3GroupFileEditorDlg::MyReloadTabs(void)
{
	mc_tab.DeleteAllItems();
	mc_listCategory.DeleteAllItems();

	std::vector<CGroupItem>& groups = theApp.m_group_info.groups;
	for (int i=0; i<(int)groups.size(); i++) {
		int idx = mc_tab.InsertItem(i, groups[i].name);
	}

	if (mc_tab.GetItemCount() > 0 ) {
		MyReloadCategory();
	}

	return true;
}

bool CMZ3GroupFileEditorDlg::MyReloadCategory(void)
{
	int idx = mc_tab.GetCurSel();
	if (idx<0) {
		MyUpdateControlsState();
		return false;
	}

	mc_listCategory.DeleteAllItems();
	CGroupItem& item = theApp.m_group_info.groups[idx];
	for (int i=0; i<item.categories.size(); i++) {
		mc_listCategory.InsertItem(i, item.categories[i].m_name);
	}
	if (mc_listCategory.GetItemCount() > 0) {
		util::MySetListCtrlItemFocusedAndSelected( mc_listCategory, 0, true );
	}

	MyUpdateControlsState();

	return true;
}

/**
 * タブ・カテゴリの状態に応じてボタン等の状態を変更。
 */
bool CMZ3GroupFileEditorDlg::MyUpdateControlsState(void)
{
	CWnd* wnd = GetFocus();
	if (wnd==NULL) {
		return false;
	}
/*
	// 上下ボタン
	if (wnd->m_hWnd != mc_listCategory.m_hWnd) {
		GetDlgItem(IDC_MOVE_UP_BUTTON  )->EnableWindow( FALSE );
		GetDlgItem(IDC_MOVE_DOWN_BUTTON)->EnableWindow( FALSE );
	} else {
		GetDlgItem(IDC_MOVE_UP_BUTTON  )->EnableWindow( (mc_listCategory.GetItemState(0, LVIS_FOCUSED) != FALSE) ? FALSE : TRUE );
		GetDlgItem(IDC_MOVE_DOWN_BUTTON)->EnableWindow( (mc_listCategory.GetItemState(mc_listCategory.GetItemCount()-1, LVIS_FOCUSED) != FALSE) ? FALSE : TRUE );
	}

	// 左右ボタン
	if (wnd->m_hWnd != mc_listCategory.m_hWnd &&
		wnd->m_hWnd != mc_tab.m_hWnd) 
	{
		GetDlgItem(IDC_MOVE_LEFT_BUTTON )->EnableWindow( FALSE );
		GetDlgItem(IDC_MOVE_RIGHT_BUTTON)->EnableWindow( FALSE );
	} else {
		GetDlgItem(IDC_MOVE_LEFT_BUTTON )->EnableWindow( (mc_tab.GetCurSel()==0) ? FALSE : TRUE );
		GetDlgItem(IDC_MOVE_RIGHT_BUTTON)->EnableWindow( (mc_tab.GetCurSel()==mc_tab.GetItemCount()-1) ? FALSE : TRUE );
	}
*/
	return true;
}

void CMZ3GroupFileEditorDlg::OnLvnItemchangedCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState != 3) {
		return;
	}

	MyUpdateControlsState();

	*pResult = 0;
}

void CMZ3GroupFileEditorDlg::OnTcnFocusChangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// この機能は Internet Explorer 5 かそれ以降のバージョンを必要とします。
	// シンボル _WIN32_IE は >= 0x0500 にならなければなりません。
	MyUpdateControlsState();

	*pResult = 0;
}

void CMZ3GroupFileEditorDlg::OnNMSetfocusCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MyUpdateControlsState();

	*pResult = 0;
}

void CMZ3GroupFileEditorDlg::OnNMKillfocusCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MyUpdateControlsState();

	*pResult = 0;
}

void CMZ3GroupFileEditorDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

/**
 * 現在選択中のカテゴリのインデックス取得
 */
int CMZ3GroupFileEditorDlg::MyGetSelectedCategory(void)
{
	for (int i=0; i<mc_listCategory.GetItemCount(); i++) {
		if (mc_listCategory.GetItemState(i, LVIS_FOCUSED) != FALSE) {
			return i;
		}
	}

	return -1;
}

/**
 * 上に移動
 */
void CMZ3GroupFileEditorDlg::OnAcceleratorMoveUp()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL ||
		wndFocused->m_hWnd != mc_listCategory.m_hWnd )
	{
		return;
	}

	int idx = MyGetSelectedCategory();
	if (idx<=0) {
		return;
	}

	// idx => idx-1
	MyMoveCategoryItem( idx, idx-1 );
}

/**
 * 下に移動
 */
void CMZ3GroupFileEditorDlg::OnAcceleratorMoveDown()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL ||
		wndFocused->m_hWnd != mc_listCategory.m_hWnd )
	{
		return;
	}

	int idx = MyGetSelectedCategory();
	if (idx<0 || idx>=mc_listCategory.GetItemCount()-1) {
		return;
	}

	// idx => idx+1
	MyMoveCategoryItem( idx, idx+1 );
}

/**
 * 左に移動
 */
void CMZ3GroupFileEditorDlg::OnAcceleratorMoveLeft()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL) {
		return;
	}

	if (wndFocused->m_hWnd == mc_listCategory.m_hWnd) {
		// カテゴリの移動
		int idxTab = mc_tab.GetCurSel();
		if (idxTab<=0) {
			return;
		}

		MyMoveCategoryItemToOtherTab( idxTab, idxTab-1 );

		return;
	}

	if (wndFocused->m_hWnd == mc_tab.m_hWnd) {
		// タブの移動
		return;
	}
}

/**
 * 右に移動
 */
void CMZ3GroupFileEditorDlg::OnAcceleratorMoveRight()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL) {
		return;
	}

	if (wndFocused->m_hWnd == mc_listCategory.m_hWnd) {
		// カテゴリの移動
		int idxTab = mc_tab.GetCurSel();
		if (idxTab>=mc_tab.GetItemCount()-1) {
			return;
		}

		MyMoveCategoryItemToOtherTab( idxTab, idxTab+1 );

		return;
	}

	if (wndFocused->m_hWnd == mc_tab.m_hWnd) {
		// タブの移動
		return;
	}
}

BOOL CMZ3GroupFileEditorDlg::PreTranslateMessage(MSG* pMsg)
{
    // アクセラレータ処理
    if(m_hAccelTable != NULL){
        if(::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
            return TRUE;
    }

	return CDialog::PreTranslateMessage(pMsg);
}

/**
 * 現在のカテゴリのアイテム idx を idxNew に移動する
 */
bool CMZ3GroupFileEditorDlg::MyMoveCategoryItem(int idx, int idxNew)
{
	// 現在のカテゴリ項目を取得する。
	int idxTab = mc_tab.GetCurSel();
	if (idxTab < 0) {
		return false;
	}

	CGroupItem& group = theApp.m_group_info.groups[idxTab];
	CCategoryItemList& categories = group.categories;
	CCategoryItem category = categories[idx];
	categories.erase( categories.begin()+idx );
	categories.insert( categories.begin()+idxNew, category );

	// リロード
	MyReloadCategory();

	// 選択状態変更
	util::MySetListCtrlItemFocusedAndSelected( mc_listCategory, idxNew, true );

	return true;
}

void CMZ3GroupFileEditorDlg::OnAcceleratorTabLeft()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL) {
		return;
	}

	if (wndFocused->m_hWnd == mc_listCategory.m_hWnd) {
		// カテゴリの移動
		int idx = mc_tab.GetCurSel();
		if (idx > 0) {
			mc_tab.SetCurSel(idx-1);
		} else {
			mc_tab.SetCurSel(mc_tab.GetItemCount()-1);
		}
		MyReloadCategory();

	}
}

void CMZ3GroupFileEditorDlg::OnAcceleratorTabRight()
{
	CWnd* wndFocused = GetFocus();
	if (wndFocused==NULL) {
		return;
	}

	if (wndFocused->m_hWnd == mc_listCategory.m_hWnd) {
		// カテゴリの移動
		int idx = mc_tab.GetCurSel();
		if (idx < mc_tab.GetItemCount()-1) {
			mc_tab.SetCurSel(idx+1);
		} else {
			mc_tab.SetCurSel(0);
		}
		MyReloadCategory();
	}
}

/**
 * タブ移動
 */
bool CMZ3GroupFileEditorDlg::MyMoveCategoryItemToOtherTab(int idxTab, int idxTabNew)
{
	int idxCategory = MyGetSelectedCategory();
	if (idxCategory<0) {
		return false;
	}

	CGroupItem& groupFrom = theApp.m_group_info.groups[idxTab];
	CCategoryItemList& categories = groupFrom.categories;
	CCategoryItem category = categories[idxCategory];
	categories.erase( categories.begin()+idxCategory );

	CGroupItem& groupTo = theApp.m_group_info.groups[idxTabNew];
	CCategoryItemList& categoriesTo = groupTo.categories;
	categoriesTo.insert( categoriesTo.begin(), category );

	// リロード
	mc_tab.SetCurSel( idxTabNew );
	MyReloadCategory();

	return true;
}

/**
 * 保存
 */
void CMZ3GroupFileEditorDlg::OnAcceleratorSave()
{
	Mz3GroupDataWriter::save( theApp.m_accessTypeInfo, theApp.m_group_info, theApp.m_strGroupInifilePath );
}

void CMZ3GroupFileEditorDlg::OnFileSave()
{
	OnAcceleratorSave();
}

void CMZ3GroupFileEditorDlg::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	if (theApp.m_strGroupInifilePath.IsEmpty() ||
		util::ExistFile(theApp.m_strGroupInifilePath) == false ) 
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMZ3GroupFileEditorDlg::OnOK()
{
//	CDialog::OnOK();
}

/**
 * タブの追加
 */
void CMZ3GroupFileEditorDlg::OnAddTab()
{
	int idx = mc_tab.GetCurSel();
	if (idx<0) {
		return;
	}

	CNameDlg dlg;
	dlg.mc_strName = L"新規タブ";
	if (dlg.DoModal() == IDOK) {
		CGroupItem group;
		group.init( dlg.mc_strName, L"", ACCESS_GROUP_OTHERS );

		std::vector<CGroupItem>& groups = theApp.m_group_info.groups;
		groups.insert( groups.begin()+idx, group );

		// リロード
		MyReloadTabs();
	}
}

/**
 * タブの削除
 */
void CMZ3GroupFileEditorDlg::OnDeleteTab()
{
	int idx = mc_tab.GetCurSel();
	std::vector<CGroupItem>& groups = theApp.m_group_info.groups;

	CString msg;
	msg.Format( L"【%s】タブを削除してもよろしいですか？", groups[idx].name );
	if (MessageBox( msg, 0, MB_YESNO ) == IDYES) {
		// 削除実行
		groups.erase( groups.begin()+idx );

		// リロード
		MyReloadTabs();
	}
}

/**
 * タブの名称変更
 */
void CMZ3GroupFileEditorDlg::OnTabRename()
{
	int idx = mc_tab.GetCurSel();
	if (idx<0) {
		return;
	}

	CGroupItem& group = theApp.m_group_info.groups[idx];

	CNameDlg dlg;
	dlg.mc_strName = group.name;
	if (dlg.DoModal() == IDOK) {

		group.name = dlg.mc_strName;

		// リロード
		MyReloadTabs();
		mc_tab.SetCurSel( idx );
		MyReloadCategory();
	}
}

/**
 * タブを左に移動
 */
void CMZ3GroupFileEditorDlg::OnTabMoveLeft()
{
	int idx = mc_tab.GetCurSel();
	if (idx<=0) {
		return;
	}
	int idxNew = idx-1;

	std::vector<CGroupItem>& groups = theApp.m_group_info.groups;
	CGroupItem group = groups[idx];
	groups.erase( groups.begin()+idx );
	groups.insert( groups.begin()+idxNew, group );

	// リロード
	MyReloadTabs();
	mc_tab.SetCurSel( idxNew );
	MyReloadCategory();
}

/**
 * タブを右に移動
 */
void CMZ3GroupFileEditorDlg::OnTabMoveRight()
{
	int idx = mc_tab.GetCurSel();
	if (idx<0 || idx>=mc_tab.GetItemCount()-1) {
		return;
	}
	int idxNew = idx+1;

	std::vector<CGroupItem>& groups = theApp.m_group_info.groups;
	CGroupItem group = groups[idx];
	groups.erase( groups.begin()+idx );
	groups.insert( groups.begin()+idxNew, group );

	// リロード
	MyReloadTabs();
	mc_tab.SetCurSel( idxNew );
	MyReloadCategory();
}

/**
 * カテゴリの設定
 */
void CMZ3GroupFileEditorDlg::OnCategorySetting()
{
	int idxCategory = MyGetSelectedCategory();
	if (idxCategory<0) {
		return;
	}

	int idx = mc_tab.GetCurSel();
	if (idx<0) {
		return;
	}
	CGroupItem& group = theApp.m_group_info.groups[idx];

	CCategorySettingDlg dlg;
	dlg.m_item = group.categories[idxCategory];
	if (dlg.DoModal() == IDOK) {
		// 更新
		group.categories[idxCategory] = dlg.m_item;

		// リロード
		MyReloadCategory();
		util::MySetListCtrlItemFocusedAndSelected( mc_listCategory, idxCategory, true );
	}

}

/**
 * カテゴリの追加
 */
void CMZ3GroupFileEditorDlg::OnCategoryAdd()
{
	int idxCategory = MyGetSelectedCategory();
	if (idxCategory<0) {
		return;
	}

	int idx = mc_tab.GetCurSel();
	if (idx<0) {
		return;
	}
	CGroupItem& group = theApp.m_group_info.groups[idx];

	CCategorySettingDlg dlg;
	dlg.m_item.m_name = L"新しいカテゴリ";
	if (dlg.DoModal() == IDOK) {
		// 追加
		group.categories.insert( group.categories.begin() + idxCategory, dlg.m_item );

		// リロード
		MyReloadCategory();
		util::MySetListCtrlItemFocusedAndSelected( mc_listCategory, idxCategory, true );
	}
}

/**
 * カテゴリの削除
 */
void CMZ3GroupFileEditorDlg::OnCategoryDelete()
{
	int idxCategory = MyGetSelectedCategory();
	CString name = mc_listCategory.GetItemText( idxCategory, 0 );

	CString msg;
	msg.Format( L"【%s】を削除してもよろしいですか？", name );
	if (MessageBox( msg, 0, MB_YESNO ) == IDYES) {
		// 削除実行
		int idx = mc_tab.GetCurSel();
		CGroupItem& group = theApp.m_group_info.groups[idx];
		group.categories.erase( group.categories.begin()+idxCategory );

		// リロード
		MyReloadCategory();
	}
}

void CMZ3GroupFileEditorDlg::OnCategoryMoveUp()
{
	OnAcceleratorMoveUp();
}

void CMZ3GroupFileEditorDlg::OnCategoryMoveDown()
{
	OnAcceleratorMoveDown();
}

void CMZ3GroupFileEditorDlg::OnCategoryMoveLeft()
{
	OnAcceleratorMoveLeft();
}

void CMZ3GroupFileEditorDlg::OnCategoryMoveRight()
{
	OnAcceleratorMoveRight();
}

/**
 * カテゴリリストのダブルクリック
 */
void CMZ3GroupFileEditorDlg::OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnCategorySetting();

	*pResult = 0;
}

/**
 * タブクリック
 */
void CMZ3GroupFileEditorDlg::OnNMClickTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 仮想的にダブルクリックを判定し、変更画面を表示する。
	static int s_idxLast = -1;
	static DWORD s_dwLastClickedTickCount = GetTickCount();

	int idx = mc_tab.GetCurSel();
	if (s_idxLast != idx) {
		s_idxLast = idx;
		s_dwLastClickedTickCount = GetTickCount();
	} else {

		// しきい値をシステムから取得し、ダブルクリック判定
		if (GetTickCount() - s_dwLastClickedTickCount < GetDoubleClickTime()) {
			s_idxLast = -1;
			// 変更画面表示
			OnTabRename();
		} else {
			s_dwLastClickedTickCount = GetTickCount();
		}
	}

	*pResult = 0;
}

void CMZ3GroupFileEditorDlg::OnClose()
{
	switch (MessageBox( L"変更内容を保存しますか？", NULL, MB_YESNOCANCEL)) {
	case IDYES:
		OnAcceleratorSave();
		break;

	case IDNO:
		break;

	case IDCANCEL:
		return;
	}

	CDialog::OnClose();
}
