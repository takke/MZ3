#pragma once

#include <vector>

class DownloadItem {
public:
	CString url;			///< URL
	CString text;			///< ����
	CString localpath;		///< �_�E�����[�h��t�@�C���p�X

	bool bBinary;			///< �o�C�i���H�i�{����MIME/TYPE�Ƃ��ׂ��j
	CInetAccess::ENCODING encoding;	///< ��o�C�i���i�e�L�X�g�j���̃G���R�[�f�B���O

	bool bFinished;			///< �����H

	DownloadItem( LPCTSTR url_, LPCTSTR text_, LPCTSTR localpath_, bool bBinary_, 
		CInetAccess::ENCODING encoding_=CInetAccess::ENCODING_EUC )
		: bFinished(false)
		, url(url_)
		, text(text_)
		, localpath(localpath_)
		, bBinary(bBinary_)
		, encoding(encoding_)
	{
	}
};


// CDownloadView �t�H�[�� �r���[

class CDownloadView : public CFormView
{
	DECLARE_DYNCREATE(CDownloadView)

public:
	CDownloadView();           // ���I�����Ŏg�p����� protected �R���X�g���N�^
	virtual ~CDownloadView();

	CEdit			m_titleEdit;
	CProgressCtrl	mc_progressBar;
	CListCtrl		m_list;
	CEdit			m_infoEdit;
	CButton			mc_checkContinue;
	CImageList		m_imageList;

	std::vector<DownloadItem> m_items;
	int				m_targetItemIndex;
	BOOL			m_access;
	BOOL			m_abortRequested;	///< ���[�U�����f�������s�������H

public:
	enum { IDD = IDD_DOWNLOADVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ResetColumnWidth(void);
	int GetListWidth(void);
	void MyUpdateControls(void);
	afx_msg void OnBnClickedContinueCheck();
	afx_msg LRESULT OnFit(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedStartStopButton();
	afx_msg void OnBnClickedExitButton();
	afx_msg LRESULT OnAccessInformation(WPARAM, LPARAM);
	afx_msg LRESULT OnGetEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnGetEndBinary(WPARAM, LPARAM);
	afx_msg LRESULT OnGetError(WPARAM, LPARAM);
	afx_msg LRESULT OnGetAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessLoaded(WPARAM, LPARAM);
	bool DoDownloadSelectedItem(void);
	bool AppendDownloadItem(const DownloadItem& item);
};


