#pragma once
#include "afxwin.h"


// COptionTabLog �_�C�A���O

class COptionTabLog : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabLog)

public:
	COptionTabLog();
	virtual ~COptionTabLog();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	void Load();
	void Save();

	afx_msg void OnBnClickedChangeLogFolderButton();
};