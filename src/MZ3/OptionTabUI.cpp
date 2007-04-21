// OptionTabUI.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUI.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabUI �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabUI, CPropertyPage)

COptionTabUI::COptionTabUI()
	: CPropertyPage(COptionTabUI::IDD)
{

}

COptionTabUI::~COptionTabUI()
{
}

void COptionTabUI::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabUI, CPropertyPage)
	ON_BN_CLICKED(IDC_RESET_TAB_BUTTON, &COptionTabUI::OnBnClickedResetTabButton)
	ON_BN_CLICKED(IDC_SELECT_FONT_BUTTON, &COptionTabUI::OnBnClickedSelectFontButton)
END_MESSAGE_MAP()


// COptionTabUI ���b�Z�[�W �n���h��
BOOL COptionTabUI::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �w�iOn/Off
	((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->SetCheck(
		theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// �t�H���g�i��j
	SetDlgItemText( IDC_FONT_BIG_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightBig) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_MEDIUM_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightMedium) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_SMALL_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightSmall) );

	return TRUE;
}

void COptionTabUI::OnOK()
{
	// �N�����̐V���`�F�b�N
	theApp.m_optionMng.SetBootCheckMnC(
		((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// �w�iOn/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );


	// �t�H���g�i��j
	{
		CString s;
		GetDlgItemText( IDC_FONT_BIG_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightBig = n;
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_MEDIUM_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightMedium = n;
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_SMALL_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightSmall = n;
	}

	CPropertyPage::OnOK();
}

void COptionTabUI::OnBnClickedResetTabButton()
{
	if( MessageBox( L"�^�u�����Z�b�g���Ă���낵���ł����H\n�i���������MZ3���I�����܂��j", 
			0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) 
	{
		return;
	}

	// ������
	theApp.m_root.initForTopPage();

	// �ċN��
	// �E�E�E�ł��Ȃ��̂Ń��b�Z�[�W��\�����ďI���B
	MessageBox( L"������������������������MZ3���I�����܂�" );
	theApp.m_pReportView->SaveIndex();
	ShowWindow(SW_HIDE);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

/// �t�H���g�ύX
void COptionTabUI::OnBnClickedSelectFontButton()
{
	CChooseFontDlg dlg;
	if( dlg.DoModal() == IDOK ) {
		theApp.m_optionMng.m_fontFace = dlg.m_strSelectedFont;
	}
}
