/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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

class MyLuaData
{
public:
	enum MyLuaDataType
	{
		MyLuaDataType_Invalid  = -1,
		MyLuaDataType_String   = 0,
		MyLuaDataType_Integer  = 1,
		MyLuaDataType_UserData = 2,
	};

	MyLuaDataType	m_type;
	CStringA		m_strText;
	int				m_number;
	void*			m_pUserData;

	MyLuaData()
		: m_type(MyLuaDataType_Invalid)
		, m_strText("")
		, m_number(0)
		, m_pUserData(NULL)
	{
	}

	MyLuaData(const char* szText)
		: m_type(MyLuaDataType_String)
		, m_strText(szText)
		, m_number(0)
		, m_pUserData(NULL)
	{
	}

	MyLuaData(int number)
		: m_type(MyLuaDataType_Integer)
		, m_strText("")
		, m_number(number)
		, m_pUserData(NULL)
	{
	}

	MyLuaData(void* pUserData)
		: m_type(MyLuaDataType_UserData)
		, m_strText("")
		, m_number(0)
		, m_pUserData(pUserData)
	{
	}
};
typedef const MyLuaData* MyLuaDataPtr;
typedef std::vector<MyLuaData> MyLuaDataList;

CString MakeLogfilePath( const CMixiData& data );
CString MakeImageLogfilePath( CMixiData& data );

/// URL ����摜�t�@�C���̃p�X�𐶐�����
CString MakeImageLogfilePathFromUrl( const CString& url );


/// URL ����摜�t�@�C���̃p�X��MD5�Ő�������
CString MakeImageLogfilePathFromUrlMD5( const CString& url );

POINT GetPopupPosForSoftKeyMenu1();
int GetPopupFlagsForSoftKeyMenu1();

POINT GetPopupPosForSoftKeyMenu2();
int GetPopupFlagsForSoftKeyMenu2();

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
inline void OpenUrlByBrowserWithConfirm( LPCTSTR url )
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
/*
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif
*/
	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * �w�肳�ꂽ���[�U�̃v���t�B�[���y�[�W���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenUrlByBrowserWithConfirmForUser( LPCTSTR url, LPCTSTR szUserName )
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
/*
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif
*/
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

bool CallMZ3ScriptHookFunction(const char* szSerializeKey, const char* szEventName, const char* szFuncName, void* pUserData1, void* pUserData2=NULL);
bool CallMZ3ScriptHookFunctions(const char* szSerializeKey, 
								const char* szEventName, 
								void* pUserData1, 
								void* pUserData2=NULL);
bool CallMZ3ScriptHookFunctions2(const char* szEventName, 
								MyLuaDataList* pRetValList,
								const MyLuaData& pData1, 
								const MyLuaData& pData2=MyLuaData(), 
								const MyLuaData& pData3=MyLuaData(), 
								const MyLuaData& pData4=MyLuaData(),
								const MyLuaData& pData5=MyLuaData()
								);

// ���p���[�U���X�g���Ȃ���΍쐬����
inline void SetTwitterQuoteUsersWhenNotGenerated(MZ3Data* pSelectedData)
{
	if (pSelectedData->GetIntValue(L"quote_users_gened", 0)==0) {

		// ���p���[�U���X�g�̍쐬
		CString target = pSelectedData->GetBody();
		if (target.Find(L"@")!=-1) {
			// ���K�\���̃R���p�C���i���̂݁j
			static MyRegex reg;
			util::CompileRegex(reg, L"@([0-9a-zA-Z_]+)");

			for (int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++) {
				std::vector<MyRegex::Result>* pResults = NULL;
				if (reg.exec(target) == false || reg.results.size() != 2) {
					// ������
					break;
				}

				pSelectedData->AddTextArray(L"quote_users", reg.results[1].str.c_str());

				// �^�[�Q�b�g���X�V�B
				target.Delete(0, reg.results[0].end);
			}
		}
		pSelectedData->SetIntValue(L"quote_users_gened", 1);
	}
}

}
