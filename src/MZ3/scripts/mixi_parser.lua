--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi_parser.lua start');
module("mixi", package.seeall)


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- 【イベント参加者一覧】
type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='body', service_type='mixi', key='MIXI_EVENT_MEMBER',
  short_title='イベント参加者一覧', request_method='GET', request_encoding='euc-jp'
});

-- 【ボイス詳細】
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='body', service_type='mixi', key='MIXI_RECENT_VOICE_DETAIL',
  short_title='ボイス', request_method='GET', request_encoding='euc-jp'
});
-- view_voice.pl?post_time=xx&owner_id=xx
type:set_cache_file_pattern('mixi\\mixi_voice_detail_{urlparam:post_time}_{urlparam:owner_id}.html');
type:set_body_header(1, 'title', 'コメント');
type:set_body_header(2, 'name', '名前');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');

-- 【ニュース一覧】
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='mixi', key='NEWS',
  short_title='ニュース一覧', request_method='GET', request_encoding='euc-jp'
});
type:set_cache_file_pattern('mixi\\list_news_category_{urlparam:id}_{urlparam:page}.html');
-- url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
-- url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
-- url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
-- url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn
type:set_body_header(1, 'title', '見出し');
type:set_body_header(2, 'date', '配信時刻>>');
type:set_body_header(3, 'name', '配給元>>');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2 (%3)');

-- 【紹介文一覧】
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='mixi', key='INTRO',
  short_title='紹介文', request_method='GET', request_encoding='euc-jp'
});
-- url(1) : http://mixi.jp/show_intro.pl
-- url(2) : http://mixi.jp/show_intro.pl?page=2
type:set_cache_file_pattern('mixi\\show_intro{urlparam:page}.html');
type:set_default_url('http://mixi.jp/show_intro.pl');
type:set_body_header(1, 'name', '名前');
type:set_body_header(2, 'body', '紹介文');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');


-- TODO 本来は Lua 側でやるべき↓
--[[
-- 【お気に入りコミュ一覧】
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('FAVORITE_COMMUNITY');
type:set_short_title('お気に入りコミュ');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\bookmark_community.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/list_bookmark.pl?kind=community');
type:set_body_header(1, 'name', 'コミュニティ');
type:set_body_header(2, 'title', '説明');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]


--[[
-- 【お気に入りユーザー一覧】
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('FAVORITE');
type:set_short_title('お気に入りユーザー');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\bookmark_user.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/view_mylist.pl');
type:set_body_header(1, 'name', 'ユーザー名');
type:set_body_header(2, 'title', '自己紹介');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
]]


-- 【メッセージ受信箱】
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('MESSAGE_IN');
type:set_short_title('メッセージ(受信箱)');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\list_message_inbox.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/list_message.pl');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);

-- 【メッセージ送信箱】
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('MESSAGE_OUT');
type:set_short_title('メッセージ(送信箱)');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\list_message_outbox.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/list_message.pl?box=outbox');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);

-- 【公式メッセージ】
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('MIXI_LIST_MESSAGE_OFFICIAL');
type:set_short_title('公式メッセージ');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\list_message_official.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/list_message.pl?box=noticebox');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);


-- 【mixi メッセージ詳細】
-- TODO ホスト側で設定しているが、本来はこちらで設定すべき。
--      ただし、
--      inbox, outbox パーサよりも先に登録しなければならない、
--      estimate 対象にしなければならない、などの課題アリ。


-- 【mixi ニュース関連日記】
type = MZ3AccessTypeInfo:create();
type:set_info_type('body');
type:set_service_type('mixi');
type:set_serialize_key('MIXI_NEWS_QUOTE_DIARY');
type:set_short_title('ニュース関連日記');
type:set_request_method('GET');
type:set_cache_file_pattern('news\\list_quote_diary_{urlparam:id}.html');
type:set_request_encoding('euc-jp');
-- url(1) : http://news.mixi.jp/list_quote_diary.pl?id=839733

-- 【逆あしあと一覧】
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');
type:set_service_type('mixi');
type:set_serialize_key('MIXI_SHOW_SELF_LOG');
type:set_short_title('逆あしあと');
type:set_request_method('GET');
type:set_cache_file_pattern('mixi\\show_self_log.html');
type:set_request_encoding('euc-jp');
type:set_default_url('http://mixi.jp/show_self_log.pl');
type:set_body_header(1, 'title', '名前');
type:set_body_header(2, 'date', '日付');
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
-- 【みんなのボイス一覧】
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

	local back_data = nil;
	local next_data = nil;

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
--				mz3.logger_debug(image_url);
				data:add_text_array("image", image_url);
			end
			-- 画像を表示フラグの設定
			data:set_integer('show_image', 1);

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
				comment = comment:gsub('<a.->.-</a>', '');
				data:add_body_with_extract(comment .. "\r\n");
			end

			-- id
			-- <span class="thumb"><a href="show_friend.pl?id=xx">
			local author_id = li_tag:match('href="show_friend.pl%?id=([0-9]+)">');
			if author_id ~= nil then
				data:set_integer("author_id", author_id);
				data:set_integer("id", author_id);
			end

			-- 時間
			local alt_url, date = voiced:match('<span>.-<a href="(.-)".->(.-)</a>');
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

			-- ブラウザで開く
			if alt_url ~= nil then
				alt_url = complement_mixi_url(alt_url);
				data:add_link_list(alt_url, 'ブラウザで開く');
				
--				mz3.logger_debug('★URL:' .. alt_url);
				data:set_text("url", alt_url);
			end

			-- コメント
			local more_comment = li_tag:match('<p class="moreLink01.->(.-)</p');
			local comment_count = nil;
			if more_comment ~= nil then
				local url = more_comment:match('href="(.-)"');
				url = complement_mixi_url(url);
				comment_count = more_comment:match("commentCount\">(.-)</span");
				if comment_count ~= nil then
					data:set_text("extra_url", url);
					data:set_text("extra_count", comment_count);
				end
			end

			-- コメント本文取得(最大2件)
			local extra_comment_all = '';
			local comment_count_on_extra = 0;
			for dd_commentRow in li_tag:gmatch('<dd class="commentRow hrule">(.-)</dd') do
				local commentName = dd_commentRow:match('class="commentNickname">(.-)<');
				local comment1 = dd_commentRow:match('<p class="commentBody">(.-)<');
				comment1 = "\r\n" .. "　　コメント by " .. commentName ..  "： " .. comment1 .. "";

				extra_comment_all = extra_comment_all .. comment1;
				comment_count_on_extra = comment_count_on_extra + 1;
			end
			if extra_comment_all ~= '' then
				data:set_text('extra_comment', extra_comment_all);
			end

			-- コメント数表示がなく、コメント本文が1件以上あれば
			if comment_count == nil and comment_count_on_extra ~= nil then
				comment_count = comment_count_on_extra;
			end

			-- コメント数を date に詰め込む
			if comment_count ~= nil and date ~= nil then
				if comment_count ~= 0 then
					data:set_date(date .. ' コメント(' .. comment_count .. ')');
				end
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
			local type = mz3.get_access_type_by_key('MIXI_RECENT_VOICE_DETAIL');
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
-- [content] view_voice.pl 用パーサ
--
-- http://mixi.jp/view_voice.pl?post_time=xx&owner_id=xx
-- 
-- 引数:
--   parent: 上ペインのオブジェクト群(MZ3Data*)
--   dummy:  NULL
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_recent_voice_detail_parser(data, dummy, html)
	mz3.logger_debug("mixi_recent_voice_detail_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();

	-- とりあえず改行出力
	data:add_body_with_extract("<br>");

	local line_count = html:get_count();
	
	-- 範囲を一括取得
	local sub_html = get_sub_html(html, 100, line_count, {'<div id="bodyMainArea">'}, {'<p class="utilityLinks03">'});
	
	-- 本文等抽出
	local voiceBody = sub_html:match('<div class="voiceBody">(.-)</div>');
	if voiceBody ~= nil then
		-- 本文
		local v = voiceBody:match('<p class="voice">(.-)</p');
		data:add_body_with_extract(v .. "\r\n");
		
		local name = voiceBody:match('<p class="name"><a.->(.-)</a');
		data:set_text('title', name);
		
--		local date = voiceBody:match('<ul class="source">.-<li>(.-)</li');
--		data:set_date(date);
	end
	
	-- コメント一覧
	local comment_list_area_html = sub_html:match('<div class="commentListArea">(.*)');
	if comment_list_area_html ~= nil then
		local comment_html = '';
		local comment_index = 1;
		for comment_html in comment_list_area_html:gmatch('<li class="commentRow">(.-)</dl>') do 
			
--[[
<dt><a href="show_friend.pl?id=590362" class="commentNickname">ドトール＠PSO2船6</a>
<span class="date">2時間前</span>
<span class="operation"></span>
</dt>
<dd>こめんと3</dd>
]]
			local url, name = comment_html:match('<dt><a href="(.-)".->(.-)</a>');
			local date = comment_html:match('<span class="date">(.-)</span>')
			local comment = comment_html:match('<dd>(.-)</dd>')
			
			child = MZ3Data:create();
			child:set_text('author', mz3.decode_html_entity(name));
			child:add_body_with_extract('<br>');
			child:add_body_with_extract(mz3.decode_html_entity(comment));
			-- コメント番号
			child:set_integer('comment_index', comment_index);
			comment_index = comment_index + 1;
			child:set_date(mz3.decode_html_entity(date));
			
			child:add_link_list(complement_mixi_url(url), name .. ' さん');
			
			data:add_child(child);
			
			child:delete();
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
	for i=180, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			if line_has_strings( line, "list_bookmark.pl" ) then
				back_data, next_data = parse_next_back_link(line, "list_bookmark.pl", "name");
			end
		end

		-- 項目探索 以下一行
		-- <div class="listIcon">
		if line_has_strings(line, "<div", "class", "listIcon") then

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

			i = i+1;
			line = html:get_at(i);

			-- 画像取得
			image_url = line:match("src=\"([^\"]+)\"");
			data:add_text_array("image", image_url);

			i = i+5;
			line = html:get_at(i);

			-- コミュニティ名
			name = line:match(">([^<]+)(<.*)$");

			if name ~= "" and name ~= nil then
				add_name = line:match(">([^<]+)(&nbsp;.*)$");
				data:set_text("name", add_name);

				-- 参加者数
				users = name:match("([0-9]+.人)");
				data:set_date(users);

				i = i+8;
				line = html:get_at(i);

				-- 説明
				-- description, after = line:match(">([^<]+)(<.*)$");
				description = line:gsub( "\t", "" );
				data:add_body_with_extract(description);

				if description == '' or description == deleted_community then
					-- 削除済みコミュは一覧に出さない
				else

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
	mz3.logger_debug("mixi_bookmark_community_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("FAVORITE_COMMUNITY", "mixi.mixi_bookmark_community_parser");


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
mz3.set_parser("FAVORITE", "mixi.mixi_bookmark_user_parser");


--------------------------------------------------
-- 【mixiトップページ】
-- [content] home.pl ログイン後のメイン画面用パーサ
--
-- http://mixi.jp/home.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--
-- parent の下記の変数に情報を格納する。
-- <ul>
--  <li>[string] "owner_id"       => 自分のID
--  <li>[int] "new_message_count" => 新着メッセージ数
--  <li>[int] "new_comment_count" => 新着コメント数
--  <li>[int] "new_apply_count"   => 承認待ち数
-- </ul>
--------------------------------------------------
function mixi_home_parser(parent, body, html)
	mz3.logger_debug("mixi_home_parser start");

	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);

	-- 結果の初期化
	parent:set_integer("new_message_count", 0);
	parent:set_integer("new_comment_count", 0);
	parent:set_integer("new_apply_count", 0);
	mz3.logger_debug("original owner_id : [" .. parent:get_text("owner_id") .. "]");
	
	-- 新着メッセージ、コメント、承認待ち件数の取得
	local line_count = html:get_count();
	local in_mixi_info_div = false;
	for i=350, line_count-1 do
		line = html:get_at(i);
		-- <div id="mixiInfo">
		-- ...
		-- <ul>
		-- ...
		-- <li class="redTxt"><a href="list_request.pl"><b>マイミクシィ追加リクエストが2件あります！</b></a></li>
		-- <li class="redTxt"><a href="list_message.pl">新着メッセージが2件あります！</a></li>
		-- <li class="redTxt"><a href="view_diary.pl?id=xxx&owner_id=xxx">1件の日記に対して新着コメントがあります！</a></li>
		-- ...
		-- </ul>
		-- を検索対象とする

		if not in_mixi_info_div then
			if line_has_strings(line, '<div', 'id=', 'mixiInfo') then
				in_mixi_info_div = true;
			end
		else
			if line_has_strings(line, '</ul>') then
				break;
			else
				-- li タグの中を検索する
				local li_text = line:match('<li[^>]*>(.*)</li>');
				if li_text ~= nil then
--					mz3.logger_debug(li_text);

					local n = 0;
					n = li_text:match('マイミクシィ追加リクエストが(.*)件あります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_apply_count', n);
					end
					
					n = li_text:match('[ジ]が(.*)件あります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_message_count', n);
					end
					
					n = li_text:match('([0-9]*)件の日記に対して新着コメントがあります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_comment_count', n);
					end
				end
			end
		end
	end
	
	
	-- owner_id の取得
--	parent:set_text('owner_id', '');	-- デバッグ用
	if parent:get_text('owner_id')=="" then
		mz3.logger_debug('owner_id が未取得なので解析します');
		
		for i=0, line_count-1 do
			line = html:get_at(i);
			if line_has_strings(line, "<a", "href=", "add_diary.pl") then
--				mz3.logger_debug("[" .. line .. "]");
				local url = line:match("href=\"([^\"]+)\"");
--				mz3.logger_debug("[" .. url .. "]");
				local id = get_param_from_url(url, 'id');
				if id=="" then
					mz3.logger_error("add_diary.pl の引数に id パラメータが見つかりません。line[" .. line .. "]");
				else
					mz3.logger_debug("owner_id[" .. id .. "]");
					parent:set_text('owner_id', id);
				end
				break;
			end
		end
		
		if parent:get_text('owner_id')=="" then
			mz3.logger_error('owner_id を取得できませんでした');
		end
	end
	
	mz3.logger_debug("parsed owner_id : [" .. parent:get_text("owner_id") .. "]");

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_home_parser end; elapsed : " .. (t2-t1) .. "[msec]");

	return true;
end
mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");


--------------------------------------------------
-- 【mixi メッセージ(受信箱), (送信箱)】
-- [list] list_message.pl 用パーサ
-- [list] list_message.pl?box=outbox 用パーサ
--
-- http://mixi.jp/list_message.pl
-- [list] list_message.pl?box=outbox 用パーサ
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_list_message_parser(parent, body, html)
	mz3.logger_debug("mixi_list_message_parser start");

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
			back_data, next_data = parse_next_back_link(line, "list_message.pl");
		end

		-- 項目探索
		if line_has_strings(line, "td", "class", "subject") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			--	件名
			--	<td class="subject">
			--		<a href="view_message.pl?id=*****&box=inbox&page=1">
			--			件名
			--		</a>
			--	</td>
			-- 件名
			title, after = line:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
			data:set_text("title", title);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- 1行戻る
			i = i -1;
			line2 = html:get_at(i);

			-- 名前
			name, after = line2:match(">([^<]+)(<.*)$");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);
			data:set_text("author", name);

			-- 2行進む
			i = i +2;
			line3 = html:get_at(i);

			-- 日付取得
			-- <td class="date">03月10日<a id=
			date, after = line3:match(">([^<]+)(<.*)$");
			data:set_date( date );

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

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
	mz3.logger_debug("mixi_list_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MESSAGE_IN", "mixi.mixi_list_message_parser");
mz3.set_parser("MESSAGE_OUT", "mixi.mixi_list_message_parser");


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
		if line_has_strings(line, "<td", "class", "sender")  then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 日付のパース
			-- sender, subject, date と回り，date に日付があるので +2 …
			line3 = html:get_at(i +2);
			date = line3:match(">([^<]+)(<.*)$");
			date = mz3.decode_html_entity(date);
			data:set_date(date);
			
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
			data:set_text("author", sender);
			
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
mz3.set_parser("MIXI_LIST_MESSAGE_OFFICIAL", "mixi.mixi_new_official_message_parser");


--------------------------------------------------
-- 【mixi メッセージ詳細】
-- [content] view_message.pl 用パーサ
--
-- http://mixi.jp/view_message.pl
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_message_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_message_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 行数取得
	local line_count = html:get_count();
	for i=200, line_count-1 do
		line = html:get_at(i);

		-- 日付抽出
		-- <dt>日付</dt>
		-- <dd>2008年11月12日 11時13分</dd>
		if line_has_strings(line, "<dt>日付</dt>") then
			line1 = html:get_at(i+1);
			
			-- 形式が特殊なので直接設定する
			data:set_date(line1:match("<dd>(.*)</dd>"));
		end

		-- 差出人ID抽出
		if line_has_strings(line, "<a", "href=", "show_friend.pl?id=", "\">", "</a>") then
			--mz3.logger_debug(line);
			id = line:match("\?id=([^\"]+)\"");

			--mz3.logger_debug("id : " .. id);
			data:set_integer("owner_id", id);
		end

		-- 本文抽出
		if line_has_strings(line, "<div", "class=", "messageDetailBody" ) then

			data:add_text_array("body", "\r\n");

			-- 整形して追加
			data:add_body_with_extract(line);

			-- </div> が存在すれば終了。
			if line_has_strings(line, "</div") then
				mz3.logger_debug("★</div>が見つかったので終了します(1)");
				break;
			end

			-- 普通のメッセージ
			for j=i+1, line_count-1 do
				line = html:get_at(j);

				-- 整形して追加
				data:add_body_with_extract(line);
				
				if line_has_strings(line, "</div") then
					mz3.logger_debug("★</div>が見つかったので終了します(2)");
					break;
				end
			end

			break;
		end
		
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("MIXI_MESSAGE", "mixi.mixi_view_message_parser");


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
mz3.set_parser("MIXI_NEWS_QUOTE_DIARY", "mixi.mixi_news_quote_diary_parser");


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
	for i=330, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_bbs.pl");
		end

		-- 項目探索
		-- <span class="date">02月16日 09:39</span>
		if line_has_strings(line, "<span", "class", "date") or
		   line_has_strings(line, "<span", "class", "name")  then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 見出し
			-- <span class="date">02月16日 09:39</span><span class="name"><a href="show_friend.pl?id=xxxxx">user_nickname</a>
			-- 退会したユーザの場合
			-- <span class="date">02月16日 09:39</span><span class="name">このユーザーは退会しました</span>

			-- 退会ユーザチェック
			-- if line:find( "このユーザーは退会しました", 1, true ) == nil then
			if line:find( "href=", 1, true ) ~= nil then

				date, after = line:match(">([^<]+)(<.*)$");
				-- 日付のセット…
				data:parse_date_line( date );

				-- URL 取得
				url = line:match("href=\"([^\"]+)\"");
				data:set_text("url", url);

				-- id
				id = get_param_from_url(url, "id");
				data:set_integer("id", id);

				-- ユーザ名
				int_start, int_end = after:find( id, 1, true )
				int_nickname_start = int_end +3;
				int_nickname_end, dummy = after:find( "</a>", 1, true )
				nickname = after:sub( int_nickname_start, int_nickname_end -1 );

				-- マイミクなら名前の後に "(マイミク)" と付与
				mymixi = "\"マイミクシィ\"";
				mymixi_mymixi = "\"マイミクシィのマイミクシィ\"";
				if after:find( "alt=" .. mymixi ) and nickname ~= nil then
					nickname = nickname .. " (マイミク)";
				elseif after:find( "alt=" .. mymixi_mymixi ) and nickname ~= nil then
					nickname = nickname .. " (マイミクのマイミク)";
				end

				nickname = mz3.decode_html_entity(nickname);
				data:set_text("title", nickname);

				-- URL に応じてアクセス種別を設定
				type = mz3.estimate_access_type_by_url(url);
				data:set_access_type(type);

				-- data 追加
				body:add(data.data);
			end

			-- data 削除
			data:delete();

-- 当月内削除可能残数
--[[
		elseif line_has_strings(line, "<div", "class", "logListCenter") then

			i = i +2;
			line2 = html:get_at(i);
			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			-- 月内削除可能件数
			if line2:find( "<strong>", 1, true ) ~= nil then
				count, after = line2:match("<strong>([^<]+)(<.*)$");
			else
				count, after = line2:match("<em>([^<]+)(<.*)$");
			end
			data:set_text("title", "当月内削除可能残数 ： " .. count .. "回");

			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();
]]

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
mz3.set_parser("MIXI_SHOW_SELF_LOG", "mixi.mixi_show_self_log_parser");


----------------------------------------
-- パーサのロード＆登録
----------------------------------------

-- コミュニティ関連パーサ
require("scripts\\mixi_community_parser");

-- 日記関連パーサ
require("scripts\\mixi_diary_parser");

mz3.logger_debug('mixi_parser.lua end');
