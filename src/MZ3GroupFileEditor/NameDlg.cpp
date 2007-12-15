/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// NameDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3GroupFileEditor.h"
#include "NameDlg.h"


// CNameDlg ダイアログ

IMPLEMENT_DYNAMIC(CNameDlg, CDialog)

CNameDlg::CNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNameDlg::IDD, pParent)
	, mc_strName(_T(""))
{

}

CNameDlg::~CNameDlg()
{
}

void CNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, mc_strName);
}


BEGIN_MESSAGE_MAP(CNameDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNameDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNameDlg メッセージ ハンドラ

void CNameDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}
