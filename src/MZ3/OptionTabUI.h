#pragma once


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
	afx_msg void OnBnClickedResetTabButton();
	afx_msg void OnBnClickedSelectFontButton();
};
