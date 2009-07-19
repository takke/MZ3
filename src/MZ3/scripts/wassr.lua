--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : wassr
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('wassr.lua start');
module("wassr", package.seeall)

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('Wassr', false);

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('Wassr', 'id_name', 'ID');
mz3_account_provider.set_param('Wassr', 'password_name', '�p�X���[�h');


----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- TODO

----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("wassr.on_read_menu_item");
menu_items.update                = mz3_menu.regist_menu("wassr.on_wassr_update");


----------------------------------------
-- �T�[�r�X�p�֐�
----------------------------------------


----------------------------------------
-- �p�[�T
----------------------------------------

--------------------------------------------------
-- [list] �^�C�����C���p�p�[�T
--
-- http://api.wassr.jp/statuses/friends_timeline.xml
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function wassr_friends_timeline_parser(parent, body, html)
	mz3.logger_debug("wassr_friends_timeline_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S�������Ȃ�
--	body:clear();
	
	-- �d���h�~�p�� id �ꗗ�𐶐��B
	id_set = {};
	n = body:get_count();
	for i=0, n-1 do
		id = mz3_data.get_integer(body:get_data(i), 'id');
--		mz3.logger_debug(id);
		id_set[ "" .. id ] = true;
	end

	local t1 = mz3.get_tick_count();
	
	-- �ꎞ���X�g
	new_list = MZ3DataList:create();
	
	line = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = line .. html:get_at(i);
	end
	
	while true do
		status, after = line:match('<status>(.-)</status>(.*)$');
		if status == nil or after == nil then
			break;
		end
		line = after;

		-- id : status/id
		id = status:match('<id>(.-)</id>');
		
		-- ����ID������Βǉ����Ȃ��B
		if id_set[ id ] then
--			mz3.logger_debug('id[' .. id .. '] �͊��ɑ��݂���̂�skip����');
		else
			-- data ����
			data = MZ3Data:create();
			
			data:set_integer('id', id);
			
			type = mz3.get_access_type_by_key('WASSR_USER');
			data:set_access_type(type);
			
			-- text : status/text
			text = status:match('<text>(.-)</text>');
			text = text:gsub('&amp;', '&');
			text = mz3.decode_html_entity(text);
			data:add_text_array('body', text);
			
			-- name : status/user/screen_name
			user = status:match('<user>.-</user>');
			s = user:match('<screen_name>(.-)</screen_name>');
			s = s:gsub('&amp;', '&');
			s = mz3.decode_html_entity(s);
			data:set_text('name', s);
			
			-- owner-id : status/user/id
			data:set_integer('owner_id', status:match('<user_login_id>(.-)</user_login_id>'));

			-- URL : status/user/url
			url = user:match('<url>(.-)</url>');
			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- Image : status/user/profile_image_url
			profile_image_url = user:match('<profile_image_url>(.-)</profile_image_url>');
			profile_image_url = mz3.decode_html_entity(profile_image_url);
--			mz3.logger_debug(profile_image_url);

			-- �t�@�C�����݂̂�URL�G���R�[�h
--			int idx = strImage.ReverseFind( '/' );
--			if (idx >= 0) {
--				CString strFileName = strImage.Mid( idx +1 );
--				strFileName = URLEncoder::encode_utf8( strFileName );
--				strImage = strImage.Left(idx + 1);
--				strImage += strFileName;
--			}
			data:add_text_array('image', profile_image_url);

			-- updated : status/epoch
			s = status:match('<epoch>(.-)</epoch>');
			data:parse_date_line(s);
			
			-- URL �𒊏o���A�����N�ɂ���
			for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
				data:add_link_list(url, url);
			end

			-- �ꎞ���X�g�ɒǉ�
			new_list:add(data.data);
			
			-- data �폜
			data:delete();
		end
	end
	
	-- ���������f�[�^���o�͂ɔ��f
	body:merge(new_list);

	new_list:delete();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("wassr_friends_timeline_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("WASSR_FRIENDS_TIMELINE", "wassr.wassr_friends_timeline_parser");


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------


--- BASIC �F�ؐݒ�
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Wassr' then
		id       = mz3_account_provider.get_value('Wassr', 'id');
		password = mz3_account_provider.get_value('Wassr', 'password');
		
		if id=='' or password=='' then
			mz3.alert('���O�C���ݒ��ʂŃ��[�UID�ƃp�X���[�h��ݒ肵�Ă�������');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "wassr.on_set_basic_auth_account");


--- Twitter���������݃��[�h�̏�����
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Wassr' then
		-- ���[�h�ύX
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('WASSR_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "wassr.on_reset_twitter_style_post_mode");


--- Twitter�X�^�C���̃{�^�����̂̍X�V
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'WASSR_UPDATE' then
		return true, 'Wassr';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "wassr.on_update_twitter_update_button");


-- �u�Ԃ₭�v���j���[�p�n���h��
function on_wassr_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_WASSR_UPDATE);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end


--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="WASSR_USER" then
		-- �S���\��
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- �W���̏����𑱍s
	return false;
end
mz3.add_event_listener("dblclk_body_list", "wassr.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "wassr.on_body_list_click");


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
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. data:get_integer('id') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
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
	if services:find(' Wassr', 1, true) ~= nil then

		-- Wassr�^�u�ǉ�
		local tab = MZ3GroupItem:create("Wassr");
		tab:append_category("�^�C�����C��", "WASSR_FRIENDS_TIMELINE");
		tab:append_category("�ԐM�ꗗ", "WASSR_FRIENDS_TIMELINE", "http://api.wassr.jp/statuses/replies.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end
mz3.add_event_listener("creating_default_group", "wassr.on_creating_default_group", false);


--- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key �{�f�B�A�C�e���̃V���A���C�Y�L�[
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="WASSR_USER" then
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
mz3.add_event_listener("popup_body_menu",  "wassr.on_popup_body_menu");


--- ViewStyle �ύX
--
-- @param event_name    'get_view_style'
-- @param serialize_key �J�e�S���̃V���A���C�Y�L�[
--
-- @return (1) [bool] �������� true, ���s���� false
-- @return (2) [int] VIEW_STYLE_*
--
function on_get_view_style(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Wassr' then
		return true, VIEW_STYLE_TWITTER;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "wassr.on_get_view_style");


mz3.logger_debug('wassr.lua end');
