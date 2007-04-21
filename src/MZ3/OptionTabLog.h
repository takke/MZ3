#pragma once
#include "afxwin.h"


// COptionTabLog ダイアログ

class COptionTabLog : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabLog)

public:
	COptionTabLog();
	virtual ~COptionTabLog();

	// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	void Load();
	void Save();

	afx_msg void OnBnClickedChangeLogFolderButton();
};
