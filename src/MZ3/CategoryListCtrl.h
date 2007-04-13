#pragma once


/**
 * カテゴリリスト（メイン画面2段目領域）
 */
class CCategoryListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCategoryListCtrl)

public:
	CCategoryListCtrl();
	virtual ~CCategoryListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	void SetActiveItem(int value) { m_activeItem = value; };
	int GetActiveItem() { return m_activeItem; };

	bool	m_bStopDraw;
private:
	COLORREF m_clrBgFirst;
	COLORREF m_clrBgSecond;
	COLORREF m_clrFgFirst;
	COLORREF m_clrFgSecond;

	int m_activeItem;			///< 赤いアイテムのインデックス

	HBITMAP m_hBitmap;			///< 背景用ビットマップ

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


