#include "stdafx.h"
#include "Mz3.h"
#include "Mz3GroupData.h"
#include "util.h"

/**
 * �g�b�v�y�[�W�p�̏�����
 *
 * �O���[�v���X�g�Ƃ��̔z���̃��X�g�̐����B
 */
bool Mz3GroupData::initForTopPage()
{
	this->groups.clear();

	static CGroupItem group;

	// ���L�O���[�v
	group.init( L"���L", L"list_diary.pl", ACCESS_GROUP_MYDIARY );
	{
		appendCategoryByIniData( group, "�ŋ߂̓��L", ACCESS_LIST_MYDIARY );
		appendCategoryByIniData( group, "�ŋ߂̃R�����g", ACCESS_LIST_COMMENT );
		appendCategoryByIniData( group, "�}�C�~�N�ŐV���L", ACCESS_LIST_DIARY );
		appendCategoryByIniData( group, "���L�R�����g�L������", ACCESS_LIST_NEW_COMMENT );
	}
	this->groups.push_back( group );

	// �R�~���j�e�B�O���[�v
	group.init( L"�R�~���j�e�B", L"", ACCESS_GROUP_COMMUNITY );
	{
		appendCategoryByIniData( group, "�ŐV�������݈ꗗ", ACCESS_LIST_NEW_BBS );
		appendCategoryByIniData( group, "�R�~���j�e�B�ꗗ", ACCESS_LIST_COMMUNITY );
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
			{NULL, NULL}
		};

		for( int i=0; news_list[i].title != NULL; i++ ) {
			appendCategoryByIniData( group, news_list[i].title, ACCESS_LIST_NEWS, news_list[i].url );
		}
	}
	this->groups.push_back( group );

	// ���b�Z�[�W�O���[�v
	group.init( L"���b�Z�[�W", L"", ACCESS_GROUP_MESSAGE );
	{
		appendCategoryByIniData( group, "���b�Z�[�W�i��M���j", ACCESS_LIST_MESSAGE_IN );
		appendCategoryByIniData( group, "���b�Z�[�W�i���M���j", ACCESS_LIST_MESSAGE_OUT );
	}
	this->groups.push_back( group );

	// ���̑��O���[�v
	group.init( L"���̑�", L"", ACCESS_GROUP_OTHERS );
	{
		appendCategoryByIniData( group, "�}�C�~�N�ꗗ", ACCESS_LIST_FRIEND );
		appendCategoryByIniData( group, "�Љ", ACCESS_LIST_INTRO );
		appendCategoryByIniData( group, "������", ACCESS_LIST_FOOTSTEP );
		appendCategoryByIniData( group, "�u�b�N�}�[�N", ACCESS_LIST_BOOKMARK );
		appendCategoryByIniData( group, "���C�ɓ��胆�[�U�[", ACCESS_LIST_FAVORITE, "list_bookmark.pl" );
		appendCategoryByIniData( group, "���C�ɓ���R�~��", ACCESS_LIST_FAVORITE, "list_bookmark.pl?kind=community" );
	}
	this->groups.push_back( group );

	return true;
}

/**
 * group �ɃJ�e�S����ǉ�����B
 *
 * category_url �� NULL �܂��͋�̏ꍇ�́A�J�e�S����ʂɉ������f�t�H���gURL���w�肷��B
 * �J�e�S����ʂɉ������J�����^�C�v���w�肷��B
 */
bool Mz3GroupData::appendCategoryByIniData( 
	CGroupItem& group, const std::string& category_name, ACCESS_TYPE category_type, const char* category_url, bool bCruise )
{
	// �f�t�H���gURL�icategory_url �����w��̏ꍇ�ɗp����URL�j
	LPCTSTR default_category_url = L"";

	CCategoryItem::BODY_INDICATE_TYPE firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
	CCategoryItem::BODY_INDICATE_TYPE secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;

	switch( category_type ) {

	// ���L�n
	case ACCESS_LIST_MYDIARY:
		// �ŋ߂̓��L
		default_category_url = L"list_diary.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_COMMENT:
		// �ŋ߂̃R�����g
		default_category_url = L"list_comment.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_DIARY:
		// �}�C�~�N�ŐV���L
		default_category_url = L"new_friend_diary.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_NEW_COMMENT:
		// ���L�R�����g�L������
		default_category_url = L"new_comment.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	// �R�~���j�e�B�n
	case ACCESS_LIST_NEW_BBS:
		// �ŐV�������݈ꗗ
		default_category_url = L"new_bbs.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case ACCESS_LIST_COMMUNITY:
		// �R�~���j�e�B�ꗗ
		default_category_url = L"list_community.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NONE;
		break;

	// �j���[�X�n
	case ACCESS_LIST_NEWS:
		// �j���[�X
		default_category_url = L"";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	// ���b�Z�[�W�n
	case ACCESS_LIST_MESSAGE_IN:
		// ���b�Z�[�W�i��M���j
		default_category_url = L"list_message.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	case  ACCESS_LIST_MESSAGE_OUT:
		// ���b�Z�[�W�i���M���j
		default_category_url = L"list_message.pl?box=outbox";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	// ���̑�
	case ACCESS_LIST_FRIEND:
		// �}�C�~�N�ꗗ
		default_category_url = L"list_friend.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_INTRO:
		// �Љ
		default_category_url = L"show_intro.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_BODY;
		break;

	case ACCESS_LIST_FOOTSTEP:
		// ������
		default_category_url = L"show_log.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_FAVORITE:
		// ���C�ɓ���
		default_category_url = L"list_bookmark.pl";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		break;

	case ACCESS_LIST_BOOKMARK:
		// �u�b�N�}�[�N
		default_category_url = L"";
		firstColType  = CCategoryItem::BODY_INDICATE_TYPE_TITLE;
		secondColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		break;

	default:
		// �T�|�[�g�O�J�e�S���^�C�v�Ȃ̂Œǉ������I���B
		return false;
	}

	// �ǉ��B

	// URL ���w�肳��Ă���΂��� URL ��p����B
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
 * ini �t�@�C���i�O���[�v��`�t�@�C���j���� Mz3GroupData �𐶐�����B
 */
bool Mz3GroupDataReader::load( Mz3GroupData& target, const CString& inifilename )
{
	target.groups.clear();

	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper;

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
		std::wstring type_value = util::my_mbstowcs( inifile.GetValue( "Type", section_name ) );
		ACCESS_TYPE group_type = helper.GroupString2Type( type_value.c_str() );
		if( group_type == ACCESS_INVALID ) {
			continue;
		}

		// "Url" �̒l���擾���AURL�Ƃ���B
		std::string url = inifile.GetValue( "Url", section_name );

		// �O���[�v�쐬
		CGroupItem group;
		group.init( util::my_mbstowcs(group_name).c_str(), util::my_mbstowcs(url.c_str()).c_str(), group_type );

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
			target.appendCategoryByIniData( group, category_name, category_type, category_url, bCruise );
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
bool Mz3GroupDataWriter::save( const Mz3GroupData& target, const CString& inifilename )
{
	inifile::IniFile inifile;
	Mz3GroupDataInifileHelper helper;

	const int n = target.groups.size();
	for( int i=0; i<n; i++ ) {
		const CGroupItem& group = target.groups[i];

		// �Z�N�V����������
		CStringA strSectionName;
		strSectionName.Format( "Group%d", i+1 );

		// Name �o��
		inifile.SetValue( L"Name", group.name, strSectionName );

//		MessageBox( NULL, group.name, L"", MB_OK );

		// Type �o��
		inifile.SetValue( L"Type", helper.GroupType2String(group.mixi.GetAccessType()), strSectionName );

		// Url �o��
		inifile.SetValue( L"Url", group.mixi.GetURL(), strSectionName );

		// CategoryXX �o��
		int nc = group.categories.size();
		for( int j=0; j<nc; j++ ) {
			// �L�[����
			CString key;
			key.Format( L"Category%02d", j+1 );

			// �E�Ӓl����
			// �E�Ӓl�͏��ɁA�u�J�e�S�����́v�A�u�J�e�S����ʕ�����v�A�uURL�v
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

			// �o��
			inifile.SetValue( key, value, strSectionName );
		}
	}

	// �ۑ�
	return inifile.Save( inifilename, false );
}
