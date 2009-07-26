--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : auone
--
-- auone メール用プラグイン
--
-- ログイン以外の処理は gmail.lua に依存している。
--
-- $Id: auone.lua 1306 2009-06-21 14:37:54Z takke $
--------------------------------------------------
mz3.logger_debug('auone.lua start');
module("auone", package.seeall)

--------------------------------------------------
-- サービスの登録(タブ初期化、ログイン設定用)
--------------------------------------------------
mz3.regist_service('auone', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('auone', 'id_name', 'au one-ID');
mz3_account_provider.set_param('auone', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- 受信トレイ
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('AUONE_INBOX');							-- シリアライズキー
type:set_short_title('auone 受信トレイ');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('auone\\inbox_{urlparam:s}.html');	-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング
type:set_default_url('https://mail.google.com/a/auone.jp/h/');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');

-- ログイン用
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('AUONE_LOGIN');							-- シリアライズキー
type:set_short_title('auone ログイン');							-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_cache_file_pattern('auone\\login.html');				-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング


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
	if services:find(' auone', 1, true) ~= nil then
		-- 受信トレイ
		local tab = MZ3GroupItem:create("auone");

		tab:append_category("受信トレイ", "AUONE_INBOX");
		tab:append_category("EZ送信ボックス", "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=l&l=EZ%E9%80%81%E4%BF%A1%E3%83%9C%E3%83%83%E3%82%AF%E3%82%B9');
		tab:append_category("送信済み",   "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=s');
		tab:append_category("スター付き", "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=r');
		tab:append_category("すべて",     "AUONE_INBOX", 'https://mail.google.com/a/auone.jp/h/?s=a');

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end

end
mz3.add_event_listener("creating_default_group", "auone.on_creating_default_group");


----------------------------------------
-- パーサ
----------------------------------------

--------------------------------------------------
-- 【受信トレイ】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function auone_inbox_parser(parent, body, html, is_from_login_parser)
	mz3.logger_debug("auone_inbox_parser start");
	
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
		
		-- <input id="sbb" type="submit" name="nvp_site_mail" value="ﾒｰﾙを検索" />
		-- 上記があればログイン済 = 既に受信箱
		if line_has_strings(line, '<input', '"nvp_site_mail"') then
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

		-- ログイン済みの受信トレイのパース
		gmail.parse_gmail_inbox(parent, body, line);
	else
		-- ログイン処理

		mail_address  = mz3_account_provider.get_value('auone', 'id');
		mail_password = mz3_account_provider.get_value('auone', 'password');
		
		if (mail_address == "" or mail_password == "") then
			mz3.alert("メールアドレスとパスワードをログイン設定画面で設定して下さい");
			return;
		end

		-- フォーム解析
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end
		base_url = 'https://integration.auone.jp';
		forms = parse_form(line, base_url);
--		dump_forms(forms);
		
		form = nil;
		for k, v in pairs(forms) do
			if v.name=="CMN2101E01Dto" or	-- 未ログイン時はこちら。
			   v.name=="acsForm"			-- "CMN2101E01Dto" のあとはこちら。
			then
				form = v;
				break;
			end
		end
		if form==nil then
			mz3.alert("ログインに失敗しました。メールアドレス・パスワードを確認してください。(form not found)");
			return;
		end
		if is_from_login_parser and form.name=="CMN2101E01Dto" then
			-- auone_login_parser から呼び出されているのに未だに "CMN2101E01Dto" のページが
			-- 取得されるということはログインに失敗しているということ。
			mz3.alert("ログインに失敗しました。メールアドレス・パスワードを確認してください。(loop)");
			return;
		end
		
		-- URL 生成
		url = form.action;
		post = mz3_post_data.create();
		mz3_post_data.append_post_body(post, "auoneid=" .. mz3.url_encode(mail_address, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "password=" .. mz3.url_encode(mail_password, 'utf8') .. "&");
		first_in_for = true;
		for name, value in pairs(form.hidden) do
			if first_in_for==false then
				mz3_post_data.append_post_body(post, '&');
			end
			name = mz3.url_encode(name, 'utf8');
			value = mz3.url_encode(value, 'utf8');
			mz3_post_data.append_post_body(post, name .. '=' .. value);
			first_in_for = false;
		end
		
		-- 通信開始
		access_type = mz3.get_access_type_by_key("AUONE_LOGIN");
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("auone_inbox_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("AUONE_INBOX", "auone.auone_inbox_parser");


--------------------------------------------------
-- 【ログイン】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function auone_login_parser(parent, body, html)
	mz3.logger_debug("auone_login_parser start");
	local t1 = mz3.get_tick_count();
	
	-- 受信トレイパーサを利用する。
	-- アクセス種別を別途用意しているのはアクセスメソッド(GET/POST)が
	-- 受信トレイとログイン処理とで異なるため。
--	mz3.alert("auone_login_parser");
	auone_inbox_parser(parent, body, html, true);

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("auone_login_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("AUONE_LOGIN", "auone.auone_login_parser");


----------------------------------------
-- イベントハンドラ
--
-- => gmail.lua 参照
--
----------------------------------------


mz3.logger_debug('auone.lua end');
