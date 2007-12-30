/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once


// CCommonEditDlg �_�C�A���O

class CCommonEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonEditDlg)

public:
	CCommonEditDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CCommonEditDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_COMMON_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CString mc_strEdit;
	CString mc_strMessage;
	afx_msg void OnBnClickedOk();
	void SetTitle(LPCTSTR szTitle);
	void SetMessage(LPCTSTR szMessage);
	virtual BOOL OnInitDialog();
};
