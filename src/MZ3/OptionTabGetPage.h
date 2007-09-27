#pragma once
#include "afxwin.h"

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL


// COptionTabGetPage ダイアログ

class COptionTabGetPage : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionTabGetPage)

public:
	COptionTabGetPage();
	virtual ~COptionTabGetPage();

	// ダイアログ データ
	enum { IDD = IDD_OPTIONTAB_GETPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

private:
	void Load();
	void Save();

	CComboBox mc_RecvBufCombo;
};

#endif