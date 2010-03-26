/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
	, mc_strEdit2(_T(""))
{

}

CCommonEditDlg::~CCommonEditDlg()
{
}

void CCommonEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mc_strEdit);
	DDX_Text(pDX, IDC_EDIT2, mc_strEdit2);
	DDX_Text(pDX, IDC_MESSAGE_STATIC, mc_strMessage);
	DDX_Control(pDX, IDC_COMBO1, mc_comboBox);
}


BEGIN_MESSAGE_MAP(CCommonEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCommonEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCommonEditDlg メッセージ ハンドラ

void CCommonEditDlg::OnBnClickedOk()
{
	UpdateData();

	// mc_comboBox が１個以上あれば mc_strEdit に選択文字列を設定する
	if (!m_comboTextList.empty()) {
		int idx = mc_comboBox.GetCurSel();
		if (0 <= idx && idx < mc_comboBox.GetCount()) {
			mc_comboBox.GetLBText(idx, m_strSelectedComboText);
		} else {
			m_strSelectedComboText = L"";
		}
	}

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

	// mc_strEdit2 が未設定であれば IDC_EDIT2 を非表示化
	if (mc_strEdit2.IsEmpty()) {
		GetDlgItem(IDC_EDIT2)->ShowWindow( SW_HIDE );
	}

	// mc_comboBox が１個以上あれば IDC_EDIT1, IDC_EDIT2 を非表示化
	if (!m_comboTextList.empty()) {
		GetDlgItem(IDC_EDIT1)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_EDIT2)->ShowWindow( SW_HIDE );
		GetDlgItem(IDC_COMBO1)->ShowWindow( SW_SHOW );

		mc_comboBox.ResetContent();
		for (u_int i=0; i<m_comboTextList.size(); i++) {
			mc_comboBox.InsertString(i, m_comboTextList[i]);
			if (i==0) {
				mc_comboBox.SetCurSel(0);
			}
		}
	}

	SetWindowText( m_strTitle );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
