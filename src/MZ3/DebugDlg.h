#pragma once

#include "MixiData.h"
#include "afxcmn.h"

// CDebugDlg ダイアログ

class CDebugDlg : public CDialog
{
	DECLARE_DYNAMIC(CDebugDlg)

public:
	CDebugDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDebugDlg();

	CMixiData* m_data;

// ダイアログ データ
	enum { IDD = IDD_DEBUG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnNMClickDebugList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLvnKeydownDebugList(NMHDR *pNMHDR, LRESULT *pResult);
};
