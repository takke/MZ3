--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : auone
--
-- auone ���[���p�v���O�C��
--
-- ���O�C���ȊO�̏����� gmail.lua �Ɉˑ����Ă���B
--
-- $Id: auone.lua 1306 2009-06-21 14:37:54Z takke $
--------------------------------------------------
mz3.logger_debug('auone.lua start');
module("auone", package.seeall)

--------------------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������A���O�C���ݒ�p)
--------------------------------------------------
mz3.regist_service('auone', true);

-- ���O�C���ݒ��ʂ̃v���_�E�����A�\�����̐ݒ�
mz3_account_provider.set_param('auone', 'id_name', 'au one-ID');
mz3_account_provider.set_param('auone', 'password_name', '�p�X���[�h');


----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------

-- ��M�g���C
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('AUONE_INBOX');							-- �V���A���C�Y�L�[
type:set_short_title('auone ��M�g���C');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('auone\\inbox_{urlparam:s}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('utf8');								-- �G���R�[�f�B���O
type:set_default_url('https://mail.google.com/a/auone.jp/h/');
type:set_body_header(1, 'title', '����');
type:set_body_header(2, 'name', '���o�l>>');
type:set_body_header(3, 'date', '���t>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');

-- ���O�C���p
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('gmail');									-- �T�[�r�X���
type:set_serialize_key('AUONE_LOGIN');							-- �V���A���C�Y�L�[
type:set_short_title('auone ���O�C��');							-- �ȈՃ^�C�g��
type:set_request_method('POST');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('auone\\login.html');				-- �L���b�V���t�@�C��
type:set_request_encoding('utf8');								-- �G���R�[�f�B���O


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
	if services:find(' auone', 1, true) ~= nil then
		-- ��M�g���C
		local tab = MZ3GroupItem:create("auone");

		tab:append_category("��M�g���C", "AUONE_INBOX");
		tab:append_category("EZ���M�{�b�N�X", "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=l&l=EZ%E9%80%81%E4%BF%A1%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9');
		tab:append_category("���M�ς�",   "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=s');
		tab:append_category("�X�^�[�t��", "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=r');
		tab:append_category("���ׂ�",     "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=a');

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end

end
mz3.add_event_listener("creating_default_group", "auone.on_creating_default_group");


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
function auone_inbox_parser(parent, body, html, is_from_login_parser)
	mz3.logger_debug("auone_inbox_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	-- ���O�C������
	is_logged_in = false;
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		-- <input id="sbb" type="submit" name="nvp_site_mail" value="Ұق�����" />
		-- ��L������΃��O�C���� = ���Ɏ�M��
		if line_has_strings(line, '<input', '"nvp_site_mail"') then
			is_logged_in = true;
			break;
		end
	end
	
	if is_logged_in then
--		mz3.alert('���O�C����');
		
		-- �����s�ɕ�����Ă���̂�1�s�Ɍ���
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end

		-- ���O�C���ς݂̎�M�g���C�̃p�[�X
		gmail.parse_gmail_inbox(parent, body, line);
	else
		-- ���O�C������

		mail_address  = mz3_account_provider.get_value('auone', 'id');
		mail_password = mz3_account_provider.get_value('auone', 'password');
		
		if (mail_address == "" or mail_password == "") then
			mz3.alert("���[���A�h���X�ƃp�X���[�h�����O�C���ݒ��ʂŐݒ肵�ĉ�����");
			return;
		end

		-- �t�H�[�����
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end
		base_url = 'https://integration.auone.jp';
		forms = parse_form(line, base_url);
--		dump_forms(forms);
		
		form = nil;
		for k, v in pairs(forms) do
			if v.name=="CMN2101E01Dto" or	-- �����O�C�����͂�����B
			   v.name=="acsForm"			-- "CMN2101E01Dto" �̂��Ƃ͂�����B
			then
				form = v;
				break;
			end
		end
		if form==nil then
			mz3.alert("���O�C���Ɏ��s���܂����B���[���A�h���X�E�p�X���[�h���m�F���Ă��������B(form not found)");
			return;
		end
		if is_from_login_parser and form.name=="CMN2101E01Dto" then
			-- auone_login_parser ����Ăяo����Ă���̂ɖ����� "CMN2101E01Dto" �̃y�[�W��
			-- �擾�����Ƃ������Ƃ̓��O�C���Ɏ��s���Ă���Ƃ������ƁB
			mz3.alert("���O�C���Ɏ��s���܂����B���[���A�h���X�E�p�X���[�h���m�F���Ă��������B(loop)");
			return;
		end
		
		-- URL ����
		url = form.action;
		post = mz3_post_data.create();
		mz3_post_data.append_post_body(post, "auoneid=" .. mz3.url_encode(mail_address, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "password=" .. mz3.url_encode(mail_password, 'utf8') .. "&");
		first_in_for = true;
		for name, value in pairs(form.hidden) do
			if first_in_for==false then
				mz3_post_data.append_post_body(post, '&');
			end
			name = mz3.url_encode(name, 'utf8');
			value = mz3.url_encode(value, 'utf8');
			mz3_post_data.append_post_body(post, name .. '=' .. value);
			first_in_for = false;
		end
		
		-- �ʐM�J�n
		access_type = mz3.get_access_type_by_key("AUONE_LOGIN");
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("auone_inbox_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("AUONE_INBOX", "auone.auone_inbox_parser");


--------------------------------------------------
-- �y���O�C���z
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function auone_login_parser(parent, body, html)
	mz3.logger_debug("auone_login_parser start");
	local t1 = mz3.get_tick_count();
	
	-- ��M�g���C�p�[�T�𗘗p����B
	-- �A�N�Z�X��ʂ�ʓr�p�ӂ��Ă���̂̓A�N�Z�X���\�b�h(GET/POST)��
	-- ��M�g���C�ƃ��O�C�������ƂňقȂ邽�߁B
--	mz3.alert("auone_login_parser");
	auone_inbox_parser(parent, body, html, true);

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("auone_login_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("AUONE_LOGIN", "auone.auone_login_parser");


----------------------------------------
-- �C�x���g�n���h��
--
-- => gmail.lua �Q��
--
----------------------------------------


mz3.logger_debug('auone.lua end');
