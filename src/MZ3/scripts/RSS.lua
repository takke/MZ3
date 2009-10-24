--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : wassr
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('RSS.lua start');
module("RSS", package.seeall)

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('RSS', true);

----------------------------------------
-- アクセス種別の登録
----------------------------------------
-- TODO

----------------------------------------
-- メニュー項目登録(静的に用意すること)
----------------------------------------
menu_items = {}
-- TODO


----------------------------------------
-- サービス用関数
----------------------------------------

----------------------------------------
-- イベントハンドラ
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
	if services:find(' RSS', 1, true) ~= nil then

		-- RSSタブ追加
		local tab = MZ3GroupItem:create("RSS");
		tab:append_category("はてブ 最近の人気エントリー", "RSS_FEED", "http://b.hatena.ne.jp/hotentry?mode=rss");
--		tab:append_category("しょこたん☆ぶろぐ", "RSS_FEED", "http://blog.excite.co.jp/shokotan/index.xml");
		tab:append_category("CNET Japan", "RSS_FEED", "http://japan.cnet.com/rss/index.rdf");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end
mz3.add_event_listener("creating_default_group", "RSS.on_creating_default_group", false);


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
	if service_type=='RSS' then
		return true, VIEW_STYLE_IMAGE;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "RSS.on_get_view_style");


mz3.logger_debug('RSS.lua end');
