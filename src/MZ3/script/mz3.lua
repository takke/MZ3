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
-- TODO mz3_plugin_dir support
--print(package.path);
package.path = mz3_script_dir .. "\\?.lua;" .. mz3_script_dir .. "\\?\\init.lua";

-- �e��r���g�C���X�N���v�g���[�h
require("util");
require("mixi");

mz3.logger_info('mz3.lua end');
