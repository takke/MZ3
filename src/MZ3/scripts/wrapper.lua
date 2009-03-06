--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
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
function MZ3Data:get_date()						return mz3_data.get_date(self.data);					end
function MZ3Data:get_text(name)					return mz3_data.get_text(self.data, name);				end
function MZ3Data:set_text(name, value)			return mz3_data.set_text(self.data, name, value);		end
function MZ3Data:get_text_array(name, idx)		return mz3_data.get_text_array(self.data, name, idx);	end
function MZ3Data:get_text_array_size(name)		return mz3_data.get_text_array_size(self.data, name);	end
function MZ3Data:add_text_array(name, value)	return mz3_data.add_text_array(self.data, name, value);	end
function MZ3Data:get_integer(name)				return mz3_data.get_integer(self.data, name);			end
function MZ3Data:set_integer(name, value)		return mz3_data.set_integer(self.data, name, value);	end
function MZ3Data:parse_date_line(line)			return mz3_data.parse_date_line(self.data, line);		end

--------------------------------------------------
-- Wrapper for mz3_data_list
--------------------------------------------------
MZ3DataList = {};

function MZ3DataList:create(data_list)
	-- インスタンス用テーブル
	local object = {}
	
	-- メンバー変数の設定
	if data_list==nil then
		return nil;
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
