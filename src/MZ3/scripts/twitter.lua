--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : twitter
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('twitter.lua start');
module("twitter", package.seeall)

ID_REPORT_URL_BASE = 36100 -37000;	-- URLを開く

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('Twitter', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('Twitter', 'id_name', 'ID');
mz3_account_provider.set_param('Twitter', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------
--[[
type = mz3.get_access_type_by_key('TWITTER_FRIENDS_TIMELINE');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '<small>%2 \t(%3)</small>');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '<small>%2 \t(%3)</small>');
]]

-- POST用アクセス種別登録
type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- カテゴリ
type:set_service_type('Twitter');							-- サービス種別
type:set_serialize_key('TWITTER_FAVOURINGS_CREATE');		-- シリアライズキー
type:set_short_title('お気に入り登録');						-- 簡易タイトル
type:set_request_method('POST');							-- リクエストメソッド
type:set_request_encoding('utf8');							-- エンコーディング

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- カテゴリ
type:set_service_type('Twitter');							-- サービス種別
type:set_serialize_key('TWITTER_FAVOURINGS_DESTROY');		-- シリアライズキー
type:set_short_title('お気に入り削除');						-- 簡易タイトル
type:set_request_method('POST');							-- リクエストメソッド
type:set_request_encoding('utf8');							-- エンコーディング

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- カテゴリ
type:set_service_type('Twitter');							-- サービス種別
type:set_serialize_key('TWITTER_FRIENDSHIPS_CREATE');		-- シリアライズキー
type:set_short_title('フォロー登録');						-- 簡易タイトル
type:set_request_method('POST');							-- リクエストメソッド
type:set_request_encoding('utf8');							-- エンコーディング

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- カテゴリ
type:set_service_type('Twitter');							-- サービス種別
type:set_serialize_key('TWITTER_FRIENDSHIPS_DESTROY');		-- シリアライズキー
type:set_short_title('フォロー解除');						-- 簡易タイトル
type:set_request_method('POST');							-- リクエストメソッド
type:set_request_encoding('utf8');							-- エンコーディング


--------------------------------------------------
-- [list] タイムライン用パーサ
--
-- http://twitter.com/statuses/friends_timeline.xml
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function twitter_friends_timeline_parser(parent, body, html)
	mz3.logger_debug("twitter_friends_timeline_parser start");
	
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
	status = '';
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<status>') then
			status = line;
		elseif line_has_strings(line, '</status>') then
			status = status .. line;
			
			-- id : status/id
			id = status:match('<id>(.-)</id>');
			
			-- 同一IDがあれば追加しない。
			if id_set[ id ] then
--				mz3.logger_debug('id[' .. id .. '] は既に存在するのでskipする');
			else
				-- data 生成
				data = MZ3Data:create();
				
				data:set_integer('id', id);
				
				type = mz3.get_access_type_by_key('TWITTER_USER');
				data:set_access_type(type);
				
				-- text : status/text
				text = status:match('<text>(.-)</text>');
				text = text:gsub('&amp;', '&');
				text = mz3.decode_html_entity(text);
				data:add_text_array('body', text);
				
				-- source : status/source
				s = status:match('<source>(.-)</source>');
				s = mz3.decode_html_entity(s);
				data:set_text('source', s);
				
				-- name : status/user/screen_name
				user = status:match('<user>.-</user>');
				s = user:match('<screen_name>(.-)</screen_name>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('name', s);
				
				-- author : status/user/name
				s = user:match('<name>(.-)</name>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('author', s);

				-- description : status/user/description
				-- title に入れるのは苦肉の策・・・
				s = user:match('<description>(.-)</description>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('title', s);

				-- owner-id : status/user/id
				data:set_integer('owner_id', user:match('<id>(.-)</id>'));

				-- URL : status/user/url
				url = user:match('<url>(.-)</url>');
				data:set_text('url', url);
				data:set_text('browse_uri', url);

				-- Image : status/user/profile_image_url
				profile_image_url = user:match('<profile_image_url>(.-)</profile_image_url>');
				profile_image_url = mz3.decode_html_entity(profile_image_url);
--				mz3.logger_debug(profile_image_url);

				-- ファイル名のみをURLエンコード
--				int idx = strImage.ReverseFind( '/' );
--				if (idx >= 0) {
--					CString strFileName = strImage.Mid( idx +1 );
--					strFileName = URLEncoder::encode_utf8( strFileName );
--					strImage = strImage.Left(idx + 1);
--					strImage += strFileName;
--				}
				data:add_text_array('image', profile_image_url);

				-- <location>East Tokyo United</location>
				data:set_text('location', mz3.decode_html_entity(user:match('<location>(.-)</location>')));
				-- <followers_count>555</followers_count>
				data:set_integer('followers_count', user:match('<followers_count>(.-)</followers_count>'));
				-- <friends_count>596</friends_count>
				data:set_integer('friends_count', user:match('<friends_count>(.-)</friends_count>'));
				-- <favourites_count>361</favourites_count>
				data:set_integer('favourites_count', user:match('<favourites_count>(.-)</favourites_count>'));
				-- <statuses_count>7889</statuses_count>
				data:set_integer('statuses_count', user:match('<statuses_count>(.-)</statuses_count>'));

				-- updated : status/created_at
				s = status:match('<created_at>(.-)</created_at>');
				data:parse_date_line(s);
				
				-- URL を抽出し、リンクにする
				for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
					data:add_link_list(url, url);
--					mz3.logger_debug(url);
				end

				-- 一時リストに追加
				new_list:add(data.data);
				
				-- data 削除
				data:delete();

			end

			-- 次の status 取得
			status = '';
		elseif status ~= '' then	-- status が空であれば <status> 未発見なので読み飛ばす
			status = status .. line;
		end
	end
	
	-- 生成したデータを出力に反映
	body:merge(new_list);

	-- 新着件数を parent(カテゴリの m_mixi) に設定する
	parent:set_integer('new_count', new_list:get_count());
	
	new_list:delete();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_friends_timeline_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_FRIENDS_TIMELINE", "twitter.twitter_friends_timeline_parser");
mz3.set_parser("TWITTER_FAVORITES", "twitter.twitter_friends_timeline_parser");


--------------------------------------------------
-- [list] DM用パーサ
--
-- http://twitter.com/direct_messages.xml
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function twitter_direct_messages_parser(parent, body, html)
	mz3.logger_debug("twitter_direct_messages_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 'sent.xml' があれば「送信メッセージ」
	category_url = parent:get_text('url');
	is_sent = line_has_strings(category_url, 'sent.xml');

	-- 全消去しない
--	body:clear();
	
	-- 重複防止用の id 一覧を生成。
	id_set = {};
	n = body:get_count();
	for i=0, n-1 do
		id = mz3_data.get_integer(body:get_data(i), 'id');
		id_set[ "" .. id ] = true;
	end

	local t1 = mz3.get_tick_count();
	
	-- 一時リスト
	new_list = MZ3DataList:create();
	
	line = '';
	local line_count = html:get_count();
	direct_message = '';
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<direct_message>') then
			direct_message = line;
		elseif line_has_strings(line, '</direct_message>') then
			direct_message = direct_message .. line;
			
			-- id : direct_message/id
			id = direct_message:match('<id>(.-)</id>');
			
			-- 同一IDがあれば追加しない。
			if id_set[ id ] then
--				mz3.logger_debug('id[' .. id .. '] は既に存在するのでskipする');
			else
				-- data 生成
				data = MZ3Data:create();
				
				data:set_integer('id', id);
				
				type = mz3.get_access_type_by_key('TWITTER_USER');
				data:set_access_type(type);
				
				-- text : direct_message/text
				text = direct_message:match('<text>(.-)</text>');
				text = text:gsub('&amp;', '&');
				text = mz3.decode_html_entity(text);
				data:add_text_array('body', text);
				
				-- updated : status/created_at
				s = direct_message:match('<created_at>(.-)</created_at>');
				data:parse_date_line(s);
				
				-- URL を抽出し、リンクにする
				for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
					data:add_link_list(url, url);
--					mz3.logger_debug(url);
				end
				

				if is_sent then
					user = direct_message:match('<recipient>.-</recipient>');
				else
					user = direct_message:match('<sender>.-</sender>');
				end
				
				-- name : direct_message/user/screen_name
				s = user:match('<screen_name>(.-)</screen_name>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('name', s);
				
				-- author : direct_message/user/name
				s = user:match('<name>(.-)</name>');
				s = mz3.decode_html_entity(s);
				data:set_text('author', s);

				-- description : direct_message/user/description
				-- title に入れるのは苦肉の策・・・
				s = user:match('<description>(.-)</description>');
				s = s:gsub('&amp;', '&');
				s = mz3.decode_html_entity(s);
				data:set_text('title', s);

				-- owner-id : direct_message/user/id
				data:set_integer('owner_id', user:match('<id>(.-)</id>'));

				-- URL : direct_message/user/url
				url = user:match('<url>(.-)</url>');
				data:set_text('url', url);
				data:set_text('browse_uri', url);

				-- Image : direct_message/user/profile_image_url
				profile_image_url = user:match('<profile_image_url>(.-)</profile_image_url>');
				profile_image_url = mz3.decode_html_entity(profile_image_url);
--				mz3.logger_debug(profile_image_url);

				-- ファイル名のみをURLエンコード
--				int idx = strImage.ReverseFind( '/' );
--				if (idx >= 0) {
--					CString strFileName = strImage.Mid( idx +1 );
--					strFileName = URLEncoder::encode_utf8( strFileName );
--					strImage = strImage.Left(idx + 1);
--					strImage += strFileName;
--				}
				data:add_text_array('image', profile_image_url);

--				<location>East Tokyo United</location>
				data:set_text('location', mz3.decode_html_entity(user:match('<location>(.-)</location>')));
--				<followers_count>555</followers_count>
				data:set_integer('followers_count', user:match('<followers_count>(.-)</followers_count>'));
--				<friends_count>596</friends_count>
				data:set_integer('friends_count', user:match('<friends_count>(.-)</friends_count>'));
--				<favourites_count>361</favourites_count>
				data:set_integer('favourites_count', user:match('<favourites_count>(.-)</favourites_count>'));
--				<statuses_count>7889</statuses_count>
				data:set_integer('statuses_count', user:match('<statuses_count>(.-)</statuses_count>'));

				-- 一時リストに追加
				new_list:add(data.data);
				
				-- data 削除
				data:delete();

			end

			-- 次の direct_message 取得
			direct_message = '';
		elseif direct_message ~= '' then	-- direct_message が空であれば <direct_message> 未発見なので読み飛ばす
			direct_message = direct_message .. line;
		end
	end
	
	-- 生成したデータを出力に反映
	body:merge(new_list);
	--TwitterParserBase::MergeNewList(out_, new_list);

	new_list:delete();
	
	-- 新着件数を parent(カテゴリの m_mixi) に設定する
	parent:set_integer('new_count', new_count);
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_direct_messages_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_DIRECT_MESSAGES", "twitter.twitter_direct_messages_parser");


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.retweet               = mz3_menu.regist_menu("twitter.on_retweet_menu_item");

-- 発言内の @xxx 抽出者のTL(5人まで)
menu_items.show_follower_tl = {}
menu_items.show_follower_tl[1]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_1");
menu_items.show_follower_tl[2]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_2");
menu_items.show_follower_tl[3]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_3");
menu_items.show_follower_tl[4]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_4");
menu_items.show_follower_tl[5]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_5");
follower_names = {}

menu_items.update                = mz3_menu.regist_menu("twitter.on_twitter_update");
menu_items.reply                 = mz3_menu.regist_menu("twitter.on_twitter_reply");
menu_items.new_dm                = mz3_menu.regist_menu("twitter.on_twitter_new_dm");
menu_items.create_favourings     = mz3_menu.regist_menu("twitter.on_twitter_create_favourings");
menu_items.destroy_favourings    = mz3_menu.regist_menu("twitter.on_twitter_destroy_favourings");
menu_items.create_friendships    = mz3_menu.regist_menu("twitter.on_twitter_create_friendships");
menu_items.destroy_friendships   = mz3_menu.regist_menu("twitter.on_twitter_destroy_friendships");
menu_items.show_friend_timeline  = mz3_menu.regist_menu("twitter.on_show_friend_timeline");
menu_items.open_home             = mz3_menu.regist_menu("twitter.on_open_home");
menu_items.open_friend_favorites = mz3_menu.regist_menu("twitter.on_open_friend_favorites");
menu_items.open_friend_favorites_by_browser = mz3_menu.regist_menu("twitter.on_open_friend_favorites_by_browser");
menu_items.open_friend_site      = mz3_menu.regist_menu("twitter.on_open_friend_site");



----------------------------------------
-- サービス用関数
----------------------------------------

--- ステータスコード解析
function get_http_status_error_status(http_status)

	if http_status==200 or http_status==304 then
		-- 200 OK: 成功
		-- 304 Not Modified: 新しい情報はない
		return nil;
	elseif http_status==400 then		-- Bad Request:
		return "API の実行回数制限に引っ掛かった、などの理由でリクエストを却下した";
	elseif http_status==401 then		-- Not Authorized:
		return "認証失敗";
	elseif http_status==403 then		-- Forbidden:
		return "権限がないAPI を実行しようとした";
	elseif http_status==404 then		-- Not Found:
		return "存在しない API を実行しようとした、存在しないユーザを引数で指定して API を実行しようとした";
	elseif http_status==500 then		-- Internal Server Error:
		return "Twitter 側で何らかの問題が発生しています";
	elseif http_status==502 then		-- Bad Gateway:
		return "Twitter のサーバが止まっています（メンテ中かもしれません）";
	elseif http_status==503 then		-- Service Unavailable:
		return "Twitter のサーバの負荷が高すぎて、リクエストを裁き切れない状態になっています";
	end

	return nil;
end


----------------------------------------
-- イベントハンドラ
----------------------------------------

-- 「つぶやく」メニュー用ハンドラ
function on_twitter_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「返信」メニュー用ハンドラ
function on_twitter_reply(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- 入力領域にユーザのスクリーン名を追加
	text = mz3_main_view.get_edit_text();
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	-- すでに含まれていれば追加しない
	if text:find("@" .. name, 1, true)~=nil then
		return;
	end
	text = text .. "@" .. name .. " ";
	
	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');

	-- 末尾へ移動
	VK_END = 0x23;
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end

-- 「メッセージ送信」メニュー用ハンドラ
function on_twitter_new_dm(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_DM);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

-- 「お気に入り登録」メニュー用ハンドラ
function on_twitter_create_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/create/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「お気に入り削除」メニュー用ハンドラ
function on_twitter_destroy_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = body:get_integer('id');
	url = "http://twitter.com/favourings/destroy/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「フォローする」メニュー用ハンドラ
function on_twitter_create_friendships(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんをフォローします。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = "http://twitter.com/friendships/create/" .. name .. ".xml";

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

-- 「フォローやめる」メニュー用ハンドラ
function on_twitter_destroy_friendships(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんのフォローを解除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = "http://twitter.com/friendships/destroy/" .. name .. ".xml";

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「@xxx のタイムライン」メニュー用ハンドラ
function on_show_friend_timeline(serialize_key, event_name, data)
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「@xxx のタイムライン」(フォロワー)メニュー用ハンドラ
function on_show_follower_tl_1(serialize_key, event_name, data)	show_follower_tl(1) end
function on_show_follower_tl_2(serialize_key, event_name, data)	show_follower_tl(2) end
function on_show_follower_tl_3(serialize_key, event_name, data)	show_follower_tl(3) end
function on_show_follower_tl_4(serialize_key, event_name, data)	show_follower_tl(4) end
function on_show_follower_tl_5(serialize_key, event_name, data)	show_follower_tl(5) end
function show_follower_tl(num)
	-- 発言内の num 番目の @xxx ユーザの TL を表示する
	name = follower_names[num];
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "http://twitter.com/statuses/user_timeline/" .. name .. ".xml";
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- 「ReTweet」メニュー用ハンドラ
function on_retweet_menu_item(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- エディットコントロールに文字列設定
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	text = "RT @" .. data:get_text('name') .. ": " .. data:get_text_array_joined_text('body');
	text = text:gsub("\r\n", "");
	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

--- 「ホーム」メニュー用ハンドラ
function on_open_home(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name'));
end

--- 「友達のお気に入り」メニュー用ハンドラ
function on_open_friend_favorites_by_browser(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name') .. "/favorites");
end

--- 「友達のサイト」メニュー用ハンドラ
function on_open_friend_site(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end

--- 「友達のお気に入り」メニュー用ハンドラ
function on_open_friend_favorites(serialize_key, event_name, data)

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "@" .. name .. "のお気に入り";
	url = "http://twitter.com/favorites/" .. name .. ".xml";
	key = "TWITTER_FAVORITES";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end

--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)
	if serialize_key=="TWITTER_USER" then
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
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
--	item = item .. "id : " .. data:get_integer('id') .. "\r\n";
--	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";

	if data:get_text('location') ~= '' then
		item = item .. "location : " .. data:get_text('location') .. "\r\n";
	end
	
	if data:get_integer('friends_count') ~= -1 then
		item = item .. "followings : " .. data:get_integer('friends_count')
		            .. ", "
		            .. "followers : " .. data:get_integer('followers_count')
		            .. ", "
		            .. "fav : " .. data:get_integer('favourites_count')
		            .. ", "
		            .. "発言 : " .. data:get_integer('statuses_count')
		            .. "\r\n";
	end
	
	-- ソース
	source = data:get_text('source');
--	item = item .. "source : " .. source .. "\r\n";
	s_url, s_name = source:match("href=\"(.*)\".*>(.*)<");
	if s_url ~= nil then
		item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";
	else
		item = item .. "source : " .. source .. "\r\n";
	end

	mz3.alert(item, data:get_text('name'));

	return true;
end

--- POST 完了イベント
--
-- @param event_name    'post_end'
-- @param serialize_key 完了項目のシリアライズキー
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param wnd           wnd
--
function on_post_end(event_name, serialize_key, http_status)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- Twitter投稿処理完了

	-- ステータスコードチェック
	msg = get_http_status_error_status(http_status);
	if msg ~= nil then
		-- エラーアリなので中断するために true を返す
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		return true;
	end

	-- リクエストの種別に応じてメッセージを表示
	if serialize_key == "TWITTER_NEW_DM" then
		mz3_main_view.set_info_text("メッセージ送信終了");
	elseif serialize_key == "TWITTER_FAVOURINGS_CREATE" then
		mz3_main_view.set_info_text("ふぁぼった！");
	elseif serialize_key == "TWITTER_FAVOURINGS_DESTROY" then
		mz3_main_view.set_info_text("ふぁぼるのやめた！");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_CREATE" then
		mz3_main_view.set_info_text("フォローした！");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_DESTROY" then
		mz3_main_view.set_info_text("フォローやめた！");
	else 
		-- TWITTER_UPDATE
		mz3_main_view.set_info_text("ステータス送信終了");
	end

	-- Wassr への投稿(クロスポスト)
--[[
	if serialize_key == "TWITTER_UPDATE" then

		text = mz3_main_view.get_edit_text();
		msg = "Wassr にも投稿しますか？\r\n";
		msg = msg .. "----\r\n";
		msg = msg .. text .. "\r\n";
		msg = msg .. "----\r\n";
		
		if mz3.confirm(msg, nil, "yes_no") == "yes" then
		
			-- URL 生成
			url = "http://api.wassr.jp/statuses/update.json";
			post = mz3_post_data.create();
			mz3_post_data.append_post_body(post, "status=");
			mz3_post_data.append_post_body(post, mz3.url_encode(text, 'utf8'));
			mz3_post_data.append_post_body(post, "&source=");
			mz3_post_data.append_post_body(post, mz3.get_app_name());
			
			-- 通信開始
			access_type = mz3.get_access_type_by_key("WASSR_UPDATE");
			referer = '';
			user_agent = nil;
			mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
			return true;
		end
	end
]]

	-- 入力値を消去
	mz3_main_view.set_edit_text("");
	
	return true;
end

--- GET 完了イベント
--
-- @param event_name    'get_end'
-- @param serialize_key 完了項目のシリアライズキー
-- @param http_status   HTTP Status Code (200, 404, etc...)
-- @param wnd           wnd
--
function on_get_end(event_name, serialize_key, http_status)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- ステータスコードチェックのみ行う
	msg = get_http_status_error_status(http_status);
	if msg ~= nil then
		-- エラーアリなので中断するために true を返す
		mz3.logger_error(msg);
		mz3_main_view.set_info_text(msg);
		mz3.alert("サーバエラー(" .. http_status .. ") : " .. msg);
		return true;
	end
	-- エラーなしなので続行する(後続の解析処理等を継続)

	return false;
end

--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key~="TWITTER_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	submenu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.read);

	menu:append_menu("separator");

	menu:append_menu("string", "つぶやく", menu_items.update);

	name = body:get_text('name');

	menu:append_menu("string", "@" .. name .. " さんに返信", menu_items.reply);
	menu:append_menu("string", "@" .. name .. " さんにメッセージ送信", menu_items.new_dm);

	menu:append_menu("separator");

	menu:append_menu("string", "ReTweet...", menu_items.retweet);
	
	-- カテゴリが「timeline」のみ、お気に入り登録を表示
	category_access_type = mz3_main_view.get_selected_category_access_type();
	category_key = mz3.get_serialize_key_by_access_type(category_access_type);
	if category_key == "TWITTER_FRIENDS_TIMELINE" then
		menu:append_menu("string", "お気に入り登録（ふぁぼる）", menu_items.create_favourings);
	end
	
	-- カテゴリが「お気に入り」のみ、お気に入り削除を表示
	if category_key == "TWITTER_FAVORITES" then
		menu:append_menu("string", "お気に入り削除", menu_items.destroy_favourings);
	end

	menu:append_menu("separator");

	menu:append_menu("string", "@" .. name .. " のタイムライン", menu_items.show_friend_timeline);
	
	-- 発言内の @XXX を抽出し、メニュー化
	body_text = body:get_text_array_joined_text('body');
	i = 1;
	for f_name in body_text:gmatch("@([0-9a-zA-Z_]+)") do
		mz3.logger_debug(f_name);
		-- Lua 変数に名前を保存しておく
		follower_names[i] = f_name;
		menu:append_menu("string", "@" .. f_name .. " のタイムライン", menu_items.show_follower_tl[i]);
		-- 最大5人までサポート
		i = i+1;
		if i>5 then
			break;
		end
	end

--	menu:append_menu("separator");
	
	submenu:append_menu("string", "@" .. name .. " をフォローする", menu_items.create_friendships);
	submenu:append_menu("string", "@" .. name .. " のフォローをやめる", menu_items.destroy_friendships);
	submenu:append_menu("string", "@" .. name .. " のホームをブラウザで開く", menu_items.open_home);
	submenu:append_menu("string", "@" .. name .. " のお気に入りをブラウザで開く", menu_items.open_friend_favorites_by_browser);
	submenu:append_menu("string", "@" .. name .. " のお気に入り", menu_items.open_friend_favorites);
	menu:append_submenu("その他", submenu);

	-- URL が空でなければ「サイト」を追加
	url = body:get_text('url');
	if url~=nil and url:len()>0 then
		submenu:append_menu("string", "@" .. name .. " のサイトをブラウザで開く", menu_items.open_friend_site);
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
	submenu:delete();
	
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
	if services:find(' Twitter', 1, true) ~= nil then

		-- Twitterタブ追加
		local tab = MZ3GroupItem:create("Twitter");
		tab:append_category("タイムライン", "TWITTER_FRIENDS_TIMELINE", "http://twitter.com/statuses/friends_timeline.xml");
		tab:append_category("返信一覧", "TWITTER_FRIENDS_TIMELINE", "http://twitter.com/statuses/replies.xml");
		tab:append_category("お気に入り", "TWITTER_FAVORITES", "http://twitter.com/favorites.xml");
		tab:append_category("受信メッセージ", "TWITTER_DIRECT_MESSAGES", "http://twitter.com/direct_messages.xml");
		tab:append_category("送信メッセージ", "TWITTER_DIRECT_MESSAGES", "http://twitter.com/direct_messages/sent.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end

----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");

-- ボディリストのポップアップメニュー表示イベントハンドラ登録
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");

-- POST完了イベントハンドラ登録
mz3.add_event_listener("post_end", "twitter.on_post_end");
mz3.add_event_listener("get_end",  "twitter.on_get_end");

-- デフォルトのグループリスト生成
mz3.add_event_listener("creating_default_group", "twitter.on_creating_default_group", false);

mz3.logger_debug('twitter.lua end');
