--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi.lua start');
module("mixi", package.seeall)

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('mixi', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('mixi', 'id_name', 'メールアドレス');
mz3_account_provider.set_param('mixi', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- イベント参加者一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');									-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('MIXI_EVENT_MEMBER');				-- シリアライズキー
type:set_short_title('イベント参加者一覧');					-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_request_encoding('euc-jp');						-- エンコーディング

-- ボイス詳細
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('MIXI_RECENT_VOICE_DETAIL');			-- シリアライズキー
type:set_short_title('ボイス');								-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_cache_file_pattern('mixi\\comment.html');			-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
type:set_body_header(1, 'title', 'コメント');
type:set_body_header(2, 'name', '名前');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');

-- ニュース一覧
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('NEWS');								-- シリアライズキー
type:set_short_title('コメント一覧');						-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_cache_file_pattern('mixi\\list_news_category_{urlparam:id}_{urlparam:page}.html');			-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
-- url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
-- url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
-- url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
-- url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn
type:set_body_header(1, 'title', '見出し');
type:set_body_header(2, 'date', '配信時刻>>');
type:set_body_header(3, 'name', '配給元>>');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2 (%3)');

-- 紹介文一覧
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('INTRO');							-- シリアライズキー
type:set_short_title('紹介文');								-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
-- url(1) : http://mixi.jp/show_intro.pl
-- url(2) : http://mixi.jp/show_intro.pl?page=2
type:set_cache_file_pattern('mixi\\show_intro{urlparam:page}.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
 type:set_default_url('http://mixi.jp/show_intro.pl');
type:set_body_header(1, 'name', '名前');
type:set_body_header(2, 'body', '紹介文');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');


--------------------------------------------------
-- 【プロフィール】
-- [content] show_friend.pl 用パーサ
--
-- http://mixi.jp/show_friend.pl
--
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_show_friend_parser(parent, body, html)
	mz3.logger_debug("mixi_show_friend_parser start");

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);
	
	parent:clear();
	
	parent:set_text('name', '');
	parent:set_date('');

	local t1 = mz3.get_tick_count();
	local line_count = html:get_count();
	
	-- 名前, 画像, 最終ログイン
	local i=100;
	local sub_html = '';
	sub_html, i = get_sub_html(html, i, line_count, {"<div", '"profilePhoto"'}, {'<div', 'id="myMixiList"'});
--	mz3.logger_debug(sub_html);
	--<p class="name">bigappleさん(30)
	--<span class="loginTime">（最終ログインは45分以内）</span></p>
	local name = sub_html:match('<p class="name">(.-)%(');
	if name ~= nil then
		name = mz3.decode_html_entity(name);
		parent:set_text('name', name);
		parent:set_text('title', name);
		parent:set_text('author', name);
	end
	
	-- ユーザ画像
	local url = sub_html:match('<img [^>]*src="(.-)"');
	if url ~= nil then
		parent:add_link_list(url, 'ユーザ画像');
		parent:add_text_array("body", "<_a><<ユーザ画像>></_a>");
	end
	
	-- 最終ログイン
	-- <p class="loginTime">（最終ログインは3日以上）</p>
	local last_login = sub_html:match('class="loginTime">(.-)<');
	if last_login ~= nil then
		parent:add_body_with_extract(" ");
		parent:add_body_with_extract(last_login);
	end
	
	-- プロフィールを全て取得し、本文に設定する。
	sub_html, i = get_sub_html(html, i, line_count, {'<div id="profile"'}, {'</table>'});
--	mz3.logger_debug(sub_html);
	parent:add_body_with_extract("<br>");
	for th, td in sub_html:gmatch("<th>(.-)</th>.-<td>(.-)</td>") do
		parent:add_body_with_extract("■ " .. th);
		parent:add_body_with_extract("<br>");
		
		parent:add_body_with_extract(td);
		parent:add_body_with_extract("<br>");
		parent:add_body_with_extract("<br>");
	end
	
	-- 最新の日記取得
	local child_number = 1;
	sub_html, i = get_sub_html(html, i, line_count, {'<div', 'id="diaryFeed"'}, {'</dl>'});
	-- dt/span, dd/a が交互に出現する。
	-- <dt><span>05月03日</span></dt>
	-- <dd><a href="view_diary.pl?id=xxx&owner_id=xxx" >WM系の偉い人と飲み会に行って…</a></dd>
	local child = MZ3Data:create();
	child:add_body_with_extract("<br>");
	local has_diary = false;
	for dt, dd in sub_html:gmatch("<dt>(.-)</dt>.-<dd>(.-)</dd>") do
		local date = dt:match("<span>(.-)<");
		
		if date ~= nil then
			child:add_body_with_extract("■ " .. date .. " : " .. dd);
			child:add_body_with_extract("<br>");
		end
		
		has_diary = true;
	end
	if has_diary then
		child:set_integer("comment_index", child_number);
		child:set_text('author', "最新の日記");
		parent:add_child(child);
		child_number = child_number + 1;
		child:delete();
	end
	
	-- 紹介文取得
	sub_html, i = get_sub_html(html, i, line_count, {'<div id="intro"'}, {'</ul>'});
--	mz3.logger_debug(sub_html);
	--[[
<dl>
<dt><a href="show_friend.pl?id=xxx"><img src="xxx.jpg" alt="ゆ" onerror="javascript:this.width=76;this.height=76;" /></a>
<br /><a href="show_friend.pl?id=xxx">ゆ</a></dt>
<dd>
<p class="relation">関係：xxx</p>
<p class="userInput">xxxです。</p>
</dd>
</dl>
]]
	local child = MZ3Data:create();
	child:add_body_with_extract("<br>");
	local has_intro = false;
	for dt, dd in sub_html:gmatch("<dt>(.-)</dt>.-<dd>(.-)</dd>") do
		local name     = dt:match('</a>.-(<a.-</a>)');
		local relation = dd:match('"relation">(.-)<');
		local text     = dd:match('"userInput">(.-)</p');

		child:add_body_with_extract("■ " .. name .. "<br>");
		child:add_body_with_extract(relation);
		child:add_body_with_extract("<br>");
		child:add_body_with_extract(text);
		child:add_body_with_extract("<br>");
		child:add_body_with_extract("<br>");
		
		has_intro = true;
	end
	if has_intro then
		child:set_integer("comment_index", child_number);
		child:set_text('author', "紹介文");
		parent:add_child(child);
		child_number = child_number + 1;
	end
	child:delete();
	
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_show_friend_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_PROFILE", "mixi.mixi_show_friend_parser");


--------------------------------------------------
-- 【イベント参加者一覧】
-- [content] list_event_member.pl 用パーサ
--
-- http://mixi.jp/list_event_member.pl
--
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_list_event_member_parser(parent, body, html)
	mz3.logger_debug("mixi_list_event_member_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);

	parent:clear();
	
	-- 複数行に分かれているので1行に結合
	local line = html:get_all_text();
	
	-- タイトルを取得する
	name = line:match('<div class="pageTitle communityTitle002">.-<h2>(.-)</');
	if name ~= nil then
		parent:set_text('title', name);
	end
	parent:set_text('name', '');
	
--	target_region = line:match('<div class="iconList03">.-<ul.->(.-)</ul>');
	target_region = line:match('<div class="iconList03">.-<ul.->(.-)<div class="pageNavigation01 bottom">');
	user_num = 1;
	for li_tag in target_region:gmatch("<li.->(.-)</li>") do
--		mz3.logger_debug(li_tag);
		
		local name = mz3.decode_html_entity(li_tag:match('<span>(.-)<'));
		local href = mz3.decode_html_entity(li_tag:match('<a.-href="(.-)"'));
		local image_url = mz3.decode_html_entity(li_tag:match('(http://profile\.img.-\.jpg)'));
		
		if name ~= '' and href ~= '' then
			local child = MZ3Data:create();
			
			child:set_integer('comment_index', user_num);
			child:set_text('name', name);
			child:set_text('title', name);
			child:set_text('author', name);
			child:set_integer('author_id', get_param_from_url(href, 'id'));
			child:set_text('url', href);
			child:set_access_type(mz3.get_access_type_by_key('MIXI_PROFILE'));
			child:add_link_list(image_url, 'ユーザ画像');

			parent:add_child(child);

			child:delete();
			user_num = user_num +1;
		end
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_list_event_member_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_EVENT_MEMBER", "mixi.mixi_list_event_member_parser");


--------------------------------------------------
-- 【みんなのエコー一覧】
-- [list] recent_voice.pl 用パーサ
--
-- http://mixi.jp/recent_voice.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_recent_voice_parser(parent, body, html)
	mz3.logger_debug("mixi_recent_voice_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();

	local line_count = html:get_count();
	
	-- post_key 探索
	mixi.post_key = '';
	for i=100, line_count-1 do
		line = html:get_at(i);
		
		-- <input type="hidden" name="post_key" id="post_key" value="xxx"> 
		if line_has_strings(line, 'hidden', 'name="post_key"') then
			mixi.post_key = line:match('value="(.-)"');
			mz3.logger_debug("post_key: " .. mixi.post_key);
			break;
		end
	end

	-- 複数行に分かれているので1行に結合
	local line = html:get_all_text();

	-- 次へ、前への抽出処理
	-- <li><a href="recent_voice.pl?direction=next&owner_id=85892&base_post_time=20120804140705">次を表示</a></li>
	if line_has_strings( line, "recent_voice.pl" ) then
		back_data, next_data = parse_next_back_link(line, "recent_voice.pl", "name");
	end


	for li_tag in line:gmatch('<li class="archive">(.-)<p class="error"') do
--		mz3.logger_debug(li_tag);
		
		-- 以下一行
		-- <span class="thumb"><a href="list_voice.pl?owner_id=xx">
		-- <img src="http://profile.img.mixi.jp/photo/member/58/92/xx.jpg" alt="たっけ" /></a></span>
		local span = li_tag:match('<span class="thumb">(.-)</span>');
		if span ~= nil then
			-- data 生成
			data = MZ3Data:create();

			-- 画像取得
			local image_url = span:match('<img src="(.-)"');
			if image_url ~= nil then
				data:add_text_array("image", image_url);
			--	mz3.alert(image_url);
			end
			
			-- URL 取得
			local url = span:match('<a href="(.-)"');
			if url ~= nil then
				url = complement_mixi_url(url);
				data:set_text("url", url);
			end

			-- ユーザ名
			local nickname = li_tag:match('name="nickname" value="(.-)"');
			if nickname ~= nil then
				data:set_text("name", nickname);
			end

			-- 発言
			local voiced = li_tag:match('<div class="voiced">(.-)</div>');
			if voiced ~= nil then
				local comment = voiced:match('<p>(.-)</p');
				comment = comment:gsub("\n", '');
				comment = comment:gsub('<a.->', '');
				comment = comment:gsub('</a>', ' ');
				data:add_body_with_extract(comment .. "\r\n");
			end

			-- id
			-- <input type="hidden" name="member_id" value="85892" class="memberId" />
			local author_id = li_tag:match('name="member_id".-value="(.-)"');
			if author_id ~= nil then
				data:set_integer("author_id", author_id);
				data:set_integer("id", author_id);
			end

			-- 時間
			local date = voiced:match('<span>.-<a.->(.-)</a>');
			if date ~= nil then
				date = date:gsub("\n", '');
				data:set_date(date);
			end

			for dd_feedback in li_tag:gmatch('<dd class="feedback">(.-)</dd') do
--				mz3.logger_debug(dd_feedback);
				
				local name = dd_feedback:match('<a .->(.-)</a>');
				local url = dd_feedback:match('href="(.-)"');
				if name ~= nil and url ~= nil then
					data:add_link_list(complement_mixi_url(url), 'イイネ！by ' .. name);
				end
			end

			-- コメント
			for more_comment in li_tag:gmatch('<p class="moreLink01 hrule">(.-)</p') do
				local url = more_comment:match('href="(.-)"');
				url = complement_mixi_url(url);
				local count = more_comment:match("commentCount\">(.-)</span");
				if count ~= nil then
					data:set_text("extra_url", url);
					data:set_text("extra_count", count);
				end
			end

			for dd_commentRow in li_tag:gmatch('<dd class="commentRow hrule">(.-)</dd') do
				local commentName = dd_commentRow:match('class="commentNickname">(.-)<');
				local extra_comment = dd_commentRow:match('<p class="commentBody">(.-)<');
				extra_comment = "\r\n" .. "　　コメント by " .. commentName ..  "： " .. extra_comment .. "";

				-- data:add_body_with_extract(extra_comment);
				local before_comment = data:get_text('extra_comment');
				if before_comment ~= nil then
					extra_comment = before_comment .. extra_comment;
				end

				data:set_text('extra_comment', extra_comment);
			end

--[[
			-- 引用ユーザがあれば抽出しておく
			if line_has_strings(comment, '<a', 'view_voice.pl', '</a>') then
				local ref_user_id, ref_user_name = comment:match('<a href=".-id=(.-)&.-">&gt;&gt;(.-)</');
				data:set_integer('ref_user_id', ref_user_id);
				data:set_text('ref_user_name', ref_user_name);
			end
			
			-- echo_post_time
			local echo_post_time = sub_html:match('class="echo_post_time".->(.-)</');
			if echo_post_time ~= nil then
				data:set_text('echo_post_time', echo_post_time);
			end
]]

			-- post_time
			-- <input type="hidden" name="post_time" value="20110106003426" class="postTime" />
			local echo_post_time = li_tag:match('name="post_time".-value="(.-)"');
			if echo_post_time ~= nil then
				data:set_text('echo_post_time', echo_post_time);
				mz3.logger_debug("echo_post_time：" .. echo_post_time);
			end

			-- URL に応じてアクセス種別を設定
			local type = mz3.get_access_type_by_key('MIXI_RECENT_VOICE_ITEM');
			data:set_access_type(type);
			
			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();

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
	mz3.logger_debug("mixi_recent_voice_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
-- みんなのエコー
mz3.set_parser("MIXI_RECENT_ECHO", "mixi.mixi_recent_voice_parser");
mz3.set_parser("MIXI_RES_VOICE"  , "mixi.mixi_recent_voice_parser");
mz3.set_parser("MIXI_LIST_VOICE" , "mixi.mixi_recent_voice_parser");


--------------------------------------------------
-- 【発言詳細】
-- 
-- 
-- 
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_recent_voice_detail_parser(parent, body, html)
	mz3.logger_debug("mixi_recent_voice_detail_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();

	local line_count = html:get_count();

--[[
	-- post_key 探索
	mixi.post_key = '';
	for i=200, line_count-1 do
		line = html:get_at(i);
		
		-- <input type="hidden" name="post_key" id="post_key" value="xxx"> 
		if line_has_strings(line, 'hidden', 'name="post_key"') then
			mixi.post_key = line:match('value="(.-)"');
			mz3.logger_debug("post_key: " .. mixi.post_key);
			break;
		end
	end
]]
	for i=300, line_count-1 do

		if line_has_strings(line, "</div") then
			break;
		end

		line = html:get_at(i);
--		if line_has_strings(line, "<dd", "class", "commentRow hrule") then
		if line_has_strings(line, "<dd", "class", "commentRow") or
		   line_has_strings(line, "<li", "class", "commentRow") then
			-- data 生成
			data = MZ3Data:create();

			i = i+1;
			line = html:get_at(i);
--[[
			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			if url ~= nil then
				url = complement_mixi_url(url);
				data:set_text("url", url);
			end
]]
			-- 画像
			image = line:match('src="(.-)"');
			-- image = line:match("src=\"([^\"]+)\"");
			if image ~= nil then
				data:add_text_array("image", image);
			end

			i = i+2;
			line = html:get_at(i);

			-- 名前
			name = line:match(">([^<]+)(<.*)$");
			if name ~= nil then
				data:set_text("name", name);
			end

			i = i+1;
			line = html:get_at(i);

			-- 日付
			date = line:match(">([^<]+)(<.*)$");
			if date ~= nil then
				date = date:gsub("\n", '');
				data:set_date(date);
			end

			i = i+3;
			line = html:get_at(i);

			-- コメント
			comment = line:match(">([^<]+)(<.*)$");
			if comment ~= nil then
				data:set_text("title", comment);
			end

			-- URL に応じてアクセス種別を設定
			local type = mz3.get_access_type_by_key('MIXI_RECENT_VOICE_DETAIL');
			data:set_access_type(type);
			
			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();

		end

	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_recent_voice_detail_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_RECENT_VOICE_DETAIL" , "mixi.mixi_recent_voice_detail_parser");


--------------------------------------------------
--- 次へ、前への抽出処理
--------------------------------------------------
function parse_next_back_link(line, base_url, title_set_at)

	local back_data = nil;
	local next_data = nil;
	if title_set_at == nil then
		title_set_at = "title";
	end
	
	-- <ul><li><a href="new_bbs.pl?page=1">前を表示</a></li>
	-- <li>51件～100件を表示</li>
	-- <li><a href="new_bbs.pl?page=3">次を表示</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- 前
		local url, t = line:match([[href="([^"]+)">(前[^<]+)<]]);
		if url~=nil then
			back_data = MZ3Data:create();
			back_data:set_text(title_set_at, "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- 次
		local url, t = line:match([[href="([^"]+)">(次[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text(title_set_at, "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end

--------------------------------------------------
--- mixi 用URL補完
--------------------------------------------------
function complement_mixi_url(url)
	if (url:find("mixi.jp", 1, true) == nil and
	    url:find("http://", 1, true) == nil) then
	    url = "http://mixi.jp/" .. url;
	end
	
	return url;
end


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.mixi_echo_item_read    = mz3_menu.regist_menu("mixi.on_mixi_echo_read_menu_item");
menu_items.mixi_echo_update       = mz3_menu.regist_menu("mixi.on_mixi_echo_update");
menu_items.mixi_echo_reply        = mz3_menu.regist_menu("mixi.on_mixi_echo_reply");
menu_items.mixi_echo_show_profile = mz3_menu.regist_menu("mixi.on_mixi_echo_show_profile");
menu_items.mixi_echo_add_user_echo_list = mz3_menu.regist_menu("mixi.on_mixi_echo_add_user_echo_list");
menu_items.mixi_echo_add_ref_user_echo_list = mz3_menu.regist_menu("mixi.on_mixi_echo_add_ref_user_echo_list");

menu_items.mixi_open_browser = mz3_menu.regist_menu("mixi.on_mixi_open_browser");

----------------------------------------
-- イベントハンドラ
----------------------------------------

--- Twitter風書き込みモードの初期化
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='mixi' then
		-- モード変更
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('MIXI_ADD_VOICE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "mixi.on_reset_twitter_style_post_mode");


--- Twitterスタイルのボタン名称の更新
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'MIXI_ADD_VOICE' then
		return true, 'voice';
	elseif serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		return true, '返信';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "mixi.on_update_twitter_update_button");


--- 「つぶやく」メニュー用ハンドラ
function on_mixi_echo_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_VOICE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- TODO 2010/4/14 仕変で消えた
--- 「返信」メニュー用ハンドラ
function on_mixi_echo_reply(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_VOICE_REPLY);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

--- 「プロフィール」メニュー用ハンドラ
function on_mixi_echo_show_profile(serialize_key, event_name, data)

	-- 選択中の要素取得
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	
	-- URL 取得
	url = data:get_text('url');
	-- view_voice.pl なのでプロフィールページURLに置換する
	url = 'http://mixi.jp/show_friend.pl?id=' .. get_param_from_url(url, 'owner_id');
--	mz3.alert(url);
	
	-- プロフィール取得アクセス開始
	access_type = mz3.get_access_type_by_key("MIXI_PROFILE");
	referer = "";
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

	-- 表示状態更新
	mz3_main_view.update_control_status();
end

--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="MIXI_RECENT_VOICE_ITEM" or
	   serialize_key=="MIXI_RECENT_VOICE_DETAIL" then
		-- 全文表示
		return on_mixi_echo_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");


--- 全文表示メニューまたはダブルクリックイベント
function on_mixi_echo_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_mixi_echo_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	-- 本文を1行に変換して表示
	item = data:get_text_array_joined_text('body');
	item = item:gsub("\r\n", "");
	
	extra_comment = data:get_text('extra_comment');
	comment = data:get_text('title');

	if extra_comment ~= nil and extra_comment ~= '' then
		if data:get_text('extra_count') ~= "" then
			item = item .. "\r\n　" .. "総コメント件数：" .. data:get_text('extra_count') .. "件";
		end
		item = item .. "　" .. extra_comment;
	elseif comment ~= nil then
		item = item .. "　" .. comment;
	end
	
	item = item .. "\r\n";
	item = item .. "----\r\n";
	item = item .. "name : " .. data:get_text('name') .. "\r\n";
	item = item .. data:get_date();

	mz3.alert(item, data:get_text('name'));

	return true;
end


--- 「xxx のボイス」メニュー用ハンドラ
function on_mixi_echo_add_user_echo_list(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = name .. "さんのボイス";
	author_id = body:get_integer('author_id');
	url = "http://mixi.jp/list_voice.pl?owner_id=" .. author_id;
	key = "MIXI_RECENT_ECHO";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- 引用ユーザのボイス追加メニュー用ハンドラ
function on_mixi_echo_add_ref_user_echo_list(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('ref_user_name');
	
	-- カテゴリ追加
	title = name .. "さんのボイス";
	author_id = body:get_integer('ref_user_id');
	url = "http://mixi.jp/list_voice.pl?id=" .. author_id;
	key = "MIXI_RECENT_ECHO";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="MIXI_RECENT_VOICE_ITEM" then
		return false;
	end
	
	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.mixi_echo_item_read);
	menu:append_menu("separator");
	menu:append_menu("string", "つぶやく", menu_items.mixi_echo_update);
--	menu:append_menu("string", "返信", menu_items.mixi_echo_reply);
	menu:append_menu("string", body:get_text('name') .. " さんのプロフィール", menu_items.mixi_echo_show_profile);
	menu:append_menu("separator");

	-- TODO 各メニューアイテムのリソース値を定数化(またはLua関数化)

	-- ユーザのエコー一覧
	menu:append_menu("string", body:get_text('name') .. " さんのボイス", menu_items.mixi_echo_add_user_echo_list);

	-- 引用ユーザのエコー一覧
	ref_user_name = body:get_text('ref_user_name');
	if ref_user_name ~= "" then
		menu:append_menu("string", ref_user_name .. " さんのボイス", menu_items.mixi_echo_add_ref_user_echo_list);
	end

	-- コメント
	extra_count   = body:get_text('extra_count')
--	extra_comment = body:get_text('extra_comment');
	extra_url = body:get_text('extra_url');
	if extra_url ~= "" then
		menu:append_menu("separator");
		menu:append_menu("string", "link : もっとコメントを読む " .. extra_count .. "件", menu_items.mixi_open_browser);
	end

	-- リンク追加
	n = body:get_link_list_size();
	if n > 0 then
		menu:append_menu("separator");
		for i=0, n-1 do
			id = ID_REPORT_URL_BASE+(i+1);
			menu:append_menu("string", "link : " .. body:get_link_list_text(i), id);
		end
	end
	
	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu", "mixi.on_popup_body_menu");


--- ブラウザで開く
function on_mixi_open_browser(serialize_key, event_name, data)

--[[
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	mz3.open_url_by_browser_with_confirm(body:get_text('extra_url'));
]]

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = name .. "の発言に対するコメント";
	url = body:get_text('extra_url');
--	key = "MIXI_RECENT_ECHO";
	key = "MIXI_RECENT_VOICE_DETAIL";
	mz3_main_view.append_category(title, url, key);

	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);


end


--- デフォルトのグループリスト生成イベントハンドラ
--
-- @param serialize_key シリアライズキー(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then

		-- 各種タブ追加

		-- 日記
		local tab = MZ3GroupItem:create("日記");
		tab:append_category("最近の日記", "MYDIARY");
		tab:append_category("最近のコメント", "COMMENT");
		tab:append_category("マイミク最新日記", "DIARY");
		tab:append_category("日記コメント記入履歴", "NEW_COMMENT");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- コミュニティ
		local tab = MZ3GroupItem:create("コミュニティ");
		tab:append_category("最新書き込み一覧", "BBS");
		tab:append_category("コミュコメント履歴", "NEW_BBS_COMMENT");
		tab:append_category("コミュニティ一覧", "COMMUNITY");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- ニュース
		local tab = MZ3GroupItem:create("ニュース");
		tab:append_category("注目のピックアップ",	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn");
		tab:append_category("国内",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1");
		tab:append_category("政治",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=2&type=bn&sort=1");
		tab:append_category("経済",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=3&type=bn&sort=1");
		tab:append_category("地域",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=4&type=bn&sort=1");
		tab:append_category("海外",				  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=5&type=bn&sort=1");
		tab:append_category("スポーツ",			  	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=6&type=bn&sort=1");
		tab:append_category("エンターテインメント",	"NEWS", "http://news.mixi.jp/list_news_category.pl?id=7&type=bn&sort=1");
		tab:append_category("IT",					"NEWS", "http://news.mixi.jp/list_news_category.pl?id=8&type=bn&sort=1");
		tab:append_category("ゲーム・アニメ",		"NEWS", "http://news.mixi.jp/list_news_category.pl?id=9&type=bn&sort=1");
		tab:append_category("コラム",				"NEWS", "http://news.mixi.jp/list_news_category.pl?id=10&type=bn&sort=1");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- メッセージ
		local tab = MZ3GroupItem:create("メッセージ");
		-- カテゴリは各パーサが追加する
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- echo
		local tab = MZ3GroupItem:create("ボイス");
		tab:append_category("みんなのボイス", "MIXI_RECENT_ECHO", "recent_voice.pl");
		tab:append_category("自分の一覧", "MIXI_RECENT_ECHO", "http://mixi.jp/list_voice.pl?id={owner_id}");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- その他
		local tab = MZ3GroupItem:create("その他");
		tab:append_category("マイミク一覧", "FRIEND");
		tab:append_category("紹介文", "INTRO");
		tab:append_category("足あと", "FOOTSTEP");
		tab:append_category("カレンダー", "CALENDAR", "show_schedule.pl");
		tab:append_category("ブックマーク", "BOOKMARK");
		tab:append_category("お気に入りユーザー", "FAVORITE", "view_mylist.pl");
		tab:append_category("お気に入りコミュ", "FAVORITE_COMMUNITY", "list_bookmark.pl?kind=community");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group", false);


--- estimate 対象判別イベントハンドラ
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        解析対象URL
--
function on_estimate_access_type(event_name, url, data1, data2)

    -- 関連ニュース
	if line_has_strings(url, 'http://news.mixi.jp/list_quote_diary.pl?') then
		return true, mz3.get_access_type_by_key('MIXI_NEWS_QUOTE_DIARY');
	end

    -- ニュース一覧
	if line_has_strings(url, 'http://news.mixi.jp/list_news_category.pl') then
		return true, mz3.get_access_type_by_key('NEWS');
	end

	-- マイミク一覧
	if line_has_strings(url, 'list_friend_simple.pl') then
		return true, mz3.get_access_type_by_key('FRIEND');
	end

    -- お気に入りユーザ・コミュニティ
	if line_has_strings(url, 'view_mylist.pl') then
		return true, mz3.get_access_type_by_key('FAVORITE');
	end
	if line_has_strings(url, 'http://mixi.jp/list_bookmark.pl', 'kind=community') then
		return true, mz3.get_access_type_by_key('FAVORITE_COMMUNITY');
	end

	-- イベント参加者一覧
	if line_has_strings(url, 'mixi.jp', 'list_event_member.pl') then
		return true, mz3.get_access_type_by_key('MIXI_EVENT_MEMBER');
	end

	-- コミュニティ一覧
	if line_has_strings(url, 'list_community.pl') then
		return true, mz3.get_access_type_by_key('COMMUNITY');
	end

    -- ボイス
	if line_has_strings(url, 'recent_voice.pl?') then
		return true, mz3.get_access_type_by_key('MIXI_RECENT_ECHO');
	end

	return false;
end
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type");


--- レポート画面からの書き込み種別の判定
--
-- @param event_name  'get_write_view_type_by_report_item_access_type'
-- @param report_item [MZ3Data] レポート画面の要素
--
function on_get_write_view_type_by_report_item_access_type(event_name, report_item)

	report_item = MZ3Data:create(report_item);
	
	serialize_key = report_item:get_serialize_key();
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='mixi' then
		if serialize_key=='MIXI_MESSAGE' then
			-- メッセージ返信の書き込み種別
			return true, mz3.get_access_type_by_key('MIXI_POST_REPLYMESSAGE_ENTRY');
		else
			-- コメント返信の書き込み種別
			return true, mz3.get_access_type_by_key('MIXI_POST_COMMENT_CONFIRM');
		end
	end

	return false;
end
mz3.add_event_listener("get_write_view_type_by_report_item_access_type", "mixi.on_get_write_view_type_by_report_item_access_type");


--- 書き込み画面で「画像が添付可能なモードか」の判定
--
-- @param event_name      'is_enable_write_view_attach_image_mode'
-- @param write_view_type 書き込み種別
-- @param write_item      [MZ3Data] 書き込み画面の要素
--
function on_is_enable_write_view_attach_image_mode(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type=='mixi' then
		if write_view_key=="MIXI_POST_REPLYMESSAGE_ENTRY" or
		   write_view_key=="MIXI_POST_NEWMESSAGE_ENTRY" then
			-- メッセージ、メッセージ返信は添付不可
			return true, 0;
		end
		
		if write_view_key=="MIXI_POST_COMMENT_CONFIRM" then
			local serialize_key = write_item:get_serialize_key();
			if serialize_key=="MIXI_BBS" or
			   serialize_key=="MIXI_EVENT" or
			   serialize_key=="MIXI_EVENT_JOIN" then
				-- コメントの BBS, EVENT であれば添付可
				return true, 1;
			else
				-- 上記以外(日記コメント、アンケートコメント等)は添付不可
				return true, 0;
			end
		end
		
		if write_view_key=="MIXI_POST_NEWDIARY_CONFIRM" then
			-- 日記は添付可
			return true, 1;
		end
		
		return false;
	end

	return false;
end
mz3.add_event_listener("is_enable_write_view_attach_image_mode", "mixi.on_is_enable_write_view_attach_image_mode");


--- 更新ボタン押下イベント
--
-- @param event_name    'click_update_button'
-- @param serialize_key Twitter風書き込みモードのシリアライズキー
--
function on_click_update_button(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="mixi" then
		return false;
	end

	-- 入力文字列を取得
	text = mz3_main_view.get_edit_text();

	-- 未入力時の処理
	if text == '' then
		if serialize_key == 'MIXI_ADD_VOICE_REPLY' then
			-- 未入力はNG => 何もせずに終了
			return true;
		elseif serialize_key == 'MIXI_ADD_VOICE' then
			-- 最新取得
			mz3_main_view.retrieve_category_item();
			return true;
		else
			-- 上記以外はNG。
			mz3.alert('未サポートのアクセス種別です');
			return true;
		end
	end

	-- 確認
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'MIXI_ADD_VOICE' then
		msg = 'mixi エコーで発言します。 \n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		local username = data:get_text('name');
		msg = 'mixi エコーで ' .. username .. ' さんに返信します。 \n'
		   .. '---- 発言 ----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	if serialize_key == 'MIXI_ADD_VOICE' then
		-- 単純投稿は共通処理で。

		-- クロスポスト管理データ初期化
		mz3.init_cross_post_info("echo");

		do_post_to_echo(text);
		return true;
	end
	
	-- POST パラメータを設定
	post = MZ3PostData:create();
	local post_key = mixi.post_key;
	if post_key=='' then
		mz3.alert('送信用のキーが見つかりません。エコー一覧をリロードして下さい。');
		return true;
	end
	if serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		-- body=test&x=36&y=12&parent_member_id=xxx&parent_post_time=20090626110655&redirect=recent_echo&post_key=xxx
		local echo_member_id = data:get_integer('author_id');
		local echo_post_time = data:get_text('echo_post_time');

		mz3.logger_debug("text：" .. mz3.url_encode(text, 'euc-jp'));
		mz3.logger_debug("parent_member_id：" .. echo_member_id);
		mz3.logger_debug("parent_post_time：" .. echo_post_time);

		if echo_member_id == -1 then
			mz3.alert('返信先ユーザが不明です');
			return true;
		end
		if echo_post_time == '' then
			mz3.alert('返信対象POSTの時刻が不明です');
			return true;
		end
		
		post:append_post_body('body=');
		post:append_post_body(mz3.url_encode(text, 'euc-jp'));
		post:append_post_body('&x=28&y=20');
 		post:append_post_body('&parent_member_id=' .. echo_member_id);
 		post:append_post_body('&parent_post_time=' .. echo_post_time);
		post:append_post_body('&redirect=recent_voice');
		post:append_post_body('&post_key=');
		post:append_post_body(post_key);
	end
	
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST先URL設定
	if serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		url = 'http://mixi.jp/add_voice.pl';
		-- url = 'http://mixi.jp/recent_voice.pl?from=home=gadget';
	end
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end
mz3.add_event_listener("click_update_button", "mixi.on_click_update_button");


--- echo に投稿する
function do_post_to_echo(text)

	serialize_key = 'MIXI_ADD_VOICE'

	post = MZ3PostData:create();
	local post_key = mixi.post_key;
	if post_key=='' then
		mz3.alert('送信用のキーが見つかりません。エコー一覧をリロードして下さい。');
		return true;
	end
	post:append_post_body('body=');
	post:append_post_body(mz3.url_encode(text, 'euc-jp'));
	post:append_post_body('&x=28&y=20');
	post:append_post_body('&post_key=');
	post:append_post_body(post_key);
	post:append_post_body('&redirect=recent_voice');

	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST先URL設定
	url = 'http://mixi.jp/add_voice.pl';
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end


--- POST 完了イベント
--
-- @param event_name    'post_end'
-- @param serialize_key 完了項目のシリアライズキー
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param filename      レスポンスファイル
-- @param wnd           wnd
--
function on_post_end(event_name, serialize_key, http_status, filename)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="mixi" then
		return false;
	end

	-- 投稿処理完了
	
	if serialize_key == 'MIXI_ADD_VOICE' or 
	   serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		if mz3.is_mixi_logout(serialize_key) then
			mz3.alert('未ログインです。エコー一覧をリロードし、mixiにログインして下さい。');
			return true;
		else
			-- 投稿成功
			mz3_main_view.set_info_text("エコー書き込み完了");

			-- クロスポスト
			if serialize_key == "MIXI_ADD_VOICE" then
				if mz3.do_cross_post() then
					return true;
				end
			end

			-- 入力値を消去
			mz3_main_view.set_edit_text("");

			return true;
		end
	end
end
mz3.add_event_listener("post_end", "mixi.on_post_end");


--- カテゴリ取得時のハンドラ
--
-- @param event_name    'retrieve_category_item'
-- @param serialize_key カテゴリアイテムのシリアライズキー
--
function on_retrieve_category_item(event_name, serialize_key, body, wnd)
	if serialize_key~="BOOKMARK" then
		return false;
	end
	
	-- ブックマークはローカルストレージ
	return true, RETRIEVE_CATEGORY_ITEM_RVAL_LOCALSTORAGE;
end
mz3.add_event_listener("retrieve_category_item", "mixi.on_retrieve_category_item");


--- ボディリストのアイコンのインデックス取得
--
-- @param event_name    'creating_default_group'
-- @param serialize_key シリアライズキー(nil)
-- @param body          body data
--
-- @return (1) [bool] 成功時は true, 続行時は false
-- @return (2) [int] アイコンインデックス
--
function on_get_body_list_default_icon_index(event_name, serialize_key, body)

	if serialize_key == "MIXI_BBS"        then return true, 0; end
	if serialize_key == "MIXI_EVENT"      then return true, 1; end
	if serialize_key == "MIXI_ENQUETE"    then return true, 2; end
	if serialize_key == "MIXI_EVENT_JOIN" then return true, 3; end
	if serialize_key == "MIXI_BIRTHDAY"   then return true, 4; end
	if serialize_key == "MIXI_SCHEDULE"   then return true, 5; end
	if serialize_key == "MIXI_MESSAGE"    then return true, 7; end
	if serialize_key == "RSS_ITEM"        then return true, 8; end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "mixi.on_get_body_list_default_icon_index");


--------------------------------------------------
-- 【マイミク一覧】
-- [list] list_friend_simple.pl 用パーサ
--
-- http://mixi.jp/list_friend_simple.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_mymixi_parser(parent, body, html)
	mz3.logger_debug("mixi_mymixi_parser start");

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
	for i=300, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			if line_has_strings( line, "list_friend_simple.pl" ) then
				back_data, next_data = parse_next_back_link(line, "list_friend_simple.pl", "name");
			end
		end

		-- 項目探索 以下一行
		-- <div class="wrapper">
		if line_has_strings(line, "<div", "class", "wrapper") then
		
			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			i = i+1;
			line = html:get_at(i);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			if url ~= nil then
				url = complement_mixi_url(url);
				data:set_text("url", url);
			end

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- 画像取得
			local image_url = line:match('url[(](.-)[)]');
			--mz3.logger_debug('image url:' .. image_url);
			if image_url ~= nil then
				data:add_text_array("image", image_url);
			end

			i = i+1;
			line = html:get_at(i);

			-- 名前取得
			name = line:match(">([^<]+)(<.*)$");
			data:set_text("name", name);
			
			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();
		end

		if in_data_region and line_has_strings(line, "</tbody>") then
			mz3.logger_debug("★</tbody>が見つかったので終了します");
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
	mz3.logger_debug("mixi_mymixi_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("FRIEND", "mixi.mixi_mymixi_parser");


--------------------------------------------------
-- 【ニュースのカテゴリ】
-- [list] list_news_category.pl 用パーサ
--
-- http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_news_list_parser(parent, body, html)
	mz3.logger_debug("mixi_news_list_parser start");

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
		
		if line_has_strings(line, '<div class="newsList">') then
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
			back_data, next_data = parse_next_back_link_for_news(line, "list_news_category.pl");
		else
			break;
		end
	end
	
	-- 範囲を一括取得
	sub_html = get_sub_html(html, i_start_line, line_count, {'<table'}, {'</table>'});
	
--	mz3.logger_debug('html:' .. sub_html);
	
	-- 各記事を取得
	local element_html = '';
	for element_html in sub_html:gmatch('<tr.->(.-)</tr>') do
--		mz3.logger_debug('element_html: ' .. element_html);
		
		-- data 生成
		data = MZ3Data:create();

		element_html = element_html:gsub('&nbsp;', ' ');

		-- 時刻
		-- <dd>2012年07月20日14:03</dd>
		local date = element_html:match('<td class="date">(.-)</td>');
		if date ~= nil then
			data:parse_date_line(date);
		end

--[[
<tr class="odd">
<td class="newsTitle"><p>・ <a href="view_news.pl?id=2109273&media_id=20">火星の生命探し 探査車着陸へ</a></p></td>
<td class="media"><a href="list_news_media.pl?id=20">xx新聞</a></td>
<td class="date">08月06日 12:21</td>
</tr>
]]
		-- 見出し、URL、名前の抽出
		url, title = element_html:match('<td class="newsTitle">.-href="(.-)">(.-)</a>');
		title = mz3.decode_html_entity(title);
		data:set_text("title", title);

		-- URL 取得
		url = 'http://news.mixi.jp/' .. url;
		data:set_text("url", url);
		
		-- ID 設定
		id = get_param_from_url(url, "id");
		data:set_integer('id', id);

		-- 名前
		name = element_html:match('<td class="media">.-href=".-">(.-)</a>');
		data:set_text("name", name);
		data:set_text("author", name);

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
	mz3.logger_debug("mixi_news_list_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("NEWS", "mixi.mixi_news_list_parser");

function parse_next_back_link_for_news(line, base_url, title_set_at)

	local back_data = nil;
	local next_data = nil;
	if title_set_at == nil then
		title_set_at = "title";
	end
	
	-- <ul><li><a href="new_bbs.pl?page=1">前を表示</a></li>
	-- <li>51件～100件を表示</li>
	-- <li><a href="new_bbs.pl?page=3">次を表示</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- 前
		local url, t = line:match([[href="([^"]+)">(前[^<]+)<]]);
		if url~=nil then
			-- URL補完
			url = 'http://news.mixi.jp/' .. url;
			back_data = MZ3Data:create();
			back_data:set_text(title_set_at, "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- 次
		local url, t = line:match([[href="([^"]+)">(次[^<]+)<]]);
		if url~=nil then
			-- URL補完
			url = 'http://news.mixi.jp/' .. url;
			next_data = MZ3Data:create();
			next_data:set_text(title_set_at, "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end


--------------------------------------------------
-- 【ニュース記事】
-- [content] view_news.pl 用パーサ
--
-- http://mixi.jp/view_news.pl
--
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_news_parser(parent, body, html)
	mz3.logger_debug("mixi_view_news_parser start");

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);
	
	parent:clear();
	
	parent:set_text('name', '');

	local t1 = mz3.get_tick_count();
	local line_count = html:get_count();

	-- title タグからタイトル抽出
	local title = '';
	for i=0, 10 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<title>') then
			title = line:match('<title>%[mixi%] *(.-) *</title>');
			title = mz3.decode_html_entity(title);
			parent:set_text('title', title);
			break;
		end
	end
	
	-- 空行
	parent:add_body_with_extract("<br>");
	
	-- 本文
	local i=200;
	local sub_html = '';
	sub_html = get_sub_html(html, i, line_count, {'<div class="article">'}, {'<div class="bottomContents clearfix">'});
--	mz3.logger_debug(sub_html);
	-- 不要タグの削除
--[[
<a name="post-check"></a>
<p class="checkButton">
<a check_key="xx" check_button="button-5.gif">チェック</a>
<script data-prefix-uri="http://mixi.jp/" 
        src="http://static.mixi.jp/js/share.js" 
        type="text/javascript"></script>
</p>
]]
	sub_html = sub_html:gsub('<a name="post%-check">.-</p>', '');
	
	-- <ul>, <li> タグの整形
	sub_html = sub_html:gsub('<ul>', '<br>');
	sub_html = sub_html:gsub('<li>(.-)</li>', '%1<br>');
	sub_html = sub_html:gsub('</ul>', '');
	
	-- <dl>, <dd> タグの整形
	sub_html = sub_html:gsub('<dl>', '<br>');
	sub_html = sub_html:gsub('<dd>(.-)</dd>', '%1<br>');
	sub_html = sub_html:gsub('</dl>', '');
	
	-- その他の謎のタグ除去
	sub_html = sub_html:gsub('<p .->', '');
	sub_html = sub_html:gsub('<script.->', '');
	sub_html = sub_html:gsub('</script>', '');
	
	parent:add_body_with_extract(sub_html);

	-- 関連日記
	-- http://news.mixi.jp/view_news.pl?id=2109259&media_id=20&from=home&position=1
	local url = parent:get_text('url');
	local newsId = get_param_from_url(url, "id");
	parent:add_link_list('http://news.mixi.jp/list_quote_diary.pl?id=' .. newsId, '関連日記');
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_news_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_NEWS", "mixi.mixi_view_news_parser");


--------------------------------------------------
-- 【紹介文】
-- [list] show_intro.pl 用パーサ
--
-- http://mixi.jp/show_intro.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_intro_list_parser(parent, body, html)
	mz3.logger_debug("mixi_intro_list_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();
	
	-- 行数取得
	local line_count = html:get_count();
	
	-- 開始フラグの探索
	local i_start_line = 100;
	while (i_start_line < line_count) do
		line = html:get_at(i_start_line);
		
		if line_has_strings(line, '<div id="bodyArea">') then
			break;
		end
		
		i_start_line = i_start_line + 1;
	end

	-- 範囲を一括取得
	sub_html = get_sub_html(html, i_start_line, line_count, {'<ul class="introListContents">'}, {'<div id="bodySub">'});
	
--	mz3.logger_debug('html:' .. sub_html);
	
	-- 各記事を取得
	local element_html = '';
	for element_html in sub_html:gmatch('(<dl.-</dl>)') do
--		mz3.logger_debug('element_html: ' .. element_html);
		
		-- data 生成
		data = MZ3Data:create();

		element_html = element_html:gsub('&nbsp;', ' ');

		-- 時刻
		data:set_date('');

		-- URL、名前の抽出
		-- <span class="name"><a href="show_friend.pl?id=xx">xx</a></span>
		url, name = element_html:match('<span class="name">.-href="(.-)">(.-)</a>');
		name = mz3.decode_html_entity(name);
		data:set_text("name", name);
		data:set_text("url", url);
		
		-- <img src="xx"
		local image_url = element_html:match('<img src="(.-)"');
		if image_url ~= nil then
			data:add_text_array("image", image_url);
		end
		
		-- ID 設定
		id = get_param_from_url(url, "id");
		data:set_integer('id', id);

		-- 紹介文
		local relation = element_html:match('<p class="relation">(.-)</p>');
		local userInput = element_html:match('<p class="userInput">(.-)</p>');
		data:add_body_with_extract(relation .. '  ' .. userInput);

		-- URL に応じてアクセス種別を設定
		type = mz3.estimate_access_type_by_url(url);
		data:set_access_type(type);

		-- data 追加
		body:add(data.data);

		-- data 削除
		data:delete();
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_intro_list_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("INTRO", "mixi.mixi_intro_list_parser");



----------------------------------------
-- パーサのロード＆登録
----------------------------------------

-- トップページ
require("scripts\\mixi\\mixi_home_parser");

-- コミュニティ最新書込一覧
require("scripts\\mixi\\mixi_new_bbs_parser");

-- マイミク最新日記一覧、日記詳細
require("scripts\\mixi\\mixi_view_diary_parser");

-- メッセージ(受信箱, 送信箱), 公式メッセージ, メッセージ詳細
--require("scripts\\mixi\\mixi_new_official_message_parser");	-- inbox_parser に統合
--require("scripts\\mixi\\mixi_message_outbox_parser");			-- inbox_parser に統合
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- 逆あしあと
require("scripts\\mixi\\mixi_show_self_log_parser");

-- お気に入りコミュ、ユーザ
require("scripts\\mixi\\mixi_bookmark_community_parser");
require("scripts\\mixi\\mixi_bookmark_user_parser");

-- ニュース関連日記
require("scripts\\mixi\\mixi_news_quote_diary_parser");

mz3.logger_debug('mixi.lua end');
