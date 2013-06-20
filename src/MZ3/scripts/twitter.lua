--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : twitter
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('twitter.lua start');
module("twitter", package.seeall)

-- uses dkjson, see: http://dkolf.de/src/dkjson-lua.fsl/home
local json = require ("scripts\\lib\\dkjson")


ID_REPORT_URL_BASE = 36100 -37000;	-- URLを開く

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('Twitter', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('Twitter', 'id_name', 'ID');
mz3_account_provider.set_param('Twitter', 'password_name', 'パスワード');

-- oauth cosumer key/secrets
local app_name = mz3.get_app_name();
if app_name=="MZ3" then
	oauth_consumer_key    = "ihrwFbmMkoCq2oGqZRy9OQ";
	oauth_consumer_secret = "ljMRj94uD1ElrJW4cctMTCLQuQ7JFVDXrSO8W6WH9H8";
elseif app_name=="MZ4" then
	oauth_consumer_key    = "pCmLA1sbYrpPnCqAf75IqA";
	oauth_consumer_secret = "4fFQsnykz5RSxqbvSdj5IORkMTOLJ2YPapgQNGo";
else
	-- TkTweets
	oauth_consumer_key    = "bhzxioodI0l2x0DljCYJQ";
	oauth_consumer_secret = "HMWZshVvg2SE4cUwI6Se8T9RiZ1FNmnxQjlXAw4CcU";
end

----------------------------------------
-- アクセス種別の登録
----------------------------------------

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='other', service_type='Twitter', key='TWITTER_USER',
  short_title='Twitter発言', request_method='GET', request_encoding='utf8'
});

-- POST用アクセス種別登録
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_UPDATE',
  short_title='更新', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_FAVOURINGS_CREATE',
  short_title='お気に入り登録', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_FAVOURINGS_DESTROY',
  short_title='お気に入り削除', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_FRIENDSHIPS_CREATE',
  short_title='フォロー登録', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_FRIENDSHIPS_DESTROY',
  short_title='フォロー解除', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_UPDATE_WITH_TWITPIC',
  short_title='twitpic投稿', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_UPDATE_RETWEET',
  short_title='ReTweet', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_UPDATE_DESTROY',
  short_title='発言削除', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_USER_BLOCK_CREATE',
  short_title='ブロック', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_UPDATE_DESTROY',
  short_title='ブロック解除', request_method='POST', request_encoding='utf8'
});

-- https://api.twitter.com/1.1/statuses/home_timeline.json
-- https://api.twitter.com/1.1/statuses/home_timeline.json?screen_name=takke
-- https://api.twitter.com/1.1/statuses/mentions_timeline.json
-- https://api.twitter.com/1.1/statuses/user_timeline.json
-- https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=takke
-- => twitter/user_timeline_takke.json
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='Twitter', key='TWITTER_FRIENDS_TIMELINE',
  short_title='タイムライン', request_method='GET', request_encoding='utf8'
});
type:set_cache_file_pattern('twitter\\{urlafter:/:friends_timeline.json}');
type:set_default_url('https://api.twitter.com/1.1/statuses/home_timeline.json');
type:set_body_header(1, 'body', '発言');
type:set_body_header(2, 'name', '名前>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 \t(%3)');	-- "名前  (日付)"
type:set_body_integrated_line_pattern(2, '%1');			-- "発言"


-- https://api.twitter.com/1.1/favorites/list.json
-- https://api.twitter.com/1.1/favorites/list.json?screen_name=takke
-- => twitter/favorites_takke.json
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='Twitter', key='TWITTER_FAVORITES',
  short_title='お気に入り', request_method='GET', request_encoding='utf8'
});
type:set_cache_file_pattern('twitter\\favorites{urlafter:screen_name=/:.json}');
type:set_default_url('https://api.twitter.com/1.1/favorites/list.json');
type:set_body_header(1, 'body', '発言');
type:set_body_header(2, 'name', '名前>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 \t(%3)');	-- "名前  (日付)"
type:set_body_integrated_line_pattern(2, '%1');			-- "発言"


-- Lists用アクセス種別登録
type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='Twitter', key='TWITTER_LISTS',
  short_title='リスト一覧', request_method='GET', request_encoding='utf8'
});
type:set_cache_file_pattern('twitter\\{urlafter:/}');
type:set_default_url('https://api.twitter.com/1.1/favorites/list.json?screen_name={twitter:id}');	-- dummy
type:set_body_header(1, 'title', 'リスト名');
type:set_body_header(2, 'name', 'members>>');	-- MZ3の制限のため
type:set_body_header(3, 'date', 'Followers>>');	-- MZ3の制限のため
type:set_body_integrated_line_pattern(1, '%1 (%2)');
type:set_body_integrated_line_pattern(2, '  Followers:%3');

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='other', service_type='Twitter', key='TWITTER_LISTS_ITEM',
  short_title='Twitterリスト', request_method='GET', request_encoding='utf8'
});

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='Twitter', key='TWITTER_FOLLOWERS',
  short_title='フォロワー一覧', request_method='GET', request_encoding='utf8'
});
type:set_cache_file_pattern('twitter\\followers_list.json');
type:set_default_url('https://api.twitter.com/1.1/followers/list.json');
type:set_body_header(1, 'name', '名前');
type:set_body_header(2, 'title', '自己紹介');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');

type = MZ3AccessTypeInfo:create();
type:set_params({
  info_type='category', service_type='Twitter', key='TWITTER_FOLLOWINGS',
  short_title='フォロー一覧', request_method='GET', request_encoding='utf8'
});
type:set_cache_file_pattern('twitter\\friends_list.json');
type:set_default_url('https://api.twitter.com/1.1/friends/list.json');
type:set_body_header(1, 'name', '名前');
type:set_body_header(2, 'title', '自己紹介');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');

-- bit.ly
type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='other', service_type='Twitter', key='TWITTER_BIT_LY',
  short_title='bit.ly', request_method='GET', request_encoding='utf8'
});

-- access_token
type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='other', service_type='Twitter', key='TWITTER_ACCESS_TOKEN',
  short_title='token', request_method='POST', request_encoding='utf8'
});

-- dummy
type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='other', service_type='TwitterX', key='TWITTER_X_DUMMY',
  short_title='dummy', request_method='GET', request_encoding='utf8'
});

-- リスト系
type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_CREATE_LIST',
  short_title='リスト作成', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_DELETE_LIST',
  short_title='リスト削除', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_ADD_LIST_MEMBER',
  short_title='リストに追加', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_DELETE_LIST_MEMBER',
  short_title='リストから削除', request_method='POST', request_encoding='utf8'
});

type = MZ3AccessTypeInfo.create();
type:set_params({
  info_type='post', service_type='Twitter', key='TWITTER_NEW_DM',
  short_title='DM', request_method='POST', request_encoding='utf8'
});


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read                  = mz3_menu.regist_menu("twitter.on_read_menu_item");
menu_items.show_user_info        = mz3_menu.regist_menu("twitter.on_show_user_info");
menu_items.show_main_view        = mz3_menu.regist_menu("twitter.on_show_main_view");
menu_items.retweet               = mz3_menu.regist_menu("twitter.on_retweet_menu_item");

menu_items.get_prev_page         = mz3_menu.regist_menu("twitter.on_get_prev_page");

-- 発言内の @xxx 抽出者のTL(5人まで)
menu_items.show_follower_tl = {}
menu_items.show_follower_tl[1]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_1");
menu_items.show_follower_tl[2]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_2");
menu_items.show_follower_tl[3]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_3");
menu_items.show_follower_tl[4]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_4");
menu_items.show_follower_tl[5]   = mz3_menu.regist_menu("twitter.on_show_follower_tl_5");
follower_names = {}

menu_items.update                    = mz3_menu.regist_menu("twitter.on_twitter_update");
menu_items.update_with_twitpic       = mz3_menu.regist_menu("twitter.on_twitter_update_with_twitpic");
menu_items.update_with_twitpic_now   = mz3_menu.regist_menu("twitter.on_twitter_update_with_twitpic_now");
menu_items.reply                     = mz3_menu.regist_menu("twitter.on_twitter_reply");
menu_items.new_dm                    = mz3_menu.regist_menu("twitter.on_twitter_new_dm");
menu_items.create_favourings         = mz3_menu.regist_menu("twitter.on_twitter_create_favourings");
menu_items.destroy_favourings        = mz3_menu.regist_menu("twitter.on_twitter_destroy_favourings");
menu_items.create_friendships        = mz3_menu.regist_menu("twitter.on_twitter_create_friendships");
menu_items.destroy_friendships       = mz3_menu.regist_menu("twitter.on_twitter_destroy_friendships");
menu_items.show_status_url           = mz3_menu.regist_menu("twitter.on_show_status_url");
menu_items.show_reply_status_url     = mz3_menu.regist_menu("twitter.on_show_reply_status_url");
menu_items.show_friend_timeline      = mz3_menu.regist_menu("twitter.on_show_friend_timeline");
menu_items.open_home                 = mz3_menu.regist_menu("twitter.on_open_home");
menu_items.open_friend_favorites     = mz3_menu.regist_menu("twitter.on_open_friend_favorites");
menu_items.open_friend_favorites_by_browser = mz3_menu.regist_menu("twitter.on_open_friend_favorites_by_browser");
menu_items.open_friend_site          = mz3_menu.regist_menu("twitter.on_open_friend_site");
menu_items.debug                     = mz3_menu.regist_menu("twitter.on_debug");
menu_items.search_post               = mz3_menu.regist_menu("twitter.on_search_post");
menu_items.search_hash               = mz3_menu.regist_menu("twitter.on_search_hash_list");
menu_items.twitter_search            = mz3_menu.regist_menu("twitter.on_twitter_search");
menu_items.twitter_update_destroy    = mz3_menu.regist_menu("twitter.on_twitter_update_destroy");
menu_items.twitter_user_block_create = mz3_menu.regist_menu("twitter.on_twitter_user_block_create");
menu_items.twitter_user_block_destroy= mz3_menu.regist_menu("twitter.on_twitter_user_block_destroy");
menu_items.twitter_user_spam_reports = mz3_menu.regist_menu("twitter.on_twitter_user_spam_reports_create");
menu_items.search_favotter           = mz3_menu.regist_menu("twitter.on_search_favotter");
menu_items.search_user_timeline      = mz3_menu.regist_menu("twitter.on_search_user_timeline");

-- 発言内のハッシュタグ #xxx 抽出リスト
menu_items.search_hash_list = {}
menu_items.search_hash_list[1]   = mz3_menu.regist_menu("twitter.on_search_hash_list_1");
menu_items.search_hash_list[2]   = mz3_menu.regist_menu("twitter.on_search_hash_list_2");
menu_items.search_hash_list[3]   = mz3_menu.regist_menu("twitter.on_search_hash_list_3");
menu_items.search_hash_list[4]   = mz3_menu.regist_menu("twitter.on_search_hash_list_4");
menu_items.search_hash_list[5]   = mz3_menu.regist_menu("twitter.on_search_hash_list_5");
hash_list = {}

menu_items.shorten_by_bitly      = mz3_menu.regist_menu("twitter.on_shorten_by_bitly");

-- リスト
menu_items.create_list           = mz3_menu.regist_menu("twitter.on_create_list");
menu_items.delete_list           = mz3_menu.regist_menu("twitter.on_delete_list");
menu_items.add_list_member       = mz3_menu.regist_menu("twitter.on_add_list_member");
menu_items.delete_list_member    = mz3_menu.regist_menu("twitter.on_delete_list_member");

-- ファイル名
twitpic_target_file = nil;
-- ダブルクリックとかメニューから @ した場合の status_id
-- is_twitter_reply_set = false;
twitter_reply_id = 0;

-- 自分のリスト一覧(リスト操作用)
list_names_self = {};


--------------------------------------------------
-- カテゴリURLの移行
--------------------------------------------------
local my_replace = function(url, before, after)
	if url:find(before, 1, true)~=nil then
		return url:gsub(before, after), true;
	end
	return url, false;
end
function on_migrate_category_url(event_name, serialize_key, url)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
		return false;
	end

--	mz3.logger_debug('on_migrate_category_url[' .. serialize_key .. '][' .. url .. ']');

	if serialize_key == 'TWITTER_FRIENDS_TIMELINE' then
	
		local r;
		
		-- home_timeline
		url, r = my_replace(url, "http://twitter.com/statuses/friends_timeline.xml",
		                         "https://api.twitter.com/1.1/statuses/home_timeline.json");
		if r == true then
			return true, url;
		end
		
		url, r = my_replace(url, "http://twitter.com/statuses/home_timeline.xml",
		                         "https://api.twitter.com/1.1/statuses/home_timeline.json");
		if r == true then
			return true, url;
		end

		url, r = my_replace(url, "http://api.twitter.com/1/statuses/home_timeline.xml",
		                         "https://api.twitter.com/1.1/statuses/home_timeline.json");
		if r == true then
			return true, url;
		end

		-- 返信
		url, r = my_replace(url, "https://api.twitter.com/1/statuses/replies.xml",
		                         "http://api.twitter.com/1.1/statuses/mentions_timeline.json");
		if r == true then
			return true, url;
		end

		-- RTされた一覧
		url, r = my_replace(url, "https://api.twitter.com/1/statuses/retweets_of_me.xml",
		                         "https://api.twitter.com/1.1/statuses/retweets_of_me.json");
		if r == true then
			return true, url;
		end
		
		-- ユーザータイムライン
		url, r = url:gsub("http://twitter.com/statuses/user_timeline/(.-).xml",
		                  "https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=%1");
		if r > 0 then
			return true, url;
		end
		
	end

	if serialize_key == 'TWITTER_FAVORITES' then
	
		local r;
		
		-- favorites
		url, r = my_replace(url, "http://twitter.com/favorites.xml",
		                         "https://api.twitter.com/1.1/favorites/list.json");
		if r == true then
			return true, url;
		end
		
		url, r = my_replace(url, "https://api.twitter.com/1/favorites.xml",
		                         "https://api.twitter.com/1.1/favorites/list.json");
		if r == true then
			return true, url;
		end
	end
	
	if serialize_key == 'TWITTER_LISTS' then
	
		local r;
		
		-- lists
		url, r = my_replace(url, "https://api.twitter.com/1/{twitter:id}/lists.xml",
		                         "https://api.twitter.com/1.1/lists/list.json");
		if r == true then
			return true, url;
		end
		
		-- 登録されているリスト一覧
		url, r = my_replace(url, "https://api.twitter.com/1.1/lists/memberships.json?screen_name={twitter:id}",
		                         "https://api.twitter.com/1.1/lists/memberships.json?screen_name={twitter:id}");
		if r == true then
			return true, url;
		end
	end
	
	if serialize_key == 'TWITTER_FOLLOWINGS' then
	
		local r;
		url, r = my_replace(url, "https://api.twitter.com/1/statuses/friends.xml",
		                         "https://api.twitter.com/1.1/friends/list.json");
		if r == true then
			return true, url;
		end
	end
	
	if serialize_key == 'TWITTER_FOLLOWERS' then
	
		local r;
		url, r = my_replace(url, "https://api.twitter.com/1/statuses/followers.xml",
		                         "https://api.twitter.com/1.1/followers/list.json");
		if r == true then
			return true, url;
		end
	end
	
	return false;
end
mz3.add_event_listener("migrate_category_url", "twitter.on_migrate_category_url");


--------------------------------------------------
-- [list] タイムライン用パーサ
--
-- https://api.twitter.com/1.1/statuses/home_timeline.json
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
reset_body_list_id = 0;	-- 取得後のボディリストの復帰位置(Twitter POST ID)
function twitter_friends_timeline_parser(parent, body, html)
	mz3.logger_debug("twitter_friends_timeline_parser start");

	-- 返信 id の初期化
	twitter_reply_id = 0;
	
	-- 現在のカーソル位置を記憶
	body_item = MZ3Data:create(mz3_main_view.get_selected_body_item());
	reset_body_list_id = body_item:get_integer64_as_string('id');

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);
	
	-- jsonパース
	local json_text = html:get_all_text();
	local obj, pos, err = json.decode (json_text, 1, nil)
	if err then
		print ("Error:", err);
		return;
	end

	my_twitter_name = mz3_account_provider.get_value('Twitter', 'id');

	-- 全消去しない
--	body:clear();
	
	-- 重複防止用の id 一覧を生成。
	id_set = {};
	n = body:get_count();
	for i=0, n-1 do
		local f = body:get_data(i);
		id = mz3_data.get_integer64_as_string(f, 'id');
--		mz3.logger_debug(id);
		id_set[ "" .. id ] = true;
		-- 全ての new フラグを解除
		mz3_data.set_integer(f, 'new_flag', 0);
	end

	local t1 = mz3.get_tick_count();
	
	-- 一時リスト
	new_list = MZ3DataList:create();
	
	-- Twitter検索ではstatusesキーにデータがある
	if obj.statuses ~= nil then
		obj = obj.statuses;
	end
	
	-- 各要素のパース
	for i = 1, #obj do
		local v = obj[i];
		
		-- 同一 ID の skip
		local id = v.id;
		if id_set[ "" .. id ] then
			print(i, id, "skip");
		else
			my_add_new_user(new_list, v, id);
		end
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


--- 1ユーザの追加
function my_add_new_user(new_list, status, id)
--	mz3.logger_debug("my_add_new_user start");

	-- data 生成
	data = MZ3Data:create();
	data:set_integer('new_flag', 1);
	
	-- id : status/id
	data:set_integer64_from_string('id', id);
	type = mz3.get_access_type_by_key('TWITTER_USER');
	data:set_access_type(type);
	
	-- updated : status/created_at
	local s = status.created_at;
	data:parse_date_line(s);
	
	-- text : status/text
	local text = mz3.decode_html_entity(status.text);
	data:add_text_array('body', text);

	-- URL を抽出し、リンクにする
	if line_has_strings(text, 'ttp') then
		for url in text:gmatch("h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%%#]+") do
			data:add_link_list(url, url);
--			mz3.logger_debug('抽出URL: ' .. url);
		end
	end

	-- source : status/source
	s = status.source
	s = mz3.decode_html_entity(s);
	data:set_text('source', s);
	
	-- name : status/user/screen_name
	local user = status.user
	if user ~= nil then
		data:set_text('name', mz3.decode_html_entity(user.screen_name));

		-- author : status/user/name
		data:set_text('author', mz3.decode_html_entity(user.name));

		-- description : status/user/description
		-- title に入れるのは苦肉の策・・・
		data:set_text('title', mz3.decode_html_entity(user.description));

		-- owner-id : status/user/id
		data:set_integer('owner_id', user.id);
	
		-- URL : status/user/url
		local url = mz3.decode_html_entity(user.url);
		data:set_text('url', url);
		data:set_text('browse_uri', url);

		-- Image : status/user/profile_image_url
		local profile_image_url = mz3.decode_html_entity(user.profile_image_url);
		data:add_text_array('image', profile_image_url);

		data:set_text('user_json', json.encode(user));
	end
	
	-- in_reply_to_status_id : status/in_reply_to_status_id
	data:set_integer64_from_string('in_reply_to_status_id', status.in_reply_to_status_id);

	-- in_reply_to_screen_name : status/in_reply_to_screen_name
	data:set_integer('in_reply_to_screen_name', mz3.decode_html_entity(status.in_reply_to_screen_name));

	-- 一時リストに追加
	new_list:add(data.data);
	
	-- data 削除
	data:delete();

--	mz3.logger_debug("my_add_new_user end");
end


--------------------------------------------------
-- [list] Lists用パーサ
--
-- https://api.twitter.com/1.1/lists/list.json
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function twitter_lists_parser(parent, body, html)
	mz3.logger_debug("twitter_lists_parser start");

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	-- jsonパース
	local json_text = html:get_all_text();
	local obj, pos, err = json.decode (json_text, 1, nil)
	if err then
		print ("Error:", err);
		return;
	end

	local t1 = mz3.get_tick_count();
	
	-- 登録されているリスト一覧は lists キーにデータがある
	if obj.lists ~= nil then
		obj = obj.lists;
	end
	
	-- 各要素のパース
	for i = 1, #obj do
		local v = obj[i];
		
		-- data 生成
		data = MZ3Data:create();
		
		-- 各種データ取得、設定
		local full_name    = v.full_name;
		local slug         = v.slug;
		local member_count = v.member_count;
		local subscriber_count = v.subscriber_count;
		local uri              = v.uri;
		local user_name        = v.user.screen_name;
		
		data:set_text('title', full_name);
		data:set_text('name', member_count);
		data:set_text('list_slug', slug);
		data:set_text('user', user_name);
		data:set_date(subscriber_count);
		data:set_text('uri', uri);

		-- Image : list/user/profile_image_url
		profile_image_url = v.user.profile_image_url;
		profile_image_url = mz3.decode_html_entity(profile_image_url);
		mz3.logger_debug(profile_image_url);
		data:add_text_array('image', profile_image_url);

		-- 画像表示フラグを立てる
		data:set_integer('show_image', 1);

		-- アクセス種別設定
		type = mz3.get_access_type_by_key('TWITTER_LISTS_ITEM');
		data:set_access_type(type);

		-- リストに追加
		body:add(data.data);
		
		-- data 削除
		data:delete();
	end

	-- リストがなければメッセージを表示
	if body:get_count()==0 then
		mz3.alert("リストがありませんでした。 \r\n\r\n"
			   .. "Twitter のWeb版からリストを作成してください。 \r\n"
			   .. "MZ3/4ではTwitter側の制限のため、「フォローしているリスト」を取得できません。");

--[[
		-- data 生成
		data = MZ3Data:create();
		
		-- 0件の表示
		data:set_text('name', '');
		data:set_date('');
		data:set_text('title', 'リストがありません。Twitter でリストを作成してください。');

		-- リストに追加
		body:add(data.data);
		
		-- data 削除
		data:delete();
]]
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_lists_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_LISTS", "twitter.twitter_lists_parser");


--------------------------------------------------
-- [list] followers/friends用パーサ
--
-- https://api.twitter.com/1.1/followers/list.json
-- https://api.twitter.com/1.1/friends/list.json
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function twitter_followers_parser(parent, body, html)
	mz3.logger_debug("twitter_followers_parser start");

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();

	-- jsonパース
	local json_text = html:get_all_text();
	local obj, pos, err = json.decode (json_text, 1, nil)
	if err then
		print ("Error:", err);
		return;
	end

	local t1 = mz3.get_tick_count();
	
	local users = obj.users;
	
	-- 各要素のパース
	for i = 1, #users do
		local user = users[i];

		-- data 生成
		data = MZ3Data:create();

		-- アクセス種別設定
		type = mz3.get_access_type_by_key('TWITTER_USER');
		data:set_access_type(type);

		-- 各種データ取得、設定

		local owner_id        = user.id;
		local user_name       = user.name; -- 日本語名
		local screen_name     = user.screen_name; -- @名前
		local location        = mz3.decode_html_entity(user.location);
		local description     = user.description;
		local uri             = user.url;
		local followers_count = user.followers_count;
		local friends_count   = user.friends_count;

		local status     = user.status;
		
		local id         = status and status.id;
		local created_at = status and status.created_at;
		local text       = status and status.text;
		local source     = status and mz3.decode_html_entity(status.source);

		user_name = user_name:gsub('&amp;', '&');
		user_name = mz3.decode_html_entity(user_name);

		screen_name = screen_name:gsub('&amp;', '&');
		screen_name = mz3.decode_html_entity(screen_name);

		if description ~= nil then
			description = description:gsub('&amp;', '&');
			description = mz3.decode_html_entity(description);
		end

		if text ~= nil then
			text = text:gsub('&amp;', '&');
			text = mz3.decode_html_entity(text);
		end

		data:set_integer64_from_string('id', id);
		data:set_integer('owner_id', owner_id);
		data:set_text('name', screen_name);
		data:set_text('author', user_name);
		data:set_text('location', location);
		data:set_text('title', description);
		data:set_text('uri', uri);
		data:set_text('text', text);
		data:parse_date_line(created_at);
		data:add_text_array('body', text);
		data:set_text('source', source);
		data:set_text('followers_count', followers_count);
		data:set_text('friends_count', friends_count);

		-- Image : user/user/profile_image_url
		profile_image_url = user.profile_image_url;
		profile_image_url = mz3.decode_html_entity(profile_image_url);
		data:add_text_array('image', profile_image_url);

		-- 画像表示フラグを立てる
		data:set_integer('show_image', 1);

		data:set_text('user_json', json.encode(user));

		-- リストに追加
		body:add(data.data);

		-- data 削除
		data:delete();
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("twitter_followers_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("TWITTER_FOLLOWERS", "twitter.twitter_followers_parser");
mz3.set_parser("TWITTER_FOLLOWINGS", "twitter.twitter_followers_parser");


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
	elseif http_status==410 then		-- Gone:
		return "APIが廃止されました";
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
function on_set_basic_auth_account(event_name, serialize_key, post, url, is_post)
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='Twitter' then
		if serialize_key=='TWITTER_UPDATE_WITH_TWITPIC' then
			-- twitpic は BASIC 認証不要
			return true, 0, '', '';
		end

--[[
		if true then
			id       = mz3_account_provider.get_value('Twitter', 'id');
			password = mz3_account_provider.get_value('Twitter', 'password');

			if id=='' or password=='' then
				mz3.alert('ログイン設定画面でユーザIDとパスワードを設定してください');
				return true, 1;
			end
			mz3.logger_debug('on_set_basic_auth_account, set id : ' .. id);
			return true, 0, id, password;
		end
]]

		if serialize_key=='TWITTER_ACCESS_TOKEN' then
			return true, 0, '', '';
		end
		
		if post ~= nil then
			-- oauth ヘッダーを設定する
			post = MZ3PostData:create(post);
			
--			mz3.logger_debug("serialize_key:" .. serialize_key);
			post:clear_additional_header();
			mz3.logger_debug("make_authorization_header");
			
			local method = "GET";
			if is_post == 1 then
				method = "POST";
			end
			
			local authorization_header = make_authorization_header(url, post:get_post_body(), method);
			
			if authorization_header == nil then
				-- トークン取得失敗
				mz3.alert('認証に失敗しました。ログイン設定画面でユーザIDとパスワードを設定して下さい');
				return true, 1;
			end
			
			-- ヘッダーの設定
			post:append_additional_header(authorization_header);
		end
		
		-- BASIC 認証は不要
		return true, 0, '', '';
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

	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- フォーカス移動
	mz3_main_view.set_focus('edit');
end


--- 「写真を投稿」メニュー用ハンドラ
function on_twitter_update_with_twitpic(serialize_key, event_name, data)

	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- ファイル選択画面
	OFN_EXPLORER = 0x00080000;
	OFN_FILEMUSTEXIST = 0x00001000;
	flags = OFN_EXPLORER + OFN_FILEMUSTEXIST;
	filter = "JPEGファイル (*.jpg)%0*.jpg;*.jpeg%0%0";
--	filter = "JPEGファイル (*.jpg)%0*.jpg;*.jpeg%0すべてのファイル (*.*)%0*.*%0%0";
	twitpic_target_file = mz3.get_open_file_name(mz3_main_view.get_wnd(),
												 "JPEGファイルを開く...", 
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


--- 「写真を投稿」メニュー用ハンドラ
function on_twitter_update_with_twitpic_now(serialize_key, event_name, data)

	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- 撮影画面
	twitpic_target_file = mz3.camera_capture(mz3_main_view.get_wnd());
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
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

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

	twitter_reply_id = body:get_integer64_as_string('id');

	mz3_main_view.set_edit_text(text);

	-- フォーカス移動
	mz3_main_view.set_focus('edit');

	-- 末尾へ移動
	mz3.keybd_event(VK_END, "keydown");
	mz3.keybd_event(VK_END, "keyup");
end


--- 「URL短縮」メニュー用ハンドラ
function on_shorten_by_bitly(serialize_key, event_name, data)

	-- 入力文字列を取得
	local text = mz3_main_view.get_edit_text();
	
	-- URL 抽出＆短縮
	local target = text;
	text = '';

	mz3_main_view.set_info_text('URL 短縮します');

	-- URL を抽出し、リンクにする
	while true do
		left, url, right = target:match("(.-)(h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%%#]+)(.*)");
		if left == nil then
			-- 残り出力
			text = text .. target;
			break;
		end
		
--		mz3.logger_debug('--');
--		mz3.logger_debug('left  :/' .. left .. '/');
--		mz3.logger_debug('url   :/' .. url .. '/');
--		mz3.logger_debug('right :/' .. right .. '/');
		
		-- 左側出力
		text = text .. left;
		
		-- URL 短縮
		if url:match('http://bit.ly/.*') then
			-- skip
			text = text .. url;
		else
			local bitly_url = 'http://api.bit.ly/shorten?version=2.0.1&longUrl='
			                .. mz3.url_encode(url, 'utf8')
			                .. '&login=takke&apiKey=R_bc9636a2f9e02b56320fb4e616011759 ';
			local referer = '';
			local user_agent = nil;
			local is_blocking = true;
			local access_type = mz3.get_access_type_by_key("TWITTER_BIT_LY");
			local status, result_json = mz3.open_url(mz3_main_view.get_wnd(), access_type, bitly_url,
													 referer, "text", user_agent, nil, is_blocking);
	--		mz3.logger_debug(status);
			mz3.logger_debug(result_json);

			-- URL 短縮用通信、状態復帰
			mz3_main_view.update_control_status();
			mz3_main_view.set_focus('edit');
			
			short_url = result_json:match('"shortUrl": "(.-)"');
			if status ~= 200
			   or result_json:match('"statusCode": "OK"') == nil
			   or short_url == nil then
				mz3.alert('URL 短縮に失敗しました。 \nURL : ' .. url);
				return;
			end
			
			text = text .. short_url;
		end
		target = right;
	end
	
	-- 反映
--	mz3.logger_debug('text :/' .. text .. '/');
	mz3_main_view.set_edit_text(text);

	mz3_main_view.set_info_text('URL 短縮しました');
end


--- 「メッセージ送信」メニュー用ハンドラ
function on_twitter_new_dm(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

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
	id = body:get_integer64_as_string('id');
	url = 'https://api.twitter.com/1.1/favorites/create.json?id=' .. id;

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
	id = body:get_integer64_as_string('id');
	url = 'https://api.twitter.com/1.1/favorites/destroy.json?id=' .. id;

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
	url = "https://api.twitter.com/1.1/friendships/create.json";

	-- ヘッダーの設定
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST パラメータを設定
	post:append_post_body('screen_name=' .. name);

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);

end


--- 「フォローやめる」メニュー用ハンドラ
function on_twitter_destroy_friendships(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんのフォローを解除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = "https://api.twitter.com/1.1/friendships/destroy.json";
	
	-- ヘッダーの設定
	post = MZ3PostData:create();
	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST パラメータを設定
	post:append_post_body('screen_name=' .. name);

	-- 通信開始
	key = "TWITTER_FRIENDSHIPS_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);
end


--- 「ステータスページを開く」メニュー用ハンドラ
function on_show_status_url(serialize_key, event_name, data)
	local body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	local url = "http://twitter.com/" .. body:get_text('name') .. "/statuses/" .. body:get_integer64_as_string('id');
	mz3.open_url_by_browser_with_confirm(url);
end


--- 「返信先の発言を開く」メニュー用ハンドラ
function on_show_reply_status_url(serialize_key, event_name, data)
	local body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	local url = "http://twitter.com/" .. body:get_text('in_reply_to_screen_name') .. "/statuses/" .. body:get_integer64_as_string('in_reply_to_status_id');
	mz3.open_url_by_browser_with_confirm(url);
end


--- 「@xxx のタイムライン」メニュー用ハンドラ
function on_show_friend_timeline(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "https://api.twitter.com/1.1/statuses/user_timeline.json?include_entities=false&include_rts=true&screen_name=" .. name;
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
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- 発言内の num 番目の @xxx ユーザの TL を表示する
	name = follower_names[num];
	
	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = "https://api.twitter.com/1.1/statuses/user_timeline.json?include_entities=false&include_rts=true&screen_name=" .. name;
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
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	local rt_style = mz3_inifile.get_value('TwitterRTStyle', 'Twitter');
	-- 0: RT
	-- 1: 公式RT
	-- 2: QT
	-- 3: 公式RT/RT

	local rt_mode = 0;
	if rt_style == "3" then
		msg = 'この発言をReTweetしますか？ \r\n'
		   .. '\r\n'
		   .. '「はい」：すぐにReTweetします(公式RT)\r\n'
		   .. '「いいえ」：コメントを追加できます';
		if mz3.confirm(msg, nil, 'yes_no') == 'yes' then
			rt_mode = "1";
		else
			rt_mode = "0";
		end
	else
		rt_mode = rt_style;
	end
	
	if rt_mode == "1" then
		-- 公式RT
		serialize_key = 'TWITTER_UPDATE_RETWEET';

		-- ヘッダーの設定
		post = MZ3PostData:create();
		post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
		post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
		post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

		-- POST パラメータを設定
		data = mz3_main_view.get_selected_body_item();
		data = MZ3Data:create(data);
		local id = data:get_integer64_as_string('id');
		post:append_post_body('id=' .. id);

		-- POST先URL設定
		url = 'https://api.twitter.com/1.1/statuses/retweet/' .. id .. '.json';

		-- 通信開始
		access_type = mz3.get_access_type_by_key(serialize_key);
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);
	else
		-- モード変更
		mz3_main_view.set_post_mode(MAIN_VIEW_POST_MODE_TWITTER_UPDATE);

		-- モード変更反映(ボタン名称変更)
		mz3_main_view.update_control_status();

		-- エディットコントロールに文字列設定
		data = mz3_main_view.get_selected_body_item();
		data = MZ3Data:create(data);
		if rt_mode == "0" then
			-- RT
			text = "RT @" .. data:get_text('name') .. ": " .. data:get_text_array_joined_text('body');
		else
			-- QT
			text = "QT @" .. data:get_text('name') .. ": " .. data:get_text_array_joined_text('body');
		end
		text = text:gsub("\r\n", "");
		mz3_main_view.set_edit_text(text);

		-- フォーカス移動
		mz3_main_view.set_focus('edit');
	end
end


--- 「ホーム」メニュー用ハンドラ
function on_open_home(serialize_key, event_name, data)
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	mz3.open_url_by_browser_with_confirm("http://twitter.com/" .. body:get_text('name'));
end


--- 「友達のお気に入り」メニュー用ハンドラ
function on_open_friend_favorites_by_browser(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

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
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
	
	-- カテゴリ追加
	title = "@" .. name .. "のお気に入り";
	url = "https://api.twitter.com/1.1/favorites/list.json?screen_name=" .. name;
	key = "TWITTER_FAVORITES";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- body_item_list から最大のIDを取得する
-- 
-- ただし、ログから取得した項目は含まない
--
-- 未発見時は nil を返す
--
function get_max_id_from_body_list()
	local max_id = nil;
	list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		data = MZ3Data:create(data);
		local id = data:get_integer64_as_string('id');
		local from_log_flag = data:get_integer('from_log_flag');
		mz3.logger_debug(' ' .. i .. ' : ' .. from_log_flag .. ' : ' .. id);
		if from_log_flag ~= 1 then
			max_id = id;
		end
	end
	return max_id;
end


--- 「前のページを取得」メニュー用ハンドラ
function on_get_prev_page(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- max_id の探索(log は含まない)
	local max_id = get_max_id_from_body_list();
	if max_id == nil then
		-- 未取得
		max_id = 0;
	end

	-- カテゴリのURL取得
	local category = MZ3Data:create(mz3_main_view.get_selected_category_item());
	local category_url = category:get_text('url');
	url = category_url;
	if url:find("?", 1, false)~=nil then
		-- ? を含む
		url = url .. '&max_id=' .. max_id;
	else
		-- ? を含まない
		url = url .. '?max_id=' .. max_id;
	end

	-- カテゴリの取得開始
	access_type = category:get_access_type();
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
	elseif serialize_key=="TWITTER_LISTS_ITEM" then
		-- リストのカテゴリ追加
		local data = MZ3Data:create(data);
		
		local title = data:get_text('title');
		local user = data:get_text('user');
		local list_slug = mz3.url_encode(data:get_text('list_slug'), 'utf8');
		local url = 'https://api.twitter.com/1.1/lists/statuses.json?slug=' .. list_slug .. '&owner_screen_name=' .. user;
		local key = "TWITTER_FRIENDS_TIMELINE";
		mz3_main_view.append_category(title, url, key);
		
		-- 追加したカテゴリの取得開始
		access_type = mz3.get_access_type_by_key(key);
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
		return true;
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
	
	-- 詳細画面で表示
	data = mz3_main_view.get_selected_body_item();
	mz3.show_detail_view(data);
	return true;
--[[
	-- 本文を1行に変換して表示
	item = data:get_text_array_joined_text('body');
	item = item:gsub("\r\n", "");
	
	item = item .. "\r\n";
	item = item .. "----\r\n";
	item = item .. "name : " .. data:get_text('name') .. " / " .. data:get_text('author') .. "\r\n";
--	item = item .. "description : " .. data:get_text('title') .. "\r\n";
	item = item .. data:get_date() .. "\r\n";
	item = item .. "id : " .. data:get_integer64_as_string('id') .. "\r\n";

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
]]
end


--- @userについて
function on_show_user_info(serialize_key, event_name, data)
	mz3.logger_debug('on_show_user_info : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
--	mz3.logger_debug(data:get_text('name'));
	
	item = '';
	item = item .. "name : " .. data:get_text('name')
	       .. " / " .. data:get_text('author')
	       .. " / " .. data:get_integer('owner_id')
	       .. "\r\n";
	item = item .. "description : " .. data:get_text('title') .. "\r\n";
--	item = item .. data:get_date() .. "\r\n";
--	item = item .. "id : " .. data:get_integer64_as_string('id') .. "\r\n";
--	item = item .. "owner-id : " .. data:get_integer('owner_id') .. "\r\n";


	-- location 等はここでパースする
	user_json = data:get_text('user_json');
	if user_json ~= nil then

		local user, pos, err = json.decode (user_json, 1, nil)
		if err then
			print ("Error:", err);
		else
			local location = mz3.decode_html_entity(user.location);
			local followers_count = user.followers_count;
			local friends_count = user.friends_count;
			local favourites_count = user.favourites_count;
			local statuses_count = user.statuses_count;

			if location ~= nil and mz3.get_text_length(location)>0 then
				item = item .. "location : " .. location .. "\r\n";
			end
			
			if friends_count ~= nil then
				item = item .. "followings : " .. friends_count
				            .. ", "
				            .. "followers : " .. followers_count
				            .. ", "
				            .. "fav : " .. favourites_count
				            .. ", "
				            .. "発言 : " .. statuses_count
				            .. "\r\n";
			end
		end
	end
	
	mz3.alert(item, data:get_text('name'));

	return true;
end


--- メイン画面に戻る
function on_show_main_view(serialize_key, event_name, data)
	mz3.logger_debug('on_show_main_view : (' .. serialize_key .. ', ' .. event_name .. ')');

	mz3.change_view('main_view');

	return true;
end


--- Twitter に投稿する
function do_post_to_twitter(text)

	serialize_key = 'TWITTER_UPDATE'

	-- ヘッダーの設定
	post = MZ3PostData:create();
--	post:append_additional_header('X-Twitter-Client: ' .. mz3.get_app_name());
--	post:append_additional_header('X-Twitter-Client-URL: http://mz3.jp/');
--	post:append_additional_header('X-Twitter-Client-Version: ' .. mz3.get_app_version());

	-- POST パラメータを設定
	post:append_post_body('status=');
	post:append_post_body(rawurlencode(text));

	-- 返信先ステータス
	if text:sub( 0, 1 ) == '@' then
		if twitter_reply_id ~= 0 then
			post:append_post_body('&in_reply_to_status_id=');
			post:append_post_body(twitter_reply_id);
		end
	end
	twitter_reply_id = 0;

	-- theApp.m_optionMng.m_bAddSourceTextOnTwitterPost の確認
	if mz3_inifile.get_value('AddSourceTextOnTwitterPost', 'Twitter')=='1' then
		if text:find("RT @", 1, false)~=nil or 
		   text:find("QT @", 1, false)~=nil then
			-- RT/QTが含まれているので追加しない
		else
			-- 長さが 140 文字超えるのであれば追加しない。
			local footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
			local appended_text = text .. footer_text;
			local len = mz3.get_text_length(text);
			if len <= 140 then
				post:append_post_body(rawurlencode(footer_text));
			end
		end
	end
--	post:append_post_body('&source=');
--	post:append_post_body(mz3.get_app_name());

	-- POST先URL設定
	url = 'https://api.twitter.com/1.1/statuses/update.json';
	
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

	-- 文字長チェック
	local len = mz3.get_text_length(text);
	if len > 140 then
		-- 140 文字になるまで減らす
		while true do
			local ulen = text:len();
			text = text:sub(1, ulen-1);
			wlen = mz3.get_text_length(text);
			if wlen <= 140 then
				break;
			end
		end
		msg = '140文字を超過しています(' .. len .. '文字)\n'
			.. '140文字までを投稿しますか？ \n'
			.. '----\n'
			.. text;
		if mz3.confirm(msg, '文字数：' .. len, 'yes_no') ~= 'yes' then
			return true;
		end
	end

	-- 確認
	data = mz3_main_view.get_selected_body_item();
	data = MZ3Data:create(data);
	if serialize_key == 'TWITTER_NEW_DM' then
		msg = data:get_text('name') .. ' さんに以下のメッセージを送信します。 \n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, nil, 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE' then
		msg = 'Twitterで発言します。 \n'
		   .. '----\n'
		   .. text .. '\n'
		   .. '----\n'
		   .. 'よろしいですか？';
		if mz3.confirm(msg, len .. '文字', 'yes_no') ~= 'yes' then
			return true;
		end
	elseif serialize_key == 'TWITTER_UPDATE_WITH_TWITPIC' then
		msg = 'twitpicで投稿します。 \n'
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
		post:append_post_body(rawurlencode(text));
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
			local footer_text = mz3_inifile.get_value('PostFotterText', 'Twitter');
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
		url = 'https://api.twitter.com/1.1/direct_messages/new.json';
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
	elseif serialize_key == "TWITTER_UPDATE_DESTROY" then
		mz3_main_view.set_info_text("発言を削除しました");
	elseif serialize_key == "TWITTER_USER_BLOCK_CREATE" then
		mz3_main_view.set_info_text("ブロックしました");
	elseif serialize_key == "TWITTER_USER_BLOCK_DESTROY" then
		mz3_main_view.set_info_text("ブロックを解除しました");
	elseif serialize_key == "TWITTER_USER_SPAM_REPORTS_CREATE" then
		mz3_main_view.set_info_text("スパム通報しました");
	elseif serialize_key == "TWITTER_UPDATE_RETWEET" then
		mz3_main_view.set_info_text("RTしました！");
	elseif serialize_key == "TWITTER_ADD_LIST_MEMBER" then
		mz3_main_view.set_info_text("リストに追加しました！");
	elseif serialize_key == "TWITTER_DELETE_LIST_MEMBER" then
		mz3_main_view.set_info_text("リストから削除しました！");
	elseif serialize_key == "TWITTER_CREATE_LIST" then
		mz3_main_view.set_info_text("リストを作成しました！");
		list_names_self = {};
	elseif serialize_key == "TWITTER_DELETE_LIST" then
		mz3_main_view.set_info_text("リストを削除しました！");
		list_names_self = {};
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
		mz3_main_view.set_info_text("サーバエラー(" .. http_status .. ") : " .. msg);
		--mz3.alert("サーバエラー(" .. http_status .. ") : " .. msg);
		return true;
	end
	-- エラーなしなので続行する(後続の解析処理等を継続)

	return false;
end
mz3.add_event_listener("get_end",  "twitter.on_get_end");


--- ボディリストのポップアップメニュー表示
--
-- @param event_name    'popup_body_menu' or others(詳細画面からの場合)
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	mz3.logger_debug('twitter.on_popup_body_menu : (' .. serialize_key .. ', ' .. event_name .. ')');

	if serialize_key~="TWITTER_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	submenu = MZ3Menu:create_popup_menu();
	submenu_hash = MZ3Menu:create_popup_menu();
	submenu_twitpic = MZ3Menu:create_popup_menu();
	submenu_List = MZ3Menu:create_popup_menu();
	
	name = body:get_text('name');

	if event_name == 'popup_body_menu' then
		menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	else
		-- 詳細画面の場合
		menu:append_menu("string", "メイン画面に戻る", menu_items.show_main_view);
	end
	
	if event_name == 'popup_body_menu' then
		-- N 件未満であれば「前のページを取得」を表示
		list = mz3_main_view.get_body_item_list();
		list = MZ3DataList:create(list);
		local n = list:get_count();
		if n < 100 then
			menu:append_menu("string", "前のページを取得", menu_items.get_prev_page);
		end

		menu:append_menu("string", "全文を読む...", menu_items.read);
		menu:append_menu("string", "@" .. name .. " さんについて...", menu_items.show_user_info);
	end

	menu:append_menu("separator");

	menu:append_menu("string", "つぶやく", menu_items.update);
	
	if mz3_pro_mode and mz3.get_app_name()=="MZ3" then
		-- Pro & MZ3 only
		submenu_twitpic:append_menu("string", "ファイルを選択...", menu_items.update_with_twitpic);
		submenu_twitpic:append_menu("string", "カメラで撮影...", menu_items.update_with_twitpic_now);
		menu:append_submenu("写真を投稿(twitpic)", submenu_twitpic);
	else
		menu:append_menu("string", "写真を投稿(twitpic)...", menu_items.update_with_twitpic);
	end

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

	-- post 削除/検索
	if name == mz3_account_provider.get_value('Twitter', 'id') then
		menu:append_menu("string", "発言削除", menu_items.twitter_update_destroy);
	end
	menu:append_menu("string", "発言検索", menu_items.search_post);
	menu:append_menu("string", "ユーザID検索", menu_items.search_user_timeline);

	submenu_List:append_menu("string", "リストを作成", menu_items.create_list);
	submenu_List:append_menu("string", "リストを削除", menu_items.delete_list);
	submenu_List:append_menu("separator");
	submenu_List:append_menu("string", "リストに追加", menu_items.add_list_member);
	submenu_List:append_menu("string", "リストから削除", menu_items.delete_list_member);
	menu:append_submenu("リスト...", submenu_List);

	-- 発言内のハッシュタグ #XXX を抽出し、メニュー化
	body_text = body:get_text_array_joined_text('body');
	i = 1;
	for hash in body_text:gmatch("#([0-9a-zA-Z_]+)") do
		mz3.logger_debug(hash);
		hash_list[i] = hash;
		submenu_hash:append_menu("string", "#" .. hash .. " の検索", menu_items.search_hash_list[i]);
		i = i+1;
		-- 最大5つまでサポート
		if i>5 then
			break;
		end
	end

	if i > 1 then
		menu:append_submenu("ハッシュタグ", submenu_hash);
	end

	menu:append_menu("separator");

	menu:append_menu("string", "ステータスページを開く", menu_items.show_status_url);
	if body:get_integer64_as_string('in_reply_to_status_id') ~= "-1" then
		menu:append_menu("string", "返信先の発言を開く", menu_items.show_reply_status_url);
	end
	menu:append_menu("string", "@" .. name .. " のタイムライン", menu_items.show_friend_timeline);
	
	-- 発言内の @XXX を抽出し、メニュー化
	body_text = body:get_text_array_joined_text('body');
	i = 1;
	follower_names_work = {};
	for f_name in body_text:gmatch("@([0-9a-zA-Z_]+)") do
		mz3.logger_debug(f_name);
		if follower_names_work[1] == nil then
			follower_names_work[1] = name;
			i=i+1;
		end

		local is_listed = false;
		for j=0, i do
			if follower_names_work[j] == f_name then
				is_listed = true;
				break;
			end
		end
		if is_listed == false then
			-- Lua 変数に名前を保存しておく
			follower_names[i] = f_name;
			follower_names_work[i] = f_name;
			menu:append_menu("string", "@" .. f_name .. " のタイムライン", menu_items.show_follower_tl[i]);
			-- 最大5人までサポート
			i = i+1;
			if i>5 then
				break;
			end
		end
	end

--	menu:append_menu("separator");
	local build_type = mz3.get_app_build_type();
	
	submenu:append_menu("string", "@" .. name .. " をフォローする", menu_items.create_friendships);
	submenu:append_menu("string", "@" .. name .. " のフォローをやめる", menu_items.destroy_friendships);
	submenu:append_menu("string", "@" .. name .. " をブロック", menu_items.twitter_user_block_create);
	submenu:append_menu("string", "@" .. name .. " のブロック解除", menu_items.twitter_user_block_destroy);
	submenu:append_menu("string", "@" .. name .. " をスパム通報する", menu_items.twitter_user_spam_reports);
	submenu:append_menu("string", "@" .. name .. " のホームをブラウザで開く", menu_items.open_home);
	submenu:append_menu("string", "@" .. name .. " のお気に入りをブラウザで開く", menu_items.open_friend_favorites_by_browser);
	submenu:append_menu("string", "@" .. name .. " のお気に入り", menu_items.open_friend_favorites);
	if build_type == "MZ3" then
		-- TkTweets では RSS がないので非表示とする
		submenu:append_menu("string", "@" .. name .. " のふぁぼられ", menu_items.search_favotter);
	end

	-- URL が空でなければ「サイト」を追加
	url = body:get_text('url');
	if url~=nil and url:len()>0 then
		submenu:append_menu("string", "@" .. name .. " のサイトをブラウザで開く", menu_items.open_friend_site);
	end

	if build_type == "MZ3" then
		-- TkTweets では RSS がないので非表示とする
		submenu:append_menu("string", "Twitter検索...", menu_items.twitter_search);
	end
	menu:append_submenu("その他", submenu);

	-- リンク追加
	n = body:get_link_list_size();
	if n > 0 then
		menu:append_menu("separator");
		for i=0, n-1 do
			id = ID_REPORT_URL_BASE+(i+1);
			menu:append_menu("string", "link : " .. body:get_link_list_text(i), id);
		end
	end

--	menu:append_menu("string", "debug", menu_items.debug);

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	submenu:delete();
	submenu_hash:delete();
	submenu_twitpic:delete();
	submenu_List:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu",  "twitter.on_popup_body_menu");


--- 入力エリアのポップアップメニュー表示
--
-- @param event_name    'popup_edit_menu'
-- @param serialize_key ボディアイテムのシリアライズキー
-- @param body          body
-- @param wnd           wnd
--
function on_popup_edit_menu(event_name, serialize_key, body, wnd)
	mz3.logger_debug('twitter.on_popup_edit_menu : (' .. serialize_key .. ', ' .. event_name .. ')');

	if serialize_key~="TWITTER_USER" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	submenu = MZ3Menu:create_popup_menu();
	submenu_twitpic = MZ3Menu:create_popup_menu();
	
	name = body:get_text('name');

	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
--	menu:append_menu("string", "つぶやく", menu_items.update);
	
	if mz3_pro_mode and mz3.get_app_name()=="MZ3" then
		-- Pro & MZ3 only
		submenu_twitpic:append_menu("string", "ファイルを選択...", menu_items.update_with_twitpic);
		submenu_twitpic:append_menu("string", "カメラで撮影...", menu_items.update_with_twitpic_now);
		menu:append_submenu("写真を投稿(twitpic)", submenu_twitpic);
	else
		menu:append_menu("string", "写真を投稿(twitpic)...", menu_items.update_with_twitpic);
	end

	menu:append_menu("string", "URL短縮(bit.ly)", menu_items.shorten_by_bitly);

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	submenu:delete();
	submenu_twitpic:delete();
	
	return true;
end
mz3.add_event_listener("popup_edit_menu",  "twitter.on_popup_edit_menu");


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
		tab:append_category("タイムライン", "TWITTER_FRIENDS_TIMELINE", "https://api.twitter.com/1.1/statuses/home_timeline.json");
		tab:append_category("返信一覧", "TWITTER_FRIENDS_TIMELINE", "https://api.twitter.com/1.1/statuses/mentions_timeline.json");
		tab:append_category("フォローしている一覧", "TWITTER_FOLLOWINGS", "https://api.twitter.com/1.1/friends/list.json");
		tab:append_category("フォローされている一覧", "TWITTER_FOLLOWERS", "https://api.twitter.com/1.1/followers/list.json");
		tab:append_category("リスト一覧", "TWITTER_LISTS", "https://api.twitter.com/1.1/lists/list.json");
		tab:append_category("登録されているリスト一覧", "TWITTER_LISTS", "https://api.twitter.com/1.1/lists/memberships.json?screen_name={twitter:id}");
		tab:append_category("RTされた一覧", "TWITTER_FRIENDS_TIMELINE", "https://api.twitter.com/1.1/statuses/retweets_of_me.json");
		tab:append_category("ブロックユーザ一覧", "TWITTER_FOLLOWINGS", "https://api.twitter.com/1.1/blocks/list.json");
		tab:append_category("お気に入り", "TWITTER_FAVORITES", "https://api.twitter.com/1.1/favorites/list.json");
		
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


--- debug
last_searched_index = 0;
function on_debug(serialize_key, event_name, data)

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		if (mz3_data.get_text(data, 'name')=='doutor') then
			mz3_main_view.select_body_item(i);
			break;
		end
	end
end


mz3.logger_debug('twitter.lua end');


--- post 検索
last_searched_index = 0;
last_searched_key = '';
function on_search_post(serialize_key, event_name, data)

	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	local key = mz3.show_common_edit_dlg("発言検索", "検索したい文字列を入力して下さい", last_searched_key);
	if key == nil then
		return false;
	end
	last_searched_key = key;
	key = string.upper( key );

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		data = MZ3Data:create(data);
		s = data:get_text_array_joined_text('body')
		s = string.upper( s );
		if s:find( key, 1, true ) ~= nil then
			mz3_main_view.select_body_item(i);
			last_searched_index = i;
			break;
		end
	end
end


--- Twitter 検索
function on_twitter_search(serialize_key, event_name, data)

	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	local key = mz3.show_common_edit_dlg("Twitter検索", "検索したい文字列を入力して下さい");
	if key == nil then
		return false;
	end

	-- カテゴリ追加
	title = key .. " - Twitter Search";
	url = "https://api.twitter.com/1.1/search/tweets.json?q=" .. mz3.url_encode(key, 'utf8');
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);
	
	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- ステータス削除メニュー用ハンドラ
function on_twitter_update_destroy(serialize_key, event_name, data)

	if mz3.confirm( "選択された発言を削除します。よろしいですか？", nil, "yes_no") == 'yes' then
		-- URL 生成
		body = MZ3Data:create(mz3_main_view.get_selected_body_item());
		id = body:get_integer64_as_string('id');
		name = body:get_text('name');
		url = 'https://api.twitter.com/1.1/statuses/destroy/' .. id .. '.json';

		-- 通信開始
		key = "TWITTER_UPDATE_DESTROY";
		access_type = mz3.get_access_type_by_key(key);
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
end


--- ブロックメニュー用ハンドラ
function on_twitter_user_block_create(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんをブロックします。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = 'https://api.twitter.com/1.1/blocks/create.json?screen_name=' .. name;

	-- 通信開始
	key = "TWITTER_USER_BLOCK_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- ブロック解除メニュー用ハンドラ
function on_twitter_user_block_destroy(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんのブロックを解除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = 'https://api.twitter.com/1.1/blocks/destroy.json?screen_name=' .. name;

	-- 通信開始
	key = "TWITTER_USER_BLOCK_DESTROY";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- 「#xxx の検索」メニュー用ハンドラ
function on_search_hash_list_1(serialize_key, event_name, data)	search_hash_list(1) end
function on_search_hash_list_2(serialize_key, event_name, data)	search_hash_list(2) end
function on_search_hash_list_3(serialize_key, event_name, data)	search_hash_list(3) end
function on_search_hash_list_4(serialize_key, event_name, data)	search_hash_list(4) end
function on_search_hash_list_5(serialize_key, event_name, data)	search_hash_list(5) end

function search_hash_list(num)
	-- 発言内の num 番目のハッシュタグ #xxx を検索する
	hash = hash_list[num];
	-- カテゴリ追加
	title = "#" .. hash .. " を検索";
	url = "https://twitter.com/search?q=%23" .. hash;
	mz3.open_url_by_browser_with_confirm(url);
end


--- スパム通報メニュー用ハンドラ
function on_twitter_user_spam_reports_create(serialize_key, event_name, data)

	-- 確認
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	if mz3.confirm(name .. " さんをスパムとして通報します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	url = 'https://api.twitter.com/1.1/users/report_spam.json?screen_name=' .. name;

	-- 通信開始
	key = "TWITTER_USER_SPAM_REPORTS_CREATE";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- ふぁぼられメニュー用ハンドラ
function on_search_favotter(serialize_key, event_name, data)
	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');
	title = "@" .. name .. "のふぁぼられ";
	
	key = "RSS_FEED";

	-- URL 生成
	url = "http://favotter.net/userrss.php?user=" .. name .. "&mode=new.feed";

	mz3_main_view.append_category(title, url, key);

	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- 任意ユーザの TL 取得用ハンドラ
function on_search_user_timeline(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

--[[
	body = mz3_main_view.get_selected_body_item();
	body = MZ3Data:create(body);
	name = body:get_text('name');
]]
	local name = mz3.show_common_edit_dlg("ユーザID検索", "検索したいユーザIDを入力して下さい", last_searched_key);
	if name == nil then
		return false;
	end

	-- カテゴリ追加
	title = "@" .. name .. "のタイムライン";
	url = 'https://api.twitter.com/1.1/statuses/user_timeline.json?screen_name=' .. name;
	key = "TWITTER_FRIENDS_TIMELINE";
	mz3_main_view.append_category(title, url, key);

	-- 追加したカテゴリの取得開始
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


-- リスト作成
function on_create_list(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	local name = mz3.show_common_edit_dlg("リスト作成", "リスト名を入力して下さい(半角のみ可)", '');
	if name == nil then
		return false;
	end

	if mz3.confirm("リスト " .. name .. " を作成します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	id  = mz3_account_provider.get_value('Twitter', 'id');
	url = 'https://api.twitter.com/1.1/lists/create.json?name=' .. name;
-- mz3.alert(url);
	-- 通信開始
	key = "TWITTER_CREATE_LIST";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- リスト一覧の取得
function gather_my_list_names_blocking()
	id  = mz3_account_provider.get_value('Twitter', 'id');

	-- ブロッキング通信開始
	key = "TWITTER_LISTS";
	url = 'https://api.twitter.com/1.1/lists/list.json';
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	is_blocking = true;
	status, result = mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, nil, is_blocking);
	if status ~= 200 then
		mz3.alert('リストの取得に失敗しました');
		return false;
	end
	
	list_names_self = {};
	
	local json_text = result;
	local obj, pos, err = json.decode (json_text, 1, nil)
	if err then
		mz3.alert("Error:" .. err);
		return false;
	end
	local myid = mz3_account_provider.get_value('Twitter', 'id');
	for i = 1, #obj do
		local v = obj[i];
		if v.user.screen_name == myid then
			table.insert(list_names_self, v.slug);
		end
	end

	return true;
end


-- リスト削除
function on_delete_list(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	-- リスト一覧未取得であればここで取得する
	if #list_names_self == 0 then
		-- リスト一覧取得 (list_names_self)
		if gather_my_list_names_blocking() == false then
			return false;
		end
	end

	local list_name = mz3.show_common_select_dlg("リスト削除", list_names_self);
	if list_name == nil then
		return false;
	end

	if mz3.confirm("リスト " .. list_name .. " を削除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	id  = mz3_account_provider.get_value('Twitter', 'id');
	url = 'https://api.twitter.com/1.1/lists/destroy.json?slug=' .. mz3.url_encode(list_name, 'utf8') .. '&owner_screen_name=' .. id;
-- mz3.alert(url);
	-- 通信開始
	key = "TWITTER_DELETE_LIST";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = '';
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


-- リストにメンバーを追加
function on_add_list_member(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');

	-- リスト一覧未取得であればここで取得する
	if #list_names_self == 0 then
		-- リスト一覧取得 (list_names_self)
		if gather_my_list_names_blocking() == false then
			return false;
		end
	end

	local list_name = mz3.show_common_select_dlg("リストに追加", list_names_self);
	if list_name == nil then
		return false;
	end

	if mz3.confirm("@" .. name .. "さんをリスト " .. list_name .. " に追加します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	id  = mz3_account_provider.get_value('Twitter', 'id');
	url = 'https://api.twitter.com/1.1/lists/members/create.json?owner_screen_name=' .. id .. '&slug=' .. list_name .. '&screen_name=' .. name;

	-- 通信開始
	key = "TWITTER_ADD_LIST_MEMBER";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


-- リストからメンバーを削除
function on_delete_list_member(serialize_key, event_name, data)
	-- ビューをメイン画面に移す(詳細画面のメニューに対応するため)
	mz3.change_view('main_view');

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	name = body:get_text('name');

	-- リスト一覧未取得であればここで取得する
	if #list_names_self == 0 then
		-- リスト一覧取得 (list_names_self)
		if gather_my_list_names_blocking() == false then
			return false;
		end
	end

	local list_name = mz3.show_common_select_dlg("リストから削除", list_names_self);
	if list_name == nil then
		return false;
	end

	if mz3.confirm("@" .. name .. "さんをリスト " .. list_name .. " から削除します。よろしいですか？", nil, "yes_no") ~= 'yes' then
		-- 中止
		return;
	end

	-- URL 生成
	id  = mz3_account_provider.get_value('Twitter', 'id');
	url = 'https://api.twitter.com/1.1/lists/members/destroy.json?owner_screen_name=' .. id .. '&slug=' .. list_name .. '&screen_name=' .. name;

	-- 通信開始
	key = "TWITTER_DELETE_LIST_MEMBER";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
end


--- メイン画面のキー押下イベント
function on_keyup_main_view(event_name, key, is_shift, is_ctrl, is_alt)
--	mz3.logger_debug('twitter.on_keyup_main_view : (' .. event_name .. ', ' .. key .. ')');

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	serialize_key = body:get_serialize_key();
	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
		return false;
	end

	local focus = mz3_main_view.get_focus();

--[[
	if key == VK_G and is_ctrl~=0 then
--		-- TODO TEST
--		my_test();
		local base_string = "test";
		local key = "takke";
		local sha1 = mz3.hmac_sha1(
					key,
					base_string);
		local oauth_signature = base64.enc(hex_to_binary(sha1));
		mz3.logger_debug(sha1);
		mz3.logger_debug(oauth_signature);
		
		-- "o+Y/AceexaFreG0LVS9VLH0xIsI="

		return true;
	end
]]
	if focus == "category_list" then
		-- カテゴリリスト
		if key == VK_U then
			-- つぶやく
			on_twitter_update(serialize_key, event_name, nil);
			return true;
		end
		
	elseif focus == "body_list" then
		-- ボディリスト

		if key == VK_U then
			-- つぶやく
			on_twitter_update(serialize_key, event_name, nil);
			return true;
		end
		
		if key == VK_F then
			-- ふぁぼる
			on_twitter_create_favourings(serialize_key, event_name, nil);
			return true;
		end
		
		if key == VK_D then
			-- DM
			on_twitter_new_dm(serialize_key, event_name, nil);
			return true;
		end
		
		if key == VK_R then
			-- 返信
			on_twitter_reply(serialize_key, event_name, nil);
			return true;
		end
		
		if key == VK_T then
			-- RT
			on_retweet_menu_item(serialize_key, event_name, nil);
			return true;
		end
		
		--[[
		if key == VK_Z and is_ctrl~=0 and is_alt~=0 then
			local caption = "キャプション";
			local list_names = {"test1", "test2"};
			local v = mz3.show_common_select_dlg(caption, list_names);
			mz3.alert(v);
		end
		]]
		
	elseif focus == "edit" then
		-- エディット
		if key == VK_B and is_ctrl~=0 then
			-- URL短縮
			on_shorten_by_bitly(serialize_key, event_name, nil);
			return true;
		end
	end
	
	return false;
end
mz3.add_event_listener("keyup_main_view", "twitter.on_keyup_main_view");


function on_after_get_end(event_name, serialize_key, body)
	service_type = mz3.get_service_type(serialize_key);
	if service_type == 'Twitter' then
		if mz3_inifile.get_value('CursorRestoreAfterTLFetch', 'Twitter')=='1' and reset_body_list_id ~= 0 then
		
			-- カーソル位置の復帰
			local list = mz3_main_view.get_body_item_list();
			if list ~= nil then
				list = MZ3DataList:create(list);
				local n = list:get_count();
				for i=0, n-1 do
					local data = list:get_data(i);
					data = MZ3Data:create(data);
					if data:get_integer64_as_string('id') == reset_body_list_id then
						mz3_main_view.select_body_item(i);
						break;
					end
				end
			end
		end
		-- Twitter のデフォルト動作も継続する(複数ページ取得など)
		return false;
	end
	
	return false;
end
mz3.add_event_listener("after_get_end", "twitter.on_after_get_end");


--- アイコンの描画
function draw_user_icon(g, f, x, y, w, h)
	image_url = f:get_text_array('image', 0);
	image_cache_index = mz3_image_cache.get_image_index_by_url(image_url);
	if image_cache_index >= 0 then
		g:draw_image(image_cache_index, x, y, w, h);
	end
end


--- 詳細画面の描画
function on_draw_detail_view(event_name, serialize_key, data, dc, cx, cy)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
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
	-- ユーザアイコンの描画
	----------------------------------------------
	icon_size = 48;
	x = x_margin;
--	y_icon = y_margin + line_height;
	y_icon = y_margin;
	y = y_icon;
	w = icon_size;
	h = icon_size;
	draw_user_icon(g, data, x, y, w, h);

	----------------------------------------------
	-- アイコンの右側文字列の描画
	----------------------------------------------
	g:set_font_size(1);		-- 大サイズフォント
	line_height = g:get_line_height();
	
	-- アイコンの右側

	-- 名前
	color_text_org = g:set_color("text", "MainBodyListNonreadText");
	color_bk_org = g:set_color("bk", "MainStatusBG");

	x = x + icon_size +x_margin;
	y = y_icon;
	w = cx - x - x_margin;
	h = line_height;
	format = DT_LEFT;
	text = data:get_text('name') .. " / " .. data:get_text('author');
	g:draw_text(text, x, y, w, h, format);

	-- 日付
	g:set_color("text", "MainBodyListDefaultText");
	text = data:get_date();
	y = y + line_height*1.5;
	format = DT_RIGHT;
	g:set_font_size(0);		-- 中サイズフォント
	line_height = g:get_line_height();
	g:draw_text(text, x, y, w, h, format);

	-- source
	source = data:get_text('source');
--	item = item .. "source : " .. source .. "\r\n";
	s_url, s_name = source:match("href=\"(.-)\".*>(.*)<");
	if s_url ~= nil then
		text = s_name;
	else
		text = source;
	end
	y = y + line_height;
	format = DT_RIGHT;
	g:draw_text(text, x, y, w, h, format);
	--[[
	if s_url ~= nil then
		text = " (" .. s_url .. ")";
		y = y + line_height;
		g:draw_text(text, x, y, w, h, format);
	end
	]]

	----------------------------------------------
	-- 本文～その他の情報
	----------------------------------------------
	
	-- 開始Y座標はクライアント名の下側とアイコンの下側の下の方とする
	local y_source_bottom = y + line_height + y_margin;
	local y_icon_bottom   = y_icon + icon_size + line_height/2;

	-- 本文
	g:set_font_size(1);		-- 大サイズフォント
	line_height = g:get_line_height();
	g:set_color("text", "MainBodyListNonreadText");
	text = data:get_text_array_joined_text('body');
	text = text:gsub("\r\n", "");
	if y_source_bottom > y_icon_bottom then
		y = y_source_bottom;
	else
		y = y_icon_bottom;
	end
--	h = line_height * 7;
	-- 高さは画面の高さの 1/3 程度
	if detail_view_mode == "normal" then
		h = cy / 3;
	elseif detail_view_mode == "all" then
		h = cy - y - 3*line_height;
	else
		h = 0;
	end
	x = x_margin;
	w = cx - x - x_margin;
	
	-- 本文用枠描画
	if h > 0 then
		g:draw_rect("border", x, y, w, h, "MainBodyListNonreadText");
		
		border_margin = 5;
		x = x +border_margin;
		y = y +border_margin;
		w = w -border_margin*2;
		h = h -border_margin*2;
		if mz3.get_app_name()=="MZ3" then
			format = DT_WORDBREAK + DT_NOPREFIX + DT_EDITCONTROL + DT_LEFT + DT_END_ELLIPSIS;
		else
			format = DT_WORDBREAK + DT_NOPREFIX + DT_EDITCONTROL + DT_LEFT + DT_END_ELLIPSIS;
		end
		g:draw_text(text, x, y, w, h, format);
	end

	-- その他の情報用枠線
	x = x_margin;
	y = y + (h +border_margin*2);
	w = cx - x - x_margin;
	h = cy - y - y_margin - y_margin;
--	g:draw_rect("border", x, y, w, h, "MainBodyListDefaultText");
	
	-- その他の情報
	if detail_view_mode == "normal" or detail_view_mode == "info" then
		--[[
		x = x +border_margin;
		y = y +border_margin;
		w = w -border_margin*2;
		h = h -border_margin*2;
		]]

		item = '';
		item = item .. "id : " .. data:get_integer64_as_string('id')
		            .. ", owner-id : " .. data:get_integer('owner_id') .. "\r\n";

		-- location 等はここでパースする
		user_json = data:get_text('user_json');
		if user_json ~= nil then
			local user, pos, err = json.decode (user_json, 1, nil)
			if err then
				print ("Error:", err);
			else
			
				local location = mz3.decode_html_entity(user.location);
				local followers_count = user.followers_count;
				local friends_count = user.friends_count;
				local favourites_count = user.favourites_count;
				local statuses_count = user.statuses_count;

				if location ~= nil and mz3.get_text_length(location)>0 then
					item = item .. "location : " .. location .. "\r\n";
				end
				
				if friends_count ~= nil then
					item = item .. "followings : " .. friends_count
					            .. ", "
					            .. "followers : " .. followers_count
					            .. ", "
					            .. "fav : " .. favourites_count
					            .. "\r\n"
					            .. "発言数 : " .. statuses_count
					            .. "\r\n";
				end
			end
		end
		
		item = item --.. "description : "
		       .. data:get_text('title') .. "\r\n";
		g:set_color("text", "MainBodyListDefaultText");
		g:set_font_size(0);		-- 中サイズフォント
		if mz3.get_app_name()=="MZ3" then
			format = DT_WORDBREAK + DT_NOPREFIX + DT_EDITCONTROL + DT_LEFT + DT_END_ELLIPSIS;
		else
			format = DT_WORDBREAK + DT_NOPREFIX + DT_EDITCONTROL + DT_LEFT + DT_END_ELLIPSIS;
		end
		g:draw_text(item, x, y, w, h, format);
	end

	-- 項目番号(ページ番号風で)
--[[
	g:set_font_size(-1);	-- 小サイズフォント
	line_height = g:get_line_height();
	local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
	local n = list:get_count();
	idx = mz3_main_view.get_selected_body_item_idx();
	text = (idx+1) .. ' / ' .. n;
	g:set_color("text", "MainBodyListDefaultText");
	x = x_margin;
	y = cy - line_height - y_margin;
	w = cx - x - x_margin;
	h = line_height;
	format = DT_NOPREFIX + DT_RIGHT;
	g:draw_text(text, x, y, w, h, format);
]]
	----------------------------------------------
	-- 前後ユーザアイコンの描画
	----------------------------------------------
	g:set_color("text", "MainBodyListDefaultText");

	icon_size = 32;
	h = icon_size;
	local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
	local n = list:get_count();
	local idx = mz3_main_view.get_selected_body_item_idx();

	g:set_font_size(-1);	-- 小サイズフォント
	line_height = g:get_line_height();

	local y_lr_icon = cy - icon_size - y_margin;
	local y_lr_text = cy - line_height - y_margin;

	-- 前ユーザアイコンの描画
	if idx > 0 then
		local f = MZ3Data:create(list:get_data(idx-1));
		format = DT_NOPREFIX + DT_LEFT;

		x = x_margin;
		draw_user_icon(g, f, x, y_lr_icon, icon_size, h);

		x = x_margin + icon_size + x_margin;
		w = cx / 2;
		g:draw_text('≪' .. f:get_text('name'), x, y_lr_text, w, h, format);
	end

	-- 次ユーザアイコンの描画
	if idx < n-1 then
		local f = MZ3Data:create(list:get_data(idx+1));
		format = DT_NOPREFIX + DT_RIGHT;

		x = cx - icon_size - x_margin;
		draw_user_icon(g, f, x, y_lr_icon, icon_size, h);

		x = cx /2;
		w = cx /2 - icon_size - x_margin;
		g:draw_text(f:get_text('name') .. '≫', x, y_lr_text, w, h, format);
	end

	-- 色を戻す
	g:set_color("text", color_text_org);
	g:set_color("bk", color_bk_org);

	return true;
end
mz3.add_event_listener("draw_detail_view",  "twitter.on_draw_detail_view");


--- 詳細画面のキー押下イベント
detail_view_mode = "normal";
function on_keydown_detail_view(event_name, serialize_key, data, key)
	mz3.logger_debug('twitter.on_keydown_detail_view : (' .. serialize_key .. ', ' .. event_name .. ', ' .. key .. ')');

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
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
		-- 左右キー：同一発言者の前後の発言
		local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
		local n = list:get_count();
		local data = MZ3Data:create(data);
		local owner_id = data:get_integer('owner_id');
		
		local idx = mz3_main_view.get_selected_body_item_idx();
		if key == VK_RIGHT then
			idx = idx +1;
		else
			idx = idx -1;
		end
		while 0 <= idx and idx < n do
			local f = MZ3Data:create(list:get_data(idx));
			if owner_id == f:get_integer('owner_id') then
				mz3_main_view.select_body_item(idx);
				mz3.show_detail_view(f.data);
				return true;
			end
			if key == VK_RIGHT then
				idx = idx +1;
			else
				idx = idx -1;
			end
		end
	end
	
	if key == VK_U then
		-- つぶやく
		on_twitter_update(serialize_key, event_name, nil);
		return true;
	end
	
	if key == VK_F then
		-- ふぁぼる
		mz3.change_view('main_view');
		on_twitter_create_favourings(serialize_key, event_name, nil);
		return true;
	end
	
	if key == VK_D then
		-- DM
		on_twitter_new_dm(serialize_key, event_name, nil);
		return true;
	end
	
	if key == VK_R then
		-- 返信
		on_twitter_reply(serialize_key, event_name, nil);
		return true;
	end
	
	if key == VK_T then
		-- RT
		on_retweet_menu_item(serialize_key, event_name, nil);
		return true;
	end
	
	if key == VK_RETURN or key == VK_ESCAPE or key == VK_BACK then
		-- 閉じる
		mz3.change_view('main_view');
		return true;
	end
	
	if key == VK_SPACE then
		-- 3-state「通常表示」「全文表示」「情報表示」
		if detail_view_mode == "normal" then
			detail_view_mode = "all";
		elseif detail_view_mode == "all" then
			detail_view_mode = "info";
		else
			detail_view_mode = "normal";
		end
		data = mz3_main_view.get_selected_body_item();
		mz3.show_detail_view(data);
	end
	
	if key == VK_F2 then
		-- ボディリストのメニューを表示
		on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
		return true;
	end
	
	return true;
end
mz3.add_event_listener("keydown_detail_view", "twitter.on_keydown_detail_view");


--- 詳細画面のクリックイベント
function on_click_detail_view(event_name, serialize_key, data, x, y, cx, cy)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
		return false;
	end

	if y > cy*2/3 then
		-- 下側1/3であれば項目移動
		if x < cx/2 then
			-- 左側：前の項目を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_UP);
		else
			-- 右側：次の項目を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_DOWN);
		end
	elseif y < cy*1/3 then
		-- 上側1/3であれば同一発言者の項目移動
		if x < cx/2 then
			-- 左側：前の発言を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_LEFT);
		else
			-- 右側：次の発言を表示
			on_keydown_detail_view("keydown_detail_view", serialize_key, data, VK_RIGHT);
		end
	else
		-- 閉じる
		mz3.change_view('main_view');
	end
	
	return true;
end
mz3.add_event_listener("click_detail_view", "twitter.on_click_detail_view");


--- 詳細画面の右クリックイベント
function on_rclick_detail_view(event_name, serialize_key, data, x, y)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
		return false;
	end

	-- ボディリストのメニューを表示
	on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
	
	return true;
end
mz3.add_event_listener("rclick_detail_view", "twitter.on_rclick_detail_view");


--- 詳細画面のマウスホイールイベント
function on_mousewheel_detail_view(event_name, serialize_key, data, z_delta, vkey, x, y)

	service_type = mz3.get_service_type(serialize_key);
	if service_type~='Twitter' then
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
mz3.add_event_listener("mousewheel_detail_view", "twitter.on_mousewheel_detail_view");


--- 詳細画面のポップアップメニュー
function on_popup_detail_menu(event_name, serialize_key, data, wnd)

	-- ボディリストのメニューを表示
	return on_popup_body_menu(event_name, serialize_key, data, mz3_main_view.get_wnd());
end
mz3.add_event_listener("popup_detail_menu", "twitter.on_popup_detail_menu");


function string_split_it(str, sep)
    if str == nil then
        return nil;
    end

    return string.gmatch(str, "[^\\" .. sep .. "]+");
end


local function make_base_string(url, base_params, method)

	-- url の正規化
	-- "HTTP://Example.com:80/resource?id=123" => "http://example.com/resource"
	--
	-- ほぼ正規化されている前提とし、パラメータのみ削除する
	--
	if url:find('?', 1, true) ~= nil then
		url, url_param = url:match('^(.-)?(.-)$');
--		mz3.logger_debug('normal url:[' .. url .. '], param[' .. url_param .. ']');

		-- url_param を base_params に追加
		for seg in string_split_it(url_param, '&') do
			local k, v = seg:match('(.-)=(.*)');
			base_params[k] = rawurlencode(v);
--			mz3.logger_debug(' ' .. k .. '=>' .. v);
		end
	end

	local base_string = method
					  .. "&" .. url_rfc3986(url)
					  .. "&";

	-- キーの昇順にアクセスする
	local keys = {};
	for k, v in pairs(base_params) do
		table.insert(keys, k);
	end
	table.sort(keys);

	local is_first = true;

	for k, v in pairs(keys) do
--		mz3.logger_debug(v .. " => " .. base_params[v]);
		local original_key = v;
		local original_val = base_params[v];
		
		if is_first==false then
			base_string = base_string .. "%26";
		end
		
		base_string = base_string .. original_key .. "%3D" .. original_val;
		
		is_first = false;
	end

--	mz3.logger_debug("base_string:" .. base_string);
	return base_string;
end


--- OAuth ヘッダの生成
--
-- xAuth のアクセストークン取得時は oauth_token, oauth_token_secret を nil にすること
--
function make_oauth_authorization_header(url, add_params, oauth_token, oauth_token_secret, method)

	local oauth_nonce = mz3.md5(mz3.get_unixtime() .. math.random());
	local oauth_signature_method = "HMAC-SHA1";
	local oauth_timestamp = mz3.get_unixtime();
	local oauth_version = "1.0";
	
	local base_params = {};
	
	-- OAuth 基本パラメータ
	base_params["oauth_consumer_key"]     = url_rfc3986(oauth_consumer_key);
	base_params["oauth_nonce"]            = url_rfc3986(oauth_nonce);
	base_params["oauth_signature_method"] = url_rfc3986(oauth_signature_method);
	base_params["oauth_timestamp"]        = url_rfc3986(oauth_timestamp);
	if oauth_token ~= nil then
		-- xAuth アクセストークン取得時は未決定のため含まない
		base_params["oauth_token"]        = url_rfc3986(oauth_token);
	end
	base_params["oauth_version"]          = url_rfc3986(oauth_version);

	-- 追加パラメータ
	for k, v in pairs(add_params) do
		base_params[k] = v;
	end

	-- signature 生成
	local base_string = make_base_string(url, base_params, method);
	mz3.logger_debug("base_string:" .. base_string);

	local sha1 = mz3.hmac_sha1(
				url_rfc3986(oauth_consumer_secret) .. "&" .. url_rfc3986(oauth_token_secret),
				base_string);
	if sha1 == nil then
		return nil;
	end
	local oauth_signature = base64.enc(hex_to_binary(sha1));

--[[
	-- pure lua 版
	local oauth_signature = 
		base64.enc(
			hmac_sha1_binary(
				url_rfc3986(oauth_consumer_secret) .. "&" .. url_rfc3986(oauth_token_secret),
				base_string));
]]
	local authorization = 'Authorization: OAuth '
		.. 'oauth_nonce="' .. url_rfc3986(oauth_nonce) .. '",'
		.. 'oauth_signature_method="' .. url_rfc3986(oauth_signature_method) .. '",'
		.. 'oauth_timestamp="' .. url_rfc3986(oauth_timestamp) .. '",'
		.. 'oauth_consumer_key="' .. url_rfc3986(oauth_consumer_key) .. '",';
	if oauth_token ~= nil then
		-- xAuth アクセストークン取得時は未決定のため送信しない
		authorization = authorization .. 'oauth_token="' .. url_rfc3986(oauth_token) .. '",'
	end
	authorization = authorization
		.. 'oauth_signature="' .. url_rfc3986(oauth_signature) .. '",'
		.. 'oauth_version="' .. url_rfc3986(oauth_version) .. '"';
	--	mz3.logger_debug(authorization);

	return authorization;
end


oauth_token_file_path = mz3_dir .. "twitter_oauth_token.txt";

g_oauth_token, g_oauth_token_secret, g_oauth_id = "", "", "";

function get_oauth_tokens()

	mz3.logger_debug("get_oauth_tokens");

	if g_oauth_token == "" and g_oauth_token_secret == "" and g_oauth_id == "" then
		-- 未ロードであればロードする
		mz3.logger_debug(" load");
		
		-- パスワード変更等を考慮すること

		local f = io.open(oauth_token_file_path, "r");
		if f~= nil then
			-- 全件取得
			local file = f:read('*a');
			f:close();
			
			g_oauth_token, g_oauth_token_secret, g_oauth_id = file:match("(.-)\t(.-)\t(.-)\n");

--			mz3.logger_debug(" loaded : " .. g_oauth_token .. ", " .. g_oauth_token_secret);
			return g_oauth_token, g_oauth_token_secret, g_oauth_id;
		else
			return "", "", "";
		end
	else
		mz3.logger_debug(" use tokens in memory.");
		return g_oauth_token, g_oauth_token_secret, g_oauth_id;
	end
end


function set_oauth_tokens(oauth_token, oauth_token_secret, oauth_id)

	f = io.open(oauth_token_file_path, "w");
	if f~=nil then

		f:write(oauth_token .. "\t" .. oauth_token_secret .. "\t" .. oauth_id .. "\n");
		f:close();
		
		g_oauth_token, g_oauth_token_secret, g_oauth_id = oauth_token, oauth_token_secret, oauth_id;
	end
end


--- on_set_basic_auth_account で呼ばれるフック関数
--
--
--
function make_authorization_header(url, post_body, method)

	mz3.logger_debug("make_authorization_header start");

	local oauth_token, oauth_token_secret, oauth_id = get_oauth_tokens();
--	mz3.logger_debug(" oauth_token:" .. oauth_token);
--	mz3.logger_debug(" oauth_token_secret:" .. oauth_token_secret);
	local id        = mz3_account_provider.get_value('Twitter', 'id');

	if oauth_token == nil or oauth_token == "" or 
	   oauth_token_secret == nil or oauth_token_secret == "" or
	   oauth_id ~= id then

--		mz3.alert('認証トークンを取得します');

		-- 認証トークン未取得
		local token_url = 'https://api.twitter.com/oauth/access_token';
		local password  = mz3_account_provider.get_value('Twitter', 'password');

		local add_params = {};
		add_params["x_auth_mode"]     = "client_auth";
		add_params["x_auth_password"] = url_rfc3986(url_rfc3986(password));
		add_params["x_auth_username"] = url_rfc3986(url_rfc3986(id));
		local authorization_header = make_oauth_authorization_header(token_url, add_params, nil, nil, "POST");
		if authorization_header == nil then
			return nil;
		end

		-- ヘッダーの設定
		post = MZ3PostData:create(mz3_post_data.create(1));
		post:append_additional_header(authorization_header);

		-- body
		post:append_post_body('x_auth_mode=client_auth&x_auth_password=' .. url_rfc3986(password)
							  .. '&x_auth_username=' .. url_rfc3986(id));

		-- 通信開始
		local referer = '';
		local user_agent = nil;
		local is_blocking = true;
		local access_type = mz3.get_access_type_by_key("TWITTER_ACCESS_TOKEN");
		local status, result_json = mz3.open_url(mz3_main_view.get_wnd(), access_type, token_url,
												 referer, "text", user_agent, post.post_data, is_blocking);

		mz3.logger_debug("status:" .. status);
		mz3.logger_debug("result:/" .. result_json .. "/");
		mz3.logger_debug("url:/" .. url .. "/");
		
		oauth_token        = result_json:match("oauth_token=(.-)&");
		oauth_token_secret = result_json:match("oauth_token_secret=(.-)&");
		if status ~= 200 or oauth_token == nil or oauth_token_secret == nil then
			mz3.logger_error("認証エラー status[" .. status .. "], result[" .. result_json .. "]");
			return nil;
		end

		-- 認証OK、トークン保存
		set_oauth_tokens(oauth_token, oauth_token_secret, id);
	else
		-- 保存済みのトークンでアクセスする
		mz3.logger_debug("oauth_token:" .. oauth_token);
		mz3.logger_debug("oauth_token_secret:" .. oauth_token_secret);
	end
	
	-- post_body 解析
--	local add_params = {};
--	add_params["status"] = rawurlencode(text);
--	local authorization_header = make_oauth_authorization_header(url, add_params, oauth_token, oauth_token_secret);
--	local url = "http://api.twitter.com/1/statuses/home_timeline.xml";
	local add_params = {};
	
--	mz3.logger_debug("post_body:/" .. post_body .. "/");
--	mz3.logger_debug("分離:");
	for seg in string_split_it(post_body, '&') do
		local k, v = seg:match('(.-)=(.*)');
		add_params[k] = rawurlencode(v);
--		mz3.logger_debug(' ' .. k .. '=>' .. v);
	end
	
	local authorization_header = make_oauth_authorization_header(url, add_params, oauth_token, oauth_token_secret, 
									method);
	if authorization_header == nil then
		return nil;
	end
	mz3.logger_debug("make_authorization_header end");

	return authorization_header;
end


--[[
function my_test()

	local referer = '';
	local user_agent = nil;
	local is_blocking = true;
	local access_type = mz3.get_access_type_by_key("TWITTER_ACCESS_TOKEN");

	local url      = 'https://api.twitter.com/oauth/access_token';
	local id       = mz3_account_provider.get_value('Twitter', 'id');
	local password = mz3_account_provider.get_value('Twitter', 'password');
	local oauth_timestamp = mz3.get_unixtime();

	local add_params = {};
	add_params["x_auth_mode"]     = "client_auth";
	add_params["x_auth_password"] = password;
	add_params["x_auth_username"] = id;
	local authorization_header = make_oauth_authorization_header(url, add_params, nil, nil, "POST");

	-- ヘッダーの設定
	post = MZ3PostData:create();
	post:append_additional_header(authorization_header);

	-- body
	post:append_post_body('x_auth_mode=client_auth&x_auth_password=' .. url_rfc3986(password)
						  .. '&x_auth_username=' .. url_rfc3986(id));

	-- 通信開始
	local status, result_json = mz3.open_url(mz3_main_view.get_wnd(), access_type, url,
											 referer, "text", user_agent, post.post_data, is_blocking);

	mz3.logger_debug("status:" .. status);
	mz3.logger_debug("result:/" .. result_json .. "/");
	
	
	if status == 200 then
		oauth_token        = result_json:match("oauth_token=(.-)&");
		oauth_token_secret = result_json:match("oauth_token_secret=(.-)&");
		
		mz3.logger_debug("oauth_token       : " .. oauth_token);
		mz3.logger_debug("oauth_token_secret: " .. oauth_token_secret);
		
		if false then
			local url = "http://api.twitter.com/1/statuses/update.json";

			local add_params = {};
			local text = "★☆";
			add_params["status"] = rawurlencode(text);

			local authorization_header = make_oauth_authorization_header(url, add_params, oauth_token, oauth_token_secret, "POST");

			-- ヘッダーの設定
			post = MZ3PostData:create();
			post:append_additional_header(authorization_header);

			-- body
			post:append_post_body('status=' .. rawurlencode(text));

			-- 通信開始
			local status, result_json = mz3.open_url(mz3_main_view.get_wnd(), access_type, url,
													 referer, "text", user_agent, post.post_data, is_blocking);

			mz3.logger_debug("status:" .. status);
			mz3.logger_debug("result:/" .. result_json .. "/");
		end
		
		if true then
			local url = "http://api.twitter.com/1/statuses/home_timeline.xml";

			local add_params = {};
			local authorization_header = make_oauth_authorization_header(url, add_params, oauth_token, oauth_token_secret, "GET");

			-- ヘッダーの設定
			post = MZ3PostData:create();
			post:append_additional_header(authorization_header);

			-- 通信開始
			local access_type = mz3.get_access_type_by_key("TWITTER_X_DUMMY");
			local status, result_json = mz3.open_url(mz3_main_view.get_wnd(), access_type, url,
													 referer, "text", user_agent, post.post_data, is_blocking);

			mz3.logger_debug("status:" .. status);
			mz3.logger_debug("result:/" .. result_json .. "/");
		end
	end
	
end
]]


mz3.logger_debug('twitter.lua end');
