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
-- マイミク最新日記一覧
type = MZ3AccessTypeInfo.create();
type:set_info_type('category');									-- カテゴリ
type:set_service_type('mixi');									-- サービス種別
type:set_serialize_key('DIARY');								-- シリアライズキー
type:set_short_title('日記一覧');								-- 簡易タイトル
type:set_request_method('GET');									-- リクエストメソッド
type:set_cache_file_pattern('mixi\\new_friend_diary.html');		-- キャッシュファイル
type:set_request_encoding('euc-jp');							-- エンコーディング
type:set_default_url('http://mixi.jp/new_friend_diary.pl');
type:set_body_header(1, 'title', 'タイトル');
type:set_body_header(2, 'name', '名前>>');
type:set_body_header(3, 'date', '日時>>');
type:set_body_integrated_line_pattern(1, '%2\t(%3)');
type:set_body_integrated_line_pattern(2, '%1');
type:set_cruise_target(true);

-- TODO 「mixi 日記詳細」はホスト側で設定しているが、本来はこちらで設定すべき。


--------------------------------------------------
-- 【mixi マイミク最新日記一覧】
-- [list] new_friend_diary.pl 用パーサ
--
-- 引数:
--   parent: 上ペインの選択オブジェクト(MZ3Data*)
--   body:   下ペインのオブジェクト群(MZ3DataList*)
--   html:   HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_list_new_friend_diary_parser(parent, body, html)
	mz3.logger_debug("mixi_list_new_friend_diary_parser start");

	-- wrapperクラス化
	body = MZ3DataList:create(body);
	html = MZ3HTMLArray:create(html);

	-- 全消去
	body:clear();
	
	local t1 = mz3.get_tick_count();
	
	local back_data = nil;
	local next_data = nil;

	-- 行数取得
	local line_count = html:get_count();
	
	-- 日記開始フラグの探索
	local i_start_line = 100;
	while (i_start_line < line_count) do
		line = html:get_at(i_start_line);
		
		if line_has_strings(line, "newFriendDiary") then
			i_start_line = i_start_line + 1;
			break;
		end
		
		i_start_line = i_start_line + 1;
	end
	
	-- 各日記項目の取得
	local i=i_start_line;
	for i=i_start_line, line_count-1 do
		line = html:get_at(i);
--		mz3.logger_debug(i .. " : " .. html:get_at(i));

		-- 次へ、前への抽出処理
		if back_data==nil and next_data==nil then
			back_data, next_data = parse_next_back_link(line, "new_friend_diary.pl");
		end
	end
	
	-- ul の範囲を取得
	sub_html = get_sub_html(html, i_start_line, line_count, {'<ul'}, {'</ul>'});
	
	-- ul の中の各 dt, dd を取得
	for dt, dd in sub_html:gmatch("<dt>(.-)</dt>.-<dd>(.-)</dd>") do
--		mz3.logger_debug('dt: ' .. dt);
		
		-- data 生成
		data = MZ3Data:create();

		-- 時刻
		--<dt>12月31日&nbsp;05:32</dt>
		date = mz3.decode_html_entity(dt);
		data:set_date(date);

		-- 見出し、URL、名前の抽出
		--<dd><a href="view_diary.pl?id=xxx&owner_id=xxx">タイトル</a> (なまえ)<div style="visibility: hidden;" class="diary_pop" id="xxx"></div>
		-- or
		--<dd><a href="view_diary.pl?url=xxx&owner_id=xxx">タイトル</a> (なまえ)
		url, title, name = dd:match('href="(.-)">(.-)</a> %((.-)%)');
		title = mz3.decode_html_entity(title);
		data:set_text("title", title);

		-- URL 取得
		data:set_text("url", url);
		
		-- ID 設定
		id = get_param_from_url(url, "id");
		data:set_integer('id', id);

		-- 名前
		name = mz3.decode_html_entity(name);
		data:set_text("name", name);
		data:set_text("author", name);

		-- URL に応じてアクセス種別を設定
		type = mz3.estimate_access_type_by_url(url);
		data:set_access_type(type);

		-- data 追加
		body:add(data.data);

		-- data 削除
		data:delete();
	end

	-- 前、次へリンクの追加
	if back_data~=nil then
		-- 先頭に挿入
		body:insert(0, back_data.data);
		back_data:delete();
	end
	if next_data~=nil then
		-- 末尾に追加
		body:add(next_data.data);
		next_data:delete();
	end
	
	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_list_new_friend_diary_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


--------------------------------------------------
-- 【mixi 日記詳細】
-- [content] view_diary.pl 用パーサ
--
-- http://mixi.jp/view_diary.pl
--
-- 引数:
--   data:  上ペインのオブジェクト群(MZ3Data*)
--   dummy: NULL
--   html:  HTMLデータ(CHtmlArray*)
--------------------------------------------------
function mixi_view_diary_parser(data, dummy, html)
	mz3.logger_debug("mixi_view_diary_parser start");
	local t1 = mz3.get_tick_count();

	-- wrapperクラス化
	data = MZ3Data:create(data);
	html = MZ3HTMLArray:create(html);
	
	data:clear();

	-- とりあえず改行出力
	data:add_body_with_extract("<br>");

	-- 行数取得
	local line_count = html:get_count();
	i = 10;
	while i < line_count do
		line = html:get_at(i);

		-- <title> タグからタイトルを取得する
		if line_has_strings(line, "<title>", "</title>") then
			-- 自分の日記なら　　「<title>[mixi] タイトル</title>」
			-- 自分以外の日記なら「<title>[mixi] 名前 | タイトル</title>」
			-- という形式なので、タイトル部だけを抽出
			local title = line:match("<title>%[mixi%] (.-)</title>");
			title = mz3.decode_html_entity(title);
			after = title:match(" | (.-)$");
			if after~=nil then
				title = after;
			end

			data:set_text("title", title);
		end
		
		-- 「最近の日記」の取得
		if line_has_strings(line, '<ul class="contentsListDiary">') then
			i = i + 1;
			while i<line_count do
				line = html:get_at(i);
				if line_has_strings(line, '</ul') then
					break;
				end
				local url, title = line:match('<li><a href="(.-)">(.-)</a>');
				if url~=nil then
--					mz3.logger_debug(url .. title);
					data:add_link_list(url, mz3.decode_html_entity(title), 'page');
				end
				i = i+1;
			end
		end

		-- 日時の取得
		-- 公開範囲の取得
		if line_has_strings(line, '<div', 'listDiaryTitle') then
			i = i+1;
			while i<line_count do
				line = html:get_at(i);
				if line_has_strings(line, '</dl') then
					break;
				end

				local img_alt = line:match('<img.-alt="(.-)"');
				if img_alt~=nil then
					data:set_text('opening_range', img_alt);
				end

				local date = line:match('<dd>(.-)</dd>');
				if date~=nil then
					data:set_date(date);
				end
				i = i + 1;
			end
		end

		-- 日記の著者
		if line_has_strings(line, '<div', 'class', 'diaryTitle') then
			-- 自分の日記なら「XXXの日記」、自分以外なら「XXXさんの日記」のはず。
			-- この規則で著者を解析。

			-- 次の行の <h2> から取得
			line = html:get_at(i+1);
			if line_has_strings(line, 'diaryTitleFriend') then
				author = line:match('<h2>(.-)さんの日記</h2>');
			else
				author = line:match('<h2>(.-)の日記</h2>');
			end
			if author~=nil then
				author = mz3.decode_html_entity(author);
				data:set_text('author', author);
				data:set_text('name', author);
			end
		end

		-- 日記の添付写真
		if line_has_strings(line, '<div', 'class', 'diaryPhoto') then
			i = i + 1;
			while i<line_count do
				line = html:get_at(i);
				if line_has_strings(line, '</div') then
					break;
				end

				data:add_body_with_extract(line);
				i = i + 1;
			end
--			data:add_body_with_extract("<br>");
		end

		if line_has_strings(line, '<div', 'class', 'diaryPagingRight') then
			-- 前の日記へのリンク
			local link = line:match('<a[^>]*>前の日記</a>');
			if link~=nil then
				mz3.logger_debug('prev_diary:' .. link);
				data:set_text('prev_diary', link);
			end
			-- 次の日記へのリンク
			local link = line:match('<a[^>]*>次の日記</a>');
			if link~=nil then
				mz3.logger_debug('next_diary:' .. link);
				data:set_text('next_diary', link);
			end
		end

		-- TODO それらしいサンプルHTMLが見あたらないのでとりあえず放置
--[[
		// 全てを表示へのリンク
		try {
			const xml2stl::Node& li = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
												   .getNode(L"div", L"id=diaryComment")
												   .getNode(L"div", L"class=diaryMainArea02 deleteButton")
												   .getNode(L"form")
												   .getNode(L"div", L"class=pageNavigation01 preComment01")
												   .getNode(L"div", L"class=pageList03")
												   .getNode(L"ul")
												   .getNode(L"li");

			CString FullLink = li.getTextAll().c_str();
			data_.SetFullDiary( FullLink );

		} catch (xml2stl::NodeNotFoundException& e) {
			// 人の日記の場合
			try {
				const xml2stl::Node& li = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"id=diaryComment")
													   .getNode(L"div", L"class=pageNavigation01 preComment01")
													   .getNode(L"div", L"class=pageList03")
													   .getNode(L"ul")
													   .getNode(L"li");

				CString FullLink = li.getTextAll().c_str();
				data_.SetFullDiary( FullLink );

			} catch (xml2stl::NodeNotFoundException& e) {
				// リンクがなかったと判断する
				MZ3LOGGER_INFO( util::FormatString( L"「全てを表示」リンク取得エラー : %s", e.getMessage().c_str()) );
			}
			MZ3LOGGER_INFO( util::FormatString( L"自分の日記の「全てを表示」リンク取得エラー : %s", e.getMessage().c_str()) );
		}
]]

		-- 本文取得
		if line_has_strings(line, '<div', 'id', 'diary_body') then
			local sub_html = line;
			i = i + 1;
			while i<line_count do
				line = html:get_at(i);
				-- if line_has_strings(line, '<div class="diaryPaging01">') then
				if line_has_strings(line, '<div class="actionLink01">') or
				   line_has_strings(line, '<ul class="actionLink01">') then
					break;
				end

				-- youtube
				if line_has_strings(line, 'youtube_write') then
					local link = line:match("src=\"([^\"]+)\"");
					link2 = link:gsub('/v/', '/watch?v=');
					link2 = link2 .. "&feature=player_embedded";

					-- mz3.logger_debug(line);
					line = line:gsub( link, link2 )
					data:add_body_with_extract(line);
				end

				-- ニコ動
				if line_has_strings(line, 'div', 'class', 'nicovideo') then
					-- local link = line:match("thumb_watch/([^\"]+)\?");
					-- link = 'http://www.nicovideo.jp/watch/' .. link;
					data:add_body_with_extract(line);
				end

				-- 自前の動画
				if line_has_strings(line, 'div', 'player_1', 'insertVideo') then
					local j = i;
					while j<line_count do
						line = html:get_at(j);
						j = j +1;
						if line_has_strings( line, 'http://video.mixi.jp/view_video.pl' ) then
							link = line:match("\'([^\"]+)\'");

							line = '<div class=\"insertVideo\"><script type=\"text/javascript\" src=\"' .. link .. '></script></div>'
							-- data:add_body_with_extract(line);
							i = i + 1;
							break;
						end
					end
				end

				sub_html = sub_html .. line;
				i = i + 1;
			end
			-- script タグの除去
			sub_html = sub_html:gsub('<script.-</script>', '');
			sub_html = sub_html:gsub('/script>', '');
			data:add_body_with_extract(sub_html);
		end

		-- コメント取得
		i = parseDiaryComment(data, line, i, line_count, html);
		
		-- POST URL 解析
		if line_has_strings(line, '<form', 'comment_form') and
		   line_has_strings(line, 'add_comment.pl') then
			-- <form action="add_comment.pl?diary_id=xxx" method="post" name="comment_form">
			local url = line:match('action="(.-)"');
			if url~=nil then
				data:set_text('post_address', url);
				data:set_text('content_type', "application/x-www-form-urlencoded");
			end

			-- hidden
			i = i + 1;
			while i<line_count do
				line = html:get_at(i);
				if line_has_strings(line, '</form>') then
					break;
				end

				-- <input type="hidden" name="owner_id" value="85892" />
				if line_has_strings(line, '<input', 'hidden') then
					local v = line:match('value="(.-)"');
					data:set_integer('owner_id', v);
					break;
				end

				i = i + 1;
			end
			
			-- この投稿フォームがあったら解析終了
			break;
		end
		
		i = i + 1;
	end

	local t2 = mz3.get_tick_count();
	mz3.logger_debug("mixi_view_diary_parser end; elapsed : " .. (t2-t1) .. "[msec]");
end


-- コメントの取得
function parseDiaryComment(data, line, i, line_count, html)
	if line_has_strings(line, 'diaryMainArea02') and
	   (line_has_strings(line, 'commentList') or
	    line_has_strings(line, 'deleteButton'))
	then
		
		-- 2件目以降は子要素として投入する
		comment_index = 1;
		in_dd = false;
		child = MZ3Data:create();

		i = i + 1;
		while i<line_count do
			line = html:get_at(i);
			if line_has_strings(line, '<div class="diaryMainArea02 commentForm">') then
				break;
			end

			if line_has_strings(line, 'diaryCommentbox') then
				child:clear();
				in_dd = false;
				child:add_body_with_extract('<br>');
			end
			
			if line_has_strings(line, '</div') then
				if child:get_text('author')~="" then
					data:add_child(child);
					child:clear();
					child:set_text('author', '');
				end
			end
			
			if line_has_strings(line, 'commentTitleName') then
				local v = line:match('<a.->(.-)</a>');
				if v==nil then
					-- 自分の日記なら2行目にある
					i = i+1;
					line = html:get_at(i);
					v = line:match('<a.->(.-)</a>');
				end
				-- 名前
				child:set_text('author', v);
				-- コメント番号
				child:set_integer('comment_index', comment_index);
				comment_index = comment_index+1;
			end
			
			if line_has_strings(line, 'commentTitleDate') then
				local v = line:match('<span.->(.-)<');
				v = mz3.decode_html_entity(v);
				child:set_date(v);
			end
			
			if in_dd then
				if line_has_strings(line, '</dd>') then
					in_dd = false;
				else
					child:add_body_with_extract(line);
				end
			else
				if line_has_strings(line, '<dd>') then
					in_dd = true;
				end
			end
			
			i = i + 1;
		end
		child:delete();

	end
	
	return i;
end

----------------------------------------
-- パーサの登録
----------------------------------------
mz3.set_parser("DIARY",              "mixi.mixi_list_new_friend_diary_parser");
mz3.set_parser("MIXI_DIARY",         "mixi.mixi_view_diary_parser");
mz3.set_parser("MIXI_NEIGHBORDIARY", "mixi.mixi_view_diary_parser");
mz3.set_parser("MIXI_MYDIARY",       "mixi.mixi_view_diary_parser");


----------------------------------------
-- estimate 対象に追加
----------------------------------------

--- estimate 対象判別イベントハンドラ
--
-- @param event_name 'estimate_access_type_by_url'
-- @param url        解析対象URL
--
function on_estimate_access_type_by_url_for_mixi_diary(event_name, url, data1, data2)

    -- マイミク最新日記一覧
	if line_has_strings(url, 'new_friend_diary.pl') then
		return true, mz3.get_access_type_by_key('DIARY');
	end

	return false;
end
-- イベントフック関数の登録
mz3.add_event_listener("estimate_access_type_by_url", "mixi.on_estimate_access_type_by_url_for_mixi_diary");
