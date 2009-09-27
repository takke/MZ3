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

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('mixi', 'id_name', '���[���A�h���X');
mz3_account_provider.set_param('mixi', 'password_name', '�p�X���[�h');


--------------------------------------------------
-- �y�v���t�B�[���z
-- [content] show_friend.pl �p�p�[�T
--
-- http://mixi.jp/show_friend.pl
--
-- ����:
--   parent: ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy:  NULL
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_show_friend_parser(parent, body, html)
	mz3.logger_debug("mixi_show_friend_parser start");

	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);
	
	parent:clear();
	
	parent:set_text('name', '');
	parent:set_date('');

	local t1 = mz3.get_tick_count();
	local line_count = html:get_count();
	
	-- ���O, �摜
	local i=100;
	local sub_html = '';
	sub_html, i = get_sub_html(html, i, line_count, {"<div id=", '"myProfile"'}, {'<div id=', '"mymixiList"'});
	
--	mz3.logger_debug(sub_html);
	
	local name = sub_html:match('<h3>(.-)</h3>');
	if name ~= nil then
		name = mz3.decode_html_entity(name);
		parent:set_text('name', name);
		parent:set_text('title', name);
		parent:set_text('author', name);
	end
	
	-- ���[�U�摜
	local url = sub_html:match('<img src="(.-)"');
	if url ~= nil then
		parent:add_link_list(url, '���[�U�摜');
		parent:add_text_array("body", "<_a><<���[�U�摜>></_a>");
	end
	
	-- �ŏI���O�C��
	-- <p class="loginTime">�i�ŏI���O�C����3���ȏ�j</p>
	local last_login = sub_html:match('class="loginTime">(.-)<');
	if last_login ~= nil then
		parent:add_body_with_extract(" ");
		parent:add_body_with_extract(last_login);
	end
	
	-- �F�l�֌W
	-- <p class="friendPath">������ �� <a href="show_friend.pl?id=109835">�������イ</a></p>
	local friend_path = sub_html:match('class="friendPath">(.-)</p>');
	if friend_path ~= nil then
		parent:add_body_with_extract("<br>");
		parent:add_body_with_extract(friend_path);
	end
	
	
	-- �v���t�B�[����S�Ď擾���A�{���ɐݒ肷��B
	sub_html, i = get_sub_html(html, i, line_count, {'<div id="profile">'}, {'</ul>'});
	parent:add_body_with_extract("<br>");
	for li in sub_html:gmatch("<li>(.-)</li>") do
--		<li><dl><dt>����</dt><dd>Web�v���O���}</dd></dl></li>
		local dt = li:match("<dt>(.-)<");
		local dd = li:match("<dd>(.-)<");
		if dt ~= nil and dd ~= nil then
			parent:add_body_with_extract("�� " .. dt);
			parent:add_body_with_extract("<br>");
			
			parent:add_body_with_extract(dd);
			parent:add_body_with_extract("<br>");
			parent:add_body_with_extract("<br>");
		end
	end
	
	-- �ŐV�̓��L�擾
	local child_number = 1;
	sub_html, i = get_sub_html(html, i, line_count, {'<div id="newFriendDiary">'}, {'</dl>'});
	-- dt/span, dd/a �����݂ɏo������B
	-- <dt><span>05��03��</span></dt>
	-- <dd><a href="view_diary.pl?id=xxx&owner_id=xxx" >WM�n�̈̂��l�ƈ��݉�ɍs���āc</a></dd>
	local child = MZ3Data:create();
	child:add_body_with_extract("<br>");
	for dt, dd in sub_html:gmatch("<dt>(.-)</dt>.-<dd>(.-)</dd>") do
		local date = dt:match("<span>(.-)<");
		
		if date ~= nil then
			child:add_body_with_extract("�� " .. date .. " : " .. dd);
			child:add_body_with_extract("<br>");
		end
	end
	child:set_integer("comment_index", child_number);
	child:set_text('author', "�ŐV�̓��L");
	parent:add_child(child);
	child_number = child_number + 1;
	child:delete();
	
	-- �Љ�擾
	sub_html, i = get_sub_html(html, i, line_count, {'<div id="intro">'}, {'</ul>'});
	--[[
<dl>
<dt><a href="show_friend.pl?id=xxx"><img src="xxx.jpg" alt="��" onerror="javascript:this.width=76;this.height=76;" /></a>
<br /><a href="show_friend.pl?id=xxx">��</a></dt>
<dd>
<p class="relation">�֌W�Fxxx</p>
<p class="userInput">xxx�ł��B</p>
</dd>
</dl>
]]
	local child = MZ3Data:create();
	child:add_body_with_extract("<br>");
	local has_intro = false;
	for dt, dd in sub_html:gmatch("<dt>(.-)</dt>.-<dd>(.-)</dd>") do
		local name     = dt:match('</a>.-(<a.-</a>)');
		local relation = dd:match('"relation">(.-)<');
		local text     = dd:match('"userInput">(.-)</p');

		child:add_body_with_extract("�� " .. name .. "<br>");
		child:add_body_with_extract(relation);
		child:add_body_with_extract("<br>");
		child:add_body_with_extract(text);
		child:add_body_with_extract("<br>");
		child:add_body_with_extract("<br>");
		
		has_intro = true;
	end
	if has_intro then
		child:set_integer("comment_index", child_number);
		child:set_text('author', "�Љ");
		parent:add_child(child);
		child_number = child_number + 1;
	end
	child:delete();
	
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_show_friend_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_PROFILE", "mixi.mixi_show_friend_parser");


--------------------------------------------------
-- �y�݂�Ȃ̃G�R�[�ꗗ�z
-- [list] recent_echo.pl �p�p�[�T
--
-- http://mixi.jp/recent_echo.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_recent_echo_parser(parent, body, html)
	mz3.logger_debug("mixi_recent_echo_parser start");

	-- wrapper�N���X��
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();

	local t1 = mz3.get_tick_count();
	local in_data_region = false;

	local line_count = html:get_count();
	
	-- post_key �T��
	mixi.post_key = '';
	for i=100, line_count-1 do
		line = html:get_at(i);
		
		-- <input type="hidden" name="post_key" id="post_key" value="xxx"> 
		if line_has_strings(line, 'hidden', 'name="post_key"') then
			mixi.post_key = line:match('value="(.-)"');
			break;
		end
	end

	-- �s���擾
	for i=180, line_count-1 do
		line = html:get_at(i);

		-- ���ڒT�� �ȉ���s
		-- <td class="thumb">
		if line_has_strings(line, "<td", "class", "thumb") then
		-- <div class="echo_member_id" style="display: none;">���[�UID</div>
		-- if line_has_strings(line, "<div", "class", "echo_member_id") then

			-- data ����
			data = MZ3Data:create();

			in_data_region = true;

			-- �摜�擾
			line = html:get_at(i);
			image_url = line:match('<img src="(.-)"');
			if image_url ~= nil then
				data:add_text_array("image", image_url);
			--	mz3.alert(image_url);
			end

			-- </tr>�܂Ōp��, ��������o�b�t�@�����O
			sub_html = '';
			for i=i+2, line_count-1 do
				line = html:get_at(i);
				
				if line_has_strings(line, '</tr>') then
					break;
				end
				sub_html = sub_html .. line;
			end
			
--			mz3.alert(sub_html);
			
			-- URL �擾
			url = sub_html:match('<a href="(.-)"');
			if url ~= nil then
				url = complement_mixi_url(url);
				data:set_text("url", url);
			end
--[[
			-- �摜�擾
			image_url = sub_html:match('<img src="(.-)"');
			if image_url ~= nil then
				data:add_text_array("image", image_url);
			--	mz3.alert(image_url);
			end
]]
			-- ���[�U��
			-- name = line:match(">([^<]+)(<.*)$");
			local s = sub_html:match('<td class="nickname">(.-)</a>');
			if s ~= nil then
				if s ~= nil then
					s = s:gsub('<.->', '');
					s = s:gsub("\n", '');
					s = s:gsub('^ +', '');
					s = s:gsub(' +$', '');
					data:set_text("name", s);
				end
			else
				i = i +2;
				line = html:get_at(i);
				-- s = line:match(">([^<]+)(<.*)$");
				s = line:match("([^<]+)(<.*)$");
				data:set_text("name", s);
				-- mz3.alert(line);
			end

			-- ����
			local comment = sub_html:match('<td class="comment">(.-)<span>');
			
			-- ���p���[�U������Β��o���Ă���
			if line_has_strings(comment, '<a', 'view_echo.pl', '</a>') then
				local ref_user_id, ref_user_name = comment:match('<a href=".-id=(.-)&.-">&gt;&gt;(.-)</');
				data:set_integer('ref_user_id', ref_user_id);
				data:set_text('ref_user_name', ref_user_name);
			end
			
			-- �����擾
			if comment ~= nil then
				comment = comment:gsub("\n", '');
				comment = comment:gsub('<a.->', '');
				comment = comment:gsub('</a>', ' ');
				data:add_body_with_extract(comment);
			end

			-- ����
			local date = sub_html:match('<span>.-<a.->(.-)</a>');
			if date ~= nil then
				date = date:gsub("\n", '');
				data:set_date(date);
			end

			-- id
			local author_id = sub_html:match('class="echo_member_id".->(.-)</');
			if author_id ~= nil then
				data:set_integer("author_id", author_id);
				data:set_integer("id", author_id);
			end
			local echo_post_time = sub_html:match('class="echo_post_time".->(.-)</');
			if echo_post_time ~= nil then
				data:set_text('echo_post_time', echo_post_time);
			end

			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			--type = mz3.estimate_access_type_by_url(url);
			type = mz3.get_access_type_by_key('MIXI_RECENT_ECHO_ITEM');
			data:set_access_type(type);
			
			-- data �ǉ�
			body:add(data.data);

			-- data �폜
			data:delete();
		end

		if in_data_region then
			back_data, next_data = parse_next_back_link(line, 'recent_echo.pl', 'title');
			if back_data ~= nil then
				back_data:add_body_with_extract(back_data:get_text('title'));
				body:insert(0, back_data.data);
			end
			if next_data ~= nil then
				next_data:add_body_with_extract(next_data:get_text('title'));
				body:add(next_data.data);
			end
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("��</ul>�����������̂ŏI�����܂�");
			break;
		end

	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_recent_echo_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
-- �݂�Ȃ̃G�R�[
mz3.set_parser("MIXI_RECENT_ECHO", "mixi.mixi_recent_echo_parser");
mz3.set_parser("MIXI_RES_ECHO"   , "mixi.mixi_recent_echo_parser");
mz3.set_parser("MIXI_LIST_ECHO"  , "mixi.mixi_recent_echo_parser");


--------------------------------------------------
--- ���ցA�O�ւ̒��o����
--------------------------------------------------
function parse_next_back_link(line, base_url, title_set_at)

	local back_data = nil;
	local next_data = nil;
	if title_set_at == nil then
		title_set_at = "title";
	end
	
	-- <ul><li><a href="new_bbs.pl?page=1">�O��\��</a></li>
	-- <li>51���`100����\��</li>
	-- <li><a href="new_bbs.pl?page=3">����\��</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- �O
		local url, t = line:match([[href="([^"]+)">(�O[^<]+)<]]);
		if url~=nil then
			back_data = MZ3Data:create();
			back_data:set_text(title_set_at, "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- ��
		local url, t = line:match([[href="([^"]+)">(��[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text(title_set_at, "<< " .. t .. " >>");
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
menu_items.mixi_echo_add_user_echo_list = mz3_menu.regist_menu("mixi.on_mixi_echo_add_user_echo_list");
menu_items.mixi_echo_add_ref_user_echo_list = mz3_menu.regist_menu("mixi.on_mixi_echo_add_ref_user_echo_list");

----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- Twitter���������݃��[�h�̏�����
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='mixi' then
		-- ���[�h�ύX
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('MIXI_ADD_ECHO'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "mixi.on_reset_twitter_style_post_mode");


--- Twitter�X�^�C���̃{�^�����̂̍X�V
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'MIXI_ADD_ECHO' then
		return true, 'voice';
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		return true, '�ԐM';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "mixi.on_update_twitter_update_button");


--- �u�Ԃ₭�v���j���[�p�n���h��
function on_mixi_echo_update(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

--- �u�ԐM�v���j���[�p�n���h��
function on_mixi_echo_reply(serialize_key, event_name, data)
	-- ���[�h�ύX
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY);

	-- ���[�h�ύX���f(�{�^�����̕ύX)
	mz3_main_view.update_control_status();

	-- �t�H�[�J�X�ړ�
	mz3_main_view.set_focus('edit');
end

--- �u�v���t�B�[���v���j���[�p�n���h��
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
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");


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


--- �uxxx �̃{�C�X�v���j���[�p�n���h��
function on_mixi_echo_add_user_echo_list(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- �J�e�S���ǉ�
	title = name .. "����̃{�C�X";
	author_id = body:get_integer('author_id');
	url = "http://mixi.jp/list_echo.pl?id=" .. author_id;
	key = "MIXI_RECENT_ECHO";
	mz3_main_view.append_category(title, url, key);
	
	-- �ǉ������J�e�S���̎擾�J�n
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- ���p���[�U�̃{�C�X�ǉ����j���[�p�n���h��
function on_mixi_echo_add_ref_user_echo_list(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('ref_user_name');
	
	-- �J�e�S���ǉ�
	title = name .. "����̃{�C�X";
	author_id = body:get_integer('ref_user_id');
	url = "http://mixi.jp/list_echo.pl?id=" .. author_id;
	key = "MIXI_RECENT_ECHO";
	mz3_main_view.append_category(title, url, key);
	
	-- �ǉ������J�e�S���̎擾�J�n
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
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
	menu:append_menu("string", body:get_text('name') .. " ����̃{�C�X", menu_items.mixi_echo_add_user_echo_list);

	-- ���p���[�U�̃G�R�[�ꗗ
	ref_user_name = body:get_text('ref_user_name');
	if ref_user_name ~= "" then
		menu:append_menu("string", ref_user_name .. " ����̃{�C�X", menu_items.mixi_echo_add_ref_user_echo_list);
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
mz3.add_event_listener("popup_body_menu", "mixi.on_popup_body_menu");


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
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group", false);


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

    -- �G�R�[
	if line_has_strings(url, 'recent_echo.pl?') then
		return true, mz3.get_access_type_by_key('MIXI_RECENT_ECHO');
	end

	return false;
end
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type");


--- ���|�[�g��ʂ���̏������ݎ�ʂ̔���
--
-- @param event_name  'get_write_view_type_by_report_item_access_type'
-- @param report_item [MZ3Data] ���|�[�g��ʂ̗v�f
--
function on_get_write_view_type_by_report_item_access_type(event_name, report_item)

	report_item = MZ3Data:create(report_item);
	
	serialize_key = report_item:get_serialize_key();
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='mixi' then
		if serialize_key=='MIXI_MESSAGE' then
			-- ���b�Z�[�W�ԐM�̏������ݎ��
			return true, mz3.get_access_type_by_key('MIXI_POST_REPLYMESSAGE_ENTRY');
		else
			-- �R�����g�ԐM�̏������ݎ��
			return true, mz3.get_access_type_by_key('MIXI_POST_COMMENT_CONFIRM');
		end
	end

	return false;
end
mz3.add_event_listener("get_write_view_type_by_report_item_access_type", "mixi.on_get_write_view_type_by_report_item_access_type");


--- �������݉�ʂŁu�摜���Y�t�\�ȃ��[�h���v�̔���
--
-- @param event_name      'is_enable_write_view_attach_image_mode'
-- @param write_view_type �������ݎ��
-- @param write_item      [MZ3Data] �������݉�ʂ̗v�f
--
function on_is_enable_write_view_attach_image_mode(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type=='mixi' then
		if write_view_key=="MIXI_POST_REPLYMESSAGE_ENTRY" or
		   write_view_key=="MIXI_POST_NEWMESSAGE_ENTRY" then
			-- ���b�Z�[�W�A���b�Z�[�W�ԐM�͓Y�t�s��
			return true, 0;
		end
		
		if write_view_key=="MIXI_POST_COMMENT_CONFIRM" then
			local serialize_key = write_item:get_serialize_key();
			if serialize_key=="MIXI_BBS" or
			   serialize_key=="MIXI_EVENT" or
			   serialize_key=="MIXI_EVENT_JOIN" then
				-- �R�����g�� BBS, EVENT �ł���ΓY�t��
				return true, 1;
			else
				-- ��L�ȊO(���L�R�����g�A�A���P�[�g�R�����g��)�͓Y�t�s��
				return true, 0;
			end
		end
		
		if write_view_key=="MIXI_POST_NEWDIARY_CONFIRM" then
			-- ���L�͓Y�t��
			return true, 1;
		end
		
		return false;
	end

	return false;
end
mz3.add_event_listener("is_enable_write_view_attach_image_mode", "mixi.on_is_enable_write_view_attach_image_mode");


--- �X�V�{�^�������C�x���g
--
-- @param event_name    'click_update_button'
-- @param serialize_key Twitter���������݃��[�h�̃V���A���C�Y�L�[
--
function on_click_update_button(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="mixi" then
		return false;
	end

	-- ���͕�������擾
	text = mz3_main_view.get_edit_text();

	-- �����͎��̏���
	if text == '' then
		if serialize_key == 'MIXI_ADD_ECHO_REPLY' then
			-- �����͂�NG => ���������ɏI��
			return true;
		elseif serialize_key == 'MIXI_ADD_ECHO' then
			-- �ŐV�擾
			mz3_main_view.retrieve_category_item();
			return true;
		else
			-- ��L�ȊO��NG�B
			mz3.alert('���T�|�[�g�̃A�N�Z�X��ʂł�');
			return true;
		end
	end

	-- �m�F
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'MIXI_ADD_ECHO' then
		msg = 'mixi �G�R�[�Ŕ������܂��B\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. '��낵���ł����H';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		local username = data:get_text('name');
		msg = 'mixi �G�R�[�� ' .. username .. ' ����ɕԐM���܂��B\n'
		   .. '---- ���� ----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. '��낵���ł����H';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	if serialize_key == 'MIXI_ADD_ECHO' then
		-- �P�����e�͋��ʏ����ŁB

		-- �N���X�|�X�g�Ǘ��f�[�^������
		mz3.init_cross_post_info("echo");

		do_post_to_echo(text);
		return true;
	end
	
	-- POST �p�����[�^��ݒ�
	post = MZ3PostData:create();
	local post_key = mixi.post_key;
	if post_key=='' then
		mz3.alert('���M�p�̃L�[��������܂���B�G�R�[�ꗗ�������[�h���ĉ������B');
		return true;
	end
	if serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		-- body=test&x=36&y=12&parent_member_id=xxx&parent_post_time=20090626110655&redirect=recent_echo&post_key=xxx
		local echo_member_id = data:get_integer('author_id');
		local echo_post_time = data:get_text('echo_post_time');
		
		if echo_member_id == -1 then
			mz3.alert('�ԐM�惆�[�U���s���ł�');
			return true;
		end
		if echo_post_time == '' then
			mz3.alert('�ԐM�Ώ�POST�̎������s���ł�');
			return true;
		end
		
		post:append_post_body('body=');
		post:append_post_body(mz3.url_encode(text, 'euc-jp'));
		post:append_post_body('&x=28&y=20');
 		post:append_post_body('&parent_member_id=' .. echo_member_id);
 		post:append_post_body('&parent_post_time=' .. echo_post_time);
		post:append_post_body('&redirect=recent_echo');
		post:append_post_body('&post_key=');
		post:append_post_body(post_key);
	end
	
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost �̊m�F
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST��URL�ݒ�
	if serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		url = 'http://mixi.jp/add_echo.pl';
	end
	
	-- �ʐM�J�n
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end
mz3.add_event_listener("click_update_button", "mixi.on_click_update_button");


--- echo �ɓ��e����
function do_post_to_echo(text)

	serialize_key = 'MIXI_ADD_ECHO'

	post = MZ3PostData:create();
	local post_key = mixi.post_key;
	if post_key=='' then
		mz3.alert('���M�p�̃L�[��������܂���B�G�R�[�ꗗ�������[�h���ĉ������B');
		return true;
	end
	post:append_post_body('body=');
	post:append_post_body(mz3.url_encode(text, 'euc-jp'));
	post:append_post_body('&x=28&y=20');
	post:append_post_body('&post_key=');
	post:append_post_body(post_key);
	post:append_post_body('&redirect=recent_echo');

	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost �̊m�F
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST��URL�ݒ�
	url = 'http://mixi.jp/add_echo.pl';
	
	-- �ʐM�J�n
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end


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
	if service_type~="mixi" then
		return false;
	end

	-- ���e��������
	
	if serialize_key == 'MIXI_ADD_ECHO' or 
	   serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		if mz3.is_mixi_logout(serialize_key) then
			mz3.alert('�����O�C���ł��B�G�R�[�ꗗ�������[�h���Amixi�Ƀ��O�C�����ĉ������B');
			return true;
		else
			-- ���e����
			mz3_main_view.set_info_text("�G�R�[�������݊���");

			-- �N���X�|�X�g
			if serialize_key == "MIXI_ADD_ECHO" then
				if mz3.do_cross_post() then
					return true;
				end
			end

			-- ���͒l������
			mz3_main_view.set_edit_text("");

			return true;
		end
	end
end
mz3.add_event_listener("post_end", "mixi.on_post_end");


--- �J�e�S���擾���̃n���h��
--
-- @param event_name    'retrieve_category_item'
-- @param serialize_key �J�e�S���A�C�e���̃V���A���C�Y�L�[
--
function on_retrieve_category_item(event_name, serialize_key, body, wnd)
	if serialize_key~="BOOKMARK" then
		return false;
	end
	
	-- �u�b�N�}�[�N�̓��[�J���X�g���[�W
	return true, RETRIEVE_CATEGORY_ITEM_RVAL_LOCALSTORAGE;
end
mz3.add_event_listener("retrieve_category_item", "mixi.on_retrieve_category_item");


----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- �R�~���j�e�B�ŐV�����ꗗ
require("scripts\\mixi\\mixi_new_bbs_parser");

-- �g�b�v�y�[�W
require("scripts\\mixi\\mixi_home_parser");

-- ���L�ڍ�
require("scripts\\mixi\\mixi_view_diary_parser");

-- ���b�Z�[�W(��M��, ���M��), �������b�Z�[�W, ���b�Z�[�W�ڍ�
require("scripts\\mixi\\mixi_new_official_message_parser");
require("scripts\\mixi\\mixi_message_outbox_parser");
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- �t��������
require("scripts\\mixi\\mixi_show_self_log_parser");

-- ���C�ɓ���R�~���A���[�U
require("scripts\\mixi\\mixi_bookmark_community_parser");
require("scripts\\mixi\\mixi_bookmark_user_parser");

-- �j���[�X�֘A���L
require("scripts\\mixi\\mixi_news_quote_diary_parser");

mz3.logger_debug('mixi.lua end');
