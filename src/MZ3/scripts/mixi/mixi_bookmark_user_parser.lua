--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id: mixi_bookmark_community_parser.lua
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- アクセス種別の登録
----------------------------------------
-- TODO 本来は Lua 側でやるべき↓
--[[
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('FAVORITE');							-- シリアライズキー
type:set_short_title('お気に入りユーザー');					-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_cache_file_pattern('mixi\\bookmark_user.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
type:set_default_url('http://mixi.jp/view_mylist.pl');
type:set_body_header(1, 'name', 'ユーザー名');
type:set_body_header(2, 'title', '自己紹介');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]

--------------------------------------------------
-- 【お気に入りユーザー一覧】
-- [list] bookmark.pl 用パーサ
--
-- http://mixi.jp/list_bookmark.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_bookmark_user_parser(parent, body, html)
	mz3.logger_debug("mixi_bookmark_user_parser start");

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
	for i=200, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "view_mylist.pl", "name");
		end

		-- 項目探索 以下一行
		-- <td class="thumb">
		if line_has_strings(line, "<td", "class", "thumb") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			i = i+1;
			line = html:get_at(i);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			if url ~= nil and url ~= "" then
				url = complement_mixi_url(url);
				data:set_text("url", url);

				i = i+1;
				line = html:get_at(i);

				-- 画像取得
				image_url = line:match("src=\"([^\"]+)\"");
				data:add_text_array("image", image_url);

				i = i+4;
				line = html:get_at(i);

				-- ユーザ名
				name = line:match(">([^<]+)(<.*)$");
				data:set_text("name", name);

				if name ~= nil and name ~= "" then
					-- 退会ユーザは表示しない

					i = i+1;
					line = html:get_at(i);

					-- 最終ログイン
					login = line:match(">([^<]+)(<.*)$");
					if login~=nil then
						login = login:match('%((.-)%)');
						data:set_date(login);
					end

					for i=i+2, line_count-1 do
						line = html:get_at(i);
						if line_has_strings(line, 'date') then
							break;
						end
					end

					-- 追加日
					add_date = line:match(">([^<]+)(<.*)$");
					data:add_body_with_extract(add_date);

					-- URL に応じてアクセス種別を設定
					type = mz3.estimate_access_type_by_url(url);
					data:set_access_type(type);

					-- data 追加
					body:add(data.data);

				end
			end

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
	mz3.logger_debug("mixi_bookmark_user_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- パーサの登録
----------------------------------------
-- お気に入りユーザー
mz3.set_parser("FAVORITE", "mixi.mixi_bookmark_user_parser");
