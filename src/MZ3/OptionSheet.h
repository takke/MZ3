#pragma once

#include "OptionTabUser.h"
#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "OptionTabGeneral.h"
#include "OptionTabUI.h"
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
	COptionTabUser		m_tabUser;		///< ログインタブ
	COptionTabGeneral	m_tabGeneral;	///< 一般タブ
	COptionTabUI		m_tabUi;		///< UIタブ
	COptionTabProxy		m_tabProxy;		///< PROXYタブ
	COptionTabGetPage	m_tabGet;		///< 取得タブ
	COptionTabLog		m_tabLog;		///< ログタブ
};


