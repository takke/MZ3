/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "../MZ3/CategoryItem.h"
#include "afxwin.h"

// CCategorySettingDlg �_�C�A���O

class CCategorySettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCategorySettingDlg)

public:
	CCategorySettingDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CCategorySettingDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CATEGORY_SETTING_DIALOG };

	CCategoryItem m_item;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CString mc_strName;
	CComboBox mc_comboType;
	CString mc_strUrl;
	afx_msg void OnBnClickedTemplateButton();
	afx_msg void OnTemplateMenuItems(UINT nID);
	void MyLoadItem(void);
};
