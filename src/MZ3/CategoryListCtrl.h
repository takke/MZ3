#pragma once


/**
 * �J�e�S�����X�g�i���C�����2�i�ڗ̈�j
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

	int m_activeItem;			///< �Ԃ��A�C�e���̃C���f�b�N�X

	HBITMAP m_hBitmap;			///< �w�i�p�r�b�g�}�b�v

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


