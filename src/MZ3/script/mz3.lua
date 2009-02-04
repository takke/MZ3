--------------------------------------------------
-- MZ3 Script 基本ライブラリ
--------------------------------------------------
mz3.logger_info('mz3.lua 開始');

-- パッケージロードパスの変更
-- TODO mz3_plugin_dir support
--print(package.path);
package.path = mz3_script_dir .. "\\?.lua;" .. mz3_script_dir .. "\\?\\init.lua";

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

-- mixi用スクリプトロード
require("mixi");

mz3.logger_info('mz3.lua 終了');
