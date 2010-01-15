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
type:set_service_type('GoogleReader');							-- サービス種別
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
type:set_service_type('GoogleReader');							-- サービス種別
type:set_serialize_key('GOOGLE_READER_LOGIN');					-- シリアライズキー
type:set_short_title('GoogleReader ログイン');					-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_cache_file_pattern('google_reader\\login.html');		-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング

-- フィード一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('GoogleReader');							-- サービス種別
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
type:set_service_type('GoogleReader');							-- サービス種別
type:set_serialize_key('GOOGLE_READER_ATOM_ITEM');				-- シリアライズキー
type:set_short_title('GoogleReader フィード');					-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング

-- トークン
type = MZ3AccessTypeInfo.create();
type:set_info_type('other');									-- カテゴリ
type:set_service_type('GoogleReader');							-- サービス種別
type:set_serialize_key('GOOGLE_READER_TOKEN');					-- シリアライズキー
type:set_short_title('GoogleReader Token');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング

-- スターを付ける
type = MZ3AccessTypeInfo:create();
type:set_info_type('post');										-- カテゴリ
type:set_service_type('GoogleReader');							-- サービス種別
type:set_serialize_key('GOOGLE_READER_ADD_STAR');				-- シリアライズキー
type:set_short_title('GoogleReader スター');							-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}

-- メイン画面下ペイン用
menu_items.read               = mz3_menu.regist_menu("greader.on_read_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("greader.on_open_by_browser_menu_item");
menu_items.add_star           = mz3_menu.regist_menu("greader.on_add_star_menu_item");
menu_items.show_main_view     = mz3_menu.regist_menu("greader.on_show_main_view");


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
		line = html:get_all_text();

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
reset_body_list_pos = 0;	-- 取得後のボディリストの選択位置
function google_reader_atom_list_parser(parent, body, html)
	mz3.logger_debug("google_reader_atom_list_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- リクエストのURLが「次へ」であればクリアしない
	local last_url = mz3.get_last_request_url();
	local insert_at = 0;
	local next_link_pos = 0;
	reset_body_list_pos = 0;
	if line_has_strings(last_url, 'c=') then
		-- 「次へ」のリクエストなので既存の「次へ」を削除する
		local n = body:get_count();
		for i=0, n-1 do
			d = MZ3Data:create(body:get_data(i));
			if d:get_serialize_key() == 'GOOGLE_READER_ATOM_LIST' then
				insert_at = i;
				next_link_pos = i;
				reset_body_list_pos = i;
				break;
			end
		end
	else
		-- 全消去
		body:clear();
	end
	
	local t1 = mz3.get_tick_count();
	
	-- 複数行に分かれているので1行に結合
	local line = html:get_all_text();
	
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
			author = mz3.decode_html_entity(author);
			-- alternate 抽出＋メニュー追加
			-- <link rel="alternate" href="http://journal.mycom.co.jp" type="text/html"/>
			link_url = source:match('<link .-href="(.-)"');
			data:add_link_list(link_url, link_url);
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
		body:insert(insert_at, data.data);
		insert_at = insert_at + 1;
	end
	
	-- <gr:continuation>CPPz-oaxoZ8C</gr:continuation>
	continuation = line:match('<gr:continuation>(.-)</gr:continuation>');
	if continuation ~= nil then
		-- continuation 追加
		
		-- URL はカテゴリから取得＆連結
		local category = MZ3Data:create(mz3_main_view.get_selected_category_item());
		local category_url = category:get_text('url');
		local url = category_url;
		if url:find("?", 1, false)~=nil then
			-- ? を含む
			url = url .. '&c=' .. continuation;
		else
			-- ? を含まない
			url = url .. '?c=' .. continuation;
		end
		
		-- 既にあれば書き換える
		if next_link_pos == 0 then
			data:set_text("name", '次のページ');
			data:set_text("title", '');
			data:set_text("url", url);
			data:parse_date_line('');
			data:set_access_type(mz3.get_access_type_by_key('GOOGLE_READER_ATOM_LIST'));
			body:add(data.data);
		else
			d = MZ3Data:create(body:get_data(next_link_pos));
			d:set_text('url', url);
		end
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

function on_after_get_end(event_name, serialize_key, body)
	if serialize_key == 'GOOGLE_READER_ATOM_LIST' then
		if reset_body_list_pos ~= 0 then
			-- カーソル位置の復帰
			mz3_main_view.select_body_item(reset_body_list_pos);
		end
		return true;
	end
	
	return false;
end
mz3.add_event_listener('after_get_end', 'greader.on_after_get_end');


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


--- メイン画面に戻る
function on_show_main_view(serialize_key, event_name, data)
	mz3.logger_debug('on_show_main_view : (' .. serialize_key .. ', ' .. event_name .. ')');

	mz3.change_view('main_view');

	return true;
end


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

		-- 詳細画面で表示
		data = mz3_main_view.get_selected_body_item();
		mz3.show_detail_view(data);
		return true;
		--[[
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
		]]
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
	url = body:get_text('url');
	key = "GOOGLE_READER_ATOM_LIST";
	
	if line_has_strings(url, 'c=') == false then
		-- カテゴリ追加
		title = "+" .. name;
		mz3_main_view.append_category(title, url, key);
	end
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- スターを付ける
function on_add_star_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_add_star_menu_item: (' .. serialize_key .. ', ' .. event_name .. ')');

	-- token 取得
	url = 'http://www.google.com/reader/api/0/token';
	referer = '';
	user_agent = nil;
	is_blocking = true;
	access_type = mz3.get_access_type_by_key("GOOGLE_READER_TOKEN");
	status, token = mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, nil, is_blocking);
--	local s = 'status : ' .. status .. '\r\n' .. 'token : [' .. token .. ']';
--	mz3.alert(s);
	
	if status ~= 200 then
		mz3.alert('トークンの取得に失敗しました。');
		return true;
	end
	
	-- POST
	data = MZ3Data:create(data);

	-- POSTパラメータ生成
	post = MZ3PostData:create();

	-- iパラメータはデータから取得する
	post:append_post_body('i=' .. mz3.url_encode(data:get_text('id'), 'utf8'));
	post:append_post_body('&a=' .. mz3.url_encode('user/-/state/com.google/starred', 'utf8'));
	post:append_post_body('&ac=edit');
	post:append_post_body('&T=' .. token);
--	mz3.alert(data:get_text('id'));

	-- 通信開始
	url = 'http://www.google.co.jp/reader/api/0/edit-tag?client=MZ3';
	access_type = mz3.get_access_type_by_key("GOOGLE_READER_ADD_STAR");
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
	if service_type~="GoogleReader" then
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
	if serialize_key == "GOOGLE_READER_ADD_STAR" then
		mz3_main_view.set_info_text("スターつけた！");
	end

	return true;
end
mz3.add_event_listener("post_end", "greader.on_post_end");


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
		
		if event_name == 'popup_body_menu' then
			menu:append_menu("string", "詳細", menu_items.read);
		else
			-- 詳細画面の場合
			menu:append_menu("string", "メイン画面に戻る", menu_items.show_main_view);
		end
		menu:append_menu("string", "ブラウザで開く...", menu_items.open_by_browser);
		menu:append_menu("separator");
		menu:append_menu("string", "スターを付ける...", menu_items.add_star);

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
	
	return false;

end
mz3.add_event_listener("popup_body_menu",  "greader.on_popup_body_menu");


--- 詳細画面の描画
function on_draw_detail_view(event_name, serialize_key, data, dc, cx, cy)

	service_type = mz3.get_service_type(serialize_key);
	if service_type ~= 'GoogleReader' then
		return false;
	end

	-- オブジェクト化
	data = MZ3Data:create(data);
	g = MZ3Graphics:create(dc);

	-- 画面端のマージン
	x_margin = 5;
	y_margin = 5;

	----------------------------------------------
	-- ページ番号
	----------------------------------------------
	g:set_font_size(-1);		-- 小サイズフォント
	line_height = g:get_line_height();
	x = x_margin;
	y = y_margin;

	-- 項目番号(ページ番号風で)
	local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
	local n = list:get_count();
	idx = mz3_main_view.get_selected_body_item_idx();
	text = (idx+1) .. '/' .. n;
	g:set_color("text", "MainBodyListDefaultText");
	x = x_margin;
	y = y_margin;
	w = cx - x - x_margin;
	h = line_height;
	format = DT_NOPREFIX + DT_RIGHT;
	g:draw_text(text, x, y, w, h, format);


	----------------------------------------------
	-- 丈夫の文字列の描画
	----------------------------------------------
	g:set_font_size(1);		-- 大サイズフォント
	line_height = g:get_line_height();
	
	-- 名前
	x = x +x_margin;
	y = y_margin;
	w = cx - x - x_margin;
	h = line_height;
	format = DT_LEFT;
	
	color_text_org = g:set_color("text", "MainBodyListNonreadText");
	color_bk_org = g:set_color("bk", "MainStatusBG");
	text = data:get_text('name');
	g:draw_text(text, x, y, w, h, format);

	-- タイトル
	g:set_color("text", "MainBodyListDefaultText");
	g:set_font_size(1);		-- 大サイズフォント
	y = y + line_height *1.2;
	text = data:get_text('title') .. "\r\n";
	g:draw_text(text, x, y, w, h, format);

	-- 日付
	g:set_color("text", "MainBodyListDefaultText");
	text = data:get_date();
	y = y + line_height *1.2;
	format = DT_RIGHT;
	g:set_font_size(0);		-- 中サイズフォント
	line_height = g:get_line_height();
	g:draw_text(text, x, y, w, h, format);


	----------------------------------------------
	-- 本文
	----------------------------------------------
	
	-- 開始Y座標はクライアント名の下側とアイコンの下側の下の方とする
	local y_source_bottom = y + line_height + y_margin;

	-- 本文
	g:set_font_size(1);		-- 大サイズフォント
	line_height = g:get_line_height();
	g:set_color("text", "MainBodyListNonreadText");
	text = data:get_text_array_joined_text('body');
--	text = text:gsub("\r\n", "");
	text = text:gsub('<img .->', '[画像]');
	text = text:gsub('<br.-/?>', '\r\n');
	text = text:gsub("<.->", "");
	y = y_source_bottom;
	-- 高さは画面の高さからN行引いた程度
--	h = cy * 2 / 3;
	h = cy - line_height * 6;
	x = x_margin;
	w = cx - x - x_margin;
	
	-- 本文用枠描画
	g:draw_rect("border", x, y, w, h, "MainBodyListNonreadText");
	
	border_margin = 5;
	x = x +border_margin;
	y = y +border_margin;
	w = w -border_margin*2;
	h = h -border_margin*2;
	format = DT_WORDBREAK + DT_NOPREFIX + DT_EDITCONTROL + DT_LEFT + DT_END_ELLIPSIS;
	g:draw_text(text, x, y, w, h, format);


	----------------------------------------------
	-- 前後項目の描画
	----------------------------------------------
	local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
	local n = list:get_count();
	local idx = mz3_main_view.get_selected_body_item_idx();

	g:set_font_size(-1);	-- 小サイズフォント
	line_height = g:get_line_height();
	h = line_height;

	local y_lr_text = cy - line_height - y_margin;

	-- 前項目の描画
	if idx > 0 then
		local f = MZ3Data:create(list:get_data(idx-1));
		format = DT_NOPREFIX + DT_LEFT;

		x = x_margin  + x_margin;
		w = cx / 2;
		g:draw_text('≪' .. f:get_text('name'), x, y_lr_text, w, h, format);
	end

	-- 次項目の描画
	if idx < n-1 then
		local f = MZ3Data:create(list:get_data(idx+1));
		format = DT_NOPREFIX + DT_RIGHT;

		x = cx /2;
		w = cx /2  - x_margin;
		g:draw_text(f:get_text('name') .. '≫', x, y_lr_text, w, h, format);
	end

	-- 色を戻す
	g:set_color("text", color_text_org);
	g:set_color("bk", color_bk_org);

	return true;
end
mz3.add_event_listener("draw_detail_view",  "greader.on_draw_detail_view");


--- 詳細画面のキー押下イベント
function on_keydown_detail_view(event_name, serialize_key, data, key)
	mz3.logger_debug('on_keydown_detail_view : (' .. serialize_key .. ', ' .. event_name .. ', ' .. key .. ')');

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='GoogleReader' then
		return false;
	end

	if key == VK_DOWN or key == VK_UP then
		-- 下キー：次の発言
		-- 上キー：前の発言
		local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
		local n = list:get_count();
		local idx = mz3_main_view.get_selected_body_item_idx();
		if key == VK_DOWN then
			-- 次の項目を表示
			if idx < n-1 then
				mz3_main_view.select_body_item(idx+1);
			else
				-- 先頭に戻る
				if n>0 then
					mz3_main_view.select_body_item(0);
				end
			end
		else
			-- 前の項目を表示
			if idx >= 1 then
				mz3_main_view.select_body_item(idx-1);
			else
				-- 末尾に戻る
				mz3_main_view.select_body_item(n-1);
			end
		end
		data = mz3_main_view.get_selected_body_item();
		mz3.show_detail_view(data);
		return true;
	end
	
	if key == VK_RIGHT or key == VK_LEFT then
		-- 左右キー
		return true;
	end
	
	if key == VK_RETURN or key == VK_ESCAPE or key == VK_BACK then
		-- 閉じる
		mz3.change_view('main_view');
		return true;
	end
	
	if key == VK_F2 then
		-- ボディリストのメニューを表示
		on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
		return true;
	end
	
	return true;
end
mz3.add_event_listener("keydown_detail_view", "greader.on_keydown_detail_view");


--- 詳細画面のクリックイベント
function on_click_detail_view(event_name, serialize_key, data, x, y, cx, cy)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='GoogleReader' then
		return false;
	end

	if y > cy/2 then
		-- 下側1/2であれば項目移動
		if x < cx/2 then
			-- 左側：前の項目を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_UP);
		else
			-- 右側：次の項目を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_DOWN);
		end
	else
		-- 閉じる
		mz3.change_view('main_view');
	end
	
	return true;
end
mz3.add_event_listener("click_detail_view", "greader.on_click_detail_view");


--- 詳細画面の右クリックイベント
function on_rclick_detail_view(event_name, serialize_key, data, x, y)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='GoogleReader' then
		return false;
	end

	-- ボディリストのメニューを表示
	on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
	
	return true;
end
mz3.add_event_listener("rclick_detail_view", "greader.on_rclick_detail_view");


--- 詳細画面のマウスホイールイベント
function on_mousewheel_detail_view(event_name, serialize_key, data, z_delta, vkey, x, y)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='GoogleReader' then
		return false;
	end

--	mz3.logger_debug('z_delta : ' .. z_delta);

	if z_delta > 0 then
		-- 上方向：前の項目を表示
		on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_UP);
	else
		-- 下方向：次の項目を表示
		on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_DOWN);
	end

	return true;
end
mz3.add_event_listener("mousewheel_detail_view", "greader.on_mousewheel_detail_view");


--- 詳細画面のポップアップメニュー
function on_popup_detail_menu(event_name, serialize_key, data, wnd)

	-- ボディリストのメニューを表示
	on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
	
	return true;
end
mz3.add_event_listener("popup_detail_menu", "greader.on_popup_detail_menu");

mz3.logger_debug('google_reader.lua end');
