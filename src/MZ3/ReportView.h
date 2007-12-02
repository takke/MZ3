#pragma once

#include "MixiData.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ReportListCtrl.h"
#include "InetAccess.h"
#include "HtmlArray.h"
#include "XcrawlCanceler.h"
#include "Ran2View.h"
#include "ReportScrollBar.h"

// CReportView フォーム ビュー

class CReportView : public CFormView
{
	DECLARE_DYNCREATE(CReportView)

public:
	CReportView();           // 動的生成で使用される protected コンストラクタ
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	void SetData(const CMixiData& data);
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

	ReportScrollBar		m_vScrollbar;
	Ran2View*			m_detailView;			// 絵文字対応コントロール
	XcrawlCanceler		m_xcrawl;				///< Xcrawl 制御
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN の回数
	int					m_scrollBarHeight;		// 垂直スクロールバーが扱える行数

private:
	CMixiData*	m_focusBodyItem;
	CMixiData 	m_data;
	CMixiData*	m_currentData;
	CImageList  m_pimgList;

	BOOL		m_nochange;
	BOOL		m_access;

	int			m_lastIndex;
	BOOL		m_imageState;

	afx_msg void OnLvnItemchangedReportList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownReportList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnKeyDown(MSG* pMsg);
	BOOL OnKeyUp(MSG* pMsg);

	BOOL CommandMoveUpList();
	BOOL CommandMoveDownList();
	BOOL CommandMoveToFirstList();
	BOOL CommandMoveToLastList();
	BOOL CommandScrollUpEdit();
	BOOL CommandScrollDownEdit();


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
	afx_msg LRESULT OnGetEndBinary(WPARAM, LPARAM);
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
	BOOL m_abort;

	int m_scrollLine;
	afx_msg void OnShowDebugInfo();
	afx_msg void OnOpenBrowserUser();
	afx_msg void OnUpdateWriteComment(CCmdUI *pCmdUI);
	afx_msg void OnMenuBack();
public:
	void MyPopupReportMenu(void);
	afx_msg void OnBackMenu();
	afx_msg void OnNextMenu();
	void ResetColumnWidth(const CMixiData& mixi);
	afx_msg void OnHdnEndtrackReportList(NMHDR *pNMHDR, LRESULT *pResult);
	int GetListWidth(void);

	//--- HTML コントロールによる描画対応
	HWND m_hwndHtml;						///< HTML コントロール
	void SetHtmlText(LPCTSTR szHtmlText);	///< HTML コントロールへのHTMLの設定
	int	m_posHtmlScroll;					///< HTML コントロールのスクロール位置
	int m_posHtmlScrollMax;					///< HTML コントロールのスクロール位置の最大値
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL LoadHTMLImage(LPCTSTR szTarget, DWORD dwCookie);
public:
	afx_msg void OnLayoutReportlistMakeNarrow();
	afx_msg void OnLayoutReportlistMakeWide();
	afx_msg void OnEnVscrollReportEdit();
	afx_msg void OnNMRclickReportList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	bool MyLoadMixiViewPage( const CMixiData::Link link );
public:
	afx_msg void OnOpenProfile();
	afx_msg void OnOpenProfileLog();
	afx_msg void OnSendMessage();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


