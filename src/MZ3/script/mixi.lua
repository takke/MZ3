--------------------------------------------------
-- MZ3 API : mixi
--------------------------------------------------
module("mixi", package.seeall)

function hoge()
	print "fuga"
	mz3.hoge();
end

--[[
function after_parse()
end
]]

----------------------------------------
-- �R�~�������ꗗ
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
----------------------------------------
function bbs_parser(parent, body, html)
	mz3.logger_debug("bbs_parser start");
	
	--mz3.logger_debug("�\�t�g�E�F�A");
	mz3.logger_debug(mz3_data.get_text(parent, "url"));
--	mz3_data.set_text(parent, "url", "hogefuga");
--	mz3.logger_debug(mz3_data.get_text(parent, "url"));

	mz3.logger_debug(mz3_htmlarray.get_count(html));
	mz3.logger_debug(mz3_htmlarray.get_at(html,3));
	
	line_count = mz3_htmlarray.get_count(html);
	for i=140, line_count-1 do
		line = mz3_htmlarray.get_at(html, i);
		
--		mz3.logger_debug(i .. mz3_htmlarray.get_at(html, i));
	end
	
	mz3.logger_debug("bbs_parser end");
end


----------------------------------------
-- �t�b�N�֐��̓o�^
----------------------------------------

-- �R�~�������ꗗ
--mz3.set_parser("mixi", "BBS", bbs_parser);

--mz3.set_hook("mixi", "after_parse", after_parse);
