/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once


// CChooseClientTypeDlg ダイアログ

class CChooseClientTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseClientTypeDlg)

public:
	CChooseClientTypeDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CChooseClientTypeDlg();

// ダイアログ データ
	enum { IDD = IDD_CHOOSE_CLIENT_TYPE_DIALOG };

	Mz3GroupData::InitializeType m_initType;

	CListCtrl mc_serviceList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
