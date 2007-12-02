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