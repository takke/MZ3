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

--------------------------------------------------
-- line �Ɏw�肳�ꂽ�S����������Ɋ܂ނ�
--------------------------------------------------
function line_has_strings(line, ...)
	local args = {...}
	local p = 1;
	
	for i=1, #args do
--		print(args[i]);
		p = string.find(line, args[i], p, false);
		if p==nil then
			return false;
		end
		p = p+1;
	end

	return true;
end

function get_param_from_url(url, param)
	-- ? �ȍ~�𒊏o
	local val = string.match(url, "[\?&]" .. param .. "=([^&]+).*$");
	if (val == nil) then
		return "";
	end
	
	return val;
end

----------------------------------------
-- �y�R�~���j�e�B�ŐV�������݈ꗗ�z
-- [list] new_bbs.pl �p�p�[�T
--
-- http://mixi.jp/new_bbs.pl
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
----------------------------------------
function bbs_parser(parent, body, html)
	mz3.logger_debug("bbs_parser start");
	
	local t1 = mz3.get_tick_count();
	local inDataRegion = false;
	
	-- �s���擾
	local line_count = mz3_htmlarray.get_count(html);
	for i=140, line_count-1 do
		line = mz3_htmlarray.get_at(html, i);
		
--		mz3.logger_debug(i .. " : " .. mz3_htmlarray.get_at(html, i));

		-- TODO ���ցA�O�ւ̒��o����
--		if not inDataRegion then
--			
--		end

		-- ���ڒT��
		-- <dt class="iconTopic">2007�N10��01��&nbsp;22:14</dt>
		if line_has_strings(line, "<dt", "class", "iconTopic") or 
		   line_has_strings(line, "<dt", "class", "iconEvent") or 
		   line_has_strings(line, "<dt", "class", "iconEnquete") then

			inDataRegion = true;

			-- data ����
			data = mz3_data.create();

			-- ���t�̃p�[�X
			mz3_data.parse_date_line(data, line);

--			mz3.trace(i .. " : " .. line);
			
			-- ���s�擾
			i = i+1;
			line2 = mz3_htmlarray.get_at(html, i);
--			mz3.trace(i .. " : " .. line2);
			
			-- ���o��
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- �y�`���b�g�z�W���Ixxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = string.match(line2, ">([^<]+)(<.*)$");
			
			-- �G���e�B�e�B�f�R�[�h
			title = mz3.decode_html_entity(title);
--			mz3.logger_debug(after);

			-- �A���P�[�g�A�C�x���g�̏ꍇ�̓^�C�g���̑O�Ƀ}�[�N��t����
			if line_has_strings(line, "iconEvent") then
				title = "�y���z" .. title;
			elseif line_has_strings(line, "iconEnquete") then
				title = "�y���z" .. title;
			end
--			mz3.logger_debug(title);
			mz3_data.set_text(data, "title", title);
--			mz3.logger_debug(mz3_data.get_text(data, "title"));
			
			-- URL �擾
			url = string.match(line2, "href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			mz3_data.set_text(data, "url", url);
			
			-- �R�����g��
			mz3_data.set_integer(data, "comment_count", get_param_from_url(url, "comment_count"));
			
			-- id
			id = get_param_from_url(url, "id");
			mz3_data.set_integer(data, "id", id);
--			mz3.logger_debug(get_param_from_url(url, "comm_id"));

			-- �R�~���j�e�B��
			name = string.match(after, "</a>.*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
--			mz3.logger_debug(name);
			mz3_data.set_text(data, "name", name);
			
			-- URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(data, type);
			
			-- data �ǉ�
			mz3_data_list.add(body, data);
			
			-- data �폜
			mz3_data.delete(data);
		end

		if inDataRegion and line_has_strings(line, "</ul>") then
			mz3.logger_debug("��</ul>�����������̂ŏI�����܂�");
			break;
		end

	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- �t�b�N�֐��̓o�^
----------------------------------------

-- �R�~�������ꗗ
--mz3.set_parser("mixi", "BBS", bbs_parser);

--mz3.set_hook("mixi", "after_parse", after_parse);
