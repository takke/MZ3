--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : APIラッパークラス
--
-- $Id$
--------------------------------------------------

--------------------------------------------------
-- Wrapper for mz3_data
--------------------------------------------------
MZ3Data = {};

function MZ3Data:create(data)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if data==nil then
		object.data = mz3_data.create();
	else
		object.data = data;
	end
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
	-- メモ：メタテーブルはフックのようなもの
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
function MZ3Data:get_integer64_as_string(name)	return mz3_data.get_integer64_as_string(self.data, name);	end
function MZ3Data:set_integer64_from_string(name, value)	return mz3_data.set_integer64_from_string(self.data, name, value);	end
function MZ3Data:parse_date_line(line)			return mz3_data.parse_date_line(self.data, line);		end
function MZ3Data:get_link_list_size()			return mz3_data.get_link_list_size(self.data);			end
function MZ3Data:get_link_list_url(idx)			return mz3_data.get_link_list_url(self.data, idx);		end
function MZ3Data:get_link_list_text(idx)		return mz3_data.get_link_list_text(self.data, idx);		end
function MZ3Data:add_link_list(url, text, t)	return mz3_data.add_link_list(self.data, url, text, t);	end
function MZ3Data:clear()						return mz3_data.clear(self.data);						end
function MZ3Data:add_child(child)				return mz3_data.add_child(self.data, child.data);		end

-- text_array を全て連結した文字列を返すヘルパー
function MZ3Data:get_text_array_joined_text(name)
	item = '';
	n = mz3_data.get_text_array_size(self.data, name);
	for i=0, n-1 do
		item = item .. self:get_text_array(name, i);
	end

	return item;
end

-- シリアライズキーの取得
function MZ3Data:get_serialize_key()
	return mz3.get_serialize_key_by_access_type(self:get_access_type());
end

--------------------------------------------------
-- Wrapper for mz3_data_list
--------------------------------------------------
MZ3DataList = {};

function MZ3DataList:create(data_list)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if data_list==nil then
		object.data_list = mz3_data_list.create();
	else
		object.data_list = data_list;
	end
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
	setmetatable(object, { __index = MZ3DataList });
	return object;
end

function MZ3DataList:clear()				return mz3_data_list.clear(self.data_list);					end
function MZ3DataList:add(data)				return mz3_data_list.add(self.data_list, data);				end
function MZ3DataList:insert(index, data)	return mz3_data_list.insert(self.data_list, index, data);	end
function MZ3DataList:get_count()			return mz3_data_list.get_count(self.data_list);				end
function MZ3DataList:get_data(index)		return mz3_data_list.get_data(self.data_list, index);		end
function MZ3DataList:delete()				return mz3_data_list.delete(self.data_list);				end
function MZ3DataList:merge(new_list, size)	return mz3_data_list.merge(self.data_list, new_list.data_list, size);	end


--------------------------------------------------
-- Wrapper for mz3_htmlarray
--------------------------------------------------
MZ3HTMLArray = {};

function MZ3HTMLArray:create(htmlarray)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	object.htmlarray = htmlarray;
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
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
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if type==nil then
		object.type = mz3_access_type_info.new_access_type();
	else
		object.type = type;
	end
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
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
function MZ3AccessTypeInfo:set_cruise_target(f)			return mz3_access_type_info.set_cruise_target(self.type, f);	end


--------------------------------------------------
-- Wrapper for mz3_menu
--------------------------------------------------
MZ3Menu = {};

function MZ3Menu:create_popup_menu(menu)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if menu==nil then
		object.menu = mz3_menu.create_popup_menu();
	else
		object.menu = menu;
	end
	
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
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
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	object.item = mz3_group_item.create(name);
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
	setmetatable(object, { __index = MZ3GroupItem });
	return object;
end

function MZ3GroupItem:append_category(title, key, url)	return mz3_group_item.append_category(self.item, title, key, url);	end
function MZ3GroupItem:delete()							return mz3_group_item.delete(self.item);	end


--------------------------------------------------
-- Wrapper for mz3_post_data
--------------------------------------------------
MZ3PostData = {};

function MZ3PostData:create(post_data)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if post_data == nil then
		object.post_data = mz3_post_data.create();
	else
		object.post_data = post_data;
	end
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
	setmetatable(object, { __index = MZ3PostData });
	return object;
end

function MZ3PostData:set_content_type(ct)	return mz3_post_data.set_content_type(self.post_data, ct);	end
function MZ3PostData:append_post_body(text)	return mz3_post_data.append_post_body(self.post_data, text);	end
function MZ3PostData:append_additional_header(header)	return mz3_post_data.append_additional_header(self.post_data, header);	end
function MZ3PostData:append_file(filename)	return mz3_post_data.append_file(self.post_data, filename);	end


--------------------------------------------------
-- Wrapper for mz3_graphics
--------------------------------------------------
MZ3Graphics = {};

function MZ3Graphics:create(dc)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	object.dc = dc;
	
	-- テーブルに見あたらないキーをクラスから引いてくるように設定
	setmetatable(object, { __index = MZ3Graphics });
	return object;
end

function MZ3Graphics:draw_text(text, x, y, w, h, format)		return mz3_graphics.draw_text(self.dc, text, x, y, w, h, format);		end
function MZ3Graphics:draw_image(image_cache_index, x, y, w, h)	return mz3_graphics.draw_image(self.dc, image_cache_index, x, y, w, h);	end
function MZ3Graphics:set_color(type, color_or_color_name)		return mz3_graphics.set_color(self.dc, type, color_or_color_name);		end
function MZ3Graphics:draw_rect(type, x, y, w, h, color_or_color_name)	return mz3_graphics.draw_rect(self.dc, type, x, y, w, h, color_or_color_name);	end
function MZ3Graphics:get_line_height()							return mz3_graphics.get_line_height(self.dc);	end
function MZ3Graphics:set_font_size(size)						return mz3_graphics.set_font_size(self.dc, size)	end
