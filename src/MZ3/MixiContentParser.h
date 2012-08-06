/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MixiParser.h"
#include "xml2stl.h"

#ifdef BT_MZ3

/// mixi用パーサ
namespace mixi {

/// contents 系ページに対するパーサの基本クラス
class MixiContentParser : public MixiParserBase, public parser::MZ3ContentParser
{
public:
	/**
	 * 記事投稿用URL等の取得
	 */
	static void parsePostURL(int startIndex, CMixiData& data_, const CHtmlArray& html_)
	{
		const int lastLine = html_.GetCount();

		for (int i=startIndex; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			// <form name="bbs_comment_form" action="add_bbs_comment.pl?id=xxx&comm_id=yyy" enctype="multipart/form-data" method="post">
			if (line.Find(L"<form") != -1 &&
				line.Find(L"method=") != -1 &&
				line.Find(L"action=") != -1 &&
				line.Find(L"post") != -1) 
			{
				// delete_xxx.pl の場合は無視
				if (line.Find(L"delete_") != -1) {
					continue;
				}

				// Content-Type/enctype 解決
				if (line.Find(_T("multipart")) != -1) {
					data_.SetContentType(CONTENT_TYPE_MULTIPART);
				} else {
					data_.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
				}

				// action/URL 取得
				CString action;
				if( util::GetBetweenSubString( line, L"action=\"", L"\"", action ) < 0 ) {
					continue;
				}
				data_.SetPostAddress( action );

				switch(data_.GetAccessType()) {
				case ACCESS_DIARY:
				case ACCESS_MYDIARY:
				case ACCESS_NEIGHBORDIARY:
					break;
				default:
					// 以降の解析は不要。
					return;
				}
//				continue;
			}
			
			// <input type="hidden" name="owner_id" value="yyy" />
			if (line.Find(L"<input") != -1 &&
				line.Find(L"hidden") != -1 &&
				line.Find(L"owner_id") != -1)
			{
				CString ownerId;
				if( util::GetBetweenSubString( line, L"value=\"", L"\"", ownerId ) < 0 ) {
					continue;
				}
				MZ3LOGGER_DEBUG( _T("owner_id = ") + ownerId );
				data_.SetOwnerID( _wtoi(ownerId) );
			}

		}
	}

	/**
	 * ページ変更リンクの抽出。
	 */
	static bool parsePageLink( CMixiData& data, const CHtmlArray& html, int startIndex=180 )
	{
		// startIndex 以降に下記を発見したら、解析開始。
/*
<div class="pageNavigation01">
<div class="pageList01">
<div>[<ul><li><a href="view_bbs.pl?page=1&comm_id=xxx&id=yyy">1</a></li><li><a href="view_bbs.pl?page=2&comm_id=xxx&id=23332197">2</a></li><li><a href="view_bbs.pl?page=3&comm_id=xxx&id=23332197">3</a></li><li><a href="view_bbs.pl?page=4&comm_id=xxx&id=23332197">4</a></li><li><a href="view_bbs.pl?page=5&comm_id=xxx&id=23332197">5</a></li><li><a href="view_bbs.pl?page=6&comm_id=xxx&id=23332197">6</a></li></ul>]</div>
</div>
<div class="pageList02">
<ul>
<li><a href="view_bbs.pl?id=23332197&comm_id=xxx&page=all">全てを表示</a></li>
<li>最新の20件を表示</li>
</ul>
</div>
</div>
*/
		const int count = html.GetCount();
		int i=startIndex;

		// pageList01
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> 発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}

		// pageList02
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList02" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> 発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}


		return !data.m_linkPage.empty();
	}

	/**
	 * 「最新のトピック」の抽出。
	 */
	static bool parseRecentTopics( CMixiData& data, const CHtmlArray& html, int startIndex=200 )
	{
		// startIndex 以降に下記を発見したら、解析開始。
		// <ul class="newTopicList01">

		const int count = html.GetCount();
		int i=startIndex;
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<ul", L"class", L"newTopicList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// 終了タグを発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<p", L"class", L"utilityLinks03" ) ) {
				break;
			}

			CString str = line;
			if( line.Find( L"view_enquete.pl" ) != -1 ||
				line.Find( L"view_event.pl" ) != -1 ||
				line.Find( L"view_bbs.pl" ) != -1 )
			{
				ParserUtil::ExtractURI( str, data.m_linkPage );
			}
		}

		return !data.m_linkPage.empty();
	}

	/**
	 * コミュニティ名抽出
	 */
	static bool parseCommunityName( CMixiData& mixi, const CHtmlArray& html_ )
	{
		// <p class="utilityLinks03"><a href="view_community.pl?id=1198460">[MZ3 -Mixi for ZERO3-] コミュニティトップへ</a></p>
		const int lastLine = html_.GetCount();
		for (int i=0; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"<p", L"utilityLinks03", L"view_community.pl" )) {
				CString name;
				if (util::GetBetweenSubString( line, L"<a", L"</a>", name ) >= 0) {
					if (util::GetBetweenSubString( name, L"[", L"]", name ) >= 0) {
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
						mixi.SetName( name );
						return true;
					}
				}
			}
		}
		return false;
	}

};


/**
 * [content] view_bbs.pl 用パーサ
 * 【コミュニティートピック詳細】
 * http://mixi.jp/view_bbs.pl
 */
class ViewBbsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ ) 
	{
		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() start." );

		mixi.ClearAllList();
		mixi.ClearChildren();

		// 名前を初期化
		mixi.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		if (html_.GetAt(2).Find(_T("302 Moved")) != -1) {
			// 現在は見れない
			TRACE(_T("Moved\n"));
			return false;
		}

		bool bFoundMainText = false;	///< 本文解析成功？

		// 本文取得
		int i=100;
		for (; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (bFoundMainText) {
				// 本文取得済みなので終了。
				break;
			}

			// 投稿日時を取得する
			// <span class="date">2007年07月14日 22:22</span></dt>
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, mixi);
			}

			// タイトルを取得する
			// <span class="titleSpan"><span class="title">xxxxx</span>...
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"titleSpan", L"title" ) ) {
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// タグの除去
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				mixi.SetTitle( title );
				continue;
			}


			if (util::LineHasStringsNoCase( line, L"<dd", L"class", L"bbsContent" ) ) {
				// 本文開始。トピック本文ね。
				bFoundMainText = true;

				// とりあえず改行
				mixi.AddBody(_T("\r\n"));

				// <dd>
				// から
				// </dd>
				// までを取得し、解析する。

				// <dd> を探す。
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					// "show_friend.pl" つまりトピ作成者のプロフィールリンクがあれば、
					// トピ作成者のユーザ名を取得する。
					if( util::LineHasStringsNoCase( line, L"show_friend.pl" ) ) {

						ParserUtil::GetAuthor(line, &mixi);

						// この行にはないので次の行へ。
						continue;
					}

					if (util::LineHasStringsNoCase( line, L"<dd>" )) {
						// <dd>以降を取得し、あれば つっこんで終了。
						CString after;
						util::GetAfterSubString( line, L"<dd>", after );
						ParserUtil::AddBodyWithExtract( mixi, after );
						++i;
						break;
					}
				}

				// </dd> が現れるまで、解析する。
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					if (util::LineHasStringsNoCase( line, L"</dd>" )) {
						// 終了。
						break;
					}

					// 動画用scriptタグが見つかったらscriptタグ終了まで解析。
					if(! ParserUtil::ExtractVideoLinkFromScriptTag( mixi, i, html_ ) ) {
						// scriptタグ未発見なので、解析＆投入

						// 非表示のタグを削除
						if (isBbsHiddenLine(line)) {
							// この行を強制的にスキップ
							continue;
						}
						ParserUtil::AddBodyWithExtract( mixi, line );
					}
				}
			}
		}

		if (bFoundMainText) {
			// 本文取得済みなので、コメント取得処理を行う。
			for (; i<lastLine; i++) {
				const CString& line = html_.GetAt(i);

				// <dl class="commentList01"> を発見したら、コメント取得処理を行う。
				if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentList01" )) {
					mixi.ClearChildren();

					int index = ++i;
					while( index < lastLine ) {
						index = parseBBSComment(index, lastLine, &mixi, html_);
						if( index == -1 ) {
							// エラーなので終了
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}
			}
		}

		// コミュニティ名抽出
		parseCommunityName( mixi, html_ );

		// ページ移動リンクの抽出
		parsePageLink( mixi, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( mixi, html_ );

		// 書き込み先URLの取得
		parsePostURL( 200, mixi, html_ );

		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() finished." );
		return true;
	}

private:

	static bool isBbsHiddenLine(const CString& line)
	{
		// <p class="reportLink01" style="display: none;"><!-- ... --></p>
		if (util::LineHasStringsNoCase(line, L"<p class=\"reportLink01\"", L"display: none;", L"</p")) {
			return true;
		}
		return false;
	}

	/**
	 * ＢＢＳコメント取得 トピック コメント一覧
	 */
	static int parseBBSComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ ) 
	{
		INT_PTR lastLine = html_.GetCount(); //行数

		int retIndex = eIndex;
		CMixiData cmtData;
		bool findFlag = false;

/*
<dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;番号
</span>
<span class="date">2007年07月28日 21:09</span></dt>
<dd>
<dl class="commentContent01">
<dt><a href="show_friend.pl?id=XXX">なまえ</a></dt>
<dd>
...
</dd>
*/
		int i=0;
		for( i=sIndex; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// 番号取得
			// <dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;番号
			if (util::LineHasStringsNoCase( line, L"<dt", L"commentDate", L"senderId" )) {
				CString number;
				util::GetAfterSubString( line, L"senderId\">", number );
				// &nbsp; を消す
				while(number.Replace(L"&nbsp;",L"")) {}

				cmtData.SetCommentIndex(_wtoi(number));
			}

			// 番号取得２（編集可能なコミュ）
			// <span class="senderId"><input id="commentCheck01" name="comment_id" type="checkbox" value="291541807" /><label for="commentCheck01">&nbsp;12</label></span>
			if (util::LineHasStringsNoCase( line, L"<span", L"senderId", L"checkbox" )) {
				CString number;
				util::GetAfterSubString( line, L"<label", number );
				util::GetBetweenSubString( number, L">", L"</label", number );
				// &nbsp; を消す
				while(number.Replace(L"&nbsp;",L"")) {}
				cmtData.SetCommentIndex(_wtoi(number));
			}

			// 日付
			// <span class="date">2007年07月28日 21:09</span></dt>
			if (util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, cmtData);
			}

			// <div class="pageNavigation01"> を発見したら、コメント終了なので抜ける
			if (util::LineHasStringsNoCase( line, L"<div", L"class", L"pageNavigation01" )) {
				return -1;
			}

			// bbs_comment_formを発見したら、コメント終了なので抜ける 
			if (util::LineHasStringsNoCase( line, L"<form", L"bbs_comment_form", L"action" )) { 
				return -1; 
			}

			// <!-- ADD_COMMENT: start -->を発見したら、コメント終了なので抜ける
			if (util::LineHasStringsNoCase( line, L"<!", L"ADD_COMMENT", L"start" )) {
				return -1;
			}

			// <dl class="commentContent01"> を発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentContent01" )) {
				findFlag = true;
				i++;
				break;
			}
		}

		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// なまえ
			// <dt><a href="show_friend.pl?id=XXX">なまえ</a></dt>
			if (util::LineHasStringsNoCase( line, L"<dt", L"show_friend.pl" )) {

				ParserUtil::GetAuthor( line, &cmtData );
			}

			// <dd> 発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"<dd>" )) {
				i++;

				// 改行追加
				cmtData.AddBody(_T("\r\n"));
				break;
			}
		}

		// </dd> まで解析＆追加
		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);

			// </dd> 発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"</dd>" )) {
				i++;
				break;
			}

			// 動画用scriptタグが見つかったらscriptタグ終了まで解析。
			if(! ParserUtil::ExtractVideoLinkFromScriptTag( cmtData, i, html_ ) ) {
				// scriptタグ未発見なので、解析＆投入

				// 非表示のタグを削除
				if (isBbsHiddenLine(line)) {
					// この行を強制的にスキップ
					continue;
				}

				ParserUtil::AddBodyWithExtract( cmtData, line );
			}
		}
		retIndex = i;

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}
};


/**
 * [content] view_enquete.pl 用パーサ
 * 【アンケート詳細】
 * http://mixi.jp/view_enquete.pl
 */
class ViewEnqueteParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();

		// 名前を初期化
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		bool bInEnquete = false;
		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ●タイトル
			//<dt class="bbsTitle clearfix"><span class="titleSpan">XXXXXXXX</span>
			if( util::LineHasStringsNoCase( line, L"<dt", L"bbsTitle" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// タグの除去
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}
			
			// ●日時
			//<span class="date">2007年10月06日 17:20</span>	</dt>
			if( util::LineHasStringsNoCase( line, L"<span", L"date" ) )
			{
				ParserUtil::ParseDate(line, data_);
				continue;
			}

			// ●企画者解析
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				ParserUtil::GetAuthor( line, &data_ );
				continue;
			}

			// ●質問内容解析
			if( util::LineHasStringsNoCase( line, L"<dd>" ) )	// (C4819回避のダミーコメントです)
			{
				//内容解析関数へ、<dd class="enqueteBlock">が見つかったら戻る
				if( !parseBody( data_, html_, iLine ) )
					return false;
				bInEnquete = true;
				continue;
			}

			// ●集計結果解析。
			// <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>集計結果</font></td>
			if( util::LineHasStringsNoCase( line, L"<h3>", L"集計結果" ) )
			{
				if( !parseEnqueteResult( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ●アンケート内容終了
			if( bInEnquete && 
				(util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				 util::LineHasStringsNoCase( line, L"<div", L"id", L"enqueteComment") ||
				 // 送信ボタン
				 //<li><input type="submit" value="送信する" class="formBt01" /></li>
				 util::LineHasStringsNoCase( line, L"<input", L"type=", L"submit", L"送信する" )
				 ) )
			{
				bInEnquete = false;
				break;
			}

		}

		// コメント解析
		while( iLine<lastLine ) {
			iLine = parseEnqueteComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// コミュニティ名抽出
		parseCommunityName( data_, html_ );

		// ページ移動リンクの抽出
		parsePageLink( data_, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() finished." );
		return true;
	}

private:

	/**
	 * 設問内容を取得する。
	 *
	 * iLine は "設問内容" が存在する行。
	 *
	 * (1) "<dd class=\"enqueteBlock\">" が現れるまで無視。その直後から、設問内容本文。解析して、AddBody する。
	 * (2) "</dd>" が現れたら終了。
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd class=\"enqueteBlock\"> があれば、その後ろを本文とする。
			CString target;
			if( util::GetAfterSubString( line, L"<dd class=\"enqueteBlock\">", target ) >= 0 ) {
				// <dd class=\"enqueteBlock\">発見。
			}else{
				// <dd class=\"enqueteBlock\">未発見。
				target = line;
			}

			// </dd>があれば、その前を追加し、終了。
			// なければ、その行を追加し、次の行へ。
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> が見つからなかった。
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>が見つかったので終了。
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}

			// reply_enquete.pl 用：</ul> があれば終了。
			if( target.Find(L"</ul>") != -1 ) {
				break;
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * 集計結果を取得する。
	 *
	 * iLine は "集計結果" が存在する行。
	 *
	 * ● "</table>" が現れたら終了。
	 *
	 * ● 下記の形式で、ある選択肢に対する要素がある。
	 * <!-- oneMeter -->
	 * <dl class="enqueteList">
	 * <dt>回答選択肢<br />
	 * <img src="http://img.mixi.jp/img/bar.gif" width="28" height="16" alt="" />
	 * </dt>
	 * <dd><span>9</span>(3%)</dd>
	 * </dl>
	 * <!-- oneMeter -->
	 * 解析、整形して、AddBody する。
	 *
	 * ● 下記の形式で、合計があるので、解析、整形して、AddBodyする。
	 * <dl class="enqueteTotal"><dt class="enqueteTotalNumber">合計<span>244</span></dt></dl>
	 */
	static bool parseEnqueteResult( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"◆集計結果\r\n" );

		const int lastLine = html.GetCount();
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd class="formButtons01">があれば、解析終了。
			if( util::LineHasStringsNoCase( line, L"<dd class=\"formButtons" ) ) {
				break;
			}

			// 選択肢、投票数、百分率を解析して、AddBody する。
			if( util::LineHasStringsNoCase( line, L"<dl class=\"enqueteList\">" ) ) {
				// 境界値チェック
				if( iLine+5 >= lastLine ) {
					break;
				}
				// 次の行に本文がある。
				const CString& line1 = html.GetAt(++iLine);
				CString item;
				util::GetBetweenSubString( line1, L"<dt>", L"<br />", item );
				
				// +3 行目に下記の形式で、投票数、百分率がある。
				// <dd><span>125</span>(51%)</dd>
				iLine += 3;
				const CString& line2 = html.GetAt( iLine );
				CString target;
				util::GetBetweenSubString( line2, L"<span>", L"</span>", target );

				CString target2;
				util::GetBetweenSubString( line2, L"</span>", L"</dd>", target2 );
				
				// xx (yy%)
				CString num_rate;
				num_rate = target + target2;

				CString str;
				str.Format( L"  ●%s\r\n", item );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", num_rate );
				mixi.AddBody( str );
			}

			// 合計を解析する。
			if( util::LineHasStringsNoCase( line, L"enqueteTotal" ) )
			{
				// 次の行に合計がある。
				const CString& line1 = html.GetAt( iLine );

				CString total;
				util::GetBetweenSubString( line1, L"<span>", L"</span>", total );

				CString str;
				str.Format( L"  ●合計\r\n" );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", total );
				mixi.AddBody( str );
			}
		}

		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * アンケートコメント取得
	 */
	static int parseEnqueteComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_enquete_comment.pl")) != -1) 
				{
					// コメント全体の終了タグ発見
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;

				} else {

					const CString& line = html_.GetAt(i);
					// コメント番号を取得
					//<label for="commentCheck01">37</label>
					if( util::LineHasStringsNoCase( line, L"\">", L"</label>")) {
						CString number;
						util::GetAfterSubString( line, L"<label", number );
						util::GetBetweenSubString( number, L">", L"</label", number );
						// &nbsp; を消す
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex(_wtoi(number));
					}
					
					// 日付を取得
					//<span class="date">2007年10月07日 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					// 名前を取得
					//<dt><a href="show_friend.pl?id=xxxxxx">なまえ</a></dt>
					if( util::LineHasStringsNoCase( line, L"<dt>", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//コメント開始フラグをON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// コメントコメント本文取得
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// アンケートのパターン
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// 終了タグがあった場合
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 5;
						break;
					}

					// それ以外の場合
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// 終了タグ発見
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, line );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}

};


/**
 * [content] view_event.pl 用パーサ
 * 【イベント詳細】
 * http://mixi.jp/view_event.pl
 */
class ViewEventParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();
		
		// 名前を初期化
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ●タイトル(pattern1)
			//<dt class="bbsTitle clearfix"><span class="titleSpan"><span class="title">xxxxx</span></span>
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</span>", title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}

			// ●日時
			//<dd>2007年09月27日(地域によっては遅れる場合あり)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>開催日時</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString date;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", date );
				//ParserUtil::ParseDate(date, cmtData);
				//data_.SetDate(date);
				CString buf = _T("開催日時 ") + date;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●場所
			//<dd>都道府県未定(全国の書店など)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>開催場所</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString area;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", area );
				CString buf = _T("開催場所 ") + area;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●企画者解析
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				const CString& line2 = html_.GetAt(iLine );
				CString Author;
				util::GetBetweenSubString( line2, L"<dt>", L"</dt>", Author );
				ParserUtil::GetAuthor( Author, &data_ );

				// ●内容解析
				//内容解析関数へ、</dd>が見つかったら戻る
				if( !parseBody( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ●募集期限
			//<dd>指定なし</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>募集期限</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString limit;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", limit );
				CString buf = _T("募集期限 ") + limit;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●参加者
			//<dd>NNN名</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>参加者</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString people;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", people );
				CString buf = _T("参加者 ") + people;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●イベント内容終了
			if( util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				util::LineHasStringsNoCase( line, L"<div", L"id", L"eventComment") ) {
				break;
			}

		}

		// コメント解析
		while( iLine<lastLine ) {
			iLine = parseEventComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// イベント参加メンバーへのリンク
		{
			CString url = util::FormatString(
							L"http://mixi.jp/list_event_member.pl?id=%s&comm_id=%s",
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"id"),
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"comm_id"));
			data_.m_linkPage.push_back( CMixiData::Link(url, L"★参加者一覧") );
		}

		// コミュニティ名抽出
		parseCommunityName( data_, html_ );

		// ページ移動リンクの抽出
		parsePageLink( data_, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() finished." );
		return true;
	}

private:

	/**
	 * 設問内容を取得する。
	 *
	 * iLine は "設問内容" が存在する行。
	 *
	 * (1) <dd>が現れるまで無視。その直後から、設問内容本文。解析して、AddBody する。
	 * (2) "</dd>" が現れたら終了。
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd>があれば、その後ろを本文とする。
			CString target;
			if( util::GetAfterSubString( line, L"<dd>", target ) >= 0 ) {
				// <dd>発見。
			}else{
				// <dd>未発見。
				target = line;
			}

			// </dd>があれば、その前を追加し、終了。
			// なければ、その行を追加し、次の行へ。
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> が見つからなかった。
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>が見つかったので終了。
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * イベントコメント取得
	 */
	static int parseEventComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_event_comment.pl")) != -1)
				{
					// コメント全体の終了タグ発見
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;
				
				} else {

					const CString& line = html_.GetAt(i);
					//コメント番号を取得
					// <dt class="commentDate clearfix"><span class="senderId">183
					// or
					// <dt class="commentDate clearfix"><span class="senderId"><input type="checkbox" name="comment_id" id="commentCheck290675880" value="290675880" /><label for="commentCheck290675880">44</label></span>
					if( util::LineHasStringsNoCase( line, L"commentDate", L"senderId")) {
						CString number;
						if (util::GetAfterSubString( line, L"<label", number ) >= 0) {
							// 管理コミュ版
							util::GetBetweenSubString( number, L">", L"</label", number );
						} else {
							util::GetAfterSubString( line, L"<span", number );
							util::GetAfterSubString( number, L">", number );
						}
						// &nbsp; を消す
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex( _wtoi(number) );
					}
					
					//日付を取得
					//<span class="date">2007年10月07日 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					//名前を取得
					//<dt><a href="show_friend.pl?id=xxxxxx">なまえ</a></dt>
					if( util::LineHasStringsNoCase( line, L"show_friend", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//コメント開始フラグをON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// コメントコメント本文取得
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// イベントのパターン
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// 終了タグがあった場合
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 1;
						break;
					}

					// それ以外の場合
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// 終了タグ発見
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, line );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}

};


}//namespace mixi

#endif	// BT_MZ3
