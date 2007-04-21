// OptionTabLog.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabLog.h"


// COptionTabLog �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabLog, CPropertyPage)

COptionTabLog::COptionTabLog()
	: CPropertyPage(COptionTabLog::IDD)
{

}

COptionTabLog::~COptionTabLog()
{
}

void COptionTabLog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabLog, CPropertyPage)
	ON_BN_CLICKED(IDC_CHANGE_LOG_FOLDER_BUTTON, &COptionTabLog::OnBnClickedChangeLogFolderButton)
END_MESSAGE_MAP()


// COptionTabLog ���b�Z�[�W �n���h��

void COptionTabLog::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

BOOL COptionTabLog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �I�v�V�������_�C�A���O�ɔ��f����
	Load();

	return TRUE;
}

/**
 * theApp.m_optionMng ����_�C�A���O�ɕϊ�����
 */
void COptionTabLog::Load()
{
	// ���O�̕ۑ�
	CheckDlgButton( IDC_SAVE_LOG_CHECK, theApp.m_optionMng.m_bSaveLog ? BST_CHECKED : BST_UNCHECKED );

	// ���O�̃p�X
	SetDlgItemText( IDC_LOGFOLDER_EDIT, theApp.m_optionMng.GetLogFolder() );
}

/**
 * �_�C�A���O�̃f�[�^�� theApp.m_optionMng �ɕϊ�����
 */
void COptionTabLog::Save()
{
	// ���O�̕ۑ�
	theApp.m_optionMng.m_bSaveLog = (IsDlgButtonChecked( IDC_SAVE_LOG_CHECK ) == BST_CHECKED);

	// ���O�̃p�X
	CString strFolderPath;
	GetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );

	// �I�v�V�����ɕۑ�
	theApp.m_optionMng.SetLogFolder( strFolderPath );

	// �e��p�X�̍Đ���
	theApp.m_filepath.init_logpath();
}

/**
 * ���O�t�H���_�̕ύX
 */
void COptionTabLog::OnBnClickedChangeLogFolderButton()
{
	CString strFolderPath;

	// �f�t�H���g�̃p�X����ʂ���擾����
	GetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );

	// �t�H���_�I���_�C�A���O�N��
	if( util::GetOpenFolderPath( m_hWnd, L"���O�t�H���_�̕ύX", strFolderPath ) ) {
		CString msg;
		msg.Format( 
			L"���O�t�@�C���̏o�͐��\n"
			L" %s\n"
			L"�ɕύX���܂��B��낵���ł����H",
			strFolderPath );

		if( MessageBox( msg, 0, MB_YESNO ) == IDYES ) {
			// ��ʂ̍Đݒ�
			SetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );
		}
	}

}
