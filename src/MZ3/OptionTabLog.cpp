/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabLog.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "OptionTabLog.h"
#include "MZ3FileCacheManager.h"

#ifndef SMARTPHONE2003_UI_MODEL

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
	ON_BN_CLICKED(IDC_DEBUG_MODE_CHECK, &COptionTabLog::OnBnClickedDebugModeCheck)
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

	// �f�o�b�O���[�h
	CheckDlgButton( IDC_DEBUG_MODE_CHECK, theApp.m_optionMng.IsDebugMode() ? BST_CHECKED : BST_UNCHECKED );
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

	// �f�o�b�O���[�h
	theApp.m_optionMng.SetDebugMode( IsDlgButtonChecked( IDC_DEBUG_MODE_CHECK ) == BST_CHECKED );
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

		// MZ3��p���O�f�B���N�g������ǉ�
		strFolderPath.Append( L"\\mz3log" );

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
 * ���O�̍폜
 */
void COptionTabLog::OnBnClickedCleanLogButton()
{
	MZ3FileCacheManager::CountupResult cd;		// �t�@�C����, �t�@�C���T�C�Y

	int nTargetFileLastWriteDaysBack = 0;		// �X�V���t�����F�S�Ẵt�@�C����ΏۂƂ���

	// ���O�t�@�C�����J�E���g
	MZ3FileCacheManager cacheManager;
	cacheManager.GetTargetFileCount( theApp.m_filepath.deleteTargetFolders, &cd, nTargetFileLastWriteDaysBack );

	if( cd.nFiles == 0 ) {
		MessageBox( L"�폜�Ώۃt�@�C��������܂���" );
		return;
	}

	CString msg;
	msg.Format( 
		L"���O�t�@�C�����폜���܂��B��낵���ł����H\n"
		L"�E�t�@�C�����������ꍇ�A�������x������܂�\n"
		L"�E[%s]�ȉ��̑S�Ẵt�@�C�����������܂�\n\n"
		L"�t�@�C�����F%d\n"
		L"���t�@�C���T�C�Y�F%s Bytes"
		, (LPCTSTR)theApp.m_filepath.logFolder, cd.nFiles, util::int2comma_str(cd.dwSize) );
	if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
		return;
	}

	// �폜���s
	int nDeleted = 0;	// �폜�ς݃t�@�C����
	cacheManager.DeleteFiles( theApp.m_filepath.deleteTargetFolders, &nDeleted, nTargetFileLastWriteDaysBack );

	msg.Format( 
		L"%d �̃t�@�C�����폜���܂����B\n"
		L"�i�Ώۃt�@�C���F%d �j", nDeleted, cd.nFiles );
	MessageBox( msg );
}

void COptionTabLog::OnBnClickedDebugModeCheck()
{
	MessageBox( L"�f�o�b�O���[�h�̕ύX�͍ċN����ɔ��f����܂�" );
}

#endif