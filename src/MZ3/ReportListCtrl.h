#pragma once

/**
 * レポートリスト（レポート画面上側領域）
 */
class CReportListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CReportListCtrl)

public:
	CReportListCtrl();
	virtual ~CReportListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

public:
	int GetSelectedItem(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


