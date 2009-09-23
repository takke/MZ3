/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MZ3.h"
#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"
#include "util_mz3.h"
#include "util_gui.h"
#include "MainFrm.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

/**
 * ���[���ɏ]���� data ���當����𒊏o����
 *
 * @param data    �f�[�^
 * @param strRule ���[���B��F"urlparam:page"
 * @return strRule �ɏ]���� data ���璊�o����������
 */
inline CString LogfileRuleToText( const CMixiData& data, const CString& strRule )
{
	LPCTSTR rule;
	
	// ���[���F"urlparam:�p�����[�^��"
	rule = L"urlparam:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// �p�����[�^���擾
		CString param = strRule.Mid(wcslen(rule));

		// URL ���� param �p�����[�^���擾���A���̕������Ԃ��B
		return GetParamFromURL( data.GetURL(), param );
	}
	
	// ���[���F"urlafter:TargetURL[:default_path]"
	rule = L"urlafter:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// TargetURL �擾
		CString targetURL = strRule.Mid(wcslen(rule));

		// default_path ������Ύ擾
		CString defaultPath = L"";
		int defaultPathIdx = targetURL.Find(':');
		if (defaultPathIdx>0) {
			defaultPath = targetURL.Mid( defaultPathIdx+1 );	// ':' �ȍ~
			targetURL = targetURL.Left( defaultPathIdx );		// ':' �ȑO
		}

		// URL ���� param �p�����[�^���擾���A���̕������Ԃ��B
		CString after;
		if (util::GetAfterSubString( data.GetURL(), targetURL, after )<0) {
			return defaultPath;
		} else {
			return after;
		}
	}

	// ���[���Ƀ}�b�`���Ȃ������̂ŋ󕶎����Ԃ��B
	return L"";
}

/**
 * ���[���ɏ]���ă��O�t�@�C���p�X������𐶐�����
 */
inline CString MakeLogfilePathByRule( CString strBasePath, const CMixiData& data, LPCTSTR strLogfilePathRule )
{
//	MZ3_TRACE(L"��MakeLogfilePathByRule(), base_path[%s], rule[%s], url[%s]\n", strBasePath, strLogfilePathRule, data.GetURL());

	CString rule = strLogfilePathRule;
	CString path = strBasePath;
	path += L"\\";

	// rule ���
	// "{...}" ������΁A���[���ɏ]���Ēu������B
	for (;;) {
		int pos1 = rule.Find( '{' );
		if (pos1==-1) {
			path += rule;
			break;
		}

		int pos2 = rule.Find( '}', pos1+1 );
		if (pos2==-1) {
			// ex: "{x.html"
			path += rule;
			break;
		}

		// '{' ���O�̕�������o��
		if (pos1>0) {
			path += rule.Left(pos1);
		}

		// ����������̎擾
		CString subrule = rule.Mid( pos1+1, pos2-pos1-1 );

		// ���[�����
		path += LogfileRuleToText( data, subrule );

		// '}' �ȑO�̕�������폜
		rule.Delete(0, pos2+1);
	}

	// path ���� '/' �� '_' �ɒu������
	path.Replace( L"/", L"_" );
	// path ���� '?' �� '_' �ɒu������
	path.Replace( L"?", L"_" );
	// path ���� '{', '}' �� '_' �ɒu������
	path.Replace( L"{", L"_" );
	path.Replace( L"}", L"_" );

//	MZ3_TRACE( L"MakeLogfilePathByRule()\n" );
//	MZ3_TRACE( L" URL  : /%s/\n", data.GetURL() );
//	MZ3_TRACE( L" rule : /%s/\n", strLogfilePathRule );
//	MZ3_TRACE( L" path : /%s/\n", path );

	// �f�B���N�g�����Ȃ���ΐ�������
	{
		int start = strBasePath.GetLength()+1;	// strBasePath �͏�ɑ��݂���Ɖ��肷��
		for (;;) {
			int idx = path.Find( '\\', start );
			if (idx==-1) {
				break;
			}

			CString dirpath = path.Left(idx);
//			MZ3_TRACE( L" CreateDirectory : /%s/\n", dirpath );
			CreateDirectory( dirpath, NULL/*always null*/ );

			start = idx+1;
		}
	}

	return path;
}

/**
 * CMixiData �ɑΉ����郍�O�t�@�C���̃p�X�𐶐�����
 */
CString MakeLogfilePath( const CMixiData& data )
{
	// �A�N�Z�X��ʂɉ����ăp�X�𐶐�����
	CString strCacheFilePattern = theApp.m_accessTypeInfo.getCacheFilePattern( data.GetAccessType() );
	if (strCacheFilePattern.IsEmpty()) {
		// ���w��Ȃ̂ŃL���b�V���ۑ����Ȃ��B
		return L"";
	} else {
		// ���[����͂������ʂ�Ԃ��B
		return MakeLogfilePathByRule( theApp.m_filepath.logFolder, data, strCacheFilePattern );
	}
}

CString MakeImageLogfilePathFromUrl( const CString& url )
{
	CString filename = ExtractFilenameFromUrl( url, L"" );
	if (!filename.IsEmpty()) {
		return theApp.m_filepath.imageFolder + L"\\" + filename;
	}
	return L"";
}

CString MakeImageLogfilePathFromUrlMD5( const CString& url )
{
	// http://takke.jp/hoge/fuga.png => (md5) => xxxx...xxx
	if (url.IsEmpty()) {
		return L"";
	} else {
		unsigned char ansi_string[1024];
		memset( &ansi_string[0], 0x00, sizeof(char) * 1024 );
		unsigned int len_in_mbs = wcstombs( (char*)&ansi_string[0], url, 1023 );

		MD5 md5(ansi_string, len_in_mbs);
		char* pMD5hexdigest = md5.hex_digest();
		if ( pMD5hexdigest ) {
//			MZ3_TRACE(L"MakeImageLogfilePathFromUrlMD5(), url[%s], filename[%s]\n", 
//				(LPCTSTR)url, (LPCTSTR)CString(CStringA(pMD5hexdigest)));
			CString LogfilePath = theApp.m_filepath.imageFolder + L"\\" + CString(CStringA(pMD5hexdigest));
			delete pMD5hexdigest;
			return LogfilePath;
		} else {
			return L"";
		}
	}
}

POINT GetPopupPosForSoftKeyMenu2()
{
	POINT pt;

#ifdef WINCE
	// MZ3 : ��ʂ̉E���Ń|�b�v�A�b�v����
	// �������A���j���[�̍�����������ɕ\������

	CRect rectMenuBar;
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	GetWindowRect(pMainFrame->m_hwndMenuBar, &rectMenuBar);

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	pt.x = rect.right;
	pt.y = rect.bottom - rectMenuBar.Height();
	return pt;
#else
	// MZ4 : �}�E�X�̈ʒu�Ń|�b�v�A�b�v����
	return GetPopupPos();
#endif
}

int GetPopupFlagsForSoftKeyMenu2()
{
#ifdef WINCE
	// MZ3 : ��ʂ̉E���Ń|�b�v�A�b�v����
	return TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
#else
	// MZ4 : �}�E�X�̈ʒu�Ń|�b�v�A�b�v����
	// �}�E�X�ʒu������ɂ��ĕ\���A�E�{�^����L���ɂ���
	return GetPopupFlags();
#endif
}

/**
 * MZ3 Script : �C�x���g�n���h���̌Ăяo��
 */
bool CallMZ3ScriptHookFunction(const char* szSerializeKey, const char* szEventName, const char* szFuncName, void* pUserData1, void* pUserData2)
{
	CStringA strHookFuncName(szFuncName);

	// �p�[�T�����e�[�u���Ɗ֐����ɕ�������
	int idx = strHookFuncName.Find('.');
	if (idx<=0) {
		// �֐����s��
		MZ3LOGGER_ERROR(util::FormatString(L"�t�b�N�֐������s���ł� : [%s], [%s]", 
			CString(strHookFuncName), CString(szEventName)));
		return false;
	}

	CStringA strTable    = strHookFuncName.Left(idx);
	CStringA strFuncName = strHookFuncName.Mid(idx+1);


	// �X�^�b�N�̃T�C�Y���o���Ă���
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua�֐���("table.name")��ς�
	lua_getglobal(L, strTable);				// �e�[�u�������X�^�b�N�ɐς�
	lua_pushstring(L, strFuncName);			// �Ώەϐ�(�֐���)���e�[�u���ɐς�
	lua_gettable(L, -2);					// �X�^�b�N��2�Ԗڂ̗v�f(�e�[�u��)����A
											// �e�[�u���g�b�v�̕�����(strFuncName)�Ŏ�����郁���o��
											// �X�^�b�N�ɐς�

	// ������ς�
	lua_pushstring(L, szSerializeKey);
	lua_pushstring(L, szEventName);
	lua_pushlightuserdata(L, pUserData1);
	lua_pushlightuserdata(L, pUserData2);

	// �֐����s
	int n_arg = 4;
	int n_ret = 1;
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO �G���[����
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// �Ԃ�l�擾
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, top);
	return result!=0;
}

/**
 * MZ3 Script : �t�b�N�֐��̌Ăяo��
 */
bool CallMZ3ScriptHookFunctions(const char* szSerializeKey, const char* szEventName, void* pUserData1, void* pUserData2)
{
	if (theApp.m_luaHooks.count((const char*)szEventName)==0) {
		// �t�b�N�֐����o�^�̂��ߏI��
		return false;
	}

	const std::vector<std::string>& hookFuncNames = theApp.m_luaHooks[(const char*)szEventName];

	bool rval = false;
	for (int i=(int)hookFuncNames.size()-1; i>=0; i--) {
//		MZ3LOGGER_DEBUG(util::FormatString(L"call %s on %s", 
//							CString(hookFuncNames[i].c_str()),
//							CString(szEventName)));

		if (CallMZ3ScriptHookFunction(szSerializeKey, szEventName, hookFuncNames[i].c_str(), pUserData1, pUserData2)) {
			rval = true;
			break;
		}
	}
	return rval;
}

/**
 * MZ3 Script : �C�x���g�n���h���̌Ăяo��2
 */
bool CallMZ3ScriptHookFunction2(const char* szEventName, const char* szFuncName, 
								MyLuaDataList* pRetValList,
								const MyLuaData& data1, 
								const MyLuaData& data2, 
								const MyLuaData& data3, 
								const MyLuaData& data4,
								const MyLuaData& data5
								)
{
	CStringA strHookFuncName(szFuncName);

	// �p�[�T�����e�[�u���Ɗ֐����ɕ�������
	int idx = strHookFuncName.Find('.');
	if (idx<=0) {
		// �֐����s��
		MZ3LOGGER_ERROR(util::FormatString(L"�t�b�N�֐������s���ł� : [%s], [%s]", 
			CString(strHookFuncName), CString(szEventName)));
		return false;
	}

	CStringA strTable    = strHookFuncName.Left(idx);
	CStringA strFuncName = strHookFuncName.Mid(idx+1);


	// �X�^�b�N�̃T�C�Y���o���Ă���
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua�֐���("table.name")��ς�
	lua_getglobal(L, strTable);				// �e�[�u�������X�^�b�N�ɐς�
	lua_pushstring(L, strFuncName);			// �Ώەϐ�(�֐���)���e�[�u���ɐς�
	lua_gettable(L, -2);					// �X�^�b�N��2�Ԗڂ̗v�f(�e�[�u��)����A
											// �e�[�u���g�b�v�̕�����(strFuncName)�Ŏ�����郁���o��
											// �X�^�b�N�ɐς�

	// ������ς�
	int n_arg = 1;
	lua_pushstring(L, szEventName);

	MyLuaDataPtr data_args[] = {&data1, &data2, &data3, &data4, &data5, NULL};
	for (int i=0; data_args[i]!=NULL; i++) {
		MyLuaDataPtr pData = data_args[i];
		switch (pData->m_type) {
		case MyLuaData::MyLuaDataType_String:
			lua_pushstring(L, pData->m_strText);
			n_arg ++;
			break;
		case MyLuaData::MyLuaDataType_Integer:
			lua_pushnumber(L, pData->m_number);
			n_arg ++;
			break;
		case MyLuaData::MyLuaDataType_UserData:
			lua_pushlightuserdata(L, pData->m_pUserData);
			n_arg ++;
			break;
		default:
			// skip
			break;
		}
	}

	// �֐����s
	int n_ret = 1 + pRetValList->size();
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO �G���[����
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// �Ԃ�l�擾
		//
		// a, b, c = lua_func();
		// a = -3;
		// b = -2;
		// c = -1;
		//

		// result = a
		result = lua_toboolean(L, -n_ret);

		for (u_int i=0; i<pRetValList->size(); i++) {
			MyLuaData& rdata = (*pRetValList)[i];
			int idx = -n_ret + i + 1;

			switch (rdata.m_type) {
			case MyLuaData::MyLuaDataType_String:
				rdata.m_strText = lua_tostring(L, idx);
				break;
			case MyLuaData::MyLuaDataType_Integer:
				rdata.m_number = lua_tointeger(L, idx);
				break;
			case MyLuaData::MyLuaDataType_UserData:
				rdata.m_pUserData = lua_touserdata(L, idx);
				break;
			default:
				// skip
				break;
			}
		}
	}
	lua_settop(L, top);
	return result!=0;
}


/**
 * MZ3 Script : �t�b�N�֐��̌Ăяo��2
 */
bool CallMZ3ScriptHookFunctions2(const char* szEventName, 
								 MyLuaDataList* pRetValList,
								 const MyLuaData& data1, 
								 const MyLuaData& data2, 
								 const MyLuaData& data3, 
								 const MyLuaData& data4, 
								 const MyLuaData& data5
								 )
{
	if (theApp.m_luaHooks.count((const char*)szEventName)==0) {
		// �t�b�N�֐����o�^�̂��ߏI��
		MZ3LOGGER_DEBUG(util::FormatString(L"no listeners for %s", 
							CString(szEventName)));
		return false;
	}

	const std::vector<std::string>& hookFuncNames = theApp.m_luaHooks[(const char*)szEventName];

	bool rval = false;
	for (int i=(int)hookFuncNames.size()-1; i>=0; i--) {
//		MZ3LOGGER_DEBUG(util::FormatString(L"call %s on %s", 
//							CString(hookFuncNames[i].c_str()),
//							CString(szEventName)));

		if (CallMZ3ScriptHookFunction2(szEventName, 
									   hookFuncNames[i].c_str(), 
									   pRetValList, 
									   data1, data2, data3, data4, data5))
		{
			rval = true;
			break;
		}
	}
	return rval;
}

/**
 * URL ����A�N�Z�X��ʂ𐄒肷��
 */
ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url )
{
	// TODO Lua �ōĎ������邱�Ɓ�
	// view �n
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // ���C��
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // ���L���e
	if( url.Find( L"neighbor_diary.pl" ) != -1 ){ return ACCESS_NEIGHBORDIARY;} // ���L���e(���̓��L�A�O�̓��L)
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // �R�~���j�e�B���e
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // �A���P�[�g
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // �C�x���g
	if( url.Find( L"list_event_member.pl" ) != -1 ) { return ACCESS_EVENT_MEMBER; } // �C�x���g�Q���҈ꗗ
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // �����̓��L���e
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // ���b�Z�[�W
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // �j���[�X���e
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // �l�y�[�W
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // �R�~���j�e�B�y�[�W

	// list �n
	if( url.Find( L"list_bookmark.pl?kind=community" ) != -1 ) { return ACCESS_LIST_FAVORITE_COMMUNITY; }
	if( url.Find( L"list_bookmark.pl" ) != -1 ) { return ACCESS_LIST_FAVORITE_USER; }
	if( url.Find( L"new_bbs.pl" ) != -1 )		{ return ACCESS_LIST_NEW_BBS; }

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData((int)ACCESS_INVALID));
	if (util::CallMZ3ScriptHookFunctions2("estimate_access_type_by_url", &rvals, 
			util::MyLuaData(CStringA(url)))) {
		int access_type_by_lua = rvals[0].m_number;
		MZ3LOGGER_DEBUG(util::FormatString(L"estimated access type by lua : %d", access_type_by_lua));
		return (ACCESS_TYPE)access_type_by_lua;
	}
	
	// �s���Ȃ̂� INVALID �Ƃ���
	return ACCESS_INVALID;
}

}