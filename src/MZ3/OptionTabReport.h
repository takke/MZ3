#pragma once


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
