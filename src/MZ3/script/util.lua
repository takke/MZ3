--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : ���[�e�B���e�B
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('util.lua start');

--------------------------------------------------
-- �֐��Ăяo���̃o�b�N�g���[�X���o�͂���
--------------------------------------------------
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

--------------------------------------------------
-- line �Ɏw�肳�ꂽ�S����������Ɋ܂ނ�
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
-- URL����p�����[�^���擾����
--------------------------------------------------
function get_param_from_url(url, param)
	-- ? �ȍ~�𒊏o
	local val = url:match("[\?&]" .. param .. "=([^&]+).*$");
	if (val == nil) then
		return "";
	end
	
	return val;
end

mz3.logger_debug('util.lua end');
