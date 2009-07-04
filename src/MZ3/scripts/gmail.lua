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
type:set_short_title('GMail 受信トレイ');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('gmail\\inbox_{urlparam:s}.html');	-- キャッシュファイル
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
type:set_short_title('GMail ログイン');							-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_cache_file_pattern('gmail\\login.html');				-- キャッシュファイル
type:set_request_encoding('sjis');								-- エンコーディング

-- メール本文
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');										-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_MAIL');							-- シリアライズキー
type:set_short_title('GMail メール');							-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('gmail\\mail.html');				-- キャッシュファイル
type:set_request_encoding('utf8');								-- エンコーディング

-- 絵文字
type = MZ3AccessTypeInfo.create();
type:set_info_type('other');									-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_EMOJI');							-- シリアライズキー
type:set_short_title('GMail 絵文字');							-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング

-- メール送信
type = MZ3AccessTypeInfo:create();
type:set_info_type('post');										-- カテゴリ
type:set_service_type('gmail');									-- サービス種別
type:set_serialize_key('GMAIL_SEND');							-- シリアライズキー
type:set_short_title('GMail 送信');								-- 簡易タイトル
type:set_request_method('POST');								-- リクエストメソッド
type:set_request_encoding('utf8');								-- エンコーディング


----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
menu_items.read               = mz3_menu.regist_menu("gmail.on_read_menu_item");
menu_items.read_by_reportview = mz3_menu.regist_menu("gmail.on_read_by_reportview_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("gmail.on_open_by_browser_menu_item");
--menu_items.add_star           = mz3_menu.regist_menu("gmail.on_add_star_menu_item");


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
		local tab = MZ3GroupItem:create("GMail");

		tab:append_category("受信トレイ", "GMAIL_INBOX");
		tab:append_category("スター付き", "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=r');
		tab:append_category("送信済み",   "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=s');
		tab:append_category("すべて",     "GMAIL_INBOX", 'https://mail.google.com/mail/h/?s=a');

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end


end
mz3.add_event_listener("creating_default_group", "gmail.on_creating_default_group");


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
function gmail_inbox_parser(parent, body, html)
	mz3.logger_debug("gmail_inbox_parser start");
	
	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	-- ログイン判定
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

		-- ログイン済みのHTMLのパース
		parse_gmail_inbox(parent, body, line);
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
		
--		continue_value = continue_value:gsub('&amp;', '&');
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


--- ログイン済みの GMail 受信トレイの解析
--
-- @param parent 上ペインの選択オブジェクト(MZ3Data*)
-- @param body   下ペインのオブジェクト群(MZ3DataList*)
-- @param line   HTML 全文を1行に結合した文字列
--
function parse_gmail_inbox(parent, body, line)
	mz3.logger_debug("parse_gmail_inbox start");

	-- <base href="https://mail.google.com/mail/h/xxx/">
	base_url = line:match('<base href="(.-)">');
--	mz3.alert(base_url);

	-- 1メールは '<tr ' で始まる
	pos = line:find('<tr ', 1, true);
	if pos == nil then
		-- 解析中止
		return;
	end

	pos = pos + 1;
	looping = true;
	while looping do
		found = line:find('<tr ', pos, true);
		if found == nil then
			looping = false;
			-- 最後のメールのあとは </table> が来る
			found = line:find('</table>', pos, true);
			if found == nil then
				-- </table> すら見つからないのは怪しいので中止
				break;
			end
		end

		-- 1メールの抽出
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
			title = title:gsub('&hellip;', '...');

			-- 未読・既読判定：<b> タグの有無で。
			is_new = line_has_strings(title, '<b>');
			if is_new then
				data:set_integer('is_new', 1);
			else
				data:set_integer('is_new', 0);
			end
			title = title:gsub('<.->', '');
			title = title:gsub('^ *', '');
			title = mz3.decode_html_entity(title);
			data:set_text("title", title);

			-- URL 生成 : base_url と結合して生成
			url = base_url .. href;
			data:set_text("url", url);
			date = date:gsub('<.->', '');
			data:set_date(date);

			-- 名前
			name = name:gsub('<b>', '');
			name = name:gsub('</b>', '');
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
		pos = found + 1;
	end
end


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
function gmail_mail_parser(data, dummy, html)
	mz3.logger_debug("gmail_mail_parser start");

	-- メインビュー、ボディリストのアイコンを既読にする
	mz3_data.set_integer(mz3_main_view.get_selected_body_item(), 'is_new', 0);
	mz3_main_view.redraw_body_images();
	
	
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
	
	-- base url の解析
	-- <base href="https://mail.google.com/mail/h/xxx/">
	base_url  = line:match('<base href="(.-)">');
	data:set_text('base_url', base_url);
	if base_url~=nil then
		base_host = base_url:match('(https?://.-)/');
	end
	
	-- 「全て展開」=全スレッド表示対応
	-- ※無限ループ対策のため、メイン画面から遷移した場合のみ実施する
	if read_gmail_mail_first then
		read_gmail_mail_first = false;

		-- <a href="?v=c&d=e&th=xxx" class="nu">
		-- <img src="/mail/images/expand_icon.gif" width="16" height="16" border="0" alt="すべてのメッセージを展開">
		-- &nbsp;<span class="u">すべて展開</span></a>
		
		if line_has_strings(line, 'alt="すべてのメッセージを展開"') then
			expand_url = line:match('<a href="([^">]+)"[^>]+><img[^>]+>&nbsp;<span[^>]+>すべて展開');
--			mz3.alert(expand_url);
--			expand_url = nil;
			if expand_url ~= nil then
				mz3.logger_debug('全スレッドを取得します');

				data:add_text_array("body", "全スレッドを取得しています。しばらくお待ち下さい。。。");

				-- 通信開始
				url = base_url .. expand_url;
				key = "GMAIL_MAIL";
				access_type = mz3.get_access_type_by_key(key);
				referer = '';
				user_agent = nil;
				post = nil;
				mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
				return;
			end
		end
	end
	
	-- タイトル
	-- <h2><font size="+1"><b>たいとる</b></font></h2>
	title = line:match('<h2><font size=.-><b>(.-)</b>');
	data:set_text('title', mz3.decode_html_entity(title));
	
	-- スレッド分離
	-- 特定のtableと「返信開始タグ」で分離する
	
	one_mail_start_tags = '<table width="100%" cellpadding="1" cellspacing="0" border="0" bgcolor="#efefef"> <tr> <td> ';
	reply_start_tags    = '<table width="100%" cellpadding="1" cellspacing="0" border="0" bgcolor="#e0ecff" class="qr"> <tr> ';
	
	-- 絵文字URLリスト
	-- 例) emoji_urls[0] = 'https://mail.google.com/mail/e/ezweb_ne_jp/B60';
	emoji_urls = {}
	local one_mail = '';
	local mail_count = 0;
	local start = 1;
	local pos = 1;
	while true do
		-- メール開始タグを探す
		pos = line:find(one_mail_start_tags, start, true);
		if pos == nil then
			-- 「返信開始タグ」までを1通とする
			pos = line:find(reply_start_tags, start, true);
			if pos == nil then
				-- 「返信開始タグ」がないので最後まで。
				one_mail = line:sub(start);
			else
				one_mail = line:sub(start, pos-1);
			end

			-- 整形、Data化
			parse_one_mail(data, one_mail, mail_count);
			break;
		else
			one_mail = line:sub(start, pos-1);
			
			-- 整形、Data化
			parse_one_mail(data, one_mail, mail_count);
			
			start = pos + one_mail_start_tags:len();
			mail_count = mail_count +1;
		end
	end
	
	if pos ~= nil then
		-- 「返信開始タグ」以降を用いて返信フォームを取得
		reply_form = line:sub(pos);
		-- 最初の <form>..</form> が返信フォーム。
		reply_form = reply_form:match('<form.-</form>');
		-- data にフォームをそのまま埋め込んでおく
		-- 返信実行時にこのフォームの内容を利用する
		data:set_text('reply_form', reply_form);
	end
	
--	for k, v in pairs(emoji_urls) do
--		print(k, v);
--	end
--	print(#emoji_urls);
	-- 未ロードの絵文字があればロード開始
	get_next_emoji_url();
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("gmail_mail_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("GMAIL_MAIL", "gmail.gmail_mail_parser");


--- emoji_urls の先頭要素に対してリクエストする
function get_next_emoji_url()
	if #emoji_urls >= 1 then
		-- 通信開始
		url = emoji_urls[1];
--		mz3.alert(url);
		access_type = mz3.get_access_type_by_key("GMAIL_EMOJI");
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_report_view.get_wnd(), access_type, url, referer, "binary", user_agent, post);
		return true;
	else
		return false;
	end
end


--- スレッド内の1通のメールを解析し、data に設定する
--
-- @param data MZ3Data オブジェクト
-- @param line 1通のメールに対応するHTML
--
function parse_one_mail(data, line, count)

	mz3.logger_debug("parse_one_mail(" .. count .. ")");

	if count==0 then
		-- 最初はヘッダーなので無視
		return;
	end

	if count>=2 then
		-- 2件目以降は子要素として投入する
		child = MZ3Data:create();
		parse_one_mail(child, line, 1);
		data:add_child(child);
		child:delete();
		return;
	end
--	mz3.logger_debug(line);

	-- 名前
	-- <h3>... <b>なまえ</b> ...</h3>
	name = line:match('<h3>.-<b>(.-)</b>');
	data:set_text("name", mz3.decode_html_entity(name));
	data:set_text("author", mz3.decode_html_entity(name));
	
	-- 日付
	-- <td align="right" valign="top"> 2009/05/24 8:17 <tr>
	date = line:match('<td align="right" valign="top"> (.-) <');
	if date ~= nil then
		date = date:gsub('<.->', '');
		data:set_date(date);
	end
	
	-- 本文
	body = line:match('<div class="msg"> ?(.*)$');
	
	if body ~= nil then
		-- 簡易HTML解析
		body = body:gsub('<WBR>', '');
		body = body:gsub('<wbr />', '');
		body = body:gsub('<b .->', "<b>");
		body = body:gsub('<p .->', "<p>");
		body = body:gsub('<h2.->(.-)</h2>', '<b>%1</b>');
		body = body:gsub('<b>', "\n<b>\n<br>");
		body = body:gsub('</b>', "<br>\n</b>\n");
		body = body:gsub('<br ?/>', "<br>");
		body = body:gsub('<font .->', "");
		body = body:gsub('</font>', "");
		body = body:gsub('<hr .->', "<hr>");
		body = body:gsub('<hr>', "<br>----------------------------------------<br>");

		body = body:gsub('<tr[^>]*>', "");
		body = body:gsub('<td[^>]*>', "");
		body = body:gsub('</tr>', "<br>");
		body = body:gsub('</td>', "");
		body = body:gsub('<table[^>]*>', "");
		body = body:gsub('</table>', "");
		body = body:gsub('<map.-</map>', '');

		-- 内部リンクの補完(/で始まる場合にホストを補完する)
		body = body:gsub('(<a .-href=")(/.-")', '%1' .. base_host .. '%2');
		body = body:gsub('(<img .-src=")(/.-")', '%1' .. base_host .. '%2');
		-- 内部リンクの補完(?で始まる場合にbaseを補完する)
		body = body:gsub('(<a .-href=")(\?.-")', '%1' .. base_host .. '%2');
		body = body:gsub('(<img .-src=")\?(.-")', '%1' .. base_url .. '%2');
		body = body:gsub("\r\n", "\n");
		body = body:gsub('^ *', '');

		-- <img タグだが src がないものは削除
		local post = 1;
		local start = 1;
		local body2 = '';
		while true do
			pos = body:find('<img', start, true);
			if pos == nil then
				body2 = body2 .. body:sub(start);
				break;
			else
				body2 = body2 .. body:sub(start, pos-1);
				img = body:match('<img .->', start);
				if line_has_strings(img, 'src=') then
					-- さらに絵文字であれば変換
					-- <img src="https://mail.google.com/mail/e/ezweb_ne_jp/B60" goomoji="ezweb_ne_jp.B60" ... />
					emoji_url, goomoji = img:match('src="(https://mail.google.com/mail/e/ezweb_ne_jp/.-)" goomoji="(.-)"');
					if emoji_url ~= nil and goomoji ~= nil then
						-- 未ロードの絵文字があればダウンロードする
						local idx = mz3_image_cache.get_image_index_by_url(emoji_url);
						if idx==-1 then
							-- 未ロードなのでダウンロード(予約)する
							table.insert(emoji_urls, emoji_url);
							body2 = body2 .. "[loading...]";
						else
							-- ロード済みなのでidxをらんらんビュー形式に変換する
							body2 = body2 .. "[g:" .. idx .. "]";
						end
					else
						body2 = body2 .. img;
					end
				end
				start = pos + img:len();
			end
		end

		body2 = body2:gsub('<a [^>]*></a>', "");
--		print(body2);

		data:add_text_array("body", "\r\n");
		data:add_body_with_extract(body2);
	end

end


----------------------------------------
-- イベントハンドラ
----------------------------------------

--- ボディリストのアイコンのインデックス取得
--
-- @param event_name    'get_end_binary_report_view'
-- @param serialize_key シリアライズキー(nil)
-- @param http_status   http status
-- @param url           url
--
-- @return (1) [bool] 成功時は true, 続行時は false
--
function on_get_end_binary_report_view(event_name, serialize_key, http_status, url, filename)

	mz3.logger_debug('on_get_end_binary_report_view', event_name, serialize_key, http_status, url, filename);
	if serialize_key == "GMAIL_EMOJI" then
		-- 保存
		local path = mz3.make_image_logfile_path_from_url_md5(url);
		mz3.logger_debug(path);
--		mz3.alert(path);
		mz3.copy_file(filename, path);
		
		-- 絵文字リストから削除
		table.remove(emoji_urls, 1);

		-- 未ロードの絵文字があればロード開始
		if get_next_emoji_url()==false then
			-- 全ロード完了
			-- とりあえず再度メールを受信する
			-- TODO 画面再描画のみにしたい
			url = mail_url;
			key = "GMAIL_MAIL";
			access_type = mz3.get_access_type_by_key(key);
			referer = '';
			user_agent = nil;
			post = nil;
			mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
		end

		return true;
	end

	return false;
end
mz3.add_event_listener("get_end_binary_report_view", "gmail.on_get_end_binary_report_view");

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


--- 全文表示メニューまたはダブルクリックイベント
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
	item = '';
	item = item .. "名前 : " .. data:get_text('name') .. "\r\n";
	item = item .. "日付 : " .. data:get_date() .. "\r\n";
	item = item .. "----\r\n";

	item = item .. data:get_text('title') .. "\r\n";
	
	item = item .. "\r\n";
	item = item .. data:get_text('url') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
end


--- レポートビューで開く
read_gmail_mail_first = true;
function on_read_by_reportview_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_by_reportview_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	-- メールパーサ内無限ループ対策のため、「メイン画面からの遷移フラグ」を立てておく
	read_gmail_mail_first = true;

	data = MZ3Data:create(data);

	-- 通信開始
	url = data:get_text('url');
	key = "GMAIL_MAIL";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);
	
	-- 絵文字ロード後の再ロード用
	-- TODO 画面再描画のみにしたい
	mail_url = url;

	return true;
end


--- ボディリストのダブルクリック(またはEnter)のイベントハンドラ
function on_body_list_click(serialize_key, event_name, data)

	if serialize_key=="GMAIL_MAIL" then
		-- レポートビューで開く
		return on_read_by_reportview_menu_item(serialize_key, event_name, data);

		-- ダブルクリックで全文表示したい場合は下記のコメントを外すこと
		-- 全文表示
--		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- 標準の処理を続行
	return false;
end
mz3.add_event_listener("dblclk_body_list", "gmail.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "gmail.on_body_list_click");


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
	if serialize_key~="GMAIL_MAIL" then
		return false;
	end

	-- インスタンス化
	body = MZ3Data:create(body);
	
	-- メニュー生成
	menu = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "最新の一覧を取得", IDM_CATEGORY_OPEN);
	menu:append_menu("string", "本文を読む...", menu_items.read_by_reportview);
	menu:append_menu("string", "ブラウザで開く...", menu_items.open_by_browser);

--	menu:append_menu("separator");
--	menu:append_menu("string", "スターを付ける...", menu_items.add_star);

	menu:append_menu("separator");
	menu:append_menu("string", "メールのプロパティ...", menu_items.read);

	-- ポップアップ
	menu:popup(wnd);
	
	-- メニューリソース削除
	menu:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu",  "gmail.on_popup_body_menu");


--- レポートビューのポップアップメニュー表示
--
-- @param event_name    'popup_report_menu'
-- @param serialize_key レポートアイテムのシリアライズキー
-- @param report_item   レポートアイテム
-- @param sub_item_idx  選択アイテムのインデックス
-- @param wnd           wnd
--
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


--- レポート画面からの返信時の書き込み種別の判定
--
-- @param event_name  'get_write_view_type_by_report_item_access_type'
-- @param report_item [MZ3Data] レポート画面の要素
--
function on_get_write_view_type_by_report_item_access_type(event_name, report_item)

	report_item = MZ3Data:create(report_item);
	
	serialize_key = report_item:get_serialize_key();
	service_type = mz3.get_service_type(serialize_key);
	if service_type=='gmail' then
		if serialize_key=='GMAIL_MAIL' then
			return true, mz3.get_access_type_by_key('GMAIL_SEND');
		end
	end

	return false;
end
mz3.add_event_listener("get_write_view_type_by_report_item_access_type", "gmail.on_get_write_view_type_by_report_item_access_type");


--- 書き込み画面の初期化イベント
--
-- @param event_name      'init_write_view'
-- @param write_view_type 書き込み種別
-- @param write_item      [MZ3Data] 書き込み画面の要素
--
function on_init_write_view(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if write_view_key=='GMAIL_SEND' then
		-- TODO タイトル変更：有効化

		-- タイトルの初期値設定
		local title = 'Re: ' .. write_item:get_text('title');
		mz3_write_view.set_text('title_edit', title);
		
		-- TODO 公開範囲コンボボックス：無効
		
		-- TODO フォーカス：本文から開始
		
		return true;
	end

	return false;
end
mz3.add_event_listener("init_write_view", "gmail.on_init_write_view");


--- 書き込み画面で「画像が添付可能なモードか」の判定
--
-- @param event_name      'is_enable_write_view_attach_image_mode'
-- @param write_view_type 書き込み種別
-- @param write_item      [MZ3Data] 書き込み画面の要素
--
function on_is_enable_write_view_attach_image_mode(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type=='gmail' then
		-- とりあえず添付不可
		return true, 0;
	end

	return false;
end
mz3.add_event_listener("is_enable_write_view_attach_image_mode", "gmail.on_is_enable_write_view_attach_image_mode");


--- 書き込み画面の書き込みボタン押下イベント
--
-- @param event_name      'click_write_view_send_button'
-- @param write_view_type 書き込み種別
-- @param write_item      [MZ3Data] 書き込み画面の要素
--
function on_click_write_view_send_button(event_name, write_view_type, write_item)

	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type=='gmail' then
		-- 送信処理
		
		local title = mz3_write_view.get_text('title_edit');
		local body  = mz3_write_view.get_text('body_edit');
		
		if title=='' then
			mz3.alert('タイトルを入力してください');
			return true;
		end
		
		local reply_form = write_item:get_text('reply_form');
		if reply_form==nil then
			mz3.alert('返信できません');
			return true;
		end
		
		-- <b>To:</b> <input type="hidden" name="qrr" value="o"> xxx@xxx.jp</td>
		-- <input type="radio" id="reply" name="qrr" value="o" checked> </td> <td colspan="2"> <label for="reply"><b>To:</b> NK &lt;xxx@xxx.jp&gt;</label> </td>
		local mail_to = reply_form:match('<input type="hidden" name="qrr" value="o".-> ?(.-)</');
		if mail_to==nil then
			mail_to = reply_form:match('<input type="radio" id="reply" name="qrr" value="o".-<b>To:</b> ?(.-)</');
		end
		msg = mz3.decode_html_entity(mail_to) .. ' にメールを送信します。よろしいですか？' .. "\r\n";
		msg = msg .. '----' .. "\r\n";
		msg = msg .. title .. "\r\n";
--		msg = msg .. '----';
		if mz3.confirm(msg, nil, "yes_no") ~= 'yes' then
			return true;
		end
		
		------------------------------------------
		-- POST パラメータ生成
		------------------------------------------

		-- URL 取得
		-- <form action="?v=b&qrt=n&..." name="qrf" method="POST"> 
		local url = reply_form:match('<form action="(.-)"');
		if url==nil then
			mz3.alert('返信できません(送信先url取得失敗)');
			return true;
		end
		url = write_item:get_text('base_url') .. url;
		--mz3.alert(url);
		
		-- hidden 値の収集
		local redir = reply_form:match('<input type="hidden" name="redir" value="(.-)"');
		redir = redir:gsub('&amp;', '&');
		local qrr   = reply_form:match('<input type="hidden" name="qrr" value="(.-)"');
		if qrr==nil then
			qrr = 'o';
		end
		
		
		-- POSTパラメータ生成
		post = MZ3PostData:create();
		post:set_content_type('multipart/form-data; boundary=---------------------------7d62ee108071e' .. '\r\n');
		
		-- nvp_bu_send
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="nvp_bu_send"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding('送信', 'sjis', 'utf8') .. '\r\n');
		
		-- redir
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="redir"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(redir, 'sjis', 'utf8') .. '\r\n');

		-- qrr
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="qrr"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(qrr, 'sjis', 'utf8') .. '\r\n');
		
		-- subject
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="subject"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(title, 'sjis', 'utf8'));
		post:append_post_body('\r\n');
		
		-- body
		post:append_post_body('-----------------------------7d62ee108071e' .. '\r\n');
		post:append_post_body('Content-Disposition: form-data; name="body"' .. '\r\n');
		post:append_post_body('\r\n');
		post:append_post_body(mz3.convert_encoding(body, 'sjis', 'utf8'));
--		mz3.alert(string.format('%c%c', 0x82, 0xa0));
--		post:append_post_body(string.format('%c%c%c', 0xEE, 0x95, 0x81));
		post:append_post_body('\r\n');
		-- ucs2 0xE541   = 1110 0101 0100 0001
		-- utf8 0xEE9581 = 1110 1110 1001 0101 1000 0001
		--                 ~~~~      ~~        ~~
		
		-- end of post data
		post:append_post_body('-----------------------------7d62ee108071e--' .. '\r\n');

		-- 通信開始
		access_type = mz3.get_access_type_by_key("GMAIL_SEND");
		referer = write_item:get_text('url');
		user_agent = nil;
		mz3.open_url(mz3_write_view.get_wnd(), access_type, url, referer, "text", user_agent, post.post_data);
		return true;
	end

	return false;
end
mz3.add_event_listener("click_write_view_send_button", "gmail.on_click_write_view_send_button");


--- 書き込み画面の書き込み完了イベント
--
-- @param event_name      'get_end_write_view'
-- @param write_view_type 書き込み種別
-- @param write_item      [MZ3Data] 書き込み画面の要素
-- @param http_status     HTTP Status Code (200, 404, etc...)
-- @param filename        レスポンスファイル
-- 
--
function on_get_end_write_view(event_name, write_view_type, write_item, http_status, filename)
	-- GMail メール送信では投稿後にリダイレクトするため get_end になる。
	
	-- CWriteView::OnPostEnd と同様の処理を行う。
	
	write_item = MZ3Data:create(write_item);
	
	write_view_key = mz3.get_serialize_key_by_access_type(write_view_type);
	service_type = mz3.get_service_type(write_view_key);
	if service_type~='gmail' then
		return false;
	end

	-- 投稿完了チェック
	if http_status==200 then
		-- 成功
		
		-- メッセージ表示
		mz3.alert('送信しました');
		
		-- 初期化
		mz3_write_view.set_text('title_edit', '');
		mz3_write_view.set_text('body_edit', '');
		
		-- 前の画面に戻る
		mz3.change_view('main_view');
		
	else
		-- 失敗
		mz3.logger_error('失敗:' .. http_status);
		
		mz3.alert('投稿に失敗しました。');
		
		-- TODO バックアップ
	end
	
	return true;
end
mz3.add_event_listener("get_end_write_view", "gmail.on_get_end_write_view");


mz3.logger_debug('gmail.lua end');
