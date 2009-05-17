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

-- POST�p�A�N�Z�X��ʓo�^
type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- �J�e�S��
type:set_service_type('Twitter');							-- �T�[�r�X���
type:set_serialize_key('TWITTER_FAVOURINGS_CREATE');		-- �V���A���C�Y�L�[
type:set_short_title('���C�ɓ���o�^');						-- �ȈՃ^�C�g��
type:set_request_method('POST');							-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');							-- �G���R�[�f�B���O

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- �J�e�S��
type:set_service_type('Twitter');							-- �T�[�r�X���
type:set_serialize_key('TWITTER_FAVOURINGS_DESTROY');		-- �V���A���C�Y�L�[
type:set_short_title('���C�ɓ���폜');						-- �ȈՃ^�C�g��
type:set_request_method('POST');							-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');							-- �G���R�[�f�B���O

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- �J�e�S��
type:set_service_type('Twitter');							-- �T�[�r�X���
type:set_serialize_key('TWITTER_FRIENDSHIPS_CREATE');		-- �V���A���C�Y�L�[
type:set_short_title('�t�H���[�o�^');						-- �ȈՃ^�C�g��
type:set_request_method('POST');							-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');							-- �G���R�[�f�B���O

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- �J�e�S��
type:set_service_type('Twitter');							-- �T�[�r�X���
type:set_serialize_key('TWITTER_FRIENDSHIPS_DESTROY');		-- �V���A���C�Y�L�[
type:set_short_title('�t�H���[����');						-- �ȈՃ^�C�g��
type:set_request_method('POST');							-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');							-- �G���R�[�f�B���O


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.retweet               = mz3_menu.regist_menu("twitter.on_retweet_menu_item");

-- �������� @xxx ���o�҂�TL(5�l�܂�)
menu_items.show_follower_tl = {}
menu_items.show_follower_tl[1]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_1");
menu_items.show_follower_tl[2]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_2");
menu_items.show_follower_tl[3]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_3");
menu_items.show_follower_tl[4]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_4");
menu_items.show_follower_tl[5]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_5");
follower_names = {}

menu_items.update                = mz3_menu.regist_menu("twitter.on_twitter_update");
menu_items.reply                 = mz3_menu.regist_menu("twitter.on_twitter_reply");
menu_items.new_dm                = mz3_menu.regist_menu("twitter.on_twitter_new_dm");
menu_items.create_favourings     = mz3_menu.regist_menu("twitter.on_twitter_create_favourings");
menu_items.destroy_favourings    = mz3_menu.regist_menu("twitter.on_twitter_destroy_favourings");
menu_items.create_friendships    = mz3_menu.regist_menu("twitter.on_twitter_create_friendships");
menu_items.destroy_friendships   = mz3_menu.regist_menu("twitter.on_twitter_destroy_friendships");
menu_items.show_friend_timeline  = mz3_menu.regist_menu("twitter.on_show_friend_timeline");
menu_items.open_home             = mz3_menu.regist_menu("twitter.on_open_home");
menu_items.open_friend_favorites = mz3_menu.regist_menu("twitter.on_open_friend_favorites");
menu_items.open_friend_favorites_by_browser = mz3_menu.regist_menu("twitter.on_open_friend_favorites_by_browser");
menu_items.open_friend_site      = mz3_menu.regist_menu("twitter.on_open_friend_site");



----------------------------------------
-- �T�[�r�X�p�֐�
----------------------------------------

--- �X�e�[�^�X�R�[�h���
function get_http_status_error_status(http_status)

	if http_status==200 or http_status==304 then
		-- 200 OK: ����
		-- 304 Not Modified: �V�������͂Ȃ�
		return nil;
	elseif http_status==400 then		-- Bad Request:
		return "API �̎��s�񐔐����Ɉ����|�������A�Ȃǂ̗��R�Ń��N�G�X�g���p������";
	elseif http_status==401 then		-- Not Authorized:
		return "�F�؎��s";
	elseif http_status==403 then		-- Forbidden:
		return "�������Ȃ�API �����s���悤�Ƃ���";
	elseif http_status==404 then		-- Not Found:
		return "���݂��Ȃ� API �����s���悤�Ƃ����A���݂��Ȃ����[�U�������Ŏw�肵�� API �����s���悤�Ƃ���";
	elseif http_status==500 then		-- Internal Server Error:
		return "Twitter ���ŉ��炩�̖�肪�������Ă��܂�";
	elseif http_status==502 then		-- Bad Gateway:
		return "Twitter �̃T�[�o���~�܂��Ă��܂��i�����e����������܂���j";
	elseif http_status==503 then		-- Service Unavailable:
		return "Twitter �̃T�[�o�̕��ׂ��������āA���N�G�X�g���ق��؂�Ȃ���ԂɂȂ��Ă��܂�";
	end

	return nil;
end


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

-- �u�Ԃ₭�v���j���[�p�n���h��
function on_twitter_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

-- �u�ԐM�v���j���[�p�n���h��
function on_twitter_reply(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- ���͗̈�Ƀ��[�U�̃X�N���[������ǉ�
	text = mz3_main_view.get_edit_text();
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	-- ���łɊ܂܂�Ă���Βǉ����Ȃ�
	if text:find("@" .. name, 1, true)~=nil then
		return;
	end
	text = text .. "@" .. name .. " ";
	
	mz3_main_view.set_edit_text(text);

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');

	-- �����ֈړ�
	VK_END = 0x23;
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end

-- �u���b�Z�[�W���M�v���j���[�p�n���h��
function on_twitter_new_dm(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_DM);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

-- �u���C�ɓ���o�^�v���j���[�p�n���h��
function on_twitter_create_favourings(serialize_key, event_name, data)
	-- URL ����
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/create/" .. id .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FAVOURINGS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- �u���C�ɓ���폜�v���j���[�p�n���h��
function on_twitter_destroy_favourings(serialize_key, event_name, data)
	-- URL ����
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/destroy/" .. id .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FAVOURINGS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- �u�t�H���[����v���j���[�p�n���h��
function on_twitter_create_friendships(serialize_key, event_name, data)

	-- �m�F
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " ������t�H���[���܂��B��낵���ł����H", nil, "yes_no") ~= 'yes' then
		-- ���~
		return;
	end

	-- URL ����
	url = "http://twitter.com/friendships/create/" .. name .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FRIENDSHIPS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- �u�t�H���[��߂�v���j���[�p�n���h��
function on_twitter_destroy_friendships(serialize_key, event_name, data)

	-- �m�F
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " ����̃t�H���[���������܂��B��낵���ł����H", nil, "yes_no") ~= 'yes' then
		-- ���~
		return;
	end

	-- URL ����
	url = "http://twitter.com/friendships/destroy/" .. name .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FRIENDSHIPS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- �u@xxx �̃^�C�����C���v���j���[�p�n���h��
function on_show_friend_timeline(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- �J�e�S���ǉ�
	title = "@" .. name .. "�̃^�C�����C��";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- �ǉ������J�e�S���̎擾�J�n
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- �u@xxx �̃^�C�����C���v(�t�H�����[)���j���[�p�n���h��
function on_show_follower_tl_1(serialize_key, event_name, data)	show_follower_tl(1) end
function on_show_follower_tl_2(serialize_key, event_name, data)	show_follower_tl(2) end
function on_show_follower_tl_3(serialize_key, event_name, data)	show_follower_tl(3) end
function on_show_follower_tl_4(serialize_key, event_name, data)	show_follower_tl(4) end
function on_show_follower_tl_5(serialize_key, event_name, data)	show_follower_tl(5) end
function show_follower_tl(num)
	-- �������� num �Ԗڂ� @xxx ���[�U�� TL ��\������
	name = follower_names[num];
	
	-- �J�e�S���ǉ�
	title = "@" .. name .. "�̃^�C�����C��";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- �ǉ������J�e�S���̎擾�J�n
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

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

--- �u�z�[���v���j���[�p�n���h��
function on_open_home(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name'));
end

--- �u�F�B�̂��C�ɓ���v���j���[�p�n���h��
function on_open_friend_favorites_by_browser(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name') .. "/favorites");
end

--- �u�F�B�̃T�C�g�v���j���[�p�n���h��
function on_open_friend_site(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end

--- �u�F�B�̂��C�ɓ���v���j���[�p�n���h��
function on_open_friend_favorites(serialize_key, event_name, data)

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- �J�e�S���ǉ�
	title = "@" .. name .. "�̂��C�ɓ���";
	url = "http://twitter.com/favorites/" .. name .. ".xml";
	key = "TWITTER_FAVORITES";
	mz3_main_view.append_category(title, url, key);
	
	-- �ǉ������J�e�S���̎擾�J�n
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
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
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
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

--- POST �����C�x���g
--
-- @param event_name    'post_end'
-- @param serialize_key �������ڂ̃V���A���C�Y�L�[
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param wnd           wnd
--
function on_post_end(event_name, serialize_key, http_status)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- Twitter���e��������

	-- �X�e�[�^�X�R�[�h�`�F�b�N
	msg = get_http_status_error_status(http_status);
	if msg ~= nil then
		-- �G���[�A���Ȃ̂Œ��f���邽�߂� true ��Ԃ�
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		return true;
	end

	-- ���N�G�X�g�̎�ʂɉ����ă��b�Z�[�W��\��
	if serialize_key == "TWITTER_NEW_DM" then
		mz3_main_view.set_info_text("���b�Z�[�W���M�I��");
	elseif serialize_key == "TWITTER_FAVOURINGS_CREATE" then
		mz3_main_view.set_info_text("�ӂ��ڂ����I");
	elseif serialize_key == "TWITTER_FAVOURINGS_DESTROY" then
		mz3_main_view.set_info_text("�ӂ��ڂ�̂�߂��I");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_CREATE" then
		mz3_main_view.set_info_text("�t�H���[�����I");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_DESTROY" then
		mz3_main_view.set_info_text("�t�H���[��߂��I");
	else 
		-- TWITTER_UPDATE
		mz3_main_view.set_info_text("�X�e�[�^�X���M�I��");
	end

	-- Wassr �ւ̓��e(�N���X�|�X�g)
--[[
	if serialize_key == "TWITTER_UPDATE" then

		text = mz3_main_view.get_edit_text();
		msg = "Wassr �ɂ����e���܂����H\r\n";
		msg = msg .. "----\r\n";
		msg = msg .. text .. "\r\n";
		msg = msg .. "----\r\n";
		
		if mz3.confirm(msg, nil, "yes_no") == "yes" then
		
			-- URL ����
			url = "http://api.wassr.jp/statuses/update.json";
			post = mz3_post_data.create();
			mz3_post_data.append_post_body(post, "status=");
			mz3_post_data.append_post_body(post, mz3.url_encode(text, 'utf8'));
			mz3_post_data.append_post_body(post, "&source=");
			mz3_post_data.append_post_body(post, mz3.get_app_name());
			
			-- �ʐM�J�n
			access_type = mz3.get_access_type_by_key("WASSR_UPDATE");
			referer = '';
			user_agent = nil;
			mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
			return true;
		end
	end
]]

	-- ���͒l������
	mz3_main_view.set_edit_text("");
	
	return true;
end

--- GET �����C�x���g
--
-- @param event_name    'get_end'
-- @param serialize_key �������ڂ̃V���A���C�Y�L�[
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param wnd           wnd
--
function on_get_end(event_name, serialize_key, http_status)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- �X�e�[�^�X�R�[�h�`�F�b�N�̂ݍs��
	msg = get_http_status_error_status(http_status);
	if msg ~= nil then
		-- �G���[�A���Ȃ̂Œ��f���邽�߂� true ��Ԃ�
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		mz3.alert("�T�[�o�G���[(" .. http_status .. ") : " .. msg);
		return true;
	end
	-- �G���[�Ȃ��Ȃ̂ő��s����(�㑱�̉�͏��������p��)

	return false;
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
	submenu = MZ3Menu:create_popup_menu();
	
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

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " �̃^�C�����C��", menu_items.show_friend_timeline);
	
	-- �������� @XXX �𒊏o���A���j���[��
	body_text = body:get_text_array_joined_text('body');
	i = 1;
	for f_name in body_text:gmatch("@([0-9a-zA-Z_]+)") do
		mz3.logger_debug(f_name);
		-- Lua �ϐ��ɖ��O��ۑ����Ă���
		follower_names[i] = f_name;
		menu:append_menu("string", "@" .. f_name .. " �̃^�C�����C��", menu_items.show_follower_tl[i]);
		-- �ő�5�l�܂ŃT�|�[�g
		i = i+1;
		if i>5 then
			break;
		end
	end

--	menu:append_menu("separator");
	
	submenu:append_menu("string", "@" .. name .. " ���t�H���[����", menu_items.create_friendships);
	submenu:append_menu("string", "@" .. name .. " �̃t�H���[����߂�", menu_items.destroy_friendships);
	submenu:append_menu("string", "@" .. name .. " �̃z�[�����u���E�U�ŊJ��", menu_items.open_home);
	submenu:append_menu("string", "@" .. name .. " �̂��C�ɓ�����u���E�U�ŊJ��", menu_items.open_friend_favorites_by_browser);
	submenu:append_menu("string", "@" .. name .. " �̂��C�ɓ���", menu_items.open_friend_favorites);
	menu:append_submenu("���̑�", submenu);

	-- URL ����łȂ���΁u�T�C�g�v��ǉ�
	url = body:get_text('url');
	if url~=nil and url:len()>0 then
		submenu:append_menu("string", "@" .. name .. " �̃T�C�g���u���E�U�ŊJ��", menu_items.open_friend_site);
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
	submenu:delete();
	
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

-- POST�����C�x���g�n���h���o�^
mz3.add_event_listener("post_end", "twitter.on_post_end");
mz3.add_event_listener("get_end",  "twitter.on_get_end");

mz3.logger_debug('twitter.lua end');
