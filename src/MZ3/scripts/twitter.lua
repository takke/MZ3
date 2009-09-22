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

ID_REPORT_URL_BASE = 36100 -37000;	-- URL���J��

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('Twitter', true);

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('Twitter', 'id_name', 'ID');
mz3_account_provider.set_param('Twitter', 'password_name', '�p�X���[�h');


--- MZ3����ID��TwitterID�ɕϊ�����
function id2realid(n)
	return n+2147483648;
end

--- TwitterID��MZ3����ID�ɕϊ�����
function realid2id(n)
	return n-2147483648;
end


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

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- �J�e�S��
type:set_service_type('Twitter');							-- �T�[�r�X���
type:set_serialize_key('TWITTER_UPDATE_WITH_TWITPIC');		-- �V���A���C�Y�L�[
type:set_short_title('twitpic���e');						-- �ȈՃ^�C�g��
type:set_request_method('POST');							-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');							-- �G���R�[�f�B���O

-- �t�@�C����
twitpic_target_file = nil;


--- 1���[�U�̒ǉ�
function my_add_new_user(new_list, status, id)
--	mz3.logger_debug("my_add_new_user start");

	-- data ����
	data = MZ3Data:create();
	
	-- id : status/id
--	local id = realid2id(status:match('<id>([^<]*)</id>'));
	data:set_integer('id', id);
	type = mz3.get_access_type_by_key('TWITTER_USER');
	data:set_access_type(type);
	
	-- updated : status/created_at
	local s = status:match('<created_at>([^<]*)</created_at>');
	data:parse_date_line(s);
	
	-- text : status/text
	text = status:match('<text>([^<]*)</text>');
	text = text:gsub('&amp;', '&');
	text = mz3.decode_html_entity(text);
	data:add_text_array('body', text);
	
	-- @takke �Ȃǂ�����΃o�C�u����
--	if line_has_strings(text, "@" .. my_twitter_name) then
--		mz3.set_vib_status(true);
--		mz3.set_vib_status(false);
--	end
	
	-- URL �𒊏o���A�����N�ɂ���
	for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
		data:add_link_list(url, url);
--		mz3.logger_debug(url);
	end

	-- source : status/source
	s = status:match('<source>([^<]*)</source>');
	s = mz3.decode_html_entity(s);
	data:set_text('source', s);
	
	-- name : status/user/screen_name
	user = status:match('<user>.-</user>');
	s = user:match('<screen_name>([^<]*)</screen_name>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('name', s);
	
	-- author : status/user/name
	s = user:match('<name>([^<]*)</name>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('author', s);

	-- description : status/user/description
	-- title �ɓ����̂͋���̍�E�E�E
	s = user:match('<description>([^<]*)</description>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('title', s);

	-- owner-id : status/user/id
	data:set_integer('owner_id', user:match('<id>([^<]*)</id>'));

	-- URL : status/user/url
	url = user:match('<url>([^<]*)</url>');
	data:set_text('url', url);
	data:set_text('browse_uri', url);

	-- Image : status/user/profile_image_url
	profile_image_url = user:match('<profile_image_url>([^<]*)</profile_image_url>');
	profile_image_url = mz3.decode_html_entity(profile_image_url);
--	mz3.logger_debug(profile_image_url);

	-- �t�@�C�����݂̂�URL�G���R�[�h
--	int idx = strImage.ReverseFind( '/' );
--	if (idx >= 0) {
--		CString strFileName = strImage.Mid( idx +1 );
--		strFileName = URLEncoder::encode_utf8( strFileName );
--		strImage = strImage.Left(idx + 1);
--		strImage += strFileName;
--	}
	data:add_text_array('image', profile_image_url);

	-- <location>East Tokyo United</location>
	data:set_text('location', mz3.decode_html_entity(user:match('<location>([^<]*)</location>')));
	-- <followers_count>555</followers_count>
	data:set_integer('followers_count', user:match('<followers_count>([^<]*)</followers_count>'));
	-- <friends_count>596</friends_count>
	data:set_integer('friends_count', user:match('<friends_count>([^<]*)</friends_count>'));
	-- <favourites_count>361</favourites_count>
	data:set_integer('favourites_count', user:match('<favourites_count>([^<]*)</favourites_count>'));
	-- <statuses_count>7889</statuses_count>
	data:set_integer('statuses_count', user:match('<statuses_count>([^<]*)</statuses_count>'));

	-- �ꎞ���X�g�ɒǉ�
	new_list:add(data.data);
	
	-- data �폜
	data:delete();

--	mz3.logger_debug("my_add_new_user end");
end
--------------------------------------------------
-- [list] �^�C�����C���p�p�[�T
--
-- http://twitter.com/statuses/friends_timeline.xml
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function twitter_friends_timeline_parser(parent, body, html)
	mz3.logger_debug("twitter_friends_timeline_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	my_twitter_name = mz3_account_provider.get_value('Twitter', 'id');

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
	status = '';
	local i=0;
	local id=0;
	while i<line_count do
		line = html:get_at(i);
--		mz3.logger_debug('line:' .. i);
		
		if line_has_strings(line, '<status>') then
			status = line;
			
			-- </status> �܂Ŏ擾����
			-- �������A����ID�������skip����
			i = i+1;
			i_in_status = 0;
			while i<line_count do
				line = html:get_at(i);
				status = status .. line;
				
				-- ���� skip �͐�ɂ��
				if i_in_status<3 and line_has_strings(line, '<id>') then
					-- id : status/id
					id = realid2id(line:match('<id>(.-)</id>'));
					-- ����ID������Βǉ����Ȃ��B
					if id_set[ "" .. id ] then
						mz3.logger_debug('id[' .. id .. '] �͊��ɑ��݂���̂�skip����');
						i = i+1;
						while i<line_count do
							line = html:get_at(i);
							
							if line_has_strings(line, '</status>') then
								break;
							end
							i = i+1;
						end
--						mz3.logger_debug('new i:' .. i);

						status = '';
						break;
					end
				elseif i_in_status>35 and line_has_strings(line, '</status>') then
					-- </status> ���������̂ł����܂ł� status ����͂��Ēǉ�
					my_add_new_user(new_list, status, id);
					break;
				end
				i = i+1;
				i_in_status = i_in_status+1;
			end

			-- ���� status �擾
			status = '';
		end
		i = i+1;
	end
	
	-- ���������f�[�^���o�͂ɔ��f
	if mz3.get_app_name()=="MZ3" then
		body:merge(new_list, 100);
	else
		body:merge(new_list, 1000);
	end

	-- �V�������� parent(�J�e�S���� m_mixi) �ɐݒ肷��
	parent:set_integer('new_count', new_list:get_count());
	
	new_list:delete();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_friends_timeline_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_FRIENDS_TIMELINE", "twitter.twitter_friends_timeline_parser");
mz3.set_parser("TWITTER_FAVORITES", "twitter.twitter_friends_timeline_parser");


--------------------------------------------------
-- [list] DM�p�p�[�T
--
-- http://twitter.com/direct_messages.xml
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function twitter_direct_messages_parser(parent, body, html)
	mz3.logger_debug("twitter_direct_messages_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 'sent.xml' ������΁u���M���b�Z�[�W�v
	category_url = parent:get_text('url');
	is_sent = line_has_strings(category_url, 'sent.xml');

	-- �S�������Ȃ�
--	body:clear();
	
	-- �d���h�~�p�� id �ꗗ�𐶐��B
	id_set = {};
	n = body:get_count();
	for i=0, n-1 do
		id = mz3_data.get_integer(body:get_data(i), 'id');
		id_set[ "" .. id ] = true;
	end

	local t1 = mz3.get_tick_count();
	
	-- �ꎞ���X�g
	new_list = MZ3DataList:create();
	
	line = '';
	local line_count = html:get_count();
	direct_message = '';
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<direct_message>') then
			direct_message = line;
		elseif line_has_strings(line, '</direct_message>') then
			direct_message = direct_message .. line;
			
			-- id : direct_message/id
			id = direct_message:match('<id>(.-)</id>');
			
			-- ����ID������Βǉ����Ȃ��B
			if id_set[ id ] then
--				mz3.logger_debug('id[' .. id .. '] �͊��ɑ��݂���̂�skip����');
			else
				-- data ����
				data = MZ3Data:create();
				
				data:set_integer('id', id);
				
				type = mz3.get_access_type_by_key('TWITTER_USER');
				data:set_access_type(type);
				
				-- text : direct_message/text
				text = direct_message:match('<text>(.-)</text>');
				text = text:gsub('&amp;', '&');
				text = mz3.decode_html_entity(text);
				data:add_text_array('body', text);
				
				-- updated : status/created_at
				s = direct_message:match('<created_at>(.-)</created_at>');
				data:parse_date_line(s);
				
				-- URL �𒊏o���A�����N�ɂ���
				for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
					data:add_link_list(url, url);
--					mz3.logger_debug(url);
				end
				

				if is_sent then
					user = direct_message:match('<recipient>.-</recipient>');
				else
					user = direct_message:match('<sender>.-</sender>');
				end
				
				-- name : direct_message/user/screen_name
				s = user:match('<screen_name>(.-)</screen_name>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('name', s);
				
				-- author : direct_message/user/name
				s = user:match('<name>(.-)</name>');
				s = mz3.decode_html_entity(s);
				data:set_text('author', s);

				-- description : direct_message/user/description
				-- title �ɓ����̂͋���̍�E�E�E
				s = user:match('<description>(.-)</description>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('title', s);

				-- owner-id : direct_message/user/id
				data:set_integer('owner_id', user:match('<id>(.-)</id>'));

				-- URL : direct_message/user/url
				url = user:match('<url>(.-)</url>');
				data:set_text('url', url);
				data:set_text('browse_uri', url);

				-- Image : direct_message/user/profile_image_url
				profile_image_url = user:match('<profile_image_url>(.-)</profile_image_url>');
				profile_image_url = mz3.decode_html_entity(profile_image_url);
--				mz3.logger_debug(profile_image_url);

				-- �t�@�C�����݂̂�URL�G���R�[�h
--				int idx = strImage.ReverseFind( '/' );
--				if (idx >= 0) {
--					CString strFileName = strImage.Mid( idx +1 );
--					strFileName = URLEncoder::encode_utf8( strFileName );
--					strImage = strImage.Left(idx + 1);
--					strImage += strFileName;
--				}
				data:add_text_array('image', profile_image_url);

				-- <location>East Tokyo United</location>
				data:set_text('location', mz3.decode_html_entity(user:match('<location>(.-)</location>')));
				-- <followers_count>555</followers_count>
				data:set_integer('followers_count', user:match('<followers_count>(.-)</followers_count>'));
				-- <friends_count>596</friends_count>
				data:set_integer('friends_count', user:match('<friends_count>(.-)</friends_count>'));
				-- <favourites_count>361</favourites_count>
				data:set_integer('favourites_count', user:match('<favourites_count>(.-)</favourites_count>'));
				-- <statuses_count>7889</statuses_count>
				data:set_integer('statuses_count', user:match('<statuses_count>(.-)</statuses_count>'));

				-- �ꎞ���X�g�ɒǉ�
				new_list:add(data.data);
				
				-- data �폜
				data:delete();

			end

			-- ���� direct_message �擾
			direct_message = '';
		elseif direct_message ~= '' then	-- direct_message ����ł���� <direct_message> �������Ȃ̂œǂݔ�΂�
			direct_message = direct_message .. line;
		end
	end
	
	-- ���������f�[�^���o�͂ɔ��f
	body:merge(new_list);
	--TwitterParserBase::MergeNewList(out_, new_list);

	new_list:delete();
	
	-- �V�������� parent(�J�e�S���� m_mixi) �ɐݒ肷��
	parent:set_integer('new_count', new_count);
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_direct_messages_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_DIRECT_MESSAGES", "twitter.twitter_direct_messages_parser");


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.show_user_info        = mz3_menu.regist_menu("twitter.on_show_user_info");
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
menu_items.update_with_twitpic   = mz3_menu.regist_menu("twitter.on_twitter_update_with_twitpic");
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

--- BASIC �F�ؐݒ�
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Twitter' then
		if serialize_key=='TWITTER_UPDATE_WITH_TWITPIC' then
			-- twitpic �� BASIC �F�ؕs�v
			return true, 0, '', '';
		end
		id       = mz3_account_provider.get_value('Twitter', 'id');
		password = mz3_account_provider.get_value('Twitter', 'password');
		
		if id=='' or password=='' then
			mz3.alert('���O�C���ݒ��ʂŃ��[�UID�ƃp�X���[�h��ݒ肵�Ă�������');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "twitter.on_set_basic_auth_account");


--- Twitter���������݃��[�h�̏�����
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Twitter' then
		-- ���[�h�ύX
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "twitter.on_reset_twitter_style_post_mode");


--- Twitter�X�^�C���̃{�^�����̂̍X�V
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'TWITTER_NEW_DM' then
		return true, 'DM';
	elseif serialize_key == 'TWITTER_UPDATE' then
		return true, '�X�V';
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		return true, 'TwitPic';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "twitter.on_update_twitter_update_button");


--- �u�Ԃ₭�v���j���[�p�n���h��
function on_twitter_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE'));

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

--- �u�ʐ^�𓊍e�v���j���[�p�n���h��
function on_twitter_update_with_twitpic(serialize_key, event_name, data)

	-- �t�@�C���I�����
	OFN_EXPLORER = 0x00080000;
	OFN_FILEMUSTEXIST = 0x00001000;
	flags = OFN_EXPLORER + OFN_FILEMUSTEXIST;
	filter = "JPEĢ�� (*.jpg)%0*.jpg;*.jpeg%0%0";
--	filter = "JPEĢ�� (*.jpg)%0*.jpg;*.jpeg%0���ׂĂ�̧�� (*.*)%0*.*%0%0";
	twitpic_target_file = mz3.get_open_file_name(mz3_main_view.get_wnd(),
												 "JPEĢ�ق��J��...", 
												 filter,
												 flags,
												 "");
--	mz3.alert(twitpic_target_file);
	if twitpic_target_file == nil then
		-- ���~
		return;
	end

	-- ���[�h�ύX
	mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE_WITH_TWITPIC'));

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end


--- �u�ԐM�v���j���[�p�n���h��
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
	if use_twitter_reply_with_dot then
		text = text .. ".@" .. name .. " ";
	else
		text = text .. "@" .. name .. " ";
	end
	
	mz3_main_view.set_edit_text(text);

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');

	-- �����ֈړ�
	VK_END = 0x23;
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end


--- �u���b�Z�[�W���M�v���j���[�p�n���h��
function on_twitter_new_dm(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_DM);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end


--- �u���C�ɓ���o�^�v���j���[�p�n���h��
function on_twitter_create_favourings(serialize_key, event_name, data)
	-- URL ����
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = id2realid(body:get_integer('id'));
	url = "http://twitter.com/favourings/create/" .. id .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FAVOURINGS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- �u���C�ɓ���폜�v���j���[�p�n���h��
function on_twitter_destroy_favourings(serialize_key, event_name, data)
	-- URL ����
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = id2realid(body:get_integer('id'));
	url = "http://twitter.com/favourings/destroy/" .. id .. ".xml";

	-- �ʐM�J�n
	key = "TWITTER_FAVOURINGS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- �u�t�H���[����v���j���[�p�n���h��
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


--- �u�t�H���[��߂�v���j���[�p�n���h��
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
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");


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
--	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. id2realid(data:get_integer('id')) .. "\r\n";

	mz3.alert(item, data:get_text('name'));

	return true;
end


--- @user�ɂ���
function on_show_user_info(serialize_key, event_name, data)
	mz3.logger_debug('on_show_user_info : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	item = '';
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
--	item = item .. data:get_date() .. "\r\n";
--	item = item .. "id : " .. id2realid(data:get_integer('id')) .. "\r\n";
	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";

	if data:get_text('location') ~= '' then
		item = item .. "location : " .. data:get_text('location') .. "\r\n";
	end
	
	if data:get_integer('friends_count') ~= -1 then
		item = item .. "followings : " .. data:get_integer('friends_count')
		            .. ", "
		            .. "followers : " .. data:get_integer('followers_count')
		            .. ", "
		            .. "fav : " .. data:get_integer('favourites_count')
		            .. ", "
		            .. "���� : " .. data:get_integer('statuses_count')
		            .. "\r\n";
	end
	
	-- �\�[�X
	source = data:get_text('source');
--	item = item .. "source : " .. source .. "\r\n";
	s_url, s_name = source:match("href=\"(.-)\".*>(.*)<");
	if s_url ~= nil then
		item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";
	else
		item = item .. "source : " .. source .. "\r\n";
	end

	mz3.alert(item, data:get_text('name'));

	return true;
end


--- Twitter �ɓ��e����
function do_post_to_twitter(text)

	serialize_key = 'TWITTER_UPDATE'

	-- �w�b�_�[�̐ݒ�
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST �p�����[�^��ݒ�
	post:append_post_body('status=');
	post:append_post_body(mz3.url_encode(text, 'utf8'));
	
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost �̊m�F
	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
		if text:find("RT @", 1, false)~=nil then
			-- RT���܂܂�Ă���̂Œǉ����Ȃ�
		else
			footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
			post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
		end
	end
	post:append_post_body('&source=');
	post:append_post_body(mz3.get_app_name());

	-- POST��URL�ݒ�
	url = 'http://twitter.com/statuses/update.xml';
	
	-- �ʐM�J�n
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end


--- �X�V�{�^�������C�x���g
--
-- @param event_name    'click_update_button'
-- @param serialize_key Twitter���������݃��[�h�̃V���A���C�Y�L�[
--
function on_click_update_button(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- ���͕�������擾
	text = mz3_main_view.get_edit_text();

	-- �����͎��̏���
	if text == '' then
		if serialize_key == 'TWITTER_NEW_DM' then
			-- �����͂�NG => ���������ɏI��
			return true;
		elseif serialize_key == 'TWITTER_UPDATE' then
			-- �ŐV�擾
			mz3_main_view.retrieve_category_item();
			return true;
		elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
			-- �����͂�OK => ���s
		end
	end

	-- �m�F
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'TWITTER_NEW_DM' then
		msg = data:get_text('name') .. ' ����Ɉȉ��̃��b�Z�[�W�𑗐M���܂��B\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. '��낵���ł����H';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE' then
		msg = 'Twitter�Ŕ������܂��B\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. '��낵���ł����H';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		msg = 'twitpic�œ��e���܂��B\n'
		   .. '---- ���� ----\n'
		   .. text .. '\n'
		   .. '---- �t�@�C�� ----\n'
		   .. twitpic_target_file .. '\n'
		   .. '----\n'
		   .. '��낵���ł����H';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	if serialize_key == 'TWITTER_UPDATE' then
		-- �ʏ�̓��e�͋��ʉ�
		
		-- �N���X�|�X�g�Ǘ��f�[�^������
		mz3.init_cross_post_info("twitter");

		return do_post_to_twitter(text);
	end

	-- �w�b�_�[�̐ݒ�
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST �p�����[�^��ݒ�
	if serialize_key == 'TWITTER_NEW_DM' then
		post:append_post_body('text=');
		post:append_post_body(mz3.url_encode(text, 'utf8'));
		post:append_post_body('&user=');
		post:append_post_body(data:get_integer('owner_id'));
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		id       = mz3_account_provider.get_value('Twitter', 'id');
		password = mz3_account_provider.get_value('Twitter', 'password');

		post:set_content_type('multipart/form-data; boundary=---------------------------7d62ee108071e' .. '\r\n');
		
		-- id
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="username"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.url_encode(id, 'utf8') .. '\r\n');
		
		-- password
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="password"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.url_encode(password, 'utf8') .. '\r\n');
		
		-- ���b�Z�[�W��UTF8�Ŗ��ߍ���
		-- message
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="message"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(text, 'sjis', 'utf8'));
		-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost �̊m�F
		if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
			footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
			post:append_post_body(mz3.convert_encoding(footer_text, 'sjis', 'utf8'));
		end
		post:append_post_body('\r\n');

		-- media (image binary)
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="media"; filename="photo.jpg"' .. '\r\n');
		post:append_post_body('Content-Type: image/jpeg' .. '\r\n');
		post:append_post_body('\r\n');
		
		if post:append_file(twitpic_target_file) == false then
			mz3.alert('�摜�t�@�C���̓ǂݍ��݂Ɏ��s���܂���');
			return true;
		end

		post:append_post_body('\r\n');
		
		-- end of post data
		post:append_post_body('-----------------------------7d62ee108071e--' .. '\r\n');

	end

	-- POST��URL�ݒ�
	url = '';
	if serialize_key == 'TWITTER_NEW_DM' then
		url = 'http://twitter.com/direct_messages/new.xml';
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		url = 'http://twitpic.com/api/uploadAndPost';
	end
	
	-- �ʐM�J�n
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end
mz3.add_event_listener("click_update_button", "twitter.on_click_update_button");


--- POST �����C�x���g
--
-- @param event_name    'post_end'
-- @param serialize_key �������ڂ̃V���A���C�Y�L�[
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param filename      ���X�|���X�t�@�C��
-- @param wnd           wnd
--
function on_post_end(event_name, serialize_key, http_status, filename)

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
		mz3_main_view.set_info_text("���b�Z�[�W���M���܂���");
	elseif serialize_key == "TWITTER_FAVOURINGS_CREATE" then
		mz3_main_view.set_info_text("�ӂ��ڂ����I");
	elseif serialize_key == "TWITTER_FAVOURINGS_DESTROY" then
		mz3_main_view.set_info_text("�ӂ��ڂ�̂�߂��I");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_CREATE" then
		mz3_main_view.set_info_text("�t�H���[�����I");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_DESTROY" then
		mz3_main_view.set_info_text("�t�H���[��߂��I");
	elseif serialize_key == "TWITTER_UPDATE_WITH_TWITPIC" then
		mz3_main_view.set_info_text("twitpic �摜���e����");
	else
		-- TWITTER_UPDATE
--		mz3_main_view.set_info_text("�X�e�[�^�X���M�I��");
		mz3_main_view.set_info_text("�������܂���");
	end

	-- �N���X�|�X�g
	if serialize_key == "TWITTER_UPDATE" then
		if mz3.do_cross_post() then
			return true;
		end
	end

	-- twitpic �� �ʐ^���e�݂̂��s���ꍇ�͉��L�̃R�[�h�� POST ����
--[[
	if serialize_key == "TWITTER_UPDATE_WITH_TWITPIC" then
		f = io.open(filename, 'r');
		file = f:read('*a');
		f:close();
		
		-- ���e�����t�@�C����URL�擾
		twitpic_url = file:match('<mediaurl>(.-)<');
		if twitpic_url == '' then
			mz3.alert('���e�Ɏ��s�����\��������܂��B�ēx���e���Ă��������B');
			return true;
		end
--		mz3.alert(twitpic_url);
		
		-- �ēxPOST����
		text = mz3_main_view.get_edit_text();
		do_post_to_twitter(twitpic_url .. ' - ' .. text);
		
		return true;
	end
]]

	-- ���͒l������
	mz3_main_view.set_edit_text("");
	
	return true;
end
mz3.add_event_listener("post_end", "twitter.on_post_end");


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
mz3.add_event_listener("get_end",  "twitter.on_get_end");


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
	
	name = body:get_text('name');

	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�S����ǂ�...", menu_items.read);
	menu:append_menu("string", "@" .. name .. " ����ɂ���...", menu_items.show_user_info);

	menu:append_menu("separator");

	menu:append_menu("string", "�Ԃ₭", menu_items.update);
	menu:append_menu("string", "�ʐ^�𓊍e(twitpic)...", menu_items.update_with_twitpic);

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
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");


--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' Twitter', 1, true) ~= nil then

		-- Twitter�^�u�ǉ�
		local tab = MZ3GroupItem:create("Twitter");
		tab:append_category("�^�C�����C��", "TWITTER_FRIENDS_TIMELINE", "http://twitter.com/statuses/friends_timeline.xml");
		tab:append_category("�ԐM�ꗗ", "TWITTER_FRIENDS_TIMELINE", "http://twitter.com/statuses/replies.xml");
		tab:append_category("���C�ɓ���", "TWITTER_FAVORITES", "http://twitter.com/favorites.xml");
		tab:append_category("��M���b�Z�[�W", "TWITTER_DIRECT_MESSAGES", "http://twitter.com/direct_messages.xml");
		tab:append_category("���M���b�Z�[�W", "TWITTER_DIRECT_MESSAGES", "http://twitter.com/direct_messages/sent.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end
mz3.add_event_listener("creating_default_group", "twitter.on_creating_default_group", false);


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
	if service_type=='Twitter' then
		return true, VIEW_STYLE_TWITTER;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "twitter.on_get_view_style");


mz3.logger_debug('twitter.lua end');
