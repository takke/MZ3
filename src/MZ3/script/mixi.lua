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

--------------------------------------------------
-- ���ցA�O�ւ̒��o����
--------------------------------------------------
function parse_next_back_link(line, base_url)

	local back_data = nil;
	local next_data = nil;
	
	-- <ul><li><a href="new_bbs.pl?page=1">�O��\��</a></li>
	-- <li>51���`100����\��</li>
	-- <li><a href="new_bbs.pl?page=3">����\��</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- �O
		local url, t = line:match([[href="([^"]+)">(�O[^<]+)<]]);
		if url~=nil then
			back_data = mz3_data.create();
			mz3_data.set_text(back_data, "title", "<< " .. t .. " >>");
			mz3_data.set_text(back_data, "url", url);
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(back_data, type);
		end
		
		-- ��
		local url, t = line:match([[href="([^"]+)">(��[^<]+)<]]);
		if url~=nil then
			next_data = mz3_data.create();
			mz3_data.set_text(next_data, "title", "<< " .. t .. " >>");
			mz3_data.set_text(next_data, "url", url);
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(next_data, type);
		end
	end
	
	return back_data, next_data;
end

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
function bbs_parser(parent, body, html)
	mz3.logger_debug("bbs_parser start");
	
	-- �S����
	mz3_data_list.clear(body);
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;
	
	-- �s���擾
	local line_count = mz3_htmlarray.get_count(html);
	for i=140, line_count-1 do
		line = mz3_htmlarray.get_at(html, i);
		
--		mz3.logger_debug(i .. " : " .. mz3_htmlarray.get_at(html, i));

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
			data = mz3_data.create();

			-- ���t�̃p�[�X
			mz3_data.parse_date_line(data, line);
			
			-- ���s�擾
			i = i+1;
			line2 = mz3_htmlarray.get_at(html, i);
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
			mz3_data.set_text(data, "title", title);
			
			-- URL �擾
			url = line2:match("href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			mz3_data.set_text(data, "url", url);
			
			-- �R�����g��
			mz3_data.set_integer(data, "comment_count", get_param_from_url(url, "comment_count"));
			
			-- id
			id = get_param_from_url(url, "id");
			mz3_data.set_integer(data, "id", id);

			-- �R�~���j�e�B��
			name = after:match("</a>.*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
			mz3_data.set_text(data, "name", name);
			
			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(data, type);
			
			-- data �ǉ�
			mz3_data_list.add(body, data);
			
			-- data �폜
			mz3_data.delete(data);
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("��</ul>�����������̂ŏI�����܂�");
			break;
		end

	end
	
	-- �O�A���փ����N�̒ǉ�
	if back_data~=nil then
		-- �擪�ɑ}��
		mz3_data_list.insert(body, 0, back_data);
		mz3_data.delete(back_data);
	end
	if next_data~=nil then
		-- �����ɒǉ�
		mz3_data_list.add(body, next_data);
		mz3_data.delete(next_data);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �t�b�N�֐��̓o�^
----------------------------------------

-- �R�~�������ꗗ
mz3.set_parser("BBS", "mixi.bbs_parser");
mz3.set_parser("NEW_BBS_COMMENT", "mixi.bbs_parser");

--mz3.set_hook("mixi", "after_parse", after_parse);

mz3.logger_debug('mixi.lua end');
