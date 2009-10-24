--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : 2ch
--
-- $Id: 2ch.lua 1308 2009-06-27 08:21:46Z takke $
--------------------------------------------------
mz3.logger_debug('2ch.lua start');
module("2ch", package.seeall)

--------------------------------------------------
-- サービスの登録(タブ初期化、ログイン設定用)
--------------------------------------------------
mz3.regist_service('2ch', false);


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- 板一覧   = メイン画面 カテゴリ＋ボディ
-- スレ一覧 = メイン画面 カテゴリ＋ボディ
-- スレッド = レポート画面


-- 板一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('2ch');									-- サービス種別
type:set_serialize_key('2CH_BBS_MENU');							-- シリアライズキー
type:set_short_title('2ch 板一覧');								-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('2ch\\bbs_menu.html');				-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング
type:set_default_url('http://menu.2ch.net/bbsmenu.html');
type:set_body_header(1, 'title', 'カテゴリ');
type:set_body_header(2, 'name', '板');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');


-- スレ一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('2ch');									-- サービス種別
type:set_serialize_key('2CH_SUBJECT');							-- シリアライズキー
type:set_short_title('2ch スレ一覧');							-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('2ch\\subject_{urlafter:/}.html');	-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング
type:set_default_url('http://menu.2ch.net/dummy.html');
type:set_body_header(1, 'title', 'スレ');
type:set_body_header(2, 'name', '板');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
type:set_cruise_target(true);


-- スレッド
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');										-- カテゴリ
type:set_service_type('2ch');									-- サービス種別
type:set_serialize_key('2CH_THREAD');							-- シリアライズキー
type:set_short_title('2ch スレ');								-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('2ch\\thread_{urlafter:/}.html');	-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング


-- 2chブックマーク
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('2ch');									-- サービス種別
type:set_serialize_key('2CH_BOOKMARK');							-- シリアライズキー
type:set_short_title('お気に入りスレ');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('2ch\\subject_{urlafter:/}.html');	-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング
type:set_default_url('http://menu.2ch.net/dummy.html');
type:set_body_header(1, 'title', 'スレ');
type:set_body_header(2, 'name', '板');
type:set_body_integrated_line_pattern(1, '%2');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read               = mz3_menu.regist_menu("2ch.on_read_menu_item");
menu_items.read_by_reportview = mz3_menu.regist_menu("2ch.on_read_by_reportview_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("2ch.on_open_by_browser_menu_item");
menu_items.search_post_thread = mz3_menu.regist_menu("2ch.on_search_post_thread");
menu_items.search_post_bbs    = mz3_menu.regist_menu("2ch.on_search_post_bbs");
menu_items.add_bookmark       = mz3_menu.regist_menu("2ch.on_add_bookmark");
menu_items.remove_bookmark    = mz3_menu.regist_menu("2ch.on_remove_bookmark");


----------------------------------------
-- 2ch プラグイン共通定数
----------------------------------------
bookmark_file_path = mz3_dir .. "2ch_bookmark.tsv";


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
	if services:find(' 2ch', 1, true) ~= nil then
		-- 受信トレイ
		local tab = MZ3GroupItem:create("2ch");

		tab:append_category("お気に入りスレ", "2CH_BOOKMARK");
		tab:append_category("板一覧", "2CH_BBS_MENU");

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end

end
mz3.add_event_listener("creating_default_group", "2ch.on_creating_default_group");


----------------------------------------
-- パーサ
----------------------------------------

--------------------------------------------------
-- 【板一覧】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function the_2ch_bbsmenu_parser(parent, body, html)
	mz3.logger_debug("the_2ch_bbsmenu_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	line = '';
	local line_count = html:get_count();
	local is_start = false;
	local current_group = nil;
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		local group = nil;
		if line_has_strings(line, '<BR><BR>') then
			group = line:match('^<BR><BR><B>(.-)</');
		end
		if group~=nil then
			current_group = group;
		end
		
		if is_start==false and group~=nil then
			is_start = true;
		end

		if is_start then
			-- <A HREF=http://headline.2ch.net/bbynamazu/>地震headline</A><br>
			-- <A HREF=http://www.monazilla.org/ TARGET=_blank>2chツール</A><br>
			local url, title = nil, nil;
			if line_has_strings(line, '<A') then
				url, title = line:match('<A HREF=(.-)/>(.-)</A>');
			end
			if url~= nil and title~=nil then

				-- data 生成
				data = MZ3Data:create();
				
				type = mz3.get_access_type_by_key('2CH_SUBJECT');
				data:set_access_type(type);
				
				data:set_text('name', title);
				data:set_text('title', current_group);

				url = url .. '/subject.txt';
				data:set_text('url', url);
				data:set_text('browse_uri', url);

				-- リストに追加
				body:add(data.data);
				
				-- data 削除
				data:delete();
				
				-- 2つ目以降の同一カテゴリはカテゴリを登録しない。見づらいので。
				current_group = '';
			end
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("the_2ch_bbsmenu_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_BBS_MENU", "2ch.the_2ch_bbsmenu_parser");


--------------------------------------------------
-- 【スレッド一覧】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function the_2ch_subject_parser(parent, body, html)
	mz3.logger_debug("the_2ch_subject_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	line = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		-- 1246883533.dat<>【調査】 １８～３４歳の半数が「子供欲しくない」。結婚しない理由は「いい異性がいない」「経済的に不安」など…京都市調査★３ (212)
		local tno, title = line:match('^(.-)\.dat<>(.-)$');
		if tno~= nil and title~=nil then

			-- data 生成
			data = MZ3Data:create();
			
			type = mz3.get_access_type_by_key('2CH_THREAD');
			data:set_access_type(type);
			
			data:set_text('name', '');
			data:set_text('title', title);
--			data:set_text('title', parent:get_text('name'));

			-- parent url から test/read.cgi/xxx のURLを生成する
			parent_url = parent:get_text('url');
			local domain, category = parent_url:match('http://(.-)/(.-)/subject.txt');
			url = 'http://' .. domain .. '/test/read.cgi/' .. category .. '/' .. tno .. '/l50';
			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- リストに追加
			body:add(data.data);
			
			-- data 削除
			data:delete();
			
			if body:get_count()>=t2ch_thread_view_limit then
				break;
			end
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("the_2ch_subject_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_SUBJECT", "2ch.the_2ch_subject_parser");


--------------------------------------------------
-- 【スレッド】
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function thread_parser(data, dummy, html)
	mz3.logger_debug("thread_parser start");

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();
	
	local t1 = mz3.get_tick_count();
	
	local base_url = '';
	local line_count = html:get_count();
	line = '';
	local next_line = 0;
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<base') then
			base_url = line:match('<base href="(.-)"');
		end
		
		-- タイトル
		if line_has_strings(line, '<title') then
			local title = line:match('<title>(.-)</title>');
			data:set_text('title', title);
		end

		-- <dt>1 ：<font color=green><b>☆ばぐ太☆φ ★</b></font>：2009/07/06(月) 23:34:25 ID:???i<dd> ★電車内でナイフ見せ脅す、大阪  暴力行為法違反容疑で逮捕 <br>  <br> ・電車内で携帯電話の使用を注意された相手にナイフを見せて脅したとして、大阪府警天王寺署は <br> 　６日、暴力行為処罰法違反の疑いで、兵庫県尼崎市長洲本通、会社員太田了容疑者（３０）を <br> 　現行犯逮捕した。 <br>  <br> 　逮捕容疑は６日午後２時５０分ごろ、ＪＲ関西線の東部市場前－天王寺間を走行していた <br> 　快速電車内で、奈良県大和郡山市の男性会社員（６８）から携帯電話のゲームをやめるよう <br> 　注意されたことに腹を立て、持っていたナイフを取り出し「殺してしまうぞ」と脅した疑い。 <br>  <br> 　天王寺署によると、太田容疑者は「腹が立ったので刃物を示した」と容疑を認めている。 <br>  <br> 　電車に乗り合わせていたＪＲ社員が取り押さえ、天王寺駅で警察官に引き渡した。男性や乗客に <br> 　けがはなかった。 <br>  <br> 　<a href="http://ime.nu/www.47news.jp/CN/200907/CN2009070601000800.html" target="_blank">http://www.47news.jp/CN/200907/CN2009070601000800.html</a> <br>  <br> ※前スレ <br> <a href="http://tsushima.2ch.net/test/read.cgi/newsplus/1246881623/" target="_blank">http://tsushima.2ch.net/test/read.cgi/newsplus/1246881623/</a> <br><br>
		if line_has_strings(line, '<dt>') then
			local count, name, date, content = line:match('<dt>(.-)：(.-)：(.-)<dd>(.*)$');
			count = tonumber(count);
			if count>=2 then
				-- 2件目以降は子要素として投入する
				child = MZ3Data:create();
				parse_one_item(child, count, name, date, content);
				data:add_child(child);
				child:delete();
			else
				parse_one_item(data, count, name, date, content);
			end
		end
		
		if line_has_strings(line, '<a', '新着レスの表示') then
			next_line = i+1;
			break;
		end
	end
	
	-- 内部リンク抽出
	for i=next_line, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<a href="../') then
			local url, text = line:match('href="(.-)".->(.-)</');
			data:add_link_list(base_url .. url, text, 'page');
--			mz3.logger_debug(base_url .. url .. text);
		end
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("thread_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_THREAD", "2ch.thread_parser");


function parse_one_item(data, count, name, date, content)
	
	-- レス番
	data:set_integer('comment_index', count);
	
	-- 名前
	-- <a href="mailto:sage"><b>名無しさん＠十周年</b></a>
	-- <font color=green><b>☆ばぐ太☆φ ★</b></font>
	-- <font color=green><b>名無しさん＠十周年</b></font>
	-- <a href="mailto:sage"><b> </b>◆wMzNGLjdrw <b></b></a>
	name1 = name:gsub('<.->', '');
	data:set_text("name", mz3.decode_html_entity(name1));
	data:set_text("author", mz3.decode_html_entity(name1));
	
	-- 日付
	data:set_date(date);

	data:add_text_array("body", "\r\n");
	data:add_body_with_extract(content);
end

----------------------------------------
-- イベントハンドラ
----------------------------------------

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

	if serialize_key == "2CH_SUBJECT" then
		body = MZ3Data:create(body);
		if body:get_text('title')~='' then
			return true, 0;
		end
	end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "2ch.on_get_body_list_default_icon_index");


--- 全文表示メニューまたはダブルクリックイベント
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
	item = '';
	item = item .. data:get_text('title') .. "\r\n";
--	item = item .. "日付 : " .. data:get_date() .. "\r\n";
	item = item .. "----\r\n";

--	item = item .. data:get_text('title') .. "\r\n";
	
--	item = item .. "\r\n";
	item = item .. data:get_text('url') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
end

--- レポートビューで開く
function on_read_by_reportview_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_by_reportview_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);

	-- 通信開始
	url = data:get_text('url');
	key = "2CH_THREAD";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

	return true;
end


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)

	if serialize_key=="2CH_SUBJECT" then
		-- カテゴリ追加
		body = mz3_main_view.get_selected_body_item();
		body = MZ3Data:create(body);
		name = body:get_text('name');
		
--		mz3.alert(name);
		
		-- カテゴリ追加
		title = "└" .. name;
		url = body:get_text('url');
		key = "2CH_SUBJECT";
		mz3_main_view.append_category(title, url, key);
		
		-- 追加したカテゴリの取得開始
		access_type = mz3.get_access_type_by_key(key);
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

		return true;
	end
	
	if serialize_key=="2CH_THREAD" then
		-- メニュー表示
--		return on_popup_body_menu(event_name, serialize_key, mz3_main_view.get_selected_body_item(), mz3_main_view.get_wnd());

		-- プロパティ表示
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end
mz3.add_event_listener("dblclk_body_list", "2ch.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "2ch.on_body_list_click");


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
	if serialize_key=="2CH_THREAD" or serialize_key=="2CH_SUBJECT" then
	else	
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();

	if serialize_key=="2CH_THREAD" then
		menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
		menu:append_menu("string", "スレを開く...", menu_items.read_by_reportview);
		menu:append_menu("string", "ブラウザで開く...", menu_items.open_by_browser);

		menu:append_menu("separator");
		menu:append_menu("string", "プロパティ...", menu_items.read);
		
		-- カテゴリの種別が「お気に入りスレ」だったら削除を追加する。
		-- 「お気に入りスレ」以外だったら追加メニューを追加。
		menu:append_menu("separator");
		local category_type = mz3_main_view.get_selected_category_access_type();
		if category_type==mz3.get_access_type_by_key('2CH_BOOKMARK') then
			menu:append_menu("string", "お気に入りスレから削除", menu_items.remove_bookmark);
		else
			menu:append_menu("string", "お気に入りスレに追加", menu_items.add_bookmark);
		end

		menu:append_menu("separator");
		-- スレ 検索
		menu:append_menu("string", "スレ検索", menu_items.search_post_thread);
	elseif serialize_key=="2CH_SUBJECT" then
		-- 板検索
		menu:append_menu("string", "板検索", menu_items.search_post_bbs);
	end

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu",  "2ch.on_popup_body_menu");


--- レポートビューのポップアップメニュー表示
--
-- @param event_name    'popup_report_menu'
-- @param serialize_key レポートアイテムのシリアライズキー
-- @param report_item   レポートアイテム
-- @param sub_item_idx  選択アイテムのインデックス
-- @param wnd           wnd
--
--[[
function on_popup_report_menu(event_name, serialize_key, report_item, sub_item_idx, wnd)
	if serialize_key~="GMAIL_MAIL" then
		return false;
	end

	-- インスタンス化
	report_item = MZ3Data:create(report_item);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	menu_edit = MZ3Menu:create_popup_menu();
	menu_layout = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "戻る", ID_BACK_MENU);
	menu:append_menu("separator");
	
	menu:append_menu("string", "返信", ID_WRITE_COMMENT);

	menu:append_menu("string", "再読込", IDM_RELOAD_PAGE);

	menu_edit:append_menu("string", "コピー", ID_EDIT_COPY);
	menu:append_submenu("編集", menu_edit);
	
	menu:append_menu("string", "ブラウザで開く（このページ）...", ID_OPEN_BROWSER);

	-- TODO 共通化
	menu:append_menu("separator");
	menu_layout:append_menu("string", "↑リストを狭くする", IDM_LAYOUT_REPORTLIST_MAKE_NARROW);
	menu_layout:append_menu("string", "↓リストを広くする", IDM_LAYOUT_REPORTLIST_MAKE_WIDE);
	menu:append_submenu("画面レイアウト", menu_layout);

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	menu_edit:delete();
	menu_layout:delete();
	
	return true;
end
mz3.add_event_listener("popup_report_menu",  "gmail.on_popup_report_menu");
]]


----------------------------------------
-- estimate 対象に追加
----------------------------------------

--- estimate 対象判別イベントハンドラ
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        解析対象URL
--
function on_estimate_access_type(event_name, url, data1, data2)

    -- 受信箱
    if string.match(url, 'http://.-\.2ch\.net/') then
		return true, mz3.get_access_type_by_key('2CH_THREAD');
	end

	return false;
end
mz3.add_event_listener("estimate_access_type_by_url", "2ch.on_estimate_access_type");


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
	if service_type=='2ch' then
		return true, VIEW_STYLE_IMAGE;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "2ch.on_get_view_style");


--- スレ検索
last_searched_index_thread = 0;
last_searched_key_thread = '';
function on_search_post_thread(serialize_key, event_name, data)

	local key = mz3.show_common_edit_dlg("スレ検索", "検索したい文字列を入力して下さい", last_searched_key_thread);
	if key == nil then
		return false;
	end
	last_searched_key_thread = key;
	key = string.upper( key );

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		s = mz3_data.get_text(data, 'title');
		s = string.upper( s );
		if s:find( key, 1, true ) ~= nil then
			mz3_main_view.select_body_item(i);
			last_searched_index_thread = i;
			break;
		end
	end
end


--- 板検索
last_searched_index_bbs = 0;
last_searched_key_bbs = '';
function on_search_post_bbs(serialize_key, event_name, data)

	local key = mz3.show_common_edit_dlg("板検索", "検索したい文字列を入力して下さい", last_searched_key_bbs);
	if key == nil then
		return false;
	end
	last_searched_key_bbs = key;
	key = string.upper( key );

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		s = mz3_data.get_text(data, 'name');
		s = string.upper( s );
		if s:find( key, 1, true ) ~= nil then
			mz3_main_view.select_body_item(i);
			last_searched_index_bbs = i;
			break;
		end
	end
end


--- カテゴリ取得時のハンドラ
--
-- @param event_name    'retrieve_category_item'
-- @param serialize_key カテゴリアイテムのシリアライズキー
--
function on_retrieve_category_item(event_name, serialize_key, body, wnd)
	if serialize_key~="2CH_BOOKMARK" then
		return false;
	end
	
	-- ブックマーク書き換え
	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);

	list:clear();

	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- 全件取得
		local file = f:read('*a');
		f:close();
		
		for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
			-- data 生成
			data = MZ3Data:create();
			
			type = mz3.get_access_type_by_key('2CH_THREAD');
			data:set_access_type(type);
			
			data:set_text('name', thread_name);
			data:set_text('title', title);

			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- リストに追加
			list:add(data.data);
			
			-- data 削除
			data:delete();
		end
	end

	-- ブックマークはローカルストレージ
	return true, RETRIEVE_CATEGORY_ITEM_RVAL_LOCALSTORAGE;
end
mz3.add_event_listener("retrieve_category_item", "2ch.on_retrieve_category_item");


--- お気に入りスレ追加
function on_add_bookmark(serialize_key, event_name, data)

	-- 追加項目取得＆サニタイジング
	local body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	local category = MZ3Data:create(mz3_main_view.get_selected_category_item());
	
	local thread_name = category:get_text('name');
	thread_name = thread_name:gsub('└', '');
	thread_name = thread_name:gsub("\n", "");
	
	local new_url = body:get_text('url');
	local new_title = body:get_text('title');
	new_url = new_url:gsub("\n", "");
	new_title = new_title:gsub("\n", "");

	local new_item_line = new_url .. "\t" .. new_title .. "\t" .. thread_name;

	-- 存在チェック
	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- 全件取得
		local file = f:read('*a');
		f:close();
		
		for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
			if url==new_url then
				mz3.alert(new_title .. 'は既に登録されています');
				return;
			end
		end
	end
	
	
	-- 追加
	f = io.open(bookmark_file_path, "a");
	if f~=nil then
		f:write(new_item_line .. "\n");
		f:close();
		
		mz3.alert(new_title .. "を追加しました。");
	end
end


--- お気に入りスレ削除
function on_remove_bookmark(serialize_key, event_name, data)

	-- 削除対象取得
	local body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	local target_url = body:get_text('url');
	local target_title = body:get_text('title');

	-- 削除しつつ作成
	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- 全件取得
		local file = f:read('*a');
		f:close();
		
		-- 削除
		f = io.open(bookmark_file_path, "w");
		if f~=nil then
			local deleted = false;
			for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
				if url==target_url then
					deleted = true;
				else
					local item_line = url .. "\t" .. title .. "\t" .. thread_name;
					f:write(item_line .. "\n");
				end
			end
			f:close();
			
			if deleted then
				mz3.alert(target_title .. " を削除しました。");
			else
				mz3.alert(target_title .. " は削除済みです。");
			end
		end
	end
end


mz3.logger_debug('2ch.lua end');
