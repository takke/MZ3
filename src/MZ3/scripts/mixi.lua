--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi.lua start');
module("mixi", package.seeall)

--------------------------------------------------
-- ���ցA�O�ւ̒��o����
--------------------------------------------------
function parse_next_back_link(line, base_url)

	local back_data = nil;
	local next_data = nil;
	
	-- <ul><li><a href="new_bbs.pl?page=1">�O��\��</a></li>
	-- <li>51���`100����\��</li>
	-- <li><a href="new_bbs.pl?page=3">����\��</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- �O
		local url, t = line:match([[href="([^"]+)">(�O[^<]+)<]]);
		if url~=nil then
			back_data = MZ3Data:create();
			back_data:set_text("title", "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- ��
		local url, t = line:match([[href="([^"]+)">(��[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text("title", "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end


----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- �����X�g�n
-- �R�~���j�e�B�ŐV�����ꗗ
require("scripts\\mixi\\mixi_new_bbs_parser");
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- �R�~���j�e�B�R�����g�L������ : �ŐV�����ꗗ�Ɠ���
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");

-- �g�b�v�y�[�W
require("scripts\\mixi\\mixi_home_parser");
mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");

-- ���b�Z�[�W(��M��, ���M��), �������b�Z�[�W, ���b�Z�[�W�ڍ�
require("scripts\\mixi\\mixi_new_official_message_parser");
require("scripts\\mixi\\mixi_message_outbox_parser");
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- �t��������
require("scripts\\mixi\\mixi_show_self_log_parser");


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.mixi_echo_item_read = mz3_menu.regist_menu("mixi.on_read_menu_item");
menu_items.mixi_echo_update    = mz3_menu.regist_menu("mixi.on_mixi_echo_update");
menu_items.mixi_echo_reply     = mz3_menu.regist_menu("mixi.on_mixi_echo_reply");


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

-- �u�Ԃ₭�v���j���[�p�n���h��
function on_mixi_echo_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

-- �u�ԐM�v���j���[�p�n���h��
function on_mixi_echo_reply(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="MIXI_RECENT_ECHO_ITEM" then
		-- �S���\��
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- �W���̏����𑱍s
	return false;
end

--- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	-- �{����1�s�ɕϊ����ĕ\��
	item = data:get_text_array_joined_text('body');
	item = item:gsub("\r\n", "");
	
	item = item .. "\r\n";
	item = item .. "----\r\n";
	item = item .. "name : " .. data:get_text('name') .. "\r\n";
	item = item .. data:get_date();

	mz3.alert(item, data:get_text('name'));

	return true;
end

--- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key �{�f�B�A�C�e���̃V���A���C�Y�L�[
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="MIXI_RECENT_ECHO_ITEM" then
		return false;
	end
	
	-- �C���X�^���X��
	body = MZ3Data:create(body);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	
	-- TODO �e���j���[�A�C�e���̃��\�[�X�l��萔��(�܂���Lua�֐���)
	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�S����ǂ�...", menu_items.mixi_echo_item_read);
	menu:append_menu("separator", "", 0);
	menu:append_menu("string", "�Ԃ₭", menu_items.mixi_echo_update);
	menu:append_menu("string", "�ԐM", menu_items.mixi_echo_reply);
	menu:append_menu("string", "�v���t�B�[���y�[�W", 34170 -37000);				-- 34170 : ID_MENU_MIXI_ECHO_SHOW_PROFILE
	menu:append_menu("separator", "", 0);
	
	-- TODO �����N
	--[[ C++ �ŁF
	// �����N
	int n = (int)bodyItem.m_linkList.size();
	if( n > 0 ) {
		pSubMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
		for( int i=0; i<n; i++ ) {
			// �ǉ�
			CString s;
			s.Format( L"link : %s", bodyItem.m_linkList[i].text );
			pSubMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
		}
	}
	]]

	-- ���[�U�̃G�R�[�ꗗ
	-- 34192 : ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST
	menu:append_menu("string", body:get_text('name') .. " ����̃G�R�[", 34192 -37000);

	-- ���p���[�U�̃G�R�[�ꗗ
	ref_user_name = body:get_text('ref_user_name');
	if ref_user_name ~= "" then
		-- 34193 : ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST
		menu:append_menu("string", ref_user_name .. " ����̃G�R�[", 34193 -37000);
	end

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	
	return true;
end


----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------

-- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");

-- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\���C�x���g�n���h���o�^
mz3.add_event_listener("popup_body_menu",  "mixi.on_popup_body_menu");

mz3.logger_debug('mixi.lua end');
