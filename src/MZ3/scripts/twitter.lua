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
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.retweet               = mz3_menu.regist_menu("twitter.on_retweet_menu_item");
menu_items.update                = 34016 - 37000;	-- ID_MENU_TWITTER_UPDATE
menu_items.reply                 = 34014 - 37000;	-- ID_MENU_TWITTER_REPLY
menu_items.new_dm                = 34143 - 37000;	-- ID_MENU_TWITTER_NEW_DM
menu_items.create_favourings     = 34146 - 37000;	-- ID_MENU_TWITTER_CREATE_FAVOURINGS
menu_items.destroy_favourings    = 34147 - 37000;	-- ID_MENU_TWITTER_DESTROY_FAVOURINGS
menu_items.create_friendships    = 34151 - 37000;	-- ID_MENU_TWITTER_CREATE_FRIENDSHIPS
menu_items.destroy_friendships   = 34152 - 37000;	-- ID_MENU_TWITTER_DESTROY_FRIENDSHIPS
menu_items.friend_timeline       = 34025 - 37000;	-- ID_MENU_TWITTER_FRIEND_TIMELINE
menu_items.open_home             = 34020 - 37000;	-- ID_MENU_TWITTER_HOME
menu_items.open_friend_favorites = 34021 - 37000;	-- ID_MENU_TWITTER_FAVORITES
menu_items.open_friend_site      = 34022 - 37000;	-- ID_MENU_TWITTER_SITE


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �uReTweet�v���j���[�p�n���h��
function on_retweet_menu_item(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �G�f�B�b�g�R���g���[���ɕ�����ݒ�
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	text = "RT @" .. data:get_text('name') .. ": " .. data:get_text_array_joined_text('body');
	text = text:gsub("\r\n", "");
	mz3_main_view.set_edit_text(text);

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

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
	item = data:get_text_array_joined_text('body');
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
	if s_url ~= nil then
		item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";
	else
		item = item .. "source : " .. source .. "\r\n";
	end

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
	if serialize_key~="TWITTER_USER" then
		return false;
	end

	-- �C���X�^���X��
	body = MZ3Data:create(body);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�S����ǂ�...", menu_items.read);

	menu:append_menu("separator");

	menu:append_menu("string", "�Ԃ₭", menu_items.update);

	name = body:get_text('name');

	menu:append_menu("string", "@" .. name .. " ����ɕԐM", menu_items.reply);
	menu:append_menu("string", "@" .. name .. " ����Ƀ��b�Z�[�W���M", menu_items.new_dm);

	menu:append_menu("separator");

	menu:append_menu("string", "ReTweet...", menu_items.retweet);
	
	-- �J�e�S�����utimeline�v�̂݁A���C�ɓ���o�^��\��
	category_access_type = mz3_main_view.get_selected_category_access_type();
	category_key = mz3.get_serialize_key_by_access_type(category_access_type);
	if category_key == "TWITTER_FRIENDS_TIMELINE" then
		menu:append_menu("string", "���C�ɓ���o�^�i�ӂ��ڂ�j", menu_items.create_favourings);
	end
	
	-- �J�e�S�����u���C�ɓ���v�̂݁A���C�ɓ���폜��\��
	if category_key == "TWITTER_FAVORITES" then
		menu:append_menu("string", "���C�ɓ���폜", menu_items.destroy_favourings);
	end

	menu:append_menu("string", "�t�H���[����", menu_items.create_friendships);
	menu:append_menu("string", "�t�H���[��߂�", menu_items.destroy_friendships);

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " �̃^�C�����C��", menu_items.friend_timeline);

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " �̃z�[�����u���E�U�ŊJ��", menu_items.open_home);
	menu:append_menu("string", "@" .. name .. " ��Favorites���u���E�U�ŊJ��", menu_items.open_friend_favorites);

	-- URL ����łȂ���΁u�T�C�g�v��ǉ�
	url = body:get_text('url');
	if url~=nil and url:len()>0 then
		menu:append_menu("string", "@" .. name .. " �̃T�C�g���u���E�U�ŊJ��", menu_items.open_friend_site);
	end

	-- �����N�ǉ�
	n = body:get_link_list_size();
	if n > 0 then
		menu:append_menu("separator");
		for i=0, n-1 do
			id = ID_REPORT_URL_BASE+(i+1);
			menu:append_menu("string", "link : " .. body:get_link_list_text(i), id);
		end
	end

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	
	return true;
end

----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");

-- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\���C�x���g�n���h���o�^
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");

mz3.logger_debug('twitter.lua end');
