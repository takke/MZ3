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

}