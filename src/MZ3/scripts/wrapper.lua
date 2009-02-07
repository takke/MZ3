--[[
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version
 2 of the License, or (at your option) any later version.
]]
--------------------------------------------------
-- MZ3 Script : APIラッパークラス
--
-- $Id: mz3.lua 1144 2009-02-06 13:24:01Z takke $
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
--		mz3.logger_debug("new data");
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
function MZ3Data:get_text(name)					return mz3_data.get_text(self.data, name);				end
function MZ3Data:set_text(name, value)			return mz3_data.set_text(self.data, name, value);		end
function MZ3Data:get_text_array(name)			return mz3_data.get_text_array(self.data, name);		end
function MZ3Data:add_text_array(name, value)	return mz3_data.add_text_array(self.data, name, value);	end
function MZ3Data:get_integer(name)				return mz3_data.get_integer(self.data, name);			end
function MZ3Data:set_integer(name, value)		return mz3_data.set_integer(self.data, name, value);	end
function MZ3Data:parse_date_line(line)			return mz3_data.parse_date_line(self.data, line);		end

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
