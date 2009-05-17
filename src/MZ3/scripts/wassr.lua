--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : wassr
--
-- $Id$
--------------------------------------------------
mz3.logger_debug('wassr.lua start');
module("wassr", package.seeall)

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('Wassr', false);

----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------
-- TODO

----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
-- TODO


----------------------------------------
-- �T�[�r�X�p�֐�
----------------------------------------

----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �f�t�H���g�̃O���[�v���X�g�����C�x���g�n���h��
--
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param event_name    'creating_default_group'
-- @param group         MZ3GroupData
--
function on_creating_default_group(serialize_key, event_name, group)

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' Wassr', 1, true) ~= nil then

		-- Wassr�^�u�ǉ�
		local tab = MZ3GroupItem:create("Wassr");
		tab:append_category("�^�C�����C��", "WASSR_FRIENDS_TIMELINE");
		tab:append_category("�ԐM�ꗗ", "WASSR_FRIENDS_TIMELINE", "http://api.wassr.jp/statuses/replies.xml");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end

----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "wassr.on_creating_default_group", false);

mz3.logger_debug('wassr.lua end');
