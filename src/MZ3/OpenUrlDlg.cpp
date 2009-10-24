/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OpenUrlDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OpenUrlDlg.h"


// COpenUrlDlg �_�C�A���O

IMPLEMENT_DYNAMIC(COpenUrlDlg, CDialog)

COpenUrlDlg::COpenUrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenUrlDlg::IDD, pParent)
	, mc_strUrl(_T(""))
{

}

COpenUrlDlg::~COpenUrlDlg()
{
}

void COpenUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_URL_EDIT, mc_strUrl);
	DDX_Control(pDX, IDC_ENCODING_COMBO, mc_comboEncoding);
}


BEGIN_MESSAGE_MAP(COpenUrlDlg, CDialog)
END_MESSAGE_MAP()


// COpenUrlDlg ���b�Z�[�W �n���h��

BOOL COpenUrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int idx = 0;
	idx = mc_comboEncoding.InsertString( idx, L"UTF-8" );
	mc_comboEncoding.SetItemData( idx++, CInetAccess::ENCODING_UTF8 );
	idx = mc_comboEncoding.InsertString( idx, L"EUC-JP" );
	mc_comboEncoding.SetItemData( idx++, CInetAccess::ENCODING_EUC );
	idx = mc_comboEncoding.InsertString( idx, L"ShiftJIS" );
	mc_comboEncoding.SetItemData( idx++, CInetAccess::ENCODING_SJIS );

	mc_comboEncoding.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COpenUrlDlg::OnOK()
{
	m_encoding = mc_comboEncoding.GetItemData( mc_comboEncoding.GetCurSel() );

	CDialog::OnOK();
}
