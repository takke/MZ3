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
	AddPage( &m_userTab );			// ログインタブ
	AddPage( &m_infoTab );			// 一般タブ
	AddPage( &m_uiTab );			// UIタブ
	AddPage( &m_proxyTab );			// プロキシタブ
	AddPage( &m_pageTab );			// 取得タブ
	AddPage( &m_logTab );			// ログタブ
}
