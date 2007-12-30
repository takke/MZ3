/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// CommonEditDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "CommonEditDlg.h"


// CCommonEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CCommonEditDlg, CDialog)

CCommonEditDlg::CCommonEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonEditDlg::IDD, pParent)
	, mc_strEdit(_T(""))
	, mc_strMessage(_T(""))
{

}

CCommonEditDlg::~CCommonEditDlg()
{
}

void CCommonEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mc_strEdit);
	DDX_Text(pDX, IDC_MESSAGE_STATIC, mc_strMessage);
}


BEGIN_MESSAGE_MAP(CCommonEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCommonEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCommonEditDlg メッセージ ハンドラ

void CCommonEditDlg::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}

void CCommonEditDlg::SetTitle(LPCTSTR szTitle)
{
	m_strTitle = szTitle;
}

void CCommonEditDlg::SetMessage(LPCTSTR szMessage)
{
	mc_strMessage = szMessage;
}

BOOL CCommonEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( m_strTitle );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
