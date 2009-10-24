--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id: mixi_bookmark_community_parser.lua
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- TODO �{���� Lua ���ł��ׂ���
--[[
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('mixi');									-- �T�[�r�X���
type:set_serialize_key('FAVORITE');				-- �V���A���C�Y�L�[
type:set_short_title('���C�ɓ���R�~��');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\bookmark_community.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');							-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/list_bookmark.pl?kind=community');
type:set_body_header(1, 'name', '�R�~���j�e�B');
type:set_body_header(2, 'title', '����');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]

--------------------------------------------------
-- �y���C�ɓ���R�~���ꗗ�z
-- [list] bookmark.pl �p�p�[�T
--
-- http://mixi.jp/list_bookmark.pl?kind=community
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_bookmark_community_parser(parent, body, html)
	mz3.logger_debug("mixi_bookmark_community_parser start");

	-- wrapper�N���X��
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();

	local t1 = mz3.get_tick_count();
	local in_data_region = false;

	local back_data = nil;
	local next_data = nil;

	local deleted_community = "�����̃R�~���j�e�B�͂��łɕ����Ă��܂��B�s�v�ȏꍇ�͍폜���Ă��������B";

	-- �s���擾
	local line_count = html:get_count();
	for i=180, line_count-1 do
		line = html:get_at(i);

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
			if line_has_strings( line, "list_bookmark.pl" ) then
				back_data, next_data = parse_next_back_link(line, "list_bookmark.pl", "name");
			end
		end

		-- ���ڒT�� �ȉ���s
		-- <div class="listIcon">
		if line_has_strings(line, "<div", "class", "listIcon") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			i = i+1;
			line = html:get_at(i);

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			if url ~= nil then
				url = complement_mixi_url(url);
				data:set_text("url", url);
			end

			i = i+1;
			line = html:get_at(i);

			-- �摜�擾
			image_url = line:match("src=\"([^\"]+)\"");
			data:add_text_array("image", image_url);

			i = i+5;
			line = html:get_at(i);

			-- �R�~���j�e�B��
			name = line:match(">([^<]+)(<.*)$");

			if name ~= "" and name ~= nil then
				add_name = line:match(">([^<]+)(&nbsp;.*)$");
				data:set_text("name", add_name);

				-- �Q���Ґ�
				users = name:match("([0-9]+.�l)");
				data:set_date(users);

				i = i+8;
				line = html:get_at(i);

				-- ����
				-- description, after = line:match(">([^<]+)(<.*)$");
				description = line:gsub( "\t", "" );
				data:add_body_with_extract(description);

				if description == '' or description == deleted_community then
					-- �폜�ς݃R�~���͈ꗗ�ɏo���Ȃ�
				else

					-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
					type = mz3.estimate_access_type_by_url(url);
					data:set_access_type(type);

					-- data �ǉ�
					body:add(data.data);
				end
			end
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
	mz3.logger_debug("mixi_bookmark_community_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- ���C�ɓ���R�~��
mz3.set_parser("FAVORITE_COMMUNITY", "mixi.mixi_bookmark_community_parser");
