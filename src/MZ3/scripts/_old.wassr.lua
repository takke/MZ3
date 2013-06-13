--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : wassr
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('wassr.lua start');
module("wassr", package.seeall)

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('Wassr', false);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('Wassr', 'id_name', 'ID');
mz3_account_provider.set_param('Wassr', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------
-- TODO

----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("wassr.on_read_menu_item");
menu_items.update                = mz3_menu.regist_menu("wassr.on_wassr_update");


----------------------------------------
-- サービス用関数
----------------------------------------


----------------------------------------
-- パーサ
----------------------------------------

--------------------------------------------------
-- [list] タイムライン用パーサ
--
-- http://api.wassr.jp/statuses/friends_timeline.xml
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function wassr_friends_timeline_parser(parent, body, html)
	mz3.logger_debug("wassr_friends_timeline_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去しない
--	body:clear();
	
	-- 重複防止用の id 一覧を生成。
	id_set = {};
	n = body:get_count();
	for i=0, n-1 do
		id = mz3_data.get_integer(body:get_data(i), 'id');
--		mz3.logger_debug(id);
		id_set[ "" .. id ] = true;
	end

	local t1 = mz3.get_tick_count();
	
	-- 一時リスト
	new_list = MZ3DataList:create();
	
	line = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = line .. html:get_at(i);
	end
	
	while true do
		status, after = line:match('<status>(.-)</status>(.*)$');
		if status == nil or after == nil then
			break;
		end
		line = after;

		-- id : status/id
		id = status:match('<id>(.-)</id>');
		
		-- 同一IDがあれば追加しない。
		if id_set[ id ] then
--			mz3.logger_debug('id[' .. id .. '] は既に存在するのでskipする');
		else
			-- data 生成
			data = MZ3Data:create();
			
			data:set_integer('id', id);
			
			type = mz3.get_access_type_by_key('WASSR_USER');
			data:set_access_type(type);
			
			-- text : status/text
			text = status:match('<text>(.-)</text>');
			text = text:gsub('&amp;', '&');
			text = mz3.decode_html_entity(text);
			data:add_text_array('body', text);
			
			-- name : status/user/screen_name
			user = status:match('<user>.-</user>');
			s = user:match('<screen_name>(.-)</screen_name>');
			s = s:gsub('&amp;', '&');
			s = mz3.decode_html_entity(s);
			data:set_text('name', s);
			
			-- owner-id : status/user/id
			data:set_integer('owner_id', status:match('<user_login_id>(.-)</user_login_id>'));

			-- URL : status/user/url
			url = user:match('<url>(.-)</url>');
			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- Image : status/user/profile_image_url
			profile_image_url = user:match('<profile_image_url>(.-)</profile_image_url>');
			profile_image_url = mz3.decode_html_entity(profile_image_url);
--			mz3.logger_debug(profile_image_url);

			-- ファイル名のみをURLエンコード
--			int idx = strImage.ReverseFind( '/' );
--			if (idx >= 0) {
--				CString strFileName = strImage.Mid( idx +1 );
--				strFileName = URLEncoder::encode_utf8( strFileName );
--				strImage = strImage.Left(idx + 1);
--				strImage += strFileName;
--			}
			data:add_text_array('image', profile_image_url);

			-- updated : status/epoch
			s = status:match('<epoch>(.-)</epoch>');
			data:parse_date_line(s);
			
			-- URL を抽出し、リンクにする
			for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
				data:add_link_list(url, url);
			end

			-- 一時リストに追加
			new_list:add(data.data);
			
			-- data 削除
			data:delete();
		end
	end
	
	-- 生成したデータを出力に反映
	body:merge(new_list);

	new_list:delete();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("wassr_friends_timeline_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("WASSR_FRIENDS_TIMELINE", "wassr.wassr_friends_timeline_parser");


----------------------------------------
-- イベントハンドラ
----------------------------------------


--- BASIC 認証設定
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Wassr' then
		id       = mz3_account_provider.get_value('Wassr', 'id');
		password = mz3_account_provider.get_value('Wassr', 'password');
		
		if id=='' or password=='' then
			mz3.alert('ログイン設定画面でユーザIDとパスワードを設定してください');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "wassr.on_set_basic_auth_account");


--- Twitter風書き込みモードの初期化
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Wassr' then
		-- モード変更
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('WASSR_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "wassr.on_reset_twitter_style_post_mode");


--- Twitterスタイルのボタン名称の更新
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'WASSR_UPDATE' then
		return true, 'Wassr';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "wassr.on_update_twitter_update_button");


-- 「つぶやく」メニュー用ハンドラ
function on_wassr_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_WASSR_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="WASSR_USER" then
		-- 全文表示
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end
mz3.add_event_listener("dblclk_body_list", "wassr.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "wassr.on_body_list_click");


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
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. data:get_integer('id') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
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
	if services:find(' Wassr', 1, true) ~= nil then

		-- Wassrタブ追加
		local tab = MZ3GroupItem:create("Wassr");
		tab:append_category("タイムライン", "WASSR_FRIENDS_TIMELINE");
		tab:append_category("返信一覧", "WASSR_FRIENDS_TIMELINE", "http://api.wassr.jp/statuses/replies.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end
mz3.add_event_listener("creating_default_group", "wassr.on_creating_default_group", false);


--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="WASSR_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.read);

	menu:append_menu("separator");

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
mz3.add_event_listener("popup_body_menu",  "wassr.on_popup_body_menu");


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
	if service_type=='Wassr' then
		return true, VIEW_STYLE_TWITTER;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "wassr.on_get_view_style");


--- 更新ボタン押下イベント
--
-- @param event_name    'click_update_button'
-- @param serialize_key Twitter風書き込みモードのシリアライズキー
--
function on_click_update_button(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Wassr" then
		return false;
	end

	-- 入力文字列を取得
	text = mz3_main_view.get_edit_text();

	-- 未入力時の処理
	if text == '' then
		-- 最新取得
		mz3_main_view.retrieve_category_item();
		return true;
	end

	-- 確認
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'WASSR_UPDATE' then
		msg = 'Wassrで発言します。 \n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	if serialize_key == 'WASSR_UPDATE' then
		-- 通常の投稿は共通化

		-- クロスポスト管理データ初期化
		mz3.init_cross_post_info("wassr");

		return do_post_to_wassr(text);
	end

	return true;
end
mz3.add_event_listener("click_update_button", "wassr.on_click_update_button");


--- Wassr に投稿する
function do_post_to_wassr(text)

	serialize_key = 'WASSR_UPDATE'

	-- ヘッダーの設定
	post = MZ3PostData:create();

	-- POST先URL設定
	url = "http://api.wassr.jp/statuses/update.json";

	-- POST パラメータを設定
	post = MZ3PostData:create();
	post:append_post_body("status=");
	post:append_post_body(mz3.url_encode(text, 'utf8'));
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
		footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
		post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
	end
	post:append_post_body("&source=");
	post:append_post_body(mz3.get_app_name());
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key("WASSR_UPDATE");
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
	if service_type~="Wassr" then
		return false;
	end

	-- 投稿処理完了
	
	-- ステータスコードチェック
	if http_status == 200 then
		-- OK
		mz3_main_view.set_info_text("Wassrで発言しました");
	else
		-- エラーアリなので中断するために true を返す
		msg = "サーバエラー(" .. http_status .. ")";
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		return true;
	end

	-- クロスポスト
	if serialize_key == "WASSR_UPDATE" then
		if mz3.do_cross_post() then
			return true;
		end
	end

	-- 入力値を消去
	mz3_main_view.set_edit_text("");
	
	return true;
end
mz3.add_event_listener("post_end", "wassr.on_post_end");


mz3.logger_debug('wassr.lua end');
