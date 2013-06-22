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

local json = require ("scripts\\lib\\dkjson")


----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('mixi', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('mixi', 'id_name', 'メールアドレス');
mz3_account_provider.set_param('mixi', 'password_name', 'パスワード');


----------------------------------------
-- パーサおよびアクセス種別のロード
----------------------------------------
require("scripts\\mixi_parser");

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


----------------------------------------
-- メニューへの登録
----------------------------------------

--- デフォルトのグループリスト生成イベントハンドラ(メッセージ用)
--
-- @param serialize_key シリアライズキー(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group_for_mixi_list_message(serialize_key, event_name, group)

	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then
		-- 受信箱
		local tab = mz3_group_data.get_group_item_by_name(group, 'メッセージ');
		mz3_group_item.append_category(tab, "メッセージ(受信箱)", "MESSAGE_IN", "http://mixi.jp/list_message.pl");

		-- 送信箱
		local tab = mz3_group_data.get_group_item_by_name(group, 'メッセージ');
		mz3_group_item.append_category(tab, "メッセージ(送信箱)", "MESSAGE_OUT", "http://mixi.jp/list_message.pl?box=outbox");

		-- 公式メッセージ
		local tab = mz3_group_data.get_group_item_by_name(group, 'メッセージ');
		mz3_group_item.append_category(tab, "公式メッセージ", "MIXI_LIST_MESSAGE_OFFICIAL", "http://mixi.jp/list_message.pl?box=noticebox");
	end
end
-- イベントフック関数の登録
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group_for_mixi_list_message");


--- デフォルトのグループリスト生成イベントハンドラ(逆あしあと用)
--
-- @param serialize_key シリアライズキー(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group_for_mixi_show_self_log(serialize_key, event_name, group)

	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then

		-- その他/逆あしあと 追加
		local tab = mz3_group_data.get_group_item_by_name(group, 'その他');
		mz3_group_item.append_category(tab, "逆あしあと", "MIXI_SHOW_SELF_LOG", "http://mixi.jp/show_self_log.pl");

	end
end
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group_for_mixi_show_self_log");


----------------------------------------
-- estimate 対象に追加
----------------------------------------

--- estimate 対象判別イベントハンドラ
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        解析対象URL
--
function on_estimate_access_type_by_url_for_mixi_list_message(event_name, url, data1, data2)

    -- 受信箱
	if line_has_strings(url, 'list_message.pl', 'box=inbox') then
		return true, mz3.get_access_type_by_key('MESSAGE_IN');
	end
	
    -- 送信箱
	if line_has_strings(url, 'list_message.pl', 'box=outbox') then
		return true, mz3.get_access_type_by_key('MESSAGE_OUT');
	end
	
    -- 公式メッセージ
	if line_has_strings(url, 'list_message.pl', 'box=noticebox') then
		return true, mz3.get_access_type_by_key('MIXI_LIST_MESSAGE_OFFICIAL');
	end

	return false;
end
-- イベントフック関数の登録
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type_by_url_for_mixi_list_message");


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
	if serialize_key=="MIXI_RECENT_VOICE_DETAIL" then
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
	if serialize_key~="MIXI_RECENT_VOICE_DETAIL" then
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
	local extra_count = body:get_text('extra_count')
	local extra_url   = body:get_text('extra_url');
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


--- もっとコメントを読む
function on_mixi_open_browser(serialize_key, event_name, data)

	-- 通常の開く(body 項目なのでレポート画面が開く)
	mz3_main_view.open_url(data);

--[[
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	mz3.open_url_by_browser_with_confirm(body:get_text('extra_url'));
]]

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

	-- トピック一覧
	if line_has_strings(url, 'list_bbs.pl') then
		return true, mz3.get_access_type_by_key('TOPIC');
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
		msg = 'mixi でつぶやきます。 \n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'MIXI_ADD_VOICE_REPLY' then
		local username = data:get_text('name');
		msg = 'mixi で ' .. username .. ' さんに返信します。 \n'
		   .. '---- 発言 ----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	-- POST パラメータを設定
	local post_key = mixi.post_key;
	if post_key==nil or post_key=='' then
		mz3.alert('送信用のキーが見つかりません。つぶやき一覧(みんなのボイス)をリロードして下さい。');
		return true;
	end
	
	if serialize_key == 'MIXI_ADD_VOICE' then
		-- つぶやき投稿は共通処理で。

		-- クロスポスト管理データ初期化
		mz3.init_cross_post_info("echo");

		do_post_to_echo(text);
		return true;
	end
	
	
	post = MZ3PostData:create();
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
	post:set_content_type('	application/json-rpc; charset=UTF-8' .. '\r\n');
	
	local owner_id = mixi.owner_id;

	local jsontable = {
      jsonrpc="2.0",
      method="jp.mixi.voice.create",
      params={
        viewer_id=owner_id,
        owner_id=owner_id,
        body=text,
        twitter_sync=0,
        privacy={name="public"},
        via=18,
        guidance_id="1"
      },
      id=0
	};
	post:append_post_body(json.encode(jsontable));


	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST先URL設定
	local post_key = mixi.post_key;
--	mz3.logger_debug(post_key);
	url = 'http://mixi.jp/system/rpc.json?auth_type=postkey&secret=' .. post_key;
	
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
		
		-- 本文取得
		local f = io.open(filename, 'r');
		local file = f:read('*a');
		f:close();
		
--		mz3.logger_debug('file:' .. file);
		
		if mz3.is_mixi_logout(serialize_key) then
			mz3.alert('未ログインです。みんなのボイスをリロードし、mixiにログインして下さい。');
			return true;
		elseif line_has_strings(file, '"status":"OK"')==false then -- <p class="messageAlert">データがありません<br /></p>
			mz3.alert('投稿に失敗しました');
			return true;
		else
			-- 投稿成功
			mz3_main_view.set_info_text("つぶやき投稿完了");

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


mz3.logger_debug('mixi.lua end');
