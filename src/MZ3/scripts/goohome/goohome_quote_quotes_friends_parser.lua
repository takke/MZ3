--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : goohome parsers
--
-- $Id$
--------------------------------------------------
module("goohome", package.seeall)

function get_json_value(line, key)
	local json_key = "\"" .. key .. "\":";
	if line_has_strings(line, json_key) then
		return line:match(json_key .. "[^\"]*\"([^\"]*)\"");
	else
		return nil;
	end
end

--------------------------------------------------
-- [list] gooホーム 友達・注目の人のひとこと一覧パーサ
--
-- http://home.goo.ne.jp/api/quote/quotes/friends/json
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function quote_quotes_friends_parser(parent, body, html)
	mz3.logger_debug("quote_quotes_friends_parser start");
	local t1 = mz3.get_tick_count();
	
	-- wrapperクラス化
	html = MZ3HTMLArray:create(html);
	parent = MZ3Data:create(parent);

	-- 全消去
	mz3_data_list.clear(body);

	-- TODO 新着マージ＋重複防止処理
--[[
	-- 新規に追加するデータ群
	CMixiDataList new_list;

	-- 重複防止用の id 一覧を生成。
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}
]]

	-- オブジェクト生成
	local data = MZ3Data:create();
	type_user = mz3.get_access_type_by_key("GOOHOME_USER");
	data:set_access_type(type_user);

	-- ひとことAPIのjsonは行単位に要素があるので、行単位にパースする
	local line_count = html:get_count();
	local in_quote = false;
	
	for i=0, line_count-1 do
		line = html:get_at(i);
		if line_has_strings(line, "\"user\": {") then
--			mz3.logger_debug(i .. " : " .. line);
			if in_quote then
				-- 一つ前の解析結果を登録
				mz3_data_list.add(body, data.data);
				data:delete();
				
				-- 次の要素用データを生成
				data = MZ3Data:create();
				data:set_access_type(type_user);
			end
			in_quote = true;
		else
			if in_quote then
				
				local v;
				
				-- nickname
				v = get_json_value(line, "nickname");
				if v ~= nil then
					data:set_text("name", v);
				end
				
				-- goo_id
				v = get_json_value(line, "goo_id");
				if v ~= nil then
					data:set_text("goo_id", v);
				end
				
				-- profile_image_url
				v = get_json_value(line, "profile_image_url");
				if v ~= nil then
					data:add_text_array("image", v);
				end
				
				-- profile_url
				v = get_json_value(line, "profile_url");
				if v ~= nil then
					data:set_text("browse_uri", v);
					data:set_text("url", v);
				end
				-- friends
				v = get_json_value(line, "friends");
				if v ~= nil then
					data:set_integer("friends", v);
				end
				
				-- text : 複数行の可能性があるのでここでパース
				if line_has_strings(line, "\"text\":") then
					-- " 以降の文字列取得
					local after_dq = line:match(":[^\"]*\"(.*)");
					--mz3.logger_debug(after_dq);
					
					if after_dq:find("\"", 1, true) ~= nil then
						-- 終了
						local text = after_dq:match("(.*)\"");
						data:add_text_array("body", text);
					else
						-- " が見つかるまで取得する
						data:add_text_array("body", after_dq);
						data:add_text_array("body", "\r\n");
						i = i+1;
						while i<line_count-1 do
							line = html:get_at(i);
							if line:find("\"", 1, true) ~= nil then
								-- 終了
								local text = line:match("(.*)\"");
								data:add_text_array("body", text);
								break;
							else
								data:add_text_array("body", line);
								data:add_text_array("body", "\r\n");
							end
							
							i = i+1;
						end
					end
				end

				-- favorited
				v = get_json_value(line, "favorited");
				if v ~= nil then
					data:set_integer("favorited", v=="false" and 0 or 1);	-- 3項演算子
				end
				-- favorites
				v = get_json_value(line, "favorites");
				if v ~= nil then
					data:set_integer("favorites", v);
				end
				-- comments
				v = get_json_value(line, "comments");
				if v ~= nil then
					data:set_integer("comments", v);

					-- コメント数をボディの末尾につける
					data:add_text_array("body", "(" .. v .. ")");
				end
				-- created_at
				v = get_json_value(line, "created_at");
				if v ~= nil then
					data:parse_date_line(v);
				end
				-- id
				v = get_json_value(line, "id");
				if v ~= nil then
					data:set_text("id", v);

					-- 閲覧URLとしてひとことのURLを構築して設定しておく
					-- http://home.goo.ne.jp/quote/user/{goo_id}/detail/{id}?cnt={comments}
					url = string.format("http://home.goo.ne.jp/quote/user/%s/detail/%s?cnt=%d",
							data:get_text("goo_id"),
							v,
							data:get_integer("comments",0));
					data:set_text("url", url);
					data:set_text("browse_uri", url);
				end
			end
		end
	end

	-- TODO 新着マージ＋重複防止処理
	if in_quote then
		mz3_data_list.add(body, data.data);
	end
--[[

	-- 生成したデータを出力に反映
	TwitterParserBase::MergeNewList(out_, new_list);
]]

	-- オブジェクト削除
	data:delete();

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("quote_quotes_friends_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
