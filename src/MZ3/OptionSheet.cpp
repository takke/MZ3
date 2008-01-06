/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// OptionSheet.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionSheet.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
	,m_iLastPage(1)
{

}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// COptionSheet メッセージ ハンドラ

void COptionSheet::SetPage()
{
	AddPage( &m_tabGeneral );	// 一般タブ
	AddPage( &m_tabDisplay );	// 表示タブ
	AddPage( &m_tabUi );		// 操作タブ
	AddPage( &m_tabMainView );	// メイン画面タブ
	AddPage( &m_tabReport );	// レポート画面タブ
	AddPage( &m_tabGet );		// 取得タブ
	AddPage( &m_tabLog );		// ログタブ
	AddPage( &m_tabProxy );		// プロキシタブ
	AddPage( &m_tabTwitter );	// Twitterタブ
}

void COptionSheet::OnDestroy()
{
	m_iLastPage = GetActiveIndex();

	CPropertySheet::OnDestroy();
}

#endif