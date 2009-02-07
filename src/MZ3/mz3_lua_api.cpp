/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "stdafx.h"
#include "MZ3.h"
#include "MixiParserUtil.h"

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

//-----------------------------------------------
// MZ3 Core API
//-----------------------------------------------

// MZ3 API : mz3.trace
int lua_mz3_trace(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3_TRACE(s);

	return 0;
}

// MZ3 API : mz3.logger_debug
int lua_mz3_logger_debug(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_DEBUG(s);

	return 0;
}

// MZ3 API : mz3.logger_info
int lua_mz3_logger_info(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_INFO(s);

	return 0;
}

// MZ3 API : mz3.logger_error
int lua_mz3_logger_error(lua_State *L)
{
	CString s( MZ3_LUA_LOGGER_HEADER );
	s.Append( CString(lua_tostring(L, -1)) );

	MZ3LOGGER_ERROR(s);

	return 0;
}

// MZ3 API : mz3.get_tick_count()
int lua_mz3_get_tick_count(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushinteger(L, GetTickCount());

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3.decode_html_entity(text)
int lua_mz3_decode_html_entity(lua_State *L)
{
	CString value(lua_tostring(L, 1));			// ��1����

	// �ϊ�
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter(value);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, CStringA(value));

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3.estimate_access_type_by_url(url)
int lua_mz3_estimate_access_type_by_url(lua_State *L)
{
	CString url(lua_tostring(L, 1));			// ��1����

	// �ϊ�
	ACCESS_TYPE type = util::EstimateAccessTypeByUrl(url);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, (int)type);

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3.get_access_type_by_key(key)
int lua_mz3_get_access_type_by_key(lua_State *L)
{
	const char* key = lua_tostring(L, 1);			// ��1����

	// �ϊ�
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessKeyBySerializeKey(key);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, (int)type);

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3.set_parser(key, parser)
int lua_mz3_set_parser(lua_State *L)
{
	const char* szKey = lua_tostring(L, 1);			// ��1����:�V���A���C�Y�L�[
	const char* szParserName = lua_tostring(L, 2);		// ��2����:�p�[�T��

	theApp.m_luaParsers[ szKey ] = szParserName;

//	MZ3LOGGER_DEBUG(util::FormatString(L"Registered new parser [%s] for [%s].", 
//						CString(szParserName), CString(szType)));

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 Data API
//-----------------------------------------------

// MZ3 API : mz3_data.create()
int lua_mz3_data_create(lua_State *L)
{
	// ����
	MZ3Data* pData = new MZ3Data();

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushlightuserdata(L, (void*)pData);

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_data.delete(data)
int lua_mz3_data_delete(lua_State *L)
{
	const char* func_name = "mz3_data.delete";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���s
	delete data;

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data.get_text(data, name)
int lua_mz3_data_get_text(lua_State *L)
{
	const char* func_name = "mz3_data.get_text";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����

	// �l�擾
	CString value = data->GetTextValue(CString(name));

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, CStringA(value));

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_data.set_text(data, name, value)
int lua_mz3_data_set_text(lua_State *L)
{
	const char* func_name = "mz3_data.set_text";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����
	const char* value = lua_tostring(L, 3);			// ��3����

	// �l�ݒ�
	data->SetTextValue(CString(name), CString(value));

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data.get_text_array(data, name, idx)
int lua_mz3_data_get_text_array(lua_State *L)
{
	const char* func_name = "mz3_data.get_text_array";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����
	int idx = lua_tointeger(L, 3);					// ��3����

	// �l�擾
	CString value = data->GetTextArrayValue(CString(name), idx);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, CStringA(value));

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_data.add_text_array(data, name, value)
int lua_mz3_data_add_text_array(lua_State *L)
{
	const char* func_name = "mz3_data.add_text_array";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����
	const char* value = lua_tostring(L, 3);			// ��3����

	// �l�ݒ�
	data->AddTextArray(CString(name), CString(value));

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data.get_integer(data, name)
int lua_mz3_data_get_integer(lua_State *L)
{
	const char* func_name = "mz3_data.get_integer";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����

	// �l�擾
	int value = data->GetIntValue(CString(name));

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, value);

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_data.set_integer(data, name, value)
int lua_mz3_data_set_integer(lua_State *L)
{
	const char* func_name = "mz3_data.set_integer";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����
	int value = lua_tointeger(L, 3);				// ��3����

	// �l�ݒ�
	data->SetIntValue(CString(name), value);

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data.set_access_type(data, type)
int lua_mz3_data_set_access_type(lua_State *L)
{
	const char* func_name = "mz3_data.set_access_type";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int value = lua_tointeger(L, 2);				// ��2����

	// �l�ݒ�
	data->SetAccessType((ACCESS_TYPE)value);

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data.get_access_type(data)
int lua_mz3_data_get_access_type(lua_State *L)
{
	const char* func_name = "mz3_data.get_access_type";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �l�ݒ�
	ACCESS_TYPE type = data->GetAccessType();

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, (int)type);

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_data.parse_date_line(data, line)
int lua_mz3_data_parse_date_line(lua_State *L)
{
	const char* func_name = "mz3_data.parse_date_line";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* szLine = lua_tostring(L, 2);		// ��2����

	// ���t�̃p�[�X
	mixi::ParserUtil::ParseDate(CString(szLine), *data);

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 Data List API
//-----------------------------------------------

// MZ3 API : mz3_data_list.clear(data_list, data)
int lua_mz3_data_list_clear(lua_State *L)
{
	const char* func_name = "mz3_data_list.clear";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �o�^
	data_list->clear();

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data_list.add(data_list, data)
int lua_mz3_data_list_add(lua_State *L)
{
	const char* func_name = "mz3_data_list.add";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 2);					// ��2����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �o�^
	data_list->push_back(*data);

	// �߂�l�̐���Ԃ�
	return 0;
}

// MZ3 API : mz3_data_list.insert(data_list, index, data)
int lua_mz3_data_list_insert(lua_State *L)
{
	const char* func_name = "mz3_data_list.insert";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);								// ��2����
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 3);					// ��3����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �o�^
	data_list->insert(data_list->begin()+index, *data);

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 HtmlArray API
//-----------------------------------------------

// MZ3 API : mz3_htmlarray.get_count(htmlarray)
int lua_mz3_htmlarray_get_count(lua_State *L)
{
	const char* func_name = "mz3_htmlarray.get_count";

	// �����擾
	CHtmlArray* htmlarray = (CHtmlArray*)lua_touserdata(L, 1);	// ��1����
	if (htmlarray==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushinteger(L, htmlarray->GetCount());

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API : mz3_htmlarray.get_at(htmlarray, index)
int lua_mz3_htmlarray_get_at(lua_State *L)
{
	const char* func_name = "mz3_htmlarray.get_at";

	// �����擾
	CHtmlArray* htmlarray = (CHtmlArray*)lua_touserdata(L, 1);	// ��1����
	if (htmlarray==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);							// ��2����

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushstring(L, CStringA(htmlarray->GetAt(index)));

	// �߂�l�̐���Ԃ�
	return 1;
}

// MZ3 API table
static const luaL_Reg lua_mz3_lib[] = {
	{"logger_error",	lua_mz3_logger_error},
	{"logger_info",		lua_mz3_logger_info},
	{"logger_debug",	lua_mz3_logger_debug},
	{"trace",			lua_mz3_trace},
	{"get_tick_count",	lua_mz3_get_tick_count},
	{"decode_html_entity", lua_mz3_decode_html_entity},
	{"estimate_access_type_by_url", lua_mz3_estimate_access_type_by_url},
	{"get_access_type_by_key", lua_mz3_get_access_type_by_key},
	{"set_parser",		lua_mz3_set_parser},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_data_lib[] = {
	{"get_text",		lua_mz3_data_get_text},
	{"set_text",		lua_mz3_data_set_text},
	{"get_text_array",	lua_mz3_data_get_text_array},
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

void mz3_lua_open_api(lua_State *L)
{
	luaL_register(L, "mz3", lua_mz3_lib);
	luaL_register(L, "mz3_data", lua_mz3_data_lib);
	luaL_register(L, "mz3_data_list", lua_mz3_data_list_lib);
	luaL_register(L, "mz3_htmlarray", lua_mz3_htmlarray_lib);
}
