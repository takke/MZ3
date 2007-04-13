#pragma once
#include "afxwin.h"
#include "QuoteHelper.h"

// CQuoteDlg ダイアログ

class CQuoteDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuoteDlg)

public:
	CQuoteDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CQuoteDlg();

// ダイアログ データ
	enum { IDD = IDD_QUOTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_typeList;
	quote::QuoteType m_quoteType;		///< [in/out] 引用種別

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnSelchangeTypeList();

	CMixiData* m_pMixi;
	CEdit m_sampleEdit;
	CEdit m_msgEdit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
