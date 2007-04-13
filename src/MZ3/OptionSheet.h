#pragma once

#include "OptionTabUser.h"
#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "OptionTabMInfo.h"

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
	COptionTabUser m_userTab;
	COptionTabProxy m_proxyTab;
	COptionTabMInfo m_infoTab;
	COptionTabGetPage m_pageTab;
};


