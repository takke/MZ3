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
-- アクセス種別の登録
----------------------------------------
type = MZ3AccessTypeInfo:create();
type:set_info_type('body');									-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('MIXI_NEWS_QUOTE_DIARY');				-- シリアライズキー
type:set_short_title('ニュース関連日記');							-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_cache_file_pattern('news\\list_quote_diary_{urlparam:id}.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
-- url(1) : http://news.mixi.jp/list_quote_diary.pl?id=839733


--------------------------------------------------
-- 【mixi ニュース関連日記】
-- [content] http://news.mixi.jp/list_quote_diary.pl 用パーサ
--
-- http://mixi.jp/view_message.pl
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_news_quote_diary_parser(data, dummy, html)
	mz3.logger_debug("mixi_news_quote_diary_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	data:clear();

	-- タイトル取得
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

	-- 概要取得
	in_description = false;
	desc_end_line = 200;
	for i=200, line_count-1 do
		line = html:get_at(i);
		
		--<div class="relationNewsDescription">
		--<div class="heading02">
		--<h2>ニュース概要</h2>
		
		if line_has_strings(line, '<div', 'class="relationNewsDescription"') then
			in_description = true;
		end
		if in_description then
			--<p class="newsTitle"><a href="view_news.pl?id=840519&media_id=42">misonoがセミヌードに初挑戦</a></p>
			if line_has_strings(line, '<p class="newsTitle">') then
				data:add_text_array("body", "\r\n");
				
				-- URLをフルパスに変換
				line = line:gsub('view_news\.pl', 'http://news.mixi.jp/%1', 1);
				data:add_body_with_extract(line:match('>(.*)<'));
				
				data:add_text_array("body", "\r\n");
				data:add_text_array("body", "\r\n");
			end
			--<p class="media">（サンケイスポーツ - 05月19日 08:03）</p>
			if line_has_strings(line, '<p class="media">') then
				author, date = line:match("（([^ ]*) \- (.*)）");
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
		--<a href="http://mixi.jp/show_friend.pl?id=xxx">なまえ</a>
		--
		--</div>
		--<div class="diaryList01">
		--<div class="heading clearfix">
		--<span class="title"><a href="http://mixi.jp/view_diary.pl?owner_id=xxx&id=xxx">たいとる</a></span>
		--<span class="date">05月19日 15:26</span>
		--</div>
		--<p>ばっすいしたにっきのないよう</p>
		--<p class="readMore"><a href="http://mixi.jp/view_diary.pl?owner_id=xxx&id=xxx">続きを読む</a></p>
		--</div>
		--</div>
		
		if line_has_strings(line, '<div', 'diaryContents') then
			-- data 生成
			diary = MZ3Data:create();
			
			-- ニックネームとURL
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

			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			diary:set_access_type(type);

			-- data 追加
			data:add_child(diary);

			-- data 削除
			diary:delete();
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_news_quote_diary_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- パーサの登録
----------------------------------------
-- 受信箱
mz3.set_parser("MIXI_NEWS_QUOTE_DIARY", "mixi.mixi_news_quote_diary_parser");
