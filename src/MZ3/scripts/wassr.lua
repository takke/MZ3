--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : wassr
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('wassr.lua start');
module("wassr", package.seeall)

----------------------------------------
-- サービスの登録(タブ初期化用)
----------------------------------------
mz3.regist_service('Wassr', false);

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
	if services:find(' Wassr', 1, true) ~= nil then

		-- Wassrタブ追加
		local tab = MZ3GroupItem:create("Wassr");
		tab:append_category("タイムライン", "WASSR_FRIENDS_TIMELINE");
		tab:append_category("返信一覧", "WASSR_FRIENDS_TIMELINE", "http://api.wassr.jp/statuses/replies.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end

----------------------------------------
-- イベントハンドラの登録
----------------------------------------

-- デフォルトのグループリスト生成
mz3.add_event_listener("creating_default_group", "wassr.on_creating_default_group", false);

mz3.logger_debug('wassr.lua end');
