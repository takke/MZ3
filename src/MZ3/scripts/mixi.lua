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

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('mixi', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('mixi', 'id_name', 'メールアドレス');
mz3_account_provider.set_param('mixi', 'password_name', 'パスワード');


--------------------------------------------------
-- 【みんなのエコー一覧】
-- [list] recent_echo.pl 用パーサ
--
-- http://mixi.jp/recent_echo.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_recent_echo_parser(parent, body, html)
	mz3.logger_debug("mixi_recent_echo_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	local t1 = mz3.get_tick_count();
	local in_data_region = false;

	local line_count = html:get_count();
	
	-- post_key 探索
	local post_key = '';
	for i=100, line_count-1 do
		line = html:get_at(i);
		
		-- <input type="hidden" name="post_key" id="post_key" value="xxx"> 
		if line_has_strings(line, 'hidden', 'id="post_key"') then
			post_key = line:match('value="(.-)"');
			break;
		end
	end


	-- 行数取得
	for i=230, line_count-1 do
		line = html:get_at(i);

		-- 項目探索 以下一行
		-- <td class="thumb">
		if line_has_strings(line, "<td", "class", "thumb") then
		-- <div class="echo_member_id" style="display: none;">ユーザID</div>
		-- if line_has_strings(line, "<div", "class", "echo_member_id") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			i = i+2;
			line = html:get_at(i);

			-- URL 取得
			url = complement_mixi_url(data:get_text('url'));
			data:set_text("url", url);

			-- 画像取得
			i = i+2;
			line = html:get_at(i);
			image_url, after = line:match("src=\"([^\"]+)\"");
			data:add_text_array("image", image_url);

			-- ユーザ名
			-- name = line:match(">([^<]+)(<.*)$");
			i = i+11;
			line = html:get_at(i);
			name = line;
			data:set_text("name", name);

			i = i+9;
			line = html:get_at(i);
			if line == "\n" then
				-- みんなのエコーと自分の一覧で改行数が違うので… 最低
				i = i+1;
				line = html:get_at(i);
			end

			-- 発言
			if line:find( "href=", 1, true ) ~= nil then
				post = line:match(">([^<]+)(<.*)$");
				-- post をリプライ用に修正
				i = i+4;
				line = html:get_at(i);

				-- post2 = line:match(">([^<]+)(<.*)$");
				post = post .. " " .. line;
			else
				post = line;
			end

			data:add_body_with_extract(post);

			i = i+4;
			line = html:get_at(i);

			if line:find( "<span>", 1, true ) ~= nil then
				i = i+4;
				line = html:get_at(i);
			elseif line:find( "href=", 1, true ) ~= nil then
				i = i+2;
				line = html:get_at(i);
			end

			-- 時間
			-- date = line:match(">([^<]+)(<.*)$");
			date = line;
			data:set_date(date);

			i = i+8;
			line = html:get_at(i);

			if line == "\n" then
				i = i+2;
				line = html:get_at(i);
			end

			-- id
			id = line:match(">([^<]+)(<.*)$");
			data:set_integer("id", id);

			-- URL に応じてアクセス種別を設定
			--type = mz3.estimate_access_type_by_url(url);
			type = mz3.get_access_type_by_key('MIXI_RECENT_ECHO_ITEM');
			data:set_access_type(type);
			
			-- post_key 追加
			data:set_text('post_key', post_key);

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

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_recent_echo_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
-- みんなのエコー
mz3.set_parser("MIXI_RECENT_ECHO", "mixi.mixi_recent_echo_parser");
mz3.set_parser("MIXI_RES_ECHO"   , "http://mixi.jp/res_echo.pl");
mz3.set_parser("MIXI_LIST_ECHO"  , "http://mixi.jp/list_echo.pl?id={owner_id}");


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

--- Twitter風書き込みモードの初期化
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='mixi' then
		-- モード変更
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('MIXI_ADD_ECHO'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "mixi.on_reset_twitter_style_post_mode");


--- Twitterスタイルのボタン名称の更新
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'MIXI_ADD_ECHO' then
		return true, 'echo';
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		return true, '返信';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "mixi.on_update_twitter_update_button");


--- 「つぶやく」メニュー用ハンドラ
function on_mixi_echo_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

--- 「返信」メニュー用ハンドラ
function on_mixi_echo_reply(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY);

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
	menu:append_menu("separator");
	menu:append_menu("string", "つぶやく", menu_items.mixi_echo_update);
	menu:append_menu("string", "返信", menu_items.mixi_echo_reply);
	menu:append_menu("string", body:get_text('name') .. " さんのプロフィール", menu_items.mixi_echo_show_profile);
	menu:append_menu("separator");

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
mz3.add_event_listener("popup_body_menu",  "mixi.on_popup_body_menu");


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
		local tab = MZ3GroupItem:create("エコー");
		tab:append_category("みんなのエコー", "MIXI_RECENT_ECHO");
		tab:append_category("自分への返信一覧", "MIXI_RECENT_ECHO", "http://mixi.jp/res_echo.pl");
		tab:append_category("自分の一覧", "MIXI_RECENT_ECHO", "http://mixi.jp/list_echo.pl?id={owner_id}");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();

		-- その他
		local tab = MZ3GroupItem:create("その他");
		tab:append_category("マイミク一覧", "FRIEND");
		tab:append_category("紹介文", "INTRO");
		tab:append_category("足あと", "FOOTSTEP");
		tab:append_category("カレンダー", "CALENDAR", "show_calendar.pl");
		tab:append_category("ブックマーク", "BOOKMARK");
		tab:append_category("お気に入りユーザー", "FAVORITE", "list_bookmark.pl");
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
		if serialize_key == 'MIXI_ADD_ECHO_REPLY' then
			-- 未入力はNG => 何もせずに終了
			return true;
		elseif serialize_key == 'MIXI_ADD_ECHO' then
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
	if serialize_key == 'MIXI_ADD_ECHO' then
		msg = 'mixi エコーで発言します。\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		local username = data:get_text('name');
		msg = 'mixi エコーで ' .. username .. ' さんに返信します。\n'
		   .. '---- 発言 ----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	-- POST パラメータを設定
	post = MZ3PostData:create();
	local post_key = data:get_text('post_key');
	if post_key=='' then
		mz3.alert('送信用のキーが見つかりません。エコー一覧をリロードして下さい。');
		return true;
	end
	if serialize_key == 'MIXI_ADD_ECHO' then
		post:append_post_body('body=');
		post:append_post_body(mz3.url_encode(text, 'euc-jp'));
		post:append_post_body('&x=28&y=20');
		post:append_post_body('&post_key=');
		post:append_post_body(post_key);
		post:append_post_body('&redirect=recent_echo');
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		-- body=test&x=36&y=12&parent_member_id=xxx&parent_post_time=20090626110655&redirect=recent_echo&post_key=xxx
		local echo_member_id = data:get_integer('author_id');
		local echo_post_time = data:get_text('echo_post_time');
		
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
		post:append_post_body('&redirect=recent_echo');
		post:append_post_body('&post_key=');
		post:append_post_body(post_key);
	end
	
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
--	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
--		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
--		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
--	end

	-- POST先URL設定
	if serialize_key == 'MIXI_ADD_ECHO' then
		url = 'http://mixi.jp/add_echo.pl';
	elseif serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		url = 'http://mixi.jp/add_echo.pl';
	end
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end
mz3.add_event_listener("click_update_button", "mixi.on_click_update_button");


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
	
	if serialize_key == 'MIXI_ADD_ECHO' or 
	   serialize_key == 'MIXI_ADD_ECHO_REPLY' then
		if mz3.is_mixi_logout(serialize_key) then
			mz3.alert('未ログインです。エコー一覧をリロードし、mixiにログインして下さい。');
			return true;
		else
			-- 投稿成功
			mz3_main_view.set_info_text("エコー書き込み完了");

			-- 入力値を消去
			mz3_main_view.set_edit_text("");

			return true;
		end
	end
end
mz3.add_event_listener("post_end", "mixi.on_post_end");


----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- コミュニティ最新書込一覧
require("scripts\\mixi\\mixi_new_bbs_parser");

-- トップページ
require("scripts\\mixi\\mixi_home_parser");

-- 日記詳細
require("scripts\\mixi\\mixi_view_diary_parser");

-- メッセージ(受信箱, 送信箱), 公式メッセージ, メッセージ詳細
require("scripts\\mixi\\mixi_new_official_message_parser");
require("scripts\\mixi\\mixi_message_outbox_parser");
require("scripts\\mixi\\mixi_message_inbox_parser");
require("scripts\\mixi\\mixi_view_message_parser");

-- 逆あしあと
require("scripts\\mixi\\mixi_show_self_log_parser");

-- お気に入りコミュ、ユーザ
require("scripts\\mixi\\mixi_bookmark_community_parser");
require("scripts\\mixi\\mixi_bookmark_user_parser");

-- ニュース関連日記
require("scripts\\mixi\\mixi_news_quote_diary_parser");

mz3.logger_debug('mixi.lua end');
