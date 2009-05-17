--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : API���b�p�[�N���X
--
-- $Id$
--------------------------------------------------

--------------------------------------------------
-- Wrapper for mz3_data
--------------------------------------------------
MZ3Data = {};

function MZ3Data:create(data)
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	if data==nil then
		object.data = mz3_data.create();
	else
		object.data = data;
	end
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	-- �����F���^�e�[�u���̓t�b�N�̂悤�Ȃ���
	setmetatable(object, { __index = MZ3Data });
	return object;
end

function MZ3Data:delete()						return mz3_data.delete(self.data);						end
function MZ3Data:set_access_type(type)			return mz3_data.set_access_type(self.data, type);		end
function MZ3Data:get_access_type()				return mz3_data.get_access_type(self.data);				end
function MZ3Data:set_date(date)					return mz3_data.set_date(self.data, date);				end
function MZ3Data:get_date()						return mz3_data.get_date(self.data);					end
function MZ3Data:get_text(name)					return mz3_data.get_text(self.data, name);				end
function MZ3Data:set_text(name, value)			return mz3_data.set_text(self.data, name, value);		end
function MZ3Data:get_text_array(name, idx)		return mz3_data.get_text_array(self.data, name, idx);	end
function MZ3Data:get_text_array_size(name)		return mz3_data.get_text_array_size(self.data, name);	end
function MZ3Data:add_text_array(name, value)	return mz3_data.add_text_array(self.data, name, value);	end
function MZ3Data:add_body_with_extract(value)	return mz3_data.add_body_with_extract(self.data, value);end
function MZ3Data:get_integer(name)				return mz3_data.get_integer(self.data, name);			end
function MZ3Data:set_integer(name, value)		return mz3_data.set_integer(self.data, name, value);	end
function MZ3Data:parse_date_line(line)			return mz3_data.parse_date_line(self.data, line);		end
function MZ3Data:get_link_list_size()			return mz3_data.get_link_list_size(self.data);			end
function MZ3Data:get_link_list_url(idx)			return mz3_data.get_link_list_url(self.data, idx);		end
function MZ3Data:get_link_list_text(idx)		return mz3_data.get_link_list_text(self.data, idx);		end

-- text_array ��S�ĘA�������������Ԃ��w���p�[
function MZ3Data:get_text_array_joined_text(name)
	item = '';
	n = mz3_data.get_text_array_size(self.data, name);
	for i=0, n-1 do
		item = item .. self:get_text_array(name, i);
	end

	return item;
end


--------------------------------------------------
-- Wrapper for mz3_data_list
--------------------------------------------------
MZ3DataList = {};

function MZ3DataList:create(data_list)
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	if data_list==nil then
		return nil;
	else
		object.data_list = data_list;
	end
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	setmetatable(object, { __index = MZ3DataList });
	return object;
end

function MZ3DataList:clear()				return mz3_data_list.clear(self.data_list);					end
function MZ3DataList:add(data)				return mz3_data_list.add(self.data_list, data);				end
function MZ3DataList:insert(index, data)	return mz3_data_list.insert(self.data_list, index, data);	end

--------------------------------------------------
-- Wrapper for mz3_htmlarray
--------------------------------------------------
MZ3HTMLArray = {};

function MZ3HTMLArray:create(htmlarray)
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	object.htmlarray = htmlarray;
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	setmetatable(object, { __index = MZ3HTMLArray });
	return object;
end

function MZ3HTMLArray:get_count(index)	return mz3_htmlarray.get_count(self.htmlarray);		end
function MZ3HTMLArray:get_at(index)		return mz3_htmlarray.get_at(self.htmlarray, index);	end


--------------------------------------------------
-- Wrapper for mz3_access_type_info
--------------------------------------------------
MZ3AccessTypeInfo = {};

function MZ3AccessTypeInfo:create(type)
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	if type==nil then
		object.type = mz3_access_type_info.new_access_type();
	else
		object.type = type;
	end
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	setmetatable(object, { __index = MZ3AccessTypeInfo });
	return object;
end

function MZ3AccessTypeInfo:set_info_type(c)				return mz3_access_type_info.set_info_type(self.type, c);			end
function MZ3AccessTypeInfo:set_service_type(st)			return mz3_access_type_info.set_service_type(self.type, st);		end
function MZ3AccessTypeInfo:set_serialize_key(key)		return mz3_access_type_info.set_serialize_key(self.type, key);		end
function MZ3AccessTypeInfo:set_short_title(title)		return mz3_access_type_info.set_short_title(self.type, title);		end
function MZ3AccessTypeInfo:set_request_method(m)		return mz3_access_type_info.set_request_method(self.type, m);		end
function MZ3AccessTypeInfo:set_cache_file_pattern(pat)	return mz3_access_type_info.set_cache_file_pattern(self.type, pat);	end
function MZ3AccessTypeInfo:set_request_encoding(e)		return mz3_access_type_info.set_request_encoding(self.type, e);		end
function MZ3AccessTypeInfo:set_default_url(url)			return mz3_access_type_info.set_default_url(self.type, url);		end
function MZ3AccessTypeInfo:set_body_header(no, t, n)	return mz3_access_type_info.set_body_header(self.type, no, t, n);	end
function MZ3AccessTypeInfo:set_body_integrated_line_pattern(l, pat)	return mz3_access_type_info.set_body_integrated_line_pattern(self.type, l, pat);	end


--------------------------------------------------
-- Wrapper for mz3_menu
--------------------------------------------------
MZ3Menu = {};

function MZ3Menu:create_popup_menu()
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	object.menu = mz3_menu.create_popup_menu();
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	setmetatable(object, { __index = MZ3Menu });
	return object;
end

function MZ3Menu:append_menu(type, title, item_id)	return mz3_menu.append_menu(self.menu, type, title, item_id);	end
function MZ3Menu:append_submenu(title, submenu)		return mz3_menu.append_submenu(self.menu, title, submenu.menu);	end
function MZ3Menu:popup(wnd)							return mz3_menu.popup(self.menu, wnd);	end
function MZ3Menu:delete()							return mz3_menu.delete(self.menu);	end

--------------------------------------------------
-- Wrapper for mz3_group_item (TAB)
--------------------------------------------------
MZ3GroupItem = {};

function MZ3GroupItem:create(name)
	-- �C���X�^���X�p�e�[�u��
	local object = {}
	
	-- �����o�[�ϐ��̐ݒ�
	object.item = mz3_group_item.create(name);
	
	-- �e�[�u���Ɍ�������Ȃ��L�[���N���X��������Ă���悤�ɐݒ�
	setmetatable(object, { __index = MZ3GroupItem });
	return object;
end

function MZ3GroupItem:append_category(title, key, url)	return mz3_group_item.append_category(self.item, title, key, url);	end
function MZ3GroupItem:delete()							return mz3_group_item.delete(self.item);	end
