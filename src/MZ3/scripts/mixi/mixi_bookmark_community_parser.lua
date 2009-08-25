--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
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
type:set_info_type('category');									-- カテゴリ
type:set_service_type('mixi');									-- サービス種別
type:set_serialize_key('FAVORITE');				-- シリアライズキー
type:set_short_title('お気に入りコミュ');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('mixi\\bookmark_community.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');							-- エンコーディング
type:set_default_url('http://mixi.jp/list_bookmark.pl?kind=community');
type:set_body_header(1, 'name', 'コミュニティ');
type:set_body_header(2, 'title', '説明');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]

--------------------------------------------------
-- 【お気に入りコミュ一覧】
-- [list] bookmark.pl 用パーサ
--
-- http://mixi.jp/list_bookmark.pl?kind=community
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_bookmark_community_parser(parent, body, html)
	mz3.logger_debug("mixi_bookmark_community_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();
	local in_data_region = false;

	local back_data = nil;
	local next_data = nil;

	local deleted_community = "※このコミュニティはすでに閉鎖しています。不要な場合は削除してください。";

	-- 行数取得
	local line_count = html:get_count();
	for i=230, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "list_bookmark.pl");
		end

		-- 項目探索 以下一行
		-- <div class="listIcon">
		--   <a href="view_community.pl?id=3616089">
		--     <img src="http://community.img.mixi.jp/photo/comm/60/89/3616089_112s.jpg" alt="Windows Home Server" />
		--   </a>
		-- </div>
		if line_has_strings(line, "<div", "class", "listIcon") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 画像取得 それっぽいものを一応残しておく…
			image_url, after = line:match("src=\"([^\"]+)\"");
			-- image_md5 = mz3.make_image_logfile_path_from_url_md5( image_url );
			-- hoge = mz3_image_cache.get_image_index_by_url( image_url );
			data:add_text_array("image", image_url);

			i = i+2;
			line = html:get_at(i);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- コミュニティ名
			name, after = line:match(">([^<]+)(<.*)$");
			data:set_text("name", name);

			-- 参加者数
			users, after = line:match("span>%((.-)%)(<.*)$");
			data:set_date(users);

			i = i+1;
			line = html:get_at(i);

			-- 説明
			description, after = line:match(">([^<]+)(<.*)$");
--			data:set_text("title", description);
			data:add_body_with_extract(description);

			if description == deleted_community then
				-- 削除済みコミュは一覧に出さない
				data:delete();
			else

				-- URL に応じてアクセス種別を設定
				type = mz3.estimate_access_type_by_url(url);
				data:set_access_type(type);

				-- data 追加
				body:add(data.data);

				-- data 削除
				data:delete();
			end

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
	mz3.logger_debug("mixi_bookmark_community_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- パーサの登録
----------------------------------------
-- お気に入りコミュ
mz3.set_parser("FAVORITE_COMMUNITY", "mixi.mixi_bookmark_community_parser");
