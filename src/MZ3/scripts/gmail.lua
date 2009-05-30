--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : gmail
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('gmail.lua start');
module("gmail", package.seeall)

--------------------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������A���O�C���ݒ�p)
--------------------------------------------------
mz3.regist_service('gmail', true);

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('GMail', 'id_name', '���[���A�h���X');
mz3_account_provider.set_param('GMail', 'password_name', '�p�X���[�h');


----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------

-- ��M�g���C
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('GMAIL_INBOX');							-- �V���A���C�Y�L�[
type:set_short_title('GMail ��M�g���C');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('gmail\\inbox_{urlparam:s}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('utf8');								-- �G���R�[�f�B���O
type:set_default_url('https://mail.google.com/mail/h/');
--type:set_default_url('https://integration.auone.jp/login/CMN2101E01.do');
type:set_body_header(1, 'title', '����');
type:set_body_header(2, 'name', '���o�l>>');
type:set_body_header(3, 'date', '���t>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');

-- ���O�C���p
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('GMAIL_LOGIN');							-- �V���A���C�Y�L�[
type:set_short_title('GMail ���O�C��');							-- �ȈՃ^�C�g��
type:set_request_method('POST');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('gmail\\login.html');				-- �L���b�V���t�@�C��
type:set_request_encoding('sjis');								-- �G���R�[�f�B���O

-- ���[���{��
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');										-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('GMAIL_MAIL');							-- �V���A���C�Y�L�[
type:set_short_title('GMail ���[��');							-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('gmail\\mail.html');				-- �L���b�V���t�@�C��
type:set_request_encoding('utf8');								-- �G���R�[�f�B���O


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read               = mz3_menu.regist_menu("gmail.on_read_menu_item");
menu_items.read_by_reportview = mz3_menu.regist_menu("gmail.on_read_by_reportview_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("gmail.on_open_by_browser_menu_item");
--menu_items.add_star           = mz3_menu.regist_menu("gmail.on_add_star_menu_item");


----------------------------------------
-- ���j���[�ւ̓o�^
----------------------------------------

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' gmail', 1, true) ~= nil then
		-- ��M�g���C
		local tab = MZ3GroupItem:create("GMail");

		tab:append_category("��M�g���C", "GMAIL_INBOX");
		tab:append_category("�X�^�[�t��", "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=r');
		tab:append_category("���M�ς�",   "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=s');
		tab:append_category("���ׂ�",     "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=a');

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end


end
mz3.add_event_listener("creating_default_group", "gmail.on_creating_default_group");


----------------------------------------
-- �p�[�T
----------------------------------------

--------------------------------------------------
-- �y��M�g���C�z
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function gmail_inbox_parser(parent, body, html)
	mz3.logger_debug("gmail_inbox_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	-- ���O�C������
	is_logged_in = false;
	GALX = '';
	continue_value = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		-- <input id="sbb" type="submit" name="nvp_site_mail" value="Ұق�����" />
		-- ��L������΃��O�C���� = ���Ɏ�M��
		if line_has_strings(line, '<input', '"nvp_site_mail"') then
			is_logged_in = true;
			break;
		end
		
		if line_has_strings(line, 'name="GALX"') then
			i = i+1;
			line = html:get_at(i);
			GALX = line:match('value="([^"]*)"');
--			mz3.alert(GALX);
		elseif line_has_strings(line, 'name="continue"') then
			i = i+1;
			line = html:get_at(i);
			continue_value = line:match('value="([^"]*)"');
--			mz3.alert(continue_value);
		end
		
--		mz3.logger_debug(line);
	end
	
	if is_logged_in then
--		mz3.alert('���O�C����');
		
		-- �����s�ɕ�����Ă���̂�1�s�Ɍ���
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end

		-- ���O�C���ς݂�HTML�̃p�[�X
		parse_gmail_inbox(parent, body, line);
	else
		-- ���O�C������

		mail_address  = mz3_account_provider.get_value('GMail', 'id');
		mail_password = mz3_account_provider.get_value('GMail', 'password');
		
		if (mail_address == "" or mail_password == "") then
			mz3.alert("���[���A�h���X�ƃp�X���[�h�����O�C���ݒ��ʂŐݒ肵�ĉ�����");
			return;
		end

		-- URL ����
		url = "https://www.google.com/accounts/ServiceLoginAuth?service=mail";
		post = mz3_post_data.create();
		mz3_post_data.append_post_body(post, "Email=" .. mz3.url_encode(mail_address, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "Passwd=" .. mz3.url_encode(mail_password, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "service=mail&");
		mz3_post_data.append_post_body(post, "nui=5&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "btmpl=mobile&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "scc=1&");
		mz3_post_data.append_post_body(post, "GALX=" .. GALX .. "&");
		mz3_post_data.append_post_body(post, "PersistentCookie=yes&");
		mz3_post_data.append_post_body(post, "rmShown=1&");
		mz3_post_data.append_post_body(post, "continue=" .. mz3.url_encode(continue_value, 'utf8'));
		
		continue_value = continue_value:gsub('&amp;', '&');
--		mz3.alert('continue_value : ' .. mz3.url_encode(continue_value, 'utf8'));
--		mz3.alert(url);
		
		-- �ʐM�J�n
		access_type = mz3.get_access_type_by_key("GMAIL_LOGIN");
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_inbox_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_INBOX", "gmail.gmail_inbox_parser");


--- ���O�C���ς݂� GMail ��M�g���C�̉��
--
-- @param parent ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
-- @param body   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
-- @param line   HTML �S����1�s�Ɍ�������������
--
function parse_gmail_inbox(parent, body, line)
	mz3.logger_debug("parse_gmail_inbox start");

	-- <base href="https://mail.google.com/mail/h/xxx/">
	base_url = line:match('<base href="(.-)">');
--	mz3.alert(base_url);

	-- 1���[���� '<tr ' �Ŏn�܂�
	pos = line:find('<tr ', 1, true);
	if pos == nil then
		-- ��͒��~
		return;
	end

	pos = pos + 1;
	looping = true;
	while looping do
		found = line:find('<tr ', pos, true);
		if found == nil then
			looping = false;
			-- �Ō�̃��[���̂��Ƃ� </table> ������
			found = line:find('</table>', pos, true);
			if found == nil then
				-- </table> ���猩����Ȃ��͉̂������̂Œ��~
				break;
			end
		end

		-- 1���[���̒��o
		w = line:sub(pos, found-1);
--[[
<tr bgcolor="#E8EEF7"> 
<td width="1%" nowrap> 
<input type="checkbox" name="t" value="1216cfad69f86322"> 
<img src="/mail/images/cleardot.gif" width="15" height="15" border="0" alt=""> 
</td> 
<td width="25%"> Twitter (2)</td> 
<td width="73%"> 
<a href="?v=c&th=xxx"> 
<span class="ts">
<font size="1">
<font color="#006633"> 
</font>
</font> 
xxx����_�C���N�g���b�Z�[�W���͂��܂��� 
<font color="#7777CC"> 
xxx
</font>
</span> 
</a>
</td> 
<td width="1%" nowrap> 19:18 

<tr bgcolor="#ffffff"> 
<td width="1%" nowrap> 
<input type="checkbox" name="t" value="xx"> 
<img src="/mail/images/cleardot.gif" width="15" height="15" border="0" alt=""> 
</td> 
#�ȏ�A����
<td width="25%"> 
<b>xxx</b>
</td> 
#�ȏ�Aname
<td width="73%"> 
<a href="?v=c&th=xx"> 
#��L�AURL �̈ꕔ
<span class="ts">
<font size="1">
<font color="#006633"> 
</font>
</font> 
<b>�����Ƃ�</b> 
# ��L <b> �^�O�Atitle
<font color="#7777CC"> 
�ق�Ԃ�΂�����
�ق�Ԃ�΂����� &hellip;
</font>
# ��L�Aquote
</span> 
</a>
</td> 
<td width="1%" nowrap> 
<b>0:55</b> 
# ��L�A���t����
<tr bgcolor="#ffffff"> <td> 
...
]]
		name, href, span, date
			= w:match('<td.->.-</td>.-<td.-> (.-)</td>.-href="(.-)">.-<span.->(.-)</span>.-<td.->(.-)$');

		if name~=nil then

			-- data ����
			data = MZ3Data:create();
			
			--mz3.logger_debug(span);
			--mz3.logger_debug(date);
			
			-- span �ɂ� title, quote ���܂܂�邪�A�Ƃ肠�����S�� title �ɓ����
			title = span;
			title = title:gsub('&hellip;', '...');

			-- ���ǁE���ǔ���F<b> �^�O�̗L���ŁB
			is_new = line_has_strings(title, '<b>');
			if is_new then
				data:set_integer('is_new', 1);
			else
				data:set_integer('is_new', 0);
			end
			title = title:gsub('<.->', '');
			title = title:gsub('^ *', '');
			title = mz3.decode_html_entity(title);
			data:set_text("title", title);

			-- URL ���� : base_url �ƌ������Đ���
			url = base_url .. href;
			data:set_text("url", url);
			date = date:gsub('<.->', '');
			data:set_date(date);

			-- ���O
			name = name:gsub('<b>', '');
			name = name:gsub('</b>', '');
			data:set_text("name", mz3.decode_html_entity(name));
			data:set_text("author", name);

			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.get_access_type_by_key('GMAIL_MAIL');
			data:set_access_type(type);

			-- data �ǉ�
			body:add(data.data);
			
			-- data �폜
			data:delete();
		end
		pos = found + 1;
	end
end


--------------------------------------------------
-- �y���O�C���z
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function gmail_login_parser(parent, body, html)
	mz3.logger_debug("gmail_login_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	url = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, "<meta", "refresh") then
			url = line:match('0; url=([^"]*)');
--			mz3.alert(url);
		end
		
--		mz3.logger_debug(line);
	end
	url = url:gsub('&amp;', '&');
--	mz3.alert('url : ' .. url);
	if url == '' then
		mz3.alert('���O�C���Ɏ��s���܂����B\r\n���[���A�h���X�ƃp�X���[�h���m�F���Ă��������B');
		return;
	end
	
	-- �ʐM�J�n
	access_type = mz3.get_access_type_by_key("GMAIL_INBOX");
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_login_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_LOGIN", "gmail.gmail_login_parser");


--------------------------------------------------
-- �y���[���z
--
-- ����:
--   data:  ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy: NULL
--   html:  HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function gmail_mail_parser(data, dimmy, html)
	mz3.logger_debug("gmail_mail_parser start");
	
	-- wrapper�N���X��
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- �S����
	data:clear();
	
	local t1 = mz3.get_tick_count();
	
	local line_count = html:get_count();
	
	-- �����s�ɕ�����Ă���̂�1�s�Ɍ���
	line = '';
	for i=0, line_count-1 do
		line = line .. html:get_at(i);
	end
	
	-- TODO �����X���b�h�Ή�
	
	-- base url �̉��
	-- <base href="https://mail.google.com/mail/h/xxx/">
	base_url  = line:match('<base href="(.-)">');
	base_host = base_url:match('(https?://.-)/');
	
	-- �^�C�g��
	-- <h2><font size="+1"><b>�����Ƃ�</b></font></h2>
	title = line:match('<h2><font size=.-><b>(.-)</b>');
	data:set_text('title', mz3.decode_html_entity(title));
	
	-- ���O
	-- <h3><font color="#00681C"> <b>�Ȃ܂�</b> </font></h3>
	name = line:match('<h3><font color=.-> <b>(.-)</b>');
	data:set_text("name", mz3.decode_html_entity(name));
	data:set_text("author", name);
	
	-- ���t
	-- <td align="right" valign="top"> 2009/05/24 8:17 <tr>
	date = line:match('<td align="right" valign="top"> (.-) <');
	date = date:gsub('<.->', '');
	data:set_date(date);
	
	-- �{��
	-- <tr bgcolor="#ffffff">...<a name="m_">
	body = line:match('<tr bgcolor="#ffffff">(.-)<a name="m_">');

	-- �Ȉ�HTML���
	body = body:gsub('<WBR>', '');
	body = body:gsub('<b .->', "<b>");
	body = body:gsub('<p .->', "<p>");
	body = body:gsub('<b>', "\n<b>\n<br>");
	body = body:gsub('</b>', "<br>\n</b>\n");
	body = body:gsub('<br ?/>', "<br>");
	body = body:gsub('<font .->', "");
	body = body:gsub('</font>', "");
	body = body:gsub('<hr .->', "<hr>");
	body = body:gsub('<hr>', "<br>----------------------------------------<br>");

	body = body:gsub('<tr[^>]*>', "");
	body = body:gsub('<td[^>]*>', "");
	body = body:gsub('</tr>', "<br>");
	body = body:gsub('</td>', "");
	body = body:gsub('<table[^>]*>', "");
	body = body:gsub('</table>', "");
	body = body:gsub('<map.-</map>', '');

	-- ���������N�̕⊮(/�Ŏn�܂�ꍇ�Ƀz�X�g��⊮����)
	body = body:gsub('(<a .-href=")(/.-")', '%1' .. base_host .. '%2');
	body = body:gsub('(<img .-src=")(/.-")', '%1' .. base_host .. '%2');
	-- ���������N�̕⊮(?�Ŏn�܂�ꍇ��base��⊮����)
	body = body:gsub('(<a .-href=")(\?.-")', '%1' .. base_host .. '%2');
	body = body:gsub('(<img .-src=")\?(.-")', '%1' .. base_url .. '%2');
	body = body:gsub("\r\n", "\n");
	body = body:gsub('^ *', '');

	-- <img �^�O���� src ���Ȃ����͍̂폜
	start = 1;
	body2 = '';
	while true do
		pos = body:find('<img', start, true);
		if pos == nil then
			body2 = body2 .. body:sub(start);
			break;
		else
			body2 = body2 .. body:sub(start, pos-1);
			img = body:match('<img .->', start);
			if line_has_strings(img, 'src=') then
				body2 = body2 .. img;
			end
			start = pos + img:len();
		end
	end

	body2 = body2:gsub('<a [^>]*></a>', "");
--	print(body2);

	data:add_text_array("body", "\r\n");
	data:add_body_with_extract(body2);

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_mail_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_MAIL", "gmail.gmail_mail_parser");


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X�擾
--
-- @param event_name    'creating_default_group'
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param body          body data
--
-- @return (1) [bool] �������� true, ���s���� false
-- @return (2) [int] �A�C�R���C���f�b�N�X
--
function on_get_body_list_default_icon_index(event_name, serialize_key, body)

	if serialize_key == "GMAIL_MAIL" then
		if mz3_data.get_integer(body, 'is_new')~=0 then
			return true, 6;
		else
			return true, 7;
		end
	end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "gmail.on_get_body_list_default_icon_index");


--- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
	item = '';
	item = item .. "���O : " .. data:get_text('name') .. "\r\n";
	item = item .. "���t : " .. data:get_date() .. "\r\n";
	item = item .. "----\r\n";

	item = item .. data:get_text('title') .. "\r\n";
	
	item = item .. "\r\n";
	item = item .. data:get_text('url') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
end


--- ���|�[�g�r���[�ŊJ��
function on_read_by_reportview_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_by_reportview_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);

	-- �ʐM�J�n
	url = data:get_text('url');
	key = "GMAIL_MAIL";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

	return true;
end


--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)

	-- �_�u���N���b�N�őS���\���������ꍇ�͉��L�̃R�����g���O������
--	if serialize_key=="GMAIL_MAIL" then
--		-- �S���\��
--		return on_read_menu_item(serialize_key, event_name, data);
--	end
	
	-- �W���̏����𑱍s
	return false;
end
mz3.add_event_listener("dblclk_body_list", "gmail.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "gmail.on_body_list_click");


--- �u�u���E�U�ŊJ���v���j���[�p�n���h��
function on_open_by_browser_menu_item(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());

	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end


--- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key �{�f�B�A�C�e���̃V���A���C�Y�L�[
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="GMAIL_MAIL" then
		return false;
	end

	-- �C���X�^���X��
	body = MZ3Data:create(body);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "�{����ǂ�...", menu_items.read_by_reportview);
	menu:append_menu("string", "�u���E�U�ŊJ��...", menu_items.open_by_browser);

--	menu:append_menu("separator");
--	menu:append_menu("string", "�X�^�[��t����...", menu_items.add_star);

	menu:append_menu("separator");
	menu:append_menu("string", "���[���̃v���p�e�B...", menu_items.read);

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu",  "gmail.on_popup_body_menu");


mz3.logger_debug('gmail.lua end');
