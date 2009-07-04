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

-- �G����
type = MZ3AccessTypeInfo.create();
type:set_info_type('other');									-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('GMAIL_EMOJI');							-- �V���A���C�Y�L�[
type:set_short_title('GMail �G����');							-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_request_encoding('utf8');								-- �G���R�[�f�B���O

-- ���[�����M
type = MZ3AccessTypeInfo:create();
type:set_info_type('post');										-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('GMAIL_SEND');							-- �V���A���C�Y�L�[
type:set_short_title('GMail ���M');								-- �ȈՃ^�C�g��
type:set_request_method('POST');								-- ���N�G�X�g���\�b�h
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
		
--		continue_value = continue_value:gsub('&amp;', '&');
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
function gmail_mail_parser(data, dummy, html)
	mz3.logger_debug("gmail_mail_parser start");

	-- ���C���r���[�A�{�f�B���X�g�̃A�C�R�������ǂɂ���
	mz3_data.set_integer(mz3_main_view.get_selected_body_item(), 'is_new', 0);
	mz3_main_view.redraw_body_images();
	
	
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
	
	-- base url �̉��
	-- <base href="https://mail.google.com/mail/h/xxx/">
	base_url  = line:match('<base href="(.-)">');
	data:set_text('base_url', base_url);
	if base_url~=nil then
		base_host = base_url:match('(https?://.-)/');
	end
	
	-- �u�S�ēW�J�v=�S�X���b�h�\���Ή�
	-- ���������[�v�΍�̂��߁A���C����ʂ���J�ڂ����ꍇ�̂ݎ��{����
	if read_gmail_mail_first then
		read_gmail_mail_first = false;

		-- <a href="?v=c&d=e&th=xxx" class="nu">
		-- <img src="/mail/images/expand_icon.gif" width="16" height="16" border="0" alt="���ׂẴ��b�Z�[�W��W�J">
		-- &nbsp;<span class="u">���ׂēW�J</span></a>
		
		if line_has_strings(line, 'alt="���ׂẴ��b�Z�[�W��W�J"') then
			expand_url = line:match('<a href="([^">]+)"[^>]+><img[^>]+>&nbsp;<span[^>]+>���ׂēW�J');
--			mz3.alert(expand_url);
--			expand_url = nil;
			if expand_url ~= nil then
				mz3.logger_debug('�S�X���b�h���擾���܂�');

				data:add_text_array("body", "�S�X���b�h���擾���Ă��܂��B���΂炭���҂��������B�B�B");

				-- �ʐM�J�n
				url = base_url .. expand_url;
				key = "GMAIL_MAIL";
				access_type = mz3.get_access_type_by_key(key);
				referer = '';
				user_agent = nil;
				post = nil;
				mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
				return;
			end
		end
	end
	
	-- �^�C�g��
	-- <h2><font size="+1"><b>�����Ƃ�</b></font></h2>
	title = line:match('<h2><font size=.-><b>(.-)</b>');
	data:set_text('title', mz3.decode_html_entity(title));
	
	-- �X���b�h����
	-- �����table�Ɓu�ԐM�J�n�^�O�v�ŕ�������
	
	one_mail_start_tags = '<table width="100%" cellpadding="1" cellspacing="0" border="0" bgcolor="#efefef"> <tr> <td> ';
	reply_start_tags    = '<table width="100%" cellpadding="1" cellspacing="0" border="0" bgcolor="#e0ecff" class="qr"> <tr> ';
	
	-- �G����URL���X�g
	-- ��) emoji_urls[0] = 'https://mail.google.com/mail/e/ezweb_ne_jp/B60';
	emoji_urls = {}
	local one_mail = '';
	local mail_count = 0;
	local start = 1;
	local pos = 1;
	while true do
		-- ���[���J�n�^�O��T��
		pos = line:find(one_mail_start_tags, start, true);
		if pos == nil then
			-- �u�ԐM�J�n�^�O�v�܂ł�1�ʂƂ���
			pos = line:find(reply_start_tags, start, true);
			if pos == nil then
				-- �u�ԐM�J�n�^�O�v���Ȃ��̂ōŌ�܂ŁB
				one_mail = line:sub(start);
			else
				one_mail = line:sub(start, pos-1);
			end

			-- ���`�AData��
			parse_one_mail(data, one_mail, mail_count);
			break;
		else
			one_mail = line:sub(start, pos-1);
			
			-- ���`�AData��
			parse_one_mail(data, one_mail, mail_count);
			
			start = pos + one_mail_start_tags:len();
			mail_count = mail_count +1;
		end
	end
	
	if pos ~= nil then
		-- �u�ԐM�J�n�^�O�v�ȍ~��p���ĕԐM�t�H�[�����擾
		reply_form = line:sub(pos);
		-- �ŏ��� <form>..</form> ���ԐM�t�H�[���B
		reply_form = reply_form:match('<form.-</form>');
		-- data �Ƀt�H�[�������̂܂ܖ��ߍ���ł���
		-- �ԐM���s���ɂ��̃t�H�[���̓��e�𗘗p����
		data:set_text('reply_form', reply_form);
	end
	
--	for k, v in pairs(emoji_urls) do
--		print(k, v);
--	end
--	print(#emoji_urls);
	-- �����[�h�̊G����������΃��[�h�J�n
	get_next_emoji_url();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_mail_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_MAIL", "gmail.gmail_mail_parser");


--- emoji_urls �̐擪�v�f�ɑ΂��ă��N�G�X�g����
function get_next_emoji_url()
	if #emoji_urls >= 1 then
		-- �ʐM�J�n
		url = emoji_urls[1];
--		mz3.alert(url);
		access_type = mz3.get_access_type_by_key("GMAIL_EMOJI");
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_report_view.get_wnd(), access_type, url, referer, "binary", user_agent, post);
		return true;
	else
		return false;
	end
end


--- �X���b�h����1�ʂ̃��[������͂��Adata �ɐݒ肷��
--
-- @param data MZ3Data �I�u�W�F�N�g
-- @param line 1�ʂ̃��[���ɑΉ�����HTML
--
function parse_one_mail(data, line, count)

	mz3.logger_debug("parse_one_mail(" .. count .. ")");

	if count==0 then
		-- �ŏ��̓w�b�_�[�Ȃ̂Ŗ���
		return;
	end

	if count>=2 then
		-- 2���ڈȍ~�͎q�v�f�Ƃ��ē�������
		child = MZ3Data:create();
		parse_one_mail(child, line, 1);
		data:add_child(child);
		child:delete();
		return;
	end
--	mz3.logger_debug(line);

	-- ���O
	-- <h3>... <b>�Ȃ܂�</b> ...</h3>
	name = line:match('<h3>.-<b>(.-)</b>');
	data:set_text("name", mz3.decode_html_entity(name));
	data:set_text("author", mz3.decode_html_entity(name));
	
	-- ���t
	-- <td align="right" valign="top"> 2009/05/24 8:17 <tr>
	date = line:match('<td align="right" valign="top"> (.-) <');
	if date ~= nil then
		date = date:gsub('<.->', '');
		data:set_date(date);
	end
	
	-- �{��
	body = line:match('<div class="msg"> ?(.*)$');
	
	if body ~= nil then
		-- �Ȉ�HTML���
		body = body:gsub('<WBR>', '');
		body = body:gsub('<wbr />', '');
		body = body:gsub('<b .->', "<b>");
		body = body:gsub('<p .->', "<p>");
		body = body:gsub('<h2.->(.-)</h2>', '<b>%1</b>');
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
		local post = 1;
		local start = 1;
		local body2 = '';
		while true do
			pos = body:find('<img', start, true);
			if pos == nil then
				body2 = body2 .. body:sub(start);
				break;
			else
				body2 = body2 .. body:sub(start, pos-1);
				img = body:match('<img .->', start);
				if line_has_strings(img, 'src=') then
					-- ����ɊG�����ł���Εϊ�
					-- <img src="https://mail.google.com/mail/e/ezweb_ne_jp/B60" goomoji="ezweb_ne_jp.B60" ... />
					emoji_url, goomoji = img:match('src="(https://mail.google.com/mail/e/ezweb_ne_jp/.-)" goomoji="(.-)"');
					if emoji_url ~= nil and goomoji ~= nil then
						-- �����[�h�̊G����������΃_�E�����[�h����
						local idx = mz3_image_cache.get_image_index_by_url(emoji_url);
						if idx==-1 then
							-- �����[�h�Ȃ̂Ń_�E�����[�h(�\��)����
							table.insert(emoji_urls, emoji_url);
							body2 = body2 .. "[loading...]";
						else
							-- ���[�h�ς݂Ȃ̂�idx�������r���[�`���ɕϊ�����
							body2 = body2 .. "[g:" .. idx .. "]";
						end
					else
						body2 = body2 .. img;
					end
				end
				start = pos + img:len();
			end
		end

		body2 = body2:gsub('<a [^>]*></a>', "");
--		print(body2);

		data:add_text_array("body", "\r\n");
		data:add_body_with_extract(body2);
	end

end


----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X�擾
--
-- @param event_name    'get_end_binary_report_view'
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param http_status   http status
-- @param url           url
--
-- @return (1) [bool] �������� true, ���s���� false
--
function on_get_end_binary_report_view(event_name, serialize_key, http_status, url, filename)

	mz3.logger_debug('on_get_end_binary_report_view', event_name, serialize_key, http_status, url, filename);
	if serialize_key == "GMAIL_EMOJI" then
		-- �ۑ�
		local path = mz3.make_image_logfile_path_from_url_md5(url);
		mz3.logger_debug(path);
--		mz3.alert(path);
		mz3.copy_file(filename, path);
		
		-- �G�������X�g����폜
		table.remove(emoji_urls, 1);

		-- �����[�h�̊G����������΃��[�h�J�n
		if get_next_emoji_url()==false then
			-- �S���[�h����
			-- �Ƃ肠�����ēx���[������M����
			-- TODO ��ʍĕ`��݂̂ɂ�����
			url = mail_url;
			key = "GMAIL_MAIL";
			access_type = mz3.get_access_type_by_key(key);
			referer = '';
			user_agent = nil;
			post = nil;
			mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
		end

		return true;
	end

	return false;
end
mz3.add_event_listener("get_end_binary_report_view", "gmail.on_get_end_binary_report_view");

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
read_gmail_mail_first = true;
function on_read_by_reportview_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_by_reportview_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	-- ���[���p�[�T���������[�v�΍�̂��߁A�u���C����ʂ���̑J�ڃt���O�v�𗧂ĂĂ���
	read_gmail_mail_first = true;

	data = MZ3Data:create(data);

	-- �ʐM�J�n
	url = data:get_text('url');
	key = "GMAIL_MAIL";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	
	-- �G�������[�h��̍ă��[�h�p
	-- TODO ��ʍĕ`��݂̂ɂ�����
	mail_url = url;

	return true;
end


--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)

	if serialize_key=="GMAIL_MAIL" then
		-- ���|�[�g�r���[�ŊJ��
		return on_read_by_reportview_menu_item(serialize_key, event_name, data);

		-- �_�u���N���b�N�őS���\���������ꍇ�͉��L�̃R�����g���O������
		-- �S���\��
--		return on_read_menu_item(serialize_key, event_name, data);
	end
	
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


--- ���|�[�g�r���[�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_report_menu'
-- @param serialize_key ���|�[�g�A�C�e���̃V���A���C�Y�L�[
-- @param report_item   ���|�[�g�A�C�e��
-- @param sub_item_idx  �I���A�C�e���̃C���f�b�N�X
-- @param wnd           wnd
--
function on_popup_report_menu(event_name, serialize_key, report_item, sub_item_idx, wnd)
	if serialize_key~="GMAIL_MAIL" then
		return false;
	end

	-- �C���X�^���X��
	report_item = MZ3Data:create(report_item);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	menu_edit = MZ3Menu:create_popup_menu();
	menu_layout = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "�߂�", ID_BACK_MENU);
	menu:append_menu("separator");
	
	menu:append_menu("string", "�ԐM", ID_WRITE_COMMENT);

	menu:append_menu("string", "�ēǍ�", IDM_RELOAD_PAGE);

	menu_edit:append_menu("string", "�R�s�[", ID_EDIT_COPY);
	menu:append_submenu("�ҏW", menu_edit);
	
	menu:append_menu("string", "�u���E�U�ŊJ���i���̃y�[�W�j...", ID_OPEN_BROWSER);

	-- TODO ���ʉ�
	menu:append_menu("separator");
	menu_layout:append_menu("string", "�����X�g����������", IDM_LAYOUT_REPORTLIST_MAKE_NARROW);
	menu_layout:append_menu("string", "�����X�g���L������", IDM_LAYOUT_REPORTLIST_MAKE_WIDE);
	menu:append_submenu("��ʃ��C�A�E�g", menu_layout);

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	menu_edit:delete();
	menu_layout:delete();
	
	return true;
end
mz3.add_event_listener("popup_report_menu",  "gmail.on_popup_report_menu");


--- ���|�[�g��ʂ���̕ԐM���̏������ݎ�ʂ̔���
--
-- @param event_name  'get_write_view_type_by_report_item_access_type'
-- @param report_item [MZ3Data] ���|�[�g��ʂ̗v�f
--
function on_get_write_view_type_by_report_item_access_type(event_name, report_item)

	report_item = MZ3Data:create(report_item);
	
	serialize_key = report_item:get_serialize_key();
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gmail' then
		if serialize_key=='GMAIL_MAIL' then
			return true, mz3.get_access_type_by_key('GMAIL_SEND');
		end
	end

	return false;
end
mz3.add_event_listener("get_write_view_type_by_report_item_access_type", "gmail.on_get_write_view_type_by_report_item_access_type");


--- �������݉�ʂ̏������C�x���g
--
-- @param event_name      'init_write_view'
-- @param write_view_type �������ݎ��
-- @param write_item      [MZ3Data] �������݉�ʂ̗v�f
--
function on_init_write_view(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if write_view_key=='GMAIL_SEND' then
		-- TODO �^�C�g���ύX�F�L����

		-- �^�C�g���̏����l�ݒ�
		local title = 'Re: ' .. write_item:get_text('title');
		mz3_write_view.set_text('title_edit', title);
		
		-- TODO ���J�͈̓R���{�{�b�N�X�F����
		
		-- TODO �t�H�[�J�X�F�{������J�n
		
		return true;
	end

	return false;
end
mz3.add_event_listener("init_write_view", "gmail.on_init_write_view");


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
	if service_type=='gmail' then
		-- �Ƃ肠�����Y�t�s��
		return true, 0;
	end

	return false;
end
mz3.add_event_listener("is_enable_write_view_attach_image_mode", "gmail.on_is_enable_write_view_attach_image_mode");


--- �������݉�ʂ̏������݃{�^�������C�x���g
--
-- @param event_name      'click_write_view_send_button'
-- @param write_view_type �������ݎ��
-- @param write_item      [MZ3Data] �������݉�ʂ̗v�f
--
function on_click_write_view_send_button(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type=='gmail' then
		-- ���M����
		
		local title = mz3_write_view.get_text('title_edit');
		local body  = mz3_write_view.get_text('body_edit');
		
		if title=='' then
			mz3.alert('�^�C�g������͂��Ă�������');
			return true;
		end
		
		local reply_form = write_item:get_text('reply_form');
		if reply_form==nil then
			mz3.alert('�ԐM�ł��܂���');
			return true;
		end
		
		-- <b>To:</b> <input type="hidden" name="qrr" value="o"> xxx@xxx.jp</td>
		-- <input type="radio" id="reply" name="qrr" value="o" checked> </td> <td colspan="2"> <label for="reply"><b>To:</b> NK &lt;xxx@xxx.jp&gt;</label> </td>
		local mail_to = reply_form:match('<input type="hidden" name="qrr" value="o".-> ?(.-)</');
		if mail_to==nil then
			mail_to = reply_form:match('<input type="radio" id="reply" name="qrr" value="o".-<b>To:</b> ?(.-)</');
		end
		msg = mz3.decode_html_entity(mail_to) .. ' �Ƀ��[���𑗐M���܂��B��낵���ł����H' .. "\r\n";
		msg = msg .. '----' .. "\r\n";
		msg = msg .. title .. "\r\n";
--		msg = msg .. '----';
		if mz3.confirm(msg, nil, "yes_no") ~= 'yes' then
			return true;
		end
		
		------------------------------------------
		-- POST �p�����[�^����
		------------------------------------------

		-- URL �擾
		-- <form action="?v=b&qrt=n&..." name="qrf" method="POST"> 
		local url = reply_form:match('<form action="(.-)"');
		if url==nil then
			mz3.alert('�ԐM�ł��܂���(���M��url�擾���s)');
			return true;
		end
		url = write_item:get_text('base_url') .. url;
		--mz3.alert(url);
		
		-- hidden �l�̎��W
		local redir = reply_form:match('<input type="hidden" name="redir" value="(.-)"');
		redir = redir:gsub('&amp;', '&');
		local qrr   = reply_form:match('<input type="hidden" name="qrr" value="(.-)"');
		if qrr==nil then
			qrr = 'o';
		end
		
		
		-- POST�p�����[�^����
		post = MZ3PostData:create();
		post:set_content_type('multipart/form-data; boundary=---------------------------7d62ee108071e' .. '\r\n');
		
		-- nvp_bu_send
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="nvp_bu_send"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding('���M', 'sjis', 'utf8') .. '\r\n');
		
		-- redir
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="redir"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(redir, 'sjis', 'utf8') .. '\r\n');

		-- qrr
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="qrr"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(qrr, 'sjis', 'utf8') .. '\r\n');
		
		-- subject
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="subject"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(title, 'sjis', 'utf8'));
		post:append_post_body('\r\n');
		
		-- body
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="body"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(body, 'sjis', 'utf8'));
--		mz3.alert(string.format('%c%c', 0x82, 0xa0));
--		post:append_post_body(string.format('%c%c%c', 0xEE, 0x95, 0x81));
		post:append_post_body('\r\n');
		-- ucs2 0xE541   = 1110 0101 0100 0001
		-- utf8 0xEE9581 = 1110 1110 1001 0101 1000 0001
		--                 ~~~~      ~~        ~~
		
		-- end of post data
		post:append_post_body('-----------------------------7d62ee108071e--' .. '\r\n');

		-- �ʐM�J�n
		access_type = mz3.get_access_type_by_key("GMAIL_SEND");
		referer = write_item:get_text('url');
		user_agent = nil;
		mz3.open_url(mz3_write_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);
		return true;
	end

	return false;
end
mz3.add_event_listener("click_write_view_send_button", "gmail.on_click_write_view_send_button");


--- �������݉�ʂ̏������݊����C�x���g
--
-- @param event_name      'get_end_write_view'
-- @param write_view_type �������ݎ��
-- @param write_item      [MZ3Data] �������݉�ʂ̗v�f
-- @param http_status     HTTP Status Code (200, 404, etc...)
-- @param filename        ���X�|���X�t�@�C��
-- 
--
function on_get_end_write_view(event_name, write_view_type, write_item, http_status, filename)
	-- GMail ���[�����M�ł͓��e��Ƀ��_�C���N�g���邽�� get_end �ɂȂ�B
	
	-- CWriteView::OnPostEnd �Ɠ��l�̏������s���B
	
	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type~='gmail' then
		return false;
	end

	-- ���e�����`�F�b�N
	if http_status==200 then
		-- ����
		
		-- ���b�Z�[�W�\��
		mz3.alert('���M���܂���');
		
		-- ������
		mz3_write_view.set_text('title_edit', '');
		mz3_write_view.set_text('body_edit', '');
		
		-- �O�̉�ʂɖ߂�
		mz3.change_view('main_view');
		
	else
		-- ���s
		mz3.logger_error('���s:' .. http_status);
		
		mz3.alert('���e�Ɏ��s���܂����B');
		
		-- TODO �o�b�N�A�b�v
	end
	
	return true;
end
mz3.add_event_listener("get_end_write_view", "gmail.on_get_end_write_view");


mz3.logger_debug('gmail.lua end');
