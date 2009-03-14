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

type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- �J�e�S��
type:set_service_type('mixi');								-- �T�[�r�X���
type:set_serialize_key('MIXI_SHOW_SELF_LOG');				-- �V���A���C�Y�L�[
type:set_short_title('�t��������');							-- �ȈՃ^�C�g��
type:set_request_method('GET');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\show_self_log.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');						-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/show_self_log.pl');
type:set_body_header(1, 'title', '���O');
type:set_body_header(2, 'date', '���t');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
--mz3.logger_debug(type);

--------------------------------------------------
-- �y�t�������ƈꗗ�z
-- [list] show_self_log.pl �p�p�[�T
--
-- http://mixi.jp/show_self_log.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_show_self_log_parser(parent, body, html)
	mz3.logger_debug("mixi_show_self_log_parser start");

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

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- ���ڒT��
		-- <span class="date">02��16�� 09:39</span>
		if line_has_strings(line, "<span", "class", "data") or
		   line_has_strings(line, "<span", "class", "name") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			-- ���o��
			-- <span class="date">02��16�� 09:39</span><span class="name"><a href="show_friend.pl?id=xxxxx">user_nickname</a>
			date, after = line:match(">([^<]+)(<.*)$");
			-- ���t�̃Z�b�g�c
			data:parse_date_line( date );

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- ���[�U��
			int_start, int_end = after:find( id, 1, true )
			int_nickname_start = int_end +3;
			int_nickname_end, dummy = after:find( "</a>", 1, true )
			nickname = after:sub( int_nickname_start, int_nickname_end -1 );

			-- �}�C�~�N�Ȃ疼�O�̌�� "(�}�C�~�N)" �ƕt�^
			mymixi = "\"�}�C�~�N�V�B\"";
			mymixi_mymixi = "\"�}�C�~�N�V�B�̃}�C�~�N�V�B\"";
			if after:find( "alt=" .. mymixi ) and nickname ~= nil then
				nickname = nickname .. " (�}�C�~�N)";
			elseif after:find( "alt=" .. mymixi_mymixi ) and nickname ~= nil then
				nickname = nickname .. " (�}�C�~�N�̃}�C�~�N)";
			end

			nickname = mz3.decode_html_entity(nickname);
			data:set_text("title", nickname);

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
	mz3.logger_debug("mixi_show_self_log_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- �t��������
mz3.set_parser("MIXI_SHOW_SELF_LOG", "mixi.mixi_show_self_log_parser");


----------------------------------------
-- ���j���[�ւ̓o�^
----------------------------------------

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group_for_mixi_show_self_log(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then

		-- ���̑�/�t�������� �ǉ�
		local tab = mz3_group_data.get_group_item_by_name(group, '���̑�');
		mz3_group_item.append_category(tab, "�t��������", "MIXI_SHOW_SELF_LOG", "http://mixi.jp/show_self_log.pl");

	end
end


----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------
-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group_for_mixi_show_self_log");
