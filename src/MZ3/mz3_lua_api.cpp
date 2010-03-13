/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "ReportView.h"
#include "WriteView.h"
#include "MixiParserUtil.h"
#include "TwitterParser.h"
#include "IniFile.h"
#include "url_encoder.h"
#include "version.h"
#include "PostDataGenerator.h"
#include "util_gui.h"
#include "CommonEditDlg.h"
#include "MainFrm.h"
#ifdef WINCE
#include "Nled.h"
#endif

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
--- �֐����� * �ŏI���API�͖�����
*/

//-----------------------------------------------
// MZ3 Core API
//-----------------------------------------------

/*
--- �A�v���P�[�V������("MZ3", "MZ4")��Ԃ�
--
-- @return [string] �A�v���P�[�V������
--
function mz3.get_app_name()
*/
int lua_mz3_get_app_name(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushstring(L, MyWCS2UTF8(MZ3_APP_NAME));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- BuildType("MZ3", "TkTweets")��Ԃ�
--
-- @return [string] �A�v���P�[�V������
--
function mz3.get_app_build_type()
*/
int lua_mz3_get_app_build_type(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
#ifdef BT_MZ3
	lua_pushstring(L, MyWCS2UTF8(L"MZ3"));
#elif BT_TKTW
	lua_pushstring(L, MyWCS2UTF8(L"TkTweets"));
#else
	lua_pushstring(L, MyWCS2UTF8(L"?"));
#endif

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �A�v���P�[�V�����̃o�[�W����("1.0.0 Beta13")��Ԃ�
--
-- @return [string] �o�[�W����������
--
function mz3.get_app_version()
*/
int lua_mz3_get_app_version(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushstring(L, MyWCS2UTF8(MZ3_VERSION_TEXT_SHORT));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �T�[�r�X�o�^(�^�u�̏������p)
--
-- @param service_name     �T�[�r�X��
-- @param default_selected �f�t�H���g��ԂőI���ς݂ɂ��邩�ǂ���
--
function mz3.regist_service(service_name, default_selected)
*/
int lua_mz3_regist_service(lua_State *L)
{
	// �����̎擾
	CStringA name(MyUTF82WCS2(lua_tostring(L, 1)));
	bool selected = lua_toboolean(L, 2) != 0 ? true : false;

	// �o�^
	theApp.m_luaServices.push_back(CMZ3App::Service((const char*)name, selected));

	// �߂�l�̐���Ԃ�
	return 0;
}

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
	s.Append( MyUTF82WCS2(lua_tostring(L, -1)) );

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
	s.Append( MyUTF82WCS2(lua_tostring(L, -1)) );

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
	s.Append( MyUTF82WCS2(lua_tostring(L, -1)) );

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
	s.Append( MyUTF82WCS2(lua_tostring(L, -1)) );

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
	CString msg = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����
	CString title = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����

	MessageBox(GetActiveWindow(), msg, title, MB_OK);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �m�F���
--
-- @param msg   ���b�Z�[�W
-- @param title �^�C�g��
-- @param type  "yes_no", "yes_no_cancel"
-- @return "yes", "no", "cancel"
--
function mz3.confirm(msg, title, type)
*/
int lua_mz3_confirm(lua_State *L)
{
	const char* func_name = "mz3.confirm";
	CString  msg   = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����
	CString  title = MyUTF82WCS2(lua_tostring(L, 2));		// ��2����
	CStringA type  = lua_tostring(L, 3);					// ��3����

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

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- HTML�G���e�B�e�B���f�R�[�h�����������Ԃ��B 
--
--
function mz3.decode_html_entity(text)
*/
int lua_mz3_decode_html_entity(lua_State *L)
{
	CString value = MyUTF82WCS2(lua_tostring(L, 1));			// ��1����

	// �ϊ�
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter(value);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, MyWCS2UTF8(value));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- URL �G���R�[�h�����������Ԃ�
--
-- @param text     �Ώە�����
-- @param encoding "utf8", 'euc-jp'
--
function mz3.url_encode(text, encoding)
*/
int lua_mz3_url_encode(lua_State *L)
{
	const char* func_name = "mz3.url_encode";

	CString text = MyUTF82WCS2(lua_tostring(L, 1));			// ��1����
	CStringA encoding(lua_tostring(L, 2));					// ��2����

	// �ϊ�
	CString url_encoded_text;
	if (encoding=="utf8") {
		url_encoded_text = URLEncoder::encode_utf8(text);
	} else if (encoding=="euc-jp") {
		url_encoded_text = URLEncoder::encode_euc(text);
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, MyWCS2UTF8(url_encoded_text));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �����R�[�h�ϊ�
--
-- @param text         �Ώە�����
-- @param in_encoding  "utf8", "sjis"
-- @param out_encoding "utf8", "sjis"
--
function mz3.convert_encoding(text, in_encoding, out_encoding)
*/
int lua_mz3_convert_encoding(lua_State *L)
{
	const char* func_name = "mz3.convert_encoding";

	CString text = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����
	CStringA in_encoding(lua_tostring(L, 2));			// ��2����
	CStringA out_encoding(lua_tostring(L, 3));			// ��3����

	// �ϊ�
	if (in_encoding == "sjis" && out_encoding == "utf8") {
		// text is already ucs2
		CStringA result;
		kfm::ucs2_to_utf8(text, result);
		lua_pushstring(L, result);
	} else if (in_encoding == "utf8" && out_encoding == "sjis") {
		// text is already ucs2
/*		kfm::kf_buf_type result;
		kfm::kf_buf_type in_text;

		in_text.resize(text.GetLength()+1);
		strncpy((char*)&in_text[0], (const char*)text, text.GetLength());

		kfm::utf8_to_mbcs(in_text, result);
		lua_pushstring(L, (char*)&result[0]);
*/
		lua_pushstring(L, CStringA(text));
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �摜�t�@�C���p�X����(MD5�^)
--
-- @param url url
-- @return �p�X
--
function mz3.make_image_logfile_path_from_url_md5(url)
*/
int lua_mz3_make_image_logfile_path_from_url_md5(lua_State *L)
{
	CString url = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����

	CString path(util::MakeImageLogfilePathFromUrlMD5( url ));
	lua_pushstring(L, MyWCS2UTF8(path));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �t�@�C���R�s�[
--
-- @param from_path �R�s�[���p�X
-- @param to_path   �R�s�[��p�X
--
function mz3.copy_file(from_path, to_path)
*/
int lua_mz3_copy_file(lua_State *L)
{
	// �����擾
	CString from_path = MyUTF82WCS2(lua_tostring(L, 1));
	CString to_path   = MyUTF82WCS2(lua_tostring(L, 2));

	// �t�@�C���R�s�[
	CopyFile( from_path, to_path, FALSE/*bFailIfExists, �㏑��*/ );

	// ���݂��Ȃ��t�@�C�����X�g�ɓo�^����Ă���΍폜����
	if (theApp.m_notFoundFileList.count((LPCTSTR)to_path)>0) {
		theApp.m_notFoundFileList.erase((LPCTSTR)to_path);
	}

	lua_pushboolean(L, 1);

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
	CString url = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����

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
--- �V���A���C�Y�L�[����T�[�r�X��ʂ��擾����B
--
-- @param key �V���A���C�Y�L�[
-- @return [string] �T�[�r�X���
--
function mz3.get_service_type(key)
*/
int lua_mz3_get_service_type(lua_State *L)
{
	const char* key = lua_tostring(L, 1);			// ��1����

	// �ϊ�
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);
	CStringA service_type = theApp.m_accessTypeInfo.getServiceType(type).c_str();

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, service_type);

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
-- call_first �� true �̏ꍇ�A
-- ����C�x���g�ɑ΂��ĕ����̃t�b�N�֐����o�^����Ă���ꍇ�A�Ō�ɓo�^���ꂽ�֐����珇�ɌĂяo���B
-- false �̏ꍇ�͋t���ɌĂяo���B
--
-- @param event         �C�x���g��
-- @param event_handler �t�b�N�֐���({�e�[�u��}.{�֐���})
-- @param call_first    �R�[������(���w�莞��true�Ƃ݂Ȃ�)
--
-- @see event_listener1
-- @see event_listener2
--
-- @rerutn �Ȃ�
--
function mz3.add_event_listener(event, event_handler, call_first)

--- �t�b�N�֐�(�^�C�v1)
--
-- @param serialize_key �V���A���C�Y�L�[
-- @param event_name    �C�x���g��
-- @param data          �f�[�^(�C�x���g���ɓ��e�͈قȂ�)
-- @return [bool] ���̃t�b�N�֐��܂��̓f�t�H���g�����������ꍇ�� false, ����ȊO�� true�B
--
function event_listener1(serialize_key, event_name, data)

--- �t�b�N�֐�(�^�C�v2)
--
-- @param event_name    �C�x���g��
-- @param text          ������
-- @param data          �f�[�^(�C�x���g���ɓ��e�͈قȂ�)
-- @return [bool, integer] ���̃t�b�N�֐��܂��̓f�t�H���g�����������ꍇ�� false, ����ȊO�� true�Btrue ���� integer ��ԋp�\�B
--
function event_listener2(event_name, text, data)
*/
int lua_mz3_add_event_listener(lua_State *L)
{
	const char* szEvent = lua_tostring(L, 1);		// ��1����:�C�x���g
	const char* szParserName = lua_tostring(L, 2);	// ��2����:�p�[�T��

	bool call_first = true;	// �f�t�H���g�� true
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

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- URL ���J��
--
-- MZ3 �̒ʐM�������J�n����
--
-- @param wnd		  �E�B���h�E(mz3_main_view.get_wnd() ���Ŏ擾�����l)
-- @param access_type �A�N�Z�X���(�A�N�Z�X��ʂɉ����Ď擾���\�b�h[GET/POST]�������ݒ肳���)
-- @param url         URL
-- @param referer     ���t�@���[URL
-- @param file_type   �t�@�C�����("text", "binary")
-- @param user_agent  ���[�U�G�[�W�F���g(nil �̏ꍇ "MZ3" or "MZ4" �������ݒ肳���)
-- @param post        POST �p�I�u�W�F�N�g(���T�|�[�g)
-- @param blocking    GET �̃u���b�L���O�^(���w�莞�͔�u���b�L���O�^)
--
-- @return �Ȃ�
--
function mz3.open_url(wnd, access_type, url, referer, type, user_agent, post, blocking)
*/
int lua_mz3_open_url(lua_State *L)
{
	// �����̎擾
	CWnd* wnd = (CWnd*)lua_touserdata(L, 1);					// ��1����
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 2);	// ��2����
	const char* url = lua_tostring(L, 3);						// ��3����
	const char* referer = lua_tostring(L, 4);					// ��4����
	CStringA file_type = lua_tostring(L, 5);					// ��5����
	const char* user_agent = lua_tostring(L, 6);				// ��6����
	CPostData* post = (CPostData*)lua_touserdata(L, 7);			// ��7����
	bool blocking = lua_toboolean(L, 8) ? true : false;			// ��8����

	HWND hwnd = NULL;
	if (wnd != NULL) {
		hwnd = wnd->m_hWnd;
	}

	// �ʐM�����J�n
	if (theApp.m_access) {
		MZ3LOGGER_ERROR(L"�ʐM���̂��߃A�N�Z�X���s���܂���");

		// �߂�l�̐���Ԃ�
		return 0;
	}

	// API �pData�I�u�W�F�N�g
	static MZ3Data s_data;
	MZ3Data dummy_data;
	s_data = dummy_data;
	s_data.SetAccessType(access_type);
	s_data.SetURL(CString(url));
	s_data.SetBrowseUri(CString(url));

	theApp.m_mixi4recv = s_data;

	// �A�N�Z�X��ʂ�ݒ�
	theApp.m_accessType = access_type;

	// encoding �w��
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

	// MZ3 API : BASIC�F�ؐݒ�
	CString strUser = NULL;
	CString strPassword = NULL;
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));	// is_cancel
	rvals.push_back(util::MyLuaData(""));	// id
	rvals.push_back(util::MyLuaData(""));	// password
	if (util::CallMZ3ScriptHookFunctions2("set_basic_auth_account", &rvals, 
			util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(access_type))))
	{
		int is_cancel = rvals[0].m_number;
		if (is_cancel) {
			return 0;
		}
		strUser     = rvals[1].m_strText;
		strPassword = rvals[2].m_strText;
	}

	// �A�N�Z�X�J�n
	theApp.m_access = true;

	// TODO ���ʉ�
	//m_abort = FALSE;
	theApp.m_pMainView->m_abort = FALSE;
	theApp.m_pReportView->m_abort = FALSE;

	// GET/POST ����
	bool bPost = false;	// �f�t�H���g��GET
	switch (theApp.m_accessTypeInfo.getRequestMethod(access_type)) {
	case AccessTypeInfo::REQUEST_METHOD_POST:
		bPost = true;
		break;
	case AccessTypeInfo::REQUEST_METHOD_GET:
	case AccessTypeInfo::REQUEST_METHOD_INVALID:
	default:
		break;
	}

	// post �����w��ł���ΐ�������
	if (bPost && post==NULL) {
		static CPostData s_post;
		// ������
		CPostData dummy_post_data;
		s_post = dummy_post_data;
		s_post.SetSuccessMessage( WM_MZ3_POST_END );
		s_post.AppendAdditionalHeader(L"");

		// �f�t�H���g�� "Content-Type: multipart/form-data"�ŁB
		s_post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);

		post = &s_post;
	}

	// UserAgent�ݒ�
	CString strUserAgent(user_agent);

	// �R���g���[����Ԃ̕ύX
	theApp.m_pMainView->MyUpdateControlStatus();
	theApp.m_pReportView->MyUpdateControlStatus();
#ifdef BT_MZ3
	theApp.m_pWriteView->MyUpdateControlStatus();
#endif

	CInetAccess::FILE_TYPE type = CInetAccess::FILE_HTML;
	if (file_type=="text") {
		type = CInetAccess::FILE_HTML;
	} else if (file_type=="binary") {
		type = CInetAccess::FILE_BINARY;
	}

	// GET/POST �J�n
	theApp.m_inet.Initialize(hwnd, &s_data, encoding);
	if (bPost) {
		// POST
		theApp.m_inet.DoPost(CString(url), CString(referer), type, post, strUser, strPassword, strUserAgent );
	} else {
		// GET
		if (!blocking) {
			// ��u���b�L���O�^�A�N�Z�X
			theApp.m_inet.DoGet(CString(url), CString(referer), type, strUser, strPassword, strUserAgent );
		} else {
			// �u���b�L���O�^�A�N�Z�X
			theApp.m_inet.DoGetBlocking(CString(url), CString(referer), type);
			int status = theApp.m_inet.m_dwHttpStatus;
			const unsigned char* pszMbcs = &theApp.m_inet.out_buf[0];
			CString strWcs2(pszMbcs);
			CStringA strUtf8;
			kfm::ucs2_to_utf8(strWcs2, strUtf8);

			// �ԋp
			lua_pushinteger(L, status);
			lua_pushstring(L, strUtf8);

			// ��ԕ��A
			theApp.m_access = false;

			return 2;
		}
	}

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �O��̃��N�G�X�g��URL�擾
--
function mz3.get_last_request_url()
*/
int lua_mz3_get_last_request_url(lua_State *L)
{
	lua_pushstring(L, MyWCS2UTF8(theApp.m_inet.GetURL()));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �L�[�{�[�h����
--
-- @param key   �L�[�l
-- @param state "keydown" or "keyup"
--
function mz3.keybd_event(key, state)
*/
int lua_mz3_keybd_event(lua_State *L)
{
	int key(lua_tointeger(L, 1));			// ��1����
	CStringA state(lua_tostring(L, 2));		// ��2����

	DWORD s = 0;
	if (state=="keyup") {
		s = KEYEVENTF_KEYUP;
	}
	keybd_event( key, 0, s, 0 );

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- URL ���u���E�U�ŊJ��(�m�F�t��)
--
function mz3.open_url_by_browser_with_confirm(url)
*/
int lua_mz3_open_url_by_browser_with_confirm(lua_State *L)
{
	CString url = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����

	util::OpenUrlByBrowserWithConfirm( url );
	
	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- URL ���u���E�U�ŊJ��
--
function mz3.open_url_by_browser(url)
*/
int lua_mz3_open_url_by_browser(lua_State *L)
{
	CString url = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����

	util::OpenUrlByBrowser( url );
	
	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �t�@�C���I����ʂ̕\��
--
-- @param wnd		   �E�B���h�E(mz3_main_view.get_wnd() ���Ŏ擾�����l)
-- @param title        �L���v�V����
-- @param filter       �t�@�C���t�B���^("JPEĢ�� (*.jpg)%0*.jpg;*.jpeg%0���ׂĂ�̧�� (*.*)%0*.*%0%0"�Ȃ�)
--                     "\0" �͈����n�����ɖ�������邽�� "%0" �Ŏw�肷�邱�ƁB
-- @param flags        �t�@�C���I�[�v���t���O(OPENFILENAME:Flags �̒l)
-- @param initial_dir  �����f�B���N�g��
-- @param initial_file �����t�@�C��
--
-- @return (string) �I���t�@�C���p�X, ���[�U�L�����Z������ nil
--
function mz3.get_open_file_name(wnd, caption, title, flags, initial_dir, initial_file)
*/
int lua_mz3_get_open_file_name(lua_State *L)
{
	// �����̎擾
	CWnd* wnd            = (CWnd*)lua_touserdata(L, 1);
	CString title        = MyUTF82WCS2(lua_tostring(L, 2));
	CString filter0      = MyUTF82WCS2(lua_tostring(L, 3));
	int flags            = lua_tointeger(L, 4);
	CString initial_dir  = MyUTF82WCS2(lua_tostring(L, 5));
	CString initial_file = MyUTF82WCS2(lua_tostring(L, 6));

	// filter �� \0 �̕���
	std::vector<WCHAR> szFilter;
	int n = filter0.GetLength();
	for (int i=0; i<n; i++) {
		if (filter0[i]=='%' && i+1<n && filter0[i+1]=='0') {
			szFilter.push_back('\0');
			i++;
		} else {
			szFilter.push_back(filter0[i]);
		}
	}
//	std::wstring filter = filter0;
//	filter.Replace(L"%0", L"\0");
//	filter = L"JPEĢ�� (*.jpg)\0*.jpg;*.jpeg\0���ׂĂ�̧�� (*.*)\0*.*\0\0";

	HWND hwnd = NULL;
	if (wnd != NULL) {
		hwnd = wnd->m_hWnd;
	}

	WCHAR szFile[MAX_PATH] = L"\0";
	if (!initial_file.IsEmpty()) {
		wcscpy(szFile, initial_file);
	}

	OPENFILENAME ofn;
	memset( &(ofn), 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH; 
	ofn.lpstrTitle = title;
	ofn.lpstrFilter = &szFilter[0];
	ofn.Flags = flags;
	ofn.lpstrInitialDir = initial_dir;
//	if (GetOpenFileName(&ofn) == IDOK) {
	if (util::GetOpenFileNameEx(&ofn) == IDOK) {
		lua_pushstring(L, MyWCS2UTF8(szFile));
	} else {
		lua_pushnil(L);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �J�����B�e(�B�e���ʂ̃t�@�C������Ԃ�)
--
-- ��MZ3 & Pro Mode Only
--
-- @param wnd		   �E�B���h�E(mz3_main_view.get_wnd() ���Ŏ擾�����l)
--
-- @return (string) �I���t�@�C���p�X, ���[�U�L�����Z������ nil
--
function mz3.camera_capture()
*/
int lua_mz3_camera_capture(lua_State *L)
{
#ifdef WINCE
	if (theApp.m_bProMode) {
		CWnd* wnd = (CWnd*)lua_touserdata(L, 1);

		SHCAMERACAPTURE shcc = {0};
		shcc.cbSize = sizeof(shcc);
		shcc.hwndOwner = wnd->m_hWnd;

		if (SHCameraCapture(&shcc) == S_OK) {
			LPCTSTR szFile = shcc.szFile;
			lua_pushstring(L, MyWCS2UTF8(szFile));
		} else {
			lua_pushnil(L);
		}
	} else {
		// �� Pro ���[�h�ł͏�� nil
		lua_pushnil(L);
	}
#else
	// MZ4 �ł͏�� nil
	lua_pushnil(L);
#endif

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ʃG�f�B�b�g��ʂ̕\��
--
-- @param title        �L���v�V����
-- @param msg          ���b�Z�[�W
-- @param initial_text �����l
--
-- @return (string) ���[�U���͒l, ���[�U�L�����Z������ nil
--
function mz3.show_common_edit_dlg(caption, msg, initial_text)
*/
int lua_mz3_show_common_edit_dlg(lua_State *L)
{
	// �����̎擾
	CString title = MyUTF82WCS2(lua_tostring(L, 1));
	CString msg   = MyUTF82WCS2(lua_tostring(L, 2));
	CString initial_text = MyUTF82WCS2(lua_tostring(L, 3));

	CCommonEditDlg dlg;
	dlg.SetTitle( title );
	dlg.SetMessage( msg );
	dlg.mc_strEdit = initial_text;
	if (dlg.DoModal()==IDOK) {
		lua_pushstring(L, MyWCS2UTF8(dlg.mc_strEdit));
	} else {
		lua_pushnil(L);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �r���[�؂�ւ�
--
-- @param view_name �r���[��('main_view', 'report_view', 'write_view')
--
function mz3.change_view(view_name)
*/
int lua_mz3_change_view(lua_State *L)
{
	const char* func_name = "mz3.change_view";

	// �����擾
	CStringA view_name(lua_tostring(L, 1));

	HWND hwndTarget = NULL;
	if (view_name=="report_view") {
		hwndTarget = theApp.m_pReportView->m_hWnd;
	} else if (view_name=="main_view") {
		hwndTarget = theApp.m_pMainView->m_hWnd;
#ifdef BT_MZ3
	} else if (view_name=="write_view") {
		hwndTarget = theApp.m_pWriteView->m_hWnd;
#endif
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	::SendMessage(hwndTarget, WM_MZ3_CHANGE_VIEW, NULL, NULL);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �������݉�ʂ̋N��
--
-- @param write_view_type �������ݎ��
-- @param data            MZ3Data(allow nil)
--
function mz3.start_write_view(write_view_type, data)
*/
int lua_mz3_start_write_view(lua_State *L)
{
#ifdef BT_MZ3
	const char* func_name = "mz3.start_write_view";

	// �����擾
	CStringA write_view_type(lua_tostring(L, 1));
	MZ3Data* pData = (MZ3Data*)lua_touserdata(L, 2);

	theApp.m_pWriteView->StartWriteView(
		theApp.m_accessTypeInfo.getAccessTypeBySerializeKey((const char*)write_view_type), pData);
#endif

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �ڍ׉�ʂ̋N��
--
-- @param data            MZ3Data(allow nil)
--
function mz3.show_detail_view(write_view_type, data)
*/
int lua_mz3_show_detail_view(lua_State *L)
{
	const char* func_name = "mz3.show_detail_view";

	// �����擾
	MZ3Data* pData = (MZ3Data*)lua_touserdata(L, 1);

	theApp.m_pMainView->MyShowDetailView(*pData);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- mixi�̃��O�A�E�g��Ԕ���
--
-- @param serialize_key �V���A���C�Y�L�[
--
function mz3.is_mixi_logout(serialize_key)
*/
#ifdef BT_MZ3
int lua_mz3_is_mixi_logout(lua_State *L)
{
	const char* func_name = "mz3.is_mixi_logout";

	// �����擾
	CStringA serialize_key(lua_tostring(L, 1));

	ACCESS_TYPE aType = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey((const char*)serialize_key);

	if (theApp.IsMixiLogout(aType)) {
		lua_pushboolean(L, 1);
	} else {
		lua_pushboolean(L, 0);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}
#endif

/*
--- [MZ3 only] �o�C�u��ON/OFF����
--
-- �o�C�u��ON/OFF����B�@��ˑ��ɂ�蓮�삵�Ȃ��\��������B
--
-- @param vib_status �o�C�u���(true:ON, false:OFF)
--
function mz3.set_vib_status(vib_status)
*/
int lua_mz3_set_vib_status(lua_State *L)
{
	int vib_status = lua_toboolean(L, 1);

#ifdef WINCE
	struct NLED_SETTINGS_INFO info = { 0 };

	if (vib_status) {
		info.LedNum = (UINT)1;
		info.OffOnBlink = (INT)1;
	} else {
		info.LedNum = (UINT)1;
		info.OffOnBlink = (INT)0;
	}

	NLedSetDevice( NLED_SETTINGS_INFO_ID, (void*)( &info ) );
#endif
	
	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ������̒����擾
--
-- @param text �Ώە�����
--
-- @return integer ����
--
function mz3.get_text_length(text)
*/
int lua_mz3_get_text_length(lua_State *L)
{
	const char* func_name = "mz3.get_text_length";

	// �����擾
	CString text = MyUTF82WCS2(lua_tostring(L, 1));

	int length = text.GetLength();

	lua_pushinteger(L, length);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �ėpMZ3�R�}���h�̎��s
--
-- @param command �R�}���h("FONT_SHRINK", 
--                         "FONT_MAGNIFY", 
--                         "NEXT_TAB", 
--                         "PREV_TAB", 
--                         "CHANGE_MAIN_BODY_HEADER_MODE", 
--                         "SHORT_CUT_MOVE_ON_MAIN_BODY_LIST")
--
function mz3.exec_mz3_command(command)
*/
int lua_mz3_exec_mz3_command(lua_State *L)
{
	// �����擾
	CStringA cmd = lua_tostring(L, 1);

	if (cmd == "FONT_SHRINK") {
		theApp.m_pMainView->OnAcceleratorFontShrink();
	} else if (cmd == "FONT_MAGNIFY") {
		theApp.m_pMainView->OnAcceleratorFontMagnify();
	} else if (cmd == "NEXT_TAB") {
		theApp.m_pMainView->OnAcceleratorNextTab();
	} else if (cmd == "PREV_TAB") {
		theApp.m_pMainView->OnAcceleratorPrevTab();
	} else if (cmd == "CHANGE_MAIN_BODY_HEADER_MODE") {
		theApp.m_pMainView->MyChangeBodyHeader();
	} else if (cmd == "SHORT_CUT_MOVE_ON_MAIN_BODY_LIST") {
		theApp.m_pMainView->MyShortCutMoveOnBodyList();
	}

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 Account Provider API
//-----------------------------------------------

/*
--- �A�J�E���g���o�^(�e�v���O�C���łǂ̂悤�ȏ�񂪕K�v��)
--
-- @param service_name �T�[�r�X��
-- @param param_name   �p�����[�^��('id_name', 'password_name')
-- @param param_value  �p�����[�^�l
--
function mz3_account_provider.set_param(service_name, param_name, param_value)
*/
int lua_mz3_account_provider_set_param(lua_State *L)
{
	const char* func_name = "mz3_account_provider.set_param";

	// �����̎擾
	CStringA service_name(MyUTF82WCS2(lua_tostring(L, 1)));
	CStringA param_name(MyUTF82WCS2(lua_tostring(L, 2)));
	CStringA param_value(MyUTF82WCS2(lua_tostring(L, 3)));

	CMZ3App::AccountData* pData = NULL;
	for (size_t i=0; i<theApp.m_luaAccounts.size(); i++) {
		if (theApp.m_luaAccounts[i].service_name==(const char*)service_name) {
			pData = &theApp.m_luaAccounts[i];
			break;
		}
	}
	if (pData==NULL) {
		theApp.m_luaAccounts.push_back( CMZ3App::AccountData((const char*)service_name) );

		pData = &theApp.m_luaAccounts[ theApp.m_luaAccounts.size()-1 ];
	}

	if (param_name=="id_name") {
		// ���O�C���ݒ��ʂ� ID �̕\����
		pData->id_name = param_value;
	} else if (param_name=="password_name") {
		// ���O�C���ݒ��ʂ� �p�X���[�h �̕\����
		pData->password_name = param_value;
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �A�J�E���g���̎擾(���[�U�ݒ�l�̎擾)
--
-- @param service_name �T�[�r�X��
-- @param param_name   �p�����[�^��('id', 'password')
--
function mz3_account_provider.get_value(service_name, param_name)
*/
int lua_mz3_account_provider_get_value(lua_State *L)
{
	const char* func_name = "mz3_account_provider.get_value";

	// �����̎擾
	CString service_name = MyUTF82WCS2(lua_tostring(L, 1));
	std::string param_name   = lua_tostring(L, 2);

	if (param_name=="id") {
		CStringA v = MyWCS2UTF8( theApp.m_loginMng.GetId(service_name) );
		lua_pushstring(L, v);
	} else if (param_name=="password") {
		CStringA v = MyWCS2UTF8( theApp.m_loginMng.GetPassword(CString(service_name)) );
		lua_pushstring(L, v);
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �A�J�E���g���̐ݒ�
--
-- @param service_name �T�[�r�X��
-- @param param_name   �p�����[�^��('id', 'password')
-- @param param_value  �p�����[�^�l
--
function mz3_account_provider.set_value(service_name, param_name, param_value)
*/
int lua_mz3_account_provider_set_value(lua_State *L)
{
	const char* func_name = "mz3_account_provider.set_value";

	// �����̎擾
	CString service_name = MyUTF82WCS2(lua_tostring(L, 1));
	CStringA param_name   = lua_tostring(L, 2);
	CString param_value  = MyUTF82WCS2(lua_tostring(L, 3));

	if (param_name=="id") {
		theApp.m_loginMng.SetId(service_name, param_value);
		theApp.m_loginMng.Write();
		lua_pushboolean(L, 1);
	} else if (param_name=="password") {
		theApp.m_loginMng.SetPassword(service_name, param_value);
		theApp.m_loginMng.Write();
		lua_pushboolean(L, 1);
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

//-----------------------------------------------
// MZ3 Image Cache API
//-----------------------------------------------

/*
--- URL�ɑ�������L���b�V���C���f�b�N�X���擾����
--
-- @param url �摜URL
--
function mz3_image_cache.get_image_index_by_url(url)
*/
int lua_mz3_image_cache_get_image_index_by_url(lua_State *L)
{
	// �����̎擾
	CString url = MyUTF82WCS2(lua_tostring(L, 1));

	CString path = util::MakeImageLogfilePathFromUrlMD5( url );
	int imageIndex = theApp.m_imageCache.GetImageIndex(path);
	if (imageIndex == -1) {
		// �����[�h�Ȃ̂Ń��[�h����
		CMZ3BackgroundImage image(L"");
		if (!image.load( path )) {
			// ���[�h�G���[
			MZ3LOGGER_ERROR(util::FormatString(L"�摜���[�h���s [%s][%s]", path, url));
			imageIndex = -1;
		} else {
			// ���T�C�Y���ĉ摜�L���b�V���ɒǉ�����B
			imageIndex = theApp.AddImageToImageCache(theApp.m_pMainView, image, path);
		}
	}

	lua_pushinteger(L, imageIndex);

	// �߂�l�̐���Ԃ�
	return 1;
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
	CString name = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����

	// �l�擾
	CString value = data->GetTextValue(name);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, MyWCS2UTF8(value));

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
	lua_pushstring(L, MyWCS2UTF8(data->GetDate()));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���t�̐ݒ�
--
-- �C�ӂ̓��t(������`��)��ݒ肷�邽�߂ɗ��p���܂��B
--
-- �ʏ�� mz3_data.parse_date_line �𗘗p���ĉ������B
--
-- @param  data MZ3Data �I�u�W�F�N�g
-- @param  date ���t������
-- @return �Ȃ�
--
function mz3_data.set_date(data, date)
*/
int lua_mz3_data_set_date(lua_State *L)
{
	const char* func_name = "mz3_data.set_date";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString date = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����

	// �l�ݒ�
	data->SetDate(date);

	// �߂�l�̐���Ԃ�
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

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString name  = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����
	CString value = MyUTF82WCS2(lua_tostring(L, 3));			// ��3����

	// �l�ݒ�
	data->SetTextValue(name, value);

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
	CString name = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����
	int idx = lua_tointeger(L, 3);					// ��3����

	// �l�擾
	CString value = data->GetTextArrayValue(name, idx);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushstring(L, MyWCS2UTF8(value));

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
	CString name = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����

	// �l�擾
	int size = data->GetTextArraySize(name);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, size);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- name �Ƃ������O�̔z��� value ��ǉ�����B
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
	CString name  = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����
	CString value = MyUTF82WCS2(lua_tostring(L, 3));	// ��3����

	// �l�ݒ�
	data->AddTextArray(name, value);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- value ���烊���N���𒊏o�A���`���AHTML �̐��`�����āAbody �z��ɒǉ�����
--
--
function mz3_data.add_body_with_extract(data, value)
*/
int lua_mz3_data_add_body_with_extract(lua_State *L)
{
	const char* func_name = "mz3_data.add_body_with_extract";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString value = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����

	// �l�ݒ�
	mixi::ParserUtil::AddBodyWithExtract(*data, value);

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
	CString name = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����

	// �l�擾
	int value = data->GetIntValue(name);

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
	CString name = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����
	int value = lua_tointeger(L, 3);				// ��3����

	// �l�ݒ�
	data->SetIntValue(name, value);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- 
--
--
function mz3_data.get_integer64_as_string(data, name)
*/
int lua_mz3_data_get_integer64_as_string(lua_State *L)
{
	const char* func_name = "mz3_data.get_integer64_as_string";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString name = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����

	// �l�擾
	INT64 value = data->GetInt64Value(name);

	// ���ʂ��X�^�b�N�ɖ߂�
	// 64bit�l�Ȃ̂ŕ�����Ƃ��ĕԂ�
	CStringA value_text;
	value_text.Format("%I64d", value);
	lua_pushstring(L, value_text);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- 
--
--
function mz3_data.set_integer64_from_string(data, name, value)
*/
int lua_mz3_data_set_integer64_from_string(lua_State *L)
{
	const char* func_name = "mz3_data.set_integer64_from_string";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString name = MyUTF82WCS2(lua_tostring(L, 2));			// ��2����
	CStringA value(MyUTF82WCS2(lua_tostring(L, 3)));		// ��3����

	// �l�ݒ�
	data->SetInt64Value(name, _atoi64(value));

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �e���X�g�A�q�v�f�̍폜
--
--
function mz3_data.clear(data)
*/
int lua_mz3_data_clear(lua_State *L)
{
	const char* func_name = "mz3_data.clear";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	data->ClearAllList();
	data->ClearChildren();

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �q�v�f�̒ǉ�
--
--
function mz3_data.add_child(data, child)
*/
int lua_mz3_data_add_child(lua_State *L)
{
	const char* func_name = "mz3_data.add_child";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3Data* child = (MZ3Data*)lua_touserdata(L, 2);	// ��2����
	if (child==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	data->AddChild(*child);

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

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString szLine = MyUTF82WCS2(lua_tostring(L, 2));		// ��2����

	// ���t�̃p�[�X
	mixi::ParserUtil::ParseDate(szLine, *data);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- link_list �̌����擾����
--
--
function mz3_data.get_link_list_size(data)
*/
int lua_mz3_data_get_link_list_size(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_size";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �l�擾
	int size = (int)data->m_linkList.size();

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushinteger(L, size);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- link_list ���� URL ���擾����
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @param idx  �C���f�b�N�X
--
-- @return idx ���s���ȏꍇ�� nil�A����ȊO�� URL �������Ԃ�
--
function mz3_data.get_link_list_url(data, idx)
*/
int lua_mz3_data_get_link_list_url(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_url";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// ��2����

	// �l�擾
	if (0<=idx && (unsigned int)idx<data->m_linkList.size()) {
		CStringA url = MyWCS2UTF8( data->m_linkList[idx].url );

		// ���ʂ��X�^�b�N�ɖ߂�
		lua_pushstring(L, url);
	} else {
		// �C���f�b�N�X�s��
		lua_pushnil(L);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- link_list ���� TEXT(�L���v�V����) ���擾����
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @param idx  �C���f�b�N�X
--
-- @return idx ���s���ȏꍇ�� nil�A����ȊO�� URL �������Ԃ�
--
function mz3_data.get_link_list_text(data, idx)
*/
int lua_mz3_data_get_link_list_text(lua_State *L)
{
	const char* func_name = "mz3_data.get_link_list_text";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// ��2����

	// �l�擾
	if (0<=idx && (unsigned int)idx<data->m_linkList.size()) {
		CStringA text = MyWCS2UTF8( data->m_linkList[idx].text );

		// ���ʂ��X�^�b�N�ɖ߂�
		lua_pushstring(L, text);
	} else {
		// �C���f�b�N�X�s��
		lua_pushnil(L);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- link_list �ɒǉ�����
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @param url  URL
-- @param text Text
-- @param type 'list'(default) or 'page'(�y�[�W�ύX�����N�p)
--
function mz3_data.add_link_list(data, url, text, type)
*/
int lua_mz3_data_add_link_list(lua_State *L)
{
	const char* func_name = "mz3_data.add_link_list";

	// �����擾
	MZ3Data* data = (MZ3Data*)lua_touserdata(L, 1);	// ��1����
	if (data==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString url  = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����
	CString text = MyUTF82WCS2(lua_tostring(L, 3));	// ��3����
	const char* type = lua_tostring(L, 4);			// ��4����

	if (type!=NULL && strcmp(type, "page")==0) {
		data->m_linkPage.push_back(CMixiData::Link(url, text));
	} else {
		data->m_linkList.push_back(CMixiData::Link(url, text));
	}

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 Data List API
//-----------------------------------------------

/*
--- data_list �̐���
--
--
function mz3_data_list.create()
*/
int lua_mz3_data_list_create(lua_State *L)
{
	const char* func_name = "mz3_data_list.create";

	MZ3DataList* data_list = new MZ3DataList();
	lua_pushlightuserdata(L, data_list);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- data_list �̏���
--
--
function mz3_data_list.delete(data_list)
*/
int lua_mz3_data_list_delete(lua_State *L)
{
	const char* func_name = "mz3_data_list.delete";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// �o�^
	delete data_list;

	// �߂�l�̐���Ԃ�
	return 0;
}

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

/*
--- data_list �̌��擾
--
function mz3_data_list.get_count(data_list)
*/
int lua_mz3_data_list_get_count(lua_State *L)
{
	const char* func_name = "mz3_data_list.get_count";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
//		lua_pushstring(L, make_invalid_arg_error_string(func_name));
//		lua_error(L);
		lua_pushinteger(L, 0);
//		return 0;
	} else {
		// ��
		lua_pushinteger(L, data_list->size());
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- data_list �̗v�f�擾
--
function mz3_data_list.get_data(data_list, idx)
*/
int lua_mz3_data_list_get_data(lua_State *L)
{
	const char* func_name = "mz3_data_list.get_data";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int idx = lua_tointeger(L, 2);	// ��2����

	lua_pushlightuserdata(L, (void*)&((*data_list)[idx]));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- data_list ��merge
--
--
function mz3_data_list.merge(data_list, new_list, max_size)
*/
int lua_mz3_data_list_merge(lua_State *L)
{
	const char* func_name = "mz3_data_list.merge";

	// �����擾
	MZ3DataList* data_list = (MZ3DataList*)lua_touserdata(L, 1);	// ��1����
	if (data_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	MZ3DataList* new_list = (MZ3DataList*)lua_touserdata(L, 2);	// ��2����
	if (new_list==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	int max_size = lua_tointeger(L, 3);							// ��3����
	if (max_size==0) {
		parser::TwitterParserBase::MergeNewList(*data_list, *new_list);
	} else {
		parser::TwitterParserBase::MergeNewList(*data_list, *new_list, max_size);
	}

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 Post Data API
//-----------------------------------------------

/*
--- PostData �̐���
--
-- �X���b�h�Z�[�t�ł͂Ȃ��_�ɒ��ӁI
--
--
function mz3_post_data.create()
*/
int lua_mz3_post_data_create(lua_State *L)
{
	static CPostData s_post;
	// ������
	CPostData dummy_post_data;
	s_post = dummy_post_data;
	s_post.SetSuccessMessage( WM_MZ3_POST_END );
	s_post.AppendAdditionalHeader(L"");

	// �f�t�H���g�� "Content-Type: multipart/form-data"�ŁB
	s_post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);

	// ���ʂ��X�^�b�N�ɖ߂�
	lua_pushlightuserdata(L, (void*)&s_post);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- Content-Type �̐ݒ�
--
-- @param post         POST �p�I�u�W�F�N�g
-- @param content_type Content-Type �l
--
function mz3_post_data.set_content_type(post, content_type)
*/
int lua_mz3_post_data_set_content_type(lua_State *L)
{
	const char* func_name = "mz3_post_data.set_content_type";

	// �����擾
	CPostData* post = (CPostData*)lua_touserdata(L, 1);
	if (post==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString content_type = MyUTF82WCS2(lua_tostring(L, 2));

	// �ǉ�
	post->SetContentType(content_type);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- POST ���镶����̒ǉ�
--
-- @param post        POST �p�I�u�W�F�N�g
-- @param text        �ǉ����镶����
--
function mz3_post_data.append_post_body(post, text)
*/
int lua_mz3_post_data_append_post_body(lua_State *L)
{
	const char* func_name = "mz3_post_data.append_post_body";

	// �����擾
	CPostData* post = (CPostData*)lua_touserdata(L, 1);
	if (post==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	// URL�G���R�[�h�ς݂̂͂��Ȃ̂ŕϊ��s�v
	CStringA text = lua_tostring(L, 2);

	// �ǉ�
	post->AppendPostBody(text);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- POST ����w�b�_�[�̒ǉ�
--
-- @param post        POST �p�I�u�W�F�N�g
-- @param text        �ǉ����镶����
--
function mz3_post_data.append_additional_header(post, text)
*/
int lua_mz3_post_data_append_additional_header(lua_State *L)
{
	const char* func_name = "mz3_post_data.append_additional_header";

	// �����擾
	CPostData* post = (CPostData*)lua_touserdata(L, 1);
	if (post==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString text = MyUTF82WCS2(lua_tostring(L, 2));

	// �ǉ�
	post->AppendAdditionalHeader(text);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �t�@�C��(�o�C�i���t�@�C��)��ǉ�
--
-- @param post        POST �p�I�u�W�F�N�g
-- @param filename    �t�@�C����
--
function mz3_post_data.append_file(post, filename)
*/
int lua_mz3_post_data_append_file(lua_State *L)
{
	const char* func_name = "mz3_post_data.append_file";

	// �����擾
	CPostData* post = (CPostData*)lua_touserdata(L, 1);
	if (post==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString filename = MyUTF82WCS2(lua_tostring(L, 2));

	// �ǉ�
	bool rval = mixi::PostDataGeneratorBase::appendFile(*post, filename);

	lua_pushboolean(L, rval ? 1 : 0);

	// �߂�l�̐���Ԃ�
	return 1;
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
	lua_pushstring(L, MyWCS2UTF8(htmlarray->GetAt(index)));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �S�Č��������������Ԃ�
--
--
function mz3_htmlarray.get_all_text(htmlarray, index)
*/
int lua_mz3_htmlarray_get_all_text(lua_State *L)
{
	const char* func_name = "mz3_htmlarray.get_all_text";

	// �����擾
	CHtmlArray* htmlarray = (CHtmlArray*)lua_touserdata(L, 1);	// ��1����
	if (htmlarray==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	CString s;
	u_int count = htmlarray->GetCount();
	for (u_int i=0; i<count; i++) {
		s += htmlarray->GetAt(i);
	}
	lua_pushstring(L, MyWCS2UTF8(s));

	// �߂�l�̐���Ԃ�
	return 1;
}

//-----------------------------------------------
// MZ3 Menu API
//-----------------------------------------------

/*
--- ���j���[�쐬
--
-- �쐬�������j���[(�Ԃ�l)�͕K�� mz3_menu.delete() �ō폜���邱��
--
function mz3_menu.create_popup_menu()
*/
int lua_mz3_menu_create_popup_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.create_popup_menu";

	// ���j���[�쐬
	CMenu* pMenu = new CMenu();
	pMenu->CreatePopupMenu();

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, (void*)pMenu);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���j���[�p�t�b�N�֐��̓o�^�B
--
-- �o�^�����֐��� insert_menu, append_menu �ŗ��p�\�B
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
--- ���j���[�̑}��
--
-- 2009/02/10 ���݁A���C����ʂ̂݃T�|�[�g�B
--
-- @param menu    ���j���[�p�I�u�W�F�N�g
-- @param index   �ǉ��ʒu(0�I���W��)
-- @param title   �^�C�g��
-- @param item_id regist_menu �̕Ԃ�l
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
	CString title = MyUTF82WCS2(lua_tostring(L, 3));	// ��3����
	int item_id = lua_tointeger(L, 4);					// ��4����

	// ���j���[�쐬
	pMenu->InsertMenu(index, MF_BYPOSITION | MF_STRING, ID_LUA_MENU_BASE +item_id, title);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ���j���[�̒ǉ�
--
-- @param menu    ���j���[�p�I�u�W�F�N�g
-- @param type    ���j���[���("string", "separator")
-- @param title   �^�C�g��
-- @param item_id regist_menu �̕Ԃ�l
--
function mz3_menu.append_menu(menu, type, title, item_id)
*/
int lua_mz3_menu_append_menu(lua_State *L)
{
	const char* func_name = "mz3_menu.append_menu";

	// �����擾
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// ��1����
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	const char* type = lua_tostring(L, 2);				// ��2����
	CString title = MyUTF82WCS2(lua_tostring(L, 3));	// ��3����
	int item_id = lua_tointeger(L, 4);					// ��4����

	// ���j���[�쐬
	UINT flags = MF_STRING;
	if (strcmp(type, "string")==0) {
		flags = MF_STRING;
	} else if (strcmp(type, "separator")==0) {
		flags = MF_SEPARATOR;
	}
	pMenu->AppendMenu(flags, ID_LUA_MENU_BASE +item_id, CString(title));

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �T�u���j���[�̒ǉ�
--
-- @param menu    ���j���[�p�I�u�W�F�N�g
-- @param title   �^�C�g��
-- @param submenu �T�u���j���[�p�I�u�W�F�N�g
--
function mz3_menu.append_submenu(menu, title, submenu)
*/
int lua_mz3_menu_append_submenu(lua_State *L)
{
	const char* func_name = "mz3_menu.append_submenu";

	// �����擾
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// ��1����
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString title = MyUTF82WCS2(lua_tostring(L, 2));	// ��2����
	CMenu* pSubMenu = (CMenu*)lua_touserdata(L, 3);		// ��3����
	if (pSubMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���j���[�쐬
	pMenu->AppendMenu(MF_POPUP, (UINT)pSubMenu->m_hMenu, title);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ���j���[�̃|�b�v�A�b�v
--
-- @param menu ���j���[�p�I�u�W�F�N�g
-- @param wnd  �e�E�B���h�E
--
function mz3_menu.popup(menu, wnd)
*/
int lua_mz3_menu_popup(lua_State *L)
{
	const char* func_name = "mz3_menu.popup";

	// �����擾
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// ��1����
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CWnd* pWnd = (CWnd*)lua_touserdata(L, 2);		// ��2����
	if (pWnd==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���j���[���J��
	POINT pt = util::GetPopupPosForSoftKeyMenu2();
	UINT flags = util::GetPopupFlagsForSoftKeyMenu2();
	pMenu->TrackPopupMenu(flags, pt.x, pt.y, pWnd);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ���j���[�̔j��
--
-- @param menu ���j���[�p�I�u�W�F�N�g
--
function mz3_menu.delete(menu)
*/
int lua_mz3_menu_delete(lua_State *L)
{
	const char* func_name = "mz3_menu.delete";

	// �����擾
	CMenu* pMenu = (CMenu*)lua_touserdata(L, 1);		// ��1����
	if (pMenu==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���j���[��j��
	delete pMenu;

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 Inifile API
//-----------------------------------------------

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

	CString name    = MyUTF82WCS2(lua_tostring(L, 1));		// ��1����
	CString section = MyUTF82WCS2(lua_tostring(L, 2));		// ��2����

	// �Ǎ�
	const CString& fileName = theApp.m_filepath.inifile;

	inifile::IniFile inifile;
	CFileStatus rStatus;
	if (CFile::GetStatus(fileName, rStatus) == FALSE) {
		inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
	}

	if (!inifile.Load( theApp.m_filepath.inifile )) {
		// ����(�G���[�Ȃ̂�nil)���X�^�b�N�ɐς�
		lua_pushnil(L);
	} else {
		// ���ʂ��X�^�b�N�ɐς�
		CStringA name_a(name);
		CStringA section_a(section);
		std::string s = inifile.GetValue((const char*)name_a, (const char*)section_a);
		lua_pushstring(L, MyWCS2UTF8(CString(s.c_str())));
	}

	// �߂�l�̐���Ԃ�
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
-- @param info_type �A�N�Z�X��ʂ̎��('category', 'body', 'post', 'other')
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
	} else if (info_type=="body") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_BODY;
	} else if (info_type=="post") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_POST;
	} else if (info_type=="other") {
		theApp.m_accessTypeInfo.m_map[access_type].infoType = AccessTypeInfo::INFO_TYPE_OTHER;
	} else {
		lua_pushstring(L, MyWCS2UTF8(L"�T�|�[�g�O��info_type�ł�"));
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
	CString short_title = MyUTF82WCS2(lua_tostring(L, 2));

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
	CString file_pattern = MyUTF82WCS2(lua_tostring(L, 2));

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
	CString url = MyUTF82WCS2(lua_tostring(L, 2));

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
	CString header_text = MyUTF82WCS2(lua_tostring(L, 4));

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
		lua_pushstring(L, MyWCS2UTF8(L"header_no �� 1�`3 �̂݃T�|�[�g���Ă��܂�"));
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
	CString pattern = MyUTF82WCS2(lua_tostring(L, 3));

	switch (line_no) {
	case 1:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern1 = pattern;	break;
	case 2:	theApp.m_accessTypeInfo.m_map[access_type].bodyIntegratedLinePattern2 = pattern;	break;
	default:
		lua_pushstring(L, MyWCS2UTF8(L"line_no �� 1�`2 �̂݃T�|�[�g���Ă��܂�"));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ����ΏۂƂ��邩�̐ݒ�
--
-- @param type �A�N�Z�X���
-- @param is_cruise_target ����ΏۂƂ��邩
-- @return [bool] �������� true�A���s���� false
--
function mz3_access_type_info.set_cruise_target(type, is_cruise_target)
*/
int lua_mz3_access_type_info_set_cruise_target(lua_State *L)
{
	const char* func_name = "mz3_access_type_info.set_cruise_target";

	// �����擾
	ACCESS_TYPE access_type = (ACCESS_TYPE)lua_tointeger(L, 1);
	int is_cruise_target = lua_toboolean(L, 2);

	theApp.m_accessTypeInfo.m_map[access_type].bCruiseTarget = is_cruise_target ? true : false;

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

//-----------------------------------------------
// MZ3 GroupData API
//-----------------------------------------------

/*
--- �T�|�[�g����T�[�r�X��ʂ��擾����
--
-- @param group MZ3GroupData
-- @return �T�|�[�g����T�[�r�X��ʂ̃X�y�[�X��؂蕶����B�T�[�r�X��ʂ� mz3.regist_service() �œo�^����������B
--
function mz3_group_data.get_services(group)
*/
int lua_mz3_group_data_get_services(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_data.get_services";

	// �����擾
	Mz3GroupData* pGroup = (Mz3GroupData*)lua_touserdata(L, 1);
	if (pGroup==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushstring(L, pGroup->services.c_str());

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �^�u������O���[�v���擾����
--
-- @param group MZ3GroupData
-- @param name  �^�u��
-- @return MZ3GroupItem
--
function mz3_group_data.get_group_item_by_name(group, name)
*/
int lua_mz3_group_data_get_group_item_by_name(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_data.get_group_item_by_name";

	// �����擾
	Mz3GroupData* pGroup = (Mz3GroupData*)lua_touserdata(L, 1);
	if (pGroup==NULL) {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}
	CString name = MyUTF82WCS2(lua_tostring(L, 2));

	CGroupItem* pItem = NULL;
	size_t n = pGroup->groups.size();
	for (size_t i=0; i<n; i++) {
		if (pGroup->groups[i].name==name) {
			pItem = &pGroup->groups[i];
		}
	}

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, pItem);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �^�u���O���[�v�ɒǉ�����
--
-- @param group MZ3GroupData
-- @param tab   MZ3GroupItem
--
function mz3_group_data.append_tab(group, tab)
*/
int lua_mz3_group_data_append_tab(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_data.append_tab";

	// �����擾
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

	// �߂�l�̐���Ԃ�
	return 0;
}

//-----------------------------------------------
// MZ3 GroupItem API
//-----------------------------------------------

/*
--- �^�u���쐬����
--
-- @param title         �^�u�̖���
--
-- @return [MZ3GroupItem] tab
--
function mz3_group_item.create(title);
*/
int lua_mz3_group_item_create(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_item.create";

	// �����擾
	CString title = MyUTF82WCS2(lua_tostring(L, 1));

	CGroupItem* pTab = new CGroupItem();
	pTab->init(title, L"", ACCESS_GROUP_GENERAL);

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, pTab);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �J�e�S����ǉ�����
--
-- @param tab           [MZ3GroupItem] �^�u
-- @param title         �\���^�C�g��
-- @param serialize_key �֘A�t�����ʂ̃V���A���C�Y�L�[
-- @param url           �擾��URL
-- @return �������� true�A���s���� false
--
function mz3_group_item.append_category(tab, title, serialize_key, url);
*/
int lua_mz3_group_item_append_category(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_item.append_category";

	// �����擾
	CGroupItem* pTab = (CGroupItem*)lua_touserdata(L, 1);
	if (pTab==NULL) {
		// �s���ȃ^�u���w�肳�ꂽ�B�G���[���O���o�͂����s����B
		MZ3LOGGER_ERROR(CString(make_invalid_arg_error_string(func_name)));
//		lua_pushstring(L, make_invalid_arg_error_string(func_name));
//		lua_error(L);
		return 0;
	}
	CStringA title         (MyUTF82WCS2(lua_tostring(L, 2)));
	CStringA serialize_key (MyUTF82WCS2(lua_tostring(L, 3)));
	CStringA url           (MyUTF82WCS2(lua_tostring(L, 4)));

	// �A�N�Z�X��ʂ̎擾
	ACCESS_TYPE type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey((const char*)serialize_key);

	// �ǉ�
	Mz3GroupData::appendCategoryByIniData(theApp.m_accessTypeInfo, *pTab, (const char*)title, type, url);

	// ���ʂ��X�^�b�N�ɐς�
	lua_pushboolean(L, 1);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- MZ3GroupItem �I�u�W�F�N�g�̍폜
--
-- @param tab           [MZ3GroupItem] �^�u
--
function mz3_group_item.delete(tab);
*/
int lua_mz3_group_item_delete(lua_State *L)
{
	// �����擾
	const char* func_name = "mz3_group_item.delete";

	// �����擾
	CGroupItem* pTab = (CGroupItem*)lua_touserdata(L, 1);
	if (pTab==NULL) {
		// �s���ȃ^�u���w�肳�ꂽ�B
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
		return 0;
	}

	delete pTab;

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 MainView API
//-----------------------------------------------

/*
--- Twitter ���������ݗp���[�h�̕ύX(�������ݐ�URL/API���ʗp) : ������A�N�Z�X��ʂƓ����l
--
-- @param mode ���[�h�l
--
function mz3_main_view.set_post_mode(mode);
*/
int lua_mz3_main_view_set_post_mode(lua_State *L)
{
	const char* func_name = "mz3_main_view.set_post_mode";

	// �����̎擾
	int mode = lua_tointeger(L, 1);

	// ���[�h�ݒ�
	theApp.m_pMainView->m_twitterPostAccessType = (ACCESS_TYPE)mode;//(CMZ3View::TWITTER_STYLE_POST_MODE)mode;

	// ���o�[�W�����݊��p(TODO Lua ���̏C�����I�������폜���邱��)
	switch (theApp.m_pMainView->m_twitterPostAccessType) {
	case 0:
		//TWITTER_STYLE_POST_MODE_TWITTER_UPDATE		 = 0,	///< �^�C�����C���p�������͒�
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_TWITTER_UPDATE;
		break;

	case 1:
		//TWITTER_STYLE_POST_MODE_TWITTER_DM			 = 1,	///< DM���͒�
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_TWITTER_NEW_DM;
		break;

#ifdef BT_MZ3
	case 2:
		//TWITTER_STYLE_POST_MODE_MIXI_ECHO			 = 2,	///< mixi�G�R�[���͒�
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_MIXI_ADD_ECHO;
		break;

	case 3:
		//TWITTER_STYLE_POST_MODE_MIXI_ECHO_REPLY		 = 3,	///< mixi�G�R�[���͒�(�ԐM)
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_MIXI_ADD_ECHO_REPLY;
		break;

	case 4:
		//TWITTER_STYLE_POST_MODE_WASSR_UPDATE		 = 4,	///< Wassr �p�������͒�
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_WASSR_UPDATE;
		break;

	case 5:
		//TWITTER_STYLE_POST_MODE_GOOHOME_QUOTE_UPDATE = 5,	///< goo�z�[���ЂƂ��Ɠ��͒�
		theApp.m_pMainView->m_twitterPostAccessType = ACCESS_GOOHOME_QUOTE_UPDATE;
		break;
#endif
	}

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �R���g���[���̍Ĕz�u
--
function mz3_main_view.update_control_status(mode);
*/
int lua_mz3_main_view_update_control_status(lua_State *L)
{
	theApp.m_pMainView->MyUpdateControlStatus();

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �t�H�[�J�X�ړ�
--
-- @param focus_control �t�H�[�J�X�ړ���R���g���[��("edit", "category_list", "body_list")
--
function mz3_main_view.set_focus(focus_control);
*/
int lua_mz3_main_view_set_focus(lua_State *L)
{
	// �����̎擾
	CStringA focus_control(lua_tostring(L, 1));

	// �t�H�[�J�X�ړ�
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

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �t�H�[�J�X�擾
--
-- @return string �t�H�[�J�X�R���g���[��("edit", "category_list", "body_list", "other")
--
function mz3_main_view.get_focus();
*/
int lua_mz3_main_view_get_focus(lua_State *L)
{
	// �t�H�[�J�X�ړ�
	CWnd* pFocus = theApp.m_pMainView->GetFocus();
	if (pFocus->m_hWnd == theApp.m_pMainView->m_categoryList.m_hWnd) {
		lua_pushstring(L, "category_list");
	} else if (pFocus->m_hWnd == theApp.m_pMainView->m_bodyList.m_hWnd) {
		lua_pushstring(L, "body_list");
	} else if (pFocus->m_hWnd == theApp.m_pMainView->m_statusEdit.m_hWnd) {
		lua_pushstring(L, "edit");
	} else {
		lua_pushstring(L, "other");
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ݑI�𒆂̉��y�C���v�f�擾
--
function mz3_main_view.get_selected_body_item();
*/
int lua_mz3_main_view_get_selected_body_item(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	MZ3Data& data = theApp.m_pMainView->GetSelectedBodyItem();
	lua_pushlightuserdata(L, (void*)&data);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ݑI�𒆂̉��y�C���v�f�̃C���f�b�N�X�擾
--
function mz3_main_view.get_selected_body_item_idx();
*/
int lua_mz3_main_view_get_selected_body_item_idx(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		lua_pushnil(L);
	} else {
		int idx = pCategory->selectedBody;
		lua_pushinteger(L, idx);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���y�C���v�f�ꗗ�擾
--
function mz3_main_view.get_body_item_list();
*/
int lua_mz3_main_view_get_body_item_list(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		lua_pushnil(L);
	} else {
		MZ3DataList& list = pCategory->m_body;
		lua_pushlightuserdata(L, (void*)&list);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ݑI�𒆂̉��y�C���v�f��ύX����
--
-- @param idx �I��v�f�̃C���f�b�N�X
--
function mz3_main_view.select_body_item(idx);
*/
int lua_mz3_main_view_select_body_item(lua_State *L)
{
	int idx = lua_tointeger(L, 1);

	if (0 <= idx && idx <= theApp.m_pMainView->m_bodyList.GetItemCount()-1) {
		// �I��ύX
		util::MySetListCtrlItemFocusedAndSelected( theApp.m_pMainView->m_bodyList, idx, true );
		theApp.m_pMainView->m_bodyList.EnsureVisible(idx, FALSE);

		lua_pushboolean(L, 1);
	} else {
		// �͈͊O�̂���false��Ԃ�
		lua_pushboolean(L, 0);
	}


	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ݑI�𒆂̏�y�C���v�f�擾
--
function mz3_main_view.get_selected_category_item();
*/
int lua_mz3_main_view_get_selected_category_item(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		lua_pushnil(L);
	} else {
		MZ3Data& data = pCategory->m_mixi;
		lua_pushlightuserdata(L, (void*)&data);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- ���ݑI�𒆂̏�y�C���v�f�̎�ʎ擾
--
function mz3_main_view.get_selected_category_access_type();
*/
int lua_mz3_main_view_get_selected_category_access_type(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		lua_pushnil(L);
	} else {
		ACCESS_TYPE access_type = pCategory->m_mixi.GetAccessType();
		lua_pushinteger(L, access_type);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �J�e�S���̒ǉ�
--
-- @param title �^�C�g��
-- @param url   URL
-- @param key   �V���A���C�Y�L�[
--
function mz3_main_view.append_category(title, url, key);
*/
int lua_mz3_main_view_append_category(lua_State *L)
{
	// �����̎擾
	CString title = MyUTF82WCS2(lua_tostring(L, 1));
	CString url   = MyUTF82WCS2(lua_tostring(L, 2));
	const char* key = lua_tostring(L, 3);

	ACCESS_TYPE access_type = theApp.m_accessTypeInfo.getAccessTypeBySerializeKey(key);
	if (access_type==ACCESS_INVALID) {
		lua_pushstring(L, MyWCS2UTF8(L"�s���ȃV���A���C�Y�L�[�ł�"));
		lua_error(L);
		return 0;
	}

	// �ǉ�
	CMZ3View* pView = theApp.m_pMainView;

	CCategoryItem categoryItem;
	categoryItem.init( 
		// ���O
		title,
		// URL
		url, 
		access_type, 
		pView->m_selGroup->categories.size()+1,
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(access_type),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(access_type),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(access_type),
		CCategoryItem::SAVE_TO_GROUPFILE_YES );
	pView->AppendCategoryList(categoryItem);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ���C���r���[�̎擾
--
function mz3_main_view.get_wnd();
*/
int lua_mz3_main_view_get_wnd(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, (void*)theApp.m_pMainView);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- edit �G���A�ւ̕�����ݒ�
--
-- @param text �ݒ肷�镶����
--
function mz3_main_view.set_edit_text(text);
*/
int lua_mz3_main_view_set_edit_text(lua_State *L)
{
	// �����̎擾
	CString text = MyUTF82WCS2(lua_tostring(L, 1));

	// ������ݒ�
	theApp.m_pMainView->SetDlgItemText(IDC_STATUS_EDIT, text);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- edit �G���A�̕�����擾
--
function mz3_main_view.get_edit_text();
*/
int lua_mz3_main_view_get_edit_text(lua_State *L)
{
	// ������擾
	CString text;
	theApp.m_pMainView->GetDlgItemText(IDC_STATUS_EDIT, text);

	// �ݒ�
	lua_pushstring(L, MyWCS2UTF8(text));

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- info �G���A�ւ̕�����ݒ�
--
-- @param text �ݒ肷�镶����
--
function mz3_main_view.set_info_text(text);
*/
int lua_mz3_main_view_set_info_text(lua_State *L)
{
	// �����̎擾
	CString text = MyUTF82WCS2(lua_tostring(L, 1));

	// ������ݒ�
	util::MySetInformationText( theApp.m_pMainView->m_hWnd, text);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �J�e�S���̍ēǂݍ���
--
function mz3_main_view.retrieve_category_item();
*/
int lua_mz3_main_view_retrieve_category_item(lua_State *L)
{
	// ������ݒ�
	theApp.m_pMainView->RetrieveCategoryItem();

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ���y�C���A�C�R���̍ĕ`��
--
function mz3_main_view.redraw_body_images();
*/
int lua_mz3_main_view_redraw_body_images(lua_State *L)
{
	theApp.m_pMainView->MyRedrawBodyImages();

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 ReportView API
//-----------------------------------------------

/*
--- ���|�[�g�r���[�̎擾
--
function mz3_report_view.get_wnd();
*/
int lua_mz3_report_view_get_wnd(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, (void*)theApp.m_pReportView);

	// �߂�l�̐���Ԃ�
	return 1;
}


//-----------------------------------------------
// MZ3 WriteView API
//-----------------------------------------------

#ifdef BT_MZ3
/*
--- �������݉�ʂ̎擾
--
function mz3_write_view.get_wnd();
*/
int lua_mz3_write_view_get_wnd(lua_State *L)
{
	// ���ʂ��X�^�b�N�ɐς�
	lua_pushlightuserdata(L, (void*)theApp.m_pWriteView);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �������݉�ʂ̗v�f�̎擾
--
-- @param name �擾�������v�f��('title_edit', 'body_edit')
--
function mz3_write_view.get_text(name);
*/
int lua_mz3_write_view_get_text(lua_State *L)
{
	const char* func_name = "mz3_write_view.get_text";

	// �����̎擾
	CString name = MyUTF82WCS2(lua_tostring(L, 1));

	if (name=="title_edit") {
		CString s;
		theApp.m_pWriteView->GetDlgItemText(IDC_WRITE_TITLE_EDIT, s);

		lua_pushstring(L, MyWCS2UTF8(s));
	
	} else if (name=="body_edit") {
		CString s;
		theApp.m_pWriteView->GetDlgItemText(IDC_WRITE_BODY_EDIT, s);

		lua_pushstring(L, MyWCS2UTF8(s));
	
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
	}

	// �߂�l�̐���Ԃ�
	return 1;
}


/*
--- �������݉�ʂ̗v�f�̐ݒ�
--
-- @param name  �ݒ肵�����v�f��('title_edit', 'body_edit')
-- @param value �ݒ肵����������
--
function mz3_write_view.set_text(name, text);
*/
int lua_mz3_write_view_set_text(lua_State *L)
{
	const char* func_name = "mz3_write_view.set_text";

	// �����̎擾
	CString name  = MyUTF82WCS2(lua_tostring(L, 1));
	CString value = MyUTF82WCS2(lua_tostring(L, 2));

	if (name=="title_edit") {
		theApp.m_pWriteView->SetDlgItemText(IDC_WRITE_TITLE_EDIT, value);
	
	} else if (name=="body_edit") {
		theApp.m_pWriteView->SetDlgItemText(IDC_WRITE_BODY_EDIT, value);
	
	} else {
		lua_pushstring(L, make_invalid_arg_error_string(func_name));
		lua_error(L);
	}

	// �߂�l�̐���Ԃ�
	return 0;
}
#endif

//-----------------------------------------------
// MZ3 Graphics API
//-----------------------------------------------

/*
--- ������̕`��
--
function mz3_graphics.draw_text(dc, text, x, y, w, h, format);
*/
int lua_mz3_graphics_draw_text(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);
	CString text = MyUTF82WCS2(lua_tostring(L, 2));
	int x = lua_tointeger(L, 3);
	int y = lua_tointeger(L, 4);
	int w = lua_tointeger(L, 5);
	int h = lua_tointeger(L, 6);
	int format = lua_tointeger(L, 7);

	// ������̕`��
	CRect rect(x, y, x+w, y+h);
	pDC->DrawText(text, &rect, format);


	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �摜�̕`��
--
function mz3_graphics.draw_image(dc, image_cache_index, x, y, w, h);
*/
int lua_mz3_graphics_draw_image(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);
	int image_cache_index = lua_tointeger(L, 2);
	int x = lua_tointeger(L, 3);
	int y = lua_tointeger(L, 4);
	int w = lua_tointeger(L, 5);
	int h = lua_tointeger(L, 6);

	// �摜�̕`��
	CImageList* imageList = NULL;
	if (w >= 64) {
		imageList = &theApp.m_imageCache.GetImageList64();
	} else if (w >= 48) {
		imageList = &theApp.m_imageCache.GetImageList48();
	} else if (w >= 32) {
		imageList = &theApp.m_imageCache.GetImageList32();
	} else {
		imageList = &theApp.m_imageCache.GetImageList16();
	}
	imageList->Draw(pDC, image_cache_index, CPoint(x, y), ILD_NORMAL);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- �F�̕ύX
--
-- @param dc				  DC
-- @param type				  "text" or "bg"
-- @param color_or_color_name ������̏ꍇ�̓X�L���̃J���[���A���l�̏ꍇ��RGB�F
--
-- @return ���̐F
--
function mz3_graphics.set_color(dc, type, color_or_color_name);
*/
int lua_mz3_graphics_set_color(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);
	CStringA strColorType = lua_tostring(L, 2);
	COLORREF color = RGB(0, 0, 0);
	if (!lua_isnumber(L, 3)) {
		// ���O(�X�L����)����擾
		CStringA strColorName = lua_tostring(L, 3);
		color = theApp.m_skininfo.getColor(strColorName);
	} else {
		color = (COLORREF) lua_tointeger(L, 3);
	}

	// �F�̕ύX
	COLORREF clrOrg;
	if (strColorType == "text") {
		clrOrg = pDC->SetTextColor(color);
	} else {
		clrOrg = pDC->SetBkColor(color);
	}

	lua_pushnumber(L, clrOrg);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �s�̍���
--
-- @param dc DC
--
-- @return �s�̍���(px)
--
function mz3_graphics.get_line_height(dc);
*/
int lua_mz3_graphics_get_line_height(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);

	// �t�H���g�̍����擾
	CSize charSize = pDC->GetTextExtent(CString(L"��"));
	int lfHeightPx = charSize.cy;
	lua_pushnumber(L, lfHeightPx);

	// �߂�l�̐���Ԃ�
	return 1;
}

/*
--- �t�H���g�T�C�Y
--
-- @param dc   DC
-- @param size 1:��A0:�W���A-1:��
--
function mz3_graphics.set_font_size(dc, size);
*/
int lua_mz3_graphics_set_font_size(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);
	int size = lua_tointeger(L, 2);

	// �t�H���g�ύX
	CFont* pFont = NULL;
	switch (size) {
	case -1:
		pFont = &theApp.m_fontSmall;
		break;
	case 1:
		pFont = &theApp.m_fontBig;
		break;
	case 0:
	default:
		pFont = &theApp.m_font;
		break;
	}

	pDC->SelectObject(pFont);

	// �߂�l�̐���Ԃ�
	return 0;
}

/*
--- ��`�̕`��
--
-- @param dc				  DC
-- @param type				  "border" or "fill"
-- @param x					  x
-- @param y					  y
-- @param w					  w
-- @param h					  h
-- @param color_or_color_name ������̏ꍇ�̓X�L���̃J���[���A���l�̏ꍇ��RGB�F
--
function mz3_graphics.draw_rect(dc, type, x, y, w, h, color_or_color_name);
*/
int lua_mz3_graphics_draw_rect(lua_State *L)
{
	// �����̎擾
	CDC* pDC = (CDC*)lua_touserdata(L, 1);
	CStringA strDrawType = lua_tostring(L, 2);
	int x = lua_tointeger(L, 3);
	int y = lua_tointeger(L, 4);
	int w = lua_tointeger(L, 5);
	int h = lua_tointeger(L, 6);
	COLORREF color = RGB(0, 0, 0);
	if (!lua_isnumber(L, 7)) {
		// ���O(�X�L����)����擾
		CStringA strColorName = lua_tostring(L, 7);
		color = theApp.m_skininfo.getColor(strColorName);
	} else {
		color = (COLORREF) lua_tointeger(L, 7);
	}

	// ��`�̕`��
	CRect rect(x, y, x+w, y+h);
	if (strDrawType == "border") {
		CPen pen(PS_SOLID, 1, color);
		CPen* pOldPen = pDC->SelectObject(&pen);
		
		pDC->MoveTo(rect.left, rect.top);
		pDC->LineTo(rect.right, rect.top);		// ���
		pDC->LineTo(rect.right, rect.bottom);	// �E��
		pDC->LineTo(rect.left, rect.bottom);	// ����
		pDC->LineTo(rect.left, rect.top);		// ����

		pDC->SelectObject(pOldPen);
	} else {
		pDC->FillSolidRect(&rect, color);
	}

	// �߂�l�̐���Ԃ�
	return 0;
}


//-----------------------------------------------
// MZ3 API table
//-----------------------------------------------
static const luaL_Reg lua_mz3_lib[] = {
	{"get_app_name",						lua_mz3_get_app_name},
	{"get_app_build_type",					lua_mz3_get_app_build_type},
	{"get_app_version",						lua_mz3_get_app_version},
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
	{"get_last_request_url",				lua_mz3_get_last_request_url},
	{"keybd_event",							lua_mz3_keybd_event},
	{"open_url_by_browser_with_confirm",	lua_mz3_open_url_by_browser_with_confirm},
	{"open_url_by_browser",					lua_mz3_open_url_by_browser},
	{"get_open_file_name",					lua_mz3_get_open_file_name},
	{"camera_capture",						lua_mz3_camera_capture},
	{"show_common_edit_dlg",				lua_mz3_show_common_edit_dlg},
	{"url_encode",							lua_mz3_url_encode},
	{"convert_encoding",					lua_mz3_convert_encoding},
	{"make_image_logfile_path_from_url_md5",lua_mz3_make_image_logfile_path_from_url_md5},
	{"copy_file",							lua_mz3_copy_file},
	{"change_view",							lua_mz3_change_view},
#ifdef BT_MZ3
	{"is_mixi_logout",						lua_mz3_is_mixi_logout},
#endif
	{"start_write_view",					lua_mz3_start_write_view},
	{"show_detail_view",					lua_mz3_show_detail_view},
	{"set_vib_status",						lua_mz3_set_vib_status},
	{"get_text_length",						lua_mz3_get_text_length},
	{"exec_mz3_command",					lua_mz3_exec_mz3_command},
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
	{"get_integer64_as_string",			lua_mz3_data_get_integer64_as_string},
	{"set_integer64_from_string",		lua_mz3_data_set_integer64_from_string},
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
	{"get_all_text",	lua_mz3_htmlarray_get_all_text},
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
	{"set_cruise_target",	lua_mz3_access_type_info_set_cruise_target},
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
	{"get_focus",				lua_mz3_main_view_get_focus},
	{"get_selected_body_item",	lua_mz3_main_view_get_selected_body_item},
	{"get_selected_body_item_idx",	lua_mz3_main_view_get_selected_body_item_idx},
	{"get_body_item_list",		lua_mz3_main_view_get_body_item_list},
	{"select_body_item",		lua_mz3_main_view_select_body_item},
	{"get_selected_category_item",	lua_mz3_main_view_get_selected_category_item},
	{"get_selected_category_access_type",	lua_mz3_main_view_get_selected_category_access_type},
	{"append_category",			lua_mz3_main_view_append_category},
	{"get_wnd",					lua_mz3_main_view_get_wnd},
	{"set_edit_text",			lua_mz3_main_view_set_edit_text},
	{"get_edit_text",			lua_mz3_main_view_get_edit_text},
	{"set_info_text",			lua_mz3_main_view_set_info_text},
	{"retrieve_category_item",	lua_mz3_main_view_retrieve_category_item},
	{"redraw_body_images",		lua_mz3_main_view_redraw_body_images},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_report_view_lib[] = {
	{"get_wnd",					lua_mz3_report_view_get_wnd},
	{NULL, NULL}
};
#ifdef BT_MZ3
static const luaL_Reg lua_mz3_write_view_lib[] = {
	{"get_text",				lua_mz3_write_view_get_text},
	{"set_text",				lua_mz3_write_view_set_text},
	{"get_wnd",					lua_mz3_write_view_get_wnd},
	{NULL, NULL}
};
#endif
static const luaL_Reg lua_mz3_post_data_lib[] = {
	{"create",					lua_mz3_post_data_create},
	{"set_content_type",		lua_mz3_post_data_set_content_type},
	{"append_post_body",		lua_mz3_post_data_append_post_body},
	{"append_additional_header",lua_mz3_post_data_append_additional_header},
	{"append_file",				lua_mz3_post_data_append_file},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_account_provider_lib[] = {
	{"set_param",				lua_mz3_account_provider_set_param},
	{"get_value",				lua_mz3_account_provider_get_value},
	{"set_value",				lua_mz3_account_provider_set_value},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_image_cache_lib[] = {
	{"get_image_index_by_url",	lua_mz3_image_cache_get_image_index_by_url},
	{NULL, NULL}
};
static const luaL_Reg lua_mz3_graphics_lib[] = {
	{"draw_text",				lua_mz3_graphics_draw_text},
	{"draw_image",				lua_mz3_graphics_draw_image},
	{"set_color",				lua_mz3_graphics_set_color},
	{"draw_rect",				lua_mz3_graphics_draw_rect},
	{"get_line_height",			lua_mz3_graphics_get_line_height},
	{"set_font_size",			lua_mz3_graphics_set_font_size},
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
	luaL_register(L, "mz3_report_view", lua_mz3_report_view_lib);
#ifdef BT_MZ3
	luaL_register(L, "mz3_write_view", lua_mz3_write_view_lib);
#endif
	luaL_register(L, "mz3_post_data", lua_mz3_post_data_lib);
	luaL_register(L, "mz3_account_provider", lua_mz3_account_provider_lib);
	luaL_register(L, "mz3_image_cache", lua_mz3_image_cache_lib);
	luaL_register(L, "mz3_graphics", lua_mz3_graphics_lib);
}
