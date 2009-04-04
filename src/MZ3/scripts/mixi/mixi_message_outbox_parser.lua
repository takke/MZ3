--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- アクセス種別の登録
----------------------------------------

type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('mixi');									-- サービス種別
type:set_serialize_key('MESSAGE_OUT');							-- シリアライズキー
type:set_short_title('メッセージ(送信箱)');						-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('mixi\\list_message_outbox.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');							-- エンコーディング
type:set_default_url('http://mixi.jp/list_message.pl?box=outbox');
type:set_body_header(1, 'title', '件名');
type:set_body_header(2, 'name', '差出人>>');
type:set_body_header(3, 'date', '日付>>');
type:set_body_integrated_line_pattern(1, '%2 %3');
type:set_body_integrated_line_pattern(2, '%1');

--------------------------------------------------
-- 【mixi メッセージ(送信箱)】
-- [list] list_message.pl?box=outbox 用パーサ
--
-- http://mixi.jp/list_message.pl?box=outbox
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_message_outbox_parser(parent, body, html)
	mz3.logger_debug("mixi_message_outbox_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	local in_data_region = false;
	
	local back_data = nil;
	local next_data = nil;

	-- 行数取得
	local line_count = html:get_count();
	for i=140, line_count-1 do
		line = html:get_at(i);

		-- 次へ、前への抽出処理
		-- 項目発見前にのみ存在する
		if not in_data_region and back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "list_message.pl");
		end

		-- 項目探索
		if line_has_strings(line, "td", "class", "subject") then

			in_data_region = true;

			-- data 生成
			data = MZ3Data:create();

			--	件名
			--	<td class="subject">
			--		<a href="view_message.pl?id=*****&box=inbox&page=1">
			--			件名
			--		</a>
			--	</td>
			-- 件名
			title, after = line:match(">([^<]+)(<.*)$");
			title = mz3.decode_html_entity(title);
			data:set_text("title", title);

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- 1行戻る
			i = i -1;
			line2 = html:get_at(i);

			-- 名前
			name, after = line2:match(">([^<]+)(<.*)$");
			name = mz3.decode_html_entity(name);
			data:set_text("name", name);
			data:set_text("author", name);

			-- 2行進む
			i = i +2;
			line3 = html:get_at(i);

			-- 日付取得
			-- <td class="date">03月10日<a id=
			date, after = line3:match(">([^<]+)(<.*)$");
			data:set_date( date );

			-- URL 取得
			url = line:match("href=\"([^\"]+)\"");
			data:set_text("url", url);

			-- id
			id = get_param_from_url(url, "id");
			data:set_integer("id", id);

			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			data:set_access_type(type);

			-- data 追加
			body:add(data.data);

			-- data 削除
			data:delete();
		end

		if in_data_region and line_has_strings(line, "</ul>") then
			mz3.logger_debug("★</ul>が見つかったので終了します");
			break;
		end

	end

	-- 前、次へリンクの追加
	if back_data~=nil then
		-- 先頭に挿入
		body:insert(0, back_data.data);
		back_data:delete();
	end
	if next_data~=nil then
		-- 末尾に追加
		body:add(next_data.data);
		next_data:delete();
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_message_outbox_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- パーサの登録
----------------------------------------
-- 送信箱
mz3.set_parser("MESSAGE_OUT", "mixi.mixi_message_outbox_parser");


----------------------------------------
-- メニューへの登録
----------------------------------------

--- デフォルトのグループリスト生成イベントハンドラ
--
-- @param serialize_key シリアライズキー(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group_for_mixi_message_outbox(serialize_key, event_name, group)

	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then

		-- 送信箱
		local tab = mz3_group_data.get_group_item_by_name(group, 'メッセージ');
		mz3_group_item.append_category(tab, "メッセージ(送信箱)", "MESSAGE_OUT", "http://mixi.jp/list_message.pl?box=outbox");

	end
end
-- イベントフック関数の登録
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group_for_mixi_message_outbox");


----------------------------------------
-- estimate 対象に追加
----------------------------------------

--- estimate 対象判別イベントハンドラ
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        解析対象URL
--
function on_estimate_access_type_by_url_for_mixi_message_outbox(event_name, url, data1, data2)

	if line_has_strings(url, 'list_message.pl', 'box=outbox') then
		return true, mz3.get_access_type_by_key('MESSAGE_OUT');
	end
	
	return false;
end
-- イベントフック関数の登録
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type_by_url_for_mixi_message_outbox");
