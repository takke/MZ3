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
-- 【コミュニティ最新書き込み一覧】
-- [list] new_bbs.pl 用パーサ
--
-- http://mixi.jp/new_bbs.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function new_bbs_parser(parent, body, html)
	mz3.logger_debug("new_bbs_parser start");
	
	-- 全消去
	mz3_data_list.clear(body);
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;
	
	-- 行数取得
	local line_count = mz3_htmlarray.get_count(html);
	for i=140, line_count-1 do
		line = mz3_htmlarray.get_at(html, i);
		
--		mz3.logger_debug(i .. " : " .. mz3_htmlarray.get_at(html, i));

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- 項目探索
		-- <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
		if line_has_strings(line, "<dt", "class", "iconTopic") or 
		   line_has_strings(line, "<dt", "class", "iconEvent") or 
		   line_has_strings(line, "<dt", "class", "iconEnquete") then

			in_data_region = true;

			-- data 生成
			data = mz3_data.create();

			-- 日付のパース
			mz3_data.parse_date_line(data, line);
			
			-- 次行取得
			i = i+1;
			line2 = mz3_htmlarray.get_at(html, i);
--			mz3.trace(i .. " : " .. line2);
			
			-- 見出し
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- 【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = line2:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
--			mz3.logger_debug(after);

			-- アンケート、イベントの場合はタイトルの前にマークを付ける
			if line_has_strings(line, "iconEvent") then
				title = "【☆】" .. title;
			elseif line_has_strings(line, "iconEnquete") then
				title = "【＠】" .. title;
			end
--			mz3.logger_debug(title);
			mz3_data.set_text(data, "title", title);
			
			-- URL 取得
			url = line2:match("href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			mz3_data.set_text(data, "url", url);
			
			-- コメント数
			mz3_data.set_integer(data, "comment_count", get_param_from_url(url, "comment_count"));
			
			-- id
			id = get_param_from_url(url, "id");
			mz3_data.set_integer(data, "id", id);

			-- コミュニティ名
			name = after:match("</a>[^(]*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
			mz3_data.set_text(data, "name", name);
			
			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(data, type);
			
			-- data 追加
			mz3_data_list.add(body, data);
			
			-- data 削除
			mz3_data.delete(data);
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("★</ul>が見つかったので終了します");
			break;
		end

	end
	
	-- 前、次へリンクの追加
	if back_data~=nil then
		-- 先頭に挿入
		mz3_data_list.insert(body, 0, back_data);
		mz3_data.delete(back_data);
	end
	if next_data~=nil then
		-- 末尾に追加
		mz3_data_list.add(body, next_data);
		mz3_data.delete(next_data);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("new_bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
