#pragma once

/**
 * ���|�[�g���X�g�i���|�[�g��ʏ㑤�̈�j
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

	COLORREF m_clrBgFirst;
	COLORREF m_clrBgSecond;
	COLORREF m_clrFgFirst;
	COLORREF m_clrFgSecond;

	HBITMAP m_hBitmap;			///< �w�i�p�r�b�g�}�b�v

public:
	int GetSelectedItem(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


