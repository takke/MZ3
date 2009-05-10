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
--- 次へ、前への抽出処理
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
menu_items = {}
menu_items.mixi_echo_item_read    = mz3_menu.regist_menu("mixi.on_mixi_echo_read_menu_item");
menu_items.mixi_echo_update       = mz3_menu.regist_menu("mixi.on_mixi_echo_update");
menu_items.mixi_echo_reply        = mz3_menu.regist_menu("mixi.on_mixi_echo_reply");
menu_items.mixi_echo_show_profile = mz3_menu.regist_menu("mixi.on_mixi_echo_show_profile");

----------------------------------------
-- イベントハンドラ
----------------------------------------

-- 「つぶやく」メニュー用ハンドラ
function on_mixi_echo_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「返信」メニュー用ハンドラ
function on_mixi_echo_reply(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「プロフィール」メニュー用ハンドラ
function on_mixi_echo_show_profile(serialize_key, event_name, data)

	-- 選択中の要素取得
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	
	-- URL 取得
	url = complement_mixi_url(data:get_text('url'));
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
	if serialize_key=="MIXI_RECENT_ECHO_ITEM" then
		-- 全文表示
		return on_mixi_echo_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end

--- 全文表示メニューまたはダブルクリックイベント
function on_mixi_echo_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_mixi_echo_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
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

--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="MIXI_RECENT_ECHO_ITEM" then
		return false;
	end
	
	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.mixi_echo_item_read);
	menu:append_menu("separator", "", 0);
	menu:append_menu("string", "つぶやく", menu_items.mixi_echo_update);
	menu:append_menu("string", "返信", menu_items.mixi_echo_reply);
	menu:append_menu("string", body:get_text('name') .. " さんのプロフィール", menu_items.mixi_echo_show_profile);
	menu:append_menu("separator", "", 0);
	
	-- TODO リンク
	--[[ C++ 版：
	// リンク
	int n = (int)bodyItem.m_linkList.size();
	if( n > 0 ) {
		pSubMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
		for( int i=0; i<n; i++ ) {
			// 追加
			CString s;
			s.Format( L"link : %s", bodyItem.m_linkList[i].text );
			pSubMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
		}
	}
	]]

	-- TODO 各メニューアイテムのリソース値を定数化(またはLua関数化)

	-- ユーザのエコー一覧
	ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST = 34192 -37000;
	menu:append_menu("string", body:get_text('name') .. " さんのエコー", ID_MENU_MIXI_ECHO_ADD_USER_ECHO_LIST);

	-- 引用ユーザのエコー一覧
	ref_user_name = body:get_text('ref_user_name');
	if ref_user_name ~= "" then
		ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST = 34193 -37000;
		menu:append_menu("string", ref_user_name .. " さんのエコー", ID_MENU_MIXI_ECHO_ADD_REF_USER_ECHO_LIST);
	end

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	
	return true;
end


----------------------------------------
-- イベントフック関数の登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");

-- ボディリストのポップアップメニュー表示イベントハンドラ登録
mz3.add_event_listener("popup_body_menu",  "mixi.on_popup_body_menu");

mz3.logger_debug('mixi.lua end');
