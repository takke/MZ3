/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3Doc.cpp : CMZ3Doc クラスの実装
//

#include "stdafx.h"
#include "MZ3.h"

#include "MZ3Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3Doc

IMPLEMENT_DYNCREATE(CMZ3Doc, CDocument)

BEGIN_MESSAGE_MAP(CMZ3Doc, CDocument)
END_MESSAGE_MAP()

// CMZ3Doc コンストラクション/デストラクション

CMZ3Doc::CMZ3Doc()
{
	// TODO: この位置に 1 度だけ呼ばれる構築用のコードを追加してください。

}

CMZ3Doc::~CMZ3Doc()
{
}

BOOL CMZ3Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)

	return TRUE;
}

// CMZ3Doc シリアル化

#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
void CMZ3Doc::Serialize(CArchive& ar)
{
	(ar);
}
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT


// CMZ3Doc 診断

#ifdef _DEBUG
void CMZ3Doc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif //_DEBUG


// CMZ3Doc コマンド

