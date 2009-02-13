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
-- 【逆あしあと一覧】
-- [list] show_self_log.pl 用パーサ
--
-- http://mixi.jp/show_self_log.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_show_self_log_parser(parent, body, html)
	mz3.logger_debug("mixi_show_self_log_parser start");

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

--		mz3.logger_info( line );
		
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- 項目探索
		-- <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
		if line_has_strings(line, "<span", "class", "data") or
		   line_has_strings(line, "<span", "class", "name") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 見出し
			-- <span class="date">02月12日 22:02</span><span class="name"><a href="show_friend.pl?id=xxxxx">user_nickname</a>
			-- 【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			date, after = line:match(">([^<]+)(<.*)$");
			-- 日付のセット…
			data:parse_date_line( date );
			date = mz3.decode_html_entity(title);

--			mz3.logger_debug(title);
--			data:set_text("title", title);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- ユーザ名
--			nickname = after:match("</a>[^(]*[(](.*)[)]</dd>");
--			nickname = after:match(">[^(]*[(](.*)[)]</a>$");
--			nickname = after:match(">[^(]*[(](.*)[)]</a>$");
--			nickname = after:match("^>.*</a>$");

			int_start, int_end = after:find( id, 1, true )
			int_nickname_start = int_end +3;
			int_nickname_end, dummy = after:find( "</a>", 1, true )
			nickname = after:sub( int_nickname_start, int_nickname_end -1 );

			-- マイミクなら名前の後に "(マイミク)" と付与
			if after:find( "alt=" ) and nickname ~= nil then
				nickname = nickname .. " (マイミク)";
			end

			nickname = mz3.decode_html_entity(nickname);
			data:set_text("title", nickname);
			
			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);
			
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
	mz3.logger_debug("mixi_show_self_log_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
