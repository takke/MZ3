--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi.lua start');
module("mixi", package.seeall)

--------------------------------------------------
-- 次へ、前への抽出処理
--------------------------------------------------
function parse_next_back_link(line, base_url)

	local back_data = nil;
	local next_data = nil;
	
	-- <ul><li><a href="new_bbs.pl?page=1">前を表示</a></li>
	-- <li>51件～100件を表示</li>
	-- <li><a href="new_bbs.pl?page=3">次を表示</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- 前
		local url, t = line:match([[href="([^"]+)">(前[^<]+)<]]);
		if url~=nil then
			back_data = MZ3Data:create();
			back_data:set_text("title", "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- 次
		local url, t = line:match([[href="([^"]+)">(次[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text("title", "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end


----------------------------------------
-- アクセス種別の登録
----------------------------------------
-- 逆あしあと
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- カテゴリ
type:set_service_type('mixi');								-- サービス種別
type:set_serialize_key('MIXI_SHOW_SELF_LOG');				-- シリアライズキー
type:set_short_title('逆あしあと');							-- 簡易タイトル
type:set_request_method('GET');								-- リクエストメソッド
type:set_cache_file_pattern('mixi\\show_self_log.html');	-- キャッシュファイル
type:set_request_encoding('euc-jp');						-- エンコーディング
type:set_default_url('http://mixi.jp/show_self_log.pl');
type:set_body_header(1, 'title', '名前');
type:set_body_header(2, 'date', '日付');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
--mz3.logger_debug(type);

-- 公式メッセージ
type = mz3_access_type_info.new_access_type();
mz3_access_type_info.set_info_type(type, 'category');							-- カテゴリ
mz3_access_type_info.set_service_type(type, 'mixi');							-- サービス種別
mz3_access_type_info.set_serialize_key(type, 'MIXI_LIST_MESSAGE_OFFICIAL');		-- シリアライズキー
mz3_access_type_info.set_short_title(type, '公式メッセージ');					-- 簡易タイトル
mz3_access_type_info.set_request_method(type, 'GET');							-- リクエストメソッド
mz3_access_type_info.set_cache_file_pattern(type, 'mixi\\list_message_official.html');	-- キャッシュファイル
mz3_access_type_info.set_request_encoding(type, 'euc-jp');						-- エンコーディング
mz3_access_type_info.set_default_url(type, 'http://mixi.jp/list_message.pl?box=noticebox');
mz3_access_type_info.set_body_header(type, 1, 'title', '件名');
mz3_access_type_info.set_body_header(type, 2, 'name', '差出人>>');
mz3_access_type_info.set_body_header(type, 3, 'date', '日付>>');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%2 %3');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%1');


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
--	mz3.logger_debug('on_creating_default_group');
	
	-- サポートするサービス種別の取得(スペース区切り)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then
		-- その他/逆あしあと 追加
		local tab = mz3_group_data.get_group_item_by_name(group, 'その他');
		mz3_group_item.append_category(tab, "逆あしあと", "MIXI_SHOW_SELF_LOG", "http://mixi.jp/show_self_log.pl");

		-- メッセージ/公式メッセージ 追加
		local tab = mz3_group_data.get_group_item_by_name(group, 'メッセージ');
		mz3_group_item.append_category(tab, "公式メッセージ", "MIXI_LIST_MESSAGE_OFFICIAL", "http://mixi.jp/list_message.pl?box=noticebox");
	end
end

----------------------------------------
-- パーサのロード＆登録
----------------------------------------
-- ★リスト系
-- コミュニティ最新書込一覧
require("scripts\\mixi\\mixi_new_bbs_parser");
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- コミュニティコメント記入履歴 : 最新書込一覧と同一
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");

-- 逆あしあと
require("scripts\\mixi\\mixi_show_self_log_parser");
mz3.set_parser("MIXI_SHOW_SELF_LOG", "mixi.mixi_show_self_log_parser");

-- 公式メッセージ
require("scripts\\mixi\\mixi_new_official_message_parser");
mz3.set_parser("MIXI_LIST_MESSAGE_OFFICIAL", "mixi.mixi_new_official_message_parser");

-- トップページ
require("scripts\\mixi\\mixi_home_parser");
mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");


----------------------------------------
-- イベントフック関数の登録
----------------------------------------
-- デフォルトのグループリスト生成
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group");

mz3.logger_debug('mixi.lua end');
