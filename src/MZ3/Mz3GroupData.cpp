/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "Mz3GroupData.h"
#include "util_base.h"
#include "inifile.h"

/**
 * トップページ用の初期化
 *
 * グループリストとその配下のリストの生成。
 */
bool Mz3GroupData::initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType)
{
	this->groups.clear();

	static CGroupItem group;

	if (initType.bUseMixi) {
		// 日記グループ
		group.init( L"日記", L"list_diary.pl", ACCESS_GROUP_MYDIARY );
		{
			appendCategoryByIniData( accessTypeInfo, group, "最近の日記", ACCESS_LIST_MYDIARY );
			appendCategoryByIniData( accessTypeInfo, group, "最近のコメント", ACCESS_LIST_COMMENT );
			appendCategoryByIniData( accessTypeInfo, group, "マイミク最新日記", ACCESS_LIST_DIARY );
			appendCategoryByIniData( accessTypeInfo, group, "日記コメント記入履歴", ACCESS_LIST_NEW_COMMENT );
		}
		this->groups.push_back( group );

		// コミュニティグループ
		group.init( L"コミュニティ", L"", ACCESS_GROUP_COMMUNITY );
		{
			appendCategoryByIniData( accessTypeInfo, group, "最新書き込み一覧", ACCESS_LIST_NEW_BBS );
			appendCategoryByIniData( accessTypeInfo, group, "コミュコメント履歴", ACCESS_LIST_NEW_BBS_COMMENT );
			appendCategoryByIniData( accessTypeInfo, group, "コミュニティ一覧", ACCESS_LIST_COMMUNITY );
		}
		this->groups.push_back( group );

		// ニュースグループ
		group.init( L"ニュース", L"", ACCESS_GROUP_NEWS );
		{
			//--- カテゴリ群の追加

			struct NEWS_CATEGORY
			{
				LPCSTR title;
				LPCSTR url;
			};
			NEWS_CATEGORY news_list[] = {
				{"注目のピックアップ",    "http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn"	},
				{"国内",				  "http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1"	},
				{"政治",				  "http://news.mixi.jp/list_news_category.pl?id=2&type=bn&sort=1"	},
				{"経済",				  "http://news.mixi.jp/list_news_category.pl?id=3&type=bn&sort=1"	},
				{"地域",				  "http://news.mixi.jp/list_news_category.pl?id=4&type=bn&sort=1"	},
				{"海外",				  "http://news.mixi.jp/list_news_category.pl?id=5&type=bn&sort=1"	},
				{"スポーツ",			  "http://news.mixi.jp/list_news_category.pl?id=6&type=bn&sort=1"	},
				{"エンターテインメント",  "http://news.mixi.jp/list_news_category.pl?id=7&type=bn&sort=1"	},
				{"IT",					  "http://news.mixi.jp/list_news_category.pl?id=8&type=bn&sort=1"	},
				{"ゲーム・アニメ",		  "http://news.mixi.jp/list_news_category.pl?id=9&type=bn&sort=1"	},
				{"コラム",				  "http://news.mixi.jp/list_news_category.pl?id=10&type=bn&sort=1"	},
				{NULL, NULL}
			};

			for( int i=0; news_list[i].title != NULL; i++ ) {
				appendCategoryByIniData( accessTypeInfo, group, news_list[i].title, ACCESS_LIST_NEWS, news_list[i].url );
			}
		}
		this->groups.push_back( group );

		// メッセージグループ
		group.init( L"メッセージ", L"", ACCESS_GROUP_MESSAGE );
		{
			appendCategoryByIniData( accessTypeInfo, group, "メッセージ（受信箱）", ACCESS_LIST_MESSAGE_IN );
			appendCategoryByIniData( accessTypeInfo, group, "メッセージ（送信箱）", ACCESS_LIST_MESSAGE_OUT );
		}
		this->groups.push_back( group );

		// echoグループ
		group.init( L"エコー", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "みんなのエコー", ACCESS_MIXI_RECENT_ECHO );
			appendCategoryByIniData( accessTypeInfo, group, "自分への返信一覧", ACCESS_MIXI_RECENT_ECHO, "http://mixi.jp/res_echo.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "自分の一覧", ACCESS_MIXI_RECENT_ECHO, "http://mixi.jp/list_echo.pl?id={owner_id}" );
		}
		this->groups.push_back( group );

		// その他グループ
		group.init( L"その他", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "マイミク一覧", ACCESS_LIST_FRIEND );
			appendCategoryByIniData( accessTypeInfo, group, "紹介文", ACCESS_LIST_INTRO );
			appendCategoryByIniData( accessTypeInfo, group, "足あと", ACCESS_LIST_FOOTSTEP );
			appendCategoryByIniData( accessTypeInfo, group, "カレンダー", ACCESS_LIST_CALENDAR, "show_calendar.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "ブックマーク", ACCESS_LIST_BOOKMARK );
			appendCategoryByIniData( accessTypeInfo, group, "お気に入りユーザー", ACCESS_LIST_FAVORITE_USER, "list_bookmark.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "お気に入りコミュ", ACCESS_LIST_FAVORITE_COMMUNITY, "list_bookmark.pl?kind=community" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseTwitter) {
		// Twitterグループ
		group.init( L"Twitter", L"", ACCESS_GROUP_TWITTER );
		{
			appendCategoryByIniData( accessTypeInfo, group, "タイムライン", ACCESS_TWITTER_FRIENDS_TIMELINE, "http://twitter.com/statuses/friends_timeline.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "返信一覧", ACCESS_TWITTER_FRIENDS_TIMELINE, "http://twitter.com/statuses/replies.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "お気に入り", ACCESS_TWITTER_FAVORITES, "http://twitter.com/favorites.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "受信メッセージ", ACCESS_TWITTER_DIRECT_MESSAGES, "http://twitter.com/direct_messages.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "送信メッセージ", ACCESS_TWITTER_DIRECT_MESSAGES, "http://twitter.com/direct_messages/sent.xml" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseWassr) {
		// Wassrグループ
		group.init( L"Wassr", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "タイムライン", ACCESS_WASSR_FRIENDS_TIMELINE );
			appendCategoryByIniData( accessTypeInfo, group, "返信一覧", ACCESS_WASSR_FRIENDS_TIMELINE, "http://api.wassr.jp/statuses/replies.xml" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseGoohome) {
		// gooホームグループ
		group.init( L"gooホーム", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "友達・注目の人", ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS );
			appendCategoryByIniData( accessTypeInfo, group, "自分のひとこと一覧", ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS, "http://home.goo.ne.jp/api/quote/quotes/myself/json" );
		}
		this->groups.push_back( group );
	}

//	if (initType.bUseRss) {
	{
		// RSSグループ
		group.init( L"RSS", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, 
				"はてブ 最近の人気エントリー", ACCESS_RSS_READER_FEED, "http://b.hatena.ne.jp/hotentry?mode=rss");
//			appendCategoryByIniData( accessTypeInfo, group, 
//				"しょこたん☆ぶろぐ", ACCESS_RSS_READER_FEED, "http://blog.excite.co.jp/shokotan/index.xml");
			appendCategoryByIniData( accessTypeInfo, group, 
				"CNET Japan ", ACCESS_RSS_READER_FEED, "http://japan.cnet.com/rss/index.rdf");
		}
		this->groups.push_back( group );
	}

	return true;
}

/**
 * group にカテゴリを追加する。
 *
 * category_url が NULL または空の場合は、カテゴリ種別に応じたデフォルトURLを指定する。
 * カテゴリ種別に応じたカラムタイプを指定する。
 */
bool Mz3GroupData::appendCategoryByIniData( 
	AccessTypeInfo& accessTypeInfo, 
	CGroupItem& group, const std::string& category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
{
	// デフォルトURL（category_url が未指定の場合に用いるURL）
	LPCTSTR default_category_url = accessTypeInfo.getDefaultCategoryURL(category_type);

	AccessTypeInfo::BODY_INDICATE_TYPE colType1 = accessTypeInfo.getBodyHeaderCol1Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType2 = accessTypeInfo.getBodyHeaderCol2Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType3 = accessTypeInfo.getBodyHeaderCol3Type(category_type);

	if (colType1==AccessTypeInfo::BODY_INDICATE_TYPE_NONE || 
		colType2==AccessTypeInfo::BODY_INDICATE_TYPE_NONE) 
	{
		// サポート外のため追加せず終了
//		MZ3LOGGER_ERROR(L"サポート外のため追加しません。");
		return false;
	}

	// アクセス種別分離に伴う移行処理
	switch( category_type ) {
	case ACCESS_LIST_FAVORITE_USER:
		// お気に入りユーザ
		if (category_url != NULL && strstr(category_url, "kind=community")!=NULL) {
			// 「お気に入りコミュ」として処理する
			category_type = ACCESS_LIST_FAVORITE_COMMUNITY;
		}
		break;
	}

	// URL が指定されていればその URL を用いる。
	CString url = default_category_url;
	if( category_url!=NULL && strlen(category_url) > 0 ) {
		// 旧形式iniファイルの移行処理
		if (category_type == ACCESS_LIST_FOOTSTEP &&
			strcmp(category_url, "show_log.pl")==0)
		{
			// 旧あしあとURLはAPI用URLに置換
		} else if (category_type == ACCESS_LIST_FRIEND &&
			strcmp(category_url, "list_friend.pl")==0)
		{
			// 旧マイミク一覧URLはAPI用URLに置換
		} else {	
			url = util::my_mbstowcs(category_url).c_str();
		}
	}

	static CCategoryItem item;
	item.init( 
		util::my_mbstowcs(category_name).c_str(), 
		url, 
		category_type, 
		group.categories.size(), 
		colType1, colType2, colType3 );
	item.m_bCruise = bCruise;
	group.categories.push_back( item );

	return true;
}

/**
 * 下記のマップを生成する。
 *
 * グループ種別文字列 ←→ グループ種別
 * カテゴリ種別文字列 ←→ カテゴリ種別
 */
void Mz3GroupDataInifileHelper::InitMap(AccessTypeInfo& accessTypeInfo) {
	group_string2type.RemoveAll();
	group_string2type.InitHashTable( 10 );
	category_string2type.RemoveAll();
	category_string2type.InitHashTable( 20 );

	// AccessTypeInfo のシリアライズキーから各種ハッシュテーブルを構築する
	AccessTypeInfo::MYMAP::iterator it;
	for (it=accessTypeInfo.m_map.begin(); it!=accessTypeInfo.m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		AccessTypeInfo::Data& data = it->second;

		switch (data.infoType) {
		case AccessTypeInfo::INFO_TYPE_GROUP:
			// group_string2type の構築
			if (!data.serializeKey.empty()) {
				group_string2type[ util::my_mbstowcs(data.serializeKey.c_str()).c_str() ] = accessType;
			}
			break;

		case AccessTypeInfo::INFO_TYPE_CATEGORY:
			// category_string2type の構築
			if (!data.serializeKey.empty()) {
				category_string2type[ util::my_mbstowcs(data.serializeKey.c_str()).c_str() ] = accessType;
			}
			break;

		default:
			// nothing to do.
			break;
		}
	}
}

/**
 * ini ファイル（グループ定義ファイル）から Mz3GroupData を生成する。
 */
bool Mz3GroupDataReader::load( AccessTypeInfo& accessTypeInfo, Mz3GroupData& target, const CString& inifilename )
{
	target.groups.clear();

	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper(accessTypeInfo);

	// グループ定義ファイルのロード
	if(! inifile.Load( inifilename ) ) {
		return false;
	}

	// ソート済みセクションリストを取得する
	std::vector<std::string> group_section_names;
	{
		// セクション一覧を取得する。
		group_section_names = inifile.GetSectionNames();

		// セクション一覧のうち「Group」で始まるものを抽出する。
		for( size_t i=0; i<group_section_names.size(); ) {
			if( strncmp( group_section_names[i].c_str(), "Group", 5 ) != 0 ) {
				// 「Group」で始まらないので除去する。
				group_section_names.erase( group_section_names.begin()+i );
			}else{
				i++;
			}
		}

		// ソート
		sort( group_section_names.begin(), group_section_names.end() );
	}

	// 各セクションに対して追加処理を実行
	for( size_t i=0; i<group_section_names.size(); i++ ) {
		const char* section_name = group_section_names[i].c_str();

		// "Name" の値を取得し、グループ名とする。
		std::string group_name = inifile.GetValue( "Name", section_name );
		if( group_name.empty() ) {
			continue;
		}

		// "Type" の値を取得し、グループ種別とする。
		// グループ種別名→グループ種別変換を行う。
		std::wstring type_value = util::my_mbstowcs(inifile.GetValue( "Type", section_name )).c_str();
		ACCESS_TYPE group_type = helper.GroupString2Type( type_value.c_str() );
		if( group_type == ACCESS_INVALID ) {
			continue;
		}

		// "Url" の値を取得し、URLとする。
		std::string url = inifile.GetValue( "Url", section_name );

		// グループ作成
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url).c_str(), group_type );

		// "CategoryXX" の値を取得し、処理する。XX は [01,20] とする。
		for( int j=0; j<20; j++ ) {
			CStringA key;
			key.Format( "Category%02d", j+1 );
			
			// "CategoryXX" の値が存在しなければ終了する。
			if(! inifile.RecordExists( (const char*)key, section_name ) ) {
				break;
			}

			// "CategoryXX" の値を取得する。
			std::string value = inifile.GetValue( (const char*)key, section_name );

			// カンマ区切りで文字列リスト化する。
			std::vector<std::string> values;
			util::split_by_comma( values, value );
			if( values.size() < 2 ) {
				continue;
			}

			// 文字列リストの第1要素をカテゴリ名とする。
			std::string& category_name = values[0];

			// 文字列リストの第2要素をカテゴリ種別とする。
			// カテゴリ種別名→カテゴリ種別変換を行う。
			ACCESS_TYPE category_type = helper.CategoryString2Type( util::my_mbstowcs(values[1]).c_str() );
			if( category_type == ACCESS_INVALID ) {
				continue;
			}

			// 文字列リストの第3要素をURLとする。
			// 存在しない場合はカテゴリ種別のデフォルト値を用いる。
			const char* category_url = NULL;
			if( values.size() >= 3 ) {
				category_url = values[2].c_str();
			}

			// 文字列リストの第4要素を巡回フラグとする。
			// 存在しない場合は巡回なしとする。
			bool bCruise = false;
			if( values.size() >= 4 ) {
				bCruise = atoi(values[3].c_str()) != 0 ? true : false;
			}

			// カテゴリ追加。
			target.appendCategoryByIniData( accessTypeInfo, group, category_name, category_type, category_url, bCruise );
		}

		// カテゴリが１つ以上なら、groups に追加。
		if( group.categories.size() >= 1 ) {
			target.groups.push_back( group );
		}
	}

	// グループ要素がゼロなら終了。
	if( target.groups.empty() ) {
		return false;
	}

	return true;
}

/**
 * Mz3GroupData オブジェクトを ini ファイル（グループ定義ファイル）化する。
 */
bool Mz3GroupDataWriter::save( AccessTypeInfo& accessTypeInfo, const Mz3GroupData& target, const CString& inifilename )
{
	inifile::IniFile inifile;

	const int n = target.groups.size();
	for( int i=0; i<n; i++ ) {
		const CGroupItem& group = target.groups[i];

		if (!group.bSaveToGroupFile) {
			continue;
		}

		// セクション名生成
		CStringA strSectionName;
		strSectionName.Format( "Group%02d", i+1 );

		// Name 出力
		inifile.SetValue( L"Name", group.name, strSectionName );

		// Type 出力
		inifile.SetValue( "Type", accessTypeInfo.getSerializeKey(group.mixi.GetAccessType()), (LPCSTR)strSectionName );

		// Url 出力
		inifile.SetValue( L"Url", group.mixi.GetURL(), strSectionName );

		// CategoryXX 出力
		int nc = group.categories.size();
		int categoryNumber = 1;
		for( int j=0; j<nc; j++ ) {
			// キー生成
			CString key;
			key.Format( L"Category%02d", categoryNumber );

			// 右辺値生成
			// 右辺値は順に、「カテゴリ名称」、「カテゴリ種別文字列」、「URL」
			const CCategoryItem& item = group.categories[j];
			CString categoryString = util::my_mbstowcs(accessTypeInfo.getSerializeKey(item.m_mixi.GetAccessType())).c_str();
			if (categoryString.IsEmpty() || item.bSaveToGroupFile==false) 
				continue;

			CString value;
			value.Format( L"%s,%s,%s,%d", 
				item.m_name, 
				categoryString,
				item.m_mixi.GetURL(),
				item.m_bCruise ? 1 : 0
				);

			// 出力
			inifile.SetValue( key, value, strSectionName );
			categoryNumber ++;
		}
	}

	// 保存
	return inifile.Save( inifilename, false );
}
