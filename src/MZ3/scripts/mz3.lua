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
package.path = ".\\?.lua;" .. mz3_dir .. "\\?.lua;"
--print(package.path);

-- �e��r���g�C���X�N���v�g���[�h
require("scripts\\util");
require("scripts\\mixi");

mz3.logger_info('mz3.lua end');
