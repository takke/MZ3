--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : twitter
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('twitter.lua start');
module("twitter", package.seeall)

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
--[[
type = mz3.get_access_type_by_key('TWITTER_FRIENDS_TIMELINE');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '<small>%2 \t(%3)</small>');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '<small>%2 \t(%3)</small>');
]]


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
twitter_item_read_menu_item = mz3_menu.regist_menu("twitter.on_read_menu_item");


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="TWITTER_USER" then
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
	item = '';
	for i=0, data:get_text_array_size('body')-1 do
		item = item .. data:get_text_array('body', i);
	end
	item = item:gsub("\r\n", "");
	
	item = item .. "\r\n";
	item = item .. "----\r\n";
	item = item .. "name : " .. data:get_text('name') .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. data:get_integer('id') .. "\r\n";
	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";
	
	-- �\�[�X
	source = data:get_text('source');
--	item = item .. "source : " .. source .. "\r\n";
	s_url, s_name = source:match("href=\"(.*)\".*>(.*)<");
	item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";

	mz3.alert(item, data:get_text('name'));

	return true;
end

-- �b��̃C�x���g�FTwitter�p�R���e�L�X�g���j���[�̍쐬
function on_creating_twitter_item_context_menu(serialize_key, event_name, menu)
	-- "�S����ǂ�" ��ǉ�
	mz3_menu.insert_menu(menu, 2, "�S����ǂ�...", twitter_item_read_menu_item);
end

----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");

-- �b��̃C�x���g�ł��B
mz3.add_event_listener("creating_twitter_item_context_menu",  "twitter.on_creating_twitter_item_context_menu");

mz3.logger_debug('twitter.lua end');
