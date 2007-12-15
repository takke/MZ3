/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "afxwin.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabLog ダイアログ

class COptionTabLog : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabLog)

public:
	COptionTabLog();
	virtual ~COptionTabLog();

	// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	void Load();
	void Save();

	afx_msg void OnBnClickedChangeLogFolderButton();
public:
	afx_msg void OnBnClickedCleanLogButton();
	afx_msg void OnBnClickedDebugModeCheck();
};

#endif