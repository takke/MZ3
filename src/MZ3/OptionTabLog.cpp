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
	ON_BN_CLICKED(IDC_CLEAN_LOG_BUTTON, &COptionTabLog::OnBnClickedCleanLogButton)
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

/**
 * �t�@�C�����̃J�E���g�A�b�v�p�R�[���o�b�N�֐�
 */
int CountupCallback( const TCHAR* szDirectory,
                     const WIN32_FIND_DATA* data,
                     int* pnFiles)
{
	(*pnFiles) ++;

	return TRUE;
}

/**
 * �t�@�C���̍폜�p�R�[���o�b�N�֐�
 */
int DeleteCallback( const TCHAR* szDirectory,
                    const WIN32_FIND_DATA* data,
                    int* pnDeleted)
{
	std::basic_string< TCHAR > strFile = szDirectory + std::basic_string< TCHAR >(data->cFileName);

	if( (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY ) {
		// �f�B���N�g��
		if( RemoveDirectory( strFile.c_str() ) ) {
			(*pnDeleted) ++;
		}
	}else{
		// �t�@�C��
		if( DeleteFile( strFile.c_str() ) ) {
			(*pnDeleted) ++;
		}
	}

	return TRUE;
}

/**
 * ���O�̍폜
 */
void COptionTabLog::OnBnClickedCleanLogButton()
{
	int nFiles = 0;	// �t�@�C����
	int nDepthMax = 10;	// �ő�ċA�[�x
	
	LPCTSTR szDeleteFilePattern = L"*";
	CString strLogFolder = theApp.m_filepath.logFolder + L"\\";

	// ���O�t�@�C�����J�E���g
	util::FindFileCallback( strLogFolder, szDeleteFilePattern, CountupCallback, &nFiles, nDepthMax );

	if( nFiles == 0 ) {
		MessageBox( L"�폜�Ώۃt�@�C��������܂���" );
		return;
	}

	CString msg;
	msg.Format( 
		L"%d �̃��O�t�@�C�����폜���܂��B��낵���ł����H\n"
		L"�i�t�@�C�����������ꍇ�A�������x������܂��j", nFiles );
	if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
		return;
	}

	// �폜���s
	int nDeleted = 0;	// �폜�ς݃t�@�C����
	util::FindFileCallback( strLogFolder, szDeleteFilePattern, DeleteCallback, &nDeleted, nDepthMax );

	msg.Format( 
		L"%d �̃t�@�C�����폜���܂����B\n"
		L"�i�Ώۃt�@�C���F%d �j", nDeleted, nFiles );
	MessageBox( msg );
}
