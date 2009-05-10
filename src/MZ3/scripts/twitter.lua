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
menu_items.twitter_item_read = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.twitter_item_retweet = mz3_menu.regist_menu("twitter.on_retweet_menu_item");


----------------------------------------
-- イベントハンドラ
----------------------------------------

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
	item = item .. "name : " .. data:get_text('name') .. "\r\n";
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

-- 暫定のイベント：Twitter用コンテキストメニューの作成
function on_creating_twitter_item_context_menu(serialize_key, event_name, menu)
	-- "全文を読む" を追加
	mz3_menu.insert_menu(menu, 2, "全文を読む...", menu_items.twitter_item_read);
	-- "RT" を追加
	mz3_menu.insert_menu(menu, 3, "ReTweet...", menu_items.twitter_item_retweet);
end

----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");

-- 暫定のイベントです。
mz3.add_event_listener("creating_twitter_item_context_menu",  "twitter.on_creating_twitter_item_context_menu");

mz3.logger_debug('twitter.lua end');
