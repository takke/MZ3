/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * �o�[�W���������
 */

#ifndef __AFXWIN_H__
#error "PCH �̂��̃t�@�C�����C���N���[�h����O�ɁA'stdafx.h' ���C���N���[�h���܂�"
#endif

#include "resourceppc.h"


/**
 * MZ3 �̃o�[�W�������
 */
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStatic1();
	afx_msg void OnBnClickedResetTotalRecvBytesButton();
	void SetTotalBytes(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void MyReplaceText(void);
};

