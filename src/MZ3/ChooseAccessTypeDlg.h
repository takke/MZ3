#pragma once
#include "c:\program files\microsoft visual studio 8\vc\atlmfc\include\afxwin.h"


// CChooseAccessTypeDlg �_�C�A���O

class CChooseAccessTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CChooseAccessTypeDlg)

public:
	CChooseAccessTypeDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CChooseAccessTypeDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_CHOOSE_ACCESS_TYPE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox mc_listAccessType;
	ACCESS_TYPE m_selectedAccessType;

protected:
	virtual void OnOK();
public:
	afx_msg void OnLbnDblclkAccessTypeList();
};
