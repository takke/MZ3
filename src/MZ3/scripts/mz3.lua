--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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


-- メイン画面 カテゴリ取得時のハンドラ用定数
RETRIEVE_CATEGORY_ITEM_RVAL_LOCALSTORAGE = 1;	-- ローカルストレージ経由で取得
RETRIEVE_CATEGORY_ITEM_RVAL_INETACCESS = 0;		-- インターネットにアクセス
RETRIEVE_CATEGORY_ITEM_RVAL_DONE = 2;			-- ハンドラ側で処理完了、何もしない

-- mz3_graphics.draw_text() 用定数
DT_WORDBREAK    = 0x00000010;
DT_NOPREFIX     = 0x00000800;
DT_EDITCONTROL  = 0x00002000;
DT_BOTTOM       = 0x00000008;
DT_LEFT         = 0x00000000;
DT_RIGHT        = 0x00000002;
DT_END_ELLIPSIS = 0x00008000;

-- Key map
VK_DOWN   = 0x28;
VK_UP     = 0x26;
VK_RIGHT  = 0x27;
VK_LEFT   = 0x25;
VK_RETURN = 0x0D;
VK_ESCAPE = 0x1B;
VK_BACK   = 0x08;
VK_F1     = 0x70;
VK_F2     = 0x71;

----------------------------------------------------------------------
-- MZ3 共通ハンドラ(各サービス毎に書くと遅くなるようなハンドラ)
----------------------------------------------------------------------

--- クロスポスト管理データを初期化する
--
-- @param from どこから初期ポストしたか。管理データにそのデータを登録しないようにするため。
--             "twitter", "wassr", "echo" をサポート。
--
function mz3.init_cross_post_info(from)
	mz3.cross_posts = {}	-- どのサービスへクロスポストするか(queue)
	
	if from ~= "twitter" and use_cross_post_to_twitter then
		table.insert(mz3.cross_posts, "twitter");
	end
	if from ~= "wassr" and use_cross_post_to_wassr then
		table.insert(mz3.cross_posts, "wassr");
	end
	if from ~= "echo" and use_cross_post_to_echo then
		table.insert(mz3.cross_posts, "echo");
	end

--	mz3.alert(table.concat(mz3.cross_posts, ','));

end

function mz3.do_cross_post()

--	mz3.alert(#mz3.cross_posts);

	if #mz3.cross_posts <= 0 then
		return false;
	end

	local target = table.remove(mz3.cross_posts, 1);
	if target == "twitter" then
		text = mz3_main_view.get_edit_text();
		local msg = "Twitter にも投稿しますか？\r\n";
		msg = msg .. "----\r\n";
		msg = msg .. text .. "\r\n";
		msg = msg .. "----\r\n";
		
		if mz3.confirm(msg, nil, "yes_no") == "yes" then
			twitter.do_post_to_twitter(text);
			return true;
		end
	end
	if target == "wassr" then
		text = mz3_main_view.get_edit_text();
		local msg = "Wassr にも投稿しますか？\r\n";
		msg = msg .. "----\r\n";
		msg = msg .. text .. "\r\n";
		msg = msg .. "----\r\n";
		
		if mz3.confirm(msg, nil, "yes_no") == "yes" then
			wassr.do_post_to_wassr(text);
			return true;
		end
	end
	if target == "echo" then
		text = mz3_main_view.get_edit_text();
		local msg = "mixi echo にも投稿しますか？\r\n";
		msg = msg .. "----\r\n";
		msg = msg .. text .. "\r\n";
		msg = msg .. "----\r\n";
		
		if mz3.confirm(msg, nil, "yes_no") == "yes" then
			mixi.do_post_to_echo(text);
			return true;
		end
	end

	return false;
end

-------------------------------------------------
-- 各種ビルトインスクリプトロード
-------------------------------------------------
require("scripts\\util");		-- ユーティリティ
require("scripts\\wrapper");	-- APIラッパークラス
-- 各サービス用スクリプトロード
local build_type = mz3.get_app_build_type();
if build_type == "MZ3" then
	require("scripts\\mixi");		-- mixi
end

require("scripts\\twitter");	-- Twitter

if build_type == "MZ3" then
	require("scripts\\gmail");			-- Gmail
	require("scripts\\google_reader");	-- GoogleReader
	require("scripts\\wassr");			-- Wassr
	require("scripts\\goohome");		-- goohome
	require("scripts\\RSS");			-- RSS
	require("scripts\\2ch");			-- 2ch
	require("scripts\\auone");			-- auone
end

mz3.logger_info('mz3.lua end');
