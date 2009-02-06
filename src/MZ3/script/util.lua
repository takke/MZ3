--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : ユーティリティ
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('util.lua start');

--------------------------------------------------
-- 関数呼び出しのバックトレースを出力する
--------------------------------------------------
function show_backtrace()
	-- level=1 が この関数
	-- level=2 が この関数の呼び出しもと(wait_loop)
	-- level=3 が フック関数
	-- これらの関数の表示を避けています。
	local level = 4

	print("backtrace:")
	while true do
		local stacktrace = debug.getinfo(level, "nlS")
		if stacktrace == nil then break end
		print("    function: ", stacktrace.name, stacktrace.what)
		level = level + 1
	end
end

--------------------------------------------------
-- line に指定された全文字列を順に含むか
--------------------------------------------------
function line_has_strings(line, ...)
	local args = {...}
	local p = 1;
	
	for i=1, #args do
--		print(args[i]);
		p = line:find(args[i], p, true);
		if p==nil then
			return false;
		end
		p = p+1;
	end

	return true;
end

--------------------------------------------------
-- URLからパラメータを取得する
--------------------------------------------------
function get_param_from_url(url, param)
	-- ? 以降を抽出
	local val = url:match("[\?&]" .. param .. "=([^&]+).*$");
	if (val == nil) then
		return "";
	end
	
	return val;
end

mz3.logger_debug('util.lua end');
