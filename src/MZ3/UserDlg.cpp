/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// UserDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "UserDlg.h"
#include "util_base.h"
#include "util_mz3.h"

// CUserDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CUserDlg, CDialog)

CUserDlg::CUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserDlg::IDD, pParent)
{
}

CUserDlg::~CUserDlg()
{
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, mc_comboType);
}


BEGIN_MESSAGE_MAP(CUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUserDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CUserDlg::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CUserDlg ���b�Z�[�W �n���h��

BOOL CUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (size_t i=0; i<theApp.m_luaAccounts.size(); i++) {
		CString strServiceName(theApp.m_luaAccounts[i].service_name.c_str());
		mc_comboType.InsertString( i, strServiceName );
	}

	mc_comboType.SetCurSel( 0 );
	m_idxSelectedCombo = 0;

	MyLoadControlData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CUserDlg::OnBnClickedOk()
{
	if (MySaveControlData()) {
		OnOK();
	}
}

void CUserDlg::OnCbnSelchangeTypeCombo()
{
	MySaveControlData();

	m_idxSelectedCombo = mc_comboType.GetCurSel();

	MyLoadControlData();
}

bool CUserDlg::MySaveControlData(void)
{
	// �I�𒆂̃T�[�r�X�p�f�[�^��ۑ�
	// m_idxSelectedCombo �ɂ��A�I��ύX�ʒm��ł��u�ύX�O�v�ɑI�����Ă����T�[�r�X�p�̃f�[�^��ۑ�����B
	CString strServiceName;
	mc_comboType.GetLBText( m_idxSelectedCombo, strServiceName );

	CString id;
	GetDlgItemText( IDC_LOGIN_MAIL_EDIT, id );
	theApp.m_loginMng.SetId( strServiceName, id );

	CString password;
	GetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, password );
	theApp.m_loginMng.SetPassword( strServiceName, password );

	// MZ3 API : �C�x���g�n���h���֐��Ăяo��
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("check_account", &rvals, 
			util::MyLuaData(CStringA(strServiceName)),
			util::MyLuaData(CStringA(id)),
			util::MyLuaData(CStringA(password))
			))
	{
		// �C�x���g�n���h������
		return false;
	}

	// �ۑ�
	theApp.m_loginMng.Write();

	return true;
}

void CUserDlg::MyLoadControlData(void)
{
	CString strServiceName;
	mc_comboType.GetLBText( m_idxSelectedCombo, strServiceName );
	CStringA strServiceNameA(strServiceName);

	for (size_t i=0; i<theApp.m_luaAccounts.size(); i++) {
		const CMZ3App::AccountData& data = theApp.m_luaAccounts[i];
		if (data.service_name.c_str()==strServiceNameA) {
			SetDlgItemText( IDC_ID_STATIC, CString(data.id_name.c_str()) );
			SetDlgItemText( IDC_LOGIN_MAIL_EDIT, theApp.m_loginMng.GetId(strServiceName) );
			SetDlgItemText( IDC_PASSWORD_STATIC, CString(data.password_name.c_str()) );
			SetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, theApp.m_loginMng.GetPassword(strServiceName) );
		}
	}

}
