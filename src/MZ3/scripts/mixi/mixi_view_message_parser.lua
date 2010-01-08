--[[
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
]]
--------------------------------------------------
-- MZ3 Script : mixi parsers
--------------------------------------------------
module("mixi", package.seeall)

----------------------------------------
-- アクセス種別の登録
----------------------------------------
-- TODO ホスト側で設定しているが、本来はこちらで設定すべき。
--      ただし、
--      inbox, outbox パーサよりも先に登録しなければならない、
--      estimate 対象にしなければならない、などの課題アリ。


--------------------------------------------------
-- 【mixi メッセージ詳細】
-- [content] view_message.pl 用パーサ
--
-- http://mixi.jp/view_message.pl
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_message_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_message_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);

	-- 行数取得
	local line_count = html:get_count();
	for i=200, line_count-1 do
		line = html:get_at(i);

		-- 日付抽出
		-- <dt>日付</dt>
		-- <dd>2008年11月12日 11時13分</dd>
		if line_has_strings(line, "<dt>日付</dt>") then
			line1 = html:get_at(i+1);
			
			-- 形式が特殊なので直接設定する
			data:set_date(line1:match("<dd>(.*)</dd>"));
		end

		-- 差出人ID抽出
		if line_has_strings(line, "<a", "href=", "show_friend.pl?id=", "\">", "</a>") then
			--mz3.logger_debug(line);
			id = line:match("\?id=([^\"]+)\"");

			--mz3.logger_debug("id : " .. id);
			data:set_integer("owner_id", id);
		end

		-- 本文抽出
		if line_has_strings(line, "<div", "class=", "messageDetailBody" ) then

			data:add_text_array("body", "\r\n");

			-- 整形して追加
			data:add_body_with_extract(line);

			-- </div> が存在すれば終了。
			if line_has_strings(line, "</div") then
				mz3.logger_debug("★</div>が見つかったので終了します(1)");
				break;
			end

			-- 普通のメッセージ
			for j=i+1, line_count-1 do
				line = html:get_at(j);

				-- 整形して追加
				data:add_body_with_extract(line);
				
				if line_has_strings(line, "</div") then
					mz3.logger_debug("★</div>が見つかったので終了します(2)");
					break;
				end
			end

			break;
		end
		
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_message_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- パーサの登録
----------------------------------------
-- 受信箱
mz3.set_parser("MIXI_MESSAGE", "mixi.mixi_view_message_parser");
