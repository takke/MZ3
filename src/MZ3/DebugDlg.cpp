/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// DebugDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "DebugDlg.h"
#include "util.h"
#include "util_gui.h"
#include "MixiParserUtil.h"

// CDebugDlg ダイアログ

IMPLEMENT_DYNAMIC(CDebugDlg, CDialog)

CDebugDlg::CDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugDlg::IDD, pParent)
{

}

CDebugDlg::~CDebugDlg()
{
}

void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEBUG_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CDebugDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_DEBUG_LIST, &CDebugDlg::OnNMClickDebugList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DEBUG_LIST, &CDebugDlg::OnLvnKeydownDebugList)
	ON_NOTIFY(NM_DBLCLK, IDC_DEBUG_LIST, &CDebugDlg::OnNMDblclkDebugList)
END_MESSAGE_MAP()


// CDebugDlg メッセージ ハンドラ

BOOL CDebugDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// フォント
	m_List.SetFont( &theApp.m_font );

	// カラム追加
	m_List.InsertColumn( 0, L"項目", LVCFMT_LEFT, 150, -1 );
	m_List.InsertColumn( 1, L"内容", LVCFMT_LEFT, 300, -1 );
	// 一行選択モードの設定
	ListView_SetExtendedListViewStyle((HWND)m_List.m_hWnd, LVS_EX_FULLROWSELECT);

	// 要素追加
	int idx = 0;

	CMixiData* data = m_data;

	m_List.InsertItem( idx, L"アクセス種別" );
	m_List.SetItemText( idx, 1, theApp.m_accessTypeInfo.getShortText( data->GetAccessType() ) );
	idx++;

	m_List.InsertItem( idx, L"シリアライズキー" );
	m_List.SetItemText( idx, 1, CString(theApp.m_accessTypeInfo.getSerializeKey( data->GetAccessType() )) );
	idx++;

	m_List.InsertItem( idx, L"Date" );
	m_List.SetItemText( idx, 1, data->GetDate() );
	idx++;

	m_List.InsertItem( idx, L"Name" );
	m_List.SetItemText( idx, 1, data->GetName() );
	idx++;

	m_List.InsertItem( idx, L"Author" );
	m_List.SetItemText( idx, 1, data->GetAuthor() );
	idx++;

	m_List.InsertItem( idx, L"AuthorID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetAuthorID() ) );
	idx++;

	m_List.InsertItem( idx, L"Title" );
	m_List.SetItemText( idx, 1, data->GetTitle() );
	idx++;

	m_List.InsertItem( idx, L"URL" );
	m_List.SetItemText( idx, 1, data->GetURL() );
	idx++;

	m_List.InsertItem( idx, L"POSTアドレス" );
	m_List.SetItemText( idx, 1, data->GetPostAddress() );
	idx++;

	m_List.InsertItem( idx, L"記事ID" );
	m_List.SetItemText( idx, 1, util::FormatString(L"%I64d", data->GetID()) );
	idx++;

	m_List.InsertItem( idx, L"コメント番号" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetCommentIndex() ) );
	idx++;

	m_List.InsertItem( idx, L"コメント数" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetCommentCount() ) );
	idx++;

	m_List.InsertItem( idx, L"既読コメント番号" );
	int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(*data);
	m_List.SetItemText( idx, 1, util::int2str( lastIndex ) );
	idx++;

	m_List.InsertItem( idx, L"POST種別" );
	m_List.SetItemText( idx, 1, data->GetContentType() );
	idx++;

	m_List.InsertItem( idx, L"オーナーID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetOwnerID() ) );
	idx++;

	m_List.InsertItem( idx, L"マイミク" );
	m_List.SetItemText( idx, 1, data->IsMyMixi() ? L"true" : L"false" );
	idx++;

//	m_List.InsertItem( idx, L"外部ブログ" );
//	m_List.SetItemText( idx, 1, data->IsOtherDiary() ? L"YES" : L"NO" );
//	idx++;

	m_List.InsertItem( idx, L"ブラウズURL" );
	m_List.SetItemText( idx, 1, data->GetBrowseUri() );
	idx++;

	m_List.InsertItem( idx, L"*BodyArray数" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetBodySize() ) );
	m_List.SetItemData( idx, (DWORD_PTR)L"body" );
	idx++;

	m_List.InsertItem( idx, L"*ImageArray数" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetImageCount() ) );
	m_List.SetItemData( idx, (DWORD_PTR)L"image" );
	idx++;

	m_List.InsertItem( idx, L"抽出リンク数" );
	m_List.SetItemText( idx, 1, util::int2str( data->m_linkList.size() ) );
//	m_List.SetItemData( idx, (DWORD_PTR)L"link_list" );
	idx++;

	m_List.InsertItem( idx, L"ページ変更リンク数" );
	m_List.SetItemText( idx, 1, util::int2str( data->m_linkPage.size() ) );
//	m_List.SetItemData( idx, (DWORD_PTR)L"link_page" );
	idx++;

	m_List.InsertItem( idx, L"子要素数" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetChildrenSize() ) );
	idx++;

	m_List.InsertItem( idx, L"logfile" );
	m_List.SetItemText( idx, 1, util::MakeLogfilePath(*data) );
	idx++;

	m_List.InsertItem( idx, L"logfile-exist" );
	m_List.SetItemText( idx, 1, util::ExistFile(util::MakeLogfilePath(*data)) ? L"true" : L"false" );
	idx++;

	m_List.InsertItem( idx, L"スキンフォルダ" );
	m_List.SetItemText( idx, 1, theApp.m_filepath.skinFolder );
	idx++;

	m_List.InsertItem( idx, L"スキン名" );
	m_List.SetItemText( idx, 1, theApp.m_optionMng.m_strSkinname );
	idx++;

	m_List.InsertItem( idx, L"MZ3Data インスタンス数" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetInstanceCount() ) );
	idx++;

	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

#ifndef WINCE
	// 画面サイズを変更
	int w = 360;
	int h = 480;
	SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOOWNERZORDER );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDebugDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


	if (m_List.GetSafeHwnd()==NULL) {
		return;
	}

	m_List.MoveWindow( 0, 0, cx, cy );
}

void CDebugDlg::OnNMClickDebugList(NMHDR *pNMHDR, LRESULT *pResult)
{

	*pResult = 0;
}

void CDebugDlg::OnLvnKeydownDebugList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_RETURN) {
		MyShowSelectedItemInfo();
	}

	*pResult = 0;
}

void CDebugDlg::OnNMDblclkDebugList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MyShowSelectedItemInfo();

	*pResult = 0;
}

void CDebugDlg::MyShowSelectedItemInfo(void)
{
	int idx = util::MyGetListCtrlSelectedItemIndex( m_List );
	if( idx < 0 ) {
		return;
	}

	CString msg;

	// ItemData に識別子があればそれに従って文字列生成
	const CString& strType = (LPCTSTR)m_List.GetItemData(idx);
	if (strType==L"body") {
		msg.Format(L"[%s]", m_data->GetBody());
	} else if (strType==L"image") {
		for (int i=0; i<m_data->GetImageCount(); i++) {
			msg.AppendFormat(L"[%s]\r\n", m_data->GetImage(i));
		}
	} else {
		msg.Format(L"[%s]", m_List.GetItemText(idx,1) );
	}

	MessageBox( msg, m_List.GetItemText(idx,0) );
}
