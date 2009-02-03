--------------------------------------------------
-- MZ3 API : mixi
--------------------------------------------------
module("mixi", package.seeall)

function hoge()
	print "fuga"
	mz3.hoge();
end

--[[
function after_parse()
end
]]

--------------------------------------------------
-- line に指定された全文字列を順に含むか
--------------------------------------------------
function line_has_strings(line, ...)
	local args = {...}
	local p = 1;
	
	for i=1, #args do
--		print(args[i]);
		p = string.find(line, args[i], p, false);
		if p==nil then
			return false;
		end
		p = p+1;
	end

	return true;
end

function get_param_from_url(url, param)
	-- ? 以降を抽出
	local val = string.match(url, "[\?&]" .. param .. "=([^&]+).*$");
	if (val == nil) then
		return "";
	end
	
	return val;
end

----------------------------------------
-- 【コミュニティ最新書き込み一覧】
-- [list] new_bbs.pl 用パーサ
--
-- http://mixi.jp/new_bbs.pl
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
----------------------------------------
function bbs_parser(parent, body, html)
	mz3.logger_debug("bbs_parser start");
	
	local t1 = mz3.get_tick_count();
	local inDataRegion = false;
	
	-- 行数取得
	local line_count = mz3_htmlarray.get_count(html);
	for i=140, line_count-1 do
		line = mz3_htmlarray.get_at(html, i);
		
--		mz3.logger_debug(i .. " : " .. mz3_htmlarray.get_at(html, i));

		-- TODO 次へ、前への抽出処理
--		if not inDataRegion then
--			
--		end

		-- 項目探索
		-- <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
		if line_has_strings(line, "<dt", "class", "iconTopic") or 
		   line_has_strings(line, "<dt", "class", "iconEvent") or 
		   line_has_strings(line, "<dt", "class", "iconEnquete") then

			inDataRegion = true;

			-- data 生成
			data = mz3_data.create();

			-- 日付のパース
			mz3_data.parse_date_line(data, line);

--			mz3.trace(i .. " : " .. line);
			
			-- 次行取得
			i = i+1;
			line2 = mz3_htmlarray.get_at(html, i);
--			mz3.trace(i .. " : " .. line2);
			
			-- 見出し
			-- <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">
			-- 【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
			title, after = string.match(line2, ">([^<]+)(<.*)$");
			
			-- エンティティデコード
			title = mz3.decode_html_entity(title);
--			mz3.logger_debug(after);

			-- アンケート、イベントの場合はタイトルの前にマークを付ける
			if line_has_strings(line, "iconEvent") then
				title = "【☆】" .. title;
			elseif line_has_strings(line, "iconEnquete") then
				title = "【＠】" .. title;
			end
--			mz3.logger_debug(title);
			mz3_data.set_text(data, "title", title);
--			mz3.logger_debug(mz3_data.get_text(data, "title"));
			
			-- URL 取得
			url = string.match(line2, "href=\"([^\"]+)\"");
--			mz3.logger_debug(url);
			mz3_data.set_text(data, "url", url);
			
			-- コメント数
			mz3_data.set_integer(data, "comment_count", get_param_from_url(url, "comment_count"));
			
			-- id
			id = get_param_from_url(url, "id");
			mz3_data.set_integer(data, "id", id);
--			mz3.logger_debug(get_param_from_url(url, "comm_id"));

			-- コミュニティ名
			name = string.match(after, "</a>.*[(](.*)[)]</dd>");
			name = mz3.decode_html_entity(name);
--			mz3.logger_debug(name);
			mz3_data.set_text(data, "name", name);
			
			-- URL に応じてアクセス種別を設定
			type = mz3.estimate_access_type_by_url(url);
			mz3_data.set_access_type(data, type);
			
			-- data 追加
			mz3_data_list.add(body, data);
			
			-- data 削除
			mz3_data.delete(data);
		end

		if inDataRegion and line_has_strings(line, "</ul>") then
			mz3.logger_debug("★</ul>が見つかったので終了します");
			break;
		end

	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("bbs_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


----------------------------------------
-- フック関数の登録
----------------------------------------

-- コミュ書込一覧
--mz3.set_parser("mixi", "BBS", bbs_parser);

--mz3.set_hook("mixi", "after_parse", after_parse);
