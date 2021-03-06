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
	COptionTabGeneral	m_tabGeneral;	///< 一般タブ
	COptionTabDisplay	m_tabDisplay;	///< 表示タブ
	COptionTabUI		m_tabUi;		///< 操作タブ
	COptionTabMainView	m_tabMainView;	///< メイン画面タブ
	COptionTabReport	m_tabReport;	///< レポート画面タブ
	COptionTabGetPage	m_tabGet;		///< 取得タブ
	COptionTabLog		m_tabLog;		///< ログタブ
	COptionTabProxy		m_tabProxy;		///< PROXYタブ
	COptionTabTwitter	m_tabTwitter;	///< Twitterタブ
public:
	afx_msg void OnDestroy();
	int m_iLastPage;
};

#endif