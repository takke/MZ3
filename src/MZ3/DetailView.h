#pragma once

#include "MZ3Data.h"

// CDetailView �t�H�[�� �r���[

class CDetailView : public CFormView
{
	DECLARE_DYNCREATE(CDetailView)

public:
	CDetailView();           // ���I�����Ŏg�p����� protected �R���X�g���N�^
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
	CDC*		m_memDC;			///< �����DC
	CBitmap*	m_memBMP;			///< ����ʃo�b�t�@
	CBitmap*	m_oldBMP;			///< ����ʂ̏��

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

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


