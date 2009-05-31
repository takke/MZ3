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

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('goo�z�[��', 'id_name', 'gooID');
mz3_account_provider.set_param('goo�z�[��', 'password_name', 'goo�z�[�� �ЂƂ��ƃ��[�����e�A�h���X');


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read          = mz3_menu.regist_menu("goohome.on_read_menu_item");
menu_items.read_comments = mz3_menu.regist_menu("goohome.on_read_comments_menu_item");
menu_items.update        = mz3_menu.regist_menu("goohome.on_goohome_update");


-- �ЂƂ��Ɠ��e�A�h���X����API KEY���擾����
function get_api_key_from_quote_mail_address(address)
	key = address:match('^quote%-([0-9a-zA-Z-_]*)@home%.goo%.ne%.jp$');
	if key==nil or string.len(key)~=12 then
		return '';
	end
	
	return key;
end

-- "quote-XXXXXXXXXXXX@home.goo.ne.jp" �̌`���ł��邱�Ƃ��m�F����
function is_valid_quote_mail_address(password)
	return get_api_key_from_quote_mail_address(password)~='';
end

----------------------------------------
-- �C�x���g�n���h��
----------------------------------------


--- ���[�U���̓A�J�E���g�`�F�b�N
function on_check_account(event_name, service_name, id, password)
--	mz3.alert(service_name);
	if service_name == 'goo�z�[��' then
		if password ~= '' and is_valid_quote_mail_address(password)==false then
			msg = "goo�z�[���ЂƂ��Ɠ��e�A�h���X�͉��L�̌`���ł��B\n"
			    .." quote-XXXXXXXXXXXX@home.goo.ne.jp\n"
			    .."���͂��ꂽ�A�h���X�F\n"
			    .." " .. password
			    .."\n"
			    .."�m�F����URL���J���܂����H";
			if (mz3.confirm(msg, nil, 'yes_no')=='yes') then
				url = "http://home.goo.ne.jp/config/quote";
				mz3.open_url_by_browser(url);
			end

			return true;
		end
	end
	return false;
end
mz3.add_event_listener("check_account", "goohome.on_check_account");


--- BASIC �F�ؐݒ�
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gooHome' then
		id       = mz3_account_provider.get_value('goo�z�[��', 'id');
		password = mz3_account_provider.get_value('goo�z�[��', 'password');
		
		password = get_api_key_from_quote_mail_address(password);
		
		if id=='' or password=='' then
			mz3.alert('���O�C���ݒ��ʂ�gooID�ƂЂƂ��Ɠ��e�A�h���X��ݒ肵�Ă�������');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "goohome.on_set_basic_auth_account");


--- Twitter���������݃��[�h�̏�����
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gooHome' then
		-- ���[�h�ύX
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('GOOHOME_QUOTE_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "goohome.on_reset_twitter_style_post_mode");


--- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
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


--- �R�����g��ǂ� ���j���[�p�n���h��
function on_read_comments_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_comments_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	mz3.open_url_by_browser_with_confirm(data:get_text('url'));

	return true;
end


-- �u�Ԃ₭�v���j���[�p�n���h��
function on_goohome_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end


--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
		-- �S���\��
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- �W���̏����𑱍s
	return false;
end
mz3.add_event_listener("dblclk_body_list", "goohome.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "goohome.on_body_list_click");


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
mz3.add_event_listener("creating_default_group", "goohome.on_creating_default_group", false);


--- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key �{�f�B�A�C�e���̃V���A���C�Y�L�[
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="GOOHOME_USER" then
		return false;
	end

	-- �C���X�^���X��
	body = MZ3Data:create(body);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�S����ǂ�...", menu_items.read);

	menu:append_menu("separator");

	menu:append_menu("string", "�R�����g��ǂ�...", menu_items.read_comments);
	menu:append_menu("string", "�Ԃ₭", menu_items.update);

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
mz3.add_event_listener("popup_body_menu",  "goohome.on_popup_body_menu");


----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- goo�z�[�� �F�B�E���ڂ̐l�̂ЂƂ��ƈꗗ
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");


mz3.logger_debug('goohome.lua end');
