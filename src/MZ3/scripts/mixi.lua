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
			back_data = MZ3Data:create();
			back_data:set_text("title", "<< " .. t .. " >>");
			back_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			back_data:set_access_type(type);
		end
		
		-- ��
		local url, t = line:match([[href="([^"]+)">(��[^<]+)<]]);
		if url~=nil then
			next_data = MZ3Data:create();
			next_data:set_text("title", "<< " .. t .. " >>");
			next_data:set_text("url", url);
			type = mz3.estimate_access_type_by_url(url);
			next_data:set_access_type(type);
		end
	end
	
	return back_data, next_data;
end


----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- �t��������
type = MZ3AccessTypeInfo:create();
type:set_info_type('category');								-- �J�e�S��
type:set_service_type('mixi');								-- �T�[�r�X���
type:set_serialize_key('MIXI_SHOW_SELF_LOG');				-- �V���A���C�Y�L�[
type:set_short_title('�t��������');							-- �ȈՃ^�C�g��
type:set_request_method('GET');								-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('mixi\\show_self_log.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('euc-jp');						-- �G���R�[�f�B���O
type:set_default_url('http://mixi.jp/show_self_log.pl');
type:set_body_header(1, 'title', '���O');
type:set_body_header(2, 'date', '���t');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
--mz3.logger_debug(type);

-- �������b�Z�[�W
type = mz3_access_type_info.new_access_type();
mz3_access_type_info.set_info_type(type, 'category');							-- �J�e�S��
mz3_access_type_info.set_service_type(type, 'mixi');							-- �T�[�r�X���
mz3_access_type_info.set_serialize_key(type, 'MIXI_LIST_MESSAGE_OFFICIAL');		-- �V���A���C�Y�L�[
mz3_access_type_info.set_short_title(type, '�������b�Z�[�W');					-- �ȈՃ^�C�g��
mz3_access_type_info.set_request_method(type, 'GET');							-- ���N�G�X�g���\�b�h
mz3_access_type_info.set_cache_file_pattern(type, 'mixi\\list_message_official.html');	-- �L���b�V���t�@�C��
mz3_access_type_info.set_request_encoding(type, 'euc-jp');						-- �G���R�[�f�B���O
mz3_access_type_info.set_default_url(type, 'http://mixi.jp/list_message.pl?box=noticebox');
mz3_access_type_info.set_body_header(type, 1, 'title', '����');
mz3_access_type_info.set_body_header(type, 2, 'name', '���o�l>>');
mz3_access_type_info.set_body_header(type, 3, 'date', '���t>>');
mz3_access_type_info.set_body_integrated_line_pattern(type, 1, '%2 %3');
mz3_access_type_info.set_body_integrated_line_pattern(type, 2, '%1');


----------------------------------------
-- ���j���[�ւ̓o�^
----------------------------------------

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)
--	mz3.logger_debug('on_creating_default_group');
	
	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' mixi', 1, true) ~= nil then
		-- ���̑�/�t�������� �ǉ�
		local tab = mz3_group_data.get_group_item_by_name(group, '���̑�');
		mz3_group_item.append_category(tab, "�t��������", "MIXI_SHOW_SELF_LOG", "http://mixi.jp/show_self_log.pl");

		-- ���b�Z�[�W/�������b�Z�[�W �ǉ�
		local tab = mz3_group_data.get_group_item_by_name(group, '���b�Z�[�W');
		mz3_group_item.append_category(tab, "�������b�Z�[�W", "MIXI_LIST_MESSAGE_OFFICIAL", "http://mixi.jp/list_message.pl?box=noticebox");
	end
end

----------------------------------------
-- �p�[�T�̃��[�h���o�^
----------------------------------------
-- �����X�g�n
-- �R�~���j�e�B�ŐV�����ꗗ
require("scripts\\mixi\\mixi_new_bbs_parser");
mz3.set_parser("BBS",             "mixi.new_bbs_parser");
-- �R�~���j�e�B�R�����g�L������ : �ŐV�����ꗗ�Ɠ���
mz3.set_parser("NEW_BBS_COMMENT", "mixi.new_bbs_parser");

-- �t��������
require("scripts\\mixi\\mixi_show_self_log_parser");
mz3.set_parser("MIXI_SHOW_SELF_LOG", "mixi.mixi_show_self_log_parser");

-- �������b�Z�[�W
require("scripts\\mixi\\mixi_new_official_message_parser");
mz3.set_parser("MIXI_LIST_MESSAGE_OFFICIAL", "mixi.mixi_new_official_message_parser");

-- �g�b�v�y�[�W
require("scripts\\mixi\\mixi_home_parser");
mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");


----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------
-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "mixi.on_creating_default_group");

mz3.logger_debug('mixi.lua end');
