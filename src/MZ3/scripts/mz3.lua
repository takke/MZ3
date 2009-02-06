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
-- TODO mz3_plugin_dir support
--print(package.path);
package.path = mz3_script_dir .. "\\?.lua;" .. mz3_script_dir .. "\\?\\init.lua";

-- 各種ビルトインスクリプトロード
require("util");
require("mixi");

mz3.logger_info('mz3.lua end');
