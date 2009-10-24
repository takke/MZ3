/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "util_base.h"

/// mixi �pHTML�p�[�T
namespace mixi {

/// mixi �� URL �𕪉����郆�[�e�B���e�B
class MixiUrlParser {
public:

	/**
	 * URL ����I�[�i�[ID���擾����B
	 *
	 * view_diary.pl?id=xxx&owner_id=xxx
	 */
	static CString GetOwnerID( const CString& url )
	{
		CString id = util::GetParamFromURL( url, L"owner_id" );
		return id;
	}

	/**
	 * URL ����ID���擾����B
	 *
	 * view_community.pl?id=1231285 => 1231285
	 * view_diary.pl?id=xxxx&owner_id=yyyy => xxxx
	 */
	static int GetID( const CString& url )
	{
		CString id = util::GetParamFromURL( url, L"id" );
		return _wtoi(id);
	}

	/**
	 * �R�����g���̎擾�B
	 *
	 * http://mixi.jp/view_bbs.pl?id=xxx&comment_count=yyy&comm_id=zzz
	 */
	static int GetCommentCount(LPCTSTR url)
	{
		// "comment_count="��"&"�Ɉ͂܂ꂽ������𐔒l�ϊ��������́B
		CString strCommentCount = util::GetParamFromURL( url, L"comment_count" );
		if (strCommentCount.IsEmpty()) {
			// not found.
			return -1;
		}

		return _wtoi(strCommentCount);
	}
};

}