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
	COptionTabUser		m_tabUser;		///< ���O�C���^�u
	COptionTabGeneral	m_tabGeneral;	///< ��ʃ^�u
	COptionTabUI		m_tabUi;		///< UI�^�u
	COptionTabProxy		m_tabProxy;		///< PROXY�^�u
	COptionTabGetPage	m_tabGet;		///< �擾�^�u
	COptionTabLog		m_tabLog;		///< ���O�^�u
};


