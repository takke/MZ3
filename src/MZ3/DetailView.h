#pragma once

#include "MZ3Data.h"

// CDetailView フォーム ビュー

class CDetailView : public CFormView
{
	DECLARE_DYNCREATE(CDetailView)

public:
	CDetailView();           // 動的生成で使用される protected コンストラクタ
	virtual ~CDetailView();

	MZ3Data	m_data;

public:
	enum { IDD = IDD_DETAILVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CDC*		m_memDC;			///< 裏画面DC
	CBitmap*	m_memBMP;			///< 裏画面バッファ
	CBitmap*	m_oldBMP;			///< 旧画面の情報

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedExitButton();
	virtual void OnInitialUpdate();
	LRESULT OnFit(WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool PopupDetailMenu(POINT pt_, int flags_);
	bool MyMakeBackBuffers(CDC* pdc);
	afx_msg void OnDestroy();
};


