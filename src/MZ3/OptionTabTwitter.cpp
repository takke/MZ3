// OptionTabTwitter.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"
#include "util_base.h"


// COptionTabTwitter �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabTwitter, CPropertyPage)

COptionTabTwitter::COptionTabTwitter()
	: CPropertyPage(COptionTabTwitter::IDD)
{

}

COptionTabTwitter::~COptionTabTwitter()
{
}

void COptionTabTwitter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabTwitter, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabTwitter ���b�Z�[�W �n���h��

BOOL COptionTabTwitter::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// *MZ3* �}�[�N
	CheckDlgButton( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, theApp.m_optionMng.m_bAddSourceTextOnTwitterPost ? BST_CHECKED : BST_UNCHECKED );
	SetDlgItemText( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, L"������ *" MZ3_APP_NAME L"* �}�[�N������" );

	// �X�e�[�^�X�s��
	CComboBox* pStatusLineCountCombo = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
	int selectedNumber = theApp.m_optionMng.m_nTwitterStatusLineCount;
	int items[] = { 2, 3, -1 };	// -1 : terminater
	for (int i=0; items[i]!=-1; i++) {
		int n = items[i];
		int idx = pStatusLineCountCombo->InsertString(i, util::FormatString(L"%d", n));
		pStatusLineCountCombo->SetItemData(idx, n);
		if (selectedNumber==n) {
			pStatusLineCountCombo->SetCurSel(i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabTwitter::OnOK()
{
	// *MZ3* �}�[�N
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	// �X�e�[�^�X�s��
	CComboBox* pStatusLineCountCombo = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
	int idx = pStatusLineCountCombo->GetCurSel();
	if (idx>=0) {
		int n = pStatusLineCountCombo->GetItemData(idx);
		theApp.m_optionMng.m_nTwitterStatusLineCount = option::Option::normalizeTwitterStatusLineCount(n);
	}

	CPropertyPage::OnOK();
}
