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

--------------------------------------------------
-- �y�R�~���j�e�B�ŐV�������݈ꗗ�z
-- [list] new_bbs.pl �p�p�[�T
--
-- http://mixi.jp/new_bbs.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function new_bbs_parser(parent, body, html)
	mz3.logger_debug("new_bbs_parser start");
	
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
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- ���ڒT��
		-- <dt class="iconTopic">2007�N10��01��&nbsp;22:14</dt>
		if line_has_strings(line, "<dt", "class", "iconTopic") or 
		   line_has_strings(line, "<dt", "class", "iconEvent") or 
		   line_has_strings(line, "<dt", "class", "iconEnquete") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			-- ���t�̃p�[�X
			data:parse_date_line(line);
			
			-- ���s�擾
			i = i+1;
			line2 = html:get_at(i);
--			mz3.trace(i .. " : " .. line2);
			
			-- ���o��
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- �y�`���b�g�z�W���Ixxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = line2:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
--			mz3.logger_debug(after);

			-- �A���P�[�g�A�C�x���g�̏ꍇ�̓^�C�g���̑O�Ƀ}�[�N��t����
			if line_has_strings(line, "iconEvent") then
				title = "�y���z" .. title;
			elseif line_has_strings(line, "iconEnquete") then
				title = "�y���z" .. title;
			end
--			mz3.logger_debug(title);
			data:set_text("title", title);
			
			-- URL �擾
			url = line2:match("href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			data:set_text("url", url);
			
			-- �R�����g��
			data:set_integer("comment_count", get_param_from_url(url, "comment_count"));
			
			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- �R�~���j�e�B��
			name = after:match("</a>[^(]*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);
			
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
	mz3.logger_debug("new_bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end