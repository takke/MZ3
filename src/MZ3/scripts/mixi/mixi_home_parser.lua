--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--
-- $Id$
--------------------------------------------------
module("mixi", package.seeall)

--------------------------------------------------
-- 【mixiトップページ】
-- [content] home.pl ログイン後のメイン画面用パーサ
--
-- http://mixi.jp/home.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--
-- parent の下記の変数に情報を格納する。
-- <ul>
--  <li>[string] "owner_id"       => 自分のID
--  <li>[int] "new_message_count" => 新着メッセージ数
--  <li>[int] "new_comment_count" => 新着コメント数
--  <li>[int] "new_apply_count"   => 承認待ち数
-- </ul>
--------------------------------------------------
function mixi_home_parser(parent, body, html)
	mz3.logger_debug("mixi_home_parser start");

	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	parent = MZ3Data:create(parent);
	html = MZ3HTMLArray:create(html);

	-- 結果の初期化
	parent:set_integer("new_message_count", 0);
	parent:set_integer("new_comment_count", 0);
	parent:set_integer("new_apply_count", 0);
	mz3.logger_debug("original owner_id : [" .. parent:get_text("owner_id") .. "]");
	
	-- 新着メッセージ、コメント、承認待ち件数の取得
	local line_count = html:get_count();
	local in_mixi_info_div = false;
	for i=350, line_count-1 do
		line = html:get_at(i);
		-- <div id="mixiInfo">
		-- ...
		-- <ul>
		-- ...
		-- <li class="redTxt"><a href="list_request.pl"><b>マイミクシィ追加リクエストが2件あります！</b></a></li>
		-- <li class="redTxt"><a href="list_message.pl">新着メッセージが2件あります！</a></li>
		-- <li class="redTxt"><a href="view_diary.pl?id=xxx&owner_id=xxx">1件の日記に対して新着コメントがあります！</a></li>
		-- ...
		-- </ul>
		-- を検索対象とする

		if not in_mixi_info_div then
			if line_has_strings(line, '<div', 'id=', 'mixiInfo') then
				in_mixi_info_div = true;
			end
		else
			if line_has_strings(line, '</ul>') then
				break;
			else
				-- li タグの中を検索する
				local li_text = line:match('<li[^>]*>(.*)</li>');
				if li_text ~= nil then
--					mz3.logger_debug(li_text);

					local n = 0;
					n = li_text:match('マイミクシィ追加リクエストが(.*)件あります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_apply_count', n);
					end
					
					n = li_text:match('[ジ]が(.*)件あります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_message_count', n);
					end
					
					n = li_text:match('([0-9]*)件の日記に対して新着コメントがあります！');
--					mz3.logger_debug(n);
					if n ~= nil then
						parent:set_integer('new_comment_count', n);
					end
				end
			end
		end
	end
	
	
	-- owner_id の取得
--	parent:set_text('owner_id', '');	-- デバッグ用
	if parent:get_text('owner_id')=="" then
		mz3.logger_debug('owner_id が未取得なので解析します');
		
		for i=0, line_count-1 do
			line = html:get_at(i);
			if line_has_strings(line, "<a", "href=", "add_diary.pl?id=") then
--				mz3.logger_debug("[" .. line .. "]");
				local url = line:match("href=\"([^\"]+)\"");
--				mz3.logger_debug("[" .. url .. "]");
				local id = get_param_from_url(url, 'id');
				if id=="" then
					mz3.logger_error("add_diary.pl の引数に id パラメータが見つかりません。line[" .. line .. "]");
				else
					mz3.logger_debug("owner_id[" .. id .. "]");
					parent:set_text('owner_id', id);
				end
				break;
			end
		end
		
		if parent:get_text('owner_id')=="" then
			mz3.logger_error('owner_id を取得できませんでした');
		end
	end
	
	mz3.logger_debug("parsed owner_id : [" .. parent:get_text("owner_id") .. "]");

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_home_parser end; elapsed : " .. (t2-t1) .. "[msec]");

	return true;
end

mz3.set_parser("MIXI_HOME", "mixi.mixi_home_parser");
