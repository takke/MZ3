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
type = mz3_access_type_info.new_access_type();
mz3_access_type_info.set_info_type(type, 'category');							-- カテゴリ
mz3_access_type_info.set_service_type(type, 'mixi');							-- サービス種別
mz3_access_type_info.set_serialize_key(type, 'MIXI_SHOW_SELF_LOG');				-- シリアライズキー
mz3_access_type_info.set_short_title(type, '逆あしあと');						-- 簡易タイトル
mz3_access_type_info.set_request_method(type, 'GET');							-- リクエストメソッド
mz3_access_type_info.set_cache_file_pattern(type, 'mixi\\show_self_log.html');	-- キャッシュファイル
mz3_access_type_info.set_request_encoding(type, 'euc-jp');						-- エンコーディング
mz3_access_type_info.set_default_url(type, 'http://mixi.jp/show_self_log.pl');
mz3_access_type_info.set_body_header(type, 1, 'title', '名前');
mz3_access_type_info.set_body_header(type, 2, 'date', '日付');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%1');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%2');
--mz3.logger_debug(type);


----------------------------------------
-- メニューへの登録
----------------------------------------
-- TODO
-- local group = mz3_category.get_group_by_name('その他');
-- mz3_category.append_category(group, "逆あしあと", "MIXI_SHOW_SELF_LOG");

----------------------------------------
-- パーサロード
----------------------------------------
-- コミュニティ最新書込一覧
require("scripts\\mixi\\mixi_new_bbs_parser");
-- 逆あしあと
require("scripts\\mixi\\mixi_show_self_log_parser");

----------------------------------------
-- パーサの登録
----------------------------------------
-- コミュニティ最新書き込み一覧
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- コミュニティコメント記入履歴
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");
-- 逆あしあと
mz3.set_parser("MIXI_SHOW_SELF_LOG", "mixi.mixi_show_self_log_parser");

----------------------------------------
-- イベントフック関数の登録
----------------------------------------
--mz3.set_hook("mixi", "after_parse", after_parse);

mz3.logger_debug('mixi.lua end');
