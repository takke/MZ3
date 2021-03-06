/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// BookmarkMng.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "BookmarkMng.h"
#include "MixiData.h"
#include "util.h"
#include "IniFile.h"

/// オプションデータ
namespace option {

// Bookmark

Bookmark::Bookmark()
{
}

Bookmark::~Bookmark()
{
}


// Bookmark メンバ関数

void Bookmark::Load( CMixiDataList& bookmark )
{
	// ファイルから読み出し

	CString fileName;
	fileName.Format( _T("%s\\favorite.dat"), theApp.GetAppDirPath());

	FILE* fp;
	fp = _wfopen(fileName, _T("r"));
	if (fp!=NULL) {
		std::vector<TCHAR> line(4096);
		while (fgetws(&line[0], 4096, fp) != NULL) {
			// １行読み出し
			// 分解＆格納
			Devide( &line[0], bookmark );
		}
		fclose(fp);
	}
}

void Bookmark::Save( CMixiDataList& bookmark )
{
	CString fileName;
	fileName.Format( _T("%s\\favorite.dat"), theApp.GetAppDirPath());

	FILE* fp = _wfopen(fileName, _T("w"));

	INT_PTR count = bookmark.size();
	CString buf;
	CString type;

	for (int i=0; i<count; i++) {
		CMixiData& data = bookmark[i];
#ifdef BT_MZ3
		switch (data.GetAccessType()) {
		case ACCESS_BBS:
			type = _T("b");
			break;
		case ACCESS_ENQUETE:
			type = _T("e");
			break;
		case ACCESS_EVENT:
			type = _T("v");
			break;
		case ACCESS_EVENT_JOIN:
			type = _T("j");
			break;
		}
#endif
		buf.Format(_T("%s<>%d<>%s<>%s\n"), type, data.GetID(), data.GetName(), data.GetTitle());
		fputws(buf, fp);
	}

	fclose(fp);
}

void Bookmark::Devide( LPCTSTR line, CMixiDataList& bookmark )
{
	const CString& str = line;
	int index = 0;

	CMixiData data;

	{
		CString type = str.Tokenize(_T("<>"), index);
#ifdef BT_MZ3
		if (type == _T("b")) {
			data.SetAccessType( ACCESS_BBS );
		}else if (type == _T("e")) {
			data.SetAccessType( ACCESS_ENQUETE );
		}else if (type == _T("v")) {
			data.SetAccessType( ACCESS_EVENT );
		}else if (type == _T("j")) {
			data.SetAccessType( ACCESS_EVENT_JOIN );
		}
#endif
	}
	{
		CString id = str.Tokenize(_T("<>"), index);
		data.SetID( _wtoi(id) );
	}
	{
		CString name = str.Tokenize(_T("<>"), index);
		data.SetName( name );
	}
	{
		CString title = str.Tokenize(_T("<>"), index);
		title.Replace(_T("\n"), _T(""));
		data.SetTitle( title );
	}


	{
		// URL 生成
		CString uri;
#ifdef BT_MZ3
		switch (data.GetAccessType()) {
		case ACCESS_BBS:
			uri.Format( _T("view_bbs.pl?id=%d"), data.GetID() );
			break;
		case ACCESS_ENQUETE:
			uri.Format( _T("view_enquete.pl?id=%d"), data.GetID() );
			break;
		case ACCESS_EVENT:
		case ACCESS_EVENT_JOIN:
			uri.Format( _T("view_event.pl?id=%d"), data.GetID() );
			break;
		}
#endif

		data.SetURL(uri);
	}

	bookmark.push_back( data );
}

BOOL Bookmark::Add( CMixiData* master, CMixiDataList& bookmark )
{
	// 同一項目のチェック
	INT_PTR count = bookmark.size();
	for (int i=0; i<count; i++) {
		CMixiData& data = bookmark[i];
		if (data.GetID() == master->GetID()) {
			// 同じ項目があったので登録しない
			return FALSE;
		}
	}

	CMixiData data;
	data.SetName(master->GetName());
	data.SetID((int)master->GetID());
	data.SetAccessType(master->GetAccessType());
	data.SetCommentCount(master->GetCommentCount());
	data.SetURL(master->GetURL());

/*	CString url;
	switch (data.GetAccessType()) {
	case ACCESS_BBS:
		url.Format(_T("view_bbs.pl?id=%d"), data.GetID());
		break;
	case ACCESS_ENQUETE:
		url.Format(_T("view_enquete.pl?id=%d"), data.GetID());
		break;
	case ACCESS_EVENT:
		url.Format(_T("view_event.pl?id=%d"), data.GetID());
		break;
	}
*/
	data.SetURL(master->GetURL());

	data.SetTitle(master->GetTitle());

	CString buf = master->GetTitle();
	//int index = buf.ReverseFind(_T('(')) ;
	//if( index > 0 ) {
	//	data.SetTitle(buf.Left(index));
	//}

	// 追加
	bookmark.push_back( data );

	// 保存
	Save( bookmark );

	return TRUE;
}

BOOL Bookmark::Delete( CMixiData* data, CMixiDataList& bookmark )
{
	BOOL bRet = FALSE;
	INT_PTR count = bookmark.size();
	for (int i=0; i<count; i++) {
		if (data->GetID() == bookmark[i].GetID()) {
			bookmark.erase( bookmark.begin() + i );
			bRet = TRUE;
			break;
		}
	}

	Save( bookmark );

	return bRet;
}

}// namespace option
