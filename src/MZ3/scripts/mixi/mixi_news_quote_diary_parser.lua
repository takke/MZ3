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
type = MZ3AccessTypeInfo:create();
type:set_info_type('body');									-- �J�e�S��
type:set_service_type('mixi');								-- �T�[�r�X���
type:set_serialize_key('MIXI_NEWS_QUOTE_DIARY');				-- �V���A���C�Y�L�[
type:set_short_title('�j���[�X�֘A���L');							-- �ȈՃ^�C�g��
type:set_request_method('GET');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('news\\list_quote_diary_{urlparam:id}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');						-- �G���R�[�f�B���O
-- url(1) : http://news.mixi.jp/list_quote_diary.pl?id=839733


--------------------------------------------------
-- �ymixi �j���[�X�֘A���L�z
-- [content] http://news.mixi.jp/list_quote_diary.pl �p�p�[�T
--
-- http://mixi.jp/view_message.pl
--
-- ����:
--   data:  ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy: NULL
--   html:  HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_news_quote_diary_parser(data, dummy, html)
	mz3.logger_debug("mixi_news_quote_diary_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapper�N���X��
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	data:clear();

	-- �^�C�g���擾
	local line_count = html:get_count();
	for i=5, line_count-1 do
		line = html:get_at(i);
		if i>20 then
			break;
		end
		
		if line_has_strings(line, "<title>") then
			title = line:match("<title>(.*)</title>");
			
			data:set_text('title', title);
			break;
		end
	end

	-- �T�v�擾
	in_description = false;
	desc_end_line = 200;
	for i=200, line_count-1 do
		line = html:get_at(i);
		
		--<div class="relationNewsDescription">
		--<div class="heading02">
		--<h2>�j���[�X�T�v</h2>
		
		if line_has_strings(line, '<div', 'class="relationNewsDescription"') then
			in_description = true;
		end
		if in_description then
			--<p class="newsTitle"><a href="view_news.pl?id=840519&media_id=42">misono���Z�~�k�[�h�ɏ�����</a></p>
			if line_has_strings(line, '<p class="newsTitle">') then
				data:add_text_array("body", "\r\n");
				
				-- URL���t���p�X�ɕϊ�
				line = line:gsub('view_news\.pl', 'http://news.mixi.jp/%1', 1);
				data:add_body_with_extract(line:match('>(.*)<'));
				
				data:add_text_array("body", "\r\n");
				data:add_text_array("body", "\r\n");
			end
			--<p class="media">�i�T���P�C�X�|�[�c - 05��19�� 08:03�j</p>
			if line_has_strings(line, '<p class="media">') then
				author, date = line:match("�i([^ ]*) \- (.*)�j");
				data:set_text('author', author);
				data:parse_date_line(date);
			end
			if line_has_strings(line, '<p class="outline">') then
				data:add_body_with_extract(line:match('>(.*)<'));
				
				desc_end_line = i;
				break;
			end
		end
	end
	
	for i=desc_end_line, line_count-1 do
		line = html:get_at(i);
		
		--<div class="diaryContents clearfix">
		--<div class="thumb">
		--<a href="http://mixi.jp/show_friend.pl?id=xxx"><img src="http://profile.img.mixi.jp/photo/member/xx/xx/xx.jpg" alt="" /></a>
		--<a href="http://mixi.jp/show_friend.pl?id=xxx">�Ȃ܂�</a>
		--
		--</div>
		--<div class="diaryList01">
		--<div class="heading clearfix">
		--<span class="title"><a href="http://mixi.jp/view_diary.pl?owner_id=xxx&id=xxx">�����Ƃ�</a></span>
		--<span class="date">05��19�� 15:26</span>
		--</div>
		--<p>�΂����������ɂ����̂Ȃ��悤</p>
		--<p class="readMore"><a href="http://mixi.jp/view_diary.pl?owner_id=xxx&id=xxx">������ǂ�</a></p>
		--</div>
		--</div>
		
		if line_has_strings(line, '<div', 'diaryContents') then
			-- data ����
			diary = MZ3Data:create();
			
			-- �j�b�N�l�[����URL
			i = i+3;
			line = html:get_at(i);
			profile_url, name = line:match('href="([^\"]*)">(.*)<');

			diary:set_text("title", 'title');
			diary:set_text("name", name);
			diary:set_text("author", name);
			diary:set_text("profile_url", profile_url);
			
			while i<line_count do
				i = i+1;
				line = html:get_at(i);
			
				if line_has_strings(line, 'class', 'readMore') then
					break;
				end
				
				if line_has_strings(line, '<span', 'class="title"') then
					--diary:add_text_array("body", "\r\n");
					diary:add_body_with_extract(line);
					diary:add_text_array("body", "\r\n");
				end
				if line_has_strings(line, '<span', 'class="date"') then
					diary:parse_date_line(line);
				end
				if line_has_strings(line, '<p>') then
					diary:add_text_array("body", "\r\n");
					diary:add_body_with_extract(line);
					break;
				end
			end

			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.estimate_access_type_by_url(url);
			diary:set_access_type(type);

			-- data �ǉ�
			data:add_child(diary);

			-- data �폜
			diary:delete();
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_news_quote_diary_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- ��M��
mz3.set_parser("MIXI_NEWS_QUOTE_DIARY", "mixi.mixi_news_quote_diary_parser");
