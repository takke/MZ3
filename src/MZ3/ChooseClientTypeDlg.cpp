/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ChooseClientTypeDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ChooseClientTypeDlg.h"


// CChooseClientTypeDlg �_�C�A���O

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
	DDX_Control(pDX, IDC_SERVICE_LIST, mc_serviceList);
}


BEGIN_MESSAGE_MAP(CChooseClientTypeDlg, CDialog)
END_MESSAGE_MAP()


// CChooseClientTypeDlg ���b�Z�[�W �n���h��

BOOL CChooseClientTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �����I��
	mc_serviceList.InsertColumn(0, L"dummy", LVCFMT_LEFT, 160, 0);
	mc_serviceList.SetExtendedStyle(LVS_EX_CHECKBOXES);

	for (size_t idx=0; idx<theApp.m_luaServices.size(); idx++) {
		mc_serviceList.InsertItem(idx, CString(theApp.m_luaServices[idx].name.c_str()));
		mc_serviceList.SetCheck(idx, theApp.m_luaServices[idx].selected ? TRUE : FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CChooseClientTypeDlg::OnOK()
{
	// �����`�F�b�N����Ă��Ȃ���΃G���[
	int n = mc_serviceList.GetItemCount();
	bool bHasSelectedItem = false;
	for (int i=0; i<n; i++) {
		if (mc_serviceList.GetCheck(i)==TRUE) {
			bHasSelectedItem = true;
		}
	}
	if (!bHasSelectedItem) {
		MessageBox(L"�I�����Ă�������");
		return;
	}

	// �I������Ă���T�[�r�X�̎擾
	m_initType.strSelectedServices.clear();
	for (int i=0; i<n; i++) {
		if (mc_serviceList.GetCheck(i)==TRUE) {
			CStringA s(mc_serviceList.GetItemText(i, 0));
			m_initType.strSelectedServices += " ";
			m_initType.strSelectedServices += s;
		}
	}

	CDialog::OnOK();
}
