--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : gmail
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('gmail.lua start');
module("gmail", package.seeall)

--------------------------------------------------
-- サービスの登録(タブ初期化、ログイン設定用)
--------------------------------------------------
mz3.regist_service('gmail', true);

-- ログイン設定画面のプルダウン名、表示名の設定
mz3_account_provider.set_param('GMail', 'id_name', 'メールアドレス');
mz3_account_provider.set_param('GMail', 'password_name', 'パスワード');


----------------------------------------
-- アクセス種別の登録
----------------------------------------

-- 受信トレイ
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_INBOX');							-- シリアライズキー
type:set_short_title('gmail 受信トレイ');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('gmail\\inbox.html');				-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング
type:set_default_url('https://mail.google.com/mail/h/');
--type:set_default_url('https://integration.auone.jp/login/CMN2101E01.do');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');

-- ログイン用
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_LOGIN');							-- シリアライズキー
type:set_short_title('gmail ログイン');							-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_cache_file_pattern('gmail\\login.html');				-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング

-- メール本文
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');										-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_MAIL');							-- シリアライズキー
type:set_short_title('gmail メール');							-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('gmail\\mail.html');				-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
--menu_items.read_mail    = mz3_menu.regist_menu("gmail.on_mail_read_menu_item");

----------------------------------------
-- イベントハンドラ
----------------------------------------

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
	if services:find(' gmail', 1, true) ~= nil then
		-- 受信トレイ
		local tab = MZ3GroupItem:create("gmail");
		tab:append_category("受信トレイ", "GMAIL_INBOX");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end


end
mz3.add_event_listener("creating_default_group", "gmail.on_creating_default_group");


--------------------------------------------------
-- 【受信トレイ】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function gmail_inbox_parser(parent, body, html)
	mz3.logger_debug("gmail_inbox_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	is_logged_in = false;
	
	GALX = '';
	continue_value = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		-- <input id="sbb" type="submit" name="nvp_site_mail" value="ﾒｰﾙを検索" />
		-- 上記があればログイン済 = 既に受信箱
		if line_has_strings(line, '<input', '"nvp_site_mail"') then
			is_logged_in = true;
			break;
		end
		
		if line_has_strings(line, 'name="GALX"') then
			i = i+1;
			line = html:get_at(i);
			GALX = line:match('value="([^"]*)"');
--			mz3.alert(GALX);
		elseif line_has_strings(line, 'name="continue"') then
			i = i+1;
			line = html:get_at(i);
			continue_value = line:match('value="([^"]*)"');
--			mz3.alert(continue_value);
		end
		
--		mz3.logger_debug(line);
	end
	
	if is_logged_in then
--		mz3.alert('ログイン済');
		
		-- 複数行に分かれているので1行に結合
		line = '';
		for i=0, line_count-1 do
			line = line .. html:get_at(i);
		end
		
--		print(line);

		-- <base href="https://mail.google.com/mail/h/xxx/">
		base_url = line:match('<base href="(.-)">');
--		mz3.alert(base_url);

		pos = line:find('<tr ', 1, true);
		if pos ~= nil then
			pos = pos + 1;
			while true do
				found = line:find('<tr ', pos, true);
				
				if found ~= nil then
					w = line:sub(pos, found-1);

--[[
<tr bgcolor="#E8EEF7"> 
  <td width="1%" nowrap> 
    <input type="checkbox" name="t" value="1216cfad69f86322"> 
      <img src="/mail/images/cleardot.gif" width="15" height="15" border="0" alt=""> 
  </td> 
  <td width="25%"> Twitter (2)</td> 
  <td width="73%"> 
    <a href="?v=c&th=xxx"> 
    <span class="ts">
    <font size="1">
    <font color="#006633"> 
    </font>
    </font> 
    xxxからダイレクトメッセージが届きました 
    <font color="#7777CC"> 
    xxx
    </font>
    </span> 
    </a>
    </td> 
    <td width="1%" nowrap> 19:18 

<tr bgcolor="#ffffff"> 
  <td width="1%" nowrap> 
    <input type="checkbox" name="t" value="xx"> 
      <img src="/mail/images/cleardot.gif" width="15" height="15" border="0" alt=""> 
  </td> 
#以上、無視
  <td width="25%"> 
    <b>xxx</b>
  </td> 
#以上、name
  <td width="73%"> 
    <a href="?v=c&th=xx"> 
#上記、URL の一部
      <span class="ts">
        <font size="1">
          <font color="#006633"> 
          </font>
        </font> 
        <b>たいとる</b> 
# 上記 <b> タグ、title
        <font color="#7777CC"> 
ほんぶんばっすい
ほんぶんばっすい &hellip;
        </font>
# 上記、quote
      </span> 
    </a>
  </td> 
  <td width="1%" nowrap> 
    <b>0:55</b> 
# 上記、日付時刻
<tr bgcolor="#ffffff"> <td> 
...
]]
					name, href, span, date
						= w:match('<td.->.-</td>.-<td.-> (.-)</td>.-href="(.-)">.-<span.->(.-)</span>.-<td.->(.-)$');

					if name~=nil then

						-- data 生成
						data = MZ3Data:create();
						
						--mz3.logger_debug(span);
						--mz3.logger_debug(date);
						
						-- span には title, quote が含まれるが、とりあえず全部 title に入れる
						title = span;
						title = title:gsub('<font.->', '');
						title = title:gsub('</font>', '');
						title = title:gsub('^ *', '');
						-- 未読・既読判定：<b> タグの有無で。
						is_new = line_has_strings(title, '<b>');
						title = title:gsub('<b>', '');
						title = title:gsub('</b>', '');
						if is_new then
							data:set_integer('is_new', 1);
						else
							data:set_integer('is_new', 0);
--							title = '[NEW] ' .. title;
						end

						-- URL 生成 : base_url と結合して生成
						url = base_url .. href;
						data:set_text("url", url);
						date = date:gsub('<.->', '');
						data:set_date(date);

						-- 名前
						name = name:gsub('<b>', '');
						name = name:gsub('</b>', '');
						data:set_text("title", mz3.decode_html_entity(title));
						data:set_text("name", mz3.decode_html_entity(name));
						data:set_text("author", name);
		
						-- URL に応じてアクセス種別を設定
						type = mz3.get_access_type_by_key('GMAIL_MAIL');
						data:set_access_type(type);
		
						-- data 追加
						body:add(data.data);
						
						-- data 削除
						data:delete();
					end
				else
--					mz3.logger_debug('BREAK!!');
					break;
				end
				pos = found + 1;
			end
		end
	else
		-- ログイン処理

		mail_address  = mz3_account_provider.get_value('GMail', 'id');
		mail_password = mz3_account_provider.get_value('GMail', 'password');
		
		if (mail_address == "" or mail_password == "") then
			mz3.alert("メールアドレスとパスワードをログイン設定画面で設定して下さい");
			return;
		end

		-- URL 生成
		url = "https://www.google.com/accounts/ServiceLoginAuth?service=mail";
		post = mz3_post_data.create();
		mz3_post_data.append_post_body(post, "Email=" .. mz3.url_encode(mail_address, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "Passwd=" .. mz3.url_encode(mail_password, 'utf8') .. "&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "service=mail&");
		mz3_post_data.append_post_body(post, "nui=5&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "btmpl=mobile&");
		mz3_post_data.append_post_body(post, "ltmpl=ecobx&");
		mz3_post_data.append_post_body(post, "scc=1&");
		mz3_post_data.append_post_body(post, "GALX=" .. GALX .. "&");
		mz3_post_data.append_post_body(post, "PersistentCookie=yes&");
		mz3_post_data.append_post_body(post, "rmShown=1&");
		mz3_post_data.append_post_body(post, "continue=" .. mz3.url_encode(continue_value, 'utf8'));
		
		continue_value = continue_value:gsub('&amp;', '&');
--		mz3.alert('continue_value : ' .. mz3.url_encode(continue_value, 'utf8'));
--		mz3.alert(url);
		
		-- 通信開始
		access_type = mz3.get_access_type_by_key("GMAIL_LOGIN");
		referer = '';
		user_agent = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_inbox_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_INBOX", "gmail.gmail_inbox_parser");

--------------------------------------------------
-- 【ログイン】
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function gmail_login_parser(parent, body, html)
	mz3.logger_debug("gmail_login_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	url = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, "<meta", "refresh") then
			url = line:match('0; url=([^"]*)');
--			mz3.alert(url);
		end
		
--		mz3.logger_debug(line);
	end
	url = url:gsub('&amp;', '&');
--	mz3.alert('url : ' .. url);
	if url == '' then
		mz3.alert('ログインに失敗しました。\r\nメールアドレスとパスワードを確認してください。');
		return;
	end
	
	-- 通信開始
	access_type = mz3.get_access_type_by_key("GMAIL_INBOX");
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_login_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_LOGIN", "gmail.gmail_login_parser");

--------------------------------------------------
-- 【メール】
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function gmail_mail_parser(data, dimmy, html)
	mz3.logger_debug("gmail_mail_parser start");
	
	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	data:clear();
	
	local t1 = mz3.get_tick_count();
	
	local line_count = html:get_count();
	
	-- 複数行に分かれているので1行に結合
	line = '';
	for i=0, line_count-1 do
		line = line .. html:get_at(i);
	end
	
	-- 本文
	-- <tr bgcolor="#ffffff">...<a name="m_">
	body = line:match('<tr bgcolor="#ffffff">(.-)<a name="m_">');
	data:add_body_with_extract(body);

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_mail_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_MAIL", "gmail.gmail_mail_parser");


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

	if serialize_key == "GMAIL_MAIL" then
		if mz3_data.get_integer(body, 'is_new')~=0 then
			return true, 6;
		else
			return true, 7;
		end
	end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "gmail.on_get_body_list_default_icon_index");


----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- ボディリストのダブルクリック(またはEnter)イベントハンドラ登録
--mz3.add_event_listener("dblclk_body_list", "mixi.on_body_list_click");
--mz3.add_event_listener("enter_body_list",  "mixi.on_body_list_click");

-- ボディリストのポップアップメニュー表示イベントハンドラ登録
--mz3.add_event_listener("popup_body_menu",  "gmail.on_popup_body_menu");

-- estimate 対象の追加
--mz3.add_event_listener("estimate_access_type_by_url", "gmail.on_estimate_access_type");


mz3.logger_debug('gmail.lua end');
