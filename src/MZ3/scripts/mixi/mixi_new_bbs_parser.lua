--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id: mixi_new_bbs_parser.lua 1453 2009-12-13 13:06:06Z takke $
--------------------------------------------------
module("mixi", package.seeall)

-- ★コミュニティ関連のパーサはこのファイルに集約する


-- コミュニティ一覧
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('COMMUNITY');						-- シリアライズキー
type:set_short_title('コミュニティ一覧');					-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
-- url(1) : http://mixi.jp/list_community.pl?id=xxx
-- url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2
type:set_cache_file_pattern('mixi\\list_community{urlparam:page}.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
 type:set_default_url('http://mixi.jp/list_community.pl');
type:set_body_header(1, 'name', 'コミュニティ');
type:set_body_header(2, 'date', '人数');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');


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
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- 項目探索
		if line_has_strings(line, "<div", "class", "postData") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 次行取得
			i = i+1;
			line = html:get_at(i);

			-- コミュニティ名
			name = line:match(">([^<]+)(<.*)$");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);

			-- 次行取得
			i = i+1;
			line = html:get_at(i);

			-- 見出し
			title, after = line:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
			mz3.logger_debug(after);

			-- アンケート、イベントの場合はタイトルの前にマークを付ける
			if line_has_strings(line, "iconEvent") then
				title = "【☆】" .. title;
			elseif line_has_strings(line, "iconEnquete") then
				title = "【＠】" .. title;
			end
			data:set_text("title", title);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- 次3行取得
			i = i+3;
			line = html:get_at(i);

			-- 日付のパース
			local date = line:match('<li class="date">(.+)</li>');
			if date ~= nil then
				data:set_date(date);
			end

			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();
		end

		if in_data_region and line_has_strings(line, "<!--/communityFeed-->") then
			mz3.logger_debug("★<!--/communityFeed-->が見つかったので終了します");
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
	mz3.logger_debug("new_bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("BBS", "mixi.new_bbs_parser");


--------------------------------------------------
-- 【トピック自分の最新書き込み一覧】
-- [list] new_bbs.pl 用パーサ
--
-- http://mixi.jp/new_bbs_comment.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function new_bbs_comment_perser(parent, body, html)
	mz3.logger_debug("new_bbs_comment_parser start");

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
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- 項目探索
		-- <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
		if line_has_strings(line, "<dt", "class", "iconTopic") or 
		   line_has_strings(line, "<dt", "class", "iconEvent") or 
		   line_has_strings(line, "<dt", "class", "iconEnquete") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 日付のパース
			data:parse_date_line(line);

			-- 次行取得
			i = i+1;
			line2 = html:get_at(i);
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
			data:set_text("title", title);

			-- URL 取得
			url = line2:match("href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			data:set_text("url", url);

			-- コメント数
			data:set_integer("comment_count", get_param_from_url(url, "comment_count"));

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- コミュニティ名
			name = after:match("</a>[^(]*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);

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
	mz3.logger_debug("new_bbs_comment_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_comment_perser");


--------------------------------------------------
-- 【コミュニティ一覧】
-- [list] list_community.pl 用パーサ
--
-- http://mixi.jp/list_community.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_list_community_parser(parent, body, html)
	mz3.logger_debug("mixi_list_community_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();
	
	local back_data = nil;
	local next_data = nil;

	-- 行数取得
	local line_count = html:get_count();
	
	-- 開始フラグの探索
	local i_start_line = 100;
	while (i_start_line < line_count) do
		line = html:get_at(i_start_line);
		
		if line_has_strings(line, '<h3>コミュニティ</h3>') then
			break;
		end
		
		i_start_line = i_start_line + 1;
	end

	-- 次へ、前への取得
	local i=i_start_line;
	for i=i_start_line, line_count-1 do
		line = html:get_at(i);
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- 次へ、前への抽出処理
		if back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "list_community.pl", 'name');
		else
			break;
		end
	end

	-- 範囲を一括取得
	sub_html = get_sub_html(html, i_start_line, line_count, {'<div class="iconList03">'}, {'<div class="pageNavigation01'});
--	mz3.logger_debug('html:' .. sub_html);
	
	-- 各記事を取得
	local element_html = '';
	for element_html in sub_html:gmatch('(<li.-</li>)') do
--		mz3.logger_debug('element_html: ' .. element_html);
		
		-- data 生成
		data = MZ3Data:create();
		element_html = element_html:gsub('&nbsp;', ' ');

--[[
<li><div class="iconState01" id="xx">
<div class="iconListImage"><a href="view_community.pl?id=xx" style="background: url(http://community.img.mixi.jp/photo/comm/46/82/xxs.jpg); 
text-indent: -9999px;" class="iconTitle" title="au AQUOS PHONE IS13SH">au AQUOS PHONE IS13SHの写真</a></div><span>au AQUOS PHONE IS13SH(1753)</span>
..
</div></li>
]]
		-- URL、名前の抽出
		url, name, count = element_html:match('href="(.-)".-<span>(.*)%(([0-9]-)%)</span>');
		name = mz3.decode_html_entity(name);
		data:set_text("name", name);
		data:set_text("url", url);
		
		if count ~= nil then
			data:set_date(count .. '人');
		end
		
		local image_url = element_html:match('url[(](.-)[)]');
		if image_url ~= nil then
			data:add_text_array("image", image_url);
		end
		
		-- ID 設定
		id = get_param_from_url(url, "id");
		data:set_integer('id', id);

		-- URL に応じてアクセス種別を設定
		type = mz3.estimate_access_type_by_url(url);
		data:set_access_type(type);

		-- data 追加
		body:add(data.data);

		-- data 削除
		data:delete();
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
	mz3.logger_debug("mixi_list_community_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("COMMUNITY", "mixi.mixi_list_community_parser");
