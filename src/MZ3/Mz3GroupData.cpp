#include "stdafx.h"
#include "Mz3.h"
#include "Mz3GroupData.h"
#include "util.h"

/**
 * トップページ用の初期化
 *
 * グループリストとその配下のリストの生成。
 */
bool Mz3GroupData::initForTopPage()
{
	this->groups.clear();

	static CGroupItem group;

	// 日記グループ
	group.init( L"日記", L"list_diary.pl", ACCESS_GROUP_MYDIARY );
	{
		appendCategoryByIniData( group, "最近の日記", ACCESS_LIST_MYDIARY );
		appendCategoryByIniData( group, "最近のコメント", ACCESS_LIST_COMMENT );
		appendCategoryByIniData( group, "マイミク最新日記", ACCESS_LIST_DIARY );
		appendCategoryByIniData( group, "日記コメント記入履歴", ACCESS_LIST_NEW_COMMENT );
	}
	this->groups.push_back( group );

	// コミュニティグループ
	group.init( L"コミュニティ", L"", ACCESS_GROUP_COMMUNITY );
	{
		appendCategoryByIniData( group, "最新書き込み一覧", ACCESS_LIST_NEW_BBS );
		appendCategoryByIniData( group, "コミュニティ一覧", ACCESS_LIST_COMMUNITY );
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
			{NULL, NULL}
		};

		for( int i=0; news_list[i].title != NULL; i++ ) {
			appendCategoryByIniData( group, news_list[i].title, ACCESS_LIST_NEWS, news_list[i].url );
		}
	}
	this->groups.push_back( group );

	// メッセージグループ
	group.init( L"メッセージ", L"", ACCESS_GROUP_MESSAGE );
	{
		appendCategoryByIniData( group, "メッセージ（受信箱）", ACCESS_LIST_MESSAGE_IN );
		appendCategoryByIniData( group, "メッセージ（送信箱）", ACCESS_LIST_MESSAGE_OUT );
	}
	this->groups.push_back( group );

	// その他グループ
	group.init( L"その他", L"", ACCESS_GROUP_OTHERS );
	{
		appendCategoryByIniData( group, "マイミク一覧", ACCESS_LIST_FRIEND );
		appendCategoryByIniData( group, "紹介文", ACCESS_LIST_INTRO );
		appendCategoryByIniData( group, "足あと", ACCESS_LIST_FOOTSTEP );
		appendCategoryByIniData( group, "ブックマーク", ACCESS_LIST_BOOKMARK );
		appendCategoryByIniData( group, "お気に入りユーザー", ACCESS_LIST_FAVORITE, "list_bookmark.pl" );
		appendCategoryByIniData( group, "お気に入りコミュ", ACCESS_LIST_FAVORITE, "list_bookmark.pl?kind=community" );
	}
	this->groups.push_back( group );

	return true;
}

/**
 * group にカテゴリを追加する。
 *
 * category_url が NULL または空の場合は、カテゴリ種別に応じたデフォルトURLを指定する。
 * カテゴリ種別に応じたカラムタイプを指定する。
 */
bool Mz3GroupData::appendCategoryByIniData( 
	CGroupItem& group, const std::string& category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
{
	// デフォルトURL（category_url が未指定の場合に用いるURL）
	LPCTSTR default_category_url = L"";

	CCategoryItem::BODY_INDICATE_TYPE firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
	CCategoryItem::BODY_INDICATE_TYPE secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;

	switch( category_type ) {

	// 日記系
	case ACCESS_LIST_MYDIARY:
		// 最近の日記
		default_category_url = L"list_diary.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_COMMENT:
		// 最近のコメント
		default_category_url = L"list_comment.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_DIARY:
		// マイミク最新日記
		default_category_url = L"new_friend_diary.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_NEW_COMMENT:
		// 日記コメント記入履歴
		default_category_url = L"new_comment.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	// コミュニティ系
	case ACCESS_LIST_NEW_BBS:
		// 最新書き込み一覧
		default_category_url = L"new_bbs.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_COMMUNITY:
		// コミュニティ一覧
		default_category_url = L"list_community.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NONE;
		break;

	// ニュース系
	case ACCESS_LIST_NEWS:
		// ニュース
		default_category_url = L"";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	// メッセージ系
	case ACCESS_LIST_MESSAGE_IN:
		// メッセージ（受信箱）
		default_category_url = L"list_message.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case  ACCESS_LIST_MESSAGE_OUT:
		// メッセージ（送信箱）
		default_category_url = L"list_message.pl?box=outbox";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	// その他
	case ACCESS_LIST_FRIEND:
		// マイミク一覧
		default_category_url = L"list_friend.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_INTRO:
		// 紹介文
		default_category_url = L"show_intro.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_BODY;
		break;

	case ACCESS_LIST_FOOTSTEP:
		// 足あと
		default_category_url = L"show_log.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_FAVORITE:
		// お気に入り
		default_category_url = L"list_bookmark.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_BOOKMARK:
		// ブックマーク
		default_category_url = L"";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	default:
		// サポート外カテゴリタイプなので追加せず終了。
		return false;
	}

	// 追加。

	// URL が指定されていればその URL を用いる。
	CString url = default_category_url;
	if( category_url!=NULL && strlen(category_url) > 0 ) {
		url = util::my_mbstowcs(category_url).c_str();
	}
	static CCategoryItem item;
	item.init( 
		util::my_mbstowcs(category_name).c_str(), 
		url, 
		category_type, 
		group.categories.size(), 
		firstColType, secondColType );
	item.m_bCruise = bCruise;
	group.categories.push_back( item );

	return true;
}

/**
 * ini ファイル（グループ定義ファイル）から Mz3GroupData を生成する。
 */
bool Mz3GroupDataReader::load( Mz3GroupData& target, const CString& inifilename )
{
	target.groups.clear();

	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper;

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
		std::wstring type_value = util::my_mbstowcs( inifile.GetValue( "Type", section_name ) );
		ACCESS_TYPE group_type = helper.GroupString2Type( type_value.c_str() );
		if( group_type == ACCESS_INVALID ) {
			continue;
		}

		// "Url" の値を取得し、URLとする。
		std::string url = inifile.GetValue( "Url", section_name );

		// グループ作成
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url.c_str()).c_str(), group_type );

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
			target.appendCategoryByIniData( group, category_name, category_type, category_url, bCruise );
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
bool Mz3GroupDataWriter::save( const Mz3GroupData& target, const CString& inifilename )
{
	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper;

	const int n = target.groups.size();
	for( int i=0; i<n; i++ ) {
		const CGroupItem& group = target.groups[i];

		// セクション名生成
		CStringA strSectionName;
		strSectionName.Format( "Group%d", i+1 );

		// Name 出力
		inifile.SetValue( L"Name", group.name, strSectionName );

//		MessageBox( NULL, group.name, L"", MB_OK );

		// Type 出力
		inifile.SetValue( L"Type", helper.GroupType2String(group.mixi.GetAccessType()), strSectionName );

		// Url 出力
		inifile.SetValue( L"Url", group.mixi.GetURL(), strSectionName );

		// CategoryXX 出力
		int nc = group.categories.size();
		for( int j=0; j<nc; j++ ) {
			// キー生成
			CString key;
			key.Format( L"Category%02d", j+1 );

			// 右辺値生成
			// 右辺値は順に、「カテゴリ名称」、「カテゴリ種別文字列」、「URL」
			const CCategoryItem& item = group.categories[j];
			LPCTSTR categoryString = helper.CategoryType2String(item.m_mixi.GetAccessType());
			if( categoryString == NULL ) 
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
		}
	}

	// 保存
	return inifile.Save( inifilename, false );
}
