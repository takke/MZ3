#pragma once
#include "c:\program files\microsoft visual studio 8\vc\atlmfc\include\afxwin.h"


// COpenUrlDlg ダイアログ

class COpenUrlDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenUrlDlg)

public:
	COpenUrlDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~COpenUrlDlg();

// ダイアログ データ
	enum { IDD = IDD_OPEN_URL_DIALOG };

	int m_encoding;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString mc_strUrl;
	CComboBox mc_comboEncoding;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
