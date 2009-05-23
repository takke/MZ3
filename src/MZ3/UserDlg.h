/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "c:\program files\microsoft visual studio 8\vc\atlmfc\include\afxwin.h"


// CUserDlg �_�C�A���O

class CUserDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserDlg)

public:
	CUserDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CUserDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_USER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CComboBox					mc_comboType;
	int							m_idxSelectedCombo;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeTypeCombo();
	bool MySaveControlData(void);
	void MyLoadControlData(void);
};
