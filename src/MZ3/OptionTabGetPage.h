#pragma once
#include "afxwin.h"


// COptionTabGetPage �_�C�A���O

class COptionTabGetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabGetPage)

public:
	COptionTabGetPage();
	virtual ~COptionTabGetPage();

	// �_�C�A���O �f�[�^
	enum { IDD = IDD_OPTIONTAB_GETPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	void Load();
	void Save();

	afx_msg void OnBnClickedChangeLogFolderButton();
	CComboBox mc_RecvBufCombo;
};
