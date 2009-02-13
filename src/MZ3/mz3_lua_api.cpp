/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "stdafx.h"
#include "MZ3.h"
#include "MixiParserUtil.h"
#include "IniFile.h"

//-----------------------------------------------
// lua support
//-----------------------------------------------

#define MZ3_LUA_LOGGER_HEADER	L"(Lua) "

static CStringA make_invalid_arg_error_string(const char* func_name)
{
	CStringA s;
	s.Format("invalid argument for '%s'...", func_name);
	return s;
}

/*
--- 関数名が * で始まるAPIは未実装
*/

//-----------------------------------------------
// MZ3 Core API
//-----------------------------------------------

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
-- 同一イベントに対して複数のフック関数が登録されている場合、最後に登録された関数から順に呼び出す。
--
-- @param event イベント名
-- @param parser フック関数名({テーブル}.{関数名})
-- @see event_listener
-- @rerutn なし
--
function mz3.add_event_listener(event, event_handler)

--- フック関数
--
-- @param serialize_key シリアライズキー
-- @param event_name    イベント名
-- @param data          データ(イベント毎に内容は異なる)
-- @return [bool] 次のフック関数またはデフォルト動作をさせる場合は false, それ以外は true。
--
function event_listener(serialize_key, event_name, data)
*/
int lua_mz3_add_event_listener(lua_State *L)
{
	const char* szEvent = lua_tostring(L, 1);		// 第1引数:イベント
	const char* szParserName = lua_tostring(L, 2);	// 第2引数:パーサ名

	if (theApp.m_luaHooks.count(szEvent)!=0) {
		theApp.m_luaHooks[ szEvent ].push_back(szParserName);
	} else {
		theApp.m_luaHooks[ szEvent ] = std::vector<std::string>();
		theApp.m_luaHooks[ szEvent ].push_back(szParserName);
	}

	// 戻り値の数を返す
	return 0;
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
--- 
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
function *mz3_data.get_serialize_key(data)
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

//-----------------------------------------------
// MZ3 Data List API
//-----------------------------------------------

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

/*
--- メニュー用フック関数の登録。
--
-- 登録した関数は insert_menu で利用可能。
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
--- メニューの登録。
--
-- 2009/02/10 現在、メイン画面のみサポート。
--
-- @param menu    メニュー用オブジェクト
-- @param index   追加位置(0オリジン)
-- @param title   タイトル
-- @param item_id insert_menu の返り値
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
function *mz3_inifile.set_value(name, section, value)
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
-- @param info_type アクセス種別の種別(とりあえず'category'のみサポート)
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
	} else {
		lua_pushstring(L, "categoryのみサポートしています");
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
		lua_pushstring(L, "header_no は 1～3 のみサポートしています");
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
		lua_pushstring(L, "line_no は 1～2 のみサポートしています");
		lua_error(L);
		return 0;
	}

	// 結果をスタックに積む
	lua_pushboolean(L, 1);

	// 戻り値の数を返す
	return 1;
}

// MZ3 API table
static const luaL_Reg lua_mz3_lib[] = {
	{"logger_error",				lua_mz3_logger_error},
	{"logger_info",					lua_mz3_logger_info},
	{"logger_debug",				lua_mz3_logger_debug},
	{"trace",						lua_mz3_trace},
	{"get_tick_count",				lua_mz3_get_tick_count},
	{"alert",						lua_mz3_alert},
	{"decode_html_entity",			lua_mz3_decode_html_entity},
	{"estimate_access_type_by_url", lua_mz3_estimate_access_type_by_url},
	{"get_access_type_by_key",		lua_mz3_get_access_type_by_key},
	{"get_serialize_key_by_access_type", lua_mz3_get_serialize_key_by_access_type},
	{"set_parser",					lua_mz3_set_parser},
	{"add_event_listener",			lua_mz3_add_event_listener},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_data_lib[] = {
	{"get_date",		lua_mz3_data_get_date},
	{"get_text",		lua_mz3_data_get_text},
	{"set_text",		lua_mz3_data_set_text},
	{"get_text_array",	lua_mz3_data_get_text_array},
	{"get_text_array_size",	lua_mz3_data_get_text_array_size},
	{"add_text_array",	lua_mz3_data_add_text_array},
	{"get_integer",		lua_mz3_data_get_integer},
	{"set_integer",		lua_mz3_data_set_integer},
	{"set_access_type",	lua_mz3_data_set_access_type},
	{"get_access_type",	lua_mz3_data_get_access_type},
	{"create",			lua_mz3_data_create},
	{"delete",			lua_mz3_data_delete},
	{"parse_date_line",	lua_mz3_data_parse_date_line},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_data_list_lib[] = {
	{"clear",			lua_mz3_data_list_clear},
	{"add",				lua_mz3_data_list_add},
	{"insert",			lua_mz3_data_list_insert},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_htmlarray_lib[] = {
	{"get_count",		lua_mz3_htmlarray_get_count},
	{"get_at",			lua_mz3_htmlarray_get_at},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_menu_lib[] = {
	{"regist_menu",		lua_mz3_menu_regist_menu},
	{"insert_menu",		lua_mz3_menu_insert_menu},
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

void mz3_lua_open_api(lua_State *L)
{
	luaL_register(L, "mz3", lua_mz3_lib);
	luaL_register(L, "mz3_data", lua_mz3_data_lib);
	luaL_register(L, "mz3_data_list", lua_mz3_data_list_lib);
	luaL_register(L, "mz3_htmlarray", lua_mz3_htmlarray_lib);
	luaL_register(L, "mz3_menu", lua_mz3_menu_lib);
	luaL_register(L, "mz3_inifile", lua_mz3_inifile_lib);
	luaL_register(L, "mz3_access_type_info", lua_mz3_access_type_info_lib);
}
