/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ChooseClientTypeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ChooseClientTypeDlg.h"


// CChooseClientTypeDlg ダイアログ

IMPLEMENT_DYNAMIC(CChooseClientTypeDlg, CDialog)

CChooseClientTypeDlg::CChooseClientTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseClientTypeDlg::IDD, pParent)
{

}

CChooseClientTypeDlg::~CChooseClientTypeDlg()
{
}

void CChooseClientTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChooseClientTypeDlg, CDialog)
END_MESSAGE_MAP()


// CChooseClientTypeDlg メッセージ ハンドラ

BOOL CChooseClientTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初期選択
	CheckDlgButton( IDC_SERVICE_TYPE_MIXI_CHECK, BST_CHECKED );
	CheckDlgButton( IDC_SERVICE_TYPE_TWITTER_CHECK, BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CChooseClientTypeDlg::OnOK()
{
	// 選択状態の取得
	m_initType.bUseMixi    = (IsDlgButtonChecked( IDC_SERVICE_TYPE_MIXI_CHECK ) == BST_CHECKED);
	m_initType.bUseTwitter = (IsDlgButtonChecked( IDC_SERVICE_TYPE_TWITTER_CHECK ) == BST_CHECKED);

	// 両方チェックされていなければエラー
	if (m_initType.bUseMixi==false &&
		m_initType.bUseTwitter==false)
	{
		MessageBox(L"選択してください");
		return;
	}

	CDialog::OnOK();
}
