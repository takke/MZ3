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
	COptionTabUser		m_userTab;		///< ���O�C���^�u
	COptionTabMInfo		m_infoTab;		///< ��ʃ^�u
	COptionTabUI		m_uiTab;		///< UI�^�u
	COptionTabProxy		m_proxyTab;		///< PROXY�^�u
	COptionTabGetPage	m_pageTab;		///< �擾�^�u
	COptionTabLog		m_logTab;		///< ���O�^�u
};


