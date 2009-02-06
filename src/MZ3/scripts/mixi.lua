--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : mixi
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('mixi.lua start');
module("mixi", package.seeall)

--------------------------------------------------
-- ���ցA�O�ւ̒��o����
--------------------------------------------------
function parse_next_back_link(line, base_url)

	local back_data = nil;
	local next_data = nil;
	
	-- <ul><li><a href="new_bbs.pl?page=1">�O��\��</a></li>
	-- <li>51���`100����\��</li>
	-- <li><a href="new_bbs.pl?page=3">����\��</a></li></ul>
	if line_has_strings(line, base_url) then
		
		-- �O
		local url, t = line:match([[href="([^"]+)">(�O[^<]+)<]]);
		if url~=nil then
			back_data = mz3_data.create();
			mz3_data.set_text(back_data, "title", "<< " .. t .. " >>");
			mz3_data.set_text(back_data, "url", url);
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(back_data, type);
		end
		
		-- ��
		local url, t = line:match([[href="([^"]+)">(��[^<]+)<]]);
		if url~=nil then
			next_data = mz3_data.create();
			mz3_data.set_text(next_data, "title", "<< " .. t .. " >>");
			mz3_data.set_text(next_data, "url", url);
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(next_data, type);
		end
	end
	
	return back_data, next_data;
end


----------------------------------------
-- �p�[�T���[�h
----------------------------------------
require("scripts\\mixi\\mixi_new_bbs_parser");

----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- �R�~���j�e�B�ŐV�������݈ꗗ
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- �R�~���j�e�B�R�����g�L������
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");


----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------
--mz3.set_hook("mixi", "after_parse", after_parse);

mz3.logger_debug('mixi.lua end');
