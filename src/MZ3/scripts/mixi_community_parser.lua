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
type:set_params({
  info_type='category', service_type='mixi', key='COMMUNITY',
  short_title='コミュニティ一覧', request_method='GET', request_encoding='euc-jp'
});
-- url(1) : http://mixi.jp/list_community.pl?id=xxx
-- url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2
type:set_cache_file_pattern('mixi\\list_community{urlparam:page}.html');
type:set_default_url('http://mixi.jp/list_community.pl');
type:set_body_header(1, 'name', 'コミュニティ');
type:set_body_header(2, 'date', '人数');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');

-- トピック一覧
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='mixi', key='TOPIC',
  short_title='トピック一覧', request_method='GET', request_encoding='euc-jp'
});
-- url : list_bbs.pl?id=xxx
type:set_cache_file_pattern('mixi\\list_bbs_{urlparam:id}.html');
type:set_body_header(1, 'title', 'トピック');
type:set_body_header(2, 'date', '日付');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
type:set_cruise_target(true);



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
--			mz3.logger_debug(after);

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
-- [list] new_bbs_comment.pl 用パーサ
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


--------------------------------------------------
-- 【コミュニティーのトピック一覧】
-- [list] list_bbs.pl 用パーサ
--
-- http://mixi.jp/list_bbs.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_list_bbs_parser(parent, body, html)
	mz3.logger_debug("mixi_list_bbs_parser start");

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
		
		if line_has_strings(line, '<h3>トピック</h3>') then
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
			back_data, next_data = parse_next_back_link(line, "list_bbs.pl", 'title');
		else
			break;
		end
	end

	-- 範囲を一括取得
	sub_html = get_sub_html(html, i_start_line, line_count, {'<dl class="bbsList01">'}, {'<div class="pageNavigation01'});
--	mz3.logger_debug('html:' .. sub_html);
	
	-- 各記事を取得
	local element_html = '';
	for element_html in sub_html:gmatch('(<dt.-</dl>)') do
--		mz3.logger_debug('element_html: ' .. element_html);
		
		-- data 生成
		data = MZ3Data:create();
		element_html = element_html:gsub('&nbsp;', ' ');

--[[
<dt class="bbsTitle clearfix">
<span class="titleSpan"><a href="view_bbs.pl?id=68668641&comm_id=4405616" class="title">【Xperia】雑談トピック 11</a>
</span>
<span class="date">2012年08月07日 10:57</span>
</dt>
]]
		-- URL、名前の抽出
		url, title = element_html:match('href="(.-)".->(.-)</a>');
		title = mz3.decode_html_entity(title);
		data:set_text("title", title);
		data:set_text("url", url);
		
		-- 日付のパース
		local date = element_html:match('<span class="date">(.-)</span>');
		data:parse_date_line(date);

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
	mz3.logger_debug("mixi_list_bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TOPIC", "mixi.mixi_list_bbs_parser");


--------------------------------------------------
-- 【コミュニティートピック詳細】
-- [content] view_bbs.pl 用パーサ
--
-- http://mixi.jp/view_bbs.pl
-- 
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_bbs_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_bbs_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();

	-- 名前初期化
	data:set_text('name', '');

	-- 改行出力
	data:add_body_with_extract("<br>");

	local line_count = html:get_count();

	-- 範囲を一括取得
	local sub_html = get_sub_html(html, 100, line_count, {'<div id="bodyMainArea">'}, {'<div id="bodySub">'});
	
	-- 本文取得
	local main_area = sub_html:match('(<dl class="bbsList01.-<div id="bbsComment">)');
	if main_area ~= nil then
		parse_bbs_enquete_event_main_area(data, main_area);
	end
	
	-- コミュニティ名抽出
	local communityName = sub_html:match('<p class="utilityLinks03"><a.->%[(.-)%]');
	if communityName ~= nil then
		data:set_text('name', communityName);
	end
	
	-- ページ移動リンクの抽出
	parse_page_link(data, sub_html);
	
	-- 「最新のトピック」の抽出
	parse_recent_topics(data, sub_html);
	
	-- 投稿先URLの取得
	parse_post_url(data, sub_html);
	
	-- コメント取得
	local comment_area = sub_html:match('<dl class="commentList01">(.-)<div class="pageNavigation01');
	if comment_area ~= nil then
		parse_bbs_enquete_event_comments(data, comment_area);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_BBS" , "mixi.mixi_view_bbs_parser");


--------------------------------------------------
-- 【アンケート詳細】
-- [content] view_enquete.pl 用パーサ
--
-- http://mixi.jp/view_enquete.pl
-- 
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_enquete_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_enquete_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();

	-- 名前初期化
	data:set_text('name', '');

	-- 改行出力
	data:add_body_with_extract("<br>");

	local line_count = html:get_count();

	-- 範囲を一括取得
	local sub_html = get_sub_html(html, 100, line_count, {'<div class="pageTitle'}, {'<div id="bodySub">'});
	
	-- 本文取得
	local main_area = sub_html:match('(<dl class="bbsList01.-<div id="enqueteComment">)');
	if main_area ~= nil then
		-- 共通部分抽出
		parse_bbs_enquete_event_main_area(data, main_area);
		
		-- 集計結果解析
		local result = main_area:match('<div class="enquete_meter">(.-)</div>');
		if result ~= nil then
			
			data:add_body_with_extract("<br>");
			data:add_body_with_extract("<br>");
			data:add_body_with_extract("◆集計結果<br>");
			
			for m in result:gmatch('<dl class="enqueteList">(.-)</dl>') do
				local e_name = m:match('<dt>(.-)<');
				local e_vote, e_p = m:match('<dd><span>(.-)</span>(.-)</dd>');
				data:add_body_with_extract('  ●' .. e_name .. '<br>');
				data:add_body_with_extract('      ' .. e_vote .. ' ' .. e_p .. '<br>');
			end
			
			-- 合計
			local total = result:match('<dl class="enqueteTotal">.-<span>(.-)</span></dt></dl>');
			data:add_body_with_extract('  ●合計<br>');
			data:add_body_with_extract('      ' .. total .. '<br>');
		end
	end
	
	-- コミュニティ名抽出
	local communityName = sub_html:match('<p class="utilityLinks03"><a.->%[(.-)%]');
	if communityName ~= nil then
		data:set_text('name', communityName);
	end
	
	-- ページ移動リンクの抽出
	parse_page_link(data, sub_html);
	
	-- 「最新のトピック」の抽出
	parse_recent_topics(data, sub_html);
	
	-- 投稿先URLの取得
	parse_post_url(data, sub_html);
	
	-- コメント取得
	local comment_area = sub_html:match('<dl class="commentList01">(.-)<div class="bbsNewItem01');
	if comment_area ~= nil then
		parse_bbs_enquete_event_comments(data, comment_area);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_enquete_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_ENQUETE" , "mixi.mixi_view_enquete_parser");


--------------------------------------------------
-- 【イベント詳細】
-- [content] view_event.pl 用パーサ
--
-- http://mixi.jp/view_event.pl
-- 
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_event_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_event_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();

	-- 名前初期化
	data:set_text('name', '');

	-- 改行出力
	data:add_body_with_extract("<br>");

	local line_count = html:get_count();

	-- 範囲を一括取得
	local sub_html = get_sub_html(html, 100, line_count, {'<div id="bodyMainArea">'}, {'<div id="bodySub">'});
	
	-- 本文取得
	local main_area = sub_html:match('(<dl class="bbsList01.-<div id="eventComment">)');
	if main_area ~= nil then
		-- 共通部分抽出
		parse_bbs_enquete_event_main_area(data, main_area);
		
		-- イベントデータ
		local date = main_area:match('<dt>開催日時</dt>.-<dd>(.-)</dd>');
		if date ~= nil then
			data:add_body_with_extract('●開催日時 ： ' .. date .. '<br><br>');
		end
		
		local s = main_area:match('<dt>開催場所</dt>.-<dd>(.-)</dd>');
		if s ~= nil then
			data:add_body_with_extract('●開催場所 ： ' .. s .. '<br><br>');
		end
		
		local s = main_area:match('<dt>募集期限</dt>.-<dd>(.-)</dd>');
		if s ~= nil then
			data:add_body_with_extract('●募集期限 ： ' .. s .. '<br><br>');
		end
		
		local s = main_area:match('<dt>参加者</dt>.-<dd>(.-)</dd>');
		if s ~= nil then
			data:add_body_with_extract('●参加者 ： ' .. s .. '<br><br>');
		end
		
		-- 参加者一覧
		local current_url = data:get_text('url');
		local id = get_param_from_url(current_url, 'id');
		local comm_id = get_param_from_url(current_url, 'comm_id');
		local url = 'http://mixi.jp/list_event_member.pl?id=' .. id .. '&comm_id=' .. comm_id;
		data:add_link_list(url, '★参加者一覧', 'page');
	end
	
	-- コミュニティ名抽出
	local communityName = sub_html:match('<p class="utilityLinks03"><a.->%[(.-)%]');
	if communityName ~= nil then
		data:set_text('name', communityName);
	end
	
	-- ページ移動リンクの抽出
	parse_page_link(data, sub_html);
	
	-- 「最新のトピック」の抽出
	parse_recent_topics(data, sub_html);
	
	-- 投稿先URLの取得
	parse_post_url(data, sub_html);
	
	-- コメント取得
	local comment_area = sub_html:match('<dl class="commentList01">(.-)<div class="bbsNewItem01');
	if comment_area ~= nil then
		parse_bbs_enquete_event_comments(data, comment_area);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_event_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_EVENT" , "mixi.mixi_view_event_parser");
mz3.set_parser("MIXI_EVENT_JOIN" , "mixi.mixi_view_event_parser");


-- ページ移動リンクの抽出
function parse_page_link(data, html)
	
	local pl = html:match('<div class="pageList01">(.-)</div>');
	if pl ~= nil then
		pl = pl:gsub('<wbr/>', '');
--		mz3.logger_debug(pl);
		
		for href, name in pl:gmatch('<a href="(.-)">(.-)</a>') do
			data:add_link_list(complement_mixi_url(href), name, 'page');
		end
	end
	
	pl = html:match('<div class="pageList02">(.-)</div>');
	if pl ~= nil then
		pl = pl:gsub('<wbr/>', '');
--		mz3.logger_debug(pl);
		
		for href, name in pl:gmatch('<a href="(.-)">(.-)</a>') do
			data:add_link_list(complement_mixi_url(href), name, 'page');
		end
	end
end


-- 「最新のトピック」の抽出
function parse_recent_topics(data, html)
	
	local pl = html:match('<ul class="newTopicList01">(.-)<p class="utilityLinks03">');
	if pl ~= nil then
		pl = pl:gsub('<wbr/>', '');
		
		for href, name in pl:gmatch('<a href="(.-)">(.-)</a>') do
			if line_has_strings(href, 'view_enquete.pl') or
			   line_has_strings(href, 'view_event.pl') or
			   line_has_strings(href, 'view_bbs.pl') then
				data:add_link_list(complement_mixi_url(href), name, 'page');
			end
		end
	end
end


-- 投稿先URLの取得
function parse_post_url(data, html)

	-- <form name="bbs_comment_form" action="add_bbs_comment.pl?id=67676121&comm_id=5003071" enctype="multipart/form-data" method="post">
	local action, param = html:match('<form [^>]-action="(add_.-)"(.-)>');
	if action ~= nil then
		mz3.logger_debug('action:' .. action);
		data:set_text('post_address', action);
	end
	
	if param ~= nil then
		-- @see constants.h
		if line_has_strings(param, 'multipart') then
			data:set_text('content_type', "multipart/form-data; boundary=---------------------------7d62ee108071e");
		else
			data:set_text('content_type', "application/x-www-form-urlencoded");
		end
	end
end


-- BBS、アンケート、イベント用の共通本文パーサ
function parse_bbs_enquete_event_main_area(data, main_area)

	-- 日付取得
	local date = main_area:match('<span class="date">(.-)</span>');
	if date ~= nil then
--		mz3.logger_debug(date);
		data:parse_date_line(date);
	end
	
	-- タイトル取得
	local title = main_area:match('<span class="titleSpan"><span class="title">(.-)</span>');
	if title ~= nil then
		data:set_text('title', mz3.decode_html_entity(title));
	end
	
	-- トピック作成者のURL
	local author_profile_url, name = main_area:match('<dd class="bbsContent">.-<dt>.-href="(.-)">(.-)</a>');
	-- TODO 退会済みケア
--	mz3.logger_debug(author_profile_url);
--	mz3.logger_debug(name);
	-- ID抽出
	if author_profile_url ~= nil then
		data:set_integer('author_id', get_param_from_url(author_profile_url, 'id'));
	end
	
	-- 名前抽出
	if name ~= nil then
		data:set_text('author', mz3.decode_html_entity(name));
	else
		data:set_text('author', '');
	end
	
	-- 本文抽出
	local dd = main_area:match('<dd class="bbsContent">.-<dd>(.-)</dd>');
	if dd ~= nil then
		-- 不要なタグの除去
		dd = dd:gsub('<p class="reportLink01".-</p>', '');
		
		data:add_body_with_extract(dd);
		data:add_body_with_extract('<br>');
	end
end


-- BBS、アンケート、イベント用の共通コメントパーサ
function parse_bbs_enquete_event_comments(data, comment_area)

	for comment_html in comment_area:gmatch('(<dt class="commentDate.-<dd>.-<dd>.-</dd>)') do
		
		local child = MZ3Data:create();
		
--[[
<dt class="commentDate clearfix"><span class="senderId">
<input id="commentCheck01" name="comment_id" type="checkbox" value="xx" /><label for="commentCheck01">962</label></span>
<span class="date">2012年08月03日 22:10</span></dt>
<dd>
<dl class="commentContent01">
<dt><a href="show_friend.pl?id=xx&route_trace=xx&content_id=xxx">なまえ</a></dt>
<dd>
ほんぶん
...
</dd>
</dl>
</dd>
]]
		-- コメント番号
		local number = comment_html:match('<span class="senderId">(.-)</span>');
		if number ~= nil then
			-- タグ除去
			number = number:gsub('<.->', '');
			number = number:gsub('[^0-9]+', '');
			mz3.logger_debug(number);
			child:set_integer('comment_index', number);
		end
		
		-- 日付
		local date = comment_html:match('<span class="date">(.-)</span>');
		child:parse_date_line(date);
		
		-- 名前
		local url, name, comment = comment_html:match('<dt.-href="(.-)">(.-)</a.-<dd>(.-)</dd>');
		child:set_integer('author_id', get_param_from_url(url, 'id'));
		child:set_text('author', mz3.decode_html_entity(name));
		
		-- コメント
		comment = comment:gsub('<p class="reportLink01".-</p>', '');
		comment = comment:gsub('<ul class="listAction">.-</ul>', '');

		child:add_body_with_extract('<br>');
		child:add_body_with_extract(mz3.decode_html_entity(comment));
		
--		child:add_link_list(complement_mixi_url(url), name .. ' さん');
		
		data:add_child(child);
		
		child:delete();
	end
end
