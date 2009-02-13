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

--		mz3.logger_info( line );
		
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- ���ցA�O�ւ̒��o����
		-- ���ڔ����O�ɂ̂ݑ��݂���
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- ���ڒT��
		-- <dt class="iconTopic">2007�N10��01��&nbsp;22:14</dt>
		if line_has_strings(line, "<span", "class", "data") or
		   line_has_strings(line, "<span", "class", "name") then

			in_data_region = true;

			-- data ����
			data = MZ3Data:create();

			-- ���o��
			-- <span class="date">02��12�� 22:02</span><span class="name"><a href="show_friend.pl?id=xxxxx">user_nickname</a>
			-- �y�`���b�g�z�W���Ixxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			date, after = line:match(">([^<]+)(<.*)$");
			-- ���t�̃Z�b�g�c
			data:parse_date_line( date );
			date = mz3.decode_html_entity(title);

--			mz3.logger_debug(title);
--			data:set_text("title", title);

			-- URL �擾
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- ���[�U��
--			nickname = after:match("</a>[^(]*[(](.*)[)]</dd>");
--			nickname = after:match(">[^(]*[(](.*)[)]</a>$");
--			nickname = after:match(">[^(]*[(](.*)[)]</a>$");
--			nickname = after:match("^>.*</a>$");

			int_start, int_end = after:find( id, 1, true )
			int_nickname_start = int_end +3;
			int_nickname_end, dummy = after:find( "</a>", 1, true )
			nickname = after:sub( int_nickname_start, int_nickname_end -1 );

			-- �}�C�~�N�Ȃ疼�O�̌�� "(�}�C�~�N)" �ƕt�^
			if after:find( "alt=" ) and nickname ~= nil then
				nickname = nickname .. " (�}�C�~�N)";
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
