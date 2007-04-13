#pragma once
#include "afxwin.h"
#include "QuoteHelper.h"

// CQuoteDlg �_�C�A���O

class CQuoteDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuoteDlg)

public:
	CQuoteDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CQuoteDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_QUOTE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_typeList;
	quote::QuoteType m_quoteType;		///< [in/out] ���p���

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnSelchangeTypeList();

	CMixiData* m_pMixi;
	CEdit m_sampleEdit;
	CEdit m_msgEdit;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
