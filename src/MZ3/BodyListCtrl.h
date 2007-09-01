#pragma once

/**
 * ボディーリスト（メイン画面下側領域）
 */
class CBodyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CBodyListCtrl)

public:
	CBodyListCtrl();
	virtual ~CBodyListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	void SetUseColor(BOOL flag) { m_useColor = flag; };
	BOOL IsUseColor() { return m_useColor; };

	void SetHeader(LPCTSTR, LPCTSTR);
	bool	 m_bStopDraw;		///< DrawItem 実行可否フラグ（SetDraw）

private:
	COLORREF m_clrBgFirst;
	COLORREF m_clrBgSecond;
	COLORREF m_clrFgFirst;
	COLORREF m_clrFgSecond;

	BOOL	 m_useColor;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


