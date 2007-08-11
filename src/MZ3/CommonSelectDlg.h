#pragma once


// CCommonSelectDlg �_�C�A���O

class CCommonSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonSelectDlg)

public:
	CCommonSelectDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CCommonSelectDlg();

	/**
	 * �{�^���̃R�[�h
	 */
	enum BUTTONCODE {
		BUTTONCODE_SELECT1 = 0,		///< �I����1
		BUTTONCODE_SELECT2 = 1,		///< �I����2
		BUTTONCODE_CANCEL  = 2,		///< �L�����Z��
	};

	BUTTONCODE m_pressedButtonCode;		///< �������ꂽ�{�^���̃R�[�h

	CString m_strButton1;
	CString m_strButton2;
	CString m_strButtonCancel;
	CString m_strMessage;

// �_�C�A���O �f�[�^
	enum { IDD = IDD_COMMON_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect1Button();
	afx_msg void OnBnClickedSelect2Button();
	afx_msg void OnBnClickedCancel();
	bool SetButtonText(CCommonSelectDlg::BUTTONCODE buttoncode, LPCTSTR szText);
	bool SetMessage(LPCTSTR szMessage);
	virtual BOOL OnInitDialog();
};
