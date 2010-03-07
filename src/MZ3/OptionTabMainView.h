/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
#include "afxwin.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabMainView ダイアログ

class COptionTabMainView : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabMainView)

public:
	COptionTabMainView();
	virtual ~COptionTabMainView();

// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_MAIN_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedShowMiniImageCheck();
	void UpdateControlItemStatus(void);
	CComboBox mc_comboIconSize;
	CComboBox mc_comboBodyListLineCount;
};

#endif