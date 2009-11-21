/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
/**
 * MZ3依存ユーティリティ関数群
 * 
 * 主に theApp に依存するもの。
 */

#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"

/// MZ3 用ユーティリティ
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

/// URL から画像ファイルのパスを生成する
CString MakeImageLogfilePathFromUrl( const CString& url );


/// URL から画像ファイルのパスをMD5で生成する
CString MakeImageLogfilePathFromUrlMD5( const CString& url );

POINT GetPopupPosForSoftKeyMenu1();
int GetPopupFlagsForSoftKeyMenu1();

POINT GetPopupPosForSoftKeyMenu2();
int GetPopupFlagsForSoftKeyMenu2();

/**
 * 画面下部の情報領域にメッセージを設定する
 */
inline void MySetInformationText( HWND hWnd, LPCTSTR szMessage )
{
	CString text;
	if( theApp.m_inet.IsConnecting() ) {
		// 通信中なので、アクセス種別を頭に付ける
		text = theApp.m_accessTypeInfo.getShortText(theApp.m_accessType);

		// リダイレクト回数が１回以上なら、それも表示しておく
		int nRedirect = theApp.m_inet.GetRedirectCount();
		if( nRedirect > 0 ) {
			CString s;
			s.Format( L"[%d]", nRedirect );
			text += s;
		}

		text += L"：";
		text += szMessage;
	}else{
		text = szMessage;
	}
	::SendMessage( hWnd, WM_MZ3_ACCESS_INFORMATION, NULL, (LPARAM)&text );
}

/**
 * data から表示カラム種別に応じた文字列を取得する
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
		// 本文を1行に変換して割り当て。
		item = data->GetBody();
		while( item.Replace( L"\r\n", L"" ) );
		break;
	default:
		return L"";
	}

	// 上限設定
	if (bLimitForList) {
#ifdef WINCE
		// WindowsMobile の場合は、30文字くらいで切らないと落ちるので制限する。
		return item.Left( 30 );
#else
		// Windows の場合は、とりあえず100文字で切っておく。
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
 * Twitter 用アクセス種別かどうかを返す
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
 * mixi 用アクセス種別かどうかを返す
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
 * 指定されたURLが mixi の URL であれば、
 * mixi モバイルの自動ログイン用URLに変換する
 */
/*
inline CString ConvertToMixiMobileAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ が含まれているので mixi の URL とみなす。
		// mixi モバイル用URLに変換したURLを返す。
		return theApp.MakeLoginUrlForMixiMobile( path );
	}

	// mixi 用URLではないため、変換せずに返す。
	return url;
}
*/

/**
 * 指定されたURLが mixi の URL であれば、
 * mixi の自動ログイン用URLに変換する
 */
/*
inline CString ConvertToMixiAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ が含まれているので mixi の URL とみなす。
		// mixi の自動ログイン用URLに変換したURLを返す。
		return theApp.MakeLoginUrl( path );
	}

	// mixi 用URLではないため、変換せずに返す。
	return url;
}
*/

/**
 * 指定されたURLをブラウザで開く（確認画面付き）
 */
inline void OpenUrlByBrowserWithConfirm( LPCTSTR url )
{
	if( theApp.m_optionMng.m_bConfirmOpenURL) {
		// 確認画面
		CString msg;
		msg.Format( 
			L"下記のURLをブラウザで開きます。\n\n"
			L"%s\n\n"
			L"よろしいですか？", url );
		if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
		{
			// YES ボタン以外なので終了
			return;
		}
	}

	// 自動ログイン変換
	CString requestUrl = url;

	// WindowsMobile 版のみ自動ログインURL変換を行う
/*
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif
*/
	// ブラウザで開く
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * 指定されたユーザのプロフィールページをブラウザで開く（確認画面付き）
 */
inline void OpenUrlByBrowserWithConfirmForUser( LPCTSTR url, LPCTSTR szUserName )
{
	// 確認画面
	CString msg;
	msg.Format( 
		L"%s さんのプロフィールページをブラウザで開きます。\n\n"
		L"%s\n\n"
		L"よろしいですか？", szUserName, url );
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES ボタン以外なので終了
		return;
	}

	// 自動ログイン変換
	CString requestUrl = url;

	// WindowsMobile 版のみ自動ログインURL変換を行う
/*
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi モバイル用自動ログインURL変換
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi 用自動ログインURL変換
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif
*/
	// ブラウザで開く
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * 未コンパイルであればコンパイルする。
 *
 * コンパイル失敗時はエラーログを出力する
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

// 引用ユーザリストがなければ作成する
inline void SetTwitterQuoteUsersWhenNotGenerated(MZ3Data* pSelectedData)
{
	if (pSelectedData->GetIntValue(L"quote_users_gened", 0)==0) {

		// 引用ユーザリストの作成
		CString target = pSelectedData->GetBody();
		if (target.Find(L"@")!=-1) {
			// 正規表現のコンパイル（一回のみ）
			static MyRegex reg;
			util::CompileRegex(reg, L"@([0-9a-zA-Z_]+)");

			for (int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++) {
				std::vector<MyRegex::Result>* pResults = NULL;
				if (reg.exec(target) == false || reg.results.size() != 2) {
					// 未発見
					break;
				}

				pSelectedData->AddTextArray(L"quote_users", reg.results[1].str.c_str());

				// ターゲットを更新。
				target.Delete(0, reg.results[0].end);
			}
		}
		pSelectedData->SetIntValue(L"quote_users_gened", 1);
	}
}

}
