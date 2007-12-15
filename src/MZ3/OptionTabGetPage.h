/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "afxwin.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabGetPage �_�C�A���O

class COptionTabGetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabGetPage)

public:
	COptionTabGetPage();
	virtual ~COptionTabGetPage();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_GETPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	void Load();
	void Save();

	CComboBox mc_RecvBufCombo;
};

#endif