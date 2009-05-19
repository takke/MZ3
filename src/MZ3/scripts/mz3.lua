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
IDM_CATEGORY_OPEN = 34164 -37000;	-- 最新の一覧を取得
ID_REPORT_URL_BASE = 36100 -37000;	-- URLを開く

-- mz3_main_view.set_post_mode 用定数
MAIN_VIEW_POST_MODE_TWITTER_UPDATE 			= 0;
MAIN_VIEW_POST_MODE_TWITTER_DM 				= 1;
MAIN_VIEW_POST_MODE_MIXI_ECHO 				= 2;
MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY 		= 3;
MAIN_VIEW_POST_MODE_WASSR_UPDATE			= 4;
MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE	= 5;

-------------------------------------------------
-- 各種ビルトインスクリプトロード
-------------------------------------------------
require("scripts\\util");		-- ユーティリティ
require("scripts\\wrapper");	-- APIラッパークラス
-- 各サービス用スクリプトロード
require("scripts\\mixi");		-- mixi
require("scripts\\twitter");	-- Twitter
require("scripts\\wassr");		-- Wassr
require("scripts\\goohome");	-- goohome
require("scripts\\RSS");		-- RSS

mz3.logger_info('mz3.lua end');
