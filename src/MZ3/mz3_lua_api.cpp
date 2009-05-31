/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "MixiParserUtil.h"
#include "TwitterParser.h"
#include "IniFile.h"
#include "util_goo.h"
#include "url_encoder.h"

//-----------------------------------------------
// lua support
//-----------------------------------------------

#define MZ3_LUA_LOGGER_HEADER	L"(Lua) "

#define make_invalid_arg_error_string(X)	make_invalid_arg_error_string2(__FILE__, __LINE__, X)

static CStringA make_invalid_arg_error_string2(const char* file, int line, const char* func_name)
{
	CStringA s;
	s.Format("[%s:%d] invalid argument for '%s'...", file, line, func_name);
	return s;
}

/*
--- 関数名が * で終わるAPIは未実装
*/

//-----------------------------------------------
// MZ3 Core API
//-----------------------------------------------

/*
--- アプリケーション名("MZ3", "MZ4")を返す
--
-- @return [string] アプリケーション名
--
function mz3.get_app_name()
*/
int lua_mz3_get_app_name(lua_State *L)
{
	// 結果をスタックに積む
	lua_pushstring(L, CStringA(MZ3_APP_NAME));

	// 戻り値の数を返す
	return 1;
}

/*
--- サービス登録(タブの初期化用)
--
-- @param service_name     サービス名
-- @param default_selected デフォルト状態で選択済みにするかどうか
--
function mz3.regist_service(service_name, default_selected)
*/
int lua_mz3_regist_service(lua_State *L)
{
	// 引数の取得
	const char* name = lua_tostring(L, 1);
	bool selected = lua_toboolean(L, 2) != 0 ? true : false;

	// 登録
	theApp.m_luaServices.push_back(CMZ3App::Service(name, selected));

	// 戻り値の数を返す
	return 0;
}

/*
--- ログ出力(ERRORレベル)を行う。
--
-- @param msg 出力する文字列
--
-- @usage mz3.logger_error('text')
--
function mz3.logger_error(msg)
*/
int lua_mz3_logger_error(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_ERROR(s);

	return 0;
}

/*
--- ログ出力(INFOレベル)を行う。
--
-- @param msg 出力する文字列
--
function mz3.logger_info(msg)
*/
int lua_mz3_logger_info(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_INFO(s);

	return 0;
}

/*
--- ログ出力(DEBUGレベル)を行う。 
--
-- @param msg 出力する文字列
--
function mz3.logger_debug(msg)
*/
int lua_mz3_logger_debug(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_DEBUG(s);

	return 0;
}

/*
--- TRACE出力を行う。MZ3のコンソール付きバージョンのみで表示可能。 
--
-- @param msg 出力する文字列
--
function mz3.trace(msg)
*/
int lua_mz3_trace(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3_TRACE(s);

	return 0;
}

/*
--- Win32 API の GetTickCount() を呼び出す。
--
-- @return [integer] OS起動時からの経過時間(ms)
--
function mz3.get_tick_count()
*/
int lua_mz3_get_tick_count(lua_State *L)
{
	// 結果をスタックに積む
	lua_pushinteger(L, GetTickCount());

	// 戻り値の数を返す
	return 1;
}

/*
--- Win32 API の MessageBox() を呼び出す。
--
-- @param msg   メッセージ
-- @param title タイトル
--
function mz3.alert(msg, title)
*/
int lua_mz3_alert(lua_State *L)
{
	CString msg(lua_tostring(L, 1));		// 第1引数
	CString title(lua_tostring(L, 2));		// 第2引数

	MessageBox(GetActiveWindow(), msg, title, MB_OK);

	// 戻り値の数を返す
	return 0;
}

/*
--- 確認画面
--
-- @param msg   メッセージ
-- @param title タイトル
-- @param type  "yes_no", "yes_no_cancel"
-- @return "yes", "no", "cancel"
--
function mz3.confirm(msg, title, type)
*/
int lua_mz3_confirm(lua_State *L)
{
	const char* func_name = "mz3.confirm";
	CString msg(lua_tostring(L, 1));		// 第1引数
	CString title(lua_tostring(L, 2));		// 第2引数
	CStringA type(lua_tostring(L, 3));		// 第3引数

	UINT uType = MB_YESNO;
	if (type=="yes_no") {
		uType = MB_YESNO;
	} else if (type=="yes_no_cancel") {
		uType = MB_YESNOCANCEL;
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	int rval = MessageBox(GetActiveWindow(), msg, title, uType);
	switch (rval) {
	case IDYES:		lua_pushstring(L, "yes");		break;
	case IDNO:		lua_pushstring(L, "no");		break;
	case IDCANCEL:	lua_pushstring(L, "cancel");	break;
	default:		lua_pushstring(L, "");			break;
	}

	// 戻り値の数を返す
	return 1;
}

/*
--- HTMLエンティティをデコードした文字列を返す。 
--
--
function mz3.decode_html_entity(text)
*/
int lua_mz3_decode_html_entity(lua_State *L)
{
	CString value(lua_tostring(L, 1));			// 第1引数

	// 変換
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter(value);

	// 結果をスタックに戻す
	lua_pushstring(L, CStringA(value));

	// 戻り値の数を返す
	return 1;
}

/*
--- URL エンコードした文字列を返す
--
-- @param text     対象文字列
-- @param encoding "utf8"
--
function mz3.url_encode(text, encoding)
*/
int lua_mz3_url_encode(lua_State *L)
{
	const char* func_name = "mz3.url_encode";

	CString text(lua_tostring(L, 1));			// 第1引数
	CStringA encoding(lua_tostring(L, 2));		// 第2引数

	// 変換
	CString url_encoded_text;
	if (encoding=="utf8") {
		url_encoded_text = URLEncoder::encode_utf8(text);
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 結果をスタックに戻す
	lua_pushstring(L, CStringA(url_encoded_text));

	// 戻り値の数を返す
	return 1;
}

/*
--- アクセス種別からシリアライズキーを取得する。
--
-- @param type [integer]アクセス種別
-- @return [string] シリアライズキー
--
function mz3.get_serialize_key_by_access_type(type)
*/
int lua_mz3_get_serialize_key_by_access_type(lua_State *L)
{
	ACCESS_TYPE type = (ACCESS_TYPE)lua_tointeger(L, 1);	// 第1引数

	// 変換
	const char* serialize_key = theApp.m_accessTypeInfo.getSerializeKey(type);
	
	// 結果をスタックに戻す
	lua_pushstring(L, serialize_key);

	// 戻り値の数を返す
	return 1;
}

/*
--- URLから類推されるアクセス種別を取得する。 
--
-- @param url [string] URL
-- @return [integer] アクセス種別
--
function mz3.estimate_access_type_by_url(url)
*/
int lua_mz3_estimate_access_type_by_url(lua_State *L)
{
	CString url(lua_tostring(L, 1));			// 第1引数

	// 変換
	ACCESS_TYPE type = util::EstimateAccessTypeByUrl(url);

	// 結果をスタックに戻す
	lua_pushinteger(L, (int)type);

	// 戻り値の数を返す
	return 1;
}

/*
--- シリアライズキーからアクセス種別を取得する。
--
-- @param key シリアライズキー
-- @return [integer] アクセス種別
--
function mz3.get_access_type_by_key(key)
*/
int lua_mz3_get_access_type_by_key(lua_State *L)
{
	const char* key = lua_tostring(L, 1);			// 第1引数

	// 変換
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);

	// 結果をスタックに戻す
	lua_pushinteger(L, (int)type);

	// 戻り値の数を返す
	return 1;
}

/*
--- シリアライズキーからサービス種別を取得する。
--
-- @param key シリアライズキー
-- @return [string] サービス種別
--
function mz3.get_service_type(key)
*/
int lua_mz3_get_service_type(lua_State *L)
{
	const char* key = lua_tostring(L, 1);			// 第1引数

	// 変換
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);
	CStringA service_type = theApp.m_accessTypeInfo.getServiceType(type).c_str();

	// 結果をスタックに戻す
	lua_pushstring(L, service_type);

	// 戻り値の数を返す
	return 1;
}

/*
--- パーサを指定する。
--
-- @param key    シリアライズキー
-- @param parser パーサ名({テーブル}.{関数名})
-- @return なし
--
function mz3.set_parser(key, parser)
*/
int lua_mz3_set_parser(lua_State *L)
{
	const char* szKey = lua_tostring(L, 1);			// 第1引数:シリアライズキー
	const char* szParserName = lua_tostring(L, 2);	// 第2引数:パーサ名

	theApp.m_luaParsers[ szKey ] = szParserName;

//	MZ3LOGGER_DEBUG(util::FormatString(L"Registered new parser [%s] for [%s].", 
//						CString(szParserName), CString(szType)));

	// 戻り値の数を返す
	return 0;
}

/*
--- 各種イベントに対するフック関数を追加する。
--
-- call_first が true の場合、
-- 同一イベントに対して複数のフック関数が登録されている場合、最後に登録された関数から順に呼び出す。
-- false の場合は逆順に呼び出す。
--
-- @param event         イベント名
-- @param event_handler フック関数名({テーブル}.{関数名})
-- @param call_first    コール順序(未指定時はtrueとみなす)
--
-- @see event_listener1
-- @see event_listener2
--
-- @rerutn なし
--
function mz3.add_event_listener(event, event_handler, call_first)

--- フック関数(タイプ1)
--
-- @param serialize_key シリアライズキー
-- @param event_name    イベント名
-- @param data          データ(イベント毎に内容は異なる)
-- @return [bool] 次のフック関数またはデフォルト動作をさせる場合は false, それ以外は true。
--
function event_listener1(serialize_key, event_name, data)

--- フック関数(タイプ2)
--
-- @param event_name    イベント名
-- @param text          文字列
-- @param data          データ(イベント毎に内容は異なる)
-- @return [bool, integer] 次のフック関数またはデフォルト動作をさせる場合は false, それ以外は true。true 時は integer を返却可能。
--
function event_listener2(event_name, text, data)
*/
int lua_mz3_add_event_listener(lua_State *L)
{
	const char* szEvent = lua_tostring(L, 1);		// 第1引数:イベント
	const char* szParserName = lua_tostring(L, 2);	// 第2引数:パーサ名

	bool call_first = true;	// デフォルトは true
	if (!lua_isnil(L, 3)) {
		call_first = lua_toboolean(L, 3) != 0 ? true : false;
	}

	if (theApp.m_luaHooks.count(szEvent)==0) {
		theApp.m_luaHooks[ szEvent ] = std::vector<std::string>();
	}

	if (call_first) {
		theApp.m_luaHooks[ szEvent ].push_back(szParserName);
	} else {
		theApp.m_luaHooks[ szEvent ].insert(theApp.m_luaHooks[ szEvent ].begin(), szParserName);
	}

	// 戻り値の数を返す
	return 0;
}

/*
--- URL を開く
--
-- MZ3 の通信処理を開始する
--
-- @param wnd		  ウィンドウ(mz3_main_view.get_wnd() 等で取得した値)
-- @param access_type アクセス種別(アクセス種別に応じて取得メソッド[GET/POST]が自動設定される)
-- @param url         URL
-- @param referer     リファラーURL
-- @param file_type   ファイル種別("text", "binary")
-- @param user_agent  ユーザエージェント(nil の場合 "MZ3" or "MZ4" が自動設定される)
-- @param post        POST 用オブジェクト(未サポート)
--
-- @return なし
--
function mz3.open_url(wnd, access_type, url, referer, type, user_agent, post)
*/
int lua_mz3_open_url(lua_State *L)
{
	// 引数の取得
	CWnd* wnd = (CWnd*)lua_touserdata(L, 1);					// 第1引数
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 2);	// 第2引数
	const char* url = lua_tostring(L, 3);						// 第3引数
	const char* referer = lua_tostring(L, 4);					// 第4引数
	const char* file_type = lua_tostring(L, 5);					// 第5引数
	const char* user_agent = lua_tostring(L, 6);				// 第6引数
	CPostData* post = (CPostData*)lua_touserdata(L, 7);			// 第7引数

	HWND hwnd = NULL;
	if (wnd != NULL) {
		hwnd = wnd->m_hWnd;
	}

	// 通信処理開始
	if (theApp.m_access) {
		MZ3LOGGER_ERROR(L"通信中のためアクセスを行いません");

		// 戻り値の数を返す
		return 0;
	}

	// API 用Dataオブジェクト
	static MZ3Data s_data;
	MZ3Data dummy_data;
	s_data = dummy_data;
	s_data.SetAccessType(access_type);
	s_data.SetURL(CString(url));
	s_data.SetBrowseUri(CString(url));

	theApp.m_mixi4recv = s_data;

	// アクセス種別を設定
	theApp.m_accessType = access_type;

	// encoding 指定
	CInetAccess::ENCODING encoding;
	switch (theApp.m_accessTypeInfo.getRequestEncoding(access_type)) {
	case AccessTypeInfo::ENCODING_SJIS:
		encoding = CInetAccess::ENCODING_SJIS;
		break;
	case AccessTypeInfo::ENCODING_UTF8:
		encoding = CInetAccess::ENCODING_UTF8;
		break;
	case AccessTypeInfo::ENCODING_NOCONVERSION:
		encoding = CInetAccess::ENCODING_NOCONVERSION;
		break;
	case AccessTypeInfo::ENCODING_EUC:
	default:
		encoding = CInetAccess::ENCODING_EUC;
		break;
	}

	// 認証情報の設定
	CString strUser = NULL;
	CString strPassword = NULL;
	if (theApp.m_accessTypeInfo.getServiceType(access_type)=="Twitter") {
		// Twitter API => Basic 認証
		strUser     = theApp.m_loginMng.GetTwitterId();
		strPassword = theApp.m_loginMng.GetTwitterPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox(hwnd, L"ログイン設定画面でユーザIDとパスワードを設定してください", NULL, MB_OK);
			return 0;
		}
	}
	if (theApp.m_accessTypeInfo.getServiceType(access_type)=="Wassr") {
		// Wassr API => Basic 認証
		strUser     = theApp.m_loginMng.GetWassrId();
		strPassword = theApp.m_loginMng.GetWassrPassword();

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox(hwnd, L"ログイン設定画面でユーザIDとパスワードを設定してください", NULL, MB_OK);
			return 0;
		}
	}
	if (theApp.m_accessTypeInfo.getServiceType(access_type)=="gooHome") {
		// gooHome API => Basic 認証
		strUser     = theApp.m_loginMng.GetGooId();
		strPassword = gooutil::GetAPIKeyFromQuoteMailAddress( theApp.m_loginMng.GetGoohomeQuoteMailAddress() );

		// 未指定の場合はエラー出力
		if (strUser.IsEmpty() || strPassword.IsEmpty()) {
			MessageBox(hwnd, L"ログイン設定画面でgooIDとひとこと投稿アドレスを設定してください", NULL, MB_OK);
			return 0;
		}
	}

	// アクセス開始
	theApp.m_access = true;

	// TODO 共通化
	//m_abort = FALSE;

	// GET/POST 判定
	bool bPost = false;	// デフォルトはGET
	switch (theApp.m_accessTypeInfo.getRequestMethod(access_type)) {
	case AccessTypeInfo::REQUEST_METHOD_POST:
		bPost = true;
		break;
	case AccessTypeInfo::REQUEST_METHOD_GET:
	case AccessTypeInfo::REQUEST_METHOD_INVALID:
	default:
		break;
	}

	// post が未指定であれば生成する
	if (bPost && post==NULL) {
		static CPostData s_post;
		// 初期化
		CPostData dummy_post_data;
		s_post = dummy_post_data;
		s_post.SetSuccessMessage( WM_MZ3_POST_END );
		s_post.AppendAdditionalHeader(L"");

		// デフォルトは "Content-Type: multipart/form-data"で。
		s_post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);

		post = &s_post;
	}

	// UserAgent設定
	CString strUserAgent(user_agent);

	// GET/POST 開始
	theApp.m_inet.Initialize(hwnd, &s_data, encoding);
	if (bPost) {
		theApp.m_inet.DoPost(CString(url), CString(referer), CInetAccess::FILE_HTML, post, strUser, strPassword, strUserAgent );
	} else {
		theApp.m_inet.DoGet(CString(url), CString(referer), CInetAccess::FILE_HTML, strUser, strPassword, strUserAgent );
	}

	// 戻り値の数を返す
	return 0;
}

/*
--- キーボード操作
--
-- @param key   キー値
-- @param state "keydown" or "keyup"
--
function mz3.keybd_event(key, state)
*/
int lua_mz3_keybd_event(lua_State *L)
{
	int key(lua_tointeger(L, 1));			// 第1引数
	CStringA state(lua_tostring(L, 2));		// 第2引数

	DWORD s = 0;
	if (state=="keyup") {
		s = KEYEVENTF_KEYUP;
	}
	keybd_event( key, 0, s, 0 );

	// 戻り値の数を返す
	return 0;
}

/*
--- URL をブラウザで開く(確認付き)
--
function mz3.open_url_by_browser_with_confirm(url)
*/
int lua_mz3_open_url_by_browser_with_confirm(lua_State *L)
{
	CString url(lua_tostring(L, 1));		// 第1引数

	util::OpenUrlByBrowserWithConfirm( url );
	
	// 戻り値の数を返す
	return 0;
}

/*
--- URL をブラウザで開く
--
function mz3.open_url_by_browser(url)
*/
int lua_mz3_open_url_by_browser(lua_State *L)
{
	CString url(lua_tostring(L, 1));		// 第1引数

	util::OpenUrlByBrowser( url );
	
	// 戻り値の数を返す
	return 0;
}

//-----------------------------------------------
// MZ3 Account Provider API
//-----------------------------------------------

/*
--- アカウント情報登録(各プラグインでどのような情報が必要か)
--
-- @param service_name サービス名
-- @param param_name   パラメータ名('id_name', 'password_name')
-- @param param_value  パラメータ値
--
function mz3_account_provider.set_param(service_name, param_name, param_value)
*/
int lua_mz3_account_provider_set_param(lua_State *L)
{
	const char* func_name = "mz3_account_provider.set_param";

	// 引数の取得
	const char* service_name = lua_tostring(L, 1);
	std::string param_name   = lua_tostring(L, 2);
	const char* param_value  = lua_tostring(L, 3);

	CMZ3App::AccountData* pData = NULL;
	for (size_t i=0; i<theApp.m_luaAccounts.size(); i++) {
		if (theApp.m_luaAccounts[i].service_name==service_name) {
			pData = &theApp.m_luaAccounts[i];
			break;
		}
	}
	if (pData==NULL) {
		theApp.m_luaAccounts.push_back( CMZ3App::AccountData(service_name) );

		pData = &theApp.m_luaAccounts[ theApp.m_luaAccounts.size()-1 ];
	}

	if (param_name=="id_name") {
		// ログイン設定画面の ID の表示名
		pData->id_name = param_value;
	} else if (param_name=="password_name") {
		// ログイン設定画面の パスワード の表示名
		pData->password_name = param_value;
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 戻り値の数を返す
	return 0;
}

/*
--- アカウント情報の取得(ユーザ設定値の取得)
--
-- @param service_name サービス名
-- @param param_name   パラメータ名('id', 'password')
--
function mz3_account_provider.get_value(service_name, param_name)
*/
int lua_mz3_account_provider_get_value(lua_State *L)
{
	const char* func_name = "mz3_account_provider.get_value";

	// 引数の取得
	const char* service_name = lua_tostring(L, 1);
	std::string param_name   = lua_tostring(L, 2);

	if (param_name=="id") {
		CStringA v( theApp.m_loginMng.GetId(CString(service_name)) );
		lua_pushstring(L, v);
	} else if (param_name=="password") {
		CStringA v( theApp.m_loginMng.GetPassword(CString(service_name)) );
		lua_pushstring(L, v);
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 戻り値の数を返す
	return 1;
}

//-----------------------------------------------
// MZ3 Data API
//-----------------------------------------------

/*
--- MZ3Data オブジェクトの生成
--
-- @return MZ3Data オブジェクト
--
function mz3_data.create()
*/
int lua_mz3_data_create(lua_State *L)
{
	// 生成
	MZ3Data* pData = new MZ3Data();

	// 結果をスタックに戻す
	lua_pushlightuserdata(L, (void*)pData);

	// 戻り値の数を返す
	return 1;
}

/*
--- MZ3Data オブジェクトの破棄
--
-- 注意：Lua 内で create した MZ3Data オブジェクトは必ず delete すること。
--
-- @param  data MZ3Data オブジェクト
-- @return なし
--
function mz3_data.delete(data)
*/
int lua_mz3_data_delete(lua_State *L)
{
	const char* func_name = "mz3_data.delete";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 実行
	delete data;

	// 戻り値の数を返す
	return 0;
}

/*
--- 
--
--
function mz3_data.get_text(data, name)
*/
int lua_mz3_data_get_text(lua_State *L)
{
	const char* func_name = "mz3_data.get_text";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数

	// 値取得
	CString value = data->GetTextValue(CString(name));

	// 結果をスタックに戻す
	lua_pushstring(L, CStringA(value));

	// 戻り値の数を返す
	return 1;
}

/*
--- 
--
--
function mz3_data.get_date(data)
*/
int lua_mz3_data_get_date(lua_State *L)
{
	const char* func_name = "mz3_data.get_date";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 結果をスタックに戻す
	lua_pushstring(L, CStringA(data->GetDate()));

	// 戻り値の数を返す
	return 1;
}

/*
--- 日付の設定
--
-- 任意の日付(文字列形式)を設定するために利用します。
--
-- 通常は mz3_data.parse_date_line を利用して下さい。
--
-- @param  data MZ3Data オブジェクト
-- @param  date 日付文字列
-- @return なし
--
function mz3_data.set_date(data, date)
*/
int lua_mz3_data_set_date(lua_State *L)
{
	const char* func_name = "mz3_data.set_date";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* date = lua_tostring(L, 2);			// 第2引数

	// 値設定
	data->SetDate(CString(date));

	// 戻り値の数を返す
	return 0;
}

/*
--- 
--
--
function mz3_data.set_text(data, name, value)
*/
int lua_mz3_data_set_text(lua_State *L)
{
	const char* func_name = "mz3_data.set_text";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数
	const char* value = lua_tostring(L, 3);			// 第3引数

	// 値設定
	data->SetTextValue(CString(name), CString(value));

	// 戻り値の数を返す
	return 0;
}

/*
--- 
--
--
function mz3_data.get_text_array(data, name, idx)
*/
int lua_mz3_data_get_text_array(lua_State *L)
{
	const char* func_name = "mz3_data.get_text_array";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数
	int idx = lua_tointeger(L, 3);					// 第3引数

	// 値取得
	CString value = data->GetTextArrayValue(CString(name), idx);

	// 結果をスタックに戻す
	lua_pushstring(L, CStringA(value));

	// 戻り値の数を返す
	return 1;
}

/*
--- 
--
--
function mz3_data.get_text_array_size(data, name)
*/
int lua_mz3_data_get_text_array_size(lua_State *L)
{
	const char* func_name = "mz3_data.get_text_array_size";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数

	// 値取得
	int size = data->GetTextArraySize(CString(name));

	// 結果をスタックに戻す
	lua_pushinteger(L, size);

	// 戻り値の数を返す
	return 1;
}

/*
--- name という名前の配列に value を追加する。
--
--
function mz3_data.add_text_array(data, name, value)
*/
int lua_mz3_data_add_text_array(lua_State *L)
{
	const char* func_name = "mz3_data.add_text_array";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数
	const char* value = lua_tostring(L, 3);			// 第3引数

	// 値設定
	data->AddTextArray(CString(name), CString(value));

	// 戻り値の数を返す
	return 0;
}

/*
--- value からリンク等を抽出、整形し、HTML の整形をして、body 配列に追加する
--
--
function mz3_data.add_body_with_extract(data, value)
*/
int lua_mz3_data_add_body_with_extract(lua_State *L)
{
	const char* func_name = "mz3_data.add_body_with_extract";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* value = lua_tostring(L, 2);			// 第2引数

	// 値設定
	mixi::ParserUtil::AddBodyWithExtract(*data, CString(value));

	// 戻り値の数を返す
	return 0;
}

/*
--- 
--
--
function mz3_data.get_integer(data, name)
*/
int lua_mz3_data_get_integer(lua_State *L)
{
	const char* func_name = "mz3_data.get_integer";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数

	// 値取得
	int value = data->GetIntValue(CString(name));

	// 結果をスタックに戻す
	lua_pushinteger(L, value);

	// 戻り値の数を返す
	return 1;
}

/*
--- 
--
--
function mz3_data.set_integer(data, name, value)
*/
int lua_mz3_data_set_integer(lua_State *L)
{
	const char* func_name = "mz3_data.set_integer";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// 第2引数
	int value = lua_tointeger(L, 3);				// 第3引数

	// 値設定
	data->SetIntValue(CString(name), value);

	// 戻り値の数を返す
	return 0;
}

/*
--- 各リスト、子要素の削除
--
--
function mz3_data.clear(data)
*/
int lua_mz3_data_clear(lua_State *L)
{
	const char* func_name = "mz3_data.clear";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	data->ClearAllList();
	data->ClearChildren();

	// 戻り値の数を返す
	return 0;
}

/*
--- 子要素の追加
--
--
function mz3_data.add_child(data, child)
*/
int lua_mz3_data_add_child(lua_State *L)
{
	const char* func_name = "mz3_data.add_child";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3Data* child = (MZ3Data*)lua_touserdata(L, 2);	// 第2引数
	if (child==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	data->AddChild(*child);

	// 戻り値の数を返す
	return 0;
}

/*
--- アクセス種別を設定する。
--
-- @param data MZ3Data オブジェクト
-- @param type [integer]アクセス種別
-- @return なし
--
function mz3_data.set_access_type(data, type)
*/
int lua_mz3_data_set_access_type(lua_State *L)
{
	const char* func_name = "mz3_data.set_access_type";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int value = lua_tointeger(L, 2);				// 第2引数

	// 値設定
	data->SetAccessType((ACCESS_TYPE)value);

	// 戻り値の数を返す
	return 0;
}

/*
--- アクセス種別を取得する。
--
-- @param data MZ3Data オブジェクト
-- @return [integer]アクセス種別
--
function mz3_data.get_access_type(data)
*/
int lua_mz3_data_get_access_type(lua_State *L)
{
	const char* func_name = "mz3_data.get_access_type";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 値設定
	ACCESS_TYPE type = data->GetAccessType();

	// 結果をスタックに戻す
	lua_pushinteger(L, (int)type);

	// 戻り値の数を返す
	return 1;
}

/*
TODO
--- シリアライズキーを取得する。
--
-- @param data MZ3Data オブジェクト
-- @return [string]シリアライズキー
--
function mz3_data.get_serialize_key*(data)
*/

/*
--- 
--
--
function mz3_data.parse_date_line(data, line)
*/
int lua_mz3_data_parse_date_line(lua_State *L)
{
	const char* func_name = "mz3_data.parse_date_line";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* szLine = lua_tostring(L, 2);		// 第2引数

	// 日付のパース
	mixi::ParserUtil::ParseDate(CString(szLine), *data);

	// 戻り値の数を返す
	return 0;
}

/*
--- link_list の個数を取得する
--
--
function mz3_data.get_link_list_size(data)
*/
int lua_mz3_data_get_link_list_size(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_size";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 値取得
	int size = (int)data->m_linkList.size();

	// 結果をスタックに戻す
	lua_pushinteger(L, size);

	// 戻り値の数を返す
	return 1;
}

/*
--- link_list から URL を取得する
--
-- @param data MZ3Data オブジェクト
-- @param idx  インデックス
--
-- @return idx が不正な場合は nil、それ以外は URL 文字列を返す
--
function mz3_data.get_link_list_url(data, idx)
*/
int lua_mz3_data_get_link_list_url(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_url";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// 第2引数

	// 値取得
	if (0<=idx && idx<data->m_linkList.size()) {
		CStringA url( data->m_linkList[idx].url );

		// 結果をスタックに戻す
		lua_pushstring(L, url);
	} else {
		// インデックス不正
		lua_pushnil(L);
	}

	// 戻り値の数を返す
	return 1;
}

/*
--- link_list から TEXT(キャプション) を取得する
--
-- @param data MZ3Data オブジェクト
-- @param idx  インデックス
--
-- @return idx が不正な場合は nil、それ以外は URL 文字列を返す
--
function mz3_data.get_link_list_text(data, idx)
*/
int lua_mz3_data_get_link_list_text(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_text";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// 第2引数

	// 値取得
	if (0<=idx && idx<data->m_linkList.size()) {
		CStringA text( data->m_linkList[idx].text );

		// 結果をスタックに戻す
		lua_pushstring(L, text);
	} else {
		// インデックス不正
		lua_pushnil(L);
	}

	// 戻り値の数を返す
	return 1;
}

/*
--- link_list に追加する
--
-- @param data MZ3Data オブジェクト
-- @param url  URL
-- @param text Text
--
function mz3_data.add_link_list(data, url, text)
*/
int lua_mz3_data_add_link_list(lua_State *L)
{
	const char* func_name = "mz3_data.add_link_list";

	// 引数取得
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// 第1引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* url = lua_tostring(L, 2);	// 第2引数
	const char* text = lua_tostring(L, 3);	// 第3引数

	data->m_linkList.push_back(CMixiData::Link(CString(url), CString(text)));

	// 戻り値の数を返す
	return 0;
}


//-----------------------------------------------
// MZ3 Data List API
//-----------------------------------------------

/*
--- data_list の生成
--
--
function mz3_data_list.create()
*/
int lua_mz3_data_list_create(lua_State *L)
{
	const char* func_name = "mz3_data_list.create";

	MZ3DataList* data_list = new MZ3DataList();
	lua_pushlightuserdata(L, data_list);

	// 戻り値の数を返す
	return 1;
}

/*
--- data_list の消去
--
--
function mz3_data_list.delete(data_list)
*/
int lua_mz3_data_list_delete(lua_State *L)
{
	const char* func_name = "mz3_data_list.delete";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 登録
	delete data_list;

	// 戻り値の数を返す
	return 0;
}

/*
--- data_list の要素をすべて消去する。
--
--
function mz3_data_list.clear(data_list, data)
*/
int lua_mz3_data_list_clear(lua_State *L)
{
	const char* func_name = "mz3_data_list.clear";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 登録
	data_list->clear();

	// 戻り値の数を返す
	return 0;
}

/*
--- data_list の末尾に data を追加する。
--
--
function mz3_data_list.add(data_list, data)
*/
int lua_mz3_data_list_add(lua_State *L)
{
	const char* func_name = "mz3_data_list.add";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 2);					// 第2引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 登録
	data_list->push_back(*data);

	// 戻り値の数を返す
	return 0;
}

/*
--- data_list の index に data を挿入する。
--
--
function mz3_data_list.insert(data_list, index, data)
*/
int lua_mz3_data_list_insert(lua_State *L)
{
	const char* func_name = "mz3_data_list.insert";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);								// 第2引数
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 3);					// 第3引数
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 登録
	data_list->insert(data_list->begin()+index, *data);

	// 戻り値の数を返す
	return 0;
}

/*
--- data_list の個数取得
--
function mz3_data_list.get_count(data_list)
*/
int lua_mz3_data_list_get_count(lua_State *L)
{
	const char* func_name = "mz3_data_list.get_count";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 個数
	lua_pushinteger(L, data_list->size());

	// 戻り値の数を返す
	return 1;
}

/*
--- data_list の要素取得
--
function mz3_data_list.get_data(data_list, idx)
*/
int lua_mz3_data_list_get_data(lua_State *L)
{
	const char* func_name = "mz3_data_list.get_data";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// 第2引数

	lua_pushlightuserdata(L, (void*)&((*data_list)[idx]));

	// 戻り値の数を返す
	return 1;
}

/*
--- data_list のmerge
--
--
function mz3_data_list.merge(data_list, new_list)
*/
int lua_mz3_data_list_merge(lua_State *L)
{
	const char* func_name = "mz3_data_list.merge";

	// 引数取得
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// 第1引数
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3DataList* new_list = (MZ3DataList*)lua_touserdata(L, 2);	// 第2引数
	if (new_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	parser::TwitterParserBase::MergeNewList(*data_list, *new_list);

	// 戻り値の数を返す
	return 0;
}


//-----------------------------------------------
// MZ3 Post Data API
//-----------------------------------------------

/*
--- PostData の生成
--
-- スレッドセーフではない点に注意！
--
--
function mz3_post_data.create()
*/
int lua_mz3_post_data_create(lua_State *L)
{
	static CPostData s_post;
	// 初期化
	CPostData dummy_post_data;
	s_post = dummy_post_data;
	s_post.SetSuccessMessage( WM_MZ3_POST_END );
	s_post.AppendAdditionalHeader(L"");

	// デフォルトは "Content-Type: multipart/form-data"で。
	s_post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);

	// 結果をスタックに戻す
	lua_pushlightuserdata(L, (void*)&s_post);

	// 戻り値の数を返す
	return 1;
}

/*
--- POST する文字列の追加
--
-- @param post        POST 用オブジェクト
-- @param text        追加する文字列
--
function mz3_post_data.append_post_body(post, text)
*/
int lua_mz3_post_data_append_post_body(lua_State *L)
{
	const char* func_name = "mz3_post_data.append_post_body";

	// 引数取得
	CPostData* post = (CPostData*)lua_touserdata(L, 1);
	if (post==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CStringA text = lua_tostring(L, 2);

	// 追加
	post->AppendPostBody(text);

	// 戻り値の数を返す
	return 0;
}

//-----------------------------------------------
// MZ3 HtmlArray API
//-----------------------------------------------

/*
--- データ数を取得する。
--
--
function mz3_htmlarray.get_count(htmlarray)
*/
int lua_mz3_htmlarray_get_count(lua_State *L)
{
	const char* func_name = "mz3_htmlarray.get_count";

	// 引数取得
	CHtmlArray* htmlarray = (CHtmlArray*)lua_touserdata(L, 1);	// 第1引数
	if (htmlarray==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushinteger(L, htmlarray->GetCount());

	// 戻り値の数を返す
	return 1;
}

/*
--- index の要素を取得する。
--
--
function mz3_htmlarray.get_at(htmlarray, index)
*/
int lua_mz3_htmlarray_get_at(lua_State *L)
{
	const char* func_name = "mz3_htmlarray.get_at";

	// 引数取得
	CHtmlArray* htmlarray = (CHtmlArray*)lua_touserdata(L, 1);	// 第1引数
	if (htmlarray==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);							// 第2引数

	// 結果をスタックに積む
	lua_pushstring(L, CStringA(htmlarray->GetAt(index)));

	// 戻り値の数を返す
	return 1;
}

//-----------------------------------------------
// MZ3 Menu API
//-----------------------------------------------

/*
--- メニュー作成
--
-- 作成したメニュー(返り値)は必ず mz3_menu.delete() で削除すること
--
function mz3_menu.create_popup_menu()
*/
int lua_mz3_menu_create_popup_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.create_popup_menu";

	// メニュー作成
	CMenu* pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	// 結果をスタックに積む
	lua_pushlightuserdata(L, (void*)pMenu);

	// 戻り値の数を返す
	return 1;
}

/*
--- メニュー用フック関数の登録。
--
-- 登録した関数は insert_menu, append_menu で利用可能。
--
-- @param hook_function_name メニュー押下時のフック関数名
--
function mz3_menu.regist_menu(hook_function_name)
*/
int lua_mz3_menu_regist_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.regist_menu";

	// 引数取得
	const char* hook_function_name = lua_tostring(L, 1);	// 第1引数

	// メニュー登録
	theApp.m_luaMenus.push_back(hook_function_name);
	int item_id = theApp.m_luaMenus.size()-1;

	// 結果をスタックに積む
	lua_pushinteger(L, item_id);

	// 戻り値の数を返す
	return 1;
}

/*
--- メニューの挿入
--
-- 2009/02/10 現在、メイン画面のみサポート。
--
-- @param menu    メニュー用オブジェクト
-- @param index   追加位置(0オリジン)
-- @param title   タイトル
-- @param item_id regist_menu の返り値
--
function mz3_menu.insert_menu(menu, index, title, item_id)
*/
int lua_mz3_menu_insert_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.regist_menu";

	// 引数取得
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// 第1引数
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);					// 第2引数
	const char* title = lua_tostring(L, 3);				// 第3引数
	int item_id = lua_tointeger(L, 4);					// 第4引数

	// メニュー作成
	pMenu->InsertMenu(index, MF_BYPOSITION | MF_STRING, ID_LUA_MENU_BASE +item_id, CString(title));

	// 戻り値の数を返す
	return 0;
}

/*
--- メニューの追加
--
-- @param menu    メニュー用オブジェクト
-- @param type    メニュー種別("string", "separator")
-- @param title   タイトル
-- @param item_id regist_menu の返り値
--
function mz3_menu.append_menu(menu, type, title, item_id)
*/
int lua_mz3_menu_append_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.append_menu";

	// 引数取得
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// 第1引数
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* type = lua_tostring(L, 2);				// 第2引数
	const char* title = lua_tostring(L, 3);				// 第3引数
	int item_id = lua_tointeger(L, 4);					// 第4引数

	// メニュー作成
	UINT flags = MF_STRING;
	if (strcmp(type, "string")==0) {
		flags = MF_STRING;
	} else if (strcmp(type, "separator")==0) {
		flags = MF_SEPARATOR;
	}
	pMenu->AppendMenu(flags, ID_LUA_MENU_BASE +item_id, CString(title));

	// 戻り値の数を返す
	return 0;
}

/*
--- サブメニューの追加
--
-- @param menu    メニュー用オブジェクト
-- @param title   タイトル
-- @param submenu サブメニュー用オブジェクト
--
function mz3_menu.append_submenu(menu, title, submenu)
*/
int lua_mz3_menu_append_submenu(lua_State *L)
{
	const char* func_name = "mz3_menu.append_submenu";

	// 引数取得
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// 第1引数
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* title = lua_tostring(L, 2);				// 第2引数
	CMenu* pSubMenu = (CMenu*)lua_touserdata(L, 3);		// 第3引数
	if (pSubMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// メニュー作成
	pMenu->AppendMenu(MF_POPUP, (UINT)pSubMenu->m_hMenu, CString(title));

	// 戻り値の数を返す
	return 0;
}

/*
--- メニューのポップアップ
--
-- @param menu メニュー用オブジェクト
-- @param wnd  親ウィンドウ
--
function mz3_menu.popup(menu, wnd)
*/
int lua_mz3_menu_popup(lua_State *L)
{
	const char* func_name = "mz3_menu.popup";

	// 引数取得
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// 第1引数
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CWnd* pWnd = (CWnd*)lua_touserdata(L, 2);		// 第2引数
	if (pWnd==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// メニューを開く
	POINT pt = util::GetPopupPosForSoftKeyMenu2();
	UINT flags = util::GetPopupFlagsForSoftKeyMenu2();
	pMenu->TrackPopupMenu(flags, pt.x, pt.y, pWnd);

	// 戻り値の数を返す
	return 0;
}

/*
--- メニューの破棄
--
-- @param menu メニュー用オブジェクト
--
function mz3_menu.delete(menu)
*/
int lua_mz3_menu_delete(lua_State *L)
{
	const char* func_name = "mz3_menu.delete";

	// 引数取得
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// 第1引数
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// メニューを破棄
	delete pMenu;

	// 戻り値の数を返す
	return 0;
}

//-----------------------------------------------
// MZ3 Inifile API
//-----------------------------------------------

/*
--- ini ファイルから値を取得する
--
-- @param name    iniファイル値
-- @param section iniファイル値のセクション
-- @return [string] iniの値
-- @usage <pre>
-- t = mz3_inifile.get_value("UseGlobalProxy", "General")
-- </pre>
--
function mz3_inifile.get_value(name, section)
*/
int lua_mz3_inifile_get_value(lua_State *L)
{
	const char* func_name = "mz3_inifile.get_value";

	const char* name = lua_tostring(L, 1);				// 第1引数
	const char* section = lua_tostring(L, 2);			// 第2引数

	// 読込
	const CString& fileName = theApp.m_filepath.inifile;

	inifile::IniFile inifile;
	CFileStatus rStatus;
	if (CFile::GetStatus(fileName, rStatus) == FALSE) {
		inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
	}

	if(! inifile.Load( theApp.m_filepath.inifile ) ) {
		// 結果(エラーなのでnil)をスタックに積む
		lua_pushnil(L);
	} else {
		// 結果をスタックに積む
		std::string s = inifile.GetValue(name, section);
		lua_pushstring(L, s.c_str());
	}

	// 戻り値の数を返す
	return 1;
}

/*
TODO
---
function mz3_inifile.set_value*(name, section, value)
*/

//-----------------------------------------------
// MZ3 AccessTypeInfo API
//-----------------------------------------------

/*
--- 新しいアクセス種別の作成
--
-- @return アクセス種別
--
function mz3_access_type_info.new_access_type()
*/
int lua_mz3_access_type_info_new_access_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.new_access_type";

	// 新しいアクセス種別を払い出す
	int access_type = ++theApp.m_luaLastRegistedAccessType;

	// アクセス種別登録
	theApp.m_accessTypeInfo.m_map[(ACCESS_TYPE)access_type] = AccessTypeInfo::Data();

	// 結果をスタックに積む
	lua_pushinteger(L, access_type);

	// 戻り値の数を返す
	return 1;
}

/*
--- アクセス種別の種別の設定
--
-- @param type アクセス種別
-- @param info_type アクセス種別の種別('category', 'body', 'post')
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_info_type(type, info_type)
*/
int lua_mz3_access_type_info_set_info_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_info_type";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& info_type = lua_tostring(L, 2);

	if (info_type=="category") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_CATEGORY;
	} else if (info_type=="body") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_BODY;
	} else if (info_type=="post") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_POST;
	} else {
		lua_pushstring(L, "サポート外のinfo_typeです");
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- サービス種別の設定
--
-- @param type アクセス種別
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_service_type(type, service_type)
*/
int lua_mz3_access_type_info_set_service_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_service_type";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& service_type = lua_tostring(L, 2);

	theApp.m_accessTypeInfo.m_map[access_type].serviceType = service_type;

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- シリアライズキーの設定
--
-- @param type アクセス種別
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_serialize_key(type, serialize_key)
*/
int lua_mz3_access_type_info_set_serialize_key(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_serialize_key";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& serialize_key = lua_tostring(L, 2);

	theApp.m_accessTypeInfo.m_map[access_type].serializeKey = serialize_key;
	theApp.m_accessTypeInfo.m_serializeKeyToAccessKeyMap[serialize_key] = access_type;

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- 簡易タイトルの設定
--
-- @param type アクセス種別
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_short_title(type, short_title)
*/
int lua_mz3_access_type_info_set_short_title(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_short_title";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString short_title(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].shortText = short_title;

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- リクエストメソッドの設定
--
-- @param type アクセス種別
-- @param method_type 'GET' or 'POST'
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_request_method(type, method_type)
*/
int lua_mz3_access_type_info_set_request_method(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_request_method";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& method_type = lua_tostring(L, 2);

	if (method_type=="GET") {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_GET;
	} else if (method_type=="POST") {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_POST;
	} else {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_INVALID;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- キャッシュファイルのパターン設定
--
-- @param type アクセス種別
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_cache_file_pattern(type, file_pattern)
*/
int lua_mz3_access_type_info_set_cache_file_pattern(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_cache_file_pattern";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString file_pattern(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].cacheFilePattern = file_pattern;

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- エンコーディングの設定
--
-- @param type アクセス種別
-- @param encoding エンコーディング('sjis', 'euc-jp', 'utf8', 'no-conversion')
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_request_encoding(type, encoding)
*/
int lua_mz3_access_type_info_set_request_encoding(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_request_encoding";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& encoding = lua_tostring(L, 2);

	if (encoding=="sjis") {
		theApp.m_accessTypeInfo.m_map[access_type].requestEncoding = AccessTypeInfo::ENCODING_SJIS;
	} else if (encoding=="euc-jp") {
		theApp.m_accessTypeInfo.m_map[access_type].requestEncoding = AccessTypeInfo::ENCODING_EUC;
	} else if (encoding=="utf8") {
		theApp.m_accessTypeInfo.m_map[access_type].requestEncoding = AccessTypeInfo::ENCODING_UTF8;
	} else {
		theApp.m_accessTypeInfo.m_map[access_type].requestEncoding = AccessTypeInfo::ENCODING_NOCONVERSION;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- デフォルトURLの設定
--
-- @param type アクセス種別
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_default_url(type, url)
*/
int lua_mz3_access_type_info_set_default_url(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_default_url";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString url(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].defaultCategoryURL = url;

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- ヘッダー
--
-- @param type アクセス種別
-- @param header_no ヘッダー番号(1 to 3)
-- @param header_type ヘッダー種別(何を表示するか : 'date', 'title', 'name', 'body', 'none')
-- @param header_text ヘッダーに表示する文字列
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_body_header(type, header_no, header_type, header_text)
*/
int lua_mz3_access_type_info_set_body_header(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_body_header";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	int header_no = lua_tointeger(L, 2);
	const std::string& header_type = lua_tostring(L, 3);
	CString header_text(lua_tostring(L, 4));

	AccessTypeInfo::BodyHeaderColumn col;
	if (header_type=="date") {
		col.type = AccessTypeInfo::BODY_INDICATE_TYPE_DATE;
	} else if (header_type=="title") {
		col.type = AccessTypeInfo::BODY_INDICATE_TYPE_TITLE;
	} else if (header_type=="name") {
		col.type = AccessTypeInfo::BODY_INDICATE_TYPE_NAME;
	} else if (header_type=="body") {
		col.type = AccessTypeInfo::BODY_INDICATE_TYPE_BODY;
	} else {
		col.type = AccessTypeInfo::BODY_INDICATE_TYPE_NONE;
	}
	col.title = header_text;

	switch (header_no) {
	case 1:	theApp.m_accessTypeInfo.m_map[access_type].bodyHeaderCol1 = col;	break;
	case 2:	theApp.m_accessTypeInfo.m_map[access_type].bodyHeaderCol2 = col;	break;
	case 3:	theApp.m_accessTypeInfo.m_map[access_type].bodyHeaderCol3 = col;	break;
	default:
		lua_pushstring(L, "header_no は 1〜3 のみサポートしています");
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- 統合カラムモード(2行表示モード)の行表示パターン
--
-- @param type アクセス種別
-- @param line_no 行番号(1 or 2)
-- @param pattern パターン
-- @return [bool] 成功時は true、失敗時は false
--
function mz3_access_type_info.set_body_integrated_line_pattern(type, line_no, pattern)
*/
int lua_mz3_access_type_info_set_body_integrated_line_pattern(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_body_integrated_line_pattern";

	// 引数取得
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	int line_no = lua_tointeger(L, 2);
	CString pattern(lua_tostring(L, 3));

	switch (line_no) {
	case 1:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern1 = pattern;	break;
	case 2:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern2 = pattern;	break;
	default:
		lua_pushstring(L, "line_no は 1〜2 のみサポートしています");
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

//-----------------------------------------------
// MZ3 GroupData API
//-----------------------------------------------

/*
--- サポートするサービス種別を取得する
--
-- @param group MZ3GroupData
-- @return サポートするサービス種別のスペース区切り文字列。サービス種別は mz3.regist_service() で登録した文字列。
--
function mz3_group_data.get_services(group)
*/
int lua_mz3_group_data_get_services(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_data.get_services";

	// 引数取得
	Mz3GroupData* pGroup = (Mz3GroupData*)lua_touserdata(L, 1);
	if (pGroup==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushstring(L, pGroup->services.c_str());

	// 戻り値の数を返す
	return 1;
}

/*
--- タブ名からグループを取得する
--
-- @param group MZ3GroupData
-- @param name  タブ名
-- @return MZ3GroupItem
--
function mz3_group_data.get_group_item_by_name(group, name)
*/
int lua_mz3_group_data_get_group_item_by_name(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_data.get_group_item_by_name";

	// 引数取得
	Mz3GroupData* pGroup = (Mz3GroupData*)lua_touserdata(L, 1);
	if (pGroup==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString name(lua_tostring(L, 2));

	CGroupItem* pItem = NULL;
	size_t n = pGroup->groups.size();
	for (size_t i=0; i<n; i++) {
		if (pGroup->groups[i].name==name) {
			pItem = &pGroup->groups[i];
		}
	}

	// 結果をスタックに積む
	lua_pushlightuserdata(L, pItem);

	// 戻り値の数を返す
	return 1;
}

/*
--- タブをグループに追加する
--
-- @param group MZ3GroupData
-- @param tab   MZ3GroupItem
--
function mz3_group_data.append_tab(group, tab)
*/
int lua_mz3_group_data_append_tab(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_data.append_tab";

	// 引数取得
	Mz3GroupData* pGroup = (Mz3GroupData*)lua_touserdata(L, 1);
	if (pGroup==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CGroupItem* pTab = (CGroupItem*)lua_touserdata(L, 2);
	if (pTab==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	pGroup->groups.push_back(*pTab);

	// 戻り値の数を返す
	return 0;
}

//-----------------------------------------------
// MZ3 GroupItem API
//-----------------------------------------------

/*
--- タブを作成する
--
-- @param title         タブの名称
--
-- @return [MZ3GroupItem] tab
--
function mz3_group_item.create(title);
*/
int lua_mz3_group_item_create(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_item.create";

	// 引数取得
	const char* title = lua_tostring(L, 1);

	CGroupItem* pTab = new CGroupItem();
	pTab->init(CString(title), L"", ACCESS_GROUP_GENERAL);

	// 結果をスタックに積む
	lua_pushlightuserdata(L, pTab);

	// 戻り値の数を返す
	return 1;
}

/*
--- カテゴリを追加する
--
-- @param tab           [MZ3GroupItem] タブ
-- @param title         表示タイトル
-- @param serialize_key 関連付ける種別のシリアライズキー
-- @param url           取得先URL
-- @return 成功時は true、失敗時は false
--
function mz3_group_item.append_category(tab, title, serialize_key, url);
*/
int lua_mz3_group_item_append_category(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_item.append_category";

	// 引数取得
	CGroupItem* pTab = (CGroupItem*)lua_touserdata(L, 1);
	if (pTab==NULL) {
		// 不正なタブが指定された。エラーログを出力し続行する。
		MZ3LOGGER_ERROR(CString(make_invalid_arg_error_string(func_name)));
//		lua_pushstring(L, make_invalid_arg_error_string(func_name));
//		lua_error(L);
		return 0;
	}
	const char* title = lua_tostring(L, 2);
	const char* serialize_key = lua_tostring(L, 3);
	const char* url = lua_tostring(L, 4);

	// アクセス種別の取得
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(serialize_key);

	// 追加
	Mz3GroupData::appendCategoryByIniData(theApp.m_accessTypeInfo, *pTab, title, type, url);

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

/*
--- MZ3GroupItem オブジェクトの削除
--
-- @param tab           [MZ3GroupItem] タブ
--
function mz3_group_item.delete(tab);
*/
int lua_mz3_group_item_delete(lua_State *L)
{
	// 引数取得
	const char* func_name = "mz3_group_item.delete";

	// 引数取得
	CGroupItem* pTab = (CGroupItem*)lua_touserdata(L, 1);
	if (pTab==NULL) {
		// 不正なタブが指定された。
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	delete pTab;

	// 戻り値の数を返す
	return 0;
}

//-----------------------------------------------
// MZ3 MainView API
//-----------------------------------------------

/*
--- 書き込み用モード変更(書き込み先URL/API識別用)
--
-- @param mode モード値
--
function mz3_main_view.set_post_mode(mode);
*/
int lua_mz3_main_view_set_post_mode(lua_State *L)
{
	const char* func_name = "mz3_main_view.set_post_mode";

	// 引数の取得
	int mode = lua_tointeger(L, 1);

	// モード設定
	theApp.m_pMainView->m_twitterPostMode = (CMZ3View::TWITTER_STYLE_POST_MODE)mode;

	// 戻り値の数を返す
	return 0;
}

/*
--- コントロールの再配置
--
function mz3_main_view.update_control_status(mode);
*/
int lua_mz3_main_view_update_control_status(lua_State *L)
{
	theApp.m_pMainView->MyUpdateControlStatus();

	// 戻り値の数を返す
	return 0;
}

/*
--- フォーカス移動
--
-- @param focus_control フォーカス移動先コントロール("edit", "category_list", "body_list")
--
function mz3_main_view.set_focus(focus_control);
*/
int lua_mz3_main_view_set_focus(lua_State *L)
{
	// 引数の取得
	CStringA focus_control(lua_tostring(L, 1));

	// フォーカス移動
	int idc = 0;
	if (focus_control=="edit") {
		idc = IDC_STATUS_EDIT;
	} else if (focus_control=="category_list") {
		idc = IDC_HEADER_LIST;
	} else if (focus_control=="body_list") {
		idc = IDC_BODY_LIST;
	}
	CWnd* pWnd = theApp.m_pMainView->GetDlgItem(idc);
	if (pWnd!=NULL) {
		pWnd->SetFocus();
	}

	// 戻り値の数を返す
	return 0;
}

/*
--- 現在選択中の下ペイン要素取得
--
function mz3_main_view.get_selected_body_item();
*/
int lua_mz3_main_view_get_selected_body_item(lua_State *L)
{
	// 結果をスタックに積む
	MZ3Data& data = theApp.m_pMainView->GetSelectedBodyItem();
	lua_pushlightuserdata(L, (void*)&data);

	// 戻り値の数を返す
	return 1;
}

/*
--- 現在選択中の上ペイン要素の種別取得
--
function mz3_main_view.get_selected_category_access_type();
*/
int lua_mz3_main_view_get_selected_category_access_type(lua_State *L)
{
	// 結果をスタックに積む
	CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		lua_pushnil(L);
	} else {
		ACCESS_TYPE access_type = pCategory->m_mixi.GetAccessType();
		lua_pushinteger(L, access_type);
	}

	// 戻り値の数を返す
	return 1;
}

/*
--- カテゴリの追加
--
-- @param title タイトル
-- @param url   URL
-- @param key   シリアライズキー
--
function mz3_main_view.append_category(title, url, key);
*/
int lua_mz3_main_view_append_category(lua_State *L)
{
	// 引数の取得
	CString title(lua_tostring(L, 1));
	CString url(lua_tostring(L, 2));
	const char* key = lua_tostring(L, 3);

	ACCESS_TYPE access_type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);
	if (access_type==ACCESS_INVALID) {
		lua_pushstring(L, "不正なシリアライズキーです");
		lua_error(L);
		return 0;
	}

	// 追加
	CMZ3View* pView = theApp.m_pMainView;

	CCategoryItem categoryItem;
	categoryItem.init( 
		// 名前
		title,
		// URL
		url, 
		access_type, 
		pView->m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(access_type),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(access_type),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(access_type),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	pView->AppendCategoryList(categoryItem);

	// 戻り値の数を返す
	return 0;
}

/*
--- メインビューの取得
--
function mz3_main_view.get_wnd();
*/
int lua_mz3_main_view_get_wnd(lua_State *L)
{
	// 結果をスタックに積む
	lua_pushlightuserdata(L, (void*)theApp.m_pMainView);

	// 戻り値の数を返す
	return 1;
}

/*
--- edit エリアへの文字列設定
--
-- @param text 設定する文字列
--
function mz3_main_view.set_edit_text(text);
*/
int lua_mz3_main_view_set_edit_text(lua_State *L)
{
	// 引数の取得
	CString text(lua_tostring(L, 1));

	// 文字列設定
	theApp.m_pMainView->SetDlgItemText(IDC_STATUS_EDIT, text);

	// 戻り値の数を返す
	return 0;
}

/*
--- edit エリアの文字列取得
--
function mz3_main_view.get_edit_text();
*/
int lua_mz3_main_view_get_edit_text(lua_State *L)
{
	// 文字列取得
	CString text;
	theApp.m_pMainView->GetDlgItemText(IDC_STATUS_EDIT, text);

	// 設定
	lua_pushstring(L, CStringA(text));

	// 戻り値の数を返す
	return 1;
}

/*
--- info エリアへの文字列設定
--
-- @param text 設定する文字列
--
function mz3_main_view.set_info_text(text);
*/
int lua_mz3_main_view_set_info_text(lua_State *L)
{
	// 引数の取得
	CString text(lua_tostring(L, 1));

	// 文字列設定
	util::MySetInformationText( theApp.m_pMainView->m_hWnd, text);

	// 戻り値の数を返す
	return 0;
}


//-----------------------------------------------
// MZ3 API table
//-----------------------------------------------
static const luaL_Reg lua_mz3_lib[] = {
	{"get_app_name",						lua_mz3_get_app_name},
	{"regist_service",						lua_mz3_regist_service},
	{"logger_error",						lua_mz3_logger_error},
	{"logger_info",							lua_mz3_logger_info},
	{"logger_debug",						lua_mz3_logger_debug},
	{"trace",								lua_mz3_trace},
	{"get_tick_count",						lua_mz3_get_tick_count},
	{"alert",								lua_mz3_alert},
	{"confirm",								lua_mz3_confirm},
	{"decode_html_entity",					lua_mz3_decode_html_entity},
	{"estimate_access_type_by_url",			lua_mz3_estimate_access_type_by_url},
	{"get_access_type_by_key",				lua_mz3_get_access_type_by_key},
	{"get_serialize_key_by_access_type",	lua_mz3_get_serialize_key_by_access_type},
	{"get_service_type",					lua_mz3_get_service_type},
	{"set_parser",							lua_mz3_set_parser},
	{"add_event_listener",					lua_mz3_add_event_listener},
	{"open_url",							lua_mz3_open_url},
	{"keybd_event",							lua_mz3_keybd_event},
	{"open_url_by_browser_with_confirm",	lua_mz3_open_url_by_browser_with_confirm},
	{"open_url_by_browser",					lua_mz3_open_url_by_browser},
	{"url_encode",							lua_mz3_url_encode},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_data_lib[] = {
	{"get_date",			lua_mz3_data_get_date},
	{"set_date",			lua_mz3_data_set_date},
	{"get_text",			lua_mz3_data_get_text},
	{"set_text",			lua_mz3_data_set_text},
	{"get_text_array",		lua_mz3_data_get_text_array},
	{"get_text_array_size",	lua_mz3_data_get_text_array_size},
	{"add_text_array",		lua_mz3_data_add_text_array},
	{"add_body_with_extract",	lua_mz3_data_add_body_with_extract},
	{"get_integer",			lua_mz3_data_get_integer},
	{"set_integer",			lua_mz3_data_set_integer},
	{"set_access_type",		lua_mz3_data_set_access_type},
	{"get_access_type",		lua_mz3_data_get_access_type},
	{"create",				lua_mz3_data_create},
	{"delete",				lua_mz3_data_delete},
	{"clear",				lua_mz3_data_clear},
	{"add_child",			lua_mz3_data_add_child},
	{"parse_date_line",		lua_mz3_data_parse_date_line},
	{"get_link_list_url",	lua_mz3_data_get_link_list_url},
	{"get_link_list_text",	lua_mz3_data_get_link_list_text},
	{"get_link_list_size",	lua_mz3_data_get_link_list_size},
	{"add_link_list",		lua_mz3_data_add_link_list},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_data_list_lib[] = {
	{"clear",			lua_mz3_data_list_clear},
	{"add",				lua_mz3_data_list_add},
	{"insert",			lua_mz3_data_list_insert},
	{"get_count",		lua_mz3_data_list_get_count},
	{"get_data",		lua_mz3_data_list_get_data},
	{"merge",			lua_mz3_data_list_merge},
	{"create",			lua_mz3_data_list_create},
	{"delete",			lua_mz3_data_list_delete},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_htmlarray_lib[] = {
	{"get_count",		lua_mz3_htmlarray_get_count},
	{"get_at",			lua_mz3_htmlarray_get_at},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_menu_lib[] = {
	{"create_popup_menu",	lua_mz3_menu_create_popup_menu},
	{"regist_menu",			lua_mz3_menu_regist_menu},
	{"insert_menu",			lua_mz3_menu_insert_menu},
	{"append_menu",			lua_mz3_menu_append_menu},
	{"append_submenu",		lua_mz3_menu_append_submenu},
	{"popup",				lua_mz3_menu_popup},
	{"delete",				lua_mz3_menu_delete},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_inifile_lib[] = {
	{"get_value",		lua_mz3_inifile_get_value},
//	{"set_value",		lua_mz3_inifile_set_value},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_access_type_info_lib[] = {
	{"new_access_type",		lua_mz3_access_type_info_new_access_type},
	{"set_info_type",		lua_mz3_access_type_info_set_info_type},
	{"set_service_type",	lua_mz3_access_type_info_set_service_type},
	{"set_serialize_key",	lua_mz3_access_type_info_set_serialize_key},
	{"set_short_title",		lua_mz3_access_type_info_set_short_title},
	{"set_request_method",	lua_mz3_access_type_info_set_request_method},
	{"set_cache_file_pattern",	lua_mz3_access_type_info_set_cache_file_pattern},
	{"set_request_encoding",	lua_mz3_access_type_info_set_request_encoding},
	{"set_default_url",	lua_mz3_access_type_info_set_default_url},
	{"set_body_header",	lua_mz3_access_type_info_set_body_header},
	{"set_body_integrated_line_pattern",	lua_mz3_access_type_info_set_body_integrated_line_pattern},
	{NULL, NULL}
};
// group data : tabs
static const luaL_Reg lua_mz3_group_data_lib[] = {
	{"get_services",			lua_mz3_group_data_get_services},
	{"get_group_item_by_name",	lua_mz3_group_data_get_group_item_by_name},
	{"append_tab",				lua_mz3_group_data_append_tab},
	{NULL, NULL}
};
// group item : tab = categories
static const luaL_Reg lua_mz3_group_item_lib[] = {
	{"create",			lua_mz3_group_item_create},
	{"append_category", lua_mz3_group_item_append_category},
	{"delete",			lua_mz3_group_item_delete},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_main_view_lib[] = {
	{"set_post_mode",			lua_mz3_main_view_set_post_mode},
	{"update_control_status",	lua_mz3_main_view_update_control_status},
	{"set_focus",				lua_mz3_main_view_set_focus},
	{"get_selected_body_item",	lua_mz3_main_view_get_selected_body_item},
	{"get_selected_category_access_type",	lua_mz3_main_view_get_selected_category_access_type},
	{"append_category",			lua_mz3_main_view_append_category},
	{"get_wnd",					lua_mz3_main_view_get_wnd},
	{"set_edit_text",			lua_mz3_main_view_set_edit_text},
	{"get_edit_text",			lua_mz3_main_view_get_edit_text},
	{"set_info_text",			lua_mz3_main_view_set_info_text},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_post_data_lib[] = {
	{"create",					lua_mz3_post_data_create},
	{"append_post_body",		lua_mz3_post_data_append_post_body},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_account_provider_lib[] = {
	{"set_param",				lua_mz3_account_provider_set_param},
	{"get_value",				lua_mz3_account_provider_get_value},
	{NULL, NULL}
};

void mz3_lua_open_api(lua_State *L)
{
	luaL_register(L, "mz3", lua_mz3_lib);
	luaL_register(L, "mz3_data", lua_mz3_data_lib);
	luaL_register(L, "mz3_data_list", lua_mz3_data_list_lib);
	luaL_register(L, "mz3_htmlarray", lua_mz3_htmlarray_lib);
	luaL_register(L, "mz3_menu", lua_mz3_menu_lib);
	luaL_register(L, "mz3_inifile", lua_mz3_inifile_lib);
	luaL_register(L, "mz3_access_type_info", lua_mz3_access_type_info_lib);

	luaL_register(L, "mz3_group_data", lua_mz3_group_data_lib);
	luaL_register(L, "mz3_group_item", lua_mz3_group_item_lib);

	luaL_register(L, "mz3_main_view", lua_mz3_main_view_lib);
	luaL_register(L, "mz3_post_data", lua_mz3_post_data_lib);

	luaL_register(L, "mz3_account_provider", lua_mz3_account_provider_lib);
}
