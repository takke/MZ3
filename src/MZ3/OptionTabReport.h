/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabReport ダイアログ

class COptionTabReport : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabReport)

public:
	COptionTabReport();
	virtual ~COptionTabReport();

// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif