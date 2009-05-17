--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : goohome
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('goohome.lua start');
module("goohome", package.seeall)

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('goohome', false);

----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- gooホーム 友達・注目の人のひとこと一覧
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");

----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
goohome_user_read_menu_item = mz3_menu.regist_menu("goohome.on_read_menu_item");

----------------------------------------
-- イベントハンドラ
----------------------------------------

--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
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
	item = item .. data:get_date();

	mz3.alert(item, data:get_text('name'));

	return true;
end

-- 暫定のイベント：gooホーム用コンテキストメニューの作成
function on_creating_goohome_user_context_menu(serialize_key, event_name, menu)
	-- "全文を読む" を追加
	mz3_menu.insert_menu(menu, 2, "全文を読む...", goohome_user_read_menu_item);
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
	if services:find(' goohome', 1, true) ~= nil then

		-- Wassrタブ追加
		local tab = MZ3GroupItem:create("gooホーム");
		tab:append_category("友達・注目の人", "GOOHOME_QUOTE_QUOTES_FRIENDS");
		tab:append_category("自分のひとこと一覧", "GOOHOME_QUOTE_QUOTES_FRIENDS", "http://home.goo.ne.jp/api/quote/quotes/myself/json");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end


----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- ボディリストのクリック・ダブルクリックイベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "goohome.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "goohome.on_body_list_click");

-- 暫定のイベントです。
mz3.add_event_listener("creating_goohome_user_context_menu",  "goohome.on_creating_goohome_user_context_menu");

-- デフォルトのグループリスト生成
mz3.add_event_listener("creating_default_group", "goohome.on_creating_default_group", false);

mz3.logger_debug('goohome.lua end');
