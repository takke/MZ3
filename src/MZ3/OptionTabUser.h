#pragma once


// COptionTabUser ダイアログ

class COptionTabUser : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabUser)

public:
	COptionTabUser();
	virtual ~COptionTabUser();

// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_USER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
