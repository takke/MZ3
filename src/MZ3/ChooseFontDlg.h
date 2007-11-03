#pragma once
#include "afxcmn.h"


// CChooseFontDlg �_�C�A���O

class CChooseFontDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseFontDlg)

public:
	CChooseFontDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CChooseFontDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CHOOSE_FONT_DIALOG };

	CString m_strSelectedFont;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_FontList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMReturnFontList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
public:
	afx_msg void OnLvnKeydownFontList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkFontList(NMHDR *pNMHDR, LRESULT *pResult);
};
