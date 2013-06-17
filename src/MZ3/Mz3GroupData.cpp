/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#include "stdafx.h"
#include "MZ3.h"
#include "Mz3GroupData.h"
#include "util_base.h"
#include "util_mz3.h"
#include "inifile.h"

/**
 * トップページ用の初期化
 *
 * グループリストとその配下のリストの生成。
 */
bool Mz3GroupData::initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType)
{
	this->groups.clear();
	this->services = initType.strSelectedServices;

	// サービスが空の場合は全選択とみなし全サービスを追加する
	if (this->services.empty()) {
		for (size_t idx=0; idx<theApp.m_luaServices.size(); idx++) {
			this->services += " ";
			this->services += theApp.m_luaServices[idx].name;
		}
	}

	// イベントハンドラの呼び出し
	util::CallMZ3ScriptHookFunctions("", "creating_default_group", this);

	return true;
}

/**
 * カテゴリURLの移行処理
 *
 * カテゴリ名も変更可能(例:"みんなのecho" ⇒ "みんなのボイス")
 */
inline CString migrateCategoryUrl(LPCTSTR default_category_url,
										CStringA category_url,
										ACCESS_TYPE category_type,
										std::string& category_name)
{

#ifdef BT_MZ3
	// 旧形式iniファイルの移行処理
/*
	if (category_type == ACCESS_LIST_FOOTSTEP && category_url == "show_log.pl") {
		// 旧あしあとURLはAPI用URLに置換(デフォルトURLを採用する)
		return default_category_url;
	}
*/
	/* 下記は「マイミク一覧」を "list_friend.pl" から API に移行したときの処理
	if (category_type == ACCESS_LIST_FRIEND && category_url == "list_friend.pl") {
		// 旧マイミク一覧URLはAPI用URLに置換(デフォルトURLを採用する)
		return default_category_url;
	}
	*/

	if (category_type == ACCESS_LIST_FRIEND) {
		// マイミク一覧URLはデフォルトURLが正しい("list_friend.pl" や API の URL は廃止とする)
		return default_category_url;
	}

	if (category_type == ACCESS_MIXI_RECENT_VOICE &&
		category_url.Find("recent_echo.pl")>=0)
	{
		// 移行処理：recent_echo.pl を recent_voice.pl に書き換える
		CString strCategoryUrl(category_url);
		strCategoryUrl.Replace(L"recent_echo.pl",
							   L"recent_voice.pl");
		category_name = "みんなのボイス";
		return strCategoryUrl;
	}
	
	if (category_type == ACCESS_MIXI_RECENT_VOICE &&
		category_url.Find("res_echo.pl")>=0 &&
		strstr(category_name.c_str(), "返信")!=NULL)
	{
		// 移行処理：res_voice.pl は 2010/04/14 のmixi仕様変更で消えたので項目としても削除
		return L"";
	}
#endif

	// MZ3 API : カテゴリURLのmigrate
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(""));	// URL
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(category_type));
	if (util::CallMZ3ScriptHookFunctions2("migrate_category_url", &rvals, util::MyLuaData(serializeKey), util::MyLuaData(category_url))) {
		if (rvals.size() == 1) {
			return CString(rvals[0].m_strText);
		}
	}

	return CString(category_url);
}


/**
 * group にカテゴリを追加する。
 *
 * category_url が NULL または空の場合は、カテゴリ種別に応じたデフォルトURLを指定する。
 * カテゴリ種別に応じたカラムタイプを指定する。
 */
bool Mz3GroupData::appendCategoryByIniData( 
	AccessTypeInfo& accessTypeInfo, 
	CGroupItem& group, std::string category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
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
#ifdef BT_MZ3
	switch( category_type ) {
	case ACCESS_LIST_FAVORITE_USER:
		// お気に入りユーザ
		if (category_url != NULL && strstr(category_url, "kind=community")!=NULL) {
			// 「お気に入りコミュ」として処理する
			category_type = ACCESS_LIST_FAVORITE_COMMUNITY;
		}
		break;
	}
#endif

	// URL が指定されていればその URL を用いる。
	// 但し、いくつかの古いURLについては移行処理を行う。
	CString url = default_category_url;
	if (category_url!=NULL && strlen(category_url) > 0) {

		// カテゴリURLの移行処理
		url = migrateCategoryUrl(default_category_url, category_url, category_type, category_name);
		if (url.IsEmpty()) {
			return true;
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
 * カテゴリ種別文字列 ←→ カテゴリ種別
 */
void Mz3GroupDataInifileHelper::InitMap(AccessTypeInfo& accessTypeInfo) {
	category_string2type.RemoveAll();
	category_string2type.InitHashTable( 20 );

	// AccessTypeInfo のシリアライズキーから各種ハッシュテーブルを構築する
	AccessTypeInfo::ACCESS_TYPE_TO_DATA_MAP::iterator it;
	for (it=accessTypeInfo.m_map.begin(); it!=accessTypeInfo.m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		AccessTypeInfo::Data& data = it->second;

		switch (data.infoType) {
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
		// →グループの種別は廃止。全て GROUP_GENERAL とする。
		ACCESS_TYPE group_type = ACCESS_GROUP_GENERAL;

		// "Url" の値を取得し、URLとする。
		std::string url = inifile.GetValue( "Url", section_name );

		// グループ作成
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url).c_str(), group_type );

		// "CategoryXX" の値を取得し、処理する。XX は [01,99] とする。
		for( int j=0; j<99; j++ ) {
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

		// Type 出力(常に GROUP_GENERAL とする)
		inifile.SetValue( "Type", accessTypeInfo.getSerializeKey(ACCESS_GROUP_GENERAL), (LPCSTR)strSectionName );

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
