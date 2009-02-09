--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : goohome
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('goohome.lua start');
module("goohome", package.seeall)

----------------------------------------
-- �p�[�T���[�h
----------------------------------------
require("scripts\\goohome\\goohome_quote_quotes_friends_parser");

----------------------------------------
-- �p�[�T�̓o�^
----------------------------------------
-- goo�z�[�� �F�B�E���ڂ̐l�̂ЂƂ��ƈꗗ
mz3.set_parser("GOOHOME_QUOTE_QUOTES_FRIENDS", "goohome.quote_quotes_friends_parser");


----------------------------------------
-- �C�x���g�t�b�N�֐��̓o�^
----------------------------------------
-- TODO
function on_event(serialize_key, event_name, data)
	mz3.logger_debug('dblclk_hook : (' .. serialize_key .. ', ' .. event_name .. ')');
	data = MZ3Data:create(data);
	mz3.logger_debug(data:get_text('name'));
	
	return false
end

mz3.set_hook("GOOHOME_USER:dblclk_body_list", "goohome.on_event");
mz3.set_hook("GOOHOME_USER:enter_body_list",  "goohome.on_event");

mz3.logger_debug('goohome.lua end');
