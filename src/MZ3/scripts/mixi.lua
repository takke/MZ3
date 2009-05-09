--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi.lua start');
module("mixi", package.seeall)

--------------------------------------------------
-- 次へ、前への抽出処理
--------------------------------------------------
function parse_next_back_link(line, base_url)

	local back_data = nil;
	local next_data = nil;
	
	-- <ul><li><a href="new_bbs.pl?page=1">前を表示</a></li>
	-- <li>51件～100件を表示</li>
	-- <li><a href="new_bbs.pl?page=3">次を表示</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- 前
		local url, t = line:match([[href="([^"]+)">(前[^<]+)<]]);
		if url~=nil then
			back_data = MZ3Data:create();
			back_data:set_text("title", "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- 次
		local url, t = line:match([[href="([^"]+)">(次[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text("title", "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end


----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- ★リスト系
-- コミュニティ最新書込一覧
require("scripts\\mixi\\mixi_new_bbs_parser");
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- コミュニティコメント記入履歴 : 最新書込一覧と同一
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");

-- トップページ
require("scripts\\mixi\\mixi_home_parser");
mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");

-- メッセージ(受信箱, 送信箱), 公式メッセージ, メッセージ詳細
require("scripts\\mixi\\mixi_new_official_message_parser");
require("scripts\\mixi\\mixi_message_outbox_parser");
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- 逆あしあと
require("scripts\\mixi\\mixi_show_self_log_parser");


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
mixi_echo_item_read_menu_item = mz3_menu.regist_menu("mixi.on_read_menu_item");


----------------------------------------
-- イベントハンドラ
----------------------------------------

--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="MIXI_RECENT_ECHO_ITEM" then
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

-- 暫定のイベント：mixi echo用コンテキストメニューの作成
function on_creating_mixi_echo_item_context_menu(serialize_key, event_name, menu)
	-- "全文を読む" を追加
	mz3_menu.insert_menu(menu, 2, "全文を読む...", mixi_echo_item_read_menu_item);
end


----------------------------------------
-- イベントフック関数の登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");

-- 暫定のイベントです。
mz3.add_event_listener("creating_mixi_echo_item_context_menu",  "mixi.on_creating_mixi_echo_item_context_menu");

mz3.logger_debug('mixi.lua end');
