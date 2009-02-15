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
bool CallMZ3ScriptHookFunction(const char* szSerializeKey, const char* szEventName, const char* szFuncName, void* pUserData)
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
	lua_pushlightuserdata(L, pUserData);

	// �֐����s
	int n_arg = 3;
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
bool CallMZ3ScriptHookFunction(const char* szSerializeKey, const char* szEventName, void* pUserData)
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

		if (CallMZ3ScriptHookFunction(szSerializeKey, szEventName, hookFuncNames[i].c_str(), pUserData)) {
			rval = true;
			break;
		}
	}
	return rval;
}

}