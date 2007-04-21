#pragma once

#include "OptionTabUser.h"
#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "OptionTabMInfo.h"
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
	COptionTabUser		m_userTab;		///< ログインタブ
	COptionTabMInfo		m_infoTab;		///< 一般タブ
	COptionTabUI		m_uiTab;		///< UIタブ
	COptionTabProxy		m_proxyTab;		///< PROXYタブ
	COptionTabGetPage	m_pageTab;		///< 取得タブ
	COptionTabLog		m_logTab;		///< ログタブ
};


