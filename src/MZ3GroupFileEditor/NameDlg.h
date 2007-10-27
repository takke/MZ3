#pragma once


// CNameDlg ダイアログ

class CNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CNameDlg)

public:
	CNameDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CNameDlg();

// ダイアログ データ
	enum { IDD = IDD_NAME_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strName;
	afx_msg void OnBnClickedOk();
};
