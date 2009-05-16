--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : twitter
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('twitter.lua start');
module("twitter", package.seeall)

----------------------------------------
-- アクセス種別の登録
----------------------------------------
--[[
type = mz3.get_access_type_by_key('TWITTER_FRIENDS_TIMELINE');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '<small>%2 \t(%3)</small>');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '<small>%2 \t(%3)</small>');
]]


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.retweet               = mz3_menu.regist_menu("twitter.on_retweet_menu_item");

-- 発言内の @xxx 抽出者のTL(5人まで)
menu_items.show_follower_tl = {}
menu_items.show_follower_tl[1]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_1");
menu_items.show_follower_tl[2]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_2");
menu_items.show_follower_tl[3]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_3");
menu_items.show_follower_tl[4]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_4");
menu_items.show_follower_tl[5]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_5");
follower_names = {}

menu_items.update                = mz3_menu.regist_menu("twitter.on_twitter_update");
menu_items.reply                 = mz3_menu.regist_menu("twitter.on_twitter_reply");
menu_items.new_dm                = mz3_menu.regist_menu("twitter.on_twitter_new_dm");
menu_items.create_favourings     = mz3_menu.regist_menu("twitter.on_twitter_create_favourings");
menu_items.destroy_favourings    = mz3_menu.regist_menu("twitter.on_twitter_destroy_favourings");
menu_items.create_friendships    = mz3_menu.regist_menu("twitter.on_twitter_create_friendships");
menu_items.destroy_friendships   = mz3_menu.regist_menu("twitter.on_twitter_destroy_friendships");
menu_items.show_friend_timeline  = mz3_menu.regist_menu("twitter.on_show_friend_timeline");
menu_items.open_home             = mz3_menu.regist_menu("twitter.on_open_home");
menu_items.open_friend_favorites = mz3_menu.regist_menu("twitter.on_open_friend_favorites");
menu_items.open_friend_site      = mz3_menu.regist_menu("twitter.on_open_friend_site");


----------------------------------------
-- イベントハンドラ
----------------------------------------

-- 「つぶやく」メニュー用ハンドラ
function on_twitter_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「返信」メニュー用ハンドラ
function on_twitter_reply(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- 入力領域にユーザのスクリーン名を追加
	text = mz3_main_view.get_edit_text();
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	-- すでに含まれていれば追加しない
	if text:find("@" .. name, 1, true)~=nil then
		return;
	end
	text = text .. "@" .. name .. " ";
	
	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');

	-- 末尾へ移動
	VK_END = 0x23;
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end

-- 「メッセージ送信」メニュー用ハンドラ
function on_twitter_new_dm(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_DM);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「お気に入り登録」メニュー用ハンドラ
function on_twitter_create_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/create/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「お気に入り削除」メニュー用ハンドラ
function on_twitter_destroy_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/destroy/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「フォローする」メニュー用ハンドラ
function on_twitter_create_friendships(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんをフォローします。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = "http://twitter.com/friendships/create/" .. name .. ".xml";

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「フォローやめる」メニュー用ハンドラ
function on_twitter_destroy_friendships(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんのフォローを解除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = "http://twitter.com/friendships/destroy/" .. name .. ".xml";

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「@xxx のタイムライン」メニュー用ハンドラ
function on_show_friend_timeline(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「@xxx のタイムライン」(フォロワー)メニュー用ハンドラ
function on_show_follower_tl_1(serialize_key, event_name, data)	show_follower_tl(1) end
function on_show_follower_tl_2(serialize_key, event_name, data)	show_follower_tl(2) end
function on_show_follower_tl_3(serialize_key, event_name, data)	show_follower_tl(3) end
function on_show_follower_tl_4(serialize_key, event_name, data)	show_follower_tl(4) end
function on_show_follower_tl_5(serialize_key, event_name, data)	show_follower_tl(5) end
function show_follower_tl(num)
	-- 発言内の num 番目の @xxx ユーザの TL を表示する
	name = follower_names[num];
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「ReTweet」メニュー用ハンドラ
function on_retweet_menu_item(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- エディットコントロールに文字列設定
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	text = "RT @" .. data:get_text('name') .. ": " .. data:get_text_array_joined_text('body');
	text = text:gsub("\r\n", "");
	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

--- 「ホーム」メニュー用ハンドラ
function on_open_home(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name'));
end

--- 「友達のお気に入り」メニュー用ハンドラ
function on_open_friend_favorites(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name') .. "/favorites");
end

--- 「友達のサイト」メニュー用ハンドラ
function on_open_friend_site(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="TWITTER_USER" then
		-- 全文表示
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end

--- 全文表示メニューまたはダブルクリックイベント
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	-- 本文を1行に変換して表示
	item = data:get_text_array_joined_text('body');
	item = item:gsub("\r\n", "");
	
	item = item .. "\r\n";
	item = item .. "----\r\n";
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. data:get_integer('id') .. "\r\n";
	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";
	
	-- ソース
	source = data:get_text('source');
--	item = item .. "source : " .. source .. "\r\n";
	s_url, s_name = source:match("href=\"(.*)\".*>(.*)<");
	if s_url ~= nil then
		item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";
	else
		item = item .. "source : " .. source .. "\r\n";
	end

	mz3.alert(item, data:get_text('name'));

	return true;
end

--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="TWITTER_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.read);

	menu:append_menu("separator");

	menu:append_menu("string", "つぶやく", menu_items.update);

	name = body:get_text('name');

	menu:append_menu("string", "@" .. name .. " さんに返信", menu_items.reply);
	menu:append_menu("string", "@" .. name .. " さんにメッセージ送信", menu_items.new_dm);

	menu:append_menu("separator");

	menu:append_menu("string", "ReTweet...", menu_items.retweet);
	
	-- カテゴリが「timeline」のみ、お気に入り登録を表示
	category_access_type = mz3_main_view.get_selected_category_access_type();
	category_key = mz3.get_serialize_key_by_access_type(category_access_type);
	if category_key == "TWITTER_FRIENDS_TIMELINE" then
		menu:append_menu("string", "お気に入り登録（ふぁぼる）", menu_items.create_favourings);
	end
	
	-- カテゴリが「お気に入り」のみ、お気に入り削除を表示
	if category_key == "TWITTER_FAVORITES" then
		menu:append_menu("string", "お気に入り削除", menu_items.destroy_favourings);
	end

	menu:append_menu("string", "フォローする", menu_items.create_friendships);
	menu:append_menu("string", "フォローやめる", menu_items.destroy_friendships);

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " のタイムライン", menu_items.show_friend_timeline);
	
	-- 発言内の @XXX を抽出し、メニュー化
	body_text = body:get_text_array_joined_text('body');
	i = 1;
	for f_name in body_text:gmatch("@([0-9a-zA-Z_]+)") do
		mz3.logger_debug(f_name);
		-- Lua 変数に名前を保存しておく
		follower_names[i] = f_name;
		menu:append_menu("string", "@" .. f_name .. " のタイムライン", menu_items.show_follower_tl[i]);
		-- 最大5人までサポート
		i = i+1;
		if i>5 then
			break;
		end
	end

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " のホームをブラウザで開く", menu_items.open_home);
	menu:append_menu("string", "@" .. name .. " のFavoritesをブラウザで開く", menu_items.open_friend_favorites);

	-- URL が空でなければ「サイト」を追加
	url = body:get_text('url');
	if url~=nil and url:len()>0 then
		menu:append_menu("string", "@" .. name .. " のサイトをブラウザで開く", menu_items.open_friend_site);
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

----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");

-- ボディリストのポップアップメニュー表示イベントハンドラ登録
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");

mz3.logger_debug('twitter.lua end');
