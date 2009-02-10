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
-- �p�[�T���[�h
----------------------------------------
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");

----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- goo�z�[�� �F�B�E���ڂ̐l�̂ЂƂ��ƈꗗ
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");

----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
goohome_user_read_menu_item = mz3_menu.regist_menu("goohome.on_read_menu_item");

----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------
-- TODO ����
function on_event(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
		-- �S���\��
		return on_read_menu_item(serialize_key, event_name,data);
	end
	
	-- �W���̏����𑱍s
	return false;
end

-- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	-- �{����1�s�ɕϊ����ĕ\��
	item = '';
	for i=0, data:get_text_array_size('body')-1 do
		item = item .. data:get_text_array('body', i);
	end
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

-- �{�f�B���X�g�̃N���b�N�E�_�u���N���b�N�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "goohome.on_event");
mz3.add_event_listener("enter_body_list",  "goohome.on_event");

-- �b��̃C�x���g�ł��B
mz3.add_event_listener("creating_goohome_user_context_menu",  "goohome.on_creating_goohome_user_context_menu");

mz3.logger_debug('goohome.lua end');
