#pragma once

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabDisplay �_�C�A���O

class COptionTabDisplay : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabDisplay)

public:
	COptionTabDisplay();
	virtual ~COptionTabDisplay();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_DISPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedResetTabButton();
	afx_msg void OnBnClickedSelectFontButton();
};

#endif