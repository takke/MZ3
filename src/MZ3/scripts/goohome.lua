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
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read          = mz3_menu.regist_menu("goohome.on_read_menu_item");
menu_items.read_comments = mz3_menu.regist_menu("goohome.on_read_comments_menu_item");
menu_items.update        = mz3_menu.regist_menu("goohome.on_goohome_update");


----------------------------------------
-- イベントハンドラ
----------------------------------------


--- 全文表示メニューまたはダブルクリックイベント
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
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


--- コメントを読む メニュー用ハンドラ
function on_read_comments_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_comments_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	mz3.open_url_by_browser_with_confirm(data:get_text('url'));

	return true;
end


-- 「つぶやく」メニュー用ハンドラ
function on_goohome_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
		-- 全文表示
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
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


--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="GOOHOME_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.read);

	menu:append_menu("separator");

	menu:append_menu("string", "コメントを読む...", menu_items.read_comments);
	menu:append_menu("string", "つぶやく", menu_items.update);

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

-- ボディリストのクリック・ダブルクリックイベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "goohome.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "goohome.on_body_list_click");

-- ボディリストのポップアップメニュー表示イベントハンドラ登録
mz3.add_event_listener("popup_body_menu",  "goohome.on_popup_body_menu");

-- デフォルトのグループリスト生成
mz3.add_event_listener("creating_default_group", "goohome.on_creating_default_group", false);


----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- gooホーム 友達・注目の人のひとこと一覧
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");


mz3.logger_debug('goohome.lua end');
