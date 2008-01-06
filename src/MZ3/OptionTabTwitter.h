#pragma once


// COptionTabTwitter ダイアログ

class COptionTabTwitter : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabTwitter)

public:
	COptionTabTwitter();
	virtual ~COptionTabTwitter();

// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_TWITTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
