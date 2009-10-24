--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : 2ch
--
-- $Id: 2ch.lua 1308 2009-06-27 08:21:46Z takke $
--------------------------------------------------
mz3.logger_debug('2ch.lua start');
module("2ch", package.seeall)

--------------------------------------------------
-- �T�[�r�X�̓o�^(�^�u�������A���O�C���ݒ�p)
--------------------------------------------------
mz3.regist_service('2ch', false);


----------------------------------------
-- �A�N�Z�X��ʂ̓o�^
----------------------------------------

-- �ꗗ   = ���C����� �J�e�S���{�{�f�B
-- �X���ꗗ = ���C����� �J�e�S���{�{�f�B
-- �X���b�h = ���|�[�g���


-- �ꗗ
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('2ch');									-- �T�[�r�X���
type:set_serialize_key('2CH_BBS_MENU');							-- �V���A���C�Y�L�[
type:set_short_title('2ch �ꗗ');								-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('2ch\\bbs_menu.html');				-- �L���b�V���t�@�C��
type:set_request_encoding('sjis');								-- �G���R�[�f�B���O
type:set_default_url('http://menu.2ch.net/bbsmenu.html');
type:set_body_header(1, 'title', '�J�e�S��');
type:set_body_header(2, 'name', '��');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');


-- �X���ꗗ
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('2ch');									-- �T�[�r�X���
type:set_serialize_key('2CH_SUBJECT');							-- �V���A���C�Y�L�[
type:set_short_title('2ch �X���ꗗ');							-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('2ch\\subject_{urlafter:/}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('sjis');								-- �G���R�[�f�B���O
type:set_default_url('http://menu.2ch.net/dummy.html');
type:set_body_header(1, 'title', '�X��');
type:set_body_header(2, 'name', '��');
type:set_body_integrated_line_pattern(1, '%1');
type:set_body_integrated_line_pattern(2, '%2');
type:set_cruise_target(true);


-- �X���b�h
type = MZ3AccessTypeInfo.create();
type:set_info_type('body');										-- �J�e�S��
type:set_service_type('2ch');									-- �T�[�r�X���
type:set_serialize_key('2CH_THREAD');							-- �V���A���C�Y�L�[
type:set_short_title('2ch �X��');								-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('2ch\\thread_{urlafter:/}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('sjis');								-- �G���R�[�f�B���O


-- 2ch�u�b�N�}�[�N
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- �J�e�S��
type:set_service_type('2ch');									-- �T�[�r�X���
type:set_serialize_key('2CH_BOOKMARK');							-- �V���A���C�Y�L�[
type:set_short_title('���C�ɓ���X��');						-- �ȈՃ^�C�g��
type:set_request_method('GET');									-- ���N�G�X�g���\�b�h
type:set_cache_file_pattern('2ch\\subject_{urlafter:/}.html');	-- �L���b�V���t�@�C��
type:set_request_encoding('sjis');								-- �G���R�[�f�B���O
type:set_default_url('http://menu.2ch.net/dummy.html');
type:set_body_header(1, 'title', '�X��');
type:set_body_header(2, 'name', '��');
type:set_body_integrated_line_pattern(1, '%2');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);


----------------------------------------
-- ���j���[���ړo�^(�ÓI�ɗp�ӂ��邱��)
----------------------------------------
menu_items = {}
menu_items.read               = mz3_menu.regist_menu("2ch.on_read_menu_item");
menu_items.read_by_reportview = mz3_menu.regist_menu("2ch.on_read_by_reportview_menu_item");
menu_items.open_by_browser    = mz3_menu.regist_menu("2ch.on_open_by_browser_menu_item");
menu_items.search_post_thread = mz3_menu.regist_menu("2ch.on_search_post_thread");
menu_items.search_post_bbs    = mz3_menu.regist_menu("2ch.on_search_post_bbs");
menu_items.add_bookmark       = mz3_menu.regist_menu("2ch.on_add_bookmark");
menu_items.remove_bookmark    = mz3_menu.regist_menu("2ch.on_remove_bookmark");


----------------------------------------
-- 2ch �v���O�C�����ʒ萔
----------------------------------------
bookmark_file_path = mz3_dir .. "2ch_bookmark.tsv";


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

	-- �T�|�[�g����T�[�r�X��ʂ̎擾(�X�y�[�X��؂�)
	services = mz3_group_data.get_services(group);
	if services:find(' 2ch', 1, true) ~= nil then
		-- ��M�g���C
		local tab = MZ3GroupItem:create("2ch");

		tab:append_category("���C�ɓ���X��", "2CH_BOOKMARK");
		tab:append_category("�ꗗ", "2CH_BBS_MENU");

		mz3_group_data.append_tab(group, tab.item);
		tab:delete();
	end

end
mz3.add_event_listener("creating_default_group", "2ch.on_creating_default_group");


----------------------------------------
-- �p�[�T
----------------------------------------

--------------------------------------------------
-- �y�ꗗ�z
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function the_2ch_bbsmenu_parser(parent, body, html)
	mz3.logger_debug("the_2ch_bbsmenu_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	line = '';
	local line_count = html:get_count();
	local is_start = false;
	local current_group = nil;
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		local group = nil;
		if line_has_strings(line, '<BR><BR>') then
			group = line:match('^<BR><BR><B>(.-)</');
		end
		if group~=nil then
			current_group = group;
		end
		
		if is_start==false and group~=nil then
			is_start = true;
		end

		if is_start then
			-- <A HREF=http://headline.2ch.net/bbynamazu/>�n�kheadline</A><br>
			-- <A HREF=http://www.monazilla.org/ TARGET=_blank>2ch�c�[��</A><br>
			local url, title = nil, nil;
			if line_has_strings(line, '<A') then
				url, title = line:match('<A HREF=(.-)/>(.-)</A>');
			end
			if url~= nil and title~=nil then

				-- data ����
				data = MZ3Data:create();
				
				type = mz3.get_access_type_by_key('2CH_SUBJECT');
				data:set_access_type(type);
				
				data:set_text('name', title);
				data:set_text('title', current_group);

				url = url .. '/subject.txt';
				data:set_text('url', url);
				data:set_text('browse_uri', url);

				-- ���X�g�ɒǉ�
				body:add(data.data);
				
				-- data �폜
				data:delete();
				
				-- 2�ڈȍ~�̓���J�e�S���̓J�e�S����o�^���Ȃ��B���Â炢�̂ŁB
				current_group = '';
			end
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("the_2ch_bbsmenu_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_BBS_MENU", "2ch.the_2ch_bbsmenu_parser");


--------------------------------------------------
-- �y�X���b�h�ꗗ�z
--
-- ����:
--   parent: ��y�C���̑I���I�u�W�F�N�g(MZ3Data*)
--   body:   ���y�C���̃I�u�W�F�N�g�Q(MZ3DataList*)
--   html:   HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function the_2ch_subject_parser(parent, body, html)
	mz3.logger_debug("the_2ch_subject_parser start");
	
	-- wrapper�N���X��
	parent = MZ3Data:create(parent);
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- �S����
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	line = '';
	local line_count = html:get_count();
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		-- 1246883533.dat<>�y�����z �P�W�`�R�S�΂̔������u�q���~�����Ȃ��v�B�������Ȃ����R�́u�����ِ������Ȃ��v�u�o�ϓI�ɕs���v�Ȃǁc���s�s�������R (212)
		local tno, title = line:match('^(.-)\.dat<>(.-)$');
		if tno~= nil and title~=nil then

			-- data ����
			data = MZ3Data:create();
			
			type = mz3.get_access_type_by_key('2CH_THREAD');
			data:set_access_type(type);
			
			data:set_text('name', '');
			data:set_text('title', title);
--			data:set_text('title', parent:get_text('name'));

			-- parent url ���� test/read.cgi/xxx ��URL�𐶐�����
			parent_url = parent:get_text('url');
			local domain, category = parent_url:match('http://(.-)/(.-)/subject.txt');
			url = 'http://' .. domain .. '/test/read.cgi/' .. category .. '/' .. tno .. '/l50';
			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- ���X�g�ɒǉ�
			body:add(data.data);
			
			-- data �폜
			data:delete();
			
			if body:get_count()>=t2ch_thread_view_limit then
				break;
			end
		end
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("the_2ch_subject_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_SUBJECT", "2ch.the_2ch_subject_parser");


--------------------------------------------------
-- �y�X���b�h�z
--
-- ����:
--   data:  ��y�C���̃I�u�W�F�N�g�Q(MZ3Data*)
--   dummy: NULL
--   html:  HTML�f�[�^(CHtmlArray*)
--------------------------------------------------
function thread_parser(data, dummy, html)
	mz3.logger_debug("thread_parser start");

	-- wrapper�N���X��
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- �S����
	data:clear();
	
	local t1 = mz3.get_tick_count();
	
	local base_url = '';
	local line_count = html:get_count();
	line = '';
	local next_line = 0;
	for i=0, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<base') then
			base_url = line:match('<base href="(.-)"');
		end
		
		-- �^�C�g��
		if line_has_strings(line, '<title') then
			local title = line:match('<title>(.-)</title>');
			data:set_text('title', title);
		end

		-- <dt>1 �F<font color=green><b>���΂������� ��</b></font>�F2009/07/06(��) 23:34:25 ID:???i<dd> ���d�ԓ��Ńi�C�t���������A���  �\�͍s�ז@�ᔽ�e�^�őߕ� <br>  <br> �E�d�ԓ��Ōg�ѓd�b�̎g�p�𒍈ӂ��ꂽ����Ƀi�C�t�������ċ������Ƃ��āA���{�x�V�������� <br> �@�U���A�\�͍s�׏����@�ᔽ�̋^���ŁA���Ɍ����s���F�{�ʁA��Ј����c���e�^�ҁi�R�O�j�� <br> �@���s�Ƒߕ߂����B <br>  <br> �@�ߕߗe�^�͂U���ߌ�Q���T�O������A�i�q�֐����̓����s��O�|�V�����Ԃ𑖍s���Ă��� <br> �@�����d�ԓ��ŁA�ޗǌ���a�S�R�s�̒j����Ј��i�U�W�j����g�ѓd�b�̃Q�[������߂�悤 <br> �@���ӂ��ꂽ���Ƃɕ��𗧂āA�����Ă����i�C�t�����o���u�E���Ă��܂����v�Ƌ������^���B <br>  <br> �@�V�������ɂ��ƁA���c�e�^�҂́u�����������̂Őn�����������v�Ɨe�^��F�߂Ă���B <br>  <br> �@�d�Ԃɏ�荇�킹�Ă����i�q�Ј�����艟�����A�V�����w�Ōx�@���Ɉ����n�����B�j�����q�� <br> �@�����͂Ȃ������B <br>  <br> �@<a href="http://ime.nu/www.47news.jp/CN/200907/CN2009070601000800.html" target="_blank">http://www.47news.jp/CN/200907/CN2009070601000800.html</a> <br>  <br> ���O�X�� <br> <a href="http://tsushima.2ch.net/test/read.cgi/newsplus/1246881623/" target="_blank">http://tsushima.2ch.net/test/read.cgi/newsplus/1246881623/</a> <br><br>
		if line_has_strings(line, '<dt>') then
			local count, name, date, content = line:match('<dt>(.-)�F(.-)�F(.-)<dd>(.*)$');
			count = tonumber(count);
			if count>=2 then
				-- 2���ڈȍ~�͎q�v�f�Ƃ��ē�������
				child = MZ3Data:create();
				parse_one_item(child, count, name, date, content);
				data:add_child(child);
				child:delete();
			else
				parse_one_item(data, count, name, date, content);
			end
		end
		
		if line_has_strings(line, '<a', '�V�����X�̕\��') then
			next_line = i+1;
			break;
		end
	end
	
	-- ���������N���o
	for i=next_line, line_count-1 do
		line = html:get_at(i);
		
		if line_has_strings(line, '<a href="../') then
			local url, text = line:match('href="(.-)".->(.-)</');
			data:add_link_list(base_url .. url, text, 'page');
--			mz3.logger_debug(base_url .. url .. text);
		end
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("thread_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end
mz3.set_parser("2CH_THREAD", "2ch.thread_parser");


function parse_one_item(data, count, name, date, content)
	
	-- ���X��
	data:set_integer('comment_index', count);
	
	-- ���O
	-- <a href="mailto:sage"><b>���������񁗏\���N</b></a>
	-- <font color=green><b>���΂������� ��</b></font>
	-- <font color=green><b>���������񁗏\���N</b></font>
	-- <a href="mailto:sage"><b> </b>��wMzNGLjdrw <b></b></a>
	name1 = name:gsub('<.->', '');
	data:set_text("name", mz3.decode_html_entity(name1));
	data:set_text("author", mz3.decode_html_entity(name1));
	
	-- ���t
	data:set_date(date);

	data:add_text_array("body", "\r\n");
	data:add_body_with_extract(content);
end

----------------------------------------
-- �C�x���g�n���h��
----------------------------------------

--- �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X�擾
--
-- @param event_name    'creating_default_group'
-- @param serialize_key �V���A���C�Y�L�[(nil)
-- @param body          body data
--
-- @return (1) [bool] �������� true, ���s���� false
-- @return (2) [int] �A�C�R���C���f�b�N�X
--
function on_get_body_list_default_icon_index(event_name, serialize_key, body)

	if serialize_key == "2CH_SUBJECT" then
		body = MZ3Data:create(body);
		if body:get_text('title')~='' then
			return true, 0;
		end
	end

	return false;
end
mz3.add_event_listener("get_body_list_default_icon_index", "2ch.on_get_body_list_default_icon_index");


--- �S���\�����j���[�܂��̓_�u���N���b�N�C�x���g
function on_read_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);
	
	item = '';
	item = item .. data:get_text('title') .. "\r\n";
--	item = item .. "���t : " .. data:get_date() .. "\r\n";
	item = item .. "----\r\n";

--	item = item .. data:get_text('title') .. "\r\n";
	
--	item = item .. "\r\n";
	item = item .. data:get_text('url') .. "\r\n";
	
	mz3.alert(item, data:get_text('name'));

	return true;
end

--- ���|�[�g�r���[�ŊJ��
function on_read_by_reportview_menu_item(serialize_key, event_name, data)
	mz3.logger_debug('on_read_by_reportview_menu_item : (' .. serialize_key .. ', ' .. event_name .. ')');

	data = MZ3Data:create(data);

	-- �ʐM�J�n
	url = data:get_text('url');
	key = "2CH_THREAD";
	access_type = mz3.get_access_type_by_key(key);
	referer = '';
	user_agent = nil;
	post = nil;
	mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

	return true;
end


--- �{�f�B���X�g�̃_�u���N���b�N(�܂���Enter)�̃C�x���g�n���h��
function on_body_list_click(serialize_key, event_name, data)

	if serialize_key=="2CH_SUBJECT" then
		-- �J�e�S���ǉ�
		body = mz3_main_view.get_selected_body_item();
		body = MZ3Data:create(body);
		name = body:get_text('name');
		
--		mz3.alert(name);
		
		-- �J�e�S���ǉ�
		title = "��" .. name;
		url = body:get_text('url');
		key = "2CH_SUBJECT";
		mz3_main_view.append_category(title, url, key);
		
		-- �ǉ������J�e�S���̎擾�J�n
		access_type = mz3.get_access_type_by_key(key);
		referer = '';
		user_agent = nil;
		post = nil;
		mz3.open_url(mz3_main_view.get_wnd(), access_type, url, referer, "text", user_agent, post);

		return true;
	end
	
	if serialize_key=="2CH_THREAD" then
		-- ���j���[�\��
--		return on_popup_body_menu(event_name, serialize_key, mz3_main_view.get_selected_body_item(), mz3_main_view.get_wnd());

		-- �v���p�e�B�\��
		return on_read_menu_item(serialize_key, event_name, data);
	end
	
	-- �W���̏����𑱍s
	return false;
end
mz3.add_event_listener("dblclk_body_list", "2ch.on_body_list_click");
mz3.add_event_listener("enter_body_list",  "2ch.on_body_list_click");


--- �u�u���E�U�ŊJ���v���j���[�p�n���h��
function on_open_by_browser_menu_item(serialize_key, event_name, data)

	body = MZ3Data:create(mz3_main_view.get_selected_body_item());

	mz3.open_url_by_browser_with_confirm(body:get_text('url'));
end


--- �{�f�B���X�g�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_body_menu'
-- @param serialize_key �{�f�B�A�C�e���̃V���A���C�Y�L�[
-- @param body          body
-- @param wnd           wnd
--
function on_popup_body_menu(event_name, serialize_key, body, wnd)
	if serialize_key=="2CH_THREAD" or serialize_key=="2CH_SUBJECT" then
	else	
		return false;
	end

	-- �C���X�^���X��
	body = MZ3Data:create(body);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();

	if serialize_key=="2CH_THREAD" then
		menu:append_menu("string", "�ŐV�̈ꗗ���擾", IDM_CATEGORY_OPEN);
		menu:append_menu("string", "�X�����J��...", menu_items.read_by_reportview);
		menu:append_menu("string", "�u���E�U�ŊJ��...", menu_items.open_by_browser);

		menu:append_menu("separator");
		menu:append_menu("string", "�v���p�e�B...", menu_items.read);
		
		-- �J�e�S���̎�ʂ��u���C�ɓ���X���v��������폜��ǉ�����B
		-- �u���C�ɓ���X���v�ȊO��������ǉ����j���[��ǉ��B
		menu:append_menu("separator");
		local category_type = mz3_main_view.get_selected_category_access_type();
		if category_type==mz3.get_access_type_by_key('2CH_BOOKMARK') then
			menu:append_menu("string", "���C�ɓ���X������폜", menu_items.remove_bookmark);
		else
			menu:append_menu("string", "���C�ɓ���X���ɒǉ�", menu_items.add_bookmark);
		end

		menu:append_menu("separator");
		-- �X�� ����
		menu:append_menu("string", "�X������", menu_items.search_post_thread);
	elseif serialize_key=="2CH_SUBJECT" then
		-- ����
		menu:append_menu("string", "����", menu_items.search_post_bbs);
	end

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	
	return true;
end
mz3.add_event_listener("popup_body_menu",  "2ch.on_popup_body_menu");


--- ���|�[�g�r���[�̃|�b�v�A�b�v���j���[�\��
--
-- @param event_name    'popup_report_menu'
-- @param serialize_key ���|�[�g�A�C�e���̃V���A���C�Y�L�[
-- @param report_item   ���|�[�g�A�C�e��
-- @param sub_item_idx  �I���A�C�e���̃C���f�b�N�X
-- @param wnd           wnd
--
--[[
function on_popup_report_menu(event_name, serialize_key, report_item, sub_item_idx, wnd)
	if serialize_key~="GMAIL_MAIL" then
		return false;
	end

	-- �C���X�^���X��
	report_item = MZ3Data:create(report_item);
	
	-- ���j���[����
	menu = MZ3Menu:create_popup_menu();
	menu_edit = MZ3Menu:create_popup_menu();
	menu_layout = MZ3Menu:create_popup_menu();
	
	menu:append_menu("string", "�߂�", ID_BACK_MENU);
	menu:append_menu("separator");
	
	menu:append_menu("string", "�ԐM", ID_WRITE_COMMENT);

	menu:append_menu("string", "�ēǍ�", IDM_RELOAD_PAGE);

	menu_edit:append_menu("string", "�R�s�[", ID_EDIT_COPY);
	menu:append_submenu("�ҏW", menu_edit);
	
	menu:append_menu("string", "�u���E�U�ŊJ���i���̃y�[�W�j...", ID_OPEN_BROWSER);

	-- TODO ���ʉ�
	menu:append_menu("separator");
	menu_layout:append_menu("string", "�����X�g����������", IDM_LAYOUT_REPORTLIST_MAKE_NARROW);
	menu_layout:append_menu("string", "�����X�g���L������", IDM_LAYOUT_REPORTLIST_MAKE_WIDE);
	menu:append_submenu("��ʃ��C�A�E�g", menu_layout);

	-- �|�b�v�A�b�v
	menu:popup(wnd);
	
	-- ���j���[���\�[�X�폜
	menu:delete();
	menu_edit:delete();
	menu_layout:delete();
	
	return true;
end
mz3.add_event_listener("popup_report_menu",  "gmail.on_popup_report_menu");
]]


----------------------------------------
-- estimate �Ώۂɒǉ�
----------------------------------------

--- estimate �Ώ۔��ʃC�x���g�n���h��
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        ��͑Ώ�URL
--
function on_estimate_access_type(event_name, url, data1, data2)

    -- ��M��
    if string.match(url, 'http://.-\.2ch\.net/') then
		return true, mz3.get_access_type_by_key('2CH_THREAD');
	end

	return false;
end
mz3.add_event_listener("estimate_access_type_by_url", "2ch.on_estimate_access_type");


--- ViewStyle �ύX
--
-- @param event_name    'get_view_style'
-- @param serialize_key �J�e�S���̃V���A���C�Y�L�[
--
-- @return (1) [bool] �������� true, ���s���� false
-- @return (2) [int] VIEW_STYLE_*
--
function on_get_view_style(event_name, serialize_key)

	service_type = mz3.get_service_type(serialize_key);
	if service_type=='2ch' then
		return true, VIEW_STYLE_IMAGE;
	end

	return false;
end
mz3.add_event_listener("get_view_style", "2ch.on_get_view_style");


--- �X������
last_searched_index_thread = 0;
last_searched_key_thread = '';
function on_search_post_thread(serialize_key, event_name, data)

	local key = mz3.show_common_edit_dlg("�X������", "�������������������͂��ĉ�����", last_searched_key_thread);
	if key == nil then
		return false;
	end
	last_searched_key_thread = key;
	key = string.upper( key );

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		s = mz3_data.get_text(data, 'title');
		s = string.upper( s );
		if s:find( key, 1, true ) ~= nil then
			mz3_main_view.select_body_item(i);
			last_searched_index_thread = i;
			break;
		end
	end
end


--- ����
last_searched_index_bbs = 0;
last_searched_key_bbs = '';
function on_search_post_bbs(serialize_key, event_name, data)

	local key = mz3.show_common_edit_dlg("����", "�������������������͂��ĉ�����", last_searched_key_bbs);
	if key == nil then
		return false;
	end
	last_searched_key_bbs = key;
	key = string.upper( key );

	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);
	local n = list:get_count();
	for i=0, n-1 do
		local data = list:get_data(i);
		s = mz3_data.get_text(data, 'name');
		s = string.upper( s );
		if s:find( key, 1, true ) ~= nil then
			mz3_main_view.select_body_item(i);
			last_searched_index_bbs = i;
			break;
		end
	end
end


--- �J�e�S���擾���̃n���h��
--
-- @param event_name    'retrieve_category_item'
-- @param serialize_key �J�e�S���A�C�e���̃V���A���C�Y�L�[
--
function on_retrieve_category_item(event_name, serialize_key, body, wnd)
	if serialize_key~="2CH_BOOKMARK" then
		return false;
	end
	
	-- �u�b�N�}�[�N��������
	local list = mz3_main_view.get_body_item_list();
	list = MZ3DataList:create(list);

	list:clear();

	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- �S���擾
		local file = f:read('*a');
		f:close();
		
		for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
			-- data ����
			data = MZ3Data:create();
			
			type = mz3.get_access_type_by_key('2CH_THREAD');
			data:set_access_type(type);
			
			data:set_text('name', thread_name);
			data:set_text('title', title);

			data:set_text('url', url);
			data:set_text('browse_uri', url);

			-- ���X�g�ɒǉ�
			list:add(data.data);
			
			-- data �폜
			data:delete();
		end
	end

	-- �u�b�N�}�[�N�̓��[�J���X�g���[�W
	return true, RETRIEVE_CATEGORY_ITEM_RVAL_LOCALSTORAGE;
end
mz3.add_event_listener("retrieve_category_item", "2ch.on_retrieve_category_item");


--- ���C�ɓ���X���ǉ�
function on_add_bookmark(serialize_key, event_name, data)

	-- �ǉ����ڎ擾���T�j�^�C�W���O
	local body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	local category = MZ3Data:create(mz3_main_view.get_selected_category_item());
	
	local thread_name = category:get_text('name');
	thread_name = thread_name:gsub('��', '');
	thread_name = thread_name:gsub("\n", "");
	
	local new_url = body:get_text('url');
	local new_title = body:get_text('title');
	new_url = new_url:gsub("\n", "");
	new_title = new_title:gsub("\n", "");

	local new_item_line = new_url .. "\t" .. new_title .. "\t" .. thread_name;

	-- ���݃`�F�b�N
	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- �S���擾
		local file = f:read('*a');
		f:close();
		
		for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
			if url==new_url then
				mz3.alert(new_title .. '�͊��ɓo�^����Ă��܂�');
				return;
			end
		end
	end
	
	
	-- �ǉ�
	f = io.open(bookmark_file_path, "a");
	if f~=nil then
		f:write(new_item_line .. "\n");
		f:close();
		
		mz3.alert(new_title .. "��ǉ����܂����B");
	end
end


--- ���C�ɓ���X���폜
function on_remove_bookmark(serialize_key, event_name, data)

	-- �폜�Ώێ擾
	local body = MZ3Data:create(mz3_main_view.get_selected_body_item());
	
	local target_url = body:get_text('url');
	local target_title = body:get_text('title');

	-- �폜���쐬
	local f = io.open(bookmark_file_path, "r");
	if f~= nil then
		-- �S���擾
		local file = f:read('*a');
		f:close();
		
		-- �폜
		f = io.open(bookmark_file_path, "w");
		if f~=nil then
			local deleted = false;
			for url, title, thread_name in file:gmatch("(.-)\t(.-)\t(.-)\n") do
				if url==target_url then
					deleted = true;
				else
					local item_line = url .. "\t" .. title .. "\t" .. thread_name;
					f:write(item_line .. "\n");
				end
			end
			f:close();
			
			if deleted then
				mz3.alert(target_title .. " ���폜���܂����B");
			else
				mz3.alert(target_title .. " �͍폜�ς݂ł��B");
			end
		end
	end
end


mz3.logger_debug('2ch.lua end');
