--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
			msg = "gooホームひとこと投稿アドレスは下記の形式です。 \n"
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


--- Twitterスタイルのボタン名称の更新
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'GOOHOME_QUOTE_UPDATE' then
		return true, 'ひとこと';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "goohome.on_update_twitter_update_button");


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
	if service_type=='gooHome' then
		return true, VIEW_STYLE_TWITTER;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "goohome.on_get_view_style");


----------------------------------------
-- パーサのロード＆登録
----------------------------------------

function get_json_value(line, key)
	local json_key = "\"" .. key .. "\":";
	if line_has_strings(line, json_key) then
		return line:match(json_key .. "[^\"]*\"([^\"]*)\"");
	else
		return nil;
	end
end

--------------------------------------------------
-- [list] gooホーム 友達・注目の人のひとこと一覧パーサ
--
-- http://home.goo.ne.jp/api/quote/quotes/friends/json
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function quote_quotes_friends_parser(parent, body, html)
	mz3.logger_debug("quote_quotes_friends_parser start");
	local t1 = mz3.get_tick_count();
	
	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);
	parent = MZ3Data:create(parent);

	-- 全消去
	body:clear();

	-- TODO 新着マージ＋重複防止処理
--[[
	-- 新規に追加するデータ群
	CMixiDataList new_list;

	-- 重複防止用の id 一覧を生成。
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}
]]

	-- オブジェクト生成
	local data = MZ3Data:create();
	type_user = mz3.get_access_type_by_key("GOOHOME_USER");
	data:set_access_type(type_user);

	-- ひとことAPIのjsonは行単位に要素があるので、行単位にパースする
	local line_count = html:get_count();
	local in_quote = false;
	
	for i=0, line_count-1 do
		line = html:get_at(i);
		if line_has_strings(line, "\"user\": {") then
--			mz3.logger_debug(i .. " : " .. line);
			if in_quote then
				-- 一つ前の解析結果を登録
				body:add(data.data);
				data:delete();
				
				-- 次の要素用データを生成
				data = MZ3Data:create();
				data:set_access_type(type_user);
			end
			in_quote = true;
		else
			if in_quote then
				
				local v;
				
				-- nickname
				v = get_json_value(line, "nickname");
				if v ~= nil then
					data:set_text("name", v);
				end
				
				-- goo_id
				v = get_json_value(line, "goo_id");
				if v ~= nil then
					data:set_text("goo_id", v);
				end
				
				-- profile_image_url
				v = get_json_value(line, "profile_image_url");
				if v ~= nil then
					data:add_text_array("image", v);
				end
				
				-- profile_url
				v = get_json_value(line, "profile_url");
				if v ~= nil then
					data:set_text("browse_uri", v);
					data:set_text("url", v);
				end
				-- friends
				v = get_json_value(line, "friends");
				if v ~= nil then
					data:set_integer("friends", v);
				end
				
				-- text : 複数行の可能性があるのでここでパース
				if line_has_strings(line, "\"text\":") then
					-- " 以降の文字列取得
					local after_dq = line:match(":[^\"]*\"(.*)");
					--mz3.logger_debug(after_dq);
					
					if after_dq:find("\"", 1, true) ~= nil then
						-- 終了
						local text = after_dq:match("(.*)\"");
						data:add_text_array("body", text);
					else
						-- " が見つかるまで取得する
						data:add_text_array("body", after_dq);
						data:add_text_array("body", "\r\n");
						i = i+1;
						while i<line_count-1 do
							line = html:get_at(i);
							if line:find("\"", 1, true) ~= nil then
								-- 終了
								local text = line:match("(.*)\"");
								data:add_text_array("body", text);
								break;
							else
								data:add_text_array("body", line);
								data:add_text_array("body", "\r\n");
							end
							
							i = i+1;
						end
					end
				end

				-- favorited
				v = get_json_value(line, "favorited");
				if v ~= nil then
					data:set_integer("favorited", v=="false" and 0 or 1);	-- 3項演算子
				end
				-- favorites
				v = get_json_value(line, "favorites");
				if v ~= nil then
					data:set_integer("favorites", v);
				end
				-- comments
				v = get_json_value(line, "comments");
				if v ~= nil then
					data:set_integer("comments", v);

					-- コメント数をボディの末尾につける
					data:add_text_array("body", "(" .. v .. ")");
				end
				-- created_at
				v = get_json_value(line, "created_at");
				if v ~= nil then
					data:parse_date_line(v);
				end
				-- id
				v = get_json_value(line, "id");
				if v ~= nil then
					data:set_text("id", v);

					-- 閲覧URLとしてひとことのURLを構築して設定しておく
					-- http://home.goo.ne.jp/quote/user/{goo_id}/detail/{id}?cnt={comments}
					url = string.format("http://home.goo.ne.jp/quote/user/%s/detail/%s?cnt=%d",
							data:get_text("goo_id"),
							v,
							data:get_integer("comments",0));
					data:set_text("url", url);
					data:set_text("browse_uri", url);
				end
			end
		end
	end

	-- TODO 新着マージ＋重複防止処理
	if in_quote then
		body:add(data.data);
	end
--[[

	-- 生成したデータを出力に反映
	TwitterParserBase::MergeNewList(out_, new_list);
]]

	-- オブジェクト削除
	data:delete();

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("quote_quotes_friends_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");


mz3.logger_debug('goohome.lua end');
