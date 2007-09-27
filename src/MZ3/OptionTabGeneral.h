#pragma once

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL


// COptionTabGeneral ダイアログ

class COptionTabGeneral : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabGeneral)

public:
	COptionTabGeneral();
	virtual ~COptionTabGeneral();

	// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif