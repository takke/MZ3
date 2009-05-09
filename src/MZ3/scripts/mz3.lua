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
IDM_CATEGORY_OPEN = 34164 -37000;	-- �ŐV�̈ꗗ���擾

-- mz3_main_view.set_post_mode �p�萔
MAIN_VIEW_POST_MODE_TWITTER_UPDATE 			= 0;
MAIN_VIEW_POST_MODE_TWITTER_DM 				= 1;
MAIN_VIEW_POST_MODE_MIXI_ECHO 				= 2;
MAIN_VIEW_POST_MODE_MIXI_ECHO_REPLY 		= 3;
MAIN_VIEW_POST_MODE_WASSR_UPDATE			= 4;
MAIN_VIEW_POST_MODE_GOOHOME_QUOTE_UPDATE	= 5;

-------------------------------------------------
-- �e��r���g�C���X�N���v�g���[�h
-------------------------------------------------
require("scripts\\util");		-- ���[�e�B���e�B
require("scripts\\wrapper");	-- API���b�p�[�N���X
require("scripts\\mixi");		-- mixi
require("scripts\\twitter");	-- twitter
require("scripts\\goohome");	-- goohome

mz3.logger_info('mz3.lua end');
