/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// COpenUrlDlg ダイアログ

class COpenUrlDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenUrlDlg)

public:
	COpenUrlDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~COpenUrlDlg();

// ダイアログ データ
	enum { IDD = IDD_OPEN_URL_DIALOG };

	int m_encoding;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strUrl;
	CComboBox mc_comboEncoding;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
