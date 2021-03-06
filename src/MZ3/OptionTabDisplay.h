/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabDisplay ダイアログ

class COptionTabDisplay : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabDisplay)

public:
	COptionTabDisplay();
	virtual ~COptionTabDisplay();

// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedResetTabButton();
	afx_msg void OnBnClickedSelectFontButton();
};

#endif