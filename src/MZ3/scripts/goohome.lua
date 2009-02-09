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
-- パーサロード
----------------------------------------
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");

----------------------------------------
-- パーサの登録
----------------------------------------
-- gooホーム 友達・注目の人のひとこと一覧
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");

----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
-- TODO APIを実装すること
--goohome_user_read_menu_item = mz3_menu.create_item("goohome.on_read_menu_item");

----------------------------------------
-- イベントフック関数の登録
----------------------------------------
-- TODO 整理
function on_event(serialize_key, event_name, data)
	if serialize_key=="GOOHOME_USER" then
		-- 全文表示
		return on_read_menu_item(serialize_key, event_name,data);
	end
	
	return true;
end

-- 全文表示メニューまたは
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('dblclk_hook : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
	mz3.logger_debug(data:get_text('name'));

	return true;
end

-- 暫定のイベント：gooホーム用コンテキストメニューの作成
function on_creating_goohome_user_context_menu(serialize_key, event_name, menu)
	-- "全文を読む" を追加
	mz3_menu.insert_item(menu, 1, "全文を読む", goohome_user_read_menu_item);
end

-- ボディリストのクリック・ダブルクリックイベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "goohome.on_event");
mz3.add_event_listener("enter_body_list",  "goohome.on_event");

-- 暫定のイベントです。
mz3.add_event_listener("creating_goohome_user_context_menu",  "goohome.on_creating_goohome_user_context_menu");

mz3.logger_debug('goohome.lua end');
