#pragma once

#include "MixiData.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ReportListCtrl.h"
#include "InetAccess.h"
#include "HtmlArray.h"
#include "XcrawlCanceler.h"

// CReportView �t�H�[�� �r���[

class CReportView : public CFormView
{
  DECLARE_DYNCREATE(CReportView)

public:
  CReportView();           // ���I�����Ŏg�p����� protected �R���X�g���N�^
  virtual ~CReportView();

public:
  enum { IDD = IDD_REPORTVIEW };
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
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	HBRUSH m_whiteBr;

	void SetData(CMixiData*);
	void EndProc();
	void SaveIndex();

	void Fit();

private:
	void ShowParentData(CMixiData*);
	void ShowCommentData(CMixiData*);

public:
	//--- UI
	CReportListCtrl		m_list;
	CEdit				m_edit;
	CEdit				m_titleEdit;
	CEdit				m_infoEdit;

	XcrawlCanceler		m_xcrawl;				///< Xcrawl ����
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN �̉�

private:
	CMixiData*	m_focusBodyItem;
	CMixiData*	m_data;
	CMixiData*	m_currentData;
	CImageList* m_pimgList;

	BOOL m_nochange;
	BOOL m_access;

	char m_idKey[24];
	int m_lastIndex;
	BOOL m_imageState;

	afx_msg void OnLvnItemchangedReportList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownReportList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnKeyDown(MSG* pMsg);
	BOOL OnKeyUp(MSG* pMsg);

	BOOL CommandMoveUpList();
	BOOL CommandMoveDownList();
	BOOL CommandScrollUpList();
	BOOL CommandScrollDownList();


	afx_msg void OnAddBookmark();
	afx_msg void OnDelBookmark();
	afx_msg void OnImageButton();
	afx_msg void OnWriteButton();
	afx_msg void OnWriteComment();
	afx_msg void OnOpenBrowser();
	afx_msg void OnLoadImage(UINT);
	afx_msg void OnLoadMovie(UINT);
	afx_msg void OnLoadPageLink(UINT);
	afx_msg void OnReloadPage();
	afx_msg void OnLoadUrl(UINT);
	afx_msg LRESULT OnGetEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnGetImageEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnGetError(WPARAM, LPARAM);
	afx_msg LRESULT OnGetAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeView(WPARAM, LPARAM);
	afx_msg LRESULT OnReload(WPARAM, LPARAM);
	afx_msg LRESULT OnAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessInformation(WPARAM, LPARAM);
	afx_msg LRESULT OnFit(WPARAM, LPARAM);
	afx_msg void OnEditCopy() { m_edit.Copy(); };
	afx_msg void OnSelectAll() { m_edit.SetSel(0, -1); };
	afx_msg LRESULT OnAccessLoaded(WPARAM, LPARAM);


	CProgressCtrl mc_progressBar;
	CHtmlArray* m_htmlarray;
	BOOL m_abort;

	int m_scrollLine;
	void MyPopupReportMenu(void);
	afx_msg void OnShowDebugInfo();
	afx_msg void OnOpenBrowserUser();
	afx_msg void OnUpdateWriteComment(CCmdUI *pCmdUI);
	afx_msg void OnMenuBack();
public:
	afx_msg void OnBackMenu();
	afx_msg void OnNextMenu();
	void ResetColumnWidth(const CMixiData& mixi);
	afx_msg void OnHdnEndtrackReportList(NMHDR *pNMHDR, LRESULT *pResult);
	int GetListWidth(void);

	//--- HTML �R���g���[���ɂ��`��Ή�
	HWND m_hwndHtml;						///< HTML �R���g���[��
	void SetHtmlText(LPCTSTR szHtmlText);	///< HTML �R���g���[���ւ�HTML�̐ݒ�
	int	m_posHtmlScroll;					///< HTML �R���g���[���̃X�N���[���ʒu
	int m_posHtmlScrollMax;					///< HTML �R���g���[���̃X�N���[���ʒu�̍ő�l
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL LoadHTMLImage(LPCTSTR szTarget, DWORD dwCookie);
public:
	afx_msg void OnLayoutReportlistMakeNarrow();
	afx_msg void OnLayoutReportlistMakeWide();
};


