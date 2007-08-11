#pragma once


// CCommonSelectDlg ダイアログ

class CCommonSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonSelectDlg)

public:
	CCommonSelectDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CCommonSelectDlg();

	/**
	 * ボタンのコード
	 */
	enum BUTTONCODE {
		BUTTONCODE_SELECT1 = 0,		///< 選択肢1
		BUTTONCODE_SELECT2 = 1,		///< 選択肢2
		BUTTONCODE_CANCEL  = 2,		///< キャンセル
	};

	BUTTONCODE m_pressedButtonCode;		///< 押下されたボタンのコード

	CString m_strButton1;
	CString m_strButton2;
	CString m_strButtonCancel;
	CString m_strMessage;

// ダイアログ データ
	enum { IDD = IDD_COMMON_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect1Button();
	afx_msg void OnBnClickedSelect2Button();
	afx_msg void OnBnClickedCancel();
	bool SetButtonText(CCommonSelectDlg::BUTTONCODE buttoncode, LPCTSTR szText);
	bool SetMessage(LPCTSTR szMessage);
	virtual BOOL OnInitDialog();
};
