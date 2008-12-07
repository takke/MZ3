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
 * �g�b�v�y�[�W�p�̏�����
 *
 * �O���[�v���X�g�Ƃ��̔z���̃��X�g�̐����B
 */
bool Mz3GroupData::initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType)
{
	this->groups.clear();

	static CGroupItem group;

	if (initType.bUseMixi) {
		// ���L�O���[�v
		group.init( L"���L", L"list_diary.pl", ACCESS_GROUP_MYDIARY );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�ŋ߂̓��L", ACCESS_LIST_MYDIARY );
			appendCategoryByIniData( accessTypeInfo, group, "�ŋ߂̃R�����g", ACCESS_LIST_COMMENT );
			appendCategoryByIniData( accessTypeInfo, group, "�}�C�~�N�ŐV���L", ACCESS_LIST_DIARY );
			appendCategoryByIniData( accessTypeInfo, group, "���L�R�����g�L������", ACCESS_LIST_NEW_COMMENT );
		}
		this->groups.push_back( group );

		// �R�~���j�e�B�O���[�v
		group.init( L"�R�~���j�e�B", L"", ACCESS_GROUP_COMMUNITY );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�ŐV�������݈ꗗ", ACCESS_LIST_NEW_BBS );
			appendCategoryByIniData( accessTypeInfo, group, "�R�~���R�����g����", ACCESS_LIST_NEW_BBS_COMMENT );
			appendCategoryByIniData( accessTypeInfo, group, "�R�~���j�e�B�ꗗ", ACCESS_LIST_COMMUNITY );
		}
		this->groups.push_back( group );

		// �j���[�X�O���[�v
		group.init( L"�j���[�X", L"", ACCESS_GROUP_NEWS );
		{
			//--- �J�e�S���Q�̒ǉ�

			struct NEWS_CATEGORY
			{
				LPCSTR title;
				LPCSTR url;
			};
			NEWS_CATEGORY news_list[] = {
				{"���ڂ̃s�b�N�A�b�v",    "http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn"	},
				{"����",				  "http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1"	},
				{"����",				  "http://news.mixi.jp/list_news_category.pl?id=2&type=bn&sort=1"	},
				{"�o��",				  "http://news.mixi.jp/list_news_category.pl?id=3&type=bn&sort=1"	},
				{"�n��",				  "http://news.mixi.jp/list_news_category.pl?id=4&type=bn&sort=1"	},
				{"�C�O",				  "http://news.mixi.jp/list_news_category.pl?id=5&type=bn&sort=1"	},
				{"�X�|�[�c",			  "http://news.mixi.jp/list_news_category.pl?id=6&type=bn&sort=1"	},
				{"�G���^�[�e�C�������g",  "http://news.mixi.jp/list_news_category.pl?id=7&type=bn&sort=1"	},
				{"IT",					  "http://news.mixi.jp/list_news_category.pl?id=8&type=bn&sort=1"	},
				{"�Q�[���E�A�j��",		  "http://news.mixi.jp/list_news_category.pl?id=9&type=bn&sort=1"	},
				{"�R����",				  "http://news.mixi.jp/list_news_category.pl?id=10&type=bn&sort=1"	},
				{NULL, NULL}
			};

			for( int i=0; news_list[i].title != NULL; i++ ) {
				appendCategoryByIniData( accessTypeInfo, group, news_list[i].title, ACCESS_LIST_NEWS, news_list[i].url );
			}
		}
		this->groups.push_back( group );

		// ���b�Z�[�W�O���[�v
		group.init( L"���b�Z�[�W", L"", ACCESS_GROUP_MESSAGE );
		{
			appendCategoryByIniData( accessTypeInfo, group, "���b�Z�[�W�i��M���j", ACCESS_LIST_MESSAGE_IN );
			appendCategoryByIniData( accessTypeInfo, group, "���b�Z�[�W�i���M���j", ACCESS_LIST_MESSAGE_OUT );
		}
		this->groups.push_back( group );

		// echo�O���[�v
		group.init( L"�G�R�[", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�݂�Ȃ̃G�R�[", ACCESS_MIXI_RECENT_ECHO );
			appendCategoryByIniData( accessTypeInfo, group, "�����ւ̕ԐM�ꗗ", ACCESS_MIXI_RECENT_ECHO, "http://mixi.jp/res_echo.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "�����̈ꗗ", ACCESS_MIXI_RECENT_ECHO, "http://mixi.jp/list_echo.pl?id={owner_id}" );
		}
		this->groups.push_back( group );

		// ���̑��O���[�v
		group.init( L"���̑�", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�}�C�~�N�ꗗ", ACCESS_LIST_FRIEND );
			appendCategoryByIniData( accessTypeInfo, group, "�Љ", ACCESS_LIST_INTRO );
			appendCategoryByIniData( accessTypeInfo, group, "������", ACCESS_LIST_FOOTSTEP );
			appendCategoryByIniData( accessTypeInfo, group, "�J�����_�[", ACCESS_LIST_CALENDAR, "show_calendar.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "�u�b�N�}�[�N", ACCESS_LIST_BOOKMARK );
			appendCategoryByIniData( accessTypeInfo, group, "���C�ɓ��胆�[�U�[", ACCESS_LIST_FAVORITE_USER, "list_bookmark.pl" );
			appendCategoryByIniData( accessTypeInfo, group, "���C�ɓ���R�~��", ACCESS_LIST_FAVORITE_COMMUNITY, "list_bookmark.pl?kind=community" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseTwitter) {
		// Twitter�O���[�v
		group.init( L"Twitter", L"", ACCESS_GROUP_TWITTER );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�^�C�����C��", ACCESS_TWITTER_FRIENDS_TIMELINE, "http://twitter.com/statuses/friends_timeline.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "�ԐM�ꗗ", ACCESS_TWITTER_FRIENDS_TIMELINE, "http://twitter.com/statuses/replies.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "���C�ɓ���", ACCESS_TWITTER_FAVORITES, "http://twitter.com/favorites.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "��M���b�Z�[�W", ACCESS_TWITTER_DIRECT_MESSAGES, "http://twitter.com/direct_messages.xml" );
			appendCategoryByIniData( accessTypeInfo, group, "���M���b�Z�[�W", ACCESS_TWITTER_DIRECT_MESSAGES, "http://twitter.com/direct_messages/sent.xml" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseWassr) {
		// Wassr�O���[�v
		group.init( L"Wassr", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�^�C�����C��", ACCESS_WASSR_FRIENDS_TIMELINE );
			appendCategoryByIniData( accessTypeInfo, group, "�ԐM�ꗗ", ACCESS_WASSR_FRIENDS_TIMELINE, "http://api.wassr.jp/statuses/replies.xml" );
		}
		this->groups.push_back( group );
	}

	if (initType.bUseGoohome) {
		// goo�z�[���O���[�v
		group.init( L"goo�z�[��", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, "�F�B�E���ڂ̐l", ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS );
			appendCategoryByIniData( accessTypeInfo, group, "�����̂ЂƂ��ƈꗗ", ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS, "http://home.goo.ne.jp/api/quote/quotes/myself/json" );
		}
		this->groups.push_back( group );
	}

//	if (initType.bUseRss) {
	{
		// RSS�O���[�v
		group.init( L"RSS", L"", ACCESS_GROUP_OTHERS );
		{
			appendCategoryByIniData( accessTypeInfo, group, 
				"�͂ău �ŋ߂̐l�C�G���g���[", ACCESS_RSS_READER_FEED, "http://b.hatena.ne.jp/hotentry?mode=rss");
//			appendCategoryByIniData( accessTypeInfo, group, 
//				"���傱���񁙂Ԃ낮", ACCESS_RSS_READER_FEED, "http://blog.excite.co.jp/shokotan/index.xml");
			appendCategoryByIniData( accessTypeInfo, group, 
				"CNET Japan ", ACCESS_RSS_READER_FEED, "http://japan.cnet.com/rss/index.rdf");
		}
		this->groups.push_back( group );
	}

	return true;
}

/**
 * group �ɃJ�e�S����ǉ�����B
 *
 * category_url �� NULL �܂��͋�̏ꍇ�́A�J�e�S����ʂɉ������f�t�H���gURL���w�肷��B
 * �J�e�S����ʂɉ������J�����^�C�v���w�肷��B
 */
bool Mz3GroupData::appendCategoryByIniData( 
	AccessTypeInfo& accessTypeInfo, 
	CGroupItem& group, const std::string& category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
{
	// �f�t�H���gURL�icategory_url �����w��̏ꍇ�ɗp����URL�j
	LPCTSTR default_category_url = accessTypeInfo.getDefaultCategoryURL(category_type);

	AccessTypeInfo::BODY_INDICATE_TYPE colType1 = accessTypeInfo.getBodyHeaderCol1Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType2 = accessTypeInfo.getBodyHeaderCol2Type(category_type);
	AccessTypeInfo::BODY_INDICATE_TYPE colType3 = accessTypeInfo.getBodyHeaderCol3Type(category_type);

	if (colType1==AccessTypeInfo::BODY_INDICATE_TYPE_NONE || 
		colType2==AccessTypeInfo::BODY_INDICATE_TYPE_NONE) 
	{
		// �T�|�[�g�O�̂��ߒǉ������I��
//		MZ3LOGGER_ERROR(L"�T�|�[�g�O�̂��ߒǉ����܂���B");
		return false;
	}

	// �A�N�Z�X��ʕ����ɔ����ڍs����
	switch( category_type ) {
	case ACCESS_LIST_FAVORITE_USER:
		// ���C�ɓ��胆�[�U
		if (category_url != NULL && strstr(category_url, "kind=community")!=NULL) {
			// �u���C�ɓ���R�~���v�Ƃ��ď�������
			category_type = ACCESS_LIST_FAVORITE_COMMUNITY;
		}
		break;
	}

	// URL ���w�肳��Ă���΂��� URL ��p����B
	CString url = default_category_url;
	if( category_url!=NULL && strlen(category_url) > 0 ) {
		// ���`��ini�t�@�C���̈ڍs����
		if (category_type == ACCESS_LIST_FOOTSTEP &&
			strcmp(category_url, "show_log.pl")==0)
		{
			// ����������URL��API�pURL�ɒu��
		} else if (category_type == ACCESS_LIST_FRIEND &&
			strcmp(category_url, "list_friend.pl")==0)
		{
			// ���}�C�~�N�ꗗURL��API�pURL�ɒu��
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
 * ���L�̃}�b�v�𐶐�����B
 *
 * �O���[�v��ʕ����� ���� �O���[�v���
 * �J�e�S����ʕ����� ���� �J�e�S�����
 */
void Mz3GroupDataInifileHelper::InitMap(AccessTypeInfo& accessTypeInfo) {
	group_string2type.RemoveAll();
	group_string2type.InitHashTable( 10 );
	category_string2type.RemoveAll();
	category_string2type.InitHashTable( 20 );

	// AccessTypeInfo �̃V���A���C�Y�L�[����e��n�b�V���e�[�u�����\�z����
	AccessTypeInfo::MYMAP::iterator it;
	for (it=accessTypeInfo.m_map.begin(); it!=accessTypeInfo.m_map.end(); it++) {
		ACCESS_TYPE accessType = it->first;
		AccessTypeInfo::Data& data = it->second;

		switch (data.infoType) {
		case AccessTypeInfo::INFO_TYPE_GROUP:
			// group_string2type �̍\�z
			if (!data.serializeKey.empty()) {
				group_string2type[ util::my_mbstowcs(data.serializeKey.c_str()).c_str() ] = accessType;
			}
			break;

		case AccessTypeInfo::INFO_TYPE_CATEGORY:
			// category_string2type �̍\�z
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
 * ini �t�@�C���i�O���[�v��`�t�@�C���j���� Mz3GroupData �𐶐�����B
 */
bool Mz3GroupDataReader::load( AccessTypeInfo& accessTypeInfo, Mz3GroupData& target, const CString& inifilename )
{
	target.groups.clear();

	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper(accessTypeInfo);

	// �O���[�v��`�t�@�C���̃��[�h
	if(! inifile.Load( inifilename ) ) {
		return false;
	}

	// �\�[�g�ς݃Z�N�V�������X�g���擾����
	std::vector<std::string> group_section_names;
	{
		// �Z�N�V�����ꗗ���擾����B
		group_section_names = inifile.GetSectionNames();

		// �Z�N�V�����ꗗ�̂����uGroup�v�Ŏn�܂���̂𒊏o����B
		for( size_t i=0; i<group_section_names.size(); ) {
			if( strncmp( group_section_names[i].c_str(), "Group", 5 ) != 0 ) {
				// �uGroup�v�Ŏn�܂�Ȃ��̂ŏ�������B
				group_section_names.erase( group_section_names.begin()+i );
			}else{
				i++;
			}
		}

		// �\�[�g
		sort( group_section_names.begin(), group_section_names.end() );
	}

	// �e�Z�N�V�����ɑ΂��Ēǉ����������s
	for( size_t i=0; i<group_section_names.size(); i++ ) {
		const char* section_name = group_section_names[i].c_str();

		// "Name" �̒l���擾���A�O���[�v���Ƃ���B
		std::string group_name = inifile.GetValue( "Name", section_name );
		if( group_name.empty() ) {
			continue;
		}

		// "Type" �̒l���擾���A�O���[�v��ʂƂ���B
		// �O���[�v��ʖ����O���[�v��ʕϊ����s���B
		std::wstring type_value = util::my_mbstowcs(inifile.GetValue( "Type", section_name )).c_str();
		ACCESS_TYPE group_type = helper.GroupString2Type( type_value.c_str() );
		if( group_type == ACCESS_INVALID ) {
			continue;
		}

		// "Url" �̒l���擾���AURL�Ƃ���B
		std::string url = inifile.GetValue( "Url", section_name );

		// �O���[�v�쐬
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url).c_str(), group_type );

		// "CategoryXX" �̒l���擾���A��������BXX �� [01,20] �Ƃ���B
		for( int j=0; j<20; j++ ) {
			CStringA key;
			key.Format( "Category%02d", j+1 );
			
			// "CategoryXX" �̒l�����݂��Ȃ���ΏI������B
			if(! inifile.RecordExists( (const char*)key, section_name ) ) {
				break;
			}

			// "CategoryXX" �̒l���擾����B
			std::string value = inifile.GetValue( (const char*)key, section_name );

			// �J���}��؂�ŕ����񃊃X�g������B
			std::vector<std::string> values;
			util::split_by_comma( values, value );
			if( values.size() < 2 ) {
				continue;
			}

			// �����񃊃X�g�̑�1�v�f���J�e�S�����Ƃ���B
			std::string& category_name = values[0];

			// �����񃊃X�g�̑�2�v�f���J�e�S����ʂƂ���B
			// �J�e�S����ʖ����J�e�S����ʕϊ����s���B
			ACCESS_TYPE category_type = helper.CategoryString2Type( util::my_mbstowcs(values[1]).c_str() );
			if( category_type == ACCESS_INVALID ) {
				continue;
			}

			// �����񃊃X�g�̑�3�v�f��URL�Ƃ���B
			// ���݂��Ȃ��ꍇ�̓J�e�S����ʂ̃f�t�H���g�l��p����B
			const char* category_url = NULL;
			if( values.size() >= 3 ) {
				category_url = values[2].c_str();
			}

			// �����񃊃X�g�̑�4�v�f������t���O�Ƃ���B
			// ���݂��Ȃ��ꍇ�͏���Ȃ��Ƃ���B
			bool bCruise = false;
			if( values.size() >= 4 ) {
				bCruise = atoi(values[3].c_str()) != 0 ? true : false;
			}

			// �J�e�S���ǉ��B
			target.appendCategoryByIniData( accessTypeInfo, group, category_name, category_type, category_url, bCruise );
		}

		// �J�e�S�����P�ȏ�Ȃ�Agroups �ɒǉ��B
		if( group.categories.size() >= 1 ) {
			target.groups.push_back( group );
		}
	}

	// �O���[�v�v�f���[���Ȃ�I���B
	if( target.groups.empty() ) {
		return false;
	}

	return true;
}

/**
 * Mz3GroupData �I�u�W�F�N�g�� ini �t�@�C���i�O���[�v��`�t�@�C���j������B
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

		// �Z�N�V����������
		CStringA strSectionName;
		strSectionName.Format( "Group%02d", i+1 );

		// Name �o��
		inifile.SetValue( L"Name", group.name, strSectionName );

		// Type �o��
		inifile.SetValue( "Type", accessTypeInfo.getSerializeKey(group.mixi.GetAccessType()), (LPCSTR)strSectionName );

		// Url �o��
		inifile.SetValue( L"Url", group.mixi.GetURL(), strSectionName );

		// CategoryXX �o��
		int nc = group.categories.size();
		int categoryNumber = 1;
		for( int j=0; j<nc; j++ ) {
			// �L�[����
			CString key;
			key.Format( L"Category%02d", categoryNumber );

			// �E�Ӓl����
			// �E�Ӓl�͏��ɁA�u�J�e�S�����́v�A�u�J�e�S����ʕ�����v�A�uURL�v
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

			// �o��
			inifile.SetValue( key, value, strSectionName );
			categoryNumber ++;
		}
	}

	// �ۑ�
	return inifile.Save( inifilename, false );
}
