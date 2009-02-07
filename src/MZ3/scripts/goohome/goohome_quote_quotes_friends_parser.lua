--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : goohome parsers
--
-- $Id$
--------------------------------------------------
module("goohome", package.seeall)

function get_json_value(line, key)
	local json_key = "\"" .. key .. "\":";
	if line_has_strings(line, json_key) then
		return line:match(json_key .. "[^\"]*\"([^\"]*)\"");
	else
		return nil;
	end
end

--------------------------------------------------
-- [list] goo�z�[�� �F�B�E���ڂ̐l�̂ЂƂ��ƈꗗ�p�[�T
--
-- http://home.goo.ne.jp/api/quote/quotes/friends/json
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function quote_quotes_friends_parser(parent, body, html)
	mz3.logger_debug("quote_quotes_friends_parser start");
	local t1 = mz3.get_tick_count();
	
	-- wrapper�N���X��
	html = MZ3HTMLArray:create(html);
	parent = MZ3Data:create(parent);

	-- �S����
	mz3_data_list.clear(body);

	-- TODO �V���}�[�W�{�d���h�~����
--[[
	-- �V�K�ɒǉ�����f�[�^�Q
	CMixiDataList new_list;

	-- �d���h�~�p�� id �ꗗ�𐶐��B
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}
]]

	-- �I�u�W�F�N�g����
	local data = MZ3Data:create();
	type_user = mz3.get_access_type_by_key("GOOHOME_USER");
	data:set_access_type(type_user);

	-- �ЂƂ���API��json�͍s�P�ʂɗv�f������̂ŁA�s�P�ʂɃp�[�X����
	local line_count = html:get_count();
	local in_quote = false;
	
	for i=0, line_count-1 do
		line = html:get_at(i);
		if line_has_strings(line, "\"user\": {") then
--			mz3.logger_debug(i .. " : " .. line);
			if in_quote then
				-- ��O�̉�͌��ʂ�o�^
				mz3_data_list.add(body, data.data);
				data:delete();
				
				-- ���̗v�f�p�f�[�^�𐶐�
				data = MZ3Data:create();
				data:set_access_type(type_user);
			end
			in_quote = true;
		else
			if in_quote then
				
				local v;
				
				-- nickname
				v = get_json_value(line, "nickname");
				if v ~= nil then
					data:set_text("name", v);
				end
				
				-- goo_id
				v = get_json_value(line, "goo_id");
				if v ~= nil then
					data:set_text("goo_id", v);
				end
				
				-- profile_image_url
				v = get_json_value(line, "profile_image_url");
				if v ~= nil then
					data:add_text_array("image", v);
				end
				
				-- profile_url
				v = get_json_value(line, "profile_url");
				if v ~= nil then
					data:set_text("browse_uri", v);
					data:set_text("url", v);
				end
				-- friends
				v = get_json_value(line, "friends");
				if v ~= nil then
					data:set_integer("friends", v);
				end
				
				-- text : �����s�̉\��������̂ł����Ńp�[�X
				if line_has_strings(line, "\"text\":") then
					-- " �ȍ~�̕�����擾
					local after_dq = line:match(":[^\"]*\"(.*)");
					--mz3.logger_debug(after_dq);
					
					if after_dq:find("\"", 1, true) ~= nil then
						-- �I��
						local text = after_dq:match("(.*)\"");
						data:add_text_array("body", text);
					else
						-- " ��������܂Ŏ擾����
						data:add_text_array("body", after_dq);
						data:add_text_array("body", "\r\n");
						i = i+1;
						while i<line_count-1 do
							line = html:get_at(i);
							if line:find("\"", 1, true) ~= nil then
								-- �I��
								local text = line:match("(.*)\"");
								data:add_text_array("body", text);
								break;
							else
								data:add_text_array("body", line);
								data:add_text_array("body", "\r\n");
							end
							
							i = i+1;
						end
					end
				end

				-- favorited
				v = get_json_value(line, "favorited");
				if v ~= nil then
					data:set_integer("favorited", v=="false" and 0 or 1);	-- 3�����Z�q
				end
				-- favorites
				v = get_json_value(line, "favorites");
				if v ~= nil then
					data:set_integer("favorites", v);
				end
				-- comments
				v = get_json_value(line, "comments");
				if v ~= nil then
					data:set_integer("comments", v);

					-- �R�����g�����{�f�B�̖����ɂ���
					data:add_text_array("body", "(" .. v .. ")");
				end
				-- created_at
				v = get_json_value(line, "created_at");
				if v ~= nil then
					data:parse_date_line(v);
				end
				-- id
				v = get_json_value(line, "id");
				if v ~= nil then
					data:set_text("id", v);

					-- �{��URL�Ƃ��ĂЂƂ��Ƃ�URL���\�z���Đݒ肵�Ă���
					-- http://home.goo.ne.jp/quote/user/{goo_id}/detail/{id}?cnt={comments}
					url = string.format("http://home.goo.ne.jp/quote/user/%s/detail/%s?cnt=%d",
							data:get_text("goo_id"),
							v,
							data:get_integer("comments",0));
					data:set_text("url", url);
					data:set_text("browse_uri", url);
				end
			end
		end
	end

	-- TODO �V���}�[�W�{�d���h�~����
	if in_quote then
		mz3_data_list.add(body, data.data);
	end
--[[

	-- ���������f�[�^���o�͂ɔ��f
	TwitterParserBase::MergeNewList(out_, new_list);
]]

	-- �I�u�W�F�N�g�폜
	data:delete();

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("quote_quotes_friends_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
