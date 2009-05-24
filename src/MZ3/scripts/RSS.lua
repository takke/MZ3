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
mz3.logger_debug('RSS.lua start');
module("RSS", package.seeall)

----------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������p)
----------------------------------------
mz3.regist_service('RSS', true);

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
	if services:find(' RSS', 1, true) ~= nil then

		-- RSS�^�u�ǉ�
		local tab = MZ3GroupItem:create("RSS");
		tab:append_category("�͂ău �ŋ߂̐l�C�G���g���[", "RSS_FEED", "http://b.hatena.ne.jp/hotentry?mode=rss");
--		tab:append_category("���傱���񁙂Ԃ낮", "RSS_FEED", "http://blog.excite.co.jp/shokotan/index.xml");
		tab:append_category("CNET Japan", "RSS_FEED", "http://japan.cnet.com/rss/index.rdf");
		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end
end

----------------------------------------
-- �C�x���g�n���h���̓o�^
----------------------------------------

-- �f�t�H���g�̃O���[�v���X�g����
mz3.add_event_listener("creating_default_group", "RSS.on_creating_default_group", false);

mz3.logger_debug('RSS.lua end');