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


----------------------------------------
-- form ��͗p���i
----------------------------------------

function dump_forms(forms)
	for fk, form in pairs(forms) do
		mz3.logger_debug("name   : " .. form.name);
		mz3.logger_debug("action : " .. form.action);
		mz3.logger_debug("method : " .. form.method);
		mz3.logger_debug("hidden : ");
		for k, v in pairs(form.hidden) do
			mz3.logger_debug(" " .. k .. " : " .. v);
		end
	end
end


--- parse_form �p�����ϐ�����
function new_init_form()
	local init_form = {
		["name"] = nil,
		["action"] = nil,
		["method"] = "get",	-- "get" or "post"
		["hidden"] = {}		-- ["hidden's name"] = "hidden's value"
	}
	return init_form;
end


--- �t�H�[�����
-- 
-- HTML �̑Sform����͂��A�e�[�u���ɕϊ�����B
-- �t�H�[���� name, action, method �ƑShidden�^�O���e�[�u��������B
--
function parse_form(line, base_url)
	-- �^�O����
	target = line;
--	mz3.logger_debug(line);
	
	local forms = {}
	local form = new_init_form();
	
	-- </form> ��������ΏI��
	while true do
		-- TODO match �͒x���Bposition �ɂ��X���C�h�^�����ɕύX���邱�ƁB
		left, tag, right = target:match('^(.-)(<.->)(.-)$');
--		mz3.logger_debug(left);
--		mz3.logger_debug("tag : " .. tag);
--		mz3.logger_debug(right);
		if tag==nil then
			break;
		end
		if string.sub(tag, 1, 6)=="<form " then
			local v = tag:match('name="(.-)"');
			if v~=nil then
				form.name = mz3.decode_html_entity(v);
			end
			
			local v = tag:match('action="(.-)"');
			if v~=nil then
				form.action = mz3.decode_html_entity(v);
				-- "http://" �� "https://" �Ŏn�܂�ꍇ�� base_url ��t���Ȃ�
				if string.sub(form.action, 1, 7) ~= "http://" and
				   string.sub(form.action, 1, 8) ~= "https://" then
					form.action = base_url .. form.action;
				end
			end
			
			local v = tag:match('method="(.-)"');
			if v~=nil then
				if v:lower()=="post" then
					form.method = "post";
				end
			end
			
--			mz3.alert(tag);
		end
		if string.sub(tag, 1, 7)=="<input " then
			local t = tag:match('type="(.-)"');
			if t~=nil and t:lower()=="hidden" then
				local name = tag:match('name="(.-)"');
				if name==nil then
					name = tag:match("name='(.-)'");
				end
				local v    = tag:match('value="(.-)"');
				if v==nil then
					v = tag:match("value='(.-)'");
				end
--				mz3.alert(name);
--				mz3.alert(v);
				if name~=nil and v~=nil then
					name = mz3.decode_html_entity(name);
					v    = mz3.decode_html_entity(v);
					form.hidden[name] = v;
				end
			end
		end
		if string.sub(tag, 1, 7)=="</form>" then
			table.insert(forms, form);
			form = new_init_form()
--			break;
		end
		
		if right==nil or right=="" then
			break;
		end
		target = right;
	end

	return forms;
end


mz3.logger_debug('util.lua end');
