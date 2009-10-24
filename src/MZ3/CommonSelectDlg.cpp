/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// CommonSelectDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "CommonSelectDlg.h"


// CCommonSelectDlg ダイアログ

IMPLEMENT_DYNAMIC(CCommonSelectDlg, CDialog)

CCommonSelectDlg::CCommonSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonSelectDlg::IDD, pParent)
	, m_pressedButtonCode(BUTTONCODE_CANCEL)
{

}

CCommonSelectDlg::~CCommonSelectDlg()
{
}

void CCommonSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCommonSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECT1_BUTTON, &CCommonSelectDlg::OnBnClickedSelect1Button)
	ON_BN_CLICKED(IDC_SELECT2_BUTTON, &CCommonSelectDlg::OnBnClickedSelect2Button)
	ON_BN_CLICKED(IDCANCEL, &CCommonSelectDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CCommonSelectDlg メッセージ ハンドラ

void CCommonSelectDlg::OnBnClickedSelect1Button()
{
	m_pressedButtonCode = BUTTONCODE_SELECT1;
	OnOK();
}

void CCommonSelectDlg::OnBnClickedSelect2Button()
{
	m_pressedButtonCode = BUTTONCODE_SELECT2;
	OnOK();
}

void CCommonSelectDlg::OnBnClickedCancel()
{
	m_pressedButtonCode = BUTTONCODE_CANCEL;
	OnOK();
}

bool CCommonSelectDlg::SetButtonText(CCommonSelectDlg::BUTTONCODE buttoncode, LPCTSTR szText)
{
	int idc = 0;
	switch(buttoncode) {
		case BUTTONCODE_SELECT1:	m_strButton1 = szText;		break;
		case BUTTONCODE_SELECT2:	m_strButton2 = szText;		break;
		case BUTTONCODE_CANCEL:		m_strButtonCancel = szText;	break;
		default:
			return false;
	}

	return true;
}

bool CCommonSelectDlg::SetMessage(LPCTSTR szMessage)
{
	m_strMessage = szMessage;

	m_strMessage.Replace( L"\n", L"\r\n" );

	return true;
}

BOOL CCommonSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText( IDC_SELECT1_BUTTON, m_strButton1 );
	SetDlgItemText( IDC_SELECT2_BUTTON, m_strButton2 );
	SetDlgItemText( IDCANCEL,			m_strButtonCancel );
	SetDlgItemText( IDC_MESSAGE_EDIT,   m_strMessage );
	SetWindowText( m_strTitle );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
