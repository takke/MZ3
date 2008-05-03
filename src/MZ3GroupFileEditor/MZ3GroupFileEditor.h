/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3GroupFileEditor.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

// NOP
#define MZ3LOGGER_ERROR(msg) ;

#include "../MZ3/Mz3GroupData.h"
#include "../MZ3/AccessTypeInfo.h"

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
	AccessTypeInfo	m_accessTypeInfo;	///< �A�N�Z�X��ʖ���MZ3/4�̐U�镑�����`������

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMZ3GroupFileEditorApp theApp;