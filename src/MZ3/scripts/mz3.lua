--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : �r���g�C���X�N���v�g
--
-- $Id$
--------------------------------------------------
mz3.logger_info('mz3.lua start');

-- �p�b�P�[�W���[�h�p�X�̕ύX
--print(package.path);
package.path = ".\\?.luac;.\\?.lua;" .. mz3_dir .. "?.luac;" .. mz3_dir .. "?.lua;"
--print(package.path);

-------------------------------------------------
-- MZ3 �p�萔�ݒ�
-------------------------------------------------

-- mz3_menu.append_menu �p�萔
IDM_CATEGORY_OPEN  = 34164  -37000;		-- �ŐV�̈ꗗ���擾
ID_REPORT_URL_BASE = 36100  -37000;		-- URL���J��

ID_BACK_MENU       = 32873  -37000;		-- �߂�
ID_WRITE_COMMENT   = 32802  -37000;		-- ��������(�ԐM�A�Ȃ�)
IDM_RELOAD_PAGE    = 32885  -37000;		-- �ēǍ�
ID_EDIT_COPY       = 0xE122 -37000;		-- �R�s�[
ID_OPEN_BROWSER    = 32830  -37000;		-- �u���E�U�ŊJ���i���̃y�[�W�j...
IDM_LAYOUT_REPORTLIST_MAKE_NARROW = 32964 -37000;	-- ���X�g����������
IDM_LAYOUT_REPORTLIST_MAKE_WIDE   = 32966 -37000;	-- ���X�g���L������


-- mz3_main_view.set_post_mode �p�萔
MAIN_VIEW_POST_MODE_TWITTER_UPDATE 			= 0;
MAIN_VIEW_POST_MODE_TWITTER_DM 				= 1;
MAIN_VIEW_POST_MODE_MIXI_ECHO 				= 2;
MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY 		= 3;
MAIN_VIEW_POST_MODE_WASSR_UPDATE			= 4;
MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE	= 5;


-- ���C����� ViewStyle �萔
VIEW_STYLE_DEFAULT = 0;	-- �W���X�^�C��
VIEW_STYLE_IMAGE   = 1;	-- �W���X�^�C��+ImageIcon
VIEW_STYLE_TWITTER = 2;	-- �W���X�^�C��+ImageIcon+StatusEdit (Twitter)


----------------------------------------------------------------------
-- MZ3 ���ʃn���h��(�e�T�[�r�X���ɏ����ƒx���Ȃ�悤�ȃn���h��)
----------------------------------------------------------------------

--- �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X�擾
--
-- @param event_name    'creating_default_group'
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param body          body data
--
-- @return (1) [bool] �������� true, ���s���� false
-- @return (2) [int] �A�C�R���C���f�b�N�X
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


--- �N���X�|�X�g�Ǘ��f�[�^������������
--
-- @param from �ǂ����珉���|�X�g�������B�Ǘ��f�[�^�ɂ��̃f�[�^��o�^���Ȃ��悤�ɂ��邽�߁B
--             "twitter", "wassr", "echo" ���T�|�[�g�B
--
function mz3.init_cross_post_info(from)
	mz3.cross_posts = {}
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
		local msg = "Twitter �ɂ����e���܂����H\r\n";
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
		local msg = "Wassr �ɂ����e���܂����H\r\n";
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
		local msg = "mixi echo �ɂ����e���܂����H\r\n";
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
-- �e��r���g�C���X�N���v�g���[�h
-------------------------------------------------
require("scripts\\util");		-- ���[�e�B���e�B
require("scripts\\wrapper");	-- API���b�p�[�N���X
-- �e�T�[�r�X�p�X�N���v�g���[�h
require("scripts\\mixi");		-- mixi
require("scripts\\twitter");	-- Twitter
require("scripts\\gmail");		-- GMail
require("scripts\\wassr");		-- Wassr
require("scripts\\goohome");	-- goohome
require("scripts\\RSS");		-- RSS
require("scripts\\2ch");		-- 2ch
require("scripts\\auone");		-- auone

mz3.logger_info('mz3.lua end');
