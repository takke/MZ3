/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "../MZ3/CategoryItem.h"
#include "afxwin.h"

// CCategorySettingDlg ダイアログ

class CCategorySettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCategorySettingDlg)

public:
	CCategorySettingDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCategorySettingDlg();

// ダイアログ データ
	enum { IDD = IDD_CATEGORY_SETTING_DIALOG };

	CCategoryItem m_item;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

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
