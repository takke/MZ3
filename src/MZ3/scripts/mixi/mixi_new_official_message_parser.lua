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
-- 【公式からのメッセージ一覧】
-- [list] list_message.pl?box=noticebox 用パーサ
--
-- http://mixi.jp/list_message.pl?box=noticebox
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_new_official_message_parser(parent, body, html)
	mz3.logger_debug("mixi_new_official_message_parser start");
	
	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;

	-- 行数取得
	local line_count = html:get_count();
	for i=140, line_count-1 do
		line = html:get_at(i);
		
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
--			back_data, next_data = parse_next_back_link(line, "list_message.pl?box=noticebox");
			back_data, next_data = parse_next_back_link(line, "view_message.pl");
		end

		-- 項目探索
		-- <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
		if line_has_strings(line, "<td", "class", "sender")  then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 日付のパース
			-- sender, subject, date と回り，date に日付があるので +2 …
			line3 = html:get_at(i +2);
			date = line3:match(">([^<]+)(<.*)$");
			date = mz3.decode_html_entity(date);
			data:parse_date_line(date);
			
			-- 次行取得
			i = i+1;
			line2 = html:get_at(i);
--			mz3.trace(i .. " : " .. line2);
			
			-- 見出し
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- 【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = line2:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);

			data:set_text("title", title);
			
			-- URL 取得
			url = line2:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);
	
			-- 送信者名
			sender = line:match(">([^<]+)(<.*)$");
--			sender = mz3.decode_html_entity(sender);
			data:set_text("name", sender);
			
			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- 次行取得
			i = i+1;
			line = html:get_at(i);
			
			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("★</ul>が見つかったので終了します");
			break;
		end

	end

	-- 前、次へリンクの追加
	if back_data~=nil then
		-- 先頭に挿入
		body:insert(0, back_data.data);
		back_data:delete();
	end
	if next_data~=nil then
		-- 末尾に追加
		body:add(next_data.data);
		next_data:delete();
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_new_official_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
