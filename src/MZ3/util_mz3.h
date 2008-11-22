/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * MZ3�ˑ����[�e�B���e�B�֐��Q
 * 
 * ��� theApp �Ɉˑ�������́B
 */

#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

CString MakeLogfilePath( const CMixiData& data );

/// URL ����摜�t�@�C���̃p�X�𐶐�����
CString MakeImageLogfilePathFromUrl( const CString& url );


/// URL ����摜�t�@�C���̃p�X��MD5�Ő�������
CString MakeImageLogfilePathFromUrlMD5( const CString& url );

/**
 * ��ʉ����̏��̈�Ƀ��b�Z�[�W��ݒ肷��
 */
inline void MySetInformationText( HWND hWnd, LPCTSTR szMessage )
{
	CString text;
	if( theApp.m_inet.IsConnecting() ) {
		// �ʐM���Ȃ̂ŁA�A�N�Z�X��ʂ𓪂ɕt����
		text = theApp.m_accessTypeInfo.getShortText(theApp.m_accessType);

		// ���_�C���N�g�񐔂��P��ȏ�Ȃ�A������\�����Ă���
		int nRedirect = theApp.m_inet.GetRedirectCount();
		if( nRedirect > 0 ) {
			CString s;
			s.Format( L"[%d]", nRedirect );
			text += s;
		}

		text += L"�F";
		text += szMessage;
	}else{
		text = szMessage;
	}
	::SendMessage( hWnd, WM_MZ3_ACCESS_INFORMATION, NULL, (LPARAM)&text );
}

/**
 * data ����\���J������ʂɉ�������������擾����
 */
inline CString MyGetItemByBodyColType( const CMixiData* data, AccessTypeInfo::BODY_INDICATE_TYPE bodyColType, bool bLimitForList=true )
{
	CString item;

	switch( bodyColType ) {
	case AccessTypeInfo::BODY_INDICATE_TYPE_DATE:
		item = data->GetDate();
		break;
	case AccessTypeInfo::BODY_INDICATE_TYPE_NAME:
		item = data->GetName();
		break;
	case AccessTypeInfo::BODY_INDICATE_TYPE_TITLE:
		item = data->GetTitle();
		break;
	case AccessTypeInfo::BODY_INDICATE_TYPE_BODY:
		// �{����1�s�ɕϊ����Ċ��蓖�āB
		item = data->GetBody();
		while( item.Replace( L"\r\n", L"" ) );
		break;
	default:
		return L"";
	}

	// ����ݒ�
	if (bLimitForList) {
#ifdef WINCE
		// WindowsMobile �̏ꍇ�́A30�������炢�Ő؂�Ȃ��Ɨ�����̂Ő�������B
		return item.Left( 30 );
#else
		// Windows �̏ꍇ�́A�Ƃ肠����100�����Ő؂��Ă����B
		return item.Left( 100 );
#endif
	} else {
		return item;
	}
}

inline CString ExtractFilenameFromUrl( const CString& url, const CString& strDefault )
{
	int idx = url.ReverseFind( '/' );
	if (idx >= 0) {
		return url.Mid( idx+1 );
	}
	return strDefault;
}

/**
 * CMixiData �ɑΉ�����摜�t�@�C���̃p�X�𐶐�����
 *
 * �p�X�� "local_image_filepath" �Ƃ��ăL���b�V������B
 */
inline CString MakeImageLogfilePath( CMixiData& data )
{
	// �A�N�Z�X��ʂɉ����ăp�X�𐶐�����
	switch( data.GetAccessType() ) {
	case ACCESS_PROFILE:
	case ACCESS_COMMUNITY:
	case ACCESS_TWITTER_USER:
	case ACCESS_WASSR_USER:
	case ACCESS_MIXI_ECHO_USER:
		if (data.GetImageCount()>0) {
			const CString& image_url = data.GetImage(0);
			if (image_url.IsEmpty()) {
				return L"";
			}
			CString path = data.GetTextValue(L"local_image_filepath");
			if (path.IsEmpty()) {
				path = MakeImageLogfilePathFromUrlMD5( image_url );
				if (!path.IsEmpty()) {
					data.SetTextValue(L"local_image_filepath", path);
				}
			}
			if (!path.IsEmpty()) {
				return path;
			}
		}
		break;
	default:
		break;
	}
	return L"";
}

/**
 * Twitter �p�A�N�Z�X��ʂ��ǂ�����Ԃ�
 */
inline bool IsTwitterAccessType( ACCESS_TYPE type )
{
	if (theApp.m_accessTypeInfo.getServiceType(type) == "Twitter") {
		return true;
	} else {
		return false;
	}
}

/**
 * mixi �p�A�N�Z�X��ʂ��ǂ�����Ԃ�
 */
inline bool IsMixiAccessType( ACCESS_TYPE type )
{
	if (theApp.m_accessTypeInfo.getServiceType(type) == "mixi") {
		return true;
	} else {
		return false;
	}
}

/**
 * �w�肳�ꂽURL�� mixi �� URL �ł���΁A
 * mixi ���o�C���̎������O�C���pURL�ɕϊ�����
 */
/*
inline CString ConvertToMixiMobileAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ ���܂܂�Ă���̂� mixi �� URL �Ƃ݂Ȃ��B
		// mixi ���o�C���pURL�ɕϊ�����URL��Ԃ��B
		return theApp.MakeLoginUrlForMixiMobile( path );
	}

	// mixi �pURL�ł͂Ȃ����߁A�ϊ������ɕԂ��B
	return url;
}
*/

/**
 * �w�肳�ꂽURL�� mixi �� URL �ł���΁A
 * mixi �̎������O�C���pURL�ɕϊ�����
 */
/*
inline CString ConvertToMixiAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ ���܂܂�Ă���̂� mixi �� URL �Ƃ݂Ȃ��B
		// mixi �̎������O�C���pURL�ɕϊ�����URL��Ԃ��B
		return theApp.MakeLoginUrl( path );
	}

	// mixi �pURL�ł͂Ȃ����߁A�ϊ������ɕԂ��B
	return url;
}
*/

/**
 * �w�肳�ꂽURL���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUrl( LPCTSTR url )
{
	if( theApp.m_optionMng.m_bConfirmOpenURL) {
		// �m�F���
		CString msg;
		msg.Format( 
			L"���L��URL���u���E�U�ŊJ���܂��B\n\n"
			L"%s\n\n"
			L"��낵���ł����H", url );
		if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
		{
			// YES �{�^���ȊO�Ȃ̂ŏI��
			return;
		}
	}

	// �������O�C���ϊ�
	CString requestUrl = url;

	// WindowsMobile �ł̂ݎ������O�C��URL�ϊ����s��
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * �w�肳�ꂽ���[�U�̃v���t�B�[���y�[�W���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUser( LPCTSTR url, LPCTSTR szUserName )
{
	// �m�F���
	CString msg;
	msg.Format( 
		L"%s ����̃v���t�B�[���y�[�W���u���E�U�ŊJ���܂��B\n\n"
		L"%s\n\n"
		L"��낵���ł����H", szUserName, url );
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES �{�^���ȊO�Ȃ̂ŏI��
		return;
	}

	// �������O�C���ϊ�
	CString requestUrl = url;

	// WindowsMobile �ł̂ݎ������O�C��URL�ϊ����s��
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * ���R���p�C���ł���΃R���p�C������B
 *
 * �R���p�C�����s���̓G���[���O���o�͂���
 */
inline bool CompileRegex( MyRegex& reg, LPCTSTR szPattern )
{
	if( reg.isCompiled() ) {
		return true;
	} else {
		if(! reg.compile( szPattern ) ) {
			CString msg = FAILED_TO_COMPILE_REGEX_MSG;
			msg += L", pattern[";
			msg += szPattern;
			msg += L"]";
			MZ3LOGGER_FATAL( msg );
			return false;
		}
		return true;
	}
}

}
