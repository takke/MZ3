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
--- �֐����� * �Ŏn�܂�API�͖�����
*/

//-----------------------------------------------
// MZ3 Core API
//-----------------------------------------------

/*
--- ���O�o��(ERROR���x��)���s���B
--
-- @param msg �o�͂��镶����
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
--- ���O�o��(INFO���x��)���s���B
--
-- @param msg �o�͂��镶����
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
--- ���O�o��(DEBUG���x��)���s���B 
--
-- @param msg �o�͂��镶����
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
--- TRACE�o�͂��s���BMZ3�̃R���\�[���t���o�[�W�����݂̂ŕ\���\�B 
--
-- @param msg �o�͂��镶����
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
--- Win32 API �� GetTickCount() ���Ăяo���B
--
-- @return [integer] OS�N��������̌o�ߎ���(ms)
--
function mz3.get_tick_count()
*/
int lua_mz3_get_tick_count(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushinteger(L, GetTickCount());

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- Win32 API �� MessageBox() ���Ăяo���B
--
-- @param msg   ���b�Z�[�W
-- @param title �^�C�g��
--
function mz3.alert(msg, title)
*/
int lua_mz3_alert(lua_State *L)
{
	CString msg(lua_tostring(L, 1));		// ��1����
	CString title(lua_tostring(L, 2));		// ��2����

	MessageBox(GetActiveWindow(), msg, title, MB_OK);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- HTML�G���e�B�e�B���f�R�[�h�����������Ԃ��B 
--
--
function mz3.decode_html_entity(text)
*/
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

/*
--- �A�N�Z�X��ʂ���V���A���C�Y�L�[���擾����B
--
-- @param type [integer]�A�N�Z�X���
-- @return [string] �V���A���C�Y�L�[
--
function mz3.get_serialize_key_by_access_type(type)
*/
int lua_mz3_get_serialize_key_by_access_type(lua_State *L)
{
	ACCESS_TYPE type = (ACCESS_TYPE)lua_tointeger(L, 1);	// ��1����

	// �ϊ�
	const char* serialize_key = theApp.m_accessTypeInfo.getSerializeKey(type);
	
	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, serialize_key);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- URL����ސ������A�N�Z�X��ʂ��擾����B 
--
-- @param url [string] URL
-- @return [integer] �A�N�Z�X���
--
function mz3.estimate_access_type_by_url(url)
*/
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

/*
--- �V���A���C�Y�L�[����A�N�Z�X��ʂ��擾����B
--
-- @param key �V���A���C�Y�L�[
-- @return [integer] �A�N�Z�X���
--
function mz3.get_access_type_by_key(key)
*/
int lua_mz3_get_access_type_by_key(lua_State *L)
{
	const char* key = lua_tostring(L, 1);			// ��1����

	// �ϊ�
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, (int)type);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �p�[�T���w�肷��B
--
-- @param key    �V���A���C�Y�L�[
-- @param parser �p�[�T��({�e�[�u��}.{�֐���})
-- @return �Ȃ�
--
function mz3.set_parser(key, parser)
*/
int lua_mz3_set_parser(lua_State *L)
{
	const char* szKey = lua_tostring(L, 1);			// ��1����:�V���A���C�Y�L�[
	const char* szParserName = lua_tostring(L, 2);	// ��2����:�p�[�T��

	theApp.m_luaParsers[ szKey ] = szParserName;

//	MZ3LOGGER_DEBUG(util::FormatString(L"Registered new parser [%s] for [%s].", 
//						CString(szParserName), CString(szType)));

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �e��C�x���g�ɑ΂���t�b�N�֐���ǉ�����B
--
-- ����C�x���g�ɑ΂��ĕ����̃t�b�N�֐����o�^����Ă���ꍇ�A�Ō�ɓo�^���ꂽ�֐����珇�ɌĂяo���B
--
-- @param event �C�x���g��
-- @param parser �t�b�N�֐���({�e�[�u��}.{�֐���})
-- @see event_listener
-- @rerutn �Ȃ�
--
function mz3.add_event_listener(event, event_handler)

--- �t�b�N�֐�
--
-- @param serialize_key �V���A���C�Y�L�[
-- @param event_name    �C�x���g��
-- @param data          �f�[�^(�C�x���g���ɓ��e�͈قȂ�)
-- @return [bool] ���̃t�b�N�֐��܂��̓f�t�H���g�����������ꍇ�� false, ����ȊO�� true�B
--
function event_listener(serialize_key, event_name, data)
*/
int lua_mz3_add_event_listener(lua_State *L)
{
	const char* szEvent = lua_tostring(L, 1);		// ��1����:�C�x���g
	const char* szParserName = lua_tostring(L, 2);	// ��2����:�p�[�T��

	if (theApp.m_luaHooks.count(szEvent)!=0) {
		theApp.m_luaHooks[ szEvent ].push_back(szParserName);
	} else {
		theApp.m_luaHooks[ szEvent ] = std::vector<std::string>();
		theApp.m_luaHooks[ szEvent ].push_back(szParserName);
	}

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 Data API
//-----------------------------------------------

/*
--- MZ3Data �I�u�W�F�N�g�̐���
--
-- @return MZ3Data �I�u�W�F�N�g
--
function mz3_data.create()
*/
int lua_mz3_data_create(lua_State *L)
{
	// ����
	MZ3Data* pData = new MZ3Data();

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushlightuserdata(L, (void*)pData);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- MZ3Data �I�u�W�F�N�g�̔j��
--
-- ���ӁFLua ���� create ���� MZ3Data �I�u�W�F�N�g�͕K�� delete ���邱�ƁB
--
-- @param  data MZ3Data �I�u�W�F�N�g
-- @return �Ȃ�
--
function mz3_data.delete(data)
*/
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

/*
--- 
--
--
function mz3_data.get_text(data, name)
*/
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

/*
--- 
--
--
function mz3_data.get_date(data)
*/
int lua_mz3_data_get_date(lua_State *L)
{
	const char* func_name = "mz3_data.get_date";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, CStringA(data->GetDate()));

	// �߂�l�̐���Ԃ�
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

/*
--- 
--
--
function mz3_data.get_text_array(data, name, idx)
*/
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

/*
--- 
--
--
function mz3_data.get_text_array_size(data, name)
*/
int lua_mz3_data_get_text_array_size(lua_State *L)
{
	const char* func_name = "mz3_data.get_text_array_size";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* name = lua_tostring(L, 2);			// ��2����

	// �l�擾
	int size = data->GetTextArraySize(CString(name));

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, size);

	// �߂�l�̐���Ԃ�
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

/*
--- 
--
--
function mz3_data.get_integer(data, name)
*/
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

/*
--- 
--
--
function mz3_data.set_integer(data, name, value)
*/
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

/*
--- �A�N�Z�X��ʂ�ݒ肷��B
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @param type [integer]�A�N�Z�X���
-- @return �Ȃ�
--
function mz3_data.set_access_type(data, type)
*/
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

/*
--- �A�N�Z�X��ʂ��擾����B
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @return [integer]�A�N�Z�X���
--
function mz3_data.get_access_type(data)
*/
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

/*
TODO
--- �V���A���C�Y�L�[���擾����B
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @return [string]�V���A���C�Y�L�[
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

/*
--- data_list �̗v�f�����ׂď�������B
--
--
function mz3_data_list.clear(data_list, data)
*/
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

/*
--- data_list �̖����� data ��ǉ�����B
--
--
function mz3_data_list.add(data_list, data)
*/
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

/*
--- data_list �� index �� data ��}������B
--
--
function mz3_data_list.insert(data_list, index, data)
*/
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

/*
--- �f�[�^�����擾����B
--
--
function mz3_htmlarray.get_count(htmlarray)
*/
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

/*
--- index �̗v�f���擾����B
--
--
function mz3_htmlarray.get_at(htmlarray, index)
*/
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

/*
--- ���j���[�p�t�b�N�֐��̓o�^�B
--
-- �o�^�����֐��� insert_menu �ŗ��p�\�B
--
-- @param hook_function_name ���j���[�������̃t�b�N�֐���
--
function mz3_menu.regist_menu(hook_function_name)
*/
int lua_mz3_menu_regist_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.regist_menu";

	// �����擾
	const char* hook_function_name = lua_tostring(L, 1);	// ��1����

	// ���j���[�o�^
	theApp.m_luaMenus.push_back(hook_function_name);
	int item_id = theApp.m_luaMenus.size()-1;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushinteger(L, item_id);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���j���[�̓o�^�B
--
-- 2009/02/10 ���݁A���C����ʂ̂݃T�|�[�g�B
--
-- @param menu    ���j���[�p�I�u�W�F�N�g
-- @param index   �ǉ��ʒu(0�I���W��)
-- @param title   �^�C�g��
-- @param item_id insert_menu �̕Ԃ�l
--
function mz3_menu.insert_menu(menu, index, title, item_id)
*/
int lua_mz3_menu_insert_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.regist_menu";

	// �����擾
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// ��1����
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int index = lua_tointeger(L, 2);					// ��2����
	const char* title = lua_tostring(L, 3);				// ��3����
	int item_id = lua_tointeger(L, 4);					// ��4����

	// ���j���[�쐬
	pMenu->InsertMenu(index, MF_BYPOSITION | MF_STRING, ID_LUA_MENU_BASE +item_id, CString(title));

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ini �t�@�C������l���擾����
--
-- @param name    ini�t�@�C���l
-- @param section ini�t�@�C���l�̃Z�N�V����
-- @return [string] ini�̒l
-- @usage <pre>
-- t = mz3_inifile.get_value("UseGlobalProxy", "General")
-- </pre>
--
function mz3_inifile.get_value(name, section)
*/
int lua_mz3_inifile_get_value(lua_State *L)
{
	const char* func_name = "mz3_inifile.get_value";

	const char* name = lua_tostring(L, 1);				// ��1����
	const char* section = lua_tostring(L, 2);			// ��2����

	// �Ǎ�
	const CString& fileName = theApp.m_filepath.inifile;

	inifile::IniFile inifile;
	CFileStatus rStatus;
	if (CFile::GetStatus(fileName, rStatus) == FALSE) {
		inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
	}

	if(! inifile.Load( theApp.m_filepath.inifile ) ) {
		// ����(�G���[�Ȃ̂�nil)���X�^�b�N�ɐς�
		lua_pushnil(L);
	} else {
		// ���ʂ��X�^�b�N�ɐς�
		std::string s = inifile.GetValue(name, section);
		lua_pushstring(L, s.c_str());
	}

	// �߂�l�̐���Ԃ�
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
--- �V�����A�N�Z�X��ʂ̍쐬
--
-- @return �A�N�Z�X���
--
function mz3_access_type_info.new_access_type()
*/
int lua_mz3_access_type_info_new_access_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.new_access_type";

	// �V�����A�N�Z�X��ʂ𕥂��o��
	int access_type = ++theApp.m_luaLastRegistedAccessType;

	// �A�N�Z�X��ʓo�^
	theApp.m_accessTypeInfo.m_map[(ACCESS_TYPE)access_type] = AccessTypeInfo::Data();

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushinteger(L, access_type);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �A�N�Z�X��ʂ̎�ʂ̐ݒ�
--
-- @param type �A�N�Z�X���
-- @param info_type �A�N�Z�X��ʂ̎��(�Ƃ肠����'category'�̂݃T�|�[�g)
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_info_type(type, info_type)
*/
int lua_mz3_access_type_info_set_info_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_info_type";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& info_type = lua_tostring(L, 2);

	if (info_type=="category") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_CATEGORY;
	} else {
		lua_pushstring(L, "category�̂݃T�|�[�g���Ă��܂�");
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �T�[�r�X��ʂ̐ݒ�
--
-- @param type �A�N�Z�X���
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_service_type(type, service_type)
*/
int lua_mz3_access_type_info_set_service_type(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_service_type";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& service_type = lua_tostring(L, 2);

	theApp.m_accessTypeInfo.m_map[access_type].serviceType = service_type;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �V���A���C�Y�L�[�̐ݒ�
--
-- @param type �A�N�Z�X���
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_serialize_key(type, serialize_key)
*/
int lua_mz3_access_type_info_set_serialize_key(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_serialize_key";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& serialize_key = lua_tostring(L, 2);

	theApp.m_accessTypeInfo.m_map[access_type].serializeKey = serialize_key;
	theApp.m_accessTypeInfo.m_serializeKeyToAccessKeyMap[serialize_key] = access_type;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �ȈՃ^�C�g���̐ݒ�
--
-- @param type �A�N�Z�X���
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_short_title(type, short_title)
*/
int lua_mz3_access_type_info_set_short_title(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_short_title";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString short_title(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].shortText = short_title;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���N�G�X�g���\�b�h�̐ݒ�
--
-- @param type �A�N�Z�X���
-- @param method_type 'GET' or 'POST'
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_request_method(type, method_type)
*/
int lua_mz3_access_type_info_set_request_method(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_request_method";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	const std::string& method_type = lua_tostring(L, 2);

	if (method_type=="GET") {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_GET;
	} else if (method_type=="POST") {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_POST;
	} else {
		theApp.m_accessTypeInfo.m_map[access_type].requestMethod = AccessTypeInfo::REQUEST_METHOD_INVALID;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �L���b�V���t�@�C���̃p�^�[���ݒ�
--
-- @param type �A�N�Z�X���
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_cache_file_pattern(type, file_pattern)
*/
int lua_mz3_access_type_info_set_cache_file_pattern(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_cache_file_pattern";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString file_pattern(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].cacheFilePattern = file_pattern;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �G���R�[�f�B���O�̐ݒ�
--
-- @param type �A�N�Z�X���
-- @param encoding �G���R�[�f�B���O('sjis', 'euc-jp', 'utf8', 'no-conversion')
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_request_encoding(type, encoding)
*/
int lua_mz3_access_type_info_set_request_encoding(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_request_encoding";

	// �����擾
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

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �f�t�H���gURL�̐ݒ�
--
-- @param type �A�N�Z�X���
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_default_url(type, url)
*/
int lua_mz3_access_type_info_set_default_url(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_default_url";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	CString url(lua_tostring(L, 2));

	theApp.m_accessTypeInfo.m_map[access_type].defaultCategoryURL = url;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �w�b�_�[
--
-- @param type �A�N�Z�X���
-- @param header_no �w�b�_�[�ԍ�(1 to 3)
-- @param header_type �w�b�_�[���(����\�����邩 : 'date', 'title', 'name', 'body', 'none')
-- @param header_text �w�b�_�[�ɕ\�����镶����
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_body_header(type, header_no, header_type, header_text)
*/
int lua_mz3_access_type_info_set_body_header(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_body_header";

	// �����擾
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
		lua_pushstring(L, "header_no �� 1�`3 �̂݃T�|�[�g���Ă��܂�");
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �����J�������[�h(2�s�\�����[�h)�̍s�\���p�^�[��
--
-- @param type �A�N�Z�X���
-- @param line_no �s�ԍ�(1 or 2)
-- @param pattern �p�^�[��
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_body_integrated_line_pattern(type, line_no, pattern)
*/
int lua_mz3_access_type_info_set_body_integrated_line_pattern(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_body_integrated_line_pattern";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	int line_no = lua_tointeger(L, 2);
	CString pattern(lua_tostring(L, 3));

	switch (line_no) {
	case 1:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern1 = pattern;	break;
	case 2:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern2 = pattern;	break;
	default:
		lua_pushstring(L, "line_no �� 1�`2 �̂݃T�|�[�g���Ă��܂�");
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
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
