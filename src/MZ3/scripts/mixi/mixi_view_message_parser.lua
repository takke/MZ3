--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- TODO �z�X�g���Őݒ肵�Ă��邪�A�{���͂�����Őݒ肷�ׂ��B
--      �������A
--      inbox, outbox �p�[�T������ɓo�^���Ȃ���΂Ȃ�Ȃ��A
--      estimate �Ώۂɂ��Ȃ���΂Ȃ�Ȃ��A�Ȃǂ̉ۑ�A���B


--------------------------------------------------
-- �ymixi ���b�Z�[�W�ڍׁz
-- [content] view_message.pl �p�p�[�T
--
-- http://mixi.jp/view_message.pl
--
-- ����:
--   data:  ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy: NULL
--   html:  HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function mixi_view_message_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_message_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapper�N���X��
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- �s���擾
	local line_count = html:get_count();
	for i=250, line_count-1 do
		line = html:get_at(i);

		-- ���t���o
		-- <dt>���t</dt>
		-- <dd>2008�N11��12�� 11��13��</dd>
		if line_has_strings(line, "<dt>���t</dt>") then
			line1 = html:get_at(i+1);
			
			-- �`��������Ȃ̂Œ��ڐݒ肷��
			data:set_date(line1:match("<dd>(.*)</dd>"));
		end

		-- ���o�lID���o
		if line_has_strings(line, "<a", "href=", "show_friend.pl?id=", "\">", "</a>") then
			--mz3.logger_debug(line);
			id = line:match("\?id=([^\"]+)\"");

			--mz3.logger_debug("id : " .. id);
			data:set_integer("owner_id", id);
		end

		-- �{�����o
		if line_has_strings(line, "<div", "class=", "messageDetailBody" ) then

			data:add_text_array("body", "\r\n");

			-- ���`���Ēǉ�
			data:add_body_with_extract(line);

			-- </div> �����݂���ΏI���B
			if line_has_strings(line, "</div") then
				mz3.logger_debug("��</div>�����������̂ŏI�����܂�(1)");
				break;
			end

			if line:find( "FANCYURL_EMBED", 1, true )==nil then
				-- �������b�Z�[�W
				j = i+2;
				line = html:get_at(j);
				data:add_body_with_extract(line);
			else
				-- ���ʂ̃��b�Z�[�W
				for j=i+1, line_count-(i+1)-1 do
					line = html:get_at(j);

					-- ���`���Ēǉ�
					data:add_body_with_extract(line);
					
					if line_has_strings(line, "</div") then
						mz3.logger_debug("��</div>�����������̂ŏI�����܂�(2)");
						break;
					end
				end
			end

			break;
		end
		
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- ��M��
mz3.set_parser("MIXI_MESSAGE", "mixi.mixi_view_message_parser");
