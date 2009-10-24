/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabUI �_�C�A���O

class COptionTabUI : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabUI)

public:
	COptionTabUI();
	virtual ~COptionTabUI();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_UI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif