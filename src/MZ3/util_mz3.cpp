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

/// MZ3 用ユーティリティ
namespace util
{

/**
 * ルールに従って data から文字列を抽出する
 *
 * @param data    データ
 * @param strRule ルール。例："urlparam:page"
 * @return strRule に従って data から抽出した文字列
 */
inline CString LogfileRuleToText( const CMixiData& data, const CString& strRule )
{
	LPCTSTR rule;
	
	// ルール："urlparam:パラメータ名"
	rule = L"urlparam:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// パラメータ名取得
		CString param = strRule.Mid(wcslen(rule));

		// URL から param パラメータを取得し、その文字列を返す。
		return GetParamFromURL( data.GetURL(), param );
	}
	
	// ルール："urlafter:TargetURL[:default_path]"
	rule = L"urlafter:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// TargetURL 取得
		CString targetURL = strRule.Mid(wcslen(rule));

		// default_path があれば取得
		CString defaultPath = L"";
		int defaultPathIdx = targetURL.Find(':');
		if (defaultPathIdx>0) {
			defaultPath = targetURL.Mid( defaultPathIdx+1 );	// ':' 以降
			targetURL = targetURL.Left( defaultPathIdx );		// ':' 以前
		}

		// URL から param パラメータを取得し、その文字列を返す。
		CString after;
		if (util::GetAfterSubString( data.GetURL(), targetURL, after )<0) {
			return defaultPath;
		} else {
			return after;
		}
	}

	// ルールにマッチしなかったので空文字列を返す。
	return L"";
}

/**
 * ルールに従ってログファイルパス文字列を生成する
 */
inline CString MakeLogfilePathByRule( CString strBasePath, const CMixiData& data, LPCTSTR strLogfilePathRule )
{
//	MZ3_TRACE(L"★MakeLogfilePathByRule(), base_path[%s], rule[%s], url[%s]\n", strBasePath, strLogfilePathRule, data.GetURL());

	CString rule = strLogfilePathRule;
	CString path = strBasePath;
	path += L"\\";

	// rule 解析
	// "{...}" があれば、ルールに従って置換する。
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

		// '{' より前の文字列を出力
		if (pos1>0) {
			path += rule.Left(pos1);
		}

		// 部分文字列の取得
		CString subrule = rule.Mid( pos1+1, pos2-pos1-1 );

		// ルール解析
		path += LogfileRuleToText( data, subrule );

		// '}' 以前の文字列を削除
		rule.Delete(0, pos2+1);
	}

	// path 内の '/' は '_' に置換する
	path.Replace( L"/", L"_" );
	// path 内の '?' は '_' に置換する
	path.Replace( L"?", L"_" );
	// path 内の '{', '}' は '_' に置換する
	path.Replace( L"{", L"_" );
	path.Replace( L"}", L"_" );

//	MZ3_TRACE( L"MakeLogfilePathByRule()\n" );
//	MZ3_TRACE( L" URL  : /%s/\n", data.GetURL() );
//	MZ3_TRACE( L" rule : /%s/\n", strLogfilePathRule );
//	MZ3_TRACE( L" path : /%s/\n", path );

	// ディレクトリがなければ生成する
	{
		int start = strBasePath.GetLength()+1;	// strBasePath は常に存在すると仮定する
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
 * CMixiData に対応するログファイルのパスを生成する
 */
CString MakeLogfilePath( const CMixiData& data )
{
	// アクセス種別に応じてパスを生成する
	CString strCacheFilePattern = theApp.m_accessTypeInfo.getCacheFilePattern( data.GetAccessType() );
	if (strCacheFilePattern.IsEmpty()) {
		// 未指定なのでキャッシュ保存しない。
		return L"";
	} else {
		// ルール解析した結果を返す。
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
	// MZ3 : 画面の右下でポップアップする
	// ただし、メニューの高さ分だけ上に表示する

	CRect rectMenuBar;
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	GetWindowRect(pMainFrame->m_hwndMenuBar, &rectMenuBar);

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	pt.x = rect.right;
	pt.y = rect.bottom - rectMenuBar.Height();
	return pt;
#else
	// MZ4 : マウスの位置でポップアップする
	return GetPopupPos();
#endif
}

int GetPopupFlagsForSoftKeyMenu2()
{
#ifdef WINCE
	// MZ3 : 画面の右下でポップアップする
	return TPM_RIGHTALIGN | TPM_BOTTOMALIGN;
#else
	// MZ4 : マウスの位置でポップアップする
	// マウス位置を左上にして表示、右ボタンを有効にする
	return GetPopupFlags();
#endif
}

/**
 * MZ3 Script : イベントハンドラの呼び出し
 */
bool CallMZ3ScriptHookFunction(const char* szSerializeKey, const char* szEventName, const char* szFuncName, void* pUserData1, void* pUserData2)
{
	CStringA strHookFuncName(szFuncName);

	// パーサ名をテーブルと関数名に分離する
	int idx = strHookFuncName.Find('.');
	if (idx<=0) {
		// 関数名不正
		MZ3LOGGER_ERROR(util::FormatString(L"フック関数名が不正です : [%s], [%s]", 
			CString(strHookFuncName), CString(szEventName)));
		return false;
	}

	CStringA strTable    = strHookFuncName.Left(idx);
	CStringA strFuncName = strHookFuncName.Mid(idx+1);


	// スタックのサイズを覚えておく
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua関数名("table.name")を積む
	lua_getglobal(L, strTable);				// テーブル名をスタックに積む
	lua_pushstring(L, strFuncName);			// 対象変数(関数名)をテーブルに積む
	lua_gettable(L, -2);					// スタックの2番目の要素(テーブル)から、
											// テーブルトップの文字列(strFuncName)で示されるメンバを
											// スタックに積む

	// 引数を積む
	lua_pushstring(L, szSerializeKey);
	lua_pushstring(L, szEventName);
	lua_pushlightuserdata(L, pUserData1);
	lua_pushlightuserdata(L, pUserData2);

	// 関数実行
	int n_arg = 4;
	int n_ret = 1;
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO エラー処理
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// 返り値取得
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, top);
	return result!=0;
}

/**
 * MZ3 Script : フック関数の呼び出し
 */
bool CallMZ3ScriptHookFunctions(const char* szSerializeKey, const char* szEventName, void* pUserData1, void* pUserData2)
{
	if (theApp.m_luaHooks.count((const char*)szEventName)==0) {
		// フック関数未登録のため終了
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
 * MZ3 Script : イベントハンドラの呼び出し2
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

	// パーサ名をテーブルと関数名に分離する
	int idx = strHookFuncName.Find('.');
	if (idx<=0) {
		// 関数名不正
		MZ3LOGGER_ERROR(util::FormatString(L"フック関数名が不正です : [%s], [%s]", 
			CString(strHookFuncName), CString(szEventName)));
		return false;
	}

	CStringA strTable    = strHookFuncName.Left(idx);
	CStringA strFuncName = strHookFuncName.Mid(idx+1);


	// スタックのサイズを覚えておく
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua関数名("table.name")を積む
	lua_getglobal(L, strTable);				// テーブル名をスタックに積む
	lua_pushstring(L, strFuncName);			// 対象変数(関数名)をテーブルに積む
	lua_gettable(L, -2);					// スタックの2番目の要素(テーブル)から、
											// テーブルトップの文字列(strFuncName)で示されるメンバを
											// スタックに積む

	// 引数を積む
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

	// 関数実行
	int n_ret = 1 + pRetValList->size();
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO エラー処理
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// 返り値取得
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
 * MZ3 Script : フック関数の呼び出し2
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
		// フック関数未登録のため終了
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
 * URL からアクセス種別を推定する
 */
ACCESS_TYPE EstimateAccessTypeByUrl( const CString& url )
{
	// TODO Lua で再実装すること↓
	// view 系
	if( url.Find( L"home.pl" ) != -1 ) 			{ return ACCESS_MAIN;      } // メイン
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_DIARY;     } // 日記内容
	if( url.Find( L"neighbor_diary.pl" ) != -1 ){ return ACCESS_NEIGHBORDIARY;} // 日記内容(次の日記、前の日記)
	if( url.Find( L"view_bbs.pl" ) != -1 ) 		{ return ACCESS_BBS;       } // コミュニティ内容
	if( url.Find( L"view_enquete.pl" ) != -1 ) 	{ return ACCESS_ENQUETE;   } // アンケート
	if( url.Find( L"view_event.pl" ) != -1 ) 	{ return ACCESS_EVENT;     } // イベント
	if( url.Find( L"list_event_member.pl" ) != -1 ) { return ACCESS_EVENT_MEMBER; } // イベント参加者一覧
	if( url.Find( L"view_diary.pl" ) != -1 ) 	{ return ACCESS_MYDIARY;   } // 自分の日記内容
	if( url.Find( L"view_message.pl" ) != -1 ) 	{ return ACCESS_MESSAGE;   } // メッセージ
	if( url.Find( L"view_news.pl" ) != -1 ) 	{ return ACCESS_NEWS;      } // ニュース内容
	if( url.Find( L"show_friend.pl" ) != -1 ) 	{ return ACCESS_PROFILE;   } // 個人ページ
	if( url.Find( L"view_community.pl" ) != -1 ){ return ACCESS_COMMUNITY; } // コミュニティページ

	// list 系
	if( url.Find( L"list_bookmark.pl?kind=community" ) != -1 ) { return ACCESS_LIST_FAVORITE_COMMUNITY; }
	if( url.Find( L"list_bookmark.pl" ) != -1 ) { return ACCESS_LIST_FAVORITE_USER; }
	if( url.Find( L"new_bbs.pl" ) != -1 )		{ return ACCESS_LIST_NEW_BBS; }

	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData((int)ACCESS_INVALID));
	if (util::CallMZ3ScriptHookFunctions2("estimate_access_type_by_url", &rvals, 
			util::MyLuaData(CStringA(url)))) {
		int access_type_by_lua = rvals[0].m_number;
		MZ3LOGGER_DEBUG(util::FormatString(L"estimated access type by lua : %d", access_type_by_lua));
		return (ACCESS_TYPE)access_type_by_lua;
	}
	
	// 不明なので INVALID とする
	return ACCESS_INVALID;
}

}