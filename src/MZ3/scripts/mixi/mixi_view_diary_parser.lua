--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- TODO �z�X�g���Őݒ肵�Ă��邪�A�{���͂�����Őݒ肷�ׂ��B


--------------------------------------------------
-- �ymixi ���L�ڍׁz
-- [content] view_diary.pl �p�p�[�T
--
-- http://mixi.jp/view_diary.pl
--
-- ����:
--   data:  ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy: NULL
--   html:  HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_view_diary_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_diary_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapper�N���X��
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);
	
	data:clear();

	-- �Ƃ肠�������s�o��
	data:add_body_with_extract("<br>");

	-- �s���擾
	local line_count = html:get_count();
	for i=10, line_count-1 do
		line = html:get_at(i);

		-- <title> �^�O����^�C�g�����擾����
		if line_has_strings(line, "<title>", "</title>") then
			-- �����̓��L�Ȃ�@�@�u<title>[mixi] �^�C�g��</title>�v
			-- �����ȊO�̓��L�Ȃ�u<title>[mixi] ���O | �^�C�g��</title>�v
			-- �Ƃ����`���Ȃ̂ŁA�^�C�g���������𒊏o
			local title = line:match("<title>%[mixi%] (.-)</title>");
			title = mz3.decode_html_entity(title);
			after = title:match(" | (.-)$");
			if after~=nil then
				title = after;
			end

			data:set_text("title", title);
		end
		
		-- �u�ŋ߂̓��L�v�̎擾
		if line_has_strings(line, '<ul class="contentsListDiary">') then
			for i=i+1, line_count-1 do
				line = html:get_at(i);
				if line_has_strings(line, '</ul') then
					break;
				end
				local url, title = line:match('<li><a href="(.-)">(.-)</a>');
				if url~=nil then
--					mz3.logger_debug(url .. title);
					data:add_link_list(url, mz3.decode_html_entity(title), 'page');
				end
			end
		end

		-- �����̎擾
		-- ���J�͈͂̎擾
		if line_has_strings(line, '<div', 'listDiaryTitle') then
			for i=i+1, line_count-1 do
				line = html:get_at(i);
				if line_has_strings(line, '</dl') then
					break;
				end

				local img_alt = line:match('<img.-alt="(.-)"');
				if img_alt~=nil then
					data:set_text('opening_range', img_alt);
				end

				local date = line:match('<dd>(.-)</dd>');
				if date~=nil then
					data:set_date(date);
				end
			end
		end

		-- ���L�̒���
		if line_has_strings(line, '<div', 'class', 'diaryTitle') then
			-- �����̓��L�Ȃ�uXXX�̓��L�v�A�����ȊO�Ȃ�uXXX����̓��L�v�̂͂��B
			-- ���̋K���Œ��҂���́B

			-- ���̍s�� <h2> ����擾
			line = html:get_at(i+1);
			if line_has_strings(line, 'diaryTitleFriend') then
				author = line:match('<h2>(.-)����̓��L</h2>');
			else
				author = line:match('<h2>(.-)�̓��L</h2>');
			end
			if author~=nil then
				author = mz3.decode_html_entity(author);
				data:set_text('author', author);
				data:set_text('name', author);
			end
		end

		-- ���L�̓Y�t�ʐ^
		if line_has_strings(line, '<div', 'class', 'diaryPhoto') then
			for i=i+1, line_count-1 do
				line = html:get_at(i);
				if line_has_strings(line, '</div') then
					break;
				end

				data:add_body_with_extract(line);
			end
--			data:add_body_with_extract("<br>");
		end

		-- �O�̓��L�ւ̃����N
		if line_has_strings(line, '<div', 'class', 'diaryPagingLeft') then
			local link = line:match('<a.*</a>');
			if link~=nil then
				data:set_text('prev_diary', link);
			end
		end
		-- ���̓��L�ւ̃����N
		if line_has_strings(line, '<div', 'class', 'diaryPagingRight') then
			local link = line:match('<a.*</a>');
			if link~=nil then
				data:set_text('next_diary', link);
			end
		end

		-- TODO ����炵���T���v��HTML����������Ȃ��̂łƂ肠�������u
--[[
		// �S�Ă�\���ւ̃����N
		try {
			const xml2stl::Node& li = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
												   .getNode(L"div", L"id=diaryComment")
												   .getNode(L"div", L"class=diaryMainArea02 deleteButton")
												   .getNode(L"form")
												   .getNode(L"div", L"class=pageNavigation01 preComment01")
												   .getNode(L"div", L"class=pageList03")
												   .getNode(L"ul")
												   .getNode(L"li");

			CString FullLink = li.getTextAll().c_str();
			data_.SetFullDiary( FullLink );

		} catch (xml2stl::NodeNotFoundException& e) {
			// �l�̓��L�̏ꍇ
			try {
				const xml2stl::Node& li = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"id=diaryComment")
													   .getNode(L"div", L"class=pageNavigation01 preComment01")
													   .getNode(L"div", L"class=pageList03")
													   .getNode(L"ul")
													   .getNode(L"li");

				CString FullLink = li.getTextAll().c_str();
				data_.SetFullDiary( FullLink );

			} catch (xml2stl::NodeNotFoundException& e) {
				// �����N���Ȃ������Ɣ��f����
				MZ3LOGGER_INFO( util::FormatString( L"�u�S�Ă�\���v�����N�擾�G���[ : %s", e.getMessage().c_str()) );
			}
			MZ3LOGGER_INFO( util::FormatString( L"�����̓��L�́u�S�Ă�\���v�����N�擾�G���[ : %s", e.getMessage().c_str()) );
		}
]]

		-- �{���擾
		if line_has_strings(line, '<div', 'id', 'diary_body') then
			local sub_html = line;
			for i=i+1, line_count-1 do
				line = html:get_at(i);
				if line_has_strings(line, '<!--/viewDiaryBox--></div>') then
					break;
				end

				sub_html = sub_html .. line;
			end
			-- script �^�O�̏���
			sub_html = sub_html:gsub('<script.-</script>', '');
			data:add_body_with_extract(sub_html);
		end

		-- �R�����g�擾
		i = parseDiaryComment(data, line, i, line_count, html);
		
		-- POST URL ���
		if line_has_strings(line, '<form action', 'add_comment.pl', 'comment_form') then
			-- <form action="add_comment.pl?diary_id=xxx" method="post" name="comment_form">
			local url = line:match('action="(.-)"');
			if url~=nil then
				data:set_text('post_address', url);
				data:set_text('content_type', "application/x-www-form-urlencoded");
			end

			-- hidden
			for i=i+1, line_count-1 do
				line = html:get_at(i);
				if line_has_strings(line, '</form>') then
					break;
				end

				-- <input type="hidden" name="owner_id" value="85892" />
				if line_has_strings(line, '<input', 'hidden') then
					local v = line:match('value="(.-)"');
					data:set_integer('owner_id', v);
					break;
				end

			end
		end

	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_diary_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


-- �R�����g�̎擾
function parseDiaryComment(data, line, i, line_count, html)
	if line_has_strings(line, 'diaryMainArea02', 'commentList') or
	   line_has_strings(line, 'diaryMainArea02', 'deleteButton') then
		
		-- 2���ڈȍ~�͎q�v�f�Ƃ��ē�������
		comment_index = 1;
		in_dd = false;
		child = MZ3Data:create();
		for i=i+1, line_count-1 do
			line = html:get_at(i);
			if line_has_strings(line, '<!--/diaryMainArea02--></div>') then
				break;
			end

			if line_has_strings(line, 'diaryCommentbox') then
				child:clear();
				in_dd = false;
				child:add_body_with_extract('<br>');
			end
			
			if line_has_strings(line, '</div') then
				if child:get_text('author')~=nil then
					data:add_child(child);
				end
			end
			
			if line_has_strings(line, 'commentTitleName') then
				local v = line:match('<a.->(.-)</a>');
				-- ���O
				child:set_text('author', v);
				-- �R�����g�ԍ�
				child:set_integer('comment_index', comment_index);
				comment_index = comment_index+1;
			end
			
			if line_has_strings(line, 'commentTitleDate') then
				local v = line:match('<span.->(.-)<');
				v = mz3.decode_html_entity(v);
				child:set_date(v);
			end
			
			if in_dd then
				if line_has_strings(line, '</dd>') then
					in_dd = false;
				else
					child:add_body_with_extract(line);
				end
			else
				if line_has_strings(line, '<dd>') then
					in_dd = true;
				end
			end
			
		end
		child:delete();

	end
	
	return i;
end

----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
mz3.set_parser("MIXI_DIARY",         "mixi.mixi_view_diary_parser");
mz3.set_parser("MIXI_NEIGHBORDIARY", "mixi.mixi_view_diary_parser");
mz3.set_parser("MIXI_MYDIARY",       "mixi.mixi_view_diary_parser");
