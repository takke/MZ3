--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : GoogleReader
--
-- $Id: gmail.lua 1453 2009-12-13 13:06:06Z takke $
--------------------------------------------------
mz3.logger_debug('google_reader.lua start');
module("greader", package.seeall)

--------------------------------------------------
-- サービスの登録(タブ初期化、ログイン設定用)
--------------------------------------------------
mz3.regist_service('GoogleReader', true);

-- ログイン設定画面のプルダウン名、表示名の設定
-- TODO とりあえず GMail を使う
--mz3_account_provider.set_param('GMail', 'id_name', 'メールアドレス');
--mz3_account_provider.set_param('GMail', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- タグリスト
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('GoogleReader');								-- サービス種別
type:set_serialize_key('GOOGLE_READER_TAG_LIST');				-- シリアライズキー
type:set_short_title('GoogleReader タグリスト');				-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('google_reader\\{urlafter:/reader}.html');	-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング
type:set_default_url('http://www.google.co.jp/reader/api/0/tag/list?output=xml&client=MZ3');
type:set_body_header(1, 'title', 'タイトル');
type:set_body_header(2, 'date', '');
type:set_body_header(3, 'date', '');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '');

-- ログイン用
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('GoogleReader');								-- サービス種別
type:set_serialize_key('GOOGLE_READER_LOGIN');					-- シリアライズキー
type:set_short_title('GoogleReader ログイン');					-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_cache_file_pattern('google_reader\\login.html');		-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング

-- フィード一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('GoogleReader');								-- サービス種別
type:set_serialize_key('GOOGLE_READER_ATOM_LIST');				-- シリアライズキー
type:set_short_title('GoogleReader フィード');					-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('google_reader\\{urlafter:/reader}.html');	-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング
type:set_body_header(1, 'name', 'フィード');
type:set_body_header(2, 'title', 'タイトル');
type:set_body_header(3, 'date', '日付');
type:set_body_integrated_line_pattern(1, '%1  %3');
type:set_body_integrated_line_pattern(2, '%2');

-- フィード
type = MZ3AccessTypeInfo.create();
type:set_info_type('other');									-- カテゴリ
type:set_service_type('GoogleReader');								-- サービス種別
type:set_serialize_key('GOOGLE_READER_ATOM_ITEM');				-- シリアライズキー
type:set_short_title('GoogleReader フィード');					-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}

-- メイン画面下ペイン用
menu_items.read               = mz3_menu.regist_menu("greader.on_read_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("greader.on_open_by_browser_menu_item");
menu_items.add_star           = mz3_menu.regist_menu("greader.on_add_star_menu_item");


----------------------------------------
-- メニューへの登録
----------------------------------------

--- デフォルトのグループリスト生成イベントハンドラ
--
-- @param serialize_key シリアライズキー(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' GoogleReader', 1, true) ~= nil then
		-- 受信トレイ
		local tab = MZ3GroupItem:create("GoogleReader");

		tab:append_category("フォルダ一覧", "GOOGLE_READER_TAG_LIST");

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end

end
mz3.add_event_listener("creating_default_group", "greader.on_creating_default_group");


----------------------------------------
-- パーサ
----------------------------------------

--------------------------------------------------
-- 【タグリスト】
--
-- フォルダ一覧
--   http://www.google.co.jp/reader/api/0/tag/list?output=xml&client=MZ3
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function google_reader_tag_list_parser(parent, body, html)
	mz3.logger_debug("google_reader_tag_list_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	-- ログイン判定
	is_logged_in = false;
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
--		mz3.logger_debug(line);
		
		-- <object><list name="tags"><object>
		-- 上記があればログイン済
		if line_has_strings(line, '<object><list name="tags"><object>') then
			is_logged_in = true;
			break;
		end
	end
	
	if is_logged_in then
--		mz3.alert('ログイン済');
		
		-- 複数行に分かれているので1行に結合
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end

		-- ログイン済みのHTMLのパース
		parse_tag_list(parent, body, line);

	else
		-- ログイン処理

		-- TODO とりあえず Gmail と同じもの
		mail_address  = mz3_account_provider.get_value('GMail', 'id');
		mail_password = mz3_account_provider.get_value('GMail', 'password');
		
		if (mail_address == "" or mail_password == "") then
			mz3.alert("メールアドレスとパスワードをログイン設定画面で設定して下さい");
			return;
		end

		-- URL 生成
		url = "https://www.google.com/accounts/ClientLogin";
		post = mz3_post_data.create();
		mz3_post_data.append_post_body(post, "Email=" .. mz3.url_encode(mail_address, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "Passwd=" .. mz3.url_encode(mail_password, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "service=reader");
--		mz3_post_data.append_post_body(post, "continue=" .. mz3.url_encode(continue_value, 'utf8'));
		
		-- 通信開始
		access_type = mz3.get_access_type_by_key("GOOGLE_READER_LOGIN");
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("google_reader_tag_list_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GOOGLE_READER_TAG_LIST", "greader.google_reader_tag_list_parser");


--- ログイン済みのタグリストの解析
--
-- @param parent 上ペインの選択オブジェクト(MZ3Data*)
-- @param body   下ペインのオブジェクト群(MZ3DataList*)
-- @param line   HTML 全文を1行に結合した文字列
--
function parse_tag_list(parent, body, line)
	mz3.logger_debug("parse_tag_list start");

	-- 最初のobjectを読み飛ばす
	local list_tag = line:match('<list.->(.-)</list>');
	if list_tag == nil then
		mz3.alert('取得失敗(no list tag)');
		return;
	end
	
	-- data 生成
	data = MZ3Data:create();

	for object_tag in list_tag:gmatch("<object>(.-)</object>") do
--		mz3.logger_debug(object_tag);
		
		-- /object/list/object/string[name="id"]
		-- <string name="id">user/xxx/state/com.google/starred</string>
		local id = object_tag:match('<string name="id">(.-)</');
		
		-- 末尾の / 以降をフォルダ名とする
		local head, name = id:match('^(.*/)([^/]*)$');
		
		-- URL
		--http://www.google.co.jp/reader/atom/user/-/state/com.google/starred
		local url = 'http://www.google.co.jp/reader/atom/' .. head .. mz3.url_encode(name, 'utf8');

		if name == 'starred' then
			name = 'スター付き';
		end
		if name == 'broadcast' then
			name = '公開';
		end
		
		-- set
		data:set_text("name", name);
		data:set_text("title", name);
		data:set_text("url", url);
		data:set_access_type(mz3.get_access_type_by_key('GOOGLE_READER_ATOM_LIST'));

		-- data 追加
		body:add(data.data);
	end
	
	-- data 削除
	data:delete();

	mz3.logger_debug("parse_tag_list end");
end


--------------------------------------------------
-- 【ログイン】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function google_reader_login_parser(parent, body, html)
	mz3.logger_debug("google_reader_login_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	local login_ok = false;
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
--		mz3.logger_debug(line);
		
		-- SID=...
		if line_has_strings(line, "SID=") then
			login_ok = true;
		end
	end
--	mz3.alert('url : ' .. url);
	if login_ok == false then
		mz3.alert('ログインに失敗しました。 \nメールアドレスとパスワードを確認してください。');
		return;
	end
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key("GOOGLE_READER_TAG_LIST");
	url = nil;
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("google_reader_login_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GOOGLE_READER_LOGIN", "greader.google_reader_login_parser");


--------------------------------------------------
-- 【フィード】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function google_reader_atom_list_parser(parent, body, html)
	mz3.logger_debug("google_reader_atom_list_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	-- 複数行に分かれているので1行に結合
	line = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = line .. html:get_at(i);
	end
	
	-- data 生成
	data = MZ3Data:create();

	for entry_tag in line:gmatch("<entry.->(.-)</entry>") do
--		mz3.logger_debug(entry_tag);
		
		data:clear();

		-- id
		-- <id gr:original-id="http://rss.rssad.jp/rss/ad/xxx/xxx?type=2&ent=xxx">tag:google.com,2005:reader/item/xxx</id>
		local id = entry_tag:match('<id .->(.-)</');
		local updated = entry_tag:match('<updated>(.-)</');
		local title = entry_tag:match('<title.->(.-)</');
		title = title:gsub('&amp;', '&');
		title = mz3.decode_html_entity(title);
		local author = '';
		local source = entry_tag:match('<source.->(.-)</source>');
		if source ~= nil then
			author = source:match('<title.->(.-)</');
			author = author:gsub('&amp;', '&');
			-- TODO O&#39;Reilly 対応
			author = mz3.decode_html_entity(author);
			-- TODO alternate 抽出＋メニュー追加
		end

		-- URL : <link rel="alternate" href="..."> を対象とする。
		-- いわゆる代替URLなのでWebページとなる。
		local url = entry_tag:match('<link rel="alternate" href="(.-)"');
		if url ~= nil then
			url = url:gsub('&amp;', '&');
			-- TODO decode_html_entity 必要？
		end
		
		local summary = entry_tag:match('<summary.->(.-)</summary>');
		if summary ~= nil then
			summary = summary:gsub('&amp;', '&');
			summary = mz3.decode_html_entity(summary);
		else
			summary = '';
		end
		
		-- set
		data:set_text("id", id);		-- 通常のアイテムと違い、ID が文字列である点に注意
		data:set_text("name", author);
		data:set_text("title", title);
		data:parse_date_line(updated);
		data:set_text("url", url);
		data:add_text_array('body', summary);
		data:set_access_type(mz3.get_access_type_by_key('GOOGLE_READER_ATOM_ITEM'));

		-- data 追加
		body:add(data.data);
	end
	
	-- data 削除
	data:delete();

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("google_reader_atom_list_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GOOGLE_READER_ATOM_LIST", "greader.google_reader_atom_list_parser");


----------------------------------------
-- イベントハンドラ
----------------------------------------

--- ボディリストのアイコンのインデックス取得
--
-- @param event_name    'get_body_list_default_icon_index'
-- @param serialize_key シリアライズキー(nil)
-- @param body          body data
--
-- @return (1) [bool] 成功時は true, 続行時は false
-- @return (2) [int] アイコンインデックス
--
function on_get_body_list_default_icon_index(event_name, serialize_key, body)

	if serialize_key == "GOOGLE_READER_ATOM_LIST" then
		return true, 8;
	end
	if serialize_key == "GOOGLE_READER_ATOM_ITEM" then
		return true, 8;
	end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "greader.on_get_body_list_default_icon_index");


--- ViewStyle 変更
--
-- @param event_name    'get_view_style'
-- @param serialize_key カテゴリのシリアライズキー
--
-- @return (1) [bool] 成功時は true, 続行時は false
-- @return (2) [int] VIEW_STYLE_*
--
function on_get_view_style(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type=='GoogleReader' then
		return true, VIEW_STYLE_DEFAULT;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "greader.on_get_view_style");


--- 全文表示メニューまたはダブルクリックイベント
function on_read_menu_item(serialize_key, event_name, data)
	-- メニュー直接の場合はシリアライズキーがないため data から取得する
	data = MZ3Data:create(data);
	serialize_key = data:get_serialize_key();
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');
	
	if serialize_key == 'GOOGLE_READER_ATOM_LIST' then
		-- フィードリストのアイテム(フォルダ名)
		-- 本来はカテゴリ項目
		item = '';
		item = item .. data:get_text('name') .. "\r\n";
		item = item .. "----\r\n";
		item = item .. data:get_text('url') .. "\r\n";
		mz3.alert(item, data:get_text('name'));
	end
	
	if serialize_key == 'GOOGLE_READER_ATOM_ITEM' then
		-- フィード詳細
		-- TODO 本来はTwitter詳細画面かレポートビューで表示すべき
		item = '';
		item = item .. data:get_text('name') .. " - " .. data:get_date() .. "\r\n";
		item = item .. data:get_text('title') .. "\r\n";
		item = item .. "----\r\n";
		summary = data:get_text_array_joined_text('body');
		summary = summary:gsub('<.->', '');
		item = item .. summary .. "\r\n";
		item = item .. "----\r\n";
		item = item .. data:get_text('url') .. "\r\n";
		mz3.alert(item, data:get_text('name'));
	end

	return true;
end


--- フォルダを開くメニュー用ハンドラ
function on_show_folder(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "+" .. name;
	url = body:get_text('url');
	key = "GOOGLE_READER_ATOM_LIST";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- スターを付ける
-- TODO
function on_add_star_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_add_star_menu_item: (' .. serialize_key .. ', ' .. event_name .. ')');

	mz3.alert('本バージョンでは未対応です');

	return true;
--[[
	data = MZ3Data:create(data);

	-- POSTパラメータ生成
	post = MZ3PostData:create();

	-- tパラメータはメールURLから取得する
	t = data:get_text('url');
	t = t:match('th=(.*)$');
	post_body = 'redir=%3F&tact=st&nvp_tbu_go=%E5%AE%9F%E8%A1%8C&t=' .. t .. '&bact=';
	post:append_post_body(post_body);

	-- 通信開始
	url = data:get_text('post_url');
	access_type = mz3.get_access_type_by_key("GMAIL_ADD_STAR");
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);
	return true;
]]
end


--- POST 完了イベント
--
-- @param event_name    'post_end'
-- @param serialize_key 完了項目のシリアライズキー
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param filename      レスポンスファイル
-- @param wnd           wnd
--
-- TODO OK?
--[[
function on_post_end(event_name, serialize_key, http_status, filename)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="gmail" then
		return false;
	end

	-- ステータスコードチェック
	if http_status~=200 then
		-- エラーアリなので中断するために true を返す
		local msg="失敗しました";
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		return true;
	end

	-- リクエストの種別に応じてメッセージを表示
	if serialize_key == "GMAIL_ADD_STAR" then
		mz3_main_view.set_info_text("スターつけた！");
	end
	if serialize_key == "GMAIL_REMOVE_STAR" then
		mz3_main_view.set_info_text("スターとったどー！");
	end
	if serialize_key == "GMAIL_ARCHIVE" then
		mz3_main_view.set_info_text("アーカイブしたー！");
	end

	return true;
end
mz3.add_event_listener("post_end", "gmail.on_post_end");
]]


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)

	if serialize_key=="GOOGLE_READER_ATOM_LIST" then
		-- カテゴリ追加
		on_show_folder(serialize_key, event_name, data);
		return true;
	end
	if serialize_key=="GOOGLE_READER_ATOM_ITEM" then
		-- 全文表示
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end
mz3.add_event_listener("dblclk_body_list", "greader.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "greader.on_body_list_click");


--- 「ブラウザで開く」メニュー用ハンドラ
function on_open_by_browser_menu_item(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());

	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end


--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)

	if serialize_key=="GOOGLE_READER_ATOM_LIST" then
		-- インスタンス化
		body = MZ3Data:create(body);
		
		-- メニュー生成
		menu = MZ3Menu:create_popup_menu();
		
		menu:append_menu("string", "詳細...", menu_items.read);

		-- ポップアップ
		menu:popup(wnd);
		
		-- メニューリソース削除
		menu:delete();
		
		return true;
	end
	if serialize_key=="GOOGLE_READER_ATOM_ITEM" then
		-- インスタンス化
		body = MZ3Data:create(body);
		
		-- メニュー生成
		menu = MZ3Menu:create_popup_menu();
		
		menu:append_menu("string", "詳細...", menu_items.read);
		menu:append_menu("string", "ブラウザで開く...", menu_items.open_by_browser);
		menu:append_menu("separator");
		menu:append_menu("string", "スターを付ける...", menu_items.add_star);

		-- ポップアップ
		menu:popup(wnd);
		
		-- メニューリソース削除
		menu:delete();
		
		return true;
	end
	
	return false;

end
mz3.add_event_listener("popup_body_menu",  "greader.on_popup_body_menu");


mz3.logger_debug('google_reader.lua end');
