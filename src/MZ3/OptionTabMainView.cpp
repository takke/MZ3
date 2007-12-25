/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// OptionTabMainView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabMainView ダイアログ

IMPLEMENT_DYNAMIC(COptionTabMainView, CPropertyPage)

COptionTabMainView::COptionTabMainView()
	: CPropertyPage(COptionTabMainView::IDD)
{

}

COptionTabMainView::~COptionTabMainView()
{
}

void COptionTabMainView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MINI_IMAGE_SIZE_COMBO, mc_comboMiniImageSize);
}


BEGIN_MESSAGE_MAP(COptionTabMainView, CPropertyPage)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_DLG_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageDlgCheck)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageCheck)
END_MESSAGE_MAP()


// COptionTabMainView メッセージ ハンドラ

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ユーザやコミュニティの画像
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// 別画面に表示
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImageDlg ? BST_CHECKED : BST_UNCHECKED );

	// WINCE ではカーソル位置に表示しない
#ifdef WINCE
	// 無効
#else
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver ? BST_CHECKED : BST_UNCHECKED );
#endif

	// 画像の自動取得
	CheckDlgButton( IDC_AUTO_LOAD_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bAutoLoadMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// トピック等のアイコン表示
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	// mini画面サイズ
#ifdef WINCE
	// 無効
#else
	int sizes[] = { 25, 50, 75, 100, 125, 150, -1 };
	for (int i=0; sizes[i] != -1; i++) {
		CString s;
		s.Format( L"%dx%d", sizes[i], sizes[i] );
		int idx = mc_comboMiniImageSize.InsertString( i, s );
		mc_comboMiniImageSize.SetItemData( idx, sizes[i] );

		if (sizes[i] == theApp.m_optionMng.m_nMainViewMiniImageSize) {
			mc_comboMiniImageSize.SetCurSel(i);
		}
	}
	if (mc_comboMiniImageSize.GetCurSel() < 0) {
		mc_comboMiniImageSize.SetCurSel( 0 );
	}
	// 有効・無効
	OnBnClickedShowMiniImageDlgCheck();
#endif

	UpdateControlItemStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabMainView::OnOK()
{
	// ユーザやコミュニティの画像
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED;
	theApp.m_optionMng.m_bShowMainViewMiniImageDlg = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED;
	theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK ) == BST_CHECKED;

	// トピック等のアイコン表示
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// mini画面サイズ
#ifndef WINCE
	theApp.m_optionMng.m_nMainViewMiniImageSize = mc_comboMiniImageSize.GetItemData( mc_comboMiniImageSize.GetCurSel() );
#endif

	// 画像の自動取得
	theApp.m_optionMng.m_bAutoLoadMiniImage = IsDlgButtonChecked( IDC_AUTO_LOAD_MINI_IMAGE_CHECK ) == BST_CHECKED;

	CPropertyPage::OnOK();
}

void COptionTabMainView::OnBnClickedShowMiniImageDlgCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::OnBnClickedShowMiniImageCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::UpdateControlItemStatus(void)
{
#ifdef WINCE
	// WINCE なら無効
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
	GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
	GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );

	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	if (bImageCheck) {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}

#else
	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL bImageDlgCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED) ? TRUE : FALSE;

	if (bImageCheck) {
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( TRUE );
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
		if (bImageDlgCheck) {
			// 全て有効
			GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( TRUE );
			GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( TRUE );
			GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( TRUE );
		} else {
			// カーソル位置とコンボボックス関連は無効
			GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
			GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
			GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );
		}
	} else {
		// 全て無効
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
		GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
		GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}
#endif
}

#endif