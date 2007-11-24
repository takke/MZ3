// MZ3GroupFileEditor.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��
#include "../MZ3/Mz3GroupData.h"

// NOP
#define MZ3LOGGER_ERROR(msg) ;

// CMZ3GroupFileEditorApp:
// ���̃N���X�̎����ɂ��ẮAMZ3GroupFileEditor.cpp ���Q�Ƃ��Ă��������B
//

class CMZ3GroupFileEditorApp : public CWinApp
{
public:
	CMZ3GroupFileEditorApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

public:
	Mz3GroupData m_group_info;
	CString		 m_strGroupInifilePath;

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMZ3GroupFileEditorApp theApp;