/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#ifndef SMARTPHONE2003_UI_MODEL

#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "OptionTabGeneral.h"
#include "OptionTabUI.h"
#include "OptionTabDisplay.h"
#include "OptionTabMainView.h"
#include "OptionTabReport.h"
#include "OptionTabLog.h"

// COptionSheet

class COptionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionSheet)

public:
	COptionSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionSheet();

	void SetPage();

protected:
	DECLARE_MESSAGE_MAP()

private:
	COptionTabGeneral	m_tabGeneral;	///< ��ʃ^�u
	COptionTabDisplay	m_tabDisplay;	///< �\���^�u
	COptionTabUI		m_tabUi;		///< ����^�u
	COptionTabMainView	m_tabMainView;	///< ���C����ʃ^�u
	COptionTabReport	m_tabReport;	///< ���|�[�g��ʃ^�u
	COptionTabGetPage	m_tabGet;		///< �擾�^�u
	COptionTabLog		m_tabLog;		///< ���O�^�u
	COptionTabProxy		m_tabProxy;		///< PROXY�^�u
public:
	afx_msg void OnDestroy();
	int m_iLastPage;
};

#endif