/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once


// CNameDlg �_�C�A���O

class CNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CNameDlg)

public:
	CNameDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CNameDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_NAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strName;
	afx_msg void OnBnClickedOk();
};
