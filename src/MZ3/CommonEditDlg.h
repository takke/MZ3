/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once


// CCommonEditDlg ダイアログ

class CCommonEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonEditDlg)

public:
	CCommonEditDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCommonEditDlg();

// ダイアログ データ
	enum { IDD = IDD_COMMON_EDIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CString mc_strEdit;
	CString mc_strEdit2;
	CString mc_strMessage;
	std::vector<CString> m_comboTextList;	///< コンボボックスに設定する文字列
	CString m_strSelectedComboText;

	afx_msg void OnBnClickedOk();
	void SetTitle(LPCTSTR szTitle);
	void SetMessage(LPCTSTR szMessage);
	virtual BOOL OnInitDialog();
	CComboBox mc_comboBox;
};
