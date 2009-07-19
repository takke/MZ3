--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : ビルトインスクリプト
--
-- $Id$
--------------------------------------------------
mz3.logger_info('mz3.lua start');

-- パッケージロードパスの変更
--print(package.path);
package.path = ".\\?.luac;.\\?.lua;" .. mz3_dir .. "?.luac;" .. mz3_dir .. "?.lua;"
--print(package.path);

-------------------------------------------------
-- MZ3 用定数設定
-------------------------------------------------

-- mz3_menu.append_menu 用定数
IDM_CATEGORY_OPEN  = 34164  -37000;		-- 最新の一覧を取得
ID_REPORT_URL_BASE = 36100  -37000;		-- URLを開く

ID_BACK_MENU       = 32873  -37000;		-- 戻る
ID_WRITE_COMMENT   = 32802  -37000;		-- 書き込み(返信、など)
IDM_RELOAD_PAGE    = 32885  -37000;		-- 再読込
ID_EDIT_COPY       = 0xE122 -37000;		-- コピー
ID_OPEN_BROWSER    = 32830  -37000;		-- ブラウザで開く（このページ）...
IDM_LAYOUT_REPORTLIST_MAKE_NARROW = 32964 -37000;	-- リストを狭くする
IDM_LAYOUT_REPORTLIST_MAKE_WIDE   = 32966 -37000;	-- リストを広くする


-- mz3_main_view.set_post_mode 用定数
MAIN_VIEW_POST_MODE_TWITTER_UPDATE 			= 0;
MAIN_VIEW_POST_MODE_TWITTER_DM 				= 1;
MAIN_VIEW_POST_MODE_MIXI_ECHO 				= 2;
MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY 		= 3;
MAIN_VIEW_POST_MODE_WASSR_UPDATE			= 4;
MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE	= 5;

-- メイン画面 ViewStyle 定数
VIEW_STYLE_DEFAULT = 0;	-- 標準スタイル
VIEW_STYLE_IMAGE   = 1;	-- 標準スタイル+ImageIcon
VIEW_STYLE_TWITTER = 2;	-- 標準スタイル+ImageIcon+StatusEdit (Twitter)


----------------------------------------------------------------------
-- MZ3 共通ハンドラ(各サービス毎に書くと遅くなるようなハンドラ)
----------------------------------------------------------------------

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

	if serialize_key == "MIXI_BBS"        then return true, 0; end
	if serialize_key == "MIXI_EVENT"      then return true, 1; end
	if serialize_key == "MIXI_ENQUETE"    then return true, 2; end
	if serialize_key == "MIXI_EVENT_JOIN" then return true, 3; end
	if serialize_key == "MIXI_BIRTHDAY"   then return true, 4; end
	if serialize_key == "MIXI_SCHEDULE"   then return true, 5; end
	if serialize_key == "MIXI_MESSAGE"    then return true, 7; end
	if serialize_key == "RSS_ITEM"        then return true, 8; end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "mixi.on_get_body_list_default_icon_index");

-------------------------------------------------
-- 各種ビルトインスクリプトロード
-------------------------------------------------
require("scripts\\util");		-- ユーティリティ
require("scripts\\wrapper");	-- APIラッパークラス
-- 各サービス用スクリプトロード
require("scripts\\mixi");		-- mixi
require("scripts\\twitter");	-- Twitter
require("scripts\\gmail");		-- GMail
require("scripts\\wassr");		-- Wassr
require("scripts\\goohome");	-- goohome
require("scripts\\RSS");		-- RSS
require("scripts\\2ch");		-- 2ch

mz3.logger_info('mz3.lua end');
