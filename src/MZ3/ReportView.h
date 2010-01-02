/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MixiData.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ReportListCtrl.h"
#include "InetAccess.h"
#include "HtmlArray.h"
#include "XcrawlCanceler.h"
#include "ReportScrollBar.h"

class Ran2View;

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
	CEdit				m_titleEdit;
	CEdit				m_infoEdit;

	ReportScrollBar		m_vScrollbar;
	Ran2View*			m_detailView;			// 絵文字対応コントロール
	XcrawlCanceler		m_xcrawl;				///< Xcrawl 制御
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN の回数
	int					m_scrollBarHeight;		// 垂直スクロールバーが扱える行数
	BOOL m_abort;

private:
	CMixiData*	m_focusBodyItem;
	CMixiData 	m_data;
	CMixiData*	m_currentData;
	CImageList  m_pimgList;

	BOOL		m_nochange;

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
#ifdef BT_MZ3
	afx_msg void OnWriteButton();
	afx_msg void OnWriteComment();
#endif
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
	afx_msg void OnEditCopy();
	afx_msg LRESULT OnAccessLoaded(WPARAM, LPARAM);

	afx_msg LRESULT OnMoveDownList(WPARAM, LPARAM);
	afx_msg LRESULT OnMoveUpList(WPARAM, LPARAM);
	afx_msg LRESULT OnGetListItemCount(WPARAM, LPARAM);

	CProgressCtrl mc_progressBar;

	int m_scrollLine;
	afx_msg void OnShowDebugInfo();
	afx_msg void OnOpenBrowserUser();
	afx_msg void OnUpdateWriteComment(CCmdUI *pCmdUI);
	afx_msg void OnMenuBack();

	afx_msg void OnCopyClipboardUrl(UINT nID);
	afx_msg void OnCopyClipboardImage(UINT nID);
	afx_msg void OnCopyClipboardMovie(UINT nID);
public:
	void MyPopupReportMenu(POINT pt_=CPoint(0,0), int flags_=0);
	afx_msg void OnBackMenu();
	afx_msg void OnNextMenu();
	void ResetColumnWidth(const CMixiData& mixi);
	afx_msg void OnHdnEndtrackReportList(NMHDR *pNMHDR, LRESULT *pResult);
	int GetListWidth(void);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
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
#ifdef BT_MZ3
	afx_msg void OnOpenProfileLog();
	afx_msg void OnSendMessage();
#endif
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAcceleratorScrollOrNextComment();
	afx_msg void OnAcceleratorNextComment();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMenuNextDiary();
	afx_msg void OnMenuPrevDiary();
	afx_msg void OnUpdateMenuNextDiary(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuPrevDiary(CCmdUI *pCmdUI);
	afx_msg void OnLoadFullDiary();
	afx_msg void OnUpdateLoadFullDiary(CCmdUI *pCmdUI);
	void MyUpdateControlStatus(void);
	void OnLuaMenu(UINT nID);
};


