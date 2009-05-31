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

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('gooホーム', 'id_name', 'gooID');
mz3_account_provider.set_param('gooホーム', 'password_name', 'gooホーム ひとことメール投稿アドレス');


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read          = mz3_menu.regist_menu("goohome.on_read_menu_item");
menu_items.read_comments = mz3_menu.regist_menu("goohome.on_read_comments_menu_item");
menu_items.update        = mz3_menu.regist_menu("goohome.on_goohome_update");


-- ひとこと投稿アドレスからAPI KEYを取得する
function get_api_key_from_quote_mail_address(address)
	key = address:match('^quote%-([0-9a-zA-Z-_]*)@home%.goo%.ne%.jp$');
	if key==nil or string.len(key)~=12 then
		return '';
	end
	
	return key;
end

-- "quote-XXXXXXXXXXXX@home.goo.ne.jp" の形式であることを確認する
function is_valid_quote_mail_address(password)
	return get_api_key_from_quote_mail_address(password)~='';
end

----------------------------------------
-- イベントハンドラ
----------------------------------------


--- ユーザ入力アカウントチェック
function on_check_account(event_name, service_name, id, password)
--	mz3.alert(service_name);
	if service_name == 'gooホーム' then
		if password ~= '' and is_valid_quote_mail_address(password)==false then
			msg = "gooホームひとこと投稿アドレスは下記の形式です。\n"
			    .." quote-XXXXXXXXXXXX@home.goo.ne.jp\n"
			    .."入力されたアドレス：\n"
			    .." " .. password
			    .."\n"
			    .."確認するURLを開きますか？";
			if (mz3.confirm(msg, nil, 'yes_no')=='yes') then
				url = "http://home.goo.ne.jp/config/quote";
				mz3.open_url_by_browser(url);
			end

			return true;
		end
	end
	return false;
end
mz3.add_event_listener("check_account", "goohome.on_check_account");


--- BASIC 認証設定
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gooHome' then
		id       = mz3_account_provider.get_value('gooホーム', 'id');
		password = mz3_account_provider.get_value('gooホーム', 'password');
		
		password = get_api_key_from_quote_mail_address(password);
		
		if id=='' or password=='' then
			mz3.alert('ログイン設定画面でgooIDとひとこと投稿アドレスを設定してください');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "goohome.on_set_basic_auth_account");


--- Twitter風書き込みモードの初期化
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gooHome' then
		-- モード変更
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('GOOHOME_QUOTE_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "goohome.on_reset_twitter_style_post_mode");


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
mz3.add_event_listener("dblclk_body_list", "goohome.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "goohome.on_body_list_click");


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
mz3.add_event_listener("creating_default_group", "goohome.on_creating_default_group", false);


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
mz3.add_event_listener("popup_body_menu",  "goohome.on_popup_body_menu");


----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- gooホーム 友達・注目の人のひとこと一覧
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");


mz3.logger_debug('goohome.lua end');
