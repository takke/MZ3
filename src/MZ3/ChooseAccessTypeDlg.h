/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// CChooseAccessTypeDlg ダイアログ

class CChooseAccessTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseAccessTypeDlg)

public:
	CChooseAccessTypeDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CChooseAccessTypeDlg();

// ダイアログ データ
	enum { IDD = IDD_CHOOSE_ACCESS_TYPE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox mc_listAccessType;
	ACCESS_TYPE m_selectedAccessType;

protected:
	virtual void OnOK();
public:
	afx_msg void OnLbnDblclkAccessTypeList();
};
