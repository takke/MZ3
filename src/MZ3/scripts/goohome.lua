--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : goohome
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('goohome.lua start');
module("goohome", package.seeall)

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('goohome', false);

----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- goo�z�[�� �F�B�E���ڂ̐l�̂ЂƂ��ƈꗗ
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");

----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
goohome_user_read_menu_item = mz3_menu.regist_menu("goohome.on_read_menu_item");

----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
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

-- �b��̃C�x���g�Fgoo�z�[���p�R���e�L�X�g���j���[�̍쐬
function on_creating_goohome_user_context_menu(serialize_key, event_name, menu)
	-- "�S����ǂ�" ��ǉ�
	mz3_menu.insert_menu(menu, 2, "�S����ǂ�...", goohome_user_read_menu_item);
end


--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' goohome', 1, true) ~= nil then

		-- Wassr�^�u�ǉ�
		local tab = MZ3GroupItem:create("goo�z�[��");
		tab:append_category("�F�B�E���ڂ̐l", "GOOHOME_QUOTE_QUOTES_FRIENDS");
		tab:append_category("�����̂ЂƂ��ƈꗗ", "GOOHOME_QUOTE_QUOTES_FRIENDS", "http://home.goo.ne.jp/api/quote/quotes/myself/json");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end


----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �{�f�B���X�g�̃N���b�N�E�_�u���N���b�N�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "goohome.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "goohome.on_body_list_click");

-- �b��̃C�x���g�ł��B
mz3.add_event_listener("creating_goohome_user_context_menu",  "goohome.on_creating_goohome_user_context_menu");

-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "goohome.on_creating_default_group", false);

mz3.logger_debug('goohome.lua end');
