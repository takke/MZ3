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
MAIN_VIEW_POST_MODE_MIXI_VOICE 				= 2;
MAIN_VIEW_POST_MODE_MIXI_VOICE_REPLY 		= 3;
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
VK_SPACE  = 0x20;
VK_BACK   = 0x08;
VK_END    = 0x23;
VK_F1     = 0x70;
VK_F2     = 0x71;
VK_PRIOR  = 0x21;	-- PageUp
VK_NEXT   = 0x22;	-- PageDown
VK_A      = 0x41;
VK_B      = 0x42;
VK_C      = 0x43;
VK_D      = 0x44;
VK_E      = 0x45;
VK_F      = 0x46;
VK_G      = 0x47;
VK_H      = 0x48;
VK_I      = 0x49;
VK_J      = 0x4A;
VK_K      = 0x4B;
VK_L      = 0x4C;
VK_M      = 0x4D;
VK_N      = 0x4E;
VK_O      = 0x4F;
VK_P      = 0x50;
VK_Q      = 0x51;
VK_R      = 0x52;
VK_S      = 0x53;
VK_T      = 0x54;
VK_U      = 0x55;
VK_V      = 0x56;
VK_W      = 0x57;
VK_X      = 0x58;
VK_Y      = 0x59;
VK_Z      = 0x5A;

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


-- メイン画面下ペインのリストのページ単位スクロール
mz3_main_view.scroll_body_list_page = function (page)
	local cpp = mz3_main_view.get_body_list_count_per_page();
	local idx = mz3_main_view.get_selected_body_item_idx();
	local list = MZ3DataList:create(mz3_main_view.get_body_item_list());
	local n = list:get_count();
	idx = idx + page * (cpp -1);
	if idx > n-1 then
		idx = n-1;
	end
	if idx < 0 then
		idx = 0;
	end
	mz3_main_view.select_body_item(idx);
end


--- メイン画面のキー押下イベント
mz3.on_keyup_main_view = function(event_name, key, is_shift, is_ctrl, is_alt)
--	mz3.logger_debug('mz3.on_keyup_main_view : (' .. event_name .. ', ' .. string.format('0x%2x', key) .. ')');

	local focus = mz3_main_view.get_focus();
	if focus ~= "edit" then
		-- たいていのキーアサインは入力エリアで無効にすべき

		if key == VK_J then
			-- down
			mz3.keybd_event(VK_DOWN, "keydown");
			mz3.keybd_event(VK_DOWN, "keyup");
			return true;
		end

		if key == VK_K then
			-- up
			mz3.keybd_event(VK_UP, "keydown");
			mz3.keybd_event(VK_UP, "keyup");
			return true;
		end

		if key == VK_H then
			-- left
			mz3.keybd_event(VK_LEFT, "keydown");
			mz3.keybd_event(VK_LEFT, "keyup");
			return true;
		end

		if key == VK_L then
			-- left
			mz3.keybd_event(VK_RIGHT, "keydown");
			mz3.keybd_event(VK_RIGHT, "keyup");
			return true;
		end

		if key == VK_SPACE or key == VK_O then
			-- 最新TLの取得
			mz3_main_view.retrieve_category_item();
			return true;
		end

		if key == VK_NEXT and is_ctrl ~= 0 then
			-- Ctrl+PageDown : 次のタブ
			mz3.exec_mz3_command('NEXT_TAB');
			return true;
		end

		if key == VK_PRIOR and is_ctrl ~= 0 then
			-- Ctrl+PageUp : 前のタブ
			mz3.exec_mz3_command('PREV_TAB');
			return true;
		end

		-- MZ3 only
		local app_name = mz3.get_app_name();
		if app_name=="MZ3" or app_name=="TkTweets" then
			if key == VK_S then
				-- フォントを小さく
				mz3.exec_mz3_command("FONT_SHRINK");
				return true;
			end

			if key == VK_W then
				-- フォントを大きく
				mz3.exec_mz3_command("FONT_MAGNIFY");
				return true;
			end
		end
	end

	if focus == "category_list" then
		-- カテゴリリスト
	elseif focus == "body_list" then
		-- ボディリスト

		if key == VK_RIGHT then

			-- 標準動作のスキップ
			return true;
		end

		if key == VK_LEFT then

			-- 標準動作のスキップ
			return true;
		end
	end
	return false;
end
mz3.add_event_listener("keyup_main_view", "mz3.on_keyup_main_view");


--- メイン画面のキー押下イベント
mz3.on_keydown_main_view = function(event_name, key, is_shift, is_ctrl, is_alt)
--	mz3.logger_debug('mz3.on_keydown_main_view : (' .. event_name .. ', ' .. string.format('0x%2x', key) .. ')');

	local focus = mz3_main_view.get_focus();
	if focus == "category_list" then
		-- カテゴリリスト
	elseif focus == "body_list" then
		-- ボディリスト

		if key == VK_RIGHT then
			-- 画面単位のスクロール
			mz3_main_view.scroll_body_list_page(1);
			
			-- PageDown シミュレートでは MZ3 でフォーカス移動になるので使わない
--			mz3.keybd_event(VK_NEXT, "keydown");
--			mz3.keybd_event(VK_NEXT, "keyup");

--			mz3.exec_mz3_command('NEXT_TAB');

			-- Ver.1.3.4 以前のデフォルト動作
--			mz3.exec_mz3_command('CHANGE_MAIN_BODY_HEADER_MODE');

			-- 標準動作のスキップ
			return true;
		end

		if key == VK_LEFT then
			-- 画面単位のスクロール
			mz3_main_view.scroll_body_list_page(-1);

			-- PageUp シミュレートでは MZ3 でフォーカス移動になるので使わない
--			mz3.keybd_event(VK_PRIOR, "keydown");
--			mz3.keybd_event(VK_PRIOR, "keyup");

--			mz3.exec_mz3_command('PREV_TAB');

			-- Ver.1.3.4 以前のデフォルト動作
--			mz3.exec_mz3_command('SHORT_CUT_MOVE_ON_MAIN_BODY_LIST');

			-- 標準動作のスキップ
			return true;
		end
	end
	return false;
end
mz3.add_event_listener("keydown_main_view", "mz3.on_keydown_main_view");


mz3.on_keydown_detail_view = function (event_name, serialize_key, data, key)
	mz3.logger_debug('mz3.on_keydown_detail_view : (' .. serialize_key .. ', ' .. event_name .. ', ' .. key .. ')');

	if key == VK_J then
		-- down
		mz3.keybd_event(VK_DOWN, "keydown");
		mz3.keybd_event(VK_DOWN, "keyup");
		return true;
	end

	if key == VK_K then
		-- up
		mz3.keybd_event(VK_UP, "keydown");
		mz3.keybd_event(VK_UP, "keyup");
		return true;
	end

	return false;
end
mz3.add_event_listener("keydown_detail_view", "mz3.on_keydown_detail_view");


-------------------------------------------------
-- 各種ビルトインスクリプトロード
-------------------------------------------------
require("scripts\\base64");		-- BASE64
--require("scripts\\sha1");		-- SHA-1

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
--	require("scripts\\wassr");			-- Wassr
--	require("scripts\\goohome");		-- goohome
	require("scripts\\RSS");			-- RSS
	require("scripts\\2ch");			-- 2ch
	require("scripts\\auone");			-- auone
end

mz3.logger_info('mz3.lua end');
