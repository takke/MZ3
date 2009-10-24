/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
#include "OptionTabTwitter.h"

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
	COptionTabTwitter	m_tabTwitter;	///< Twitter�^�u
public:
	afx_msg void OnDestroy();
	int m_iLastPage;
};

#endif