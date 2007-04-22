// OptionSheet.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionSheet.h"


// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
END_MESSAGE_MAP()


// COptionSheet メッセージ ハンドラ

void COptionSheet::SetPage()
{
	AddPage( &m_tabUser );		// ログインタブ
	AddPage( &m_tabGeneral );	// 一般タブ
	AddPage( &m_tabUi );		// UIタブ
	AddPage( &m_tabProxy );		// プロキシタブ
	AddPage( &m_tabGet );		// 取得タブ
	AddPage( &m_tabLog );		// ログタブ
}
