--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id$
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------

-- ��M��
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('mixi');									-- �T�[�r�X���
type:set_serialize_key('MESSAGE_IN');							-- �V���A���C�Y�L�[
type:set_short_title('���b�Z�[�W(��M��)');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\list_message_inbox.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');							-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/list_message.pl');
type:set_body_header(1, 'title', '����');
type:set_body_header(2, 'name', '���o�l>>');
type:set_body_header(3, 'date', '���t>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);

-- ���M��
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('mixi');									-- �T�[�r�X���
type:set_serialize_key('MESSAGE_OUT');							-- �V���A���C�Y�L�[
type:set_short_title('���b�Z�[�W(���M��)');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\list_message_outbox.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');							-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/list_message.pl?box=outbox');
type:set_body_header(1, 'title', '����');
type:set_body_header(2, 'name', '���o�l>>');
type:set_body_header(3, 'date', '���t>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);

-- �������b�Z�[�W
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');										-- �J�e�S��
type:set_service_type('mixi');										-- �T�[�r�X���
type:set_serialize_key('MIXI_LIST_MESSAGE_OFFICIAL');				-- �V���A���C�Y�L�[
type:set_short_title('�������b�Z�[�W');								-- �ȈՃ^�C�g��
type:set_request_method('GET');										-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\list_message_official.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');								-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/list_message.pl?box=noticebox');
type:set_body_header(1, 'title', '����');
type:set_body_header(2, 'name', '���o�l>>');
type:set_body_header(3, 'date', '���t>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);


--------------------------------------------------
-- �ymixi ���b�Z�[�W(��M��), (���M��)�z
-- [list] list_message.pl �p�p�[�T
-- [list] list_message.pl?box=outbox �p�p�[�T
--
-- http://mixi.jp/list_message.pl
-- [list] list_message.pl?box=outbox �p�p�[�T
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_list_message_parser(parent, body, html)
	mz3.logger_debug("mixi_list_message_parser start");

	-- wrapper�N���X��
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;

	-- �s���擾
	local line_count = html:get_count();
	for i=140, line_count-1 do
		line = html:get_at(i);
		
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "list_message.pl");
		end

		-- ���ڒT��
		if line_has_strings(line, "td", "class", "subject") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			--	����
			--	<td class="subject">
			--		<a href="view_message.pl?id=*****&box=inbox&page=1">
			--			����
			--		</a>
			--	</td>
			-- ����
			title, after = line:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
			data:set_text("title", title);

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- 1�s�߂�
			i = i -1;
			line2 = html:get_at(i);

			-- ���O
			name, after = line2:match(">([^<]+)(<.*)$");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);
			data:set_text("author", name);

			-- 2�s�i��
			i = i +2;
			line3 = html:get_at(i);

			-- ���t�擾
			-- <td class="date">03��10��<a id=
			date, after = line3:match(">([^<]+)(<.*)$");
			data:set_date( date );

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- data �ǉ�
			body:add(data.data);

			-- data �폜
			data:delete();
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("��</ul>�����������̂ŏI�����܂�");
			break;
		end

	end

	-- �O�A���փ����N�̒ǉ�
	if back_data~=nil then
		-- �擪�ɑ}��
		body:insert(0, back_data.data);
		back_data:delete();
	end
	if next_data~=nil then
		-- �����ɒǉ�
		body:add(next_data.data);
		next_data:delete();
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_list_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


--------------------------------------------------
-- �y��������̃��b�Z�[�W�ꗗ�z
-- [list] list_message.pl?box=noticebox �p�p�[�T
--
-- http://mixi.jp/list_message.pl?box=noticebox
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_new_official_message_parser(parent, body, html)
	mz3.logger_debug("mixi_new_official_message_parser start");
	
	-- wrapper�N���X��
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;

	-- �s���擾
	local line_count = html:get_count();
	for i=140, line_count-1 do
		line = html:get_at(i);
		
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
--			back_data, next_data = parse_next_back_link(line, "list_message.pl?box=noticebox");
			back_data, next_data = parse_next_back_link(line, "view_message.pl");
		end

		-- ���ڒT��
		if line_has_strings(line, "<td", "class", "sender")  then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			-- ���t�̃p�[�X
			-- sender, subject, date �Ɖ��Cdate �ɓ��t������̂� +2 �c
			line3 = html:get_at(i +2);
			date = line3:match(">([^<]+)(<.*)$");
			date = mz3.decode_html_entity(date);
			data:set_date(date);
			
			-- ���s�擾
			i = i+1;
			line2 = html:get_at(i);
--			mz3.trace(i .. " : " .. line2);
			
			-- ���o��
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- �y�`���b�g�z�W���Ixxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = line2:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);

			data:set_text("title", title);
			
			-- URL �擾
			url = line2:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);
	
			-- ���M�Җ�
			sender = line:match(">([^<]+)(<.*)$");
--			sender = mz3.decode_html_entity(sender);
			data:set_text("name", sender);
			data:set_text("author", sender);
			
			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- ���s�擾
			i = i+1;
			line = html:get_at(i);
			
			-- data �ǉ�
			body:add(data.data);

			-- data �폜
			data:delete();
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("��</ul>�����������̂ŏI�����܂�");
			break;
		end

	end

	-- �O�A���փ����N�̒ǉ�
	if back_data~=nil then
		-- �擪�ɑ}��
		body:insert(0, back_data.data);
		back_data:delete();
	end
	if next_data~=nil then
		-- �����ɒǉ�
		body:add(next_data.data);
		next_data:delete();
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_new_official_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- ��M��, ���M��
mz3.set_parser("MESSAGE_IN", "mixi.mixi_list_message_parser");
mz3.set_parser("MESSAGE_OUT", "mixi.mixi_list_message_parser");
-- �������b�Z�[�W
mz3.set_parser("MIXI_LIST_MESSAGE_OFFICIAL", "mixi.mixi_new_official_message_parser");


----------------------------------------
-- ���j���[�ւ̓o�^
----------------------------------------

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group_for_mixi_list_message(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then
		-- ��M��
		local tab = mz3_group_data.get_group_item_by_name(group, '���b�Z�[�W');
		mz3_group_item.append_category(tab, "���b�Z�[�W(��M��)", "MESSAGE_IN", "http://mixi.jp/list_message.pl");

		-- ���M��
		local tab = mz3_group_data.get_group_item_by_name(group, '���b�Z�[�W');
		mz3_group_item.append_category(tab, "���b�Z�[�W(���M��)", "MESSAGE_OUT", "http://mixi.jp/list_message.pl?box=outbox");

		-- �������b�Z�[�W
		local tab = mz3_group_data.get_group_item_by_name(group, '���b�Z�[�W');
		mz3_group_item.append_category(tab, "�������b�Z�[�W", "MIXI_LIST_MESSAGE_OFFICIAL", "http://mixi.jp/list_message.pl?box=noticebox");
	end
end
-- �C�x���g�t�b�N�֐��̓o�^
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group_for_mixi_list_message");


----------------------------------------
-- estimate �Ώۂɒǉ�
----------------------------------------

--- estimate �Ώ۔��ʃC�x���g�n���h��
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        ��͑Ώ�URL
--
function on_estimate_access_type_by_url_for_mixi_list_message(event_name, url, data1, data2)

    -- ��M��
	if line_has_strings(url, 'list_message.pl', 'box=inbox') then
		return true, mz3.get_access_type_by_key('MESSAGE_IN');
	end
	
    -- ���M��
	if line_has_strings(url, 'list_message.pl', 'box=outbox') then
		return true, mz3.get_access_type_by_key('MESSAGE_OUT');
	end
	
    -- �������b�Z�[�W
	if line_has_strings(url, 'list_message.pl', 'box=noticebox') then
		return true, mz3.get_access_type_by_key('MIXI_LIST_MESSAGE_OFFICIAL');
	end

	return false;
end
-- �C�x���g�t�b�N�֐��̓o�^
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type_by_url_for_mixi_list_message");
