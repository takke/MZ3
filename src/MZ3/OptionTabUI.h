#pragma once

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabUI ダイアログ

class COptionTabUI : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabUI)

public:
	COptionTabUI();
	virtual ~COptionTabUI();

	// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_UI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif