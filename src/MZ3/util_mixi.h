/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * mixi�֘A���[�e�B���e�B
 */
#include "util_base.h"
#include "MixiUrlParser.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url );

/**
 * ���[�UID�� AuthorID, OwnerID ����擾����
 */
inline int GetUserIdFromAuthorOrOwnerID( const CMixiData& mixi )
{
	int userId = mixi.GetAuthorID();
	if( userId < 0 ) {
		userId = mixi.GetOwnerID();
	}
	return userId;
}

/**
 * url �쐬�ihttp://mixi.jp/ �̕⊮�j
 */
inline CString CreateMixiUrl(LPCTSTR str)
{
	if( wcsstr( str, L"mixi.jp" ) == NULL &&
		wcsstr( str, L"http://" ) == NULL)
	{
		CString uri;
		uri.Format(_T("http://mixi.jp/%s"), str);
		return uri;
	}else{
		return str;
	}
}

/**
 * ���ǁE���Ǐ�ԊǗ����O�t�@�C���p��ID�𐶐�����B
 */
inline CString GetLogIdString( const CMixiData& mixi )
{
	CString logId;

	switch (mixi.GetAccessType()) {
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
	case ACCESS_NEIGHBORDIARY:
		logId.Format(_T("d%d"), mixi::MixiUrlParser::GetID( mixi.GetURL() ) );
		break;
	case ACCESS_BBS:
		logId.Format(_T("b%d"), mixi.GetID());
		break;
	case ACCESS_EVENT:
		logId.Format(_T("v%d"), mixi.GetID());
		break;
	case ACCESS_EVENT_JOIN:
		logId.Format(_T("j%d"), mixi.GetID());
		break;
	case ACCESS_ENQUETE:
		logId.Format(_T("e%d"), mixi.GetID());
		break;
	case ACCESS_PROFILE:
		logId.Format(_T("p%d"), mixi.GetID());
		break;

	default:
		logId = L"";
		break;
	}

	return logId;
}

}