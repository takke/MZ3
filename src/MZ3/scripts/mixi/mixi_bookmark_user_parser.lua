--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
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
type:set_info_type('category');								-- �J�e�S��
type:set_service_type('mixi');								-- �T�[�r�X���
type:set_serialize_key('FAVORITE');							-- �V���A���C�Y�L�[
type:set_short_title('���C�ɓ��胆�[�U�[');					-- �ȈՃ^�C�g��
type:set_request_method('GET');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\bookmark_user.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');						-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/list_bookmark.pl');
type:set_body_header(1, 'name', '���[�U�[��');
type:set_body_header(2, 'title', '���ȏЉ�');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]

--------------------------------------------------
-- �y���C�ɓ��胆�[�U�[�ꗗ�z
-- [list] bookmark.pl �p�p�[�T
--
-- http://mixi.jp/list_bookmark.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_bookmark_user_parser(parent, body, html)
	mz3.logger_debug("mixi_bookmark_user_parser start");

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
	for i=230, line_count-1 do
		line = html:get_at(i);

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "list_bookmark.pl");
		end

		-- ���ڒT�� �ȉ���s
		-- <div class="listIcon">
		--   <a href="show_friend.pl?id=xxx">
		--     <img src="http://community.img.mixi.jp/photo/member/xxx.jpg" alt="���[�U��" />
		--   </a>
		-- </div>
		if line_has_strings(line, "<div", "class", "listIcon") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			-- �摜�擾
			image_url, after = line:match("src=\"([^\"]+)\"");
			-- image_md5 = mz3.make_image_logfile_path_from_url_md5( image_url );
			-- hoge = mz3_image_cache.get_image_index_by_url( image_url );
			data:add_text_array("image", image_url);

			i = i+2;
			line = html:get_at(i);

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- ���[�U��
			name, after = line:match(">([^<]+)(<.*)$");
			data:set_text("name", name);

			if name == nil then
				-- �މ�[�U�͕\�����Ȃ�
				-- data �폜
				data:delete();
			else
				-- �ŏI���O�C��
				login = line:match("span>(.-)</div");
				if login~=nil then
					login = login:match('%((.-)%)');
					data:set_date(login);
				end

				i = i+1;
				line = html:get_at(i);

				-- ����
				description, after = line:match(">([^<]+)(<.*)$");
				data:add_body_with_extract(description);

				-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
				type = mz3.estimate_access_type_by_url(url);
				data:set_access_type(type);

				-- data �ǉ�
				body:add(data.data);

				-- data �폜
				data:delete();
			end

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
	mz3.logger_debug("mixi_bookmark_user_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- ���C�ɓ��胆�[�U�[
mz3.set_parser("FAVORITE", "mixi.mixi_bookmark_user_parser");
