/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabMInfo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabGeneral.h"
#include "ReportView.h"
#include "util.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabGeneral ダイアログ

IMPLEMENT_DYNAMIC(COptionTabGeneral, CPropertyPage)

COptionTabGeneral::COptionTabGeneral()
	: CPropertyPage(COptionTabGeneral::IDD)
{

}

COptionTabGeneral::~COptionTabGeneral()
{
}

void COptionTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionTabGeneral, CPropertyPage)
END_MESSAGE_MAP()

// COptionTabGeneral メッセージ ハンドラ
BOOL COptionTabGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 起動時の新着チェック
#ifdef BT_MZ3
	CheckDlgButton( IDC_BOOT_MNC_CHECK, theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );
#else
	GetDlgItem(IDC_BOOT_MNC_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BOOT_MNC_STATIC)->ShowWindow(SW_HIDE);
#endif

	// バージョンチェックに開発版も含める
	CheckDlgButton( IDC_USE_DEV_VER_CHECK_CHECK, theApp.m_optionMng.m_bUseDevVerCheck ? BST_CHECKED : BST_UNCHECKED );

	// 引用符号
#ifdef BT_MZ3
	SetDlgItemText( IDC_QUOTE_MARK_EDIT, theApp.m_optionMng.GetQuoteMark() );
#else
	GetDlgItem(IDC_QUOTE_MARK_GROUP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_QUOTE_MARK_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_QUOTE_MARK_EDIT)->ShowWindow(SW_HIDE);
#endif

	// URLを開くときに確認する
	CheckDlgButton( IDC_CONFIRM_OPEN_URL_CHECK, 
		theApp.m_optionMng.m_bConfirmOpenURL ? BST_CHECKED : BST_UNCHECKED );

	// mixi モバイル対応
/*	CheckDlgButton( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK, 
		theApp.m_optionMng.m_bConvertUrlForMixiMobile ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 では無効
	GetDlgItem( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK )->EnableWindow( FALSE );
#endif
*/
	return TRUE;
}

void COptionTabGeneral::OnOK()
{
	// 起動時の新着チェック
	theApp.m_optionMng.SetBootCheckMnC(
		IsDlgButtonChecked( IDC_BOOT_MNC_CHECK ) == BST_CHECKED ? true : false );

	// バージョンチェックに開発版も含める
	theApp.m_optionMng.m_bUseDevVerCheck = IsDlgButtonChecked( IDC_USE_DEV_VER_CHECK_CHECK ) == BST_CHECKED ? true : false;

	// 引用符号
	CString mark;
	GetDlgItemText( IDC_QUOTE_MARK_EDIT, mark );
	theApp.m_optionMng.SetQuoteMark( mark );

	// URLを開くときに確認する
	theApp.m_optionMng.m_bConfirmOpenURL = 
		IsDlgButtonChecked( IDC_CONFIRM_OPEN_URL_CHECK ) == BST_CHECKED ? true : false;

	// mixi モバイル対応
/*	theApp.m_optionMng.m_bConvertUrlForMixiMobile = 
		IsDlgButtonChecked( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK ) == BST_CHECKED ? true : false;
*/
	CPropertyPage::OnOK();
}

#endif