#pragma once


// CNameDlg �_�C�A���O

class CNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CNameDlg)

public:
	CNameDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CNameDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_NAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strName;
	afx_msg void OnBnClickedOk();
};
