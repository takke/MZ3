/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "c:\program files\microsoft visual studio 8\vc\atlmfc\include\afxwin.h"


// CUserDlg ダイアログ

class CUserDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserDlg)

public:
	CUserDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CUserDlg();

// ダイアログ データ
	enum { IDD = IDD_USER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	/// コンボボックスのインデックスと種別の対応
	enum USER_DLG_COMBO_INDEX_TYPE {
		USER_DLG_COMBO_INDEX_TYPE_MIXI = 0,
		USER_DLG_COMBO_INDEX_TYPE_TWITTER = 1,
		USER_DLG_COMBO_INDEX_TYPE_WASSR = 2,
	};

	CComboBox					mc_comboType;
	USER_DLG_COMBO_INDEX_TYPE	m_idxSelectedCombo;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeTypeCombo();
	void MySaveControlData(void);
	void MyLoadControlData(void);
};
