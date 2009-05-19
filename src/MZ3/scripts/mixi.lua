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

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('mixi', true);

--------------------------------------------------
--- ���ցA�O�ւ̒��o����
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

--------------------------------------------------
--- mixi �pURL�⊮
--------------------------------------------------
function complement_mixi_url(url)
	if (url:find("mixi.jp", 1, true) == nil and
	    url:find("http://", 1, true) == nil) then
	    url = "http://mixi.jp/" .. url;
	end
	
	return url;
end


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.mixi_echo_item_read    = mz3_menu.regist_menu("mixi.on_mixi_echo_read_menu_item");
menu_items.mixi_echo_update       = mz3_menu.regist_menu("mixi.on_mixi_echo_update");
menu_items.mixi_echo_reply        = mz3_menu.regist_menu("mixi.on_mixi_echo_reply");
menu_items.mixi_echo_show_profile = mz3_menu.regist_menu("mixi.on_mixi_echo_show_profile");

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

-- �u�v���t�B�[���v���j���[�p�n���h��
function on_mixi_echo_show_profile(serialize_key, event_name, data)

	-- �I�𒆂̗v�f�擾
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	
	-- URL �擾
	url = complement_mixi_url(data:get_text('url'));
--	mz3.alert(url);
	
	-- �v���t�B�[���擾�A�N�Z�X�J�n
	access_type = mz3.get_access_type_by_key("MIXI_PROFILE");
	referer = "";
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

	-- �\����ԍX�V
	mz3_main_view.update_control_status();

end

--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="MIXI_RECENT_ECHO_ITEM" then
		-- �S���\��
		return on_mixi_echo_read_menu_item(serialize_key, event_name, data);
	end
	
	-- �W���̏����𑱍s
	return false;
end

--- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_mixi_echo_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_mixi_echo_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
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
	
	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�S����ǂ�...", menu_items.mixi_echo_item_read);
	menu:append_menu("separator");
	menu:append_menu("string", "�Ԃ₭", menu_items.mixi_echo_update);
	menu:append_menu("string", "�ԐM", menu_items.mixi_echo_reply);
	menu:append_menu("string", body:get_text('name') .. " ����̃v���t�B�[��", menu_items.mixi_echo_show_profile);
	menu:append_menu("separator");

	-- TODO �e���j���[�A�C�e���̃��\�[�X�l��萔��(�܂���Lua�֐���)

	-- ���[�U�̃G�R�[�ꗗ
	ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST = 34192 -37000;
	menu:append_menu("string", body:get_text('name') .. " ����̃G�R�[", ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST);

	-- ���p���[�U�̃G�R�[�ꗗ
	ref_user_name = body:get_text('ref_user_name');
	if ref_user_name ~= "" then
		ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST = 34193 -37000;
		menu:append_menu("string", ref_user_name .. " ����̃G�R�[", ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST);
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

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then

		-- �e��^�u�ǉ�

		-- ���L
		local tab = MZ3GroupItem:create("���L");
		tab:append_category("�ŋ߂̓��L", "MYDIARY");
		tab:append_category("�ŋ߂̃R�����g", "COMMENT");
		tab:append_category("�}�C�~�N�ŐV���L", "DIARY");
		tab:append_category("���L�R�����g�L������", "NEW_COMMENT");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- �R�~���j�e�B
		local tab = MZ3GroupItem:create("�R�~���j�e�B");
		tab:append_category("�ŐV�������݈ꗗ", "BBS");
		tab:append_category("�R�~���R�����g����", "NEW_BBS_COMMENT");
		tab:append_category("�R�~���j�e�B�ꗗ", "COMMUNITY");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- �j���[�X
		local tab = MZ3GroupItem:create("�j���[�X");
		tab:append_category("���ڂ̃s�b�N�A�b�v",	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn");
		tab:append_category("����",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1");
		tab:append_category("����",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=2&type=bn&sort=1");
		tab:append_category("�o��",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=3&type=bn&sort=1");
		tab:append_category("�n��",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=4&type=bn&sort=1");
		tab:append_category("�C�O",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=5&type=bn&sort=1");
		tab:append_category("�X�|�[�c",			  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=6&type=bn&sort=1");
		tab:append_category("�G���^�[�e�C�������g",	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=7&type=bn&sort=1");
		tab:append_category("IT",					"NEWS", "http://news.mixi.jp/list_news_category.pl?id=8&type=bn&sort=1");
		tab:append_category("�Q�[���E�A�j��",		"NEWS", "http://news.mixi.jp/list_news_category.pl?id=9&type=bn&sort=1");
		tab:append_category("�R����",				"NEWS", "http://news.mixi.jp/list_news_category.pl?id=10&type=bn&sort=1");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- ���b�Z�[�W
		local tab = MZ3GroupItem:create("���b�Z�[�W");
		-- �J�e�S���͊e�p�[�T���ǉ�����
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- echo
		local tab = MZ3GroupItem:create("�G�R�[");
		tab:append_category("�݂�Ȃ̃G�R�[", "MIXI_RECENT_ECHO");
		tab:append_category("�����ւ̕ԐM�ꗗ", "MIXI_RECENT_ECHO", "http://mixi.jp/res_echo.pl");
		tab:append_category("�����̈ꗗ", "MIXI_RECENT_ECHO", "http://mixi.jp/list_echo.pl?id={owner_id}");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- ���̑�
		local tab = MZ3GroupItem:create("���̑�");
		tab:append_category("�}�C�~�N�ꗗ", "FRIEND");
		tab:append_category("�Љ", "INTRO");
		tab:append_category("������", "FOOTSTEP");
		tab:append_category("�J�����_�[", "CALENDAR", "show_calendar.pl");
		tab:append_category("�u�b�N�}�[�N", "BOOKMARK");
		tab:append_category("���C�ɓ��胆�[�U�[", "FAVORITE", "list_bookmark.pl");
		tab:append_category("���C�ɓ���R�~��", "FAVORITE_COMMUNITY", "list_bookmark.pl?kind=community");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end

--- estimate �Ώ۔��ʃC�x���g�n���h��
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        ��͑Ώ�URL
--
function on_estimate_access_type(event_name, url, data1, data2)

    -- �֘A�j���[�X
	if line_has_strings(url, 'http://news.mixi.jp/list_quote_diary.pl?') then
		return true, mz3.get_access_type_by_key('MIXI_NEWS_QUOTE_DIARY');
	end

	return false;
end


----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�C�x���g�n���h���o�^
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");

-- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\���C�x���g�n���h���o�^
mz3.add_event_listener("popup_body_menu",  "mixi.on_popup_body_menu");

-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group", false);

-- estimate �Ώۂ̒ǉ�
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type");


----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- �����X�g�n
-- �R�~���j�e�B�ŐV�����ꗗ
require("scripts\\mixi\\mixi_new_bbs_parser");

-- �g�b�v�y�[�W
require("scripts\\mixi\\mixi_home_parser");

-- ���b�Z�[�W(��M��, ���M��), �������b�Z�[�W, ���b�Z�[�W�ڍ�
require("scripts\\mixi\\mixi_new_official_message_parser");
require("scripts\\mixi\\mixi_message_outbox_parser");
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- �t��������
require("scripts\\mixi\\mixi_show_self_log_parser");

-- �j���[�X�֘A���L
require("scripts\\mixi\\mixi_news_quote_diary_parser");

mz3.logger_debug('mixi.lua end');
