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


--- MZ3内部IDをTwitterIDに変換する
function id2realid(n)
	return n+2147483648;
end

--- TwitterIDをMZ3内部IDに変換する
function realid2id(n)
	return n-2147483648;
end


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

type = MZ3AccessTypeInfo:create();
type:set_info_type('post');									-- カテゴリ
type:set_service_type('Twitter');							-- サービス種別
type:set_serialize_key('TWITTER_UPDATE_WITH_TWITPIC');		-- シリアライズキー
type:set_short_title('twitpic投稿');						-- 簡易タイトル
type:set_request_method('POST');							-- リクエストメソッド
type:set_request_encoding('utf8');							-- エンコーディング

-- ファイル名
twitpic_target_file = nil;


--- 1ユーザの追加
function my_add_new_user(new_list, status, id)
--	mz3.logger_debug("my_add_new_user start");

	-- data 生成
	data = MZ3Data:create();
	
	-- id : status/id
--	local id = realid2id(status:match('<id>([^<]*)</id>'));
	data:set_integer('id', id);
	type = mz3.get_access_type_by_key('TWITTER_USER');
	data:set_access_type(type);
	
	-- updated : status/created_at
	local s = status:match('<created_at>([^<]*)</created_at>');
	data:parse_date_line(s);
	
	-- text : status/text
	text = status:match('<text>([^<]*)</text>');
	text = text:gsub('&amp;', '&');
	text = mz3.decode_html_entity(text);
	data:add_text_array('body', text);
	
	-- @takke などがあればバイブする
--	if line_has_strings(text, "@" .. my_twitter_name) then
--		mz3.set_vib_status(true);
--		mz3.set_vib_status(false);
--	end
	
	-- URL を抽出し、リンクにする
	for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+") do
		data:add_link_list(url, url);
--		mz3.logger_debug(url);
	end

	-- source : status/source
	s = status:match('<source>([^<]*)</source>');
	s = mz3.decode_html_entity(s);
	data:set_text('source', s);
	
	-- name : status/user/screen_name
	user = status:match('<user>.-</user>');
	s = user:match('<screen_name>([^<]*)</screen_name>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('name', s);
	
	-- author : status/user/name
	s = user:match('<name>([^<]*)</name>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('author', s);

	-- description : status/user/description
	-- title に入れるのは苦肉の策・・・
	s = user:match('<description>([^<]*)</description>');
	s = s:gsub('&amp;', '&');
	s = mz3.decode_html_entity(s);
	data:set_text('title', s);

	-- owner-id : status/user/id
	data:set_integer('owner_id', user:match('<id>([^<]*)</id>'));

	-- URL : status/user/url
	url = user:match('<url>([^<]*)</url>');
	data:set_text('url', url);
	data:set_text('browse_uri', url);

	-- Image : status/user/profile_image_url
	profile_image_url = user:match('<profile_image_url>([^<]*)</profile_image_url>');
	profile_image_url = mz3.decode_html_entity(profile_image_url);
--	mz3.logger_debug(profile_image_url);

	-- ファイル名のみをURLエンコード
--	int idx = strImage.ReverseFind( '/' );
--	if (idx >= 0) {
--		CString strFileName = strImage.Mid( idx +1 );
--		strFileName = URLEncoder::encode_utf8( strFileName );
--		strImage = strImage.Left(idx + 1);
--		strImage += strFileName;
--	}
	data:add_text_array('image', profile_image_url);

	-- <location>East Tokyo United</location>
	data:set_text('location', mz3.decode_html_entity(user:match('<location>([^<]*)</location>')));
	-- <followers_count>555</followers_count>
	data:set_integer('followers_count', user:match('<followers_count>([^<]*)</followers_count>'));
	-- <friends_count>596</friends_count>
	data:set_integer('friends_count', user:match('<friends_count>([^<]*)</friends_count>'));
	-- <favourites_count>361</favourites_count>
	data:set_integer('favourites_count', user:match('<favourites_count>([^<]*)</favourites_count>'));
	-- <statuses_count>7889</statuses_count>
	data:set_integer('statuses_count', user:match('<statuses_count>([^<]*)</statuses_count>'));

	-- 一時リストに追加
	new_list:add(data.data);
	
	-- data 削除
	data:delete();

--	mz3.logger_debug("my_add_new_user end");
end
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

	my_twitter_name = mz3_account_provider.get_value('Twitter', 'id');

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
	local i=0;
	local id=0;
	while i<line_count do
		line = html:get_at(i);
--		mz3.logger_debug('line:' .. i);
		
		if line_has_strings(line, '<status>') then
			status = line;
			
			-- </status> まで取得する
			-- ただし、同一IDがあればskipする
			i = i+1;
			i_in_status = 0;
			while i<line_count do
				line = html:get_at(i);
				status = status .. line;
				
				-- 同一 skip は先にやる
				if i_in_status<3 and line_has_strings(line, '<id>') then
					-- id : status/id
					id = realid2id(line:match('<id>(.-)</id>'));
					-- 同一IDがあれば追加しない。
					if id_set[ "" .. id ] then
						mz3.logger_debug('id[' .. id .. '] は既に存在するのでskipする');
						i = i+1;
						while i<line_count do
							line = html:get_at(i);
							
							if line_has_strings(line, '</status>') then
								break;
							end
							i = i+1;
						end
--						mz3.logger_debug('new i:' .. i);

						status = '';
						break;
					end
				elseif i_in_status>35 and line_has_strings(line, '</status>') then
					-- </status> 発見したのでここまでの status を解析して追加
					my_add_new_user(new_list, status, id);
					break;
				end
				i = i+1;
				i_in_status = i_in_status+1;
			end

			-- 次の status 取得
			status = '';
		end
		i = i+1;
	end
	
	-- 生成したデータを出力に反映
	if mz3.get_app_name()=="MZ3" then
		body:merge(new_list, 100);
	else
		body:merge(new_list, 1000);
	end

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
menu_items.show_user_info        = mz3_menu.regist_menu("twitter.on_show_user_info");
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
menu_items.update_with_twitpic   = mz3_menu.regist_menu("twitter.on_twitter_update_with_twitpic");
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

--- BASIC 認証設定
function on_set_basic_auth_account(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Twitter' then
		if serialize_key=='TWITTER_UPDATE_WITH_TWITPIC' then
			-- twitpic は BASIC 認証不要
			return true, 0, '', '';
		end
		id       = mz3_account_provider.get_value('Twitter', 'id');
		password = mz3_account_provider.get_value('Twitter', 'password');
		
		if id=='' or password=='' then
			mz3.alert('ログイン設定画面でユーザIDとパスワードを設定してください');
			return true, 1;
		end
		mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
		return true, 0, id, password;
	end
	return false;
end
mz3.add_event_listener("set_basic_auth_account", "twitter.on_set_basic_auth_account");


--- Twitter風書き込みモードの初期化
function on_reset_twitter_style_post_mode(event_name, serialize_key)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Twitter' then
		-- モード変更
		mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE'));
		
		return true;
	end
	return false;
end
mz3.add_event_listener("reset_twitter_style_post_mode", "twitter.on_reset_twitter_style_post_mode");


--- Twitterスタイルのボタン名称の更新
function on_update_twitter_update_button(event_name, serialize_key)
	if serialize_key == 'TWITTER_NEW_DM' then
		return true, 'DM';
	elseif serialize_key == 'TWITTER_UPDATE' then
		return true, '更新';
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		return true, 'TwitPic';
	end
	
	return false;
end
mz3.add_event_listener("update_twitter_update_button", "twitter.on_update_twitter_update_button");


--- 「つぶやく」メニュー用ハンドラ
function on_twitter_update(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE'));

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end

--- 「写真を投稿」メニュー用ハンドラ
function on_twitter_update_with_twitpic(serialize_key, event_name, data)

	-- ファイル選択画面
	OFN_EXPLORER = 0x00080000;
	OFN_FILEMUSTEXIST = 0x00001000;
	flags = OFN_EXPLORER + OFN_FILEMUSTEXIST;
	filter = "JPEGﾌｧｲﾙ (*.jpg)%0*.jpg;*.jpeg%0%0";
--	filter = "JPEGﾌｧｲﾙ (*.jpg)%0*.jpg;*.jpeg%0すべてのﾌｧｲﾙ (*.*)%0*.*%0%0";
	twitpic_target_file = mz3.get_open_file_name(mz3_main_view.get_wnd(),
												 "JPEGﾌｧｲﾙを開く...", 
												 filter,
												 flags,
												 "");
--	mz3.alert(twitpic_target_file);
	if twitpic_target_file == nil then
		-- 中止
		return;
	end

	-- モード変更
	mz3_main_view.set_post_mode(mz3.get_access_type_by_key('TWITTER_UPDATE_WITH_TWITPIC'));

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end


--- 「返信」メニュー用ハンドラ
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
	if use_twitter_reply_with_dot then
		text = text .. ".@" .. name .. " ";
	else
		text = text .. "@" .. name .. " ";
	end
	
	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');

	-- 末尾へ移動
	VK_END = 0x23;
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end


--- 「メッセージ送信」メニュー用ハンドラ
function on_twitter_new_dm(serialize_key, event_name, data)
	-- モード変更
	mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_DM);

	-- モード変更反映(ボタン名称変更)
	mz3_main_view.update_control_status();

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end


--- 「お気に入り登録」メニュー用ハンドラ
function on_twitter_create_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = id2realid(body:get_integer('id'));
	url = "http://twitter.com/favourings/create/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- 「お気に入り削除」メニュー用ハンドラ
function on_twitter_destroy_favourings(serialize_key, event_name, data)
	-- URL 生成
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	id = id2realid(body:get_integer('id'));
	url = "http://twitter.com/favourings/destroy/" .. id .. ".xml";

	-- 通信開始
	key = "TWITTER_FAVOURINGS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- 「フォローする」メニュー用ハンドラ
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


--- 「フォローやめる」メニュー用ハンドラ
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
mz3.add_event_listener("dblclk_body_list", "twitter.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "twitter.on_body_list_click");


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
--	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. id2realid(data:get_integer('id')) .. "\r\n";

	mz3.alert(item, data:get_text('name'));

	return true;
end


--- @userについて
function on_show_user_info(serialize_key, event_name, data)
	mz3.logger_debug('on_show_user_info : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	item = '';
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
--	item = item .. data:get_date() .. "\r\n";
--	item = item .. "id : " .. id2realid(data:get_integer('id')) .. "\r\n";
	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";

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
	s_url, s_name = source:match("href=\"(.-)\".*>(.*)<");
	if s_url ~= nil then
		item = item .. "source : " .. s_name .. " (" .. s_url .. ")\r\n";
	else
		item = item .. "source : " .. source .. "\r\n";
	end

	mz3.alert(item, data:get_text('name'));

	return true;
end


--- Twitter に投稿する
function do_post_to_twitter(text)

	serialize_key = 'TWITTER_UPDATE'

	-- ヘッダーの設定
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST パラメータを設定
	post:append_post_body('status=');
	post:append_post_body(mz3.url_encode(text, 'utf8'));
	
	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
		if text:find("RT @", 1, false)~=nil then
			-- RTが含まれているので追加しない
		else
			footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
			post:append_post_body(mz3.url_encode(footer_text, 'utf8'));
		end
	end
	post:append_post_body('&source=');
	post:append_post_body(mz3.get_app_name());

	-- POST先URL設定
	url = 'http://twitter.com/statuses/update.xml';
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end


--- 更新ボタン押下イベント
--
-- @param event_name    'click_update_button'
-- @param serialize_key Twitter風書き込みモードのシリアライズキー
--
function on_click_update_button(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~="Twitter" then
		return false;
	end

	-- 入力文字列を取得
	text = mz3_main_view.get_edit_text();

	-- 未入力時の処理
	if text == '' then
		if serialize_key == 'TWITTER_NEW_DM' then
			-- 未入力はNG => 何もせずに終了
			return true;
		elseif serialize_key == 'TWITTER_UPDATE' then
			-- 最新取得
			mz3_main_view.retrieve_category_item();
			return true;
		elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
			-- 未入力もOK => 続行
		end
	end

	-- 確認
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'TWITTER_NEW_DM' then
		msg = data:get_text('name') .. ' さんに以下のメッセージを送信します。\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE' then
		msg = 'Twitterで発言します。\n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		msg = 'twitpicで投稿します。\n'
		   .. '---- 発言 ----\n'
		   .. text .. '\n'
		   .. '---- ファイル ----\n'
		   .. twitpic_target_file .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	if serialize_key == 'TWITTER_UPDATE' then
		-- 通常の投稿は共通化
		
		-- クロスポスト管理データ初期化
		mz3.init_cross_post_info("twitter");

		return do_post_to_twitter(text);
	end

	-- ヘッダーの設定
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST パラメータを設定
	if serialize_key == 'TWITTER_NEW_DM' then
		post:append_post_body('text=');
		post:append_post_body(mz3.url_encode(text, 'utf8'));
		post:append_post_body('&user=');
		post:append_post_body(data:get_integer('owner_id'));
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		id       = mz3_account_provider.get_value('Twitter', 'id');
		password = mz3_account_provider.get_value('Twitter', 'password');

		post:set_content_type('multipart/form-data; boundary=---------------------------7d62ee108071e' .. '\r\n');
		
		-- id
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="username"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.url_encode(id, 'utf8') .. '\r\n');
		
		-- password
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="password"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.url_encode(password, 'utf8') .. '\r\n');
		
		-- メッセージはUTF8で埋め込む
		-- message
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="message"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(text, 'sjis', 'utf8'));
		-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
		if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
			footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
			post:append_post_body(mz3.convert_encoding(footer_text, 'sjis', 'utf8'));
		end
		post:append_post_body('\r\n');

		-- media (image binary)
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="media"; filename="photo.jpg"' .. '\r\n');
		post:append_post_body('Content-Type: image/jpeg' .. '\r\n');
		post:append_post_body('\r\n');
		
		if post:append_file(twitpic_target_file) == false then
			mz3.alert('画像ファイルの読み込みに失敗しました');
			return true;
		end

		post:append_post_body('\r\n');
		
		-- end of post data
		post:append_post_body('-----------------------------7d62ee108071e--' .. '\r\n');

	end

	-- POST先URL設定
	url = '';
	if serialize_key == 'TWITTER_NEW_DM' then
		url = 'http://twitter.com/direct_messages/new.xml';
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		url = 'http://twitpic.com/api/uploadAndPost';
	end
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key(serialize_key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

	return true;
end
mz3.add_event_listener("click_update_button", "twitter.on_click_update_button");


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
		mz3_main_view.set_info_text("メッセージ送信しました");
	elseif serialize_key == "TWITTER_FAVOURINGS_CREATE" then
		mz3_main_view.set_info_text("ふぁぼった！");
	elseif serialize_key == "TWITTER_FAVOURINGS_DESTROY" then
		mz3_main_view.set_info_text("ふぁぼるのやめた！");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_CREATE" then
		mz3_main_view.set_info_text("フォローした！");
	elseif serialize_key == "TWITTER_FRIENDSHIPS_DESTROY" then
		mz3_main_view.set_info_text("フォローやめた！");
	elseif serialize_key == "TWITTER_UPDATE_WITH_TWITPIC" then
		mz3_main_view.set_info_text("twitpic 画像投稿完了");
	else
		-- TWITTER_UPDATE
--		mz3_main_view.set_info_text("ステータス送信終了");
		mz3_main_view.set_info_text("発言しました");
	end

	-- クロスポスト
	if serialize_key == "TWITTER_UPDATE" then
		if mz3.do_cross_post() then
			return true;
		end
	end

	-- twitpic の 写真投稿のみを行う場合は下記のコードで POST する
--[[
	if serialize_key == "TWITTER_UPDATE_WITH_TWITPIC" then
		f = io.open(filename, 'r');
		file = f:read('*a');
		f:close();
		
		-- 投稿したファイルのURL取得
		twitpic_url = file:match('<mediaurl>(.-)<');
		if twitpic_url == '' then
			mz3.alert('投稿に失敗した可能性があります。再度投稿してください。');
			return true;
		end
--		mz3.alert(twitpic_url);
		
		-- 再度POSTする
		text = mz3_main_view.get_edit_text();
		do_post_to_twitter(twitpic_url .. ' - ' .. text);
		
		return true;
	end
]]

	-- 入力値を消去
	mz3_main_view.set_edit_text("");
	
	return true;
end
mz3.add_event_listener("post_end", "twitter.on_post_end");


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
mz3.add_event_listener("get_end",  "twitter.on_get_end");


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
	
	name = body:get_text('name');

	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "全文を読む...", menu_items.read);
	menu:append_menu("string", "@" .. name .. " さんについて...", menu_items.show_user_info);

	menu:append_menu("separator");

	menu:append_menu("string", "つぶやく", menu_items.update);
	menu:append_menu("string", "写真を投稿(twitpic)...", menu_items.update_with_twitpic);

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
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");


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
mz3.add_event_listener("creating_default_group", "twitter.on_creating_default_group", false);


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
	if service_type=='Twitter' then
		return true, VIEW_STYLE_TWITTER;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "twitter.on_get_view_style");


mz3.logger_debug('twitter.lua end');
