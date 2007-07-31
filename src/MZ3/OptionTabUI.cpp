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
	CheckDlgButton( IDC_USE_BGIMAGE_CHECK, theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// �t�H���g�i��j
	SetDlgItemText( IDC_FONT_BIG_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightBig) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_MEDIUM_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightMedium) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_SMALL_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightSmall) );

	// ���\�t�g�L�[�̗L���E����
	CheckDlgButton( IDC_USE_LEFTSOFTKEY_CHECK, theApp.m_optionMng.m_bUseLeftSoftKey ? BST_CHECKED : BST_UNCHECKED );

	// ���������莞��
	SetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, util::int2str(theApp.m_optionMng.m_longReturnRangeMSec) );

	// Xcrawl �듮��h�~�@�\
	CheckDlgButton( IDC_USE_XCRAWL_CANCELER_CHECK, theApp.m_optionMng.m_bUseXcrawlCanceler ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabUI::OnOK()
{
	// �w�iOn/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// �t�H���g�i��j
	{
		CString s;
		GetDlgItemText( IDC_FONT_BIG_EDIT, s );
		theApp.m_optionMng.m_fontHeightBig = option::Option::normalizeFontSize( _wtoi(s) );;
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_MEDIUM_EDIT, s );
		theApp.m_optionMng.m_fontHeightMedium = option::Option::normalizeFontSize( _wtoi(s) );
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_SMALL_EDIT, s );
		theApp.m_optionMng.m_fontHeightSmall = option::Option::normalizeFontSize( _wtoi(s) );
	}

	// ���\�t�g�L�[�̗L���E����
	theApp.m_optionMng.m_bUseLeftSoftKey = IsDlgButtonChecked( IDC_USE_LEFTSOFTKEY_CHECK ) == BST_CHECKED ? true : false;

	// ���������莞��
	{
		CString s;
		GetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, s );
		theApp.m_optionMng.m_longReturnRangeMSec = option::Option::normalizeLongReturnRangeMSec( _wtoi(s) );
	}

	// Xcrawl �듮��h�~�@�\
	theApp.m_optionMng.m_bUseXcrawlCanceler = IsDlgButtonChecked( IDC_USE_XCRAWL_CANCELER_CHECK ) == BST_CHECKED ? true : false;

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
