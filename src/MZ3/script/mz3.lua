--------------------------------------------------
-- MZ3 Script ��{���C�u����
--------------------------------------------------
mz3.logger_info('mz3.lua �J�n');

-- �p�b�P�[�W���[�h�p�X�̕ύX
-- TODO mz3_plugin_dir support
--print(package.path);
package.path = mz3_script_dir .. "\\?.lua;" .. mz3_script_dir .. "\\?\\init.lua";

function show_backtrace()
	-- level=1 �� ���̊֐�
	-- level=2 �� ���̊֐��̌Ăяo������(wait_loop)
	-- level=3 �� �t�b�N�֐�
	-- �����̊֐��̕\��������Ă��܂��B
	local level = 4

	print("backtrace:")
	while true do
		local stacktrace = debug.getinfo(level, "nlS")
		if stacktrace == nil then break end
		print("    function: ", stacktrace.name, stacktrace.what)
		level = level + 1
	end
end

-- mixi�p�X�N���v�g���[�h
require("mixi");

mz3.logger_info('mz3.lua �I��');
