/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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