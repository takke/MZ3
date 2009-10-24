/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// COpenUrlDlg �_�C�A���O

class COpenUrlDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenUrlDlg)

public:
	COpenUrlDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~COpenUrlDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPEN_URL_DIALOG };

	int m_encoding;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strUrl;
	CComboBox mc_comboEncoding;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
