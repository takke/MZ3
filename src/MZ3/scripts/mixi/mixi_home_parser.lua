--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id$
--------------------------------------------------
module("mixi", package.seeall)

--------------------------------------------------
-- �ymixi�g�b�v�y�[�W�z
-- [content] home.pl ���O�C����̃��C����ʗp�p�[�T
--
-- http://mixi.jp/home.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--
-- parent �̉��L�̕ϐ��ɏ����i�[����B
-- <ul>
--  <li>[string] "owner_id"       => ������ID
--  <li>[int] "new_message_count" => �V�����b�Z�[�W��
--  <li>[int] "new_comment_count" => �V���R�����g��
--  <li>[int] "new_apply_count"   => ���F�҂���
-- </ul>
--------------------------------------------------
function mixi_home_parser(parent, body, html)
	mz3.logger_debug("mixi_home_parser start");

	local t1 = mz3.get_tick_count();

	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);

	-- ���ʂ̏�����
	parent:set_integer("new_message_count", 0);
	parent:set_integer("new_comment_count", 0);
	parent:set_integer("new_apply_count", 0);
	mz3.logger_debug("original owner_id : [" .. parent:get_text("owner_id") .. "]");
	
	-- �V�����b�Z�[�W�A�R�����g�A���F�҂������̎擾
	local line_count = html:get_count();
	local in_mixi_info_div = false;
	for i=350, line_count-1 do
		line = html:get_at(i);
		-- <div id="mixiInfo">
		-- ...
		-- <ul>
		-- ...
		-- <li class="redTxt"><a href="list_request.pl"><b>�}�C�~�N�V�B�ǉ����N�G�X�g��2������܂��I</b></a></li>
		-- <li class="redTxt"><a href="list_message.pl">�V�����b�Z�[�W��2������܂��I</a></li>
		-- <li class="redTxt"><a href="view_diary.pl?id=xxx&owner_id=xxx">1���̓��L�ɑ΂��ĐV���R�����g������܂��I</a></li>
		-- ...
		-- </ul>
		-- �������ΏۂƂ���

		if not in_mixi_info_div then
			if line_has_strings(line, '<div', 'id=', 'mixiInfo') then
				in_mixi_info_div = true;
			end
		else
			if line_has_strings(line, '</ul>') then
				break;
			else
				-- li �^�O�̒�����������
				local li_text = line:match('<li[^>]*>(.*)</li>');
				if li_text ~= nil then
--					mz3.logger_debug(li_text);

					local n = 0;
					n = li_text:match('�}�C�~�N�V�B�ǉ����N�G�X�g��(.*)������܂��I');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_apply_count', n);
					end
					
					n = li_text:match('[�W]��(.*)������܂��I');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_message_count', n);
					end
					
					n = li_text:match('([0-9]*)���̓��L�ɑ΂��ĐV���R�����g������܂��I');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_comment_count', n);
					end
				end
			end
		end
	end
	
	
	-- owner_id �̎擾
--	parent:set_text('owner_id', '');	-- �f�o�b�O�p
	if parent:get_text('owner_id')=="" then
		mz3.logger_debug('owner_id �����擾�Ȃ̂ŉ�͂��܂�');
		
		for i=0, line_count-1 do
			line = html:get_at(i);
			if line_has_strings(line, "<a", "href=", "add_diary.pl?id=") then
--				mz3.logger_debug("[" .. line .. "]");
				local url = line:match("href=\"([^\"]+)\"");
--				mz3.logger_debug("[" .. url .. "]");
				local id = get_param_from_url(url, 'id');
				if id=="" then
					mz3.logger_error("add_diary.pl �̈����� id �p�����[�^��������܂���Bline[" .. line .. "]");
				else
					mz3.logger_debug("owner_id[" .. id .. "]");
					parent:set_text('owner_id', id);
				end
				break;
			end
		end
		
		if parent:get_text('owner_id')=="" then
			mz3.logger_error('owner_id ���擾�ł��܂���ł���');
		end
	end
	
	mz3.logger_debug("parsed owner_id : [" .. parent:get_text("owner_id") .. "]");

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_home_parser end; elapsed : " .. (t2-t1) .. "[msec]");

	return true;
end

mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");
