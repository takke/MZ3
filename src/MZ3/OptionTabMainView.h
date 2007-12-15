/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

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
	CComboBox mc_comboMiniImageSize;
	afx_msg void OnBnClickedShowMiniImageDlgCheck();
	afx_msg void OnBnClickedShowMiniImageCheck();
	void UpdateControlItemStatus(void);
};

#endif